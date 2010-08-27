namespace AsmJitNet
{
    using System;
    using System.Diagnostics.Contracts;
    using System.Linq;
    using System.Text;
    using Debug = System.Diagnostics.Debug;

    public class Function : Emittable
    {
        private FunctionPrototype _functionPrototype;
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

        private int _modifiedAndPreservedGP;

        private int _modifiedAndPreservedMM;

        private int _modifiedAndPreservedXMM;

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
        private readonly Dummy _end;

        private Function(Compiler compiler)
            : base(compiler)
        {
            Contract.Requires(compiler != null);

            _entryLabel = compiler.DefineLabel();
            _exitLabel = compiler.DefineLabel();

            _prolog = new Prolog(compiler, this);
            _epilog = new Epilog(compiler, this);
            _end = new Dummy(compiler);
        }

        public Function(Compiler compiler, CallingConvention callingConvention, Type delegateType)
            : this(compiler)
        {
            _functionPrototype = new FunctionPrototype(callingConvention, delegateType);
        }

        public Function(Compiler compiler, CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
            : this(compiler)
        {
            _functionPrototype = new FunctionPrototype(callingConvention, arguments, returnValue);
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

        public Dummy End
        {
            get
            {
                Contract.Ensures(Contract.Result<Dummy>() != null);

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
            Contract.Invariant(_entryLabel != null);
            Contract.Invariant(_exitLabel != null);
            Contract.Invariant(_prolog != null);
            Contract.Invariant(_epilog != null);
            Contract.Invariant(_end != null);
        }

        public override void Prepare(CompilerContext cc)
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
                    varData.HomeRegisterIndex = a._registerIndex;
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
            _pePushPop = true;
            _emitEMMS = false;
            _emitSFence = false;
            _emitLFence = false;

            // if another function is calleb by the function, it's needed to adjust ESP
            if (_isCaller)
                _isEspAdjusted = true;

            _isNaked = (_hints & FunctionHints.Naked) != 0;
            _pePushPop = (_hints & FunctionHints.PushPopSequence) != 0;
            _emitEMMS = (_hints & FunctionHints.Emms) != 0;
            _emitSFence = (_hints & FunctionHints.StoreFence) != 0;
            _emitLFence = (_hints & FunctionHints.LoadFence) != 0;

            if (!_isStackAlignedByOsTo16Bytes && !_isNaked && (cc.Mem16BlocksCount > 0))
            {
                // Have to align stack to 16-bytes.
                _isStackAlignedByFnTo16Bytes = true;
                _isEspAdjusted = true;
            }

            _modifiedAndPreservedGP = cc.ModifiedGPRegisters & _functionPrototype.PreservedGP & ~(1 << (int)RegIndex.Esp);
            _modifiedAndPreservedMM = cc.ModifiedMMRegisters & _functionPrototype.PreservedMM;
            _modifiedAndPreservedXMM = cc.ModifiedXMMRegisters & _functionPrototype.PreservedXMM;

            _movDqaInstruction = (_isStackAlignedByOsTo16Bytes || !_isNaked) ? InstructionCode.Movdqa : InstructionCode.Movdqu;

            // Prolog & Epilog stack size.
            {
                int memGP = BitCount(_modifiedAndPreservedGP) * IntPtr.Size;
                int memMM = BitCount(_modifiedAndPreservedMM) * 8;
                int memXMM = BitCount(_modifiedAndPreservedXMM) * 16;

                if (_pePushPop)
                {
                    _pePushPopStackSize = memGP;
                    _peMovStackSize = memXMM + AlignTo16(memMM);
                }
                else
                {
                    _pePushPopStackSize = 0;
                    _peMovStackSize = memXMM + AlignTo16(memMM + memGP);
                }
            }

            if (_isStackAlignedByFnTo16Bytes)
            {
                _peAdjustStackSize += DeltaTo16(_pePushPopStackSize);
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
            _memStackSize16 = AlignTo16(_memStackSize);

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

        private static int BitCount(int x)
        {
            return BitCount((uint)x);
        }

        private static int BitCount(uint x)
        {
            x = x - ((x >> 1) & 0x55555555);
            x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
            return (int)(((x + (x >> 4) & 0xF0F0F0F) * 0x1010101) >> 24);
        }

        private static int DeltaTo16(int x)
        {
            return AlignTo16(x) - x;
        }

        private static int AlignTo16(int x)
        {
            return (x + 15) & ~15;
        }

        internal void EmitProlog(CompilerContext cc)
        {
            int i;
            int mask;

            int preservedGP = _modifiedAndPreservedGP;
            int preservedMM = _modifiedAndPreservedMM;
            int preservedXMM = _modifiedAndPreservedXMM;

            int stackSubtract = _functionCallStackSize + _memStackSize16 + _peMovStackSize + _peAdjustStackSize;
            int nspPos;

            if (Compiler.Logger != null && Compiler.Logger.IsUsed)
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
            if (preservedGP != 0 && _pePushPop)
            {
                for (i = 0, mask = 1; i < (int)RegNum.GP; i++, mask <<= 1)
                {
                    if ((preservedGP & mask) != 0)
                        Compiler.Emit(InstructionCode.Push, Register.gpn((RegIndex)i));
                }
            }

            if (_isEspAdjusted)
            {
                nspPos = _memStackSize16;
                if (stackSubtract != 0)
                    Compiler.Emit(InstructionCode.Sub, Register.nsp, (Imm)(stackSubtract));
            }
            else
            {
                nspPos = -(_peMovStackSize + _peAdjustStackSize);
                //if (_pePushPop) nspPos += bitCount(preservedGP) * sizeof(sysint_t);
            }

            // Save XMM registers using MOVDQA/MOVDQU.
            if (preservedXMM != 0)
            {
                for (i = 0, mask = 1; i < (int)RegNum.XMM; i++, mask <<= 1)
                {
                    if ((preservedXMM & mask) != 0)
                    {
                        Compiler.Emit(_movDqaInstruction, Mem.dqword_ptr(Register.nsp, nspPos), Register.xmm((RegIndex)i));
                        nspPos += 16;
                    }
                }
            }

            // Save MM registers using MOVQ.
            if (preservedMM != 0)
            {
                for (i = 0, mask = 1; i < 8; i++, mask <<= 1)
                {
                    if ((preservedMM & mask) != 0)
                    {
                        Compiler.Emit(InstructionCode.Movq, Mem.qword_ptr(Register.nsp, nspPos), Register.mm((RegIndex)i));
                        nspPos += 8;
                    }
                }
            }

            // Save GP registers using MOV.
            if (preservedGP != 0 && !_pePushPop)
            {
                for (i = 0, mask = 1; i < (int)RegNum.GP; i++, mask <<= 1)
                {
                    if ((preservedGP & mask) != 0)
                    {
                        Compiler.Emit(InstructionCode.Mov, Mem.sysint_ptr(Register.nsp, nspPos), Register.gpn((RegIndex)i));
                        nspPos += IntPtr.Size;
                    }
                }
            }

            if (Compiler.Logger != null && Compiler.Logger.IsUsed)
            {
                Compiler.Comment("Body");
            }
        }

        internal void EmitEpilog(CompilerContext cc)
        {
            int i;
            int mask;

            int preservedGP = _modifiedAndPreservedGP;
            int preservedMM = _modifiedAndPreservedMM;
            int preservedXMM = _modifiedAndPreservedXMM;

            int stackAdd =
              _functionCallStackSize +
              _memStackSize16 +
              _peMovStackSize +
              _peAdjustStackSize;

            int nspPos;

            nspPos = (_isEspAdjusted)
              ? (_memStackSize16)
              : -(_peMovStackSize + _peAdjustStackSize);

            if (Compiler.Logger != null && Compiler.Logger.IsUsed)
            {
                Compiler.Comment("Epilog");
            }

            // Restore XMM registers using MOVDQA/MOVDQU.
            if (preservedXMM != 0)
            {
                for (i = 0, mask = 1; i < (int)RegNum.XMM; i++, mask <<= 1)
                {
                    if ((preservedXMM & mask) != 0)
                    {
                        Compiler.Emit(_movDqaInstruction, Register.xmm((RegIndex)i), Mem.dqword_ptr(Register.nsp, nspPos));
                        nspPos += 16;
                    }
                }
            }

            // Restore MM registers using MOVQ.
            if (preservedMM != 0)
            {
                for (i = 0, mask = 1; i < 8; i++, mask <<= 1)
                {
                    if ((preservedMM & mask) != 0)
                    {
                        Compiler.Emit(InstructionCode.Movq, Register.mm((RegIndex)i), Mem.qword_ptr(Register.nsp, nspPos));
                        nspPos += 8;
                    }
                }
            }

            // Restore GP registers using MOV.
            if (preservedGP != 0 && !_pePushPop)
            {
                for (i = 0, mask = 1; i < (int)RegNum.GP; i++, mask <<= 1)
                {
                    if ((preservedGP & mask) != 0)
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
            if (preservedGP != 0 && _pePushPop)
            {
                for (i = (int)RegNum.GP - 1, mask = 1 << i; i >= 0; i--, mask >>= 1)
                {
                    if ((preservedGP & mask) != 0)
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
            if (_emitSFence && !_emitLFence)
                Compiler.Emit(InstructionCode.Sfence); // Only SFence.
            if (!_emitSFence && _emitLFence)
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
            size = AlignTo16(size);

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
                        Assembler.DumpOperand(buffer, regOp);
                    }
                    else
                    {
                        Mem memOp = new Mem();
                        memOp.Base = RegIndex.Esp;
                        memOp.Displacement = (IntPtr)a._stackOffset;
                        Assembler.DumpOperand(buffer, memOp);
                    }

                    // original format string: "; %-3u| %-9s| %-3u| %-15s|\n"
                    logger.LogFormat("; {0}| {1}| {2}| {3}|" + Environment.NewLine,
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
                    Debug.Assert(vdata != null);

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
                        Assembler.DumpOperand(buffer, memOp);
                    }

                    logger.LogFormat("; {0}| {1}| {2}| {3}| r={4}w={5}x={6}| r={7}w={8}x={9}|" + Environment.NewLine,
                        // Variable id.
                        (uint)(i & Operand.OperandIdValueMask),
                        // Variable type.
                        (int)vdata.Type < _variableTypeCount ? vinfo.Name : "invalid",
                        // Variable size.
                        vdata.Size,
                        // Variable memory home.
                        buffer,
                        // Register access count.
                        vdata.RegisterReadCount,
                        vdata.RegisterWriteCount,
                        vdata.RegisterRWCount,
                        // Memory access count.
                        vdata.MemoryReadCount,
                        vdata.MemoryWriteCount,
                        vdata.MemoryRWCount
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
                    int regs;
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
                        Debug.Fail("");
                        continue;
                    }

                    for (i = 0; i < (int)RegNum.Base; i++)
                    {
                        if ((regs & (1 << i)) != 0)
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
