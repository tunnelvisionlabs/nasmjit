﻿namespace AsmJitNet
{
    using System;
    using Debug = System.Diagnostics.Debug;

    public class Jmp : Instruction
    {
        private Target _jumpTarget;
        private Jmp _jumpNext;
        private StateData _state;
        private bool _isTaken;

        public Jmp(Compiler compiler, InstructionCode code, Operand[] operands)
            : base(compiler, code, operands)
        {
            _jumpTarget = compiler.GetTarget(Operands[0].Id);
            _jumpTarget.JumpsCount++;

            _jumpNext = _jumpTarget.From;
            _jumpTarget.From = this;

            _isTaken =
                Code == InstructionCode.Jmp
                || (Operands.Length > 1 && Operands[1].IsImm && ((Imm)Operands[1]).Value == (IntPtr)Hint.Taken);
        }

        public Target JumpTarget
        {
            get
            {
                return _jumpTarget;
            }
        }

        public Jmp JumpNext
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

        public override void Prepare(CompilerContext cc)
        {
            Offset = cc.CurrentOffset;

            // Update _isTaken to true if this is conditional backward jump. This behavior
            // can be overriden by using HINT_NOT_TAKEN when using the instruction.
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
                    if (var.FirstEmittable != null)
                    {
                        if (var.LastEmittable == null)
                            throw new CompilerException();

                        int start = var.FirstEmittable.Offset;
                        int end = var.LastEmittable.Offset;

                        if (jumpOffset >= start && jumpOffset <= end)
                            var.LastEmittable = this;
                    }
                    var = var.NextActive;
                } while (var != first);
            }

            cc.CurrentOffset++;
        }

        public override void Translate(CompilerContext cc)
        {
            base.Translate(cc);
            _state = cc.SaveState();

            if (_jumpTarget.Offset > Offset)
            {
                // State is not known, so we need to call _doJump() later. Compiler will
                // do it for us.
                cc.AddForwardJump(this);
                _jumpTarget.State = _state;
            }
            else
            {
                DoJump(cc);
            }

            // Mark next code as unrecheable, cleared by a next label (ETarget).
            if (Code == InstructionCode.Jmp)
            {
                cc.Unreachable = true;
            }

            // Need to traverse all active variables and unuse them if their scope ends here
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
        }

        public override void Emit(Assembler a)
        {
            const uint MAXIMUM_SHORT_JMP_SIZE = 127;

            // Try to minimize size of jump using SHORT jump (8-bit displacement) by 
            // traversing into the target and calculating the maximum code size. We
            // end when code size reaches MAXIMUM_SHORT_JMP_SIZE.
            if ((EmitOptions & EmitOptions.ShortJump) == 0 && JumpTarget.Offset > Offset)
            {
                // Calculate the code size.
                uint codeSize = 0;
                Emittable cur = this.Next;
                Emittable target = JumpTarget;

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
            base.Emit(a);
        }

        internal void DoJump(CompilerContext cc)
        {
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
                // because next instruction depends to it.
                cc.RestoreState(_jumpTarget.State, _jumpTarget.Offset);
            }
            else
            {
                // Instruction type is JMP or conditional jump that should be not normally
                // taken. If we need add code that will switch between different states we
                // add it after the end of function body (after epilog, using 'ExtraBlock').
                Compiler compiler = cc.Compiler;

                Emittable ext = cc.ExtraBlock;
                Emittable old = compiler.CurrentEmittable;
                compiler.CurrentEmittable = ext;

                cc.RestoreState(_jumpTarget.State, _jumpTarget.Offset);

                if (compiler.CurrentEmittable != ext)
                {
                    // Add the jump to the target.
                    compiler.Jmp(_jumpTarget.Label);
                    ext = compiler.CurrentEmittable;

                    // The cc._restoreState() method emitted some instructions so we need to
                    // patch the jump.
                    Label L = compiler.DefineLabel();
                    compiler.CurrentEmittable = cc.ExtraBlock;
                    compiler.MarkLabel(L);

                    // Finally, patch the jump target.
                    if (Operands.Length == 0)
                        throw new CompilerException();

                    Operands[0] = L;                              // Operand part (Label).
                    _jumpTarget = compiler.GetTarget(L.Id); // Emittable part (ETarget).
                }

                cc.ExtraBlock = ext;
                compiler.CurrentEmittable = old;

                // Assign state back.
                cc.AssignState(_state);
            }
        }
    }
}
