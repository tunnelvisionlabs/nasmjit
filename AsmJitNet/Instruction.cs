namespace AsmJitNet
{
    using System;
    using Debug = System.Diagnostics.Debug;

    public class Instruction : Emittable
    {
        private InstructionCode _code;

        private EmitOptions _emitOptions;

        /// <summary>
        /// Operands
        /// </summary>
        private Operand[] _operands;

        /// <summary>
        /// Memory operand (if the instruction contains any)
        /// </summary>
        private Mem _memoryOperand;

        /// <summary>
        /// Variables (extracted from operands)
        /// </summary>
        private VarAllocRecord[] _variables;

        /// <summary>
        /// Whether the instruction is special
        /// </summary>
        private readonly bool _isSpecial;

        /// <summary>
        /// Whether the instruction is FPU
        /// </summary>
        private readonly bool _isFPU;

        public Instruction(Compiler compiler, InstructionCode code, Operand[] operands)
            : base(compiler)
        {
            _code = code;
            _emitOptions = compiler.EmitOptions;
            // Each created instruction takes emit options and clears it.
            compiler.EmitOptions = EmitOptions.None;

            _operands = operands;
            _memoryOperand = null;

            _variables = null;

            for (int i = 0; i < operands.Length; i++)
            {
                if (operands[i].IsMem)
                {
                    _memoryOperand = (Mem)operands[i];
                    break;
                }
            }

            InstructionDescription id = InstructionDescription.FromInstruction(_code);
            _isSpecial = id.IsSpecial;
            _isFPU = id.IsFPU;

            if (_isSpecial)
            {
                // ${SPECIAL_INSTRUCTION_HANDLING_BEGIN}
                switch ((InstructionCode)_code)
                {
                case InstructionCode.Cpuid:
                    // Special...
                    break;

                case InstructionCode.Cbw:
                case InstructionCode.Cdqe:
                case InstructionCode.Cwde:
                    // Special...
                    break;

                case InstructionCode.Cmpxchg:
                case InstructionCode.Cmpxchg8b:
#if ASMJIT_X64
                case InstructionCode.Cmpxchg16b:
#endif // ASMJIT_X64
                    // Special...
                    break;

#if ASMJIT_X86
                case InstructionCode.Daa:
                case InstructionCode.Das:
                    // Special...
                    break;
#endif // ASMJIT_X86

                case InstructionCode.Imul:
                    switch (operands.Length)
                    {
                    case 2:
                        // IMUL dst, src is not special instruction.
                        _isSpecial = false;
                        break;
                    case 3:
                        if (!(_operands[0].IsVar && _operands[1].IsVar && _operands[2].IsVarMem))
                        {
                            // Only IMUL dst_lo, dst_hi, reg/mem is special, all others not.
                            _isSpecial = false;
                        }
                        break;
                    }
                    break;
                case InstructionCode.Mul:
                case InstructionCode.Idiv:
                case InstructionCode.Div:
                    // Special...
                    break;

                case InstructionCode.MovPtr:
                    // Special...
                    break;

                case InstructionCode.Lahf:
                case InstructionCode.Sahf:
                    // Special...
                    break;

                case InstructionCode.Maskmovdqu:
                case InstructionCode.Maskmovq:
                    // Special...
                    break;

                case InstructionCode.Enter:
                case InstructionCode.Leave:
                    // Special...
                    break;

                case InstructionCode.Ret:
                    // Special...
                    break;

                case InstructionCode.Monitor:
                case InstructionCode.Mwait:
                    // Special...
                    break;

                case InstructionCode.Pop:
                case InstructionCode.Popad:
                case InstructionCode.Popfd:
                case InstructionCode.Popfq:
                    // Special...
                    break;

                case InstructionCode.Push:
                case InstructionCode.Pushad:
                case InstructionCode.Pushfd:
                case InstructionCode.Pushfq:
                    // Special...
                    break;

                case InstructionCode.Rcl:
                case InstructionCode.Rcr:
                case InstructionCode.Rol:
                case InstructionCode.Ror:
                case InstructionCode.Sal:
                case InstructionCode.Sar:
                case InstructionCode.Shl:
                case InstructionCode.Shr:
                    // Rot instruction is special only if last operand is variable (register).
                    _isSpecial = _operands[1].IsVar;
                    break;

                case InstructionCode.Shld:
                case InstructionCode.Shrd:
                    // Shld/Shrd instruction is special only if last operand is variable (register).
                    _isSpecial = _operands[2].IsVar;
                    break;

                case InstructionCode.Rdtsc:
                case InstructionCode.Rdtscp:
                    // Special...
                    break;

                case InstructionCode.RepLodsb:
                case InstructionCode.RepLodsd:
                case InstructionCode.RepLodsq:
                case InstructionCode.RepLodsw:
                case InstructionCode.RepMovsb:
                case InstructionCode.RepMovsd:
                case InstructionCode.RepMovsq:
                case InstructionCode.RepMovsw:
                case InstructionCode.RepStosb:
                case InstructionCode.RepStosd:
                case InstructionCode.RepStosq:
                case InstructionCode.RepStosw:
                case InstructionCode.RepeCmpsb:
                case InstructionCode.RepeCmpsd:
                case InstructionCode.RepeCmpsq:
                case InstructionCode.RepeCmpsw:
                case InstructionCode.RepeScasb:
                case InstructionCode.RepeScasd:
                case InstructionCode.RepeScasq:
                case InstructionCode.RepeScasw:
                case InstructionCode.RepneCmpsb:
                case InstructionCode.RepneCmpsd:
                case InstructionCode.RepneCmpsq:
                case InstructionCode.RepneCmpsw:
                case InstructionCode.RepneScasb:
                case InstructionCode.RepneScasd:
                case InstructionCode.RepneScasq:
                case InstructionCode.RepneScasw:
                    // Special...
                    break;

                default:
                    Debug.Assert(false);
                    break;
                }
                // ${SPECIAL_INSTRUCTION_HANDLING_END}
            }
        }

        public override EmittableType EmittableType
        {
            get
            {
                return EmittableType.Instruction;
            }
        }

        public override int MaxSize
        {
            get
            {
                // TODO: Do something more exact.
                return 15;
            }
        }

        public Mem MemoryOperand
        {
            get
            {
                return _memoryOperand;
            }
        }

        public bool IsSpecial
        {
            get
            {
                return _isSpecial;
            }
        }

        public bool IsFPU
        {
            get
            {
                return _isFPU;
            }
        }

        public InstructionCode Code
        {
            get
            {
                return _code;
            }

            protected set
            {
                _code = value;
            }
        }

        protected Operand[] Operands
        {
            get
            {
                return _operands;
            }
        }

        public override void Prepare(CompilerContext cc)
        {

            Offset = cc.CurrentOffset;

            InstructionDescription id = InstructionDescription.FromInstruction(_code);

            int i;
            int len = _operands.Length;
            int variablesCount = 0;

            for (i = 0; i < len; i++)
            {
                Operand o = _operands[i];

                BaseVar vo = o as BaseVar;
                if (vo != null)
                {
                    Debug.Assert(o.Id != InvalidValue);
                    VarData vdata = Compiler.GetVarData(o.Id);
                    Debug.Assert(vdata != null);

                    if (vdata.WorkOffset == Offset)
                        continue;
                    if (!cc.IsActive(vdata))
                        cc.AddActive(vdata);

                    vdata.WorkOffset = Offset;
                    variablesCount++;
                }
                else
                {
                    Mem mem = o as Mem;
                    if (mem != null)
                    {
                        if ((o.Id & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                        {
                            VarData vdata = Compiler.GetVarData(o.Id);
                            Debug.Assert(vdata != null);

                            cc.MarkMemoryUsed(vdata);
                            if (!cc.IsActive(vdata))
                                cc.AddActive(vdata);

                            continue;
                        }
                        else if (((int)mem.Base & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                        {
                            VarData vdata = Compiler.GetVarData((int)mem.Base);
                            Debug.Assert(vdata != null);

                            if (vdata.WorkOffset == Offset)
                                continue;
                            if (!cc.IsActive(vdata))
                                cc.AddActive(vdata);

                            vdata.WorkOffset = Offset;
                            variablesCount++;
                        }

                        if (((int)mem.Index & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                        {
                            VarData vdata = Compiler.GetVarData((int)mem.Index);
                            Debug.Assert(vdata != null);

                            if (vdata.WorkOffset == Offset)
                                continue;
                            if (!cc.IsActive(vdata))
                                cc.AddActive(vdata);

                            vdata.WorkOffset = Offset;
                            variablesCount++;
                        }
                    }
                }
            }

            if (variablesCount == 0)
            {
                cc.CurrentOffset++;
                return;
            }

            _variables = new VarAllocRecord[variablesCount];
            for (int j = 0; j < variablesCount; j++)
                _variables[j] = new VarAllocRecord();

            int curIndex = 0;
            VarAllocRecord var = null;
            int varIndex = -1;

            Action<VarData> __GET_VARIABLE =
                __vardata__ =>
                {
                    VarData candidate = __vardata__;

                    for (varIndex = curIndex; ; )
                    {
                        if (varIndex == 0)
                        {
                            varIndex = curIndex++;
                            _variables[varIndex].VarData = candidate;
                            _variables[varIndex].VarFlags = 0;
                            _variables[varIndex].RegIndex = RegIndex.Invalid;
                            break;
                        }

                        varIndex--;

                        if (_variables[varIndex].VarData == candidate)
                            break;
                    }

                    var = _variables[varIndex];
                    Debug.Assert(var != null);
                };

            for (i = 0; i < len; i++)
            {
                Operand o = _operands[i];

                if (o.IsVar)
                {
                    VarData vdata = Compiler.GetVarData(o.Id);
                    Debug.Assert(vdata != null);

                    __GET_VARIABLE(vdata);
                    var.VarFlags |= VariableAlloc.Register;

                    if (IsSpecial)
                    {
                        // ${SPECIAL_INSTRUCTION_HANDLING_BEGIN}
                        switch ((InstructionCode)_code)
                        {
                        case InstructionCode.Cpuid:
                            switch (i)
                            {
                            case 0:
                                vdata.RegisterRWCount++;
                                var.VarFlags |= VariableAlloc.ReadWrite;
                                var.RegIndex = RegIndex.Eax;
                                break;
                            case 1:
                                vdata.RegisterWriteCount++;
                                var.VarFlags |= VariableAlloc.Write;
                                var.RegIndex = RegIndex.Ebx;
                                break;
                            case 2:
                                vdata.RegisterWriteCount++;
                                var.VarFlags |= VariableAlloc.Write;
                                var.RegIndex = RegIndex.Ecx;
                                break;
                            case 3:
                                vdata.RegisterWriteCount++;
                                var.VarFlags |= VariableAlloc.Write;
                                var.RegIndex = RegIndex.Edx;
                                break;

                            default:
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));
                            }
                            break;

                        case InstructionCode.Cbw:
                        case InstructionCode.Cdqe:
                        case InstructionCode.Cwde:
                            switch (i)
                            {
                            case 0:
                                vdata.RegisterRWCount++;
                                var.VarFlags |= VariableAlloc.ReadWrite;
                                var.RegIndex = RegIndex.Eax;
                                break;

                            default:
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));
                            }
                            break;

                        case InstructionCode.Cmpxchg:
                            switch (i)
                            {
                            case 0:
                                vdata.RegisterRWCount++;
                                var.VarFlags |= VariableAlloc.ReadWrite;
                                var.RegIndex = RegIndex.Eax;
                                break;
                            case 1:
                                vdata.RegisterRWCount++;
                                var.VarFlags |= VariableAlloc.ReadWrite;
                                break;
                            case 2:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                break;

                            default:
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));
                            }
                            break;

                        case InstructionCode.Cmpxchg8b:
#if ASMJIT_X64
                        case InstructionCode.Cmpxchg16b:
#endif // ASMJIT_X64
                            switch (i)
                            {
                            case 0:
                                vdata.RegisterRWCount++;
                                var.VarFlags |= VariableAlloc.ReadWrite;
                                var.RegIndex = RegIndex.Edx;
                                break;
                            case 1:
                                vdata.RegisterRWCount++;
                                var.VarFlags |= VariableAlloc.ReadWrite;
                                var.RegIndex = RegIndex.Eax;
                                break;
                            case 2:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                var.RegIndex = RegIndex.Ecx;
                                break;
                            case 3:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                var.RegIndex = RegIndex.Ebx;
                                break;

                            default:
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));
                            }
                            break;

#if ASMJIT_X86
                        case InstructionCode.Daa:
                        case InstructionCode.Das:
                            if (i != 0)
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));

                            vdata.RegisterRWCount++;
                            var.VarFlags |= VariableAlloc.ReadWrite;
                            var.RegIndex = RegIndex.Eax;
                            break;
#endif // ASMJIT_X86

                        case InstructionCode.Imul:
                        case InstructionCode.Mul:
                        case InstructionCode.Idiv:
                        case InstructionCode.Div:
                            switch (i)
                            {
                            case 0:
                                vdata.RegisterWriteCount++;
                                var.VarFlags |= VariableAlloc.Write;
                                var.RegIndex = RegIndex.Edx;
                                break;
                            case 1:
                                vdata.RegisterRWCount++;
                                var.VarFlags |= VariableAlloc.ReadWrite;
                                var.RegIndex = RegIndex.Eax;
                                break;
                            case 2:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                break;

                            default:
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));
                            }
                            break;

                        case InstructionCode.MovPtr:
                            switch (i)
                            {
                            case 0:
                                vdata.RegisterWriteCount++;
                                var.VarFlags |= VariableAlloc.Write;
                                var.RegIndex = RegIndex.Eax;
                                break;
                            case 1:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                var.RegIndex = RegIndex.Eax;
                                break;
                            default:
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));
                            }
                            break;

                        case InstructionCode.Lahf:
                            if (i != 0)
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));

                            vdata.RegisterWriteCount++;
                            var.VarFlags |= VariableAlloc.Write;
                            var.RegIndex = RegIndex.Eax;
                            break;

                        case InstructionCode.Sahf:
                            if (i != 0)
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));

                            vdata.RegisterReadCount++;
                            var.VarFlags |= VariableAlloc.Read;
                            var.RegIndex = RegIndex.Eax;
                            break;

                        case InstructionCode.Maskmovdqu:
                        case InstructionCode.Maskmovq:
                            switch (i)
                            {
                            case 0:
                                vdata.MemoryReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                var.RegIndex = RegIndex.Edi;
                                break;

                            case 1:
                            case 2:
                                vdata.MemoryReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                break;

                            default:
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));
                            }
                            break;

                        case InstructionCode.Enter:
                        case InstructionCode.Leave:
                            // TODO: SPECIAL INSTRUCTION.
                            throw new NotImplementedException();

                        case InstructionCode.Ret:
                            // TODO: SPECIAL INSTRUCTION.
                            throw new NotImplementedException();

                        case InstructionCode.Monitor:
                        case InstructionCode.Mwait:
                            // TODO: MONITOR/MWAIT (COMPILER).
                            throw new NotImplementedException();

                        case InstructionCode.Pop:
                            // TODO: SPECIAL INSTRUCTION.
                            throw new NotImplementedException();

                        case InstructionCode.Popad:
                        case InstructionCode.Popfd:
                        case InstructionCode.Popfq:
                            // TODO: SPECIAL INSTRUCTION.
                            throw new NotImplementedException();

                        case InstructionCode.Push:
                            // TODO: SPECIAL INSTRUCTION.
                            throw new NotImplementedException();

                        case InstructionCode.Pushad:
                        case InstructionCode.Pushfd:
                        case InstructionCode.Pushfq:
                            // TODO: SPECIAL INSTRUCTION.
                            throw new NotImplementedException();

                        case InstructionCode.Rcl:
                        case InstructionCode.Rcr:
                        case InstructionCode.Rol:
                        case InstructionCode.Ror:
                        case InstructionCode.Sal:
                        case InstructionCode.Sar:
                        case InstructionCode.Shl:
                        case InstructionCode.Shr:
                            switch (i)
                            {
                            case 0:
                                vdata.RegisterRWCount++;
                                var.VarFlags |= VariableAlloc.ReadWrite;
                                break;
                            case 1:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                var.RegIndex = RegIndex.Ecx;
                                break;

                            default:
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));
                            }
                            break;

                        case InstructionCode.Shld:
                        case InstructionCode.Shrd:
                            switch (i)
                            {
                            case 0:
                                vdata.RegisterRWCount++;
                                var.VarFlags |= VariableAlloc.ReadWrite;
                                break;
                            case 1:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                break;
                            case 2:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                var.RegIndex = RegIndex.Ecx;
                                break;

                            default:
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));
                            }
                            break;

                        case InstructionCode.Rdtsc:
                        case InstructionCode.Rdtscp:
                            switch (i)
                            {
                            case 0:
                                vdata.RegisterWriteCount++;
                                var.VarFlags |= VariableAlloc.Write;
                                var.RegIndex = RegIndex.Edx;
                                break;
                            case 1:
                                vdata.RegisterWriteCount++;
                                var.VarFlags |= VariableAlloc.Write;
                                var.RegIndex = RegIndex.Eax;
                                break;
                            case 2:
                                Debug.Assert(_code == InstructionCode.Rdtscp);
                                vdata.RegisterWriteCount++;
                                var.VarFlags |= VariableAlloc.Write;
                                var.RegIndex = RegIndex.Ecx;
                                break;

                            default:
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));
                            }
                            break;

                        case InstructionCode.RepLodsb:
                        case InstructionCode.RepLodsd:
                        case InstructionCode.RepLodsq:
                        case InstructionCode.RepLodsw:
                            switch (i)
                            {
                            case 0:
                                vdata.RegisterWriteCount++;
                                var.VarFlags |= VariableAlloc.Write;
                                var.RegIndex = RegIndex.Eax;
                                break;
                            case 1:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                var.RegIndex = RegIndex.Esi;
                                break;
                            case 2:
                                vdata.RegisterRWCount++;
                                var.VarFlags |= VariableAlloc.ReadWrite;
                                var.RegIndex = RegIndex.Ecx;
                                break;
                            default:
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));
                            }
                            break;

                        case InstructionCode.RepMovsb:
                        case InstructionCode.RepMovsd:
                        case InstructionCode.RepMovsq:
                        case InstructionCode.RepMovsw:
                        case InstructionCode.RepeCmpsb:
                        case InstructionCode.RepeCmpsd:
                        case InstructionCode.RepeCmpsq:
                        case InstructionCode.RepeCmpsw:
                        case InstructionCode.RepneCmpsb:
                        case InstructionCode.RepneCmpsd:
                        case InstructionCode.RepneCmpsq:
                        case InstructionCode.RepneCmpsw:
                            switch (i)
                            {
                            case 0:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                var.RegIndex = RegIndex.Edi;
                                break;
                            case 1:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                var.RegIndex = RegIndex.Esi;
                                break;
                            case 2:
                                vdata.RegisterRWCount++;
                                var.VarFlags |= VariableAlloc.ReadWrite;
                                var.RegIndex = RegIndex.Ecx;
                                break;
                            default:
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));
                            }
                            break;

                        case InstructionCode.RepStosb:
                        case InstructionCode.RepStosd:
                        case InstructionCode.RepStosq:
                        case InstructionCode.RepStosw:
                            switch (i)
                            {
                            case 0:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                var.RegIndex = RegIndex.Edi;
                                break;
                            case 1:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                var.RegIndex = RegIndex.Eax;
                                break;
                            case 2:
                                vdata.RegisterRWCount++;
                                var.VarFlags |= VariableAlloc.ReadWrite;
                                var.RegIndex = RegIndex.Ecx;
                                break;
                            default:
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));
                            }
                            break;

                        case InstructionCode.RepeScasb:
                        case InstructionCode.RepeScasd:
                        case InstructionCode.RepeScasq:
                        case InstructionCode.RepeScasw:
                        case InstructionCode.RepneScasb:
                        case InstructionCode.RepneScasd:
                        case InstructionCode.RepneScasq:
                        case InstructionCode.RepneScasw:
                            switch (i)
                            {
                            case 0:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                var.RegIndex = RegIndex.Edi;
                                break;
                            case 1:
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                                var.RegIndex = RegIndex.Eax;
                                break;
                            case 2:
                                vdata.RegisterRWCount++;
                                var.VarFlags |= VariableAlloc.ReadWrite;
                                var.RegIndex = RegIndex.Ecx;
                                break;
                            default:
                                throw new NotSupportedException(string.Format("The {0} instruction does not support {1} arguments.", InstructionDescription.FromInstruction((InstructionCode)_code).Name, i));
                            }
                            break;

                        default:
                            throw new NotImplementedException(string.Format("Handling for special instruction {0} is not yet implemented.", InstructionDescription.FromInstruction((InstructionCode)_code).Name));
                        }
                        // ${SPECIAL_INSTRUCTION_HANDLING_END}
                    }
                    else
                    {
                        if (i == 0)
                        {
                            // CMP/TEST instruction.
                            if (id.Code == InstructionCode.Cmp || id.Code == InstructionCode.Test)
                            {
                                // Read-only case.
                                vdata.RegisterReadCount++;
                                var.VarFlags |= VariableAlloc.Read;
                            }
                            // MOV/MOVSS/MOVSD instructions.
                            //
                            // If instruction is MOV (source replaces the destination) or 
                            // MOVSS/MOVSD and source operand is memory location then register
                            // allocator should know that previous destination value is lost 
                            // (write only operation).
                            else if ((id.IsMov) ||
                                    ((id.Code == InstructionCode.Movss || id.Code == InstructionCode.Movsd) /* && _operands[1].isMem() */) ||
                                    (id.Code == InstructionCode.Imul && _operands.Length == 3 && !IsSpecial))
                            {
                                // Write-only case.
                                vdata.RegisterWriteCount++;
                                var.VarFlags |= VariableAlloc.Write;
                            }
                            else
                            {
                                // Read/Write.
                                vdata.RegisterRWCount++;
                                var.VarFlags |= VariableAlloc.ReadWrite;
                            }
                        }
                        else
                        {
                            // Second, third, ... operands are read-only.
                            vdata.RegisterReadCount++;
                            var.VarFlags |= VariableAlloc.Read;
                        }

                        if (_memoryOperand == null && i < 2 && (id.OperandFlags[i] & OperandFlags.MEM) != 0)
                        {
                            var.VarFlags |= VariableAlloc.Memory;
                        }
                    }

                    // If variable must be in specific register here we could add some hint
                    // to allocator to alloc it to this register on first alloc.
                    if (var.RegIndex != RegIndex.Invalid && vdata.HomeRegisterIndex == RegIndex.Invalid)
                    {
                        vdata.HomeRegisterIndex = var.RegIndex;
                    }
                }
                else if (o.IsMem)
                {
                    Mem mem = (Mem)o;
                    if ((o.Id & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = Compiler.GetVarData(o.Id);
                        Debug.Assert(vdata != null);

                        if (i == 0)
                        {
                            // If variable is MOV instruction type (source replaces the destination)
                            // or variable is MOVSS/MOVSD instruction then register allocator should
                            // know that previous destination value is lost (write only operation).
                            if (id.IsMov || ((id.Code == InstructionCode.Movss || id.Code == InstructionCode.Movsd)))
                            {
                                // Write only case.
                                vdata.MemoryWriteCount++;
                            }
                            else
                            {
                                vdata.MemoryRWCount++;
                            }
                        }
                        else
                        {
                            vdata.MemoryReadCount++;
                        }
                    }
                    else if (((int)mem.Base & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = Compiler.GetVarData((int)mem.Base);
                        Debug.Assert(vdata != null);

                        __GET_VARIABLE(vdata);
                        vdata.RegisterReadCount++;
                        var.VarFlags |= VariableAlloc.Register | VariableAlloc.Read;
                    }

                    if (((int)mem.Index & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = Compiler.GetVarData((int)mem.Index);
                        Debug.Assert(vdata != null);

                        __GET_VARIABLE(vdata);
                        vdata.RegisterReadCount++;
                        var.VarFlags |= VariableAlloc.Register | VariableAlloc.Read;
                    }
                }
            }

            // Traverse all variables and update firstEmittable / lastEmittable. This
            // function is called from iterator that scans emittables using forward
            // direction so we can use this knowledge to optimize the process.
            //
            // Same code is in ECall::prepare().
            for (i = 0; i < _variables.Length; i++)
            {
                VarData v = _variables[i].VarData;

                // First emittable (begin of variable scope).
                if (v.FirstEmittable == null)
                    v.FirstEmittable = this;

                // Last emittable (end of variable scope).
                v.LastEmittable = this;
            }

            // There are some instructions that can be used to clear register or to set
            // register to some value (ideal case is all zeros or all ones).
            //
            // xor/pxor reg, reg    ; Set all bits in reg to 0.
            // sub/psub reg, reg    ; Set all bits in reg to 0.
            // andn reg, reg        ; Set all bits in reg to 0.
            // pcmpgt reg, reg      ; Set all bits in reg to 0.
            // pcmpeq reg, reg      ; Set all bits in reg to 1.

            if (_variables.Length == 1 &&
                _operands.Length > 1 &&
                _operands[0].IsVar &&
                _operands[1].IsVar &&
                _memoryOperand == null)
            {
                switch ((InstructionCode)_code)
                {
                // XOR Instructions.
                case InstructionCode.Xor:
                case InstructionCode.Xorpd:
                case InstructionCode.Xorps:
                case InstructionCode.Pxor:

                // ANDN Instructions.
                case InstructionCode.Pandn:

                // SUB Instructions.
                case InstructionCode.Sub:
                case InstructionCode.Psubb:
                case InstructionCode.Psubw:
                case InstructionCode.Psubd:
                case InstructionCode.Psubq:
                case InstructionCode.Psubsb:
                case InstructionCode.Psubsw:
                case InstructionCode.Psubusb:
                case InstructionCode.Psubusw:

                // PCMPEQ Instructions.
                case InstructionCode.Pcmpeqb:
                case InstructionCode.Pcmpeqw:
                case InstructionCode.Pcmpeqd:
                case InstructionCode.Pcmpeqq:

                // PCMPGT Instructions.
                case InstructionCode.Pcmpgtb:
                case InstructionCode.Pcmpgtw:
                case InstructionCode.Pcmpgtd:
                case InstructionCode.Pcmpgtq:
                    // Clear read flag. We are not interested about reading spilled variable
                    // into register, because result of instruction not depends to it.
                    _variables[0].VarFlags = VariableAlloc.Write;
                    _variables[0].VarData.RegisterReadCount--;
                    break;
                }
            }
            cc.CurrentOffset++;
        }

        public override void Translate(CompilerContext cc)
        {
            int i;
            int variablesCount = (_variables != null) ? _variables.Length : 0;

            if (variablesCount > 0)
            {
                // These variables are used by the instruction and we set current offset
                // to their work offsets -> getSpillCandidate never return the variable
                // used this instruction.
                for (i = 0; i < variablesCount; i++)
                {
                    _variables[0].VarData.WorkOffset = cc.CurrentOffset;
                }

                // Alloc variables used by the instruction.
                for (i = 0; i < variablesCount; i++)
                {
                    VarAllocRecord r = _variables[i];
                    // Alloc variables with specific register first.
                    if (r.RegIndex != RegIndex.Invalid)
                        cc.AllocVar(r.VarData, r.RegIndex, r.VarFlags);
                }
                for (i = 0; i < variablesCount; i++)
                {
                    VarAllocRecord r = _variables[i];
                    // Alloc variables without specific register last.
                    if (r.RegIndex == RegIndex.Invalid)
                        cc.AllocVar(r.VarData, r.RegIndex, r.VarFlags);
                }

                cc.TranslateOperands(_operands);
            }

            if (_memoryOperand != null && (_memoryOperand.Id & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
            {
                VarData vdata = Compiler.GetVarData(_memoryOperand.Id);
                Debug.Assert(vdata != null);

                switch (vdata.State)
                {
                case VariableState.Unused:
                    vdata.State = VariableState.Memory;
                    break;

                case VariableState.Register:
                    vdata.Changed = false;
                    cc.UnuseVar(vdata, VariableState.Memory);
                    break;
                }
            }

            for (i = 0; i < variablesCount; i++)
            {
                cc.UnuseVarOnEndOfScope(this, _variables[i].VarData);
            }
        }

        public override void Emit(Assembler a)
        {
            a.Comment = Comment;
            a.EmitOptions = _emitOptions;

            if (IsSpecial)
            {
                // ${SPECIAL_INSTRUCTION_HANDLING_BEGIN}
                switch (_code)
                {
                case InstructionCode.Cpuid:
                    a.EmitInstruction(_code);
                    return;

                case InstructionCode.Cbw:
                case InstructionCode.Cdqe:
                case InstructionCode.Cwde:
                    a.EmitInstruction(_code);
                    return;

                case InstructionCode.Cmpxchg:
                    a.EmitInstruction(_code, _operands[1], _operands[2]);
                    return;

                case InstructionCode.Cmpxchg8b:
#if ASMJIT_X64||true
                case InstructionCode.Cmpxchg16b:
#endif // ASMJIT_X64
                    a.EmitInstruction(_code, _operands[4]);
                    return;

#if ASMJIT_X86||true
                case InstructionCode.Daa:
                case InstructionCode.Das:
                    a.EmitInstruction(_code);
                    return;
#endif // ASMJIT_X86

                case InstructionCode.Imul:
                case InstructionCode.Mul:
                case InstructionCode.Idiv:
                case InstructionCode.Div:
                    // INST dst_lo (implicit), dst_hi (implicit), src (explicit)
                    Debug.Assert(_operands.Length == 3);
                    a.EmitInstruction(_code, _operands[2]);
                    return;

                case InstructionCode.MovPtr:
                    break;

                case InstructionCode.Lahf:
                case InstructionCode.Sahf:
                    a.EmitInstruction(_code);
                    return;

                case InstructionCode.Maskmovdqu:
                case InstructionCode.Maskmovq:
                    a.EmitInstruction(_code, _operands[1], _operands[2]);
                    return;

                case InstructionCode.Enter:
                case InstructionCode.Leave:
                    // TODO: SPECIAL INSTRUCTION.
                    break;

                case InstructionCode.Ret:
                    // TODO: SPECIAL INSTRUCTION.
                    break;

                case InstructionCode.Monitor:
                case InstructionCode.Mwait:
                    // TODO: MONITOR/MWAIT (COMPILER).
                    break;

                case InstructionCode.Pop:
                case InstructionCode.Popad:
                case InstructionCode.Popfd:
                case InstructionCode.Popfq:
                    // TODO: SPECIAL INSTRUCTION.
                    break;

                case InstructionCode.Push:
                case InstructionCode.Pushad:
                case InstructionCode.Pushfd:
                case InstructionCode.Pushfq:
                    // TODO: SPECIAL INSTRUCTION.
                    break;

                case InstructionCode.Rcl:
                case InstructionCode.Rcr:
                case InstructionCode.Rol:
                case InstructionCode.Ror:
                case InstructionCode.Sal:
                case InstructionCode.Sar:
                case InstructionCode.Shl:
                case InstructionCode.Shr:
                    a.EmitInstruction(_code, _operands[0], GPReg.cl);
                    return;

                case InstructionCode.Shld:
                case InstructionCode.Shrd:
                    a.EmitInstruction(_code, _operands[0], _operands[1], GPReg.cl);
                    return;

                case InstructionCode.Rdtsc:
                case InstructionCode.Rdtscp:
                    a.EmitInstruction(_code);
                    return;

                case InstructionCode.RepLodsb:
                case InstructionCode.RepLodsd:
                case InstructionCode.RepLodsq:
                case InstructionCode.RepLodsw:
                case InstructionCode.RepMovsb:
                case InstructionCode.RepMovsd:
                case InstructionCode.RepMovsq:
                case InstructionCode.RepMovsw:
                case InstructionCode.RepStosb:
                case InstructionCode.RepStosd:
                case InstructionCode.RepStosq:
                case InstructionCode.RepStosw:
                case InstructionCode.RepeCmpsb:
                case InstructionCode.RepeCmpsd:
                case InstructionCode.RepeCmpsq:
                case InstructionCode.RepeCmpsw:
                case InstructionCode.RepeScasb:
                case InstructionCode.RepeScasd:
                case InstructionCode.RepeScasq:
                case InstructionCode.RepeScasw:
                case InstructionCode.RepneCmpsb:
                case InstructionCode.RepneCmpsd:
                case InstructionCode.RepneCmpsq:
                case InstructionCode.RepneCmpsw:
                case InstructionCode.RepneScasb:
                case InstructionCode.RepneScasd:
                case InstructionCode.RepneScasq:
                case InstructionCode.RepneScasw:
                    a.EmitInstruction(_code);
                    return;

                default:
                    Debug.Assert(false);
                    return;
                }
                // ${SPECIAL_INSTRUCTION_HANDLING_END}
            }

            switch (_operands.Length)
            {
            case 0:
                a.EmitInstruction(_code);
                break;
            case 1:
                a.EmitInstruction(_code, _operands[0]);
                break;
            case 2:
                a.EmitInstruction(_code, _operands[0], _operands[1]);
                break;
            case 3:
                a.EmitInstruction(_code, _operands[0], _operands[1], _operands[2]);
                break;
            default:
                Debug.Assert(false);
                break;
            }
        }
    }
}
