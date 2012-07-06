namespace NAsmJit
{
    using System;
    using System.Diagnostics.Contracts;
    using System.Linq;
    using System.Text;

    public class CompilerFunction : CompilerItem
    {
        private readonly FunctionDeclaration _functionPrototype;
        internal VarData[] _argumentVariables;

        private FunctionHints _hints;

        private readonly bool _isStackAlignedByOsTo16Bytes = CompilerUtil.IsStack16ByteAligned;

        private bool _isStackAlignedByFnTo16Bytes;

        private bool _isNaked;

        private bool _isEspAdjusted;

        private bool _isCaller;

        private bool _pePushPop = true;

        private bool _emitEMMS;

        private bool _emitSFence;

        private bool _emitLFence;

        private bool _finished;

        private RegisterMask _modifiedAndPreservedGP;

        private RegisterMask _modifiedAndPreservedMM;

        private RegisterMask _modifiedAndPreservedXMM;

        private InstructionCode _movDqInstruction;

        private int _pePushPopStackSize;
        private int _peMovStackSize;
        private int _peAdjustStackSize;

        private int _memStackSize;
        private int _memStackSize16;

        private int _functionCallStackSize;

        /// <summary>Function entry label.</summary>
        private readonly Label _entryLabel;
        /// <summary>Function exit label.</summary>
        private readonly Label _exitLabel;
        /// <summary>Function entry target.</summary>
        private readonly CompilerTarget _entryTarget;
        /// <summary>Function exit target.</summary>
        private readonly CompilerTarget _exitTarget;
        /// <summary>Function end item.</summary>
        private readonly CompilerFunctionEnd _end;

        private CompilerFunction(Compiler compiler, FunctionDeclaration prototype)
            : base(compiler)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(prototype != null);

            _functionPrototype = prototype;

            _entryLabel = compiler.DefineLabel();
            _exitLabel = compiler.DefineLabel();
            _entryTarget = compiler.GetTarget(_entryLabel.Id);
            _exitTarget = compiler.GetTarget(_exitLabel.Id);

            _end = new CompilerFunctionEnd(compiler, this);
        }

        public CompilerFunction(Compiler compiler, CallingConvention callingConvention, Type delegateType)
            : this(compiler, new FunctionDeclaration(callingConvention, delegateType))
        {
            Contract.Requires(compiler != null);
        }

        public CompilerFunction(Compiler compiler, CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
            : this(compiler, new FunctionDeclaration(callingConvention, arguments, returnValue))
        {
            Contract.Requires(compiler != null);
            Contract.Requires(arguments != null);
        }

        public override ItemType ItemType
        {
            get
            {
                return ItemType.Function;
            }
        }

        public override int MaxSize
        {
            get
            {
                // Function is NOP
                return 0;
            }
        }

        public FunctionDeclaration Declaration
        {
            get
            {
                Contract.Ensures(Contract.Result<FunctionDeclaration>() != null);

                return _functionPrototype;
            }
        }

        public Label EntryLabel
        {
            get
            {
                Contract.Ensures(Contract.Result<Label>() != null);

                return _entryLabel;
            }
        }

        public Label ExitLabel
        {
            get
            {
                Contract.Ensures(Contract.Result<Label>() != null);

                return _exitLabel;
            }
        }

        /// <summary>Get function entry target.</summary>
        public CompilerTarget EntryTarget
        {
            get
            {
                return _entryTarget;
            }
        }

        /// <summary>Get function exit target.</summary>
        public CompilerTarget ExitTarget
        {
            get
            {
                return _exitTarget;
            }
        }

        /// <summary>
        /// Get whether the stack is aligned to 16 bytes by OS.
        /// </summary>
        public bool IsStackAlignedByOsTo16Bytes
        {
            get
            {
                return _isStackAlignedByOsTo16Bytes;
            }
        }

        /// <summary>
        /// Get whether the stack is aligned to 16 bytes by function itself.
        /// </summary>
        public bool IsStackAlignedByFnTo16Bytes
        {
            get
            {
                return _isStackAlignedByFnTo16Bytes;
            }
        }

        public bool IsEspAdjusted
        {
            get
            {
                return _isEspAdjusted;
            }

            set
            {
                _isEspAdjusted = value;
            }
        }

        public CompilerFunctionEnd End
        {
            get
            {
                Contract.Ensures(Contract.Result<CompilerFunctionEnd>() != null);

                return _end;
            }
        }

        public bool IsCaller
        {
            get
            {
                return _isCaller;
            }
        }

        internal bool Finished
        {
            get
            {
                return _finished;
            }

            set
            {
                _finished = value;
            }
        }

        public bool IsNaked
        {
            get
            {
                return _isNaked;
            }
        }

        public int FunctionCallStackSize
        {
            get
            {
                return _functionCallStackSize;
            }
        }

        private int RequiredStackOffset
        {
            get
            {
                return _functionCallStackSize + _memStackSize16 + _peMovStackSize + _peAdjustStackSize;
            }
        }

        [ContractInvariantMethod]
        private void ObjectInvariants()
        {
            Contract.Invariant(_functionPrototype != null);
            Contract.Invariant(_entryLabel != null);
            Contract.Invariant(_exitLabel != null);
            Contract.Invariant(_end != null);
        }

        protected override void PrepareImpl(CompilerContext cc)
        {
            Offset = cc.CurrentOffset++;
        }

        public void SetHint(FunctionHints hint, bool value)
        {
            if (value)
                _hints |= hint;
            else
                _hints &= ~hint;
        }

        public void SetHints(FunctionHints hints)
        {
            _hints |= hints;
        }

        public void ClearHints(FunctionHints hints)
        {
            _hints &= ~hints;
        }

        internal void CreateVariables()
        {
            int count = _functionPrototype.Arguments.Length;
            if (count == 0)
                return;

            _argumentVariables = new VarData[count];

            bool debug = Compiler.Logger != null;
            string argName = null;
            for (int i = 0; i < count; i++)
            {
                FunctionDeclaration.Argument a = _functionPrototype.Arguments[i];
                if (debug)
                    argName = "arg_" + i;

                int size = VariableInfo.GetVariableSize(a._variableType);
                VarData varData = Compiler.NewVarData(argName, a._variableType, size);

                if (a._registerIndex != RegIndex.Invalid)
                {
                    varData.IsRegArgument = true;
                    varData.RegisterIndex = a._registerIndex;
                }

                if (a._stackOffset != InvalidValue)
                {
                    varData.IsMemArgument = true;
                    varData.HomeMemoryOffset = a._stackOffset;
                }

                _argumentVariables[i] = varData;
            }
        }

        internal void PreparePrologEpilog(CompilerContext cc)
        {
            Contract.Requires(cc != null);

            _pePushPop = false;
            _emitEMMS = false;
            _emitSFence = false;
            _emitLFence = false;
            _isStackAlignedByFnTo16Bytes = false;

            uint accessibleMemoryBelowStack = 0;
            if (_functionPrototype.CallingConvention == CallingConvention.X64U)
                accessibleMemoryBelowStack = 128;

            if (_isCaller && (cc.MemBytesTotal > 0 || _isStackAlignedByOsTo16Bytes))
                _isEspAdjusted = true;

            if (cc.MemBytesTotal > accessibleMemoryBelowStack)
                _isEspAdjusted = true;

            _isNaked = (_hints & FunctionHints.Naked) != 0;
            _pePushPop = (_hints & FunctionHints.PushPopSequence) != 0;
            _emitEMMS = (_hints & FunctionHints.Emms) != 0;
            _emitSFence = (_hints & FunctionHints.StoreFence) != 0;
            _emitLFence = (_hints & FunctionHints.LoadFence) != 0;

            // Updated to respect comment from issue #47, align also when using MMX code.
            if (!_isStackAlignedByOsTo16Bytes && !_isNaked && (cc.Mem16BlocksCount + cc.Mem8BlocksCount > 0))
            {
                // Have to align stack to 16-bytes.
                _isStackAlignedByFnTo16Bytes = true;
                _isEspAdjusted = true;
            }

            _modifiedAndPreservedGP = cc.ModifiedGPRegisters & _functionPrototype.PreservedGP & ~RegisterMask.FromIndex(RegIndex.Esp);
            _modifiedAndPreservedMM = cc.ModifiedMMRegisters & _functionPrototype.PreservedMM;
            _modifiedAndPreservedXMM = cc.ModifiedXMMRegisters & _functionPrototype.PreservedXMM;

            _movDqInstruction = (IsStackAlignedByOsTo16Bytes || IsStackAlignedByFnTo16Bytes) ? InstructionCode.Movdqa : InstructionCode.Movdqu;

            // Prolog & Epilog stack size.
            {
                int memGpSize = _modifiedAndPreservedGP.RegisterCount * IntPtr.Size;
                int memMmSize = _modifiedAndPreservedMM.RegisterCount * 8;
                int memXmmSize = _modifiedAndPreservedXMM.RegisterCount * 16;

                if (_pePushPop)
                {
                    _pePushPopStackSize = memGpSize;
                    _peMovStackSize = memXmmSize + Util.AlignTo16(memMmSize);
                }
                else
                {
                    _pePushPopStackSize = 0;
                    _peMovStackSize = memXmmSize + Util.AlignTo16(memMmSize + memGpSize);
                }
            }

            if (IsStackAlignedByFnTo16Bytes)
            {
                _peAdjustStackSize += Util.DeltaTo16(_pePushPopStackSize);
            }
            else
            {
                int v = 16 - IntPtr.Size;
                if (!_isNaked)
                    v -= IntPtr.Size;

                v -= _pePushPopStackSize & 15;
                if (v < 0)
                    v += 16;
                _peAdjustStackSize = v;

                //_peAdjustStackSize += deltaTo16(_pePushPopStackSize + v);
            }

            // Memory stack size.
            _memStackSize = cc.MemBytesTotal;
            _memStackSize16 = Util.AlignTo16(_memStackSize);

            if (_isNaked)
            {
                cc.ArgumentsBaseReg = RegIndex.Esp;
                cc.ArgumentsBaseOffset = (_isEspAdjusted)
                  ? (_functionCallStackSize + _memStackSize16 + _peMovStackSize + _pePushPopStackSize + _peAdjustStackSize)
                  : (_pePushPopStackSize);
            }
            else
            {
                cc.ArgumentsBaseReg = RegIndex.Ebp;
                cc.ArgumentsBaseOffset = IntPtr.Size;
            }

            cc.VariablesBaseReg = RegIndex.Esp;
            cc.VariablesBaseOffset = _functionCallStackSize;
            if (!_isEspAdjusted)
                cc.VariablesBaseOffset = -_memStackSize16 - _peMovStackSize - _peAdjustStackSize;
        }

        internal void EmitProlog(CompilerContext cc)
        {
            RegisterMask preservedGP = _modifiedAndPreservedGP;
            RegisterMask preservedMM = _modifiedAndPreservedMM;
            RegisterMask preservedXMM = _modifiedAndPreservedXMM;

            int stackOffset = RequiredStackOffset;
            int nspPos;

            // --------------------------------------------------------------------------
            // [Prolog]
            // --------------------------------------------------------------------------

            if (Compiler.Logger != null)
                Compiler.Comment("Prolog");

            // Emit standard prolog entry code (but don't do it if function is set to be
            // naked).
            //
            // Also see the _prologEpilogStackAdjust variable. If function is naked (so
            // prolog and epilog will not contain "push ebp" and "mov ebp, esp", we need
            // to adjust stack by 8 bytes in 64-bit mode (this will give us that stack
            // will remain aligned to 16 bytes).
            if (!_isNaked)
            {
                Compiler.Emit(InstructionCode.Push, Register.nbp);
                Compiler.Emit(InstructionCode.Mov, Register.nbp, Register.nsp);
            }

            // Align manually stack-pointer to 16-bytes.
            if (_isStackAlignedByFnTo16Bytes)
            {
                if (_isNaked)
                    throw new CompilerException();

                Compiler.Emit(InstructionCode.And, Register.nsp, (Imm)(-16));
            }

            // --------------------------------------------------------------------------
            // [Save Gp - Push/Pop]
            // --------------------------------------------------------------------------

            if (preservedGP != RegisterMask.Zero && _pePushPop)
            {
                for (int i = 0; i < RegNum.GP; i++)
                {
                    RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                    if ((preservedGP & mask) != RegisterMask.Zero)
                        Compiler.Emit(InstructionCode.Push, Register.gpn((RegIndex)i));
                }
            }

            // --------------------------------------------------------------------------
            // [Adjust Scack]
            // --------------------------------------------------------------------------

            if (_isEspAdjusted)
            {
                nspPos = _memStackSize16 + _functionCallStackSize;
                if (stackOffset != 0)
                    Compiler.Emit(InstructionCode.Sub, Register.nsp, (Imm)stackOffset);
            }
            else
            {
                nspPos = -(_peMovStackSize + _peAdjustStackSize);
                //if (_pePushPop) nspPos += bitCount(preservedGP) * sizeof(sysint_t);
            }

            // --------------------------------------------------------------------------
            // [Save Xmm - MovDqa/MovDqu]
            // --------------------------------------------------------------------------

            if (preservedXMM != RegisterMask.Zero)
            {
                for (int i = 0; i < RegNum.XMM; i++)
                {
                    RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                    if ((preservedXMM & mask) != RegisterMask.Zero)
                    {
                        Compiler.Emit(_movDqInstruction, Mem.dqword_ptr(Register.nsp, nspPos), Register.xmm((RegIndex)i));
                        nspPos += 16;
                    }
                }
            }

            // --------------------------------------------------------------------------
            // [Save Mm - MovQ]
            // --------------------------------------------------------------------------

            if (preservedMM != RegisterMask.Zero)
            {
                for (int i = 0; i < 8; i++)
                {
                    RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                    if ((preservedMM & mask) != RegisterMask.Zero)
                    {
                        Compiler.Emit(InstructionCode.Movq, Mem.qword_ptr(Register.nsp, nspPos), Register.mm((RegIndex)i));
                        nspPos += 8;
                    }
                }
            }

            // --------------------------------------------------------------------------
            // [Save Gp - Mov]
            // --------------------------------------------------------------------------

            if (preservedGP != RegisterMask.Zero && !_pePushPop)
            {
                for (int i = 0; i < RegNum.GP; i++)
                {
                    RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                    if ((preservedGP & mask) != RegisterMask.Zero)
                    {
                        Compiler.Emit(InstructionCode.Mov, Mem.sysint_ptr(Register.nsp, nspPos), Register.gpn((RegIndex)i));
                        nspPos += IntPtr.Size;
                    }
                }
            }

            // --------------------------------------------------------------------------
            // [...]
            // --------------------------------------------------------------------------

            if (Compiler.Logger != null)
                Compiler.Comment("Body");
        }

        internal void EmitEpilog(CompilerContext cc)
        {
            // --------------------------------------------------------------------------
            // [Init]
            // --------------------------------------------------------------------------

            RegisterMask preservedGP = _modifiedAndPreservedGP;
            RegisterMask preservedMM = _modifiedAndPreservedMM;
            RegisterMask preservedXMM = _modifiedAndPreservedXMM;

            int stackOffset = RequiredStackOffset;
            int nspPos = _isEspAdjusted
              ? (_memStackSize16 + _functionCallStackSize)
              : -(_peMovStackSize + _peAdjustStackSize);

            // --------------------------------------------------------------------------
            // [Epilog]
            // --------------------------------------------------------------------------

            if (Compiler.Logger != null)
                Compiler.Comment("Epilog");

            // --------------------------------------------------------------------------
            // [Restore Xmm - MovDqa/ModDqu]
            // --------------------------------------------------------------------------

            if (preservedXMM != RegisterMask.Zero)
            {
                for (int i = 0; i < RegNum.XMM; i++)
                {
                    RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                    if ((preservedXMM & mask) != RegisterMask.Zero)
                    {
                        Compiler.Emit(_movDqInstruction, Register.xmm((RegIndex)i), Mem.dqword_ptr(Register.nsp, nspPos));
                        nspPos += 16;
                    }
                }
            }

            // --------------------------------------------------------------------------
            // [Restore Mm - MovQ]
            // --------------------------------------------------------------------------

            if (preservedMM != RegisterMask.Zero)
            {
                for (int i = 0; i < 8; i++)
                {
                    RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                    if ((preservedMM & mask) != RegisterMask.Zero)
                    {
                        Compiler.Emit(InstructionCode.Movq, Register.mm((RegIndex)i), Mem.qword_ptr(Register.nsp, nspPos));
                        nspPos += 8;
                    }
                }
            }

            // --------------------------------------------------------------------------
            // [Restore Gp - Mov]
            // --------------------------------------------------------------------------

            if (preservedGP != RegisterMask.Zero && !_pePushPop)
            {
                for (int i = 0; i < (int)RegNum.GP; i++)
                {
                    RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                    if ((preservedGP & mask) != RegisterMask.Zero)
                    {
                        Compiler.Emit(InstructionCode.Mov, Register.gpn((RegIndex)i), Mem.sysint_ptr(Register.nsp, nspPos));
                        nspPos += IntPtr.Size;
                    }
                }
            }

            // --------------------------------------------------------------------------
            // [Adjust Stack]
            // --------------------------------------------------------------------------

            if (_isEspAdjusted && stackOffset != 0)
                Compiler.Emit(InstructionCode.Add, Register.nsp, (Imm)stackOffset);

            // --------------------------------------------------------------------------
            // [Restore Gp - Push/Pop]
            // --------------------------------------------------------------------------

            if (preservedGP != RegisterMask.Zero && _pePushPop)
            {
                for (int i = RegNum.GP - 1; i >= 0; i--)
                {
                    RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                    if ((preservedGP & mask) != RegisterMask.Zero)
                    {
                        Compiler.Emit(InstructionCode.Pop, Register.gpn((RegIndex)i));
                    }
                }
            }

            // --------------------------------------------------------------------------
            // [Emms]
            // --------------------------------------------------------------------------

            if (_emitEMMS)
                Compiler.Emit(InstructionCode.Emms);

            // --------------------------------------------------------------------------
            // [MFence/SFence/LFence]
            // --------------------------------------------------------------------------

            if (_emitSFence && _emitLFence)
                Compiler.Emit(InstructionCode.Mfence); // MFence == SFence & LFence.
            else if (_emitSFence)
                Compiler.Emit(InstructionCode.Sfence); // Only SFence.
            else if (_emitLFence)
                Compiler.Emit(InstructionCode.Lfence); // Only LFence.

            // --------------------------------------------------------------------------
            // [Epilog]
            // --------------------------------------------------------------------------

            if (!_isNaked)
            {
                CpuInfo cpuInfo = CpuInfo.Instance;

                // AMD seems to prefer LEAVE instead of MOV/POP sequence.
                if (cpuInfo.VendorId == CpuVendor.Amd)
                {
                    Compiler.Emit(InstructionCode.Leave);
                }
                else
                {
                    Compiler.Emit(InstructionCode.Mov, Register.nsp, Register.nbp);
                    Compiler.Emit(InstructionCode.Pop, Register.nbp);
                }
            }

            // Emit return.
            if (_functionPrototype.CalleePopsStack)
            {
                Compiler.Emit(InstructionCode.Ret, (Imm)((short)_functionPrototype.ArgumentsStackSize));
            }
            else
            {
                Compiler.Emit(InstructionCode.Ret);
            }
        }

        internal void ReserveStackForFunctionCall(int size)
        {
            size = Util.AlignTo16(size);

            if (size > _functionCallStackSize)
                _functionCallStackSize = size;
            _isCaller = true;
        }

        private static readonly int _variableTypeCount = Enum.GetValues(typeof(VariableType)).Cast<int>().Max() + 1;

        internal void DumpFunction(CompilerContext cc)
        {
            Logger logger = Compiler.Logger;
            if (logger == null)
                throw new InvalidOperationException("Cannot dump a function without a logger.");

            int i;
            StringBuilder buffer = new StringBuilder();

            // Log function prototype.
            {
                int argumentsCount = _functionPrototype.Arguments.Length;
                bool first = true;

                logger.LogString("; Function Prototype:" + Environment.NewLine);
                logger.LogString(";" + Environment.NewLine);

                for (i = 0; i < argumentsCount; i++)
                {
                    FunctionDeclaration.Argument a = _functionPrototype.Arguments[i];
                    VarData vdata = _argumentVariables[i];

                    if (first)
                    {
                        logger.LogString("; IDX| Type     | Sz | Home           |" + Environment.NewLine);
                        logger.LogString("; ---+----------+----+----------------+" + Environment.NewLine);
                    }

                    buffer.Clear();

                    if (a._registerIndex != RegIndex.Invalid)
                    {
                        var regOp = new GPReg(Register.NativeRegisterType, a._registerIndex);
                        Assembler.DumpOperand(buffer, regOp, Register.NativeRegisterType);
                    }
                    else
                    {
                        Mem memOp = new Mem();
                        memOp.Base = RegIndex.Esp;
                        memOp.Displacement = (IntPtr)a._stackOffset;
                        Assembler.DumpOperand(buffer, memOp, Register.NativeRegisterType);
                    }

                    // original format string: "; %-3u| %-9s| %-3u| %-15s|\n"
                    logger.LogFormat("; {0,-3}| {1,-9}| {2,-3}| {3,-15}|" + Environment.NewLine,
                        // Argument index.
                        i,
                        // Argument type.
                        (int)vdata.Type < _variableTypeCount ? VariableInfo.GetVariableInfo(vdata.Type).Name : "invalid",
                        // Argument size.
                        vdata.Size,
                        // Argument memory home.
                        buffer
                        );

                    first = false;
                }
                logger.LogString(";" + Environment.NewLine);
            }

            // Log variables.
            {
                int variablesCount = Compiler.Variables.Count;
                bool first = true;

                logger.LogString("; Variables:" + Environment.NewLine);
                logger.LogString(";" + Environment.NewLine);

                for (i = 0; i < variablesCount; i++)
                {
                    VarData vdata = Compiler.GetVarData(i);
                    Contract.Assert(vdata != null);

                    // If this variable is not related to this function then skip it.
                    if (vdata.Scope != this)
                        continue;

                    // Get some information about variable type.
                    VariableInfo vinfo = VariableInfo.GetVariableInfo(vdata.Type);

                    if (first)
                    {
                        logger.LogString("; ID | Type     | Sz | Home           | Register Access   | Memory Access     |" + Environment.NewLine);
                        logger.LogString("; ---+----------+----+----------------+-------------------+-------------------+" + Environment.NewLine);
                    }

                    buffer.Clear();
                    buffer.Append("[None]");

                    if (vdata.HomeMemoryData != null)
                    {
                        buffer.Clear();

                        Mem memOp = new Mem();
                        if (vdata.IsMemArgument)
                        {
                            FunctionDeclaration.Argument a = _functionPrototype.Arguments[i];

                            memOp.Base = cc.ArgumentsBaseReg;
                            memOp.Displacement += cc.ArgumentsBaseOffset;
                            memOp.Displacement += a._stackOffset;
                        }
                        else
                        {
                            VarMemBlock memBlock = vdata.HomeMemoryData;
                            memOp.Base = cc.VariablesBaseReg;
                            memOp.Displacement += cc.VariablesBaseOffset;
                            memOp.Displacement += memBlock.Offset;
                        }

                        Assembler.DumpOperand(buffer, memOp, Register.NativeRegisterType);
                    }

                    string registerAccess = string.Format("r={0}w={1}x={2}", vdata.RegisterReadCount, vdata.RegisterWriteCount, vdata.RegisterRWCount);
                    string memoryAccess = string.Format("r={0}w={1}x={2}", vdata.MemoryReadCount, vdata.MemoryWriteCount, vdata.MemoryRWCount);
                    logger.LogFormat("; {0,-3}| {1,-9}| {2,-3}| {3,-15}| {4,-18}| {5,-18}|" + Environment.NewLine,
                        // Variable id.
                        (uint)(i & Operand.OperandIdValueMask),
                        // Variable type.
                        (int)vdata.Type < _variableTypeCount ? vinfo.Name : "invalid",
                        // Variable size.
                        vdata.Size,
                        // Variable memory home.
                        buffer,
                        // Register access count.
                        registerAccess,
                        // Memory access count.
                        memoryAccess
                        );

                    first = false;
                }
                logger.LogString(";" + Environment.NewLine);
            }

            // Log modified registers.
            {
                buffer.Clear();

                int r;
                int modifiedRegisters = 0;

                for (r = 0; r < 3; r++)
                {
                    bool first = true;
                    RegisterMask regs;
                    RegType type;

                    switch (r)
                    {
                    case 0:
                        regs = cc.ModifiedGPRegisters;
                        type = Register.NativeRegisterType;
                        buffer.Append("; GP : ");
                        break;
                    case 1:
                        regs = cc.ModifiedMMRegisters;
                        type = RegType.MM;
                        buffer.Append("; MM : ");
                        break;
                    case 2:
                        regs = cc.ModifiedXMMRegisters;
                        type = RegType.XMM;
                        buffer.Append("; XMM: ");
                        break;
                    default:
                        Contract.Assert(false, "");
                        continue;
                    }

                    for (i = 0; i < RegNum.Base; i++)
                    {
                        if ((regs & RegisterMask.FromIndex((RegIndex)i)) != RegisterMask.Zero)
                        {
                            if (!first)
                            {
                                buffer.Append(',');
                                buffer.Append(' ');
                            }

                            DumpRegister(buffer, type, i);
                            first = false;
                            modifiedRegisters++;
                        }
                    }

                    buffer.AppendLine();
                }

                logger.LogFormat("; Modified registers ({0}):" + Environment.NewLine, modifiedRegisters);
                logger.LogString(buffer.ToString());
            }

            logger.LogString(Environment.NewLine);
        }

        private static readonly string[] _reg8l = { "al", "cl", "dl", "bl", "spl", "bpl", "sil", "dil" };
        private static readonly string[] _reg8h = { "ah", "ch", "dh", "bh", "NE", "NE", "NE", "NE" };
        private static readonly string[] _reg16 = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };

        private static void DumpRegister(StringBuilder buffer, RegType type, int index)
        {
            Contract.Requires(buffer != null);

            switch (type)
            {
            case RegType.GPB_LO:
                if (index < 8)
                    buffer.AppendFormat("{0}", _reg8l[index]);
                else
                    buffer.AppendFormat("r{0}b", index);

                return;

            case RegType.GPB_HI:
                if (index < 8)
                    buffer.AppendFormat("{0}", _reg8h[index]);
                else
                    buffer.AppendFormat("r{0}b", index);

                return;

            case RegType.GPW:
                if (index < 8)
                    buffer.AppendFormat("{0}", _reg16[index]);
                else
                    buffer.AppendFormat("r{0}w", index);

                return;

            case RegType.GPD:
                if (index < 8)
                    buffer.AppendFormat("e{0}", _reg16[index]);
                else
                    buffer.AppendFormat("r{0}d", index);

                return;

            case RegType.GPQ:
                if (index < 8)
                    buffer.AppendFormat("r{0}", _reg16[index]);
                else
                    buffer.AppendFormat("r{0}", index);

                return;

            case RegType.X87:
                buffer.AppendFormat("st{0}", index);
                return;

            case RegType.MM:
                buffer.AppendFormat("mm{0}", index);
                return;

            case RegType.XMM:
                buffer.AppendFormat("xmm{0}", index);
                return;

            default:
                return;
            }
        }

        internal void PrepareVariables(CompilerItem first)
        {
            int i;
            int count = _functionPrototype.Arguments.Length;

            for (i = 0; i < count; i++)
            {
                VarData vdata = _argumentVariables[i];

                // This is where variable scope starts.
                vdata.FirstItem = first;
                // If this will not be changed then it will be deallocated immediately.
                vdata.LastItem = first;
            }
        }

        internal void AllocVariables(CompilerContext cc)
        {
            int i;
            int count = _functionPrototype.Arguments.Length;
            if (count == 0)
                return;

            for (i = 0; i < count; i++)
            {
                VarData vdata = _argumentVariables[i];

                if (vdata.FirstItem != null ||
                    vdata.IsRegArgument ||
                    vdata.IsMemArgument)
                {
                    // Variable is used.
                    if (vdata.RegisterIndex != RegIndex.Invalid)
                    {
                        vdata.State = VariableState.Register;
                        // If variable is in register -> mark it as changed so it will not be
                        // lost by first spill.
                        vdata.Changed = true;
                        cc.AllocatedVariable(vdata);
                    }
                    else if (vdata.IsMemArgument)
                    {
                        vdata.State = VariableState.Memory;
                    }
                }
                else
                {
                    // Variable is not used.
                    vdata.RegisterIndex = RegIndex.Invalid;
                }
            }
        }
    }
}
