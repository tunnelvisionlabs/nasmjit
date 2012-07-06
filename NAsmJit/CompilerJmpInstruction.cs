namespace NAsmJit
{
    using System;
    using System.Diagnostics.Contracts;

    public class CompilerJmpInstruction : CompilerInstruction
    {
        private CompilerTarget _jumpTarget;
        private CompilerJmpInstruction _jumpNext;
        private StateData _state;
        private bool _isTaken;

        public CompilerJmpInstruction(Compiler compiler, InstructionCode code, Operand[] operands)
            : base(compiler, code, operands)
        {
            if (code < InstructionDescription.JumpBegin || code > InstructionDescription.JumpEnd)
                throw new ArgumentException("The specified instruction code is not a valid jump.");
            Contract.Requires(compiler != null);
            Contract.Requires(operands != null);
            Contract.EndContractBlock();

            _jumpTarget = compiler.GetTarget(Operands[0].Id);
            _jumpTarget.JumpsCount++;

            _jumpNext = _jumpTarget.From;
            _jumpTarget.From = this;

            _isTaken =
                Code == InstructionCode.Jmp
                || (Operands.Length > 1 && Operands[1].IsImm && ((Imm)Operands[1]).Value == (IntPtr)Hint.Taken);
        }

        public CompilerTarget JumpTarget
        {
            get
            {
                return _jumpTarget;
            }
        }

        public CompilerJmpInstruction JumpNext
        {
            get
            {
                return _jumpNext;
            }
        }

        public bool IsTaken
        {
            get
            {
                return _isTaken;
            }
        }

        protected override void PrepareImpl(CompilerContext cc)
        {
            Offset = cc.CurrentOffset;

            // Update _isTaken to true if this is conditional backward jump. This behavior
            // can be overridden by using HINT_NOT_TAKEN when using the instruction.
            if ((Code != InstructionCode.Jmp)
                && Operands.Length == 1 && _jumpTarget.Offset < Offset)
            {
                _isTaken = true;
            }

            // Now patch all variables where jump location is in the active range.
            if (_jumpTarget.Offset != InvalidValue && cc.Active != null)
            {
                VarData first = cc.Active;
                VarData var = first;
                int jumpOffset = _jumpTarget.Offset;

                do
                {
                    if (var.FirstItem != null)
                    {
                        if (var.LastItem == null)
                            throw new CompilerException();

                        int start = var.FirstItem.Offset;
                        int end = var.LastItem.Offset;

                        if (jumpOffset >= start && jumpOffset <= end)
                            var.LastItem = this;
                    }
                    var = var.NextActive;
                } while (var != first);
            }

            cc.CurrentOffset++;
        }

        protected override CompilerItem TranslateImpl(CompilerContext cc)
        {
            // translate using Instruction
            CompilerItem ret = base.TranslateImpl(cc);

            // we jump with item if it's InstructionCode.JMP (unconditional) and points to yet unknown location.
            if (Code == InstructionCode.Jmp && !JumpTarget.IsTranslated)
            {
                cc.AddBackwardCode(this);
                ret = JumpTarget;
            }
            else
            {
                _state = cc.SaveState();
                if (JumpTarget.IsTranslated)
                {
                    DoJump(cc);
                }
                else
                {
                    // state is not known, so we need to call DoJump() later. Compiler will do it for us.
                    cc.AddForwardJump(this);
                    JumpTarget.State = _state;
                }

                // Mark next code as unrecheable, cleared by a next label (ETarget).
                if (Code == InstructionCode.Jmp)
                {
                    cc.Unreachable = true;
                }
            }

            // Need to traverse over all active variables and unuse them if their scope ends here
            if (cc.Active != null)
            {
                VarData first = cc.Active;
                VarData var = first;

                do
                {
                    cc.UnuseVarOnEndOfScope(this, var);
                    var = var.NextActive;
                } while (var != first);
            }

            return ret;
        }

        protected override void EmitImpl(Assembler a)
        {
            const uint MAXIMUM_SHORT_JMP_SIZE = 127;

            // Try to minimize size of jump using SHORT jump (8-bit displacement) by 
            // traversing into the target and calculating the maximum code size. We
            // end when code size reaches MAXIMUM_SHORT_JMP_SIZE.
            if ((EmitOptions & EmitOptions.ShortJump) == 0 && JumpTarget.Offset > Offset)
            {
                // Calculate the code size.
                uint codeSize = 0;
                CompilerItem cur = this.Next;
                CompilerItem target = JumpTarget;

                while (cur != null)
                {
                    if (cur == target)
                    {
                        // Target found, we can tell assembler to generate short form of jump.
                        EmitOptions |= EmitOptions.ShortJump;
                        goto end;
                    }

                    int s = cur.MaxSize;
                    if (s == -1)
                        break;

                    codeSize += (uint)s;
                    if (codeSize > MAXIMUM_SHORT_JMP_SIZE)
                        break;

                    cur = cur.Next;
                }
            }

        end:
            base.EmitImpl(a);
        }

        internal void DoJump(CompilerContext cc)
        {
            Contract.Requires(cc != null);

            // The state have to be already known. The _doJump() method is called by
            // translate() or by Compiler in case that it's forward jump.
            if (_jumpTarget.State == null)
                throw new CompilerException("Cannot jump to a target without knowing its state.");

            if ((Code == InstructionCode.Jmp) || (IsTaken && _jumpTarget.Offset < Offset))
            {
                // Instruction type is JMP or conditional jump that should be taken (likely).
                // We can set state here instead of jumping out, setting state and jumping
                // to _jumpTarget.
                //
                // NOTE: We can't use this technique if instruction is forward conditional
                // jump. The reason is that when generating code we can't change state here,
                // because the next instruction depends on it.
                cc.RestoreState(_jumpTarget.State, _jumpTarget.Offset);
            }
            else
            {
                // Instruction type is JMP or conditional jump that should be not normally
                // taken. If we need add code that will switch between different states we
                // add it after the end of function body (after epilog, using 'ExtraBlock').
                Compiler compiler = cc.Compiler;

                CompilerItem ext = cc.ExtraBlock;
                CompilerItem old = compiler.CurrentItem;
                compiler.CurrentItem = ext;

                cc.RestoreState(_jumpTarget.State, _jumpTarget.Offset);

                if (compiler.CurrentItem != ext)
                {
                    // Add the jump to the target.
                    compiler.Jmp(_jumpTarget.Label);
                    ext = compiler.CurrentItem;

                    // The cc._restoreState() method emitted some instructions so we need to
                    // patch the jump.
                    Label L = compiler.DefineLabel();
                    compiler.CurrentItem = cc.ExtraBlock;
                    compiler.MarkLabel(L);

                    // Finally, patch the jump target.
                    if (Operands.Length == 0)
                        throw new CompilerException();

                    Operands[0] = L;                              // Operand part (Label).
                    _jumpTarget = compiler.GetTarget(L.Id); // Item part (ETarget).
                }

                cc.ExtraBlock = ext;
                compiler.CurrentItem = old;

                // Assign state back.
                cc.AssignState(_state);
            }
        }
    }
}
