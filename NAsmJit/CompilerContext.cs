namespace NAsmJit
{
    using System;
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.Diagnostics.Contracts;

    public class CompilerContext
    {
        private readonly Compiler _compiler;
        private CompilerFunction _function;
        private CompilerItem _start;
        private CompilerItem _stop;
        private CompilerItem _extraBlock;
        private readonly StateData _state = new StateData(0);
        private CompilerVar _active;
        private ForwardJumpData _forwardJumps;

        /// <summary>
        /// Current offset, used in Prepare() stage. Each item should increment it.
        /// </summary>
        private int _currentOffset;

        /// <summary>
        /// Whether current code is unreachable
        /// </summary>
        private bool _unreachable;

        private RegisterMask _modifiedGPRegisters;
        private RegisterMask _modifiedMMRegisters;
        private RegisterMask _modifiedXMMRegisters;

        /// <summary>
        /// Whether the EBP/RBP register can be used by register allocator
        /// </summary>
        private bool _allocableEBP;

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

        /// <summary>
        /// List of items which need to be translated. These items are filled with AddBackwardCode().
        /// </summary>
        private readonly List<CompilerJmpInstruction> _backCode = new List<CompilerJmpInstruction>();

        /// <summary>
        /// Backward code position
        /// </summary>
        private int _backPos;

        internal CompilerContext(Compiler compiler)
        {
            if (compiler == null)
                throw new ArgumentNullException("compiler");
            Contract.EndContractBlock();

            _compiler = compiler;
            Clear();

            _emitComments = compiler.Logger != null;
        }

        public Compiler Compiler
        {
            get
            {
                Contract.Ensures(Contract.Result<Compiler>() != null);

                return _compiler;
            }
        }

        public bool AllocableEbp
        {
            get
            {
                return _allocableEBP;
            }
        }

        public CompilerFunction Function
        {
            get
            {
                return _function;
            }

            internal set
            {
                Contract.Requires(value != null);
                Contract.Ensures(Function != null);

                _function = value;
            }
        }

        public CompilerItem Start
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

        public CompilerItem Stop
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

        public CompilerItem ExtraBlock
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

        public CompilerVar Active
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

        internal RegisterMask ModifiedGPRegisters
        {
            get
            {
                return _modifiedGPRegisters;
            }
        }

        internal RegisterMask ModifiedMMRegisters
        {
            get
            {
                return _modifiedMMRegisters;
            }
        }

        internal RegisterMask ModifiedXMMRegisters
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

        internal int Mem8BlocksCount
        {
            get
            {
                return _mem8BlocksCount;
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
                Contract.Ensures(Contract.Result<StateData>() != null);

                return _state;
            }
        }

        internal ReadOnlyCollection<CompilerJmpInstruction> BackwardsCode
        {
            get
            {
                Contract.Ensures(Contract.Result<ReadOnlyCollection<CompilerJmpInstruction>>() != null);

                return _backCode.AsReadOnly();
            }
        }

        internal int BackwardsPosition
        {
            get
            {
                Contract.Ensures(Contract.Result<int>() >= 0);

                return _backPos;
            }

            set
            {
                Contract.Requires(value >= 0);

                _backPos = value;
            }
        }

        [ContractInvariantMethod]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance", "CA1822:MarkMembersAsStatic", Justification = "Required for code contracts.")]
        private void ObjectInvariant()
        {
            Contract.Invariant(_compiler != null);
            Contract.Invariant(_state != null);
            Contract.Invariant(_backPos >= 0);
        }


        internal void Clear()
        {
            Contract.Ensures(Function == null);

            //_zone.clear();
            _function = null;

            _start = null;
            _stop = null;

            _state.Clear();
            _active = null;

            _forwardJumps = null;

            _currentOffset = 0;
            //_unreachable = 0;

            _modifiedGPRegisters = RegisterMask.Zero;
            _modifiedMMRegisters = RegisterMask.Zero;
            _modifiedXMMRegisters = RegisterMask.Zero;

            _allocableEBP = false;

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

            _backCode.Clear();
            _backPos = 0;
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

        internal void PatchMemoryOperands(CompilerItem start, CompilerItem stop)
        {
            CompilerItem current;

            for (current = start; ; current = current.Next)
            {
                CompilerInstruction instruction = current as CompilerInstruction;
                if (instruction != null)
                {
                    Mem mem = instruction.MemoryOperand;

                    if (mem != null && (mem.Id & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        CompilerVar varData = _compiler.GetVarData(mem.Id);
                        Contract.Assert(varData != null);

                        if (varData.IsMemArgument)
                        {
                            mem.Base = _argumentsBaseReg;
                            mem.Displacement += varData.HomeMemoryOffset;
                            mem.Displacement += _argumentsBaseOffset;
                        }
                        else
                        {
                            VarMemBlock mb = varData.HomeMemoryData;
                            if (mb == null)
                                throw new CompilerException();

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

        internal bool IsActive(CompilerVar var)
        {
            Contract.Requires(var != null);

            return var.NextActive != null;
        }

        internal void AddActive(CompilerVar var)
        {
            Contract.Requires(var != null);

            if (var.NextActive != null)
                throw new ArgumentException();
            if (var.PreviousActive != null)
                throw new ArgumentException();

            if (_active == null)
            {
                var.NextActive = var;
                var.PreviousActive = var;
                _active = var;
            }
            else
            {
                CompilerVar vlast = _active.PreviousActive;
                vlast.NextActive = var;
                _active.PreviousActive = var;
                var.NextActive = _active;
                var.PreviousActive = vlast;
            }
        }

        internal void FreeActive(CompilerVar var)
        {
            Contract.Requires(var != null);

            CompilerVar next = var.NextActive;
            CompilerVar prev = var.PreviousActive;

            if (prev == next)
            {
                _active = null;
            }
            else
            {
                if (_active == var)
                    _active = next;
                prev.NextActive = next;
                next.PreviousActive = prev;
            }

            var.NextActive = null;
            var.PreviousActive = null;
        }

        public void AllocVar(CompilerVar var, RegisterMask regMask, VariableAlloc variableAlloc)
        {
            Contract.Requires(var != null);

            switch (var.Type)
            {
            case VariableType.GPD:
            case VariableType.GPQ:
                if (var.Type == VariableType.GPQ && !Util.IsX64)
                    throw new NotSupportedException();

                AllocGPVar(var, regMask, variableAlloc);
                break;

            case VariableType.X87:
            case VariableType.X87_1F:
            case VariableType.X87_1D:
                // TODO: X87 VARIABLES NOT IMPLEMENTED.
                break;

            case VariableType.MM:
                AllocMMVar(var, regMask, variableAlloc);
                break;

            case VariableType.XMM:
            case VariableType.XMM_1F:
            case VariableType.XMM_4F:
            case VariableType.XMM_1D:
            case VariableType.XMM_2D:
                AllocXMMVar(var, regMask, variableAlloc);
                break;
            }

            PostAlloc(var, variableAlloc);
        }

        public void SaveVar(CompilerVar var)
        {
            Contract.Requires(var != null);

            if (var == null)
                throw new ArgumentNullException("var");
            if (var.State != VariableState.Register || var.RegisterIndex == RegIndex.Invalid)
                throw new ArgumentException("Can't save a variable that isn't allocated.");

            switch (var.Type)
            {
            case VariableType.GPD:
            case VariableType.GPQ:
                if (var.Type == VariableType.GPQ && !Util.IsX64)
                    throw new NotSupportedException();

                break;

            case VariableType.X87:
            case VariableType.X87_1F:
            case VariableType.X87_1D:
                // TODO: X87 VARIABLES NOT IMPLEMENTED.
                throw new NotImplementedException("X87 variables are not yet implemented.");

            case VariableType.MM:
                break;

            case VariableType.XMM:
            case VariableType.XMM_1F:
            case VariableType.XMM_4F:
            case VariableType.XMM_1D:
            case VariableType.XMM_2D:
                break;

            default:
                throw new ArgumentException("The variable type is not supported.");
            }

            RegIndex idx = var.RegisterIndex;
            EmitSaveVar(var, idx);

            // Update VarData.
            var.Changed = false;
        }

        public void AllocGPVar(CompilerVar var, RegisterMask regMask, VariableAlloc variableAlloc)
        {
            Contract.Requires(var != null);
            Contract.Requires(Function != null);

            RegisterMask fullMask = RegisterMask.MaskToIndex(RegNum.GP) & ~RegisterMask.FromIndex(RegIndex.Esp);
            if (!_allocableEBP)
                fullMask &= ~RegisterMask.FromIndex(RegIndex.Ebp);

              // Fix the regMask (0 or full bit-array means that any register may be used).
            if (regMask == RegisterMask.Zero)
                regMask = RegisterMask.MaskToIndex(RegNum.GP);
            regMask &= fullMask;

            int i;
            RegisterMask mask;

            // Last register code (aka home).
            RegIndex home = var.HomeRegisterIndex;
            // New register code.
            RegIndex idx = RegIndex.Invalid;

            // Preserved GP variables.
            RegisterMask preservedGP = var.Scope.Declaration.PreservedGP;

            // Spill candidate.
            CompilerVar spillCandidate = null;
            // spill caused by direct jump to L_Spill
            bool doSpill = false;

            // Whether to alloc the non-preserved variables first.
            bool nonPreservedFirst = true;
            if (Function.IsCaller)
            {
                nonPreservedFirst = var.FirstCallable == null ||
                       var.FirstCallable.Offset >= var.LastItem.Offset;
            }

            // --------------------------------------------------------------------------
            // [Already Allocated]
            // --------------------------------------------------------------------------

            // Go away if variable is already allocated.
            if (var.State == VariableState.Register)
            {
                RegIndex oldIndex = var.RegisterIndex;

                // Already allocated in the right register.
                if ((RegisterMask.FromIndex(oldIndex) & regMask) != RegisterMask.Zero)
                    return;

                // Try to find unallocated register first.
                mask = regMask & ~_state.UsedGP;
                if (mask != RegisterMask.Zero)
                {
                    idx = ((nonPreservedFirst && (mask & ~preservedGP) != RegisterMask.Zero) ? mask & ~preservedGP : mask).FirstRegister;
                }
                // Then find the allocated and later exchange.
                else
                {
                    idx = (regMask & _state.UsedGP).FirstRegister;
                }

                Contract.Assert(idx != RegIndex.Invalid);

                CompilerVar other = _state.GP[(int)idx];
                EmitExchangeVar(var, idx, variableAlloc, other);

                _state.GP[(int)oldIndex] = other;
                _state.GP[(int)idx] = var;

                if (other != null)
                    other.RegisterIndex = oldIndex;
                else
                    FreedGPRegister(oldIndex);

                // Update VarData.
                var.State = VariableState.Register;
                var.RegisterIndex = idx;
                var.HomeRegisterIndex = idx;

                AllocatedGPRegister(idx);
                return;
            }

            // --------------------------------------------------------------------------
            // [Find Unused GP]
            // --------------------------------------------------------------------------

            // If regMask contains restricted registers which may be used then everything
            // is handled in this block.
            if (regMask != fullMask)
            {
                // Try to find unallocated register first.
                mask = regMask & ~_state.UsedGP;
                if (mask != RegisterMask.Zero)
                {
                    idx = ((nonPreservedFirst && (mask & ~preservedGP) != RegisterMask.Zero) ? (mask & ~preservedGP) : mask).FirstRegister;
                    Contract.Assert(idx != RegIndex.Invalid);
                }
                // Then find the allocated and later spill.
                else
                {
                    idx = (regMask & _state.UsedGP).FirstRegister;
                    Contract.Assert(idx != RegIndex.Invalid);

                    // Spill register we need.
                    spillCandidate = _state.GP[(int)idx];

                    // Jump to spill part of allocation.
                    doSpill = true;
                    goto L_Spill;
                }
            }

            // Home register code.
            if (idx == RegIndex.Invalid && home != RegIndex.Invalid)
            {
                if ((_state.UsedGP & RegisterMask.FromIndex(home)) == RegisterMask.Zero)
                    idx = home;
            }

            // We start from 1, because EAX/RAX register is sometimes explicitly
            // needed. So we trying to prevent reallocation in near future.
            if (idx == RegIndex.Invalid)
            {
                for (i = 1, mask = RegisterMask.FromIndex((RegIndex)i); i < (int)RegNum.GP; i++, mask = RegisterMask.FromIndex((RegIndex)i))
                {
                    if ((_state.UsedGP & mask) == RegisterMask.Zero && (i != (int)RegIndex.Ebp || _allocableEBP) && (i != (int)RegIndex.Esp))
                    {
                        // Convenience to alloc non-preserved first or non-preserved last.
                        if (nonPreservedFirst)
                        {
                            if (idx != RegIndex.Invalid && (preservedGP & mask) != RegisterMask.Zero)
                                continue;
                            idx = (RegIndex)i;
                            // If current register is preserved, we should try to find different
                            // one that is not. This can save one push / pop in prolog / epilog.
                            if ((preservedGP & mask) == RegisterMask.Zero)
                                break;
                        }
                        else
                        {
                            if (idx != RegIndex.Invalid && (preservedGP & mask) == RegisterMask.Zero)
                                continue;
                            idx = (RegIndex)i;
                            // The opposite.
                            if ((preservedGP & mask) != RegisterMask.Zero)
                                break;
                        }
                    }
                }
            }

            // If not found, try EAX/RAX.
            if (idx == RegIndex.Invalid && (_state.UsedGP & RegisterMask.FromIndex(RegIndex.Eax)) == RegisterMask.Zero)
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
                    throw new CompilerException("Not enough registers.");
                }
            }

        L_Spill:

            if (idx == RegIndex.Invalid || doSpill)
            {
                // Prevented variables can't be spilled. _getSpillCandidate() never returns
                // prevented variables, but when jumping to L_Spill it can happen.
                if (spillCandidate.WorkOffset == _currentOffset)
                {
                    throw new CompilerException("Registers overlap.");
                }

                idx = spillCandidate.RegisterIndex;
                SpillGPVar(spillCandidate);
            }

            // --------------------------------------------------------------------------
            // [Alloc]
            // --------------------------------------------------------------------------

            if (var.State == VariableState.Memory && (variableAlloc & VariableAlloc.Read) != 0)
            {
                EmitLoadVar(var, idx);
            }

            // Update VarData.
            var.State = VariableState.Register;
            var.RegisterIndex = idx;
            var.HomeRegisterIndex = idx;

            // Update StateData.
            AllocatedVariable(var);
        }

        public void EmitExchangeVar(CompilerVar var, RegIndex regIndex, VariableAlloc vflags, CompilerVar other)
        {
            Contract.Requires(var != null);
            Contract.Requires(other != null);

            if (var.RegisterIndex == RegIndex.Invalid)
                throw new CompilerException("Caller must ensure that variable is allocated.");

            // If other is not valid then we can just emit MOV (or other similar instruction).
            if (other == null)
            {
                EmitMoveVar(var, regIndex, vflags);
                return;
            }

            // If we need to alloc for write-only operation then we can move other
            // variable away instead of exchanging them.
            if ((vflags & VariableAlloc.Read) == 0)
            {
                EmitMoveVar(other, var.RegisterIndex, VariableAlloc.Read);
                return;
            }

            switch (var.Type)
            {
            case VariableType.GPD:
                _compiler.Emit(InstructionCode.Xchg, Register.gpd(regIndex), Register.gpd(var.RegisterIndex));
                break;

            case VariableType.GPQ:
                if (!Util.IsX64)
                    throw new NotSupportedException();

                _compiler.Emit(InstructionCode.Xchg, Register.gpq(regIndex), Register.gpq(var.RegisterIndex));
                break;

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
                    MMReg b = Register.mm(var.RegisterIndex);

                    _compiler.Emit(InstructionCode.Pxor, a, b);
                    _compiler.Emit(InstructionCode.Pxor, b, a);
                    _compiler.Emit(InstructionCode.Pxor, a, b);
                    break;
                }

            case VariableType.XMM_1F:
            case VariableType.XMM_4F:
                {
                    XMMReg a = Register.xmm(regIndex);
                    XMMReg b = Register.xmm(var.RegisterIndex);

                    _compiler.Emit(InstructionCode.Xorps, a, b);
                    _compiler.Emit(InstructionCode.Xorps, b, a);
                    _compiler.Emit(InstructionCode.Xorps, a, b);
                    break;
                }

            case VariableType.XMM_1D:
            case VariableType.XMM_2D:
                {
                    XMMReg a = Register.xmm(regIndex);
                    XMMReg b = Register.xmm(var.RegisterIndex);

                    _compiler.Emit(InstructionCode.Xorpd, a, b);
                    _compiler.Emit(InstructionCode.Xorpd, b, a);
                    _compiler.Emit(InstructionCode.Xorpd, a, b);
                    break;
                }

            case VariableType.XMM:
                {
                    XMMReg a = Register.xmm(regIndex);
                    XMMReg b = Register.xmm(var.RegisterIndex);

                    _compiler.Emit(InstructionCode.Pxor, a, b);
                    _compiler.Emit(InstructionCode.Pxor, b, a);
                    _compiler.Emit(InstructionCode.Pxor, a, b);
                    break;
                }
            }
        }

        private CompilerVar GetSpillCandidateGP()
        {
            Contract.Requires(_compiler.CurrentItem != null);

            return GetSpillCandidateGeneric(_state.GP);
        }

        private CompilerVar GetSpillCandidateMM()
        {
            Contract.Requires(_compiler.CurrentItem != null);

            return GetSpillCandidateGeneric(_state.MM);
        }

        private CompilerVar GetSpillCandidateXMM()
        {
            Contract.Requires(_compiler.CurrentItem != null);

            return GetSpillCandidateGeneric(_state.XMM);
        }

        private CompilerVar GetSpillCandidateGeneric(IList<CompilerVar> varArray)
        {
            Contract.Requires(varArray != null);
            Contract.Requires(_compiler.CurrentItem != null);

            int i;

            CompilerVar candidate = null;
            int candidatePriority = 0;
            int candidateScore = 0;
            int count = varArray.Count;

            int currentOffset = _compiler.CurrentItem.Offset;

            for (i = 0; i < count; i++)
            {
                // Get variable.
                CompilerVar vdata = varArray[i];

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

        private static int GetSpillScore(CompilerVar var, int currentOffset)
        {
            Contract.Requires(var != null);

            int score = 0;

            if (var.LastItem == null)
                throw new ArgumentException();

            int lastOffset = var.LastItem.Offset;

            if (lastOffset >= currentOffset)
                score += lastOffset - currentOffset;

            // Each write access decreases probability of spill.
            score -= var.RegisterWriteCount + var.RegisterRWCount;
            // Each read-only access increases probability of spill.
            score += var.RegisterReadCount;

            // Each memory access increases probability of spill.
            score += var.MemoryWriteCount + var.MemoryRWCount;
            score += var.MemoryReadCount;

            return score;
        }

        public void SpillGPVar(CompilerVar var)
        {
            Contract.Requires(var != null);
            Contract.Requires(var.State == VariableState.Register);
            Contract.Requires(var.RegisterIndex != RegIndex.Invalid);

            SpillVar(var, _state.GP, FreedGPRegister);
        }

        public void AllocMMVar(CompilerVar var, RegisterMask regMask, VariableAlloc vflags)
        {
            Contract.Requires(var != null);
            Contract.Requires(Function != null);

            AllocNonGPVar(var, regMask, vflags, RegNum.MM, var.Scope.Declaration.PreservedMM, _state.UsedMM, _state.MM, AllocatedMMRegister, SpillMMVar, FreedMMRegister);
        }

        public void SpillMMVar(CompilerVar var)
        {
            Contract.Requires(var != null);
            SpillVar(var, _state.MM, FreedMMRegister);
        }

        public void AllocXMMVar(CompilerVar var, RegisterMask regMask, VariableAlloc vflags)
        {
            Contract.Requires(var != null);
            Contract.Requires(Function != null);

            AllocNonGPVar(var, regMask, vflags, RegNum.XMM, var.Scope.Declaration.PreservedXMM, _state.UsedXMM, _state.XMM, AllocatedXMMRegister, SpillXMMVar, FreedXMMRegister);
        }

        public void SpillXMMVar(CompilerVar var)
        {
            Contract.Requires(var != null);
            SpillVar(var, _state.XMM, FreedXMMRegister);
        }

        private void AllocNonGPVar(CompilerVar var, RegisterMask regMask, VariableAlloc vflags, int regNum, RegisterMask preserved, RegisterMask used, IList<CompilerVar> stateData, Action<RegIndex> allocatedAction, Action<CompilerVar> spillAction, Action<RegIndex> freeAction)
        {
            Contract.Requires(var != null);
            Contract.Requires(stateData != null);
            Contract.Requires(freeAction != null);
            Contract.Requires(Function != null);

            // Fix the regMask (0 or full bit-array means that any register may be used).
            if (regMask == RegisterMask.Zero)
                regMask = RegisterMask.MaskToIndex(regNum);

            regMask &= RegisterMask.MaskToIndex(regNum);

            // Last register code (aka home).
            RegIndex home = var.HomeRegisterIndex;
            // New register code.
            RegIndex idx = RegIndex.Invalid;

            RegisterMask mask = RegisterMask.Zero;

            // Spill candidate.
            CompilerVar spillCandidate = null;
            // spill caused by direct jump to L_Spill
            bool doSpill = false;

            // Whether to alloc non-preserved first or last.
            bool nonPreservedFirst = true;
            if (this.Function.IsCaller)
            {
                nonPreservedFirst = var.FirstCallable == null ||
                                    var.FirstCallable.Offset >= var.LastItem.Offset;
            }

            // --------------------------------------------------------------------------
            // [Already Allocated]
            // --------------------------------------------------------------------------

            // Go away if variable is already allocated.
            if (var.State == VariableState.Register)
            {
                RegIndex oldIndex = var.RegisterIndex;

                // Already allocated in the right register.
                if ((RegisterMask.FromIndex(oldIndex) & regMask) != RegisterMask.Zero)
                    return;

                // Try to find unallocated register first.
                mask = regMask & ~used;
                if (mask != RegisterMask.Zero)
                {
                    idx = ((nonPreservedFirst && (mask & ~preserved) != RegisterMask.Zero) ? mask & ~preserved : mask).FirstRegister;
                }
                // Then find the allocated and later exchange.
                else
                {
                    idx = (regMask & used).FirstRegister;
                }
                Contract.Assert(idx != RegIndex.Invalid);

                CompilerVar other = stateData[(int)idx];
                if (other != null)
                    spillAction(other);

                EmitMoveVar(var, idx, vflags);
                FreedMMRegister(oldIndex);
                stateData[(int)idx] = var;

                // Update VarData.
                var.State = VariableState.Register;
                var.RegisterIndex = idx;
                var.HomeRegisterIndex = idx;

                allocatedAction(idx);
                return;
            }

            // --------------------------------------------------------------------------
            // [Find Unused MM/XMM]
            // --------------------------------------------------------------------------

            // If regMask contains restricted registers which may be used then everything
            // is handled in this block.
            if (regMask != RegisterMask.MaskToIndex(regNum))
            {
                // Try to find unallocated register first.
                mask = regMask & ~used;
                if (mask != RegisterMask.Zero)
                {
                    idx = ((nonPreservedFirst && (mask & ~preserved) != RegisterMask.Zero) ? mask & ~preserved : mask).FirstRegister;
                    Contract.Assert(idx != RegIndex.Invalid);
                }
                // Then find the allocated and later spill.
                else
                {
                    idx = (regMask & used).FirstRegister;
                    Contract.Assert(idx != RegIndex.Invalid);

                    // Spill register we need.
                    spillCandidate = stateData[(int)idx];

                    // Jump to spill part of allocation.
                    doSpill = true;
                    goto L_Spill;
                }
            }

            // Home register code.
            if (idx == RegIndex.Invalid && home != RegIndex.Invalid)
            {
                if ((used & RegisterMask.FromIndex(home)) == RegisterMask.Zero)
                    idx = home;
            }

            if (idx == RegIndex.Invalid)
            {
                RegIndex i;
                for (i = 0, mask = RegisterMask.FromIndex((RegIndex)i); i < (RegIndex)stateData.Count; i++, mask = RegisterMask.FromIndex((RegIndex)i))
                {
                    if ((used & mask) == RegisterMask.Zero)
                    {
                        // Convenience to alloc non-preserved first or non-preserved last.
                        if (nonPreservedFirst)
                        {
                            if (idx != RegIndex.Invalid && (preserved & mask) != RegisterMask.Zero)
                                continue;
                            idx = i;
                            // If current register is preserved, we should try to find different
                            // one that is not. This can save one push / pop in prolog / epilog.
                            if ((preserved & mask) == RegisterMask.Zero)
                                break;
                        }
                        else
                        {
                            if (idx != RegIndex.Invalid && (preserved & mask) == RegisterMask.Zero)
                                continue;
                            idx = i;
                            // The opposite.
                            if ((preserved & mask) != RegisterMask.Zero)
                                break;
                        }
                    }
                }
            }

            // --------------------------------------------------------------------------
            // [Spill]
            // --------------------------------------------------------------------------

            // If register is still not found, spill other variable.
            if (idx == RegIndex.Invalid)
            {
                if (spillCandidate == null)
                    spillCandidate = GetSpillCandidateGeneric(stateData);

                // Spill candidate not found?
                if (spillCandidate == null)
                {
                    throw new CompilerException("Not enough registers.");
                }
            }

        L_Spill:

            if (idx == RegIndex.Invalid || doSpill)
            {
                // Prevented variables can't be spilled. _getSpillCandidate() never returns
                // prevented variables, but when jumping to L_spill it can happen.
                if (spillCandidate.WorkOffset == _currentOffset)
                {
                    throw new CompilerException("Registers overlap.");
                }

                idx = spillCandidate.RegisterIndex;
                SpillVar(spillCandidate, stateData, freeAction);
            }

            // --------------------------------------------------------------------------
            // [Alloc]
            // --------------------------------------------------------------------------

            if (var.State == VariableState.Memory && (vflags & VariableAlloc.Read) != 0)
            {
                EmitLoadVar(var, idx);
            }

            // Update VarData.
            var.State = VariableState.Register;
            var.RegisterIndex = idx;
            var.HomeRegisterIndex = idx;

            // Update StateData.
            AllocatedVariable(var);
        }

        private void SpillVar(CompilerVar var, IList<CompilerVar> stateData, Action<RegIndex> freeAction)
        {
            if (var == null)
                throw new ArgumentNullException("var");
            Contract.Requires(stateData != null);
            Contract.Requires(freeAction != null);
            Contract.EndContractBlock();

            if (var.State != VariableState.Register || var.RegisterIndex == RegIndex.Invalid)
                throw new ArgumentException("Can't spill a variable that isn't allocated.");

            RegIndex idx = var.RegisterIndex;

            if (var.Changed)
                EmitSaveVar(var, idx);

            // Update VarData.
            var.RegisterIndex = RegIndex.Invalid;
            var.State = VariableState.Memory;
            var.Changed = false;

            // Update StateData.
            stateData[(int)idx] = null;
            freeAction(idx);
        }

        private void FreedGPRegister(RegIndex index)
        {
            _state.UsedGP &= ~RegisterMask.FromIndex(index);
        }

        private void FreedMMRegister(RegIndex index)
        {
            _state.UsedMM &= ~RegisterMask.FromIndex(index);
        }

        private void FreedXMMRegister(RegIndex index)
        {
            _state.UsedXMM &= ~RegisterMask.FromIndex(index);
        }

        internal void MarkGPRegisterModified(RegIndex index)
        {
            _modifiedGPRegisters |= RegisterMask.FromIndex(index);
        }

        internal void MarkMMRegisterModified(RegIndex index)
        {
            _modifiedMMRegisters |= RegisterMask.FromIndex(index);
        }

        internal void MarkXMMRegisterModified(RegIndex index)
        {
            _modifiedXMMRegisters |= RegisterMask.FromIndex(index);
        }

        // TODO: Find code which uses this and improve.
        internal void NewRegisterHomeIndex(CompilerVar var, RegIndex idx)
        {
            Contract.Requires(var != null);

            if (var.HomeRegisterIndex == RegIndex.Invalid)
                var.HomeRegisterIndex = idx;

            var.PreferredRegisterMask |= RegisterMask.FromIndex(idx);
        }

        // TODO: Find code which uses this and improve.
        internal void NewRegisterHomeMask(CompilerVar var, RegisterMask mask)
        {
            Contract.Requires(var != null);

            var.PreferredRegisterMask |= mask;
        }

        public void EmitLoadVar(CompilerVar var, RegIndex regIndex)
        {
            Contract.Requires(var != null);

            Mem m = GetVarMem(var);

            switch (var.Type)
            {
            case VariableType.GPD:
                _compiler.Emit(InstructionCode.Mov, Register.gpd(regIndex), m);
                if (_emitComments)
                    goto addComment;
                break;

            case VariableType.GPQ:
                if (!Util.IsX64)
                    throw new NotSupportedException();

                _compiler.Emit(InstructionCode.Mov, Register.gpq(regIndex), m);
                if (_emitComments)
                    goto addComment;
                break;

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
            default:
                throw new CompilerException("Invalid variable type.");
            }
            return;

        addComment:
            _compiler.CurrentItem.Comment = string.Format("Alloc {0}", var.Name);
        }

        internal Mem GetVarMem(CompilerVar var)
        {
            Contract.Requires(var != null);
            Contract.Ensures(Contract.Result<Mem>() != null);

            Mem m = new Mem(var.Id);
            if (!var.IsMemArgument)
                m.Displacement = (IntPtr)_adjustESP;

            MarkMemoryUsed(var);
            return m;
        }

        private void PostAlloc(CompilerVar var, VariableAlloc variableAlloc)
        {
            Contract.Requires(var != null);

            if ((variableAlloc & VariableAlloc.Write) != 0)
                var.Changed = true;
        }

        public void SpillVar(CompilerVar var)
        {
            Contract.Requires(var != null);

            switch (var.Type)
            {
            case VariableType.GPD:
            case VariableType.GPQ:
                if (var.Type == VariableType.GPQ && !Util.IsX64)
                    throw new NotSupportedException();

                SpillGPVar(var);
                break;

            case VariableType.X87:
            case VariableType.X87_1F:
            case VariableType.X87_1D:
                // TODO: X87 VARIABLES NOT IMPLEMENTED.
                throw new NotImplementedException("X87 variables are not yet implemented.");

            case VariableType.MM:
                SpillMMVar(var);
                break;

            case VariableType.XMM:
            case VariableType.XMM_1F:
            case VariableType.XMM_4F:
            case VariableType.XMM_1D:
            case VariableType.XMM_2D:
                SpillXMMVar(var);
                break;
            default:
                throw new CompilerException("Invalid variable type.");
            }
        }

        public void EmitSaveVar(CompilerVar var, RegIndex regIndex)
        {
            Contract.Requires(var != null);

            if (var == null)
                throw new ArgumentNullException("var");
            if (regIndex == RegIndex.Invalid)
                throw new ArgumentException("Caller must ensure that variable is allocated.");

            Mem m = GetVarMem(var);

            switch (var.Type)
            {
            case VariableType.GPD:
                _compiler.Emit(InstructionCode.Mov, m, Register.gpd(regIndex));
                if (_emitComments)
                    goto addComment;
                break;

            case VariableType.GPQ:
                if (!Util.IsX64)
                    throw new NotSupportedException();

                _compiler.Emit(InstructionCode.Mov, m, Register.gpq(regIndex));
                if (_emitComments)
                    goto addComment;
                break;

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
            default:
                throw new CompilerException("Invalid variable type.");
            }
            return;

        addComment:
            _compiler.CurrentItem.Comment = string.Format("Spill {0}", var.Name);
        }

        public void EmitMoveVar(CompilerVar var, RegIndex regIndex, VariableAlloc vflags)
        {
            Contract.Requires(var != null);

            if (var.RegisterIndex == RegIndex.Invalid)
                throw new ArgumentException("Caller must ensure that variable is allocated.");

            if ((vflags & VariableAlloc.Read) == 0)
                return;

            switch (var.Type)
            {
            case VariableType.GPD:
                _compiler.Emit(InstructionCode.Mov, Register.gpd(regIndex), Register.gpd(var.RegisterIndex));
                break;

            case VariableType.GPQ:
                if (!Util.IsX64)
                    throw new NotSupportedException();

                _compiler.Emit(InstructionCode.Mov, Register.gpq(regIndex), Register.gpq(var.RegisterIndex));
                break;

            case VariableType.X87:
            case VariableType.X87_1F:
            case VariableType.X87_1D:
                // TODO: X87 VARIABLES NOT IMPLEMENTED.
                throw new NotImplementedException("X87 variables are not yet implemented.");

            case VariableType.MM:
                _compiler.Emit(InstructionCode.Movq, Register.mm(regIndex), Register.mm(var.RegisterIndex));
                break;

            case VariableType.XMM:
                _compiler.Emit(InstructionCode.Movdqa, Register.xmm(regIndex), Register.xmm(var.RegisterIndex));
                break;
            case VariableType.XMM_1F:
                _compiler.Emit(InstructionCode.Movss, Register.xmm(regIndex), Register.xmm(var.RegisterIndex));
                break;
            case VariableType.XMM_1D:
                _compiler.Emit(InstructionCode.Movsd, Register.xmm(regIndex), Register.xmm(var.RegisterIndex));
                break;
            case VariableType.XMM_4F:
                _compiler.Emit(InstructionCode.Movaps, Register.xmm(regIndex), Register.xmm(var.RegisterIndex));
                break;
            case VariableType.XMM_2D:
                _compiler.Emit(InstructionCode.Movapd, Register.xmm(regIndex), Register.xmm(var.RegisterIndex));
                break;
            default:
                throw new CompilerException("Invalid variable type.");
            }
        }

        public void UnuseVar(CompilerVar var, VariableState toState)
        {
            Contract.Requires(var != null);

            if (toState == VariableState.Register)
                throw new ArgumentException();

            if (var.State == VariableState.Register)
            {
                RegIndex registerIndex = var.RegisterIndex;
                switch (var.Type)
                {
                case VariableType.GPD:
                case VariableType.GPQ:
                    if (var.Type == VariableType.GPQ && !Util.IsX64)
                        throw new NotSupportedException();

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
                default:
                    throw new CompilerException("Invalid variable type.");
                }
            }

            var.State = toState;
            var.Changed = false;
            var.RegisterIndex = RegIndex.Invalid;
        }

        public void UnuseVarOnEndOfScope(CompilerItem e, CompilerVar vdata)
        {
            Contract.Requires(e != null);
            Contract.Requires(vdata != null);

            if (vdata.LastItem == e)
                UnuseVar(vdata, VariableState.Unused);
        }

        public void UnuseVarOnEndOfScope(CompilerItem e, VarAllocRecord rec)
        {
            Contract.Requires(e != null);
            Contract.Requires(rec != null);
            Contract.Requires(rec.VarData != null);

            CompilerVar v = rec.VarData;
            if (v.LastItem == e || (rec.VarFlags & VariableAlloc.UnuseAfterUse) != 0)
                UnuseVar(v, VariableState.Unused);
        }

        internal void UnuseVarOnEndOfScope(CompilerItem e, VarCallRecord rec)
        {
            Contract.Requires(e != null);
            Contract.Requires(rec != null);
            Contract.Requires(rec.vdata != null);

            CompilerVar v = rec.vdata;
            if (v.LastItem == e || (rec.Flags & VarCallFlags.UnuseAfterUse) != 0)
                UnuseVar(v, VariableState.Unused);
        }

        internal void AllocatedVariable(CompilerVar var)
        {
            Contract.Requires(var != null);

            RegIndex idx = var.RegisterIndex;

            switch (var.Type)
            {
            case VariableType.GPD:
            case VariableType.GPQ:
                _state.GP[(int)idx] = var;
                AllocatedGPRegister(idx);
                break;

            case VariableType.MM:
                _state.MM[(int)idx] = var;
                AllocatedMMRegister(idx);
                break;

            case VariableType.XMM:
            case VariableType.XMM_1F:
            case VariableType.XMM_4F:
            case VariableType.XMM_1D:
            case VariableType.XMM_2D:
                _state.XMM[(int)idx] = var;
                AllocatedXMMRegister(idx);
                break;

            default:
                throw new CompilerException("Invalid variable type.");
            }
        }

        internal void AllocatedGPRegister(RegIndex index)
        {
            _state.UsedGP |= RegisterMask.FromIndex(index);
            _modifiedGPRegisters |= RegisterMask.FromIndex(index);
        }

        internal void AllocatedMMRegister(RegIndex index)
        {
            _state.UsedMM |= RegisterMask.FromIndex(index);
            _modifiedMMRegisters |= RegisterMask.FromIndex(index);
        }

        internal void AllocatedXMMRegister(RegIndex index)
        {
            _state.UsedXMM |= RegisterMask.FromIndex(index);
            _modifiedXMMRegisters |= RegisterMask.FromIndex(index);
        }

        internal void MarkMemoryUsed(CompilerVar var)
        {
            Contract.Requires(var != null);

            if (var.HomeMemoryData != null)
                return;

            VarMemBlock mem = AllocMemBlock(var.Size);
            if (mem == null)
                return;

            var.HomeMemoryData = mem;
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
            Contract.Requires(operands != null);

            int i;
            int count = operands.Length;

            // Translate variables to registers.
            for (i = 0; i < count; i++)
            {
                Operand o = operands[i];

                if (o.IsVar)
                {
                    CompilerVar vdata = _compiler.GetVarData(o.Id);
                    Contract.Assert(vdata != null);

                    operands[i] = new GPReg(((BaseVar)o).RegisterType, vdata.RegisterIndex);
                }
                else if (o.IsMem)
                {
                    Mem mem = (Mem)o;

                    if ((o.Id & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        // Memory access. We just increment here actual displacement.
                        CompilerVar vdata = _compiler.GetVarData(o.Id);
                        Contract.Assert(vdata != null);

                        mem.Displacement += vdata.IsMemArgument
                          ? _argumentsActualDisp
                          : _variablesActualDisp;
                        // NOTE: This is not enough, variable position will be patched later
                        // by CompilerContext::_patchMemoryOperands().
                    }
                    else if (((int)mem.Base & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        CompilerVar vdata = _compiler.GetVarData((int)mem.Base);
                        Contract.Assert(vdata != null);

                        mem.Base = vdata.RegisterIndex;
                    }

                    if (((int)mem.Index & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        CompilerVar vdata = _compiler.GetVarData((int)mem.Index);
                        Contract.Assert(vdata != null);

                        mem.Index = vdata.RegisterIndex;
                    }
                }
            }
        }

        public void AddBackwardCode(CompilerJmpInstruction from)
        {
            if (from == null)
                throw new ArgumentNullException("from");
            Contract.EndContractBlock();

            _backCode.Add(from);
        }

        public void AddForwardJump(CompilerJmpInstruction instruction)
        {
            ForwardJumpData j = new ForwardJumpData()
            {
                Inst = instruction,
                State = SaveState(),
                Next = _forwardJumps,
            };

            _forwardJumps = j;
        }

        internal StateData SaveState()
        {
            // Get count of variables stored in memory.
            int memVarsCount = 0;
            CompilerVar cur = _active;
            if (cur != null)
            {
                do
                {
                    if (cur.State == VariableState.Memory)
                        memVarsCount++;
                    cur = cur.NextActive;
                } while (cur != _active);
            }

            // Alloc StateData structure (using zone allocator) and copy current state into it.
            StateData state = new StateData(_state, memVarsCount);

            // Clear changed flags.
            state.ChangedGP = RegisterMask.Zero;
            state.ChangedMM = RegisterMask.Zero;
            state.ChangedXMM = RegisterMask.Zero;

            // Save variables stored in REGISTERs and CHANGE flag.
            for (int i = 0; i < (int)RegNum.GP; i++)
            {
                RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                if (state.GP[i] != null && state.GP[i].Changed)
                    state.ChangedGP |= mask;
            }

            for (int i = 0; i < (int)RegNum.MM; i++)
            {
                RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                if (state.MM[i] != null && state.MM[i].Changed)
                    state.ChangedMM |= mask;
            }

            for (int i = 0; i < (int)RegNum.XMM; i++)
            {
                RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                if (state.XMM[i] != null && state.XMM[i].Changed)
                    state.ChangedXMM |= mask;
            }

            // Save variables stored in MEMORY.
            memVarsCount = 0;

            cur = _active;
            if (cur != null)
            {
                do
                {
                    if (cur.State == VariableState.Memory)
                        state.MemVarsData[memVarsCount++] = cur;
                    cur = cur.NextActive;
                } while (cur != _active);
            }

            // Finished.
            return state;
        }

        internal void AssignState(StateData state)
        {
            Contract.Requires(state != null);

            Compiler compiler = Compiler;
            _state.CopyFrom(state);
            _state.MemVarsData = new CompilerVar[0];

            int i;
            CompilerVar vdata;

            // Unuse all variables first.
            vdata = _active;
            if (vdata != null)
            {
                do
                {
                    vdata.State = VariableState.Unused;
                    vdata = vdata.NextActive;
                } while (vdata != _active);
            }

            // Assign variables stored in memory which are not unused.
            for (i = 0; i < state.MemVarsData.Length; i++)
            {
                state.MemVarsData[i].State = VariableState.Memory;
            }

            // Assign allocated variables.
            for (i = 0; i < RegNum.GP; i++)
            {
                RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                if ((vdata = _state.GP[i]) != null)
                {
                    vdata.State = VariableState.Register;
                    vdata.RegisterIndex = (RegIndex)i;
                    vdata.Changed = (_state.ChangedGP & mask) != RegisterMask.Zero;
                }
            }

            for (i = 0; i < RegNum.MM; i++)
            {
                RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                if ((vdata = _state.MM[i]) != null)
                {
                    vdata.State = VariableState.Register;
                    vdata.RegisterIndex = (RegIndex)i;
                    vdata.Changed = (_state.ChangedMM & mask) != RegisterMask.Zero;
                }
            }

            for (i = 0; i < RegNum.XMM; i++)
            {
                RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                if ((vdata = _state.XMM[i]) != null)
                {
                    vdata.State = VariableState.Register;
                    vdata.RegisterIndex = (RegIndex)i;
                    vdata.Changed = (_state.ChangedXMM & mask) != RegisterMask.Zero;
                }
            }
        }

        internal void RestoreState(StateData state, int targetOffset = Operand.InvalidValue)
        {
            // 16 + 8 + 16 = GP + MMX + XMM registers.
            const int STATE_REGS_COUNT = 16 + 8 + 16;

            StateData fromState = _state;
            StateData toState = state;

            // No change, rare...
            if (fromState == toState)
                return;

            int @base;
            int i;

            // TODO: 16 + 8 + 16 is cryptic, make constants instead!

            // --------------------------------------------------------------------------
            // Set target state to all variables. vdata->tempInt is target state in this
            // function.
            // --------------------------------------------------------------------------

            {
                // UNUSED.
                CompilerVar vdata = _active;
                if (vdata != null)
                {
                    do
                    {
                        vdata.Temp = VariableState.Unused;
                        vdata = vdata.NextActive;
                    } while (vdata != _active);
                }

                // MEMORY.
                for (i = 0; i < toState.MemVarsData.Length; i++)
                {
                    toState.MemVarsData[i].Temp = VariableState.Memory;
                }

                // REGISTER.
                for (i = 0; i < StateData.RegisterCount; i++)
                {
                    if ((vdata = toState.Registers[i]) != null)
                        vdata.Temp = VariableState.Register;
                }
            }

#if false
            // --------------------------------------------------------------------------
            // [GP-Registers Switch]
            // --------------------------------------------------------------------------

            // TODO.
            for (i = 0; i < RegNum.GP; i++)
            {
                VarData fromVar = fromState.GP[i];
                VarData toVar = toState.GP[i];

                if (fromVar != toVar)
                {
                    if (fromVar != null)
                    {
                        if (toVar != null)
                        {
                            if (fromState.GP[to
                        }
                        else
                        {
                            // It is possible that variable that was saved in state currently not
                            // exists (tempInt is target scope!).
                            if (fromVar.State == VariableState.Unused)
                            {
                                UnuseVar(fromVar, VariableState.Unused);
                            }
                            else
                            {
                                SpillVar(fromVar);
                            }
                        }
                    }
                }
                else if (fromVar != null)
                {
                    int mask = RegisterMask.FromIndex((RegIndex)i);
                    // Variables are the same, we just need to compare changed flags.
                    if ((fromState.ChangedGP & mask) != 0 && (toState.ChangedGP & mask) == 0)
                        SaveVar(fromVar);
                }
            }
#endif

            // Spill.
            for (@base = 0, i = 0; i < STATE_REGS_COUNT; i++)
            {
                // Change the base offset (from base offset the register index can be calculated).
                if (i == 16 || i == 16 + 8)
                    @base = i;

                int regIndex = i - @base;

                CompilerVar fromVar = fromState.Registers[i];
                CompilerVar toVar = toState.Registers[i];

                if (fromVar != toVar)
                {
                    // Spill the register
                    if (fromVar != null)
                    {
                        // It is possible that variable that was saved in state currently not
                        // exists.
                        if ((fromVar.Temp is VariableState) && ((VariableState)fromVar.Temp) == VariableState.Unused)
                        {
                            UnuseVar(fromVar, VariableState.Unused);
                        }
                        else
                        {
                            SpillVar(fromVar);
                        }
                    }
                }
                else if (fromVar != null)
                {
                    RegisterMask mask = RegisterMask.FromIndex((RegIndex)regIndex);
                    // Variables are the same, we just need to compare changed flags.
                    if ((fromState.ChangedGP & mask) != RegisterMask.Zero && (toState.ChangedGP & mask) == RegisterMask.Zero)
                    {
                        SaveVar(fromVar);
                    }
                }
            }

            // Alloc.
            for (@base = 0, i = 0; i < STATE_REGS_COUNT; i++)
            {
                if (i == 16 || i == 24)
                    @base = i;

                CompilerVar fromVar = fromState.Registers[i];
                CompilerVar toVar = toState.Registers[i];

                if (fromVar != toVar)
                {
                    int regIndex = i - @base;

                    // Alloc register
                    if (toVar != null)
                    {
                        AllocVar(toVar, RegisterMask.FromIndex((RegIndex)regIndex), VariableAlloc.Read);
                    }
                }

                // TODO:
                //if (toVar)
                //{
                // toVar->changed = to->changed;
                //}
            }

            // --------------------------------------------------------------------------
            // Update used masks.
            // --------------------------------------------------------------------------

            _state.UsedGP = state.UsedGP;
            _state.UsedMM = state.UsedMM;
            _state.UsedXMM = state.UsedXMM;

            // --------------------------------------------------------------------------
            // Update changed masks and cleanup.
            // --------------------------------------------------------------------------

            {
                CompilerVar vdata = _active;
                if (vdata != null)
                {
                    do
                    {
                        if (!((vdata.Temp is VariableState) && ((VariableState)vdata.Temp) == VariableState.Register))
                        {
                            vdata.State = (VariableState)vdata.Temp;
                            vdata.Changed = false;
                        }

                        vdata.Temp = null;
                        vdata = vdata.NextActive;
                    } while (vdata != _active);
                }
            }
        }
    }
}
