﻿namespace AsmJitNet2
{
    using System;
    using System.Collections.Generic;
    using Debug = System.Diagnostics.Debug;

    public class CompilerContext
    {
        private Compiler _compiler;
        private Function _function;
        private Emittable _start;
        private Emittable _stop;
        private Emittable _extraBlock;
        private readonly StateData _state = new StateData();
        private VarData _active;
        private ForwardJumpData _forwardJumps;
        private int _currentOffset;

        /// <summary>
        /// Whether current code is unreachable
        /// </summary>
        private bool _unreachable;

        private int _modifiedGPRegisters;
        private int _modifiedMMRegisters;
        private int _modifiedXMMRegisters;

        /// <summary>
        /// Whether EBP/RBP register can be used by register allocator
        /// </summary>
        private bool _allocableEBP;

        /// <summary>
        /// Whether ESP/RSP register can be used by register allocator
        /// </summary>
        private bool _allocableESP;

        /// <summary>
        /// ESP adjust constant (changed during PUSH/POP or when using stack)
        /// </summary>
        private int _adjustESP;

        /// <summary>
        /// Function arguments base pointer (register)
        /// </summary>
        private RegIndex _argumentsBaseReg;
        /// <summary>
        /// Function arguments base offset
        /// </summary>
        private int _argumentsBaseOffset;
        private int _argumentsActualDisp;

        /// <summary>
        /// Function variables base pointer (register)
        /// </summary>
        private RegIndex _variablesBaseReg;
        /// <summary>
        /// Function variables base offset
        /// </summary>
        private int _variablesBaseOffset;
        private int _variablesActualDisp;

        /// <summary>
        /// Used memory blocks (for variables, here is each created memory block that can be also in _memFree list)
        /// </summary>
        private VarMemBlock _memUsed;
        /// <summary>
        /// Free memory blocks (freed, prepared for another allocation)
        /// </summary>
        private VarMemBlock _memFree;
        /// <summary>
        /// Count of 4-bytes memory blocks used by the function
        /// </summary>
        private int _mem4BlocksCount;
        /// <summary>
        /// Count of 8-bytes memory blocks used by the function
        /// </summary>
        private int _mem8BlocksCount;
        /// <summary>
        /// Count of 16-bytes memory blocks used by the function
        /// </summary>
        private int _mem16BlocksCount;
        /// <summary>
        /// Count of total bytes of stack memory used by the function
        /// </summary>
        private int _memBytesTotal;

        private bool _emitComments;

        public CompilerContext(Compiler compiler)
        {
            _compiler = compiler;
            Clear();

            _emitComments = compiler.Logger != null && compiler.Logger.IsUsed;
        }

        public Compiler Compiler
        {
            get
            {
                return _compiler;
            }
        }

        public Function Function
        {
            get
            {
                return _function;
            }

            internal set
            {
                _function = value;
            }
        }

        public Emittable Start
        {
            get
            {
                return _start;
            }

            internal set
            {
                _start = value;
            }
        }

        public Emittable Stop
        {
            get
            {
                return _stop;
            }

            internal set
            {
                _stop = value;
            }
        }

        public Emittable ExtraBlock
        {
            get
            {
                return _extraBlock;
            }

            set
            {
                _extraBlock = value;
            }
        }

        public VarData Active
        {
            get
            {
                return _active;
            }

            internal set
            {
                _active = value;
            }
        }

        public ForwardJumpData ForwardJumps
        {
            get
            {
                return _forwardJumps;
            }

            internal set
            {
                _forwardJumps = value;
            }
        }

        public int CurrentOffset
        {
            get
            {
                return _currentOffset;
            }

            internal set
            {
                _currentOffset = value;
            }
        }

        public bool Unreachable
        {
            get
            {
                return _unreachable;
            }

            set
            {
                _unreachable = value;
            }
        }

        internal int ModifiedGPRegisters
        {
            get
            {
                return _modifiedGPRegisters;
            }
        }

        internal int ModifiedMMRegisters
        {
            get
            {
                return _modifiedMMRegisters;
            }
        }

        internal int ModifiedXMMRegisters
        {
            get
            {
                return _modifiedXMMRegisters;
            }
        }

        internal RegIndex ArgumentsBaseReg
        {
            get
            {
                return _argumentsBaseReg;
            }

            set
            {
                _argumentsBaseReg = value;
            }
        }

        internal int ArgumentsBaseOffset
        {
            get
            {
                return _argumentsBaseOffset;
            }

            set
            {
                _argumentsBaseOffset = value;
            }
        }

        internal RegIndex VariablesBaseReg
        {
            get
            {
                return _variablesBaseReg;
            }

            set
            {
                _variablesBaseReg = value;
            }
        }

        internal int VariablesBaseOffset
        {
            get
            {
                return _variablesBaseOffset;
            }

            set
            {
                _variablesBaseOffset = value;
            }
        }

        internal int Mem16BlocksCount
        {
            get
            {
                return _mem16BlocksCount;
            }
        }

        internal int MemBytesTotal
        {
            get
            {
                return _memBytesTotal;
            }
        }

        internal StateData State
        {
            get
            {
                return _state;
            }
        }

        internal void Clear()
        {
            //_zone.clear();
            _function = null;

            _start = null;
            _stop = null;

            _state.Clear();
            _active = null;

            _forwardJumps = null;

            _currentOffset = 0;
            //_unreachable = 0;

            _modifiedGPRegisters = 0;
            _modifiedMMRegisters = 0;
            _modifiedXMMRegisters = 0;

            _allocableEBP = false;
            _allocableESP = false;

            _adjustESP = 0;

            _argumentsBaseReg = RegIndex.Invalid;
            _argumentsBaseOffset = 0;
            _argumentsActualDisp = 0;

            _variablesBaseReg = RegIndex.Invalid;
            _variablesBaseOffset = 0;
            _variablesActualDisp = 0;

            _memUsed = null;
            _memFree = null;

            _mem4BlocksCount = 0;
            _mem8BlocksCount = 0;
            _mem16BlocksCount = 0;

            _memBytesTotal = 0;
        }

        internal void AllocMemoryOperands()
        {
            VarMemBlock mem;

            // Variables are allocated in this order:
            // 1. 16-bytes variables.
            // 2. 8-bytes variables.
            // 3. 4-bytes variables.
            // 4. All others.

            int start16 = 0;
            int start8 = start16 + _mem16BlocksCount * 16;
            int start4 = start8 + _mem8BlocksCount * 8;
            int startX = (start4 + _mem4BlocksCount * 4 + 15) & ~15;

            for (mem = _memUsed; mem != null; mem = mem.NextUsed)
            {
                int size = mem.Size;
                int offset;

                switch (size)
                {
                case 16:
                    offset = start16;
                    start16 += 16;
                    break;

                case 8:
                    offset = start8;
                    start8 += 8;
                    break;

                case 4:
                    offset = start4;
                    start4 += 4;
                    break;

                default:
                    // Align to 16 bytes if size is 16 or more.
                    if (size >= 16)
                    {
                        size = (size + 15) & ~15;
                        startX = (startX + 15) & ~15;
                    }
                    offset = startX;
                    startX += size;
                    break;
                }

                mem.Offset = offset;
                _memBytesTotal += size;
            }
        }

        internal void PatchMemoryOperands(Emittable start, Emittable stop)
        {
            Emittable current;

            for (current = start; ; current = current.Next)
            {
                Instruction instruction = current as Instruction;
                if (instruction != null)
                {
                    Mem mem = instruction.MemoryOperand;

                    if (mem != null && (mem.Id & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData varData = _compiler.GetVarData(mem.Id);
                        Debug.Assert(varData != null);

                        if (varData.IsMemArgument)
                        {
                            mem.Base = _argumentsBaseReg;
                            mem.Displacement += varData.HomeMemoryOffset;
                            mem.Displacement += _argumentsBaseOffset;
                        }
                        else
                        {
                            VarMemBlock mb = varData.HomeMemoryData;
                            Debug.Assert(mb != null);

                            mem.Base = _variablesBaseReg;
                            mem.Displacement += mb.Offset;
                            mem.Displacement += _variablesBaseOffset;
                        }
                    }
                }

                if (current == stop)
                    break;
            }
        }

        internal bool IsActive(VarData varData)
        {
            return varData.NextActive != null;
        }

        internal void AddActive(VarData varData)
        {
            if (varData.NextActive != null)
                throw new ArgumentException();
            if (varData.PreviousActive != null)
                throw new ArgumentException();

            if (_active == null)
            {
                varData.NextActive = varData;
                varData.PreviousActive = varData;
                _active = varData;
            }
            else
            {
                VarData vlast = _active.PreviousActive;
                vlast.NextActive = varData;
                _active.PreviousActive = varData;
                varData.NextActive = _active;
                varData.PreviousActive = vlast;
            }
        }

        internal void FreeActive(VarData varData)
        {
            throw new NotImplementedException();
        }

        public void AllocVar(VarData varData, RegIndex regIndex, VariableAlloc variableAlloc)
        {
            switch (varData.Type)
            {
            case VariableType.GPD:
#if ASMJIT_X64
            case VariableType.GPQ:
#endif // ASMJIT_X64
                AllocGPVar(varData, regIndex, variableAlloc);
                break;

            case VariableType.X87:
            case VariableType.X87_1F:
            case VariableType.X87_1D:
                // TODO: X87 VARIABLES NOT IMPLEMENTED.
                break;

            case VariableType.MM:
                AllocMMVar(varData, regIndex, variableAlloc);
                break;

            case VariableType.XMM:
            case VariableType.XMM_1F:
            case VariableType.XMM_4F:
            case VariableType.XMM_1D:
            case VariableType.XMM_2D:
                AllocXMMVar(varData, regIndex, variableAlloc);
                break;
            }

            PostAlloc(varData, variableAlloc);
        }

        public void SaveVar(VarData vdata)
        {
            throw new NotImplementedException();
        }

        public void AllocGPVar(VarData varData, RegIndex regIndex, VariableAlloc variableAlloc)
        {
            int i;

            // Preferred register code.
            RegIndex pref = (regIndex != RegIndex.Invalid) ? regIndex : varData.PreferredRegisterIndex;
            // Last register code (aka home).
            RegIndex home = varData.HomeRegisterIndex;
            // New register code.
            RegIndex idx = RegIndex.Invalid;

            // Preserved GP variables.
            int preservedGP = varData.Scope.Prototype.PreservedGP;

            // Spill candidate.
            VarData spillCandidate = null;

            // Whether to alloc non-preserved first or last.
            bool nonPreservedFirst = true;
            if (this.Function.IsCaller)
                nonPreservedFirst = false;

            // --------------------------------------------------------------------------
            // [Already Allocated]
            // --------------------------------------------------------------------------

            // Go away if variable is already allocated.
            if (varData.State == VariableState.Register)
            {
                RegIndex oldIndex = varData.RegisterIndex;
                RegIndex newIndex = pref;

                // Preferred register is none or same as currently allocated one, this is
                // best case.
                if (pref == RegIndex.Invalid || oldIndex == newIndex)
                    return;

                VarData other = _state.GP[(int)newIndex];

                EmitExchangeVar(varData, pref, variableAlloc, other);
                if (other != null)
                    other.RegisterIndex = oldIndex;

                // Update VarData.
                varData.State = VariableState.Register;
                varData.RegisterIndex = newIndex;
                varData.HomeRegisterIndex = newIndex;

                _state.GP[(int)oldIndex] = other;
                _state.GP[(int)newIndex] = varData;
                return;
            }

            // --------------------------------------------------------------------------
            // [Find Unused GP]
            // --------------------------------------------------------------------------

            // Preferred register.
            if (pref != RegIndex.Invalid)
            {
                if ((_state.UsedGP & (1U << (int)pref)) == 0)
                {
                    idx = pref;
                }
                else
                {
                    throw new NotImplementedException();
                    //// Spill register we need
                    //spillCandidate = _state.gp[pref];

                    //// Jump to spill part of allocation
                    //goto L_Spill;
                }
            }

            // Home register code.
            if (idx == RegIndex.Invalid && home != RegIndex.Invalid)
            {
                if ((_state.UsedGP & (1U << (int)home)) == 0)
                    idx = home;
            }

            // We start from 1, because EAX/RAX register is sometimes explicitly
            // needed. So we trying to prevent register reallocation.
            if (idx == RegIndex.Invalid)
            {
                int mask;
                for (i = 1, mask = (1 << i); i < (int)RegNum.GP; i++, mask <<= 1)
                {
                    if ((_state.UsedGP & mask) == 0 &&
                        (i != (int)RegIndex.Ebp || _allocableEBP) &&
                        (i != (int)RegIndex.Esp || _allocableESP))
                    {
                        // Convenience to alloc non-preserved first or non-preserved last.
                        if (nonPreservedFirst)
                        {
                            if (idx != RegIndex.Invalid && (preservedGP & mask) != 0)
                                continue;
                            idx = (RegIndex)i;
                            // If current register is preserved, we should try to find different
                            // one that is not. This can save one push / pop in prolog / epilog.
                            if ((preservedGP & mask) == 0)
                                break;
                        }
                        else
                        {
                            if (idx != RegIndex.Invalid && (preservedGP & mask) == 0)
                                continue;
                            idx = (RegIndex)i;
                            // The opposite.
                            if ((preservedGP & mask) != 0)
                                break;
                        }
                    }
                }
            }

            // If not found, try EAX/RAX.
            if (idx == RegIndex.Invalid && (_state.UsedGP & 1) == 0)
            {
                idx = RegIndex.Eax;
            }

            // --------------------------------------------------------------------------
            // [Spill]
            // --------------------------------------------------------------------------

            // If register is still not found, spill other variable.
            if (idx == RegIndex.Invalid)
            {
                if (spillCandidate == null)
                {
                    spillCandidate = GetSpillCandidateGP();
                }

                // Spill candidate not found?
                if (spillCandidate == null)
                {
                    _compiler.Error = Errors.NotEnoughRegisters;
                    return;
                }

            L_Spill:

                // Prevented variables can't be spilled. _getSpillCandidate() never returns
                // prevented variables, but when jumping to L_spill it can happen.
                if (spillCandidate.WorkOffset == _currentOffset)
                {
                    _compiler.Error = Errors.RegistersOverlap;
                    return;
                }

                idx = spillCandidate.RegisterIndex;
                SpillGPVar(spillCandidate);
            }

            // --------------------------------------------------------------------------
            // [Alloc]
            // --------------------------------------------------------------------------

            if (varData.State == VariableState.Memory && (variableAlloc & VariableAlloc.Read) != 0)
            {
                EmitLoadVar(varData, idx);
            }

            // Update VarData.
            varData.State = VariableState.Register;
            varData.RegisterIndex = idx;
            varData.HomeRegisterIndex = idx;

            // Update StateData.
            AllocatedVariable(varData);
        }

        public void EmitExchangeVar(VarData vdata, RegIndex regIndex, VariableAlloc vflags, VarData other)
        {
            // Caller must ensure that variable is allocated.
            Debug.Assert(vdata.RegisterIndex != RegIndex.Invalid);

            // If other is not valid then we can just emit MOV (or other similar instruction).
            if (other == null)
            {
                EmitMoveVar(vdata, regIndex, vflags);
                return;
            }

            // If we need to alloc for write-only operation then we can move other
            // variable away instead of exchanging them.
            if ((vflags & VariableAlloc.Read) == 0)
            {
                EmitMoveVar(other, vdata.RegisterIndex, VariableAlloc.Read);
                return;
            }

            switch (vdata.Type)
            {
            case VariableType.GPD:
                _compiler.Emit(InstructionCode.Xchg, Register.gpd(regIndex), Register.gpd(vdata.RegisterIndex));
                break;
#if ASMJIT_X64
            case VariableType.GPQ:
                _compiler.Emit(InstructionCode.Xchg, Register.gpq(regIndex), Register.gpq(vdata.RegisterIndex));
                break;
#endif // ASMJIT_X64

            case VariableType.X87:
            case VariableType.X87_1F:
            case VariableType.X87_1D:
                // TODO: X87 VARIABLES NOT IMPLEMENTED.
                break;

            // NOTE: MM and XMM registers shoudln't be exchanged using this way, it's
            // correct, but it sucks.

            case VariableType.MM:
                {
                    MMReg a = Register.mm(regIndex);
                    MMReg b = Register.mm(vdata.RegisterIndex);

                    _compiler.Emit(InstructionCode.Pxor, a, b);
                    _compiler.Emit(InstructionCode.Pxor, b, a);
                    _compiler.Emit(InstructionCode.Pxor, a, b);
                    break;
                }

            case VariableType.XMM_1F:
            case VariableType.XMM_4F:
                {
                    XMMReg a = Register.xmm(regIndex);
                    XMMReg b = Register.xmm(vdata.RegisterIndex);

                    _compiler.Emit(InstructionCode.Xorps, a, b);
                    _compiler.Emit(InstructionCode.Xorps, b, a);
                    _compiler.Emit(InstructionCode.Xorps, a, b);
                    break;
                }

            case VariableType.XMM_1D:
            case VariableType.XMM_2D:
                {
                    XMMReg a = Register.xmm(regIndex);
                    XMMReg b = Register.xmm(vdata.RegisterIndex);

                    _compiler.Emit(InstructionCode.Xorpd, a, b);
                    _compiler.Emit(InstructionCode.Xorpd, b, a);
                    _compiler.Emit(InstructionCode.Xorpd, a, b);
                    break;
                }

            case VariableType.XMM:
                {
                    XMMReg a = Register.xmm(regIndex);
                    XMMReg b = Register.xmm(vdata.RegisterIndex);

                    _compiler.Emit(InstructionCode.Pxor, a, b);
                    _compiler.Emit(InstructionCode.Pxor, b, a);
                    _compiler.Emit(InstructionCode.Pxor, a, b);
                    break;
                }
            }
        }

        private VarData GetSpillCandidateGP()
        {
            return GetSpillCandidateGeneric(_state.GP);
        }

        private VarData GetSpillCandidateMM()
        {
            return GetSpillCandidateGeneric(_state.MM);
        }

        private VarData GetSpillCandidateXMM()
        {
            return GetSpillCandidateGeneric(_state.XMM);
        }

        private VarData GetSpillCandidateGeneric(IList<VarData> varArray)
        {
            int i;

            VarData candidate = null;
            int candidatePriority = 0;
            int candidateScore = 0;
            int count = varArray.Count;

            int currentOffset = _compiler.CurrentEmittable.Offset;

            for (i = 0; i < count; i++)
            {
                // Get variable.
                VarData vdata = varArray[i];

                // Never spill variables needed for next instruction.
                if (vdata == null || vdata.WorkOffset == _currentOffset)
                    continue;

                int variablePriority = vdata.Priority;
                int variableScore = GetSpillScore(vdata, currentOffset);

                if ((candidate == null) ||
                    (variablePriority > candidatePriority) ||
                    (variablePriority == candidatePriority && variableScore > candidateScore))
                {
                    candidate = vdata;
                    candidatePriority = variablePriority;
                    candidateScore = variableScore;
                }
            }

            return candidate;
        }

        private static int GetSpillScore(VarData vdata, int currentOffset)
        {
            int score = 0;

            Debug.Assert(vdata.LastEmittable != null);
            int lastOffset = vdata.LastEmittable.Offset;

            if (lastOffset >= currentOffset)
                score += lastOffset - currentOffset;

            // Each write access decreases probability of spill.
            score -= vdata.RegisterWriteCount + vdata.RegisterRWCount;
            // Each read-only access increases probability of spill.
            score += vdata.RegisterReadCount;

            // Each memory access increases probability of spill.
            score += vdata.MemoryWriteCount + vdata.MemoryRWCount;
            score += vdata.MemoryReadCount;

            return score;
        }

        public void SpillGPVar(VarData vdata)
        {
            // Can't spill variable that isn't allocated.
            Debug.Assert(vdata.State == VariableState.Register);
            Debug.Assert(vdata.RegisterIndex != RegIndex.Invalid);

            RegIndex idx = vdata.RegisterIndex;

            if (vdata.Changed)
                EmitSaveVar(vdata, idx);

            // Update VarData.
            vdata.RegisterIndex = RegIndex.Invalid;
            vdata.State = VariableState.Memory;
            vdata.Changed = false;

            // Update StateData.
            _state.GP[(int)idx] = null;
            FreedGPRegister(idx);
        }

        public void SaveMMVar(VarData vdata)
        {
            throw new NotImplementedException();
        }

        public void SpillMMVar(VarData vdata)
        {
            throw new NotImplementedException();
        }

        public void SaveXMMVar(VarData vdata)
        {
            throw new NotImplementedException();
        }

        public void SpillXMMVar(VarData vdata)
        {
            throw new NotImplementedException();
        }

        private void FreedGPRegister(RegIndex index)
        {
            _state.UsedGP &= ~(1 << (int)index);
        }

        private void FreedMMRegister(RegIndex index)
        {
            _state.UsedMM &= ~(1 << (int)index);
        }

        private void FreedXMMRegister(RegIndex index)
        {
            _state.UsedXMM &= ~(1 << (int)index);
        }

        public void EmitLoadVar(VarData varData, RegIndex regIndex)
        {
            Mem m = GetVarMem(varData);

            switch (varData.Type)
            {
            case VariableType.GPD:
                _compiler.Emit(InstructionCode.Mov, Register.gpd(regIndex), m);
                if (_emitComments)
                    goto addComment;
                break;
#if ASMJIT_X64
            case VariableType.GPQ:
                _compiler.Emit(InstructionCode.Mov, Register.gpq(regIndex), m);
                if (_emitComments)
                    goto addComment;
                break;
#endif // ASMJIT_X64

            case VariableType.X87:
            case VariableType.X87_1F:
            case VariableType.X87_1D:
                // TODO: X87 VARIABLES NOT IMPLEMENTED.
                break;

            case VariableType.MM:
                _compiler.Emit(InstructionCode.Movq, Register.mm(regIndex), m);
                if (_emitComments)
                    goto addComment;
                break;

            case VariableType.XMM:
                _compiler.Emit(InstructionCode.Movdqa, Register.xmm(regIndex), m);
                if (_emitComments)
                    goto addComment;
                break;
            case VariableType.XMM_1F:
                _compiler.Emit(InstructionCode.Movss, Register.xmm(regIndex), m);
                if (_emitComments)
                    goto addComment;
                break;
            case VariableType.XMM_1D:
                _compiler.Emit(InstructionCode.Movsd, Register.xmm(regIndex), m);
                if (_emitComments)
                    goto addComment;
                break;
            case VariableType.XMM_4F:
                _compiler.Emit(InstructionCode.Movaps, Register.xmm(regIndex), m);
                if (_emitComments)
                    goto addComment;
                break;
            case VariableType.XMM_2D:
                _compiler.Emit(InstructionCode.Movapd, Register.xmm(regIndex), m);
                if (_emitComments)
                    goto addComment;
                break;
            }
            return;

        addComment:
            _compiler.CurrentEmittable.Comment = string.Format("Alloc {0}", varData.Name);
        }

        internal Mem GetVarMem(VarData varData)
        {
            Mem m = new Mem();
            m.Id = varData.Id;
            if (!varData.IsMemArgument)
                m.Displacement = (IntPtr)_adjustESP;

            MarkMemoryUsed(varData);
            return m;
        }

        public void AllocMMVar(VarData varData, RegIndex regIndex, VariableAlloc variableAlloc)
        {
            throw new NotImplementedException();
        }

        public void AllocXMMVar(VarData varData, RegIndex regIndex, VariableAlloc variableAlloc)
        {
            throw new NotImplementedException();
        }

        private void PostAlloc(VarData varData, VariableAlloc variableAlloc)
        {
            if ((variableAlloc & VariableAlloc.Write) != 0)
                varData.Changed = true;
        }

        public void SpillVar(VarData vdata)
        {
            switch (vdata.Type)
            {
            case VariableType.GPD:
#if ASMJIT_X64
            case VariableType.GPQ:
#endif // ASMJIT_X64
                SpillGPVar(vdata);
                break;

            case VariableType.X87:
            case VariableType.X87_1F:
            case VariableType.X87_1D:
                // TODO: X87 VARIABLES NOT IMPLEMENTED.
                throw new NotImplementedException();

            case VariableType.MM:
                SpillMMVar(vdata);
                break;

            case VariableType.XMM:
            case VariableType.XMM_1F:
            case VariableType.XMM_4F:
            case VariableType.XMM_1D:
            case VariableType.XMM_2D:
                SpillXMMVar(vdata);
                break;
            }
        }

        public void EmitSaveVar(VarData vdata, RegIndex regIndex)
        {
            // Caller must ensure that variable is allocated.
            Debug.Assert(regIndex != RegIndex.Invalid);

            Mem m = GetVarMem(vdata);

            switch (vdata.Type)
            {
            case VariableType.GPD:
                _compiler.Emit(InstructionCode.Mov, m, Register.gpd(regIndex));
                if (_emitComments)
                    goto addComment;
                break;
#if ASMJIT_X64
            case VariableType.GPQ:
                _compiler.Emit(InstructionCode.Mov, m, Register.gpq(regIndex));
                if (_emitComments)
                    goto addComment;
                break;
#endif // ASMJIT_X64

            case VariableType.X87:
            case VariableType.X87_1F:
            case VariableType.X87_1D:
                // TODO: X87 VARIABLES NOT IMPLEMENTED.
                break;

            case VariableType.MM:
                _compiler.Emit(InstructionCode.Movq, m, Register.mm(regIndex));
                if (_emitComments)
                    goto addComment;
                break;

            case VariableType.XMM:
                _compiler.Emit(InstructionCode.Movdqa, m, Register.xmm(regIndex));
                if (_emitComments)
                    goto addComment;
                break;
            case VariableType.XMM_1F:
                _compiler.Emit(InstructionCode.Movss, m, Register.xmm(regIndex));
                if (_emitComments)
                    goto addComment;
                break;
            case VariableType.XMM_1D:
                _compiler.Emit(InstructionCode.Movsd, m, Register.xmm(regIndex));
                if (_emitComments)
                    goto addComment;
                break;
            case VariableType.XMM_4F:
                _compiler.Emit(InstructionCode.Movaps, m, Register.xmm(regIndex));
                if (_emitComments)
                    goto addComment;
                break;
            case VariableType.XMM_2D:
                _compiler.Emit(InstructionCode.Movapd, m, Register.xmm(regIndex));
                if (_emitComments)
                    goto addComment;
                break;
            }
            return;

        addComment:
            _compiler.CurrentEmittable.Comment = string.Format("Spill {0}", vdata.Name);
        }

        public void EmitMoveVar(VarData vdata, RegIndex regIndex, VariableAlloc vflags)
        {
            // Caller must ensure that variable is allocated.
            Debug.Assert(vdata.RegisterIndex != RegIndex.Invalid);

            if ((vflags & VariableAlloc.Read) == 0)
                return;

            switch (vdata.Type)
            {
            case VariableType.GPD:
                _compiler.Emit(InstructionCode.Mov, Register.gpd(regIndex), Register.gpd(vdata.RegisterIndex));
                break;
#if ASMJIT_X64
            case VariableType.GPQ:
                _compiler.Emit(InstructionCode.Mov, Register.gpq(regIndex), Register.gpq(vdata.RegisterIndex));
                break;
#endif // ASMJIT_X64

            case VariableType.X87:
            case VariableType.X87_1F:
            case VariableType.X87_1D:
                // TODO: X87 VARIABLES NOT IMPLEMENTED.
                throw new NotImplementedException();

            case VariableType.MM:
                _compiler.Emit(InstructionCode.Movq, Register.mm(regIndex), Register.mm(vdata.RegisterIndex));
                break;

            case VariableType.XMM:
                _compiler.Emit(InstructionCode.Movdqa, Register.xmm(regIndex), Register.xmm(vdata.RegisterIndex));
                break;
            case VariableType.XMM_1F:
                _compiler.Emit(InstructionCode.Movss, Register.xmm(regIndex), Register.xmm(vdata.RegisterIndex));
                break;
            case VariableType.XMM_1D:
                _compiler.Emit(InstructionCode.Movsd, Register.xmm(regIndex), Register.xmm(vdata.RegisterIndex));
                break;
            case VariableType.XMM_4F:
                _compiler.Emit(InstructionCode.Movaps, Register.xmm(regIndex), Register.xmm(vdata.RegisterIndex));
                break;
            case VariableType.XMM_2D:
                _compiler.Emit(InstructionCode.Movapd, Register.xmm(regIndex), Register.xmm(vdata.RegisterIndex));
                break;
            }
        }

        public void UnuseVar(VarData vdata, VariableState toState)
        {
            Debug.Assert(toState != VariableState.Register);

            if (vdata.State == VariableState.Register)
            {
                RegIndex registerIndex = vdata.RegisterIndex;
                switch (vdata.Type)
                {
                case VariableType.GPD:
#if ASMJIT_X64
                case VariableType.GPQ:
#endif // ASMJIT_X64
                    _state.GP[(int)registerIndex] = null;
                    FreedGPRegister(registerIndex);
                    break;

                case VariableType.X87:
                case VariableType.X87_1F:
                case VariableType.X87_1D:
                    // TODO: X87 VARIABLES NOT IMPLEMENTED.
                    break;

                case VariableType.MM:
                    _state.MM[(int)registerIndex] = null;
                    FreedMMRegister(registerIndex);
                    break;

                case VariableType.XMM:
                case VariableType.XMM_1F:
                case VariableType.XMM_4F:
                case VariableType.XMM_1D:
                case VariableType.XMM_2D:
                    _state.XMM[(int)registerIndex] = null;
                    FreedXMMRegister(registerIndex);
                    break;
                }
            }

            vdata.State = toState;
            vdata.Changed = false;
            vdata.RegisterIndex = RegIndex.Invalid;
        }

        public void UnuseVarOnEndOfScope(Emittable e, VarData vdata)
        {
            if (vdata.LastEmittable == e)
                UnuseVar(vdata, VariableState.Unused);
        }

        internal void AllocatedVariable(VarData vdata)
        {
            RegIndex idx = vdata.RegisterIndex;

            switch (vdata.Type)
            {
            case VariableType.GPD:
            case VariableType.GPQ:
                _state.GP[(int)idx] = vdata;
                AllocatedGPRegister(idx);
                break;

            case VariableType.MM:
                _state.MM[(int)idx] = vdata;
                AllocatedMMRegister(idx);
                break;

            case VariableType.XMM:
            case VariableType.XMM_1F:
            case VariableType.XMM_4F:
            case VariableType.XMM_1D:
            case VariableType.XMM_2D:
                _state.XMM[(int)idx] = vdata;
                AllocatedXMMRegister(idx);
                break;

            default:
                Debug.Assert(false);
                break;
            }
        }

        private void AllocatedGPRegister(RegIndex index)
        {
            _state.UsedGP |= (1 << (int)index);
            _modifiedGPRegisters |= (1 << (int)index);
        }

        private void AllocatedMMRegister(RegIndex index)
        {
            _state.UsedMM |= (1 << (int)index);
            _modifiedMMRegisters |= (1 << (int)index);
        }

        private void AllocatedXMMRegister(RegIndex index)
        {
            _state.UsedXMM |= (1 << (int)index);
            _modifiedXMMRegisters |= (1 << (int)index);
        }

        internal void MarkMemoryUsed(VarData varData)
        {
            if (varData.HomeMemoryData != null)
                return;

            VarMemBlock mem = AllocMemBlock(varData.Size);
            if (mem == null)
                return;

            varData.HomeMemoryData = mem;
        }

        private VarMemBlock AllocMemBlock(int size)
        {
            if (size <= 0)
                throw new ArgumentOutOfRangeException();

            // First try to find mem blocks.
            VarMemBlock mem = _memFree;
            VarMemBlock prev = null;

            while (mem != null)
            {
                VarMemBlock next = mem.NextFree;

                if (mem.Size == size)
                {
                    if (prev != null)
                        prev.NextFree = next;
                    else
                        _memFree = next;

                    mem.NextFree = null;
                    return mem;
                }

                prev = mem;
                mem = next;
            }

            // Never mind, create new.
            mem = new VarMemBlock();
            if (mem == null)
            {
                _compiler.Error = Errors.NoHeapMemory;
                return null;
            }

            mem.Offset = 0;
            mem.Size = size;

            mem.NextUsed = _memUsed;
            mem.NextFree = null;

            _memUsed = mem;

            switch (size)
            {
            case 16:
                _mem16BlocksCount++;
                break;
            case 8:
                _mem8BlocksCount++;
                break;
            case 4:
                _mem4BlocksCount++;
                break;
            }

            return mem;
        }

        public void TranslateOperands(Operand[] operands)
        {
            int i;
            int count = operands.Length;

            // Translate variables to registers.
            for (i = 0; i < count; i++)
            {
                Operand o = operands[i];

                if (o.IsVar)
                {
                    VarData vdata = _compiler.GetVarData(o.Id);
                    Debug.Assert(vdata != null);
                    operands[i] = new GPReg((int)((BaseVar)o).RegisterCode | (int)vdata.RegisterIndex);
                }
                else if (o.IsMem)
                {
                    Mem mem = (Mem)o;

                    if ((o.Id & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        // Memory access. We just increment here actual displacement.
                        VarData vdata = _compiler.GetVarData(o.Id);
                        Debug.Assert(vdata != null);

                        mem.Displacement += vdata.IsMemArgument
                          ? _argumentsActualDisp
                          : _variablesActualDisp;
                        // NOTE: This is not enough, variable position will be patched later
                        // by CompilerContext::_patchMemoryOperands().
                    }
                    else if (((int)mem.Base & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = _compiler.GetVarData((int)mem.Base);
                        Debug.Assert(vdata != null);

                        mem.Base = vdata.RegisterIndex;
                    }

                    if (((int)mem.Index & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = _compiler.GetVarData((int)mem.Index);
                        Debug.Assert(vdata != null);

                        mem.Index = vdata.RegisterIndex;
                    }
                }
            }
        }

        public void AddForwardJump(Jmp instruction)
        {
            ForwardJumpData j;

            try
            {
                j = new ForwardJumpData();
            }
            catch (OutOfMemoryException)
            {
                _compiler.Error = Errors.NoHeapMemory;
                return;
            }

            j.Inst = instruction;
            j.State = SaveState();
            j.Next = _forwardJumps;
            _forwardJumps = j;
        }

        internal StateData SaveState()
        {
            StateData state = new StateData(_state);
            //memcpy(state, &_state, sizeof(StateData));

            state.ChangedGP = 0;
            state.ChangedMM = 0;
            state.ChangedXMM = 0;

            int i;
            int mask;

            for (i = 0, mask = 1; i < (int)RegNum.GP; i++, mask <<= 1)
            {
                if (state.GP[i] != null && state.GP[i].Changed)
                    state.ChangedGP |= mask;
            }

            for (i = 0, mask = 1; i < (int)RegNum.MM; i++, mask <<= 1)
            {
                if (state.MM[i] != null && state.MM[i].Changed)
                    state.ChangedMM |= mask;
            }

            for (i = 0, mask = 1; i < (int)RegNum.XMM; i++, mask <<= 1)
            {
                if (state.XMM[i] != null && state.XMM[i].Changed)
                    state.ChangedXMM |= mask;
            }

            return state;
        }

        internal void AssignState(StateData stateData)
        {
            Compiler compiler = Compiler;
            _state.CopyFrom(stateData);
            //memcpy(&_state, state, sizeof(StateData));

            int i;
            int len;
            int mask;

            // Clear all variables in registers.
            for (i = 0, len = compiler.Variables.Count; i < len; i++)
            {
                VarData varData = compiler.Variables[i];
                if (varData.State == VariableState.Register)
                {
                    varData.State = VariableState.Memory;
                }
            }

            for (i = 0, mask = 1; i < (int)RegNum.GP; i++, mask <<= 1)
            {
                VarData varData = _state.GP[i];
                if (varData != null)
                {
                    varData.State = VariableState.Register;
                    varData.RegisterIndex = (RegIndex)i;
                    varData.Changed = (_state.ChangedGP & mask) != 0;
                }
            }

            for (i = 0, mask = 1; i < (int)RegNum.MM; i++, mask <<= 1)
            {
                VarData varData = _state.MM[i];
                if (varData != null)
                {
                    varData.State = VariableState.Register;
                    varData.RegisterIndex = (RegIndex)i;
                    varData.Changed = (_state.ChangedMM & mask) != 0;
                }
            }

            for (i = 0, mask = 1; i < (int)RegNum.XMM; i++, mask <<= 1)
            {
                VarData varData = _state.XMM[i];
                if (varData != null)
                {
                    varData.State = VariableState.Register;
                    varData.RegisterIndex = (RegIndex)i;
                    varData.Changed = (_state.ChangedXMM & mask) != 0;
                }
            }
        }

        internal void RestoreState(StateData state, int targetOffset = Operand.InvalidValue)
        {
            // 16 + 8 + 16 = GP + MMX + XMM registers.

            StateData fromState = _state;
            StateData toState = state;

            if (fromState == toState)
                return;

            int @base;
            int i;

            // TODO: 16 + 8 + 16 is cryptic, make constants instead!

            // Spill.
            for (@base = 0, i = 0; i < 16 + 8 + 16; i++)
            {
                if (i == 16 || i == 16 + 8)
                    @base = i;

                VarData fromVar = fromState.Registers[i];
                VarData toVar = toState.Registers[i];

                if (fromVar != toVar)
                {
                    int regIndex = i - @base;

                    // Spill the register
                    if (fromVar != null)
                    {
                        // It is possible that variable that was saved in state currently not
                        // exists.
                        if (fromVar.State == VariableState.Unused)
                        {
                            // TODO: Implement it.
                            UnuseVar(fromVar, VariableState.Unused);

                            // Optimization, do not spill it, we can simply abandon it
                            // _freeReg(getVariableRegisterCode(from_v->type(), regIndex));
                        }
                        else
                        {
                            if (targetOffset != Operand.InvalidValue && fromVar.LastEmittable.Offset < targetOffset)
                            {
                                // Do not spill a variable that is out of scope.
                                UnuseVar(fromVar, VariableState.Unused);
                            }
                            else
                            {
                                // Normal state change, spill...
                                SpillVar(fromVar);
                            }
                        }
                    }
                }
                else if (fromVar != null)
                {
                    int msk = (1 << i);
                    // Variables are the same, we just need to compare changed flags.
                    if ((fromState.ChangedGP & msk) != 0 && (toState.ChangedGP & msk) == 0)
                    {
                        SaveVar(fromVar);
                    }
                }
            }

            // Alloc.
            for (@base = 0, i = 0; i < 16 + 8 + 16; i++)
            {
                if (i == 16 || i == 24)
                    @base = i;

                VarData fromVar = fromState.Registers[i];
                VarData toVar = toState.Registers[i];

                if (fromVar != toVar)
                {
                    int regIndex = i - @base;

                    // Alloc register
                    if (toVar != null)
                    {
                        AllocVar(toVar, (RegIndex)regIndex, VariableAlloc.Read);
                    }
                }

                // TODO:
                //if (toVar)
                //{
                // toVar->changed = to->changed;
                //}
            }

            // Update masks
            _state.UsedGP = state.UsedGP;
            _state.UsedMM = state.UsedMM;
            _state.UsedXMM = state.UsedXMM;
        }
    }
}