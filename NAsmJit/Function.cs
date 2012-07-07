namespace NAsmJit
{
    using System;
    using System.Diagnostics.Contracts;
    using System.Linq;
    using System.Text;

    public class Function : Emittable
    {
        private readonly FunctionPrototype _functionPrototype;
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

        private InstructionCode _movDqaInstruction;

        private int _pePushPopStackSize;
        private int _peMovStackSize;
        private int _peAdjustStackSize;

        private int _memStackSize;
        private int _memStackSize16;

        private int _functionCallStackSize;

        private readonly Label _entryLabel;
        private readonly Label _exitLabel;
        private readonly Prolog _prolog;
        private readonly Epilog _epilog;
        private readonly FunctionEnd _end;

        private Function(Compiler compiler, FunctionPrototype prototype)
            : base(compiler)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(prototype != null);

            _functionPrototype = prototype;

            _entryLabel = compiler.DefineLabel();
            _exitLabel = compiler.DefineLabel();

            _prolog = new Prolog(compiler, this);
            _epilog = new Epilog(compiler, this);
            _end = new FunctionEnd(compiler);
        }

        public Function(Compiler compiler, CallingConvention callingConvention, Type delegateType)
            : this(compiler, new FunctionPrototype(callingConvention, delegateType))
        {
            Contract.Requires(compiler != null);
        }

        public Function(Compiler compiler, CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
            : this(compiler, new FunctionPrototype(callingConvention, arguments, returnValue))
        {
            Contract.Requires(compiler != null);
            Contract.Requires(arguments != null);
        }

        public override EmittableType EmittableType
        {
            get
            {
                return EmittableType.Function;
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

        public FunctionPrototype Prototype
        {
            get
            {
                Contract.Ensures(Contract.Result<FunctionPrototype>() != null);

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

        public Prolog Prolog
        {
            get
            {
                Contract.Ensures(Contract.Result<Prolog>() != null);

                return _prolog;
            }
        }

        public Epilog Epilog
        {
            get
            {
                Contract.Ensures(Contract.Result<Epilog>() != null);

                return _epilog;
            }
        }

        public FunctionEnd End
        {
            get
            {
                Contract.Ensures(Contract.Result<FunctionEnd>() != null);

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

        [ContractInvariantMethod]
        private void ObjectInvariants()
        {
            Contract.Invariant(_functionPrototype != null);
            Contract.Invariant(_entryLabel != null);
            Contract.Invariant(_exitLabel != null);
            Contract.Invariant(_prolog != null);
            Contract.Invariant(_epilog != null);
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
                FunctionPrototype.Argument a = _functionPrototype.Arguments[i];
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

            _pePushPop = true;
            _emitEMMS = false;
            _emitSFence = false;
            _emitLFence = false;

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

            _movDqaInstruction = (_isStackAlignedByOsTo16Bytes || !_isNaked) ? InstructionCode.Movdqa : InstructionCode.Movdqu;

            // Prolog & Epilog stack size.
            {
                int memGP = _modifiedAndPreservedGP.RegisterCount * IntPtr.Size;
                int memMM = _modifiedAndPreservedMM.RegisterCount * 8;
                int memXMM = _modifiedAndPreservedXMM.RegisterCount * 16;

                if (_pePushPop)
                {
                    _pePushPopStackSize = memGP;
                    _peMovStackSize = memXMM + Util.AlignTo16(memMM);
                }
                else
                {
                    _pePushPopStackSize = 0;
                    _peMovStackSize = memXMM + Util.AlignTo16(memMM + memGP);
                }
            }

            if (_isStackAlignedByFnTo16Bytes)
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

            int stackSubtract = _functionCallStackSize + _memStackSize16 + _peMovStackSize + _peAdjustStackSize;
            int nspPos;

            if (Compiler.Logger != null)
            {
                // Here function prolog starts.
                Compiler.Comment("Prolog");
            }

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

            // Save GP registers using PUSH/POP.
            if (preservedGP != RegisterMask.Zero && _pePushPop)
            {
                for (int i = 0; i < RegNum.GP; i++)
                {
                    RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                    if ((preservedGP & mask) != RegisterMask.Zero)
                        Compiler.Emit(InstructionCode.Push, Register.gpn((RegIndex)i));
                }
            }

            if (_isEspAdjusted)
            {
                nspPos = _memStackSize16 + _functionCallStackSize;
                if (stackSubtract != 0)
                    Compiler.Emit(InstructionCode.Sub, Register.nsp, (Imm)(stackSubtract));
            }
            else
            {
                nspPos = -(_peMovStackSize + _peAdjustStackSize);
                //if (_pePushPop) nspPos += bitCount(preservedGP) * sizeof(sysint_t);
            }

            // Save XMM registers using MOVDQA/MOVDQU.
            if (preservedXMM != RegisterMask.Zero)
            {
                for (int i = 0; i < RegNum.XMM; i++)
                {
                    RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                    if ((preservedXMM & mask) != RegisterMask.Zero)
                    {
                        Compiler.Emit(_movDqaInstruction, Mem.dqword_ptr(Register.nsp, nspPos), Register.xmm((RegIndex)i));
                        nspPos += 16;
                    }
                }
            }

            // Save MM registers using MOVQ.
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

            // Save GP registers using MOV.
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

            if (Compiler.Logger != null)
            {
                Compiler.Comment("Body");
            }
        }

        internal void EmitEpilog(CompilerContext cc)
        {
            RegisterMask preservedGP = _modifiedAndPreservedGP;
            RegisterMask preservedMM = _modifiedAndPreservedMM;
            RegisterMask preservedXMM = _modifiedAndPreservedXMM;

            int stackAdd =
              _functionCallStackSize +
              _memStackSize16 +
              _peMovStackSize +
              _peAdjustStackSize;

            int nspPos;

            nspPos = (_isEspAdjusted)
              ? (_memStackSize16 + _functionCallStackSize)
              : -(_peMovStackSize + _peAdjustStackSize);

            if (Compiler.Logger != null)
            {
                Compiler.Comment("Epilog");
            }

            // Restore XMM registers using MOVDQA/MOVDQU.
            if (preservedXMM != RegisterMask.Zero)
            {
                for (int i = 0; i < RegNum.XMM; i++)
                {
                    RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                    if ((preservedXMM & mask) != RegisterMask.Zero)
                    {
                        Compiler.Emit(_movDqaInstruction, Register.xmm((RegIndex)i), Mem.dqword_ptr(Register.nsp, nspPos));
                        nspPos += 16;
                    }
                }
            }

            // Restore MM registers using MOVQ.
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

            // Restore GP registers using MOV.
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

            if (_isEspAdjusted && stackAdd != 0)
            {
                Compiler.Emit(InstructionCode.Add, Register.nsp, (Imm)stackAdd);
            }

            // Restore GP registers using POP.
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

            // Emit Emms.
            if (_emitEMMS)
                Compiler.Emit(InstructionCode.Emms);

            // Emit SFence / LFence / MFence.
            if (_emitSFence && _emitLFence)
                Compiler.Emit(InstructionCode.Mfence); // MFence == SFence & LFence.
            else if (_emitSFence)
                Compiler.Emit(InstructionCode.Sfence); // Only SFence.
            else if (_emitLFence)
                Compiler.Emit(InstructionCode.Lfence); // Only LFence.

            // Emit standard epilog leave code (if needed).
            if (!_isNaked)
            {
                CpuInfo cpuInfo = CpuInfo.Instance;

                if (cpuInfo.VendorId == CpuVendor.Amd)
                {
                    // AMD seems to prefer LEAVE instead of MOV/POP sequence.
                    Compiler.Emit(InstructionCode.Leave);
                }
                else
                {
                    Compiler.Emit(InstructionCode.Mov, Register.nsp, Register.nbp);
                    Compiler.Emit(InstructionCode.Pop, Register.nbp);
                }
            }

            // Emit return using correct instruction.
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
                    FunctionPrototype.Argument a = _functionPrototype.Arguments[i];
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
                            FunctionPrototype.Argument a = _functionPrototype.Arguments[i];

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

        internal void PrepareVariables(Emittable first)
        {
            int i;
            int count = _functionPrototype.Arguments.Length;

            for (i = 0; i < count; i++)
            {
                VarData vdata = _argumentVariables[i];

                // This is where variable scope starts.
                vdata.FirstEmittable = first;
                // If this will not be changed then it will be deallocated immediately.
                vdata.LastEmittable = first;
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

                if (vdata.FirstEmittable != null ||
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
