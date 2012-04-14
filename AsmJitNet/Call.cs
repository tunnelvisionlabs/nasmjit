namespace AsmJitNet
{
    using System;
    using System.Diagnostics.Contracts;

    public class Call : Emittable
    {
        private readonly Function _caller;
        private Operand _target;
        private readonly Operand[] _ret = new Operand[2];
        private readonly Operand[] _args;
        private readonly FunctionPrototype _functionPrototype;

        /// <summary>
        /// Mask of GP registers used as function arguments
        /// </summary>
        private RegisterMask _gpParams;

        /// <summary>
        /// Mask of MM registers used as function arguments
        /// </summary>
        private RegisterMask _mmParams;

        /// <summary>
        /// Mask of XMM registers used as function arguments
        /// </summary>
        private RegisterMask _xmmParams;

        /// <summary>
        /// Variables (extracted from operands)
        /// </summary>
        private VarCallRecord[] _variables;

        private readonly VarCallRecord[] _argumentToVarRecord = new VarCallRecord[32];

        public Call(Compiler compiler, Function caller, Operand target, CallingConvention callingConvention, Type delegateType)
            : base(compiler)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(caller != null);

            _caller = caller;
            _target = target;

            _functionPrototype = new FunctionPrototype(callingConvention, delegateType);
            if (_functionPrototype.Arguments != null && _functionPrototype.Arguments.Length > 0)
                _args = new Operand[_functionPrototype.Arguments.Length];
        }

        public Call(Compiler compiler, Function caller, Operand target, CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
            : base(compiler)
        {
            Contract.Requires(arguments != null);

            _caller = caller;
            _target = target;

            _functionPrototype = new FunctionPrototype(callingConvention, arguments, returnValue);
            if (arguments != null && arguments.Length > 0)
                _args = new Operand[arguments.Length];
        }

        public override EmittableType EmittableType
        {
            get
            {
                return EmittableType.Call;
            }
        }

        public override int MaxSize
        {
            get
            {
                // TODO: Not optimal
                return 15;
            }
        }

        public Function Caller
        {
            get
            {
                Contract.Ensures(Contract.Result<Function>() != null);

                return _caller;
            }
        }

        public Operand Target
        {
            get
            {
                return _target;
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

        [ContractInvariantMethod]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance", "CA1822:MarkMembersAsStatic", Justification = "Required for code contracts.")]
        private void ObjectInvariant()
        {
            Contract.Invariant(_functionPrototype != null);
            Contract.Invariant(_caller != null);
        }

        public void SetArgument(int i, Operand operand)
        {
            if (i < 0)
                throw new ArgumentOutOfRangeException("i");
            if (i >= _functionPrototype.Arguments.Length)
                throw new ArgumentException();

            _args[i] = operand;
        }

        public bool SetReturn(Operand first)
        {
            return SetReturn(first, Operand.None);
        }

        public bool SetReturn(Operand first, Operand second)
        {
            first = first ?? Operand.None;
            second = second ?? Operand.None;

            if (!(first.IsNone || first.IsVarMem) || !(second.IsNone || second.IsVarMem))
                throw new ArgumentException("The return value storage location must be a variable or memory location.");

            _ret[0] = first;
            _ret[1] = second;
            return true;
        }

        protected override void PrepareImpl(CompilerContext cc)
        {
            // Prepare is similar to EInstruction::prepare(). We collect unique variables
            // and update statistics, but we don't use standard alloc/free register calls.
            //
            // The calling function is also unique in variable allocator point of view,
            // because we need to alloc some variables that may be destroyed be the 
            // callee (okay, may not, but this is not guaranteed).
            Offset = cc.CurrentOffset;

            // Tell EFunction that another function will be called inside. It needs this
            // information to reserve stack for the call and to mark esp adjustable.
            Caller.ReserveStackForFunctionCall(Prototype.ArgumentsStackSize);

            int i;
            int argumentsCount = Prototype.Arguments.Length;
            int operandsCount = argumentsCount;
            int variablesCount = 0;

            // Create registers used as arguments mask.
            for (i = 0; i < argumentsCount; i++)
            {
                FunctionPrototype.Argument fArg = Prototype.Arguments[i];

                if (fArg._registerIndex != RegIndex.Invalid)
                {
                    switch (fArg._variableType)
                    {
                    case VariableType.GPD:
                    case VariableType.GPQ:
                        _gpParams |= RegisterMask.FromIndex(fArg._registerIndex);
                        break;
                    case VariableType.MM:
                        _mmParams |= RegisterMask.FromIndex(fArg._registerIndex);
                        break;
                    case VariableType.XMM:
                    case VariableType.XMM_1F:
                    case VariableType.XMM_4F:
                    case VariableType.XMM_1D:
                    case VariableType.XMM_2D:
                        _xmmParams |= RegisterMask.FromIndex(fArg._registerIndex);
                        break;
                    default:
                        throw new CompilerException("Invalid variable type in function call argument.");
                    }
                }
                else
                {
                    cc.Function.IsEspAdjusted = true;
                }
            }

            // Call address.
            operandsCount++;

            // The first return value.
            if (_ret[0] != null && !_ret[0].IsNone)
                operandsCount++;

            // The second return value.
            if (_ret[1] != null && !_ret[1].IsNone)
                operandsCount++;

            for (i = 0; i < operandsCount; i++)
            {
                Operand o = (i < argumentsCount)
                  ? (_args[i])
                  : (i == argumentsCount ? _target : _ret[i - argumentsCount - 1]);

                if (o.IsVar)
                {
                    if (o.Id == InvalidValue)
                        throw new CompilerException();

                    VarData vdata = Compiler.GetVarData(o.Id);
                    Contract.Assert(vdata != null);

                    if (vdata.WorkOffset == Offset)
                        continue;
                    if (!cc.IsActive(vdata))
                        cc.AddActive(vdata);

                    vdata.WorkOffset = Offset;
                    variablesCount++;
                }
                else if (o.IsMem)
                {
                    if ((o.Id & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = Compiler.GetVarData(o.Id);
                        Contract.Assert(vdata != null);

                        cc.MarkMemoryUsed(vdata);
                        if (!cc.IsActive(vdata))
                            cc.AddActive(vdata);

                        continue;
                    }
                    else if (((int)((Mem)o).Base & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = Compiler.GetVarData((int)((Mem)o).Base);
                        Contract.Assert(vdata != null);

                        if (vdata.WorkOffset == Offset)
                            continue;
                        if (!cc.IsActive(vdata))
                            cc.AddActive(vdata);

                        vdata.WorkOffset = Offset;
                        variablesCount++;
                    }

                    if (((int)((Mem)o).Index & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = Compiler.GetVarData((int)((Mem)o).Index);
                        Contract.Assert(vdata != null);

                        if (vdata.WorkOffset == Offset)
                            continue;
                        if (!cc.IsActive(vdata))
                            cc.AddActive(vdata);

                        vdata.WorkOffset = Offset;
                        variablesCount++;
                    }
                }
                else
                {
                    if (o != _target)
                        throw new NotImplementedException();
                }
            }

            _variables = new VarCallRecord[variablesCount];

            // Traverse all active variables and set their firstCallable pointer to this
            // call. This information can be used to choose between the preserved-first
            // and preserved-last register allocation.
            if (cc.Active != null)
            {
                VarData first = cc.Active;
                VarData active = first;
                do
                {
                    if (active.FirstCallable == null)
                        active.FirstCallable = this;
                    active = active.NextActive;
                } while (active != first);
            }

            if (variablesCount == 0)
            {
                cc.CurrentOffset++;
                return;
            }

            for (int j = 0; j < variablesCount; j++)
                _variables[j] = new VarCallRecord();

            VarCallRecord var = null;
            int curIndex = 0;
            int varIndex = -1;

            Action<VarData> __GET_VARIABLE =
                __vardata__ =>
                {
                    VarData _candidate = __vardata__;

                    for (varIndex = curIndex; ; )
                    {
                        if (varIndex == 0)
                        {
                            varIndex = curIndex++;
                            _variables[varIndex].vdata = _candidate;
                            break;
                        }

                        varIndex--;

                        if (_variables[varIndex].vdata == _candidate)
                            break;
                    }

                    var = _variables[varIndex];
                    Contract.Assert(var != null);
                };

            for (i = 0; i < operandsCount; i++)
            {
                Operand o = (i < argumentsCount)
                  ? (_args[i])
                  : (i == argumentsCount ? _target : _ret[i - argumentsCount - 1]);

                if (o.IsVar)
                {
                    VarData vdata = Compiler.GetVarData(o.Id);
                    Contract.Assert(vdata != null);

                    __GET_VARIABLE(vdata);
                    _argumentToVarRecord[i] = var;

                    if (i < argumentsCount)
                    {
                        FunctionPrototype.Argument fArg = Prototype.Arguments[i];

                        if (fArg._registerIndex != RegIndex.Invalid)
                        {
                            cc.NewRegisterHomeIndex(vdata, fArg._registerIndex);

                            switch (fArg._variableType)
                            {
                            case VariableType.GPD:
                            case VariableType.GPQ:
                                var.Flags |= VarCallFlags.IN_GP;
                                var.InCount++;
                                break;
                            case VariableType.MM:
                                var.Flags |= VarCallFlags.IN_MM;
                                var.InCount++;
                                break;
                            case VariableType.XMM:
                            case VariableType.XMM_1F:
                            case VariableType.XMM_4F:
                            case VariableType.XMM_1D:
                            case VariableType.XMM_2D:
                                var.Flags |= VarCallFlags.IN_XMM;
                                var.InCount++;
                                break;
                            default:
                                throw new CompilerException("Invalid variable type in function call argument.");
                            }
                        }
                        else
                        {
                            var.InCount++;
                        }

                        vdata.RegisterReadCount++;
                    }
                    else if (i == argumentsCount)
                    {
                        RegisterMask mask = ~Prototype.PreservedGP &
                                        ~Prototype.PassedGP &
                                        (RegisterMask.MaskToIndex(RegNum.GP) & ~RegisterMask.FromIndex(RegIndex.Eax));

                        cc.NewRegisterHomeIndex(vdata, mask.FirstRegister);
                        cc.NewRegisterHomeMask(vdata, mask);

                        var.Flags |= VarCallFlags.CALL_OPERAND_REG;
                        vdata.RegisterReadCount++;
                    }
                    else
                    {
                        switch (vdata.Type)
                        {
                        case VariableType.GPD:
                        case VariableType.GPQ:
                            if (i == argumentsCount + 1)
                                var.Flags |= VarCallFlags.OUT_EAX;
                            else
                                var.Flags |= VarCallFlags.OUT_EDX;
                            break;

                        case VariableType.X87:
                        case VariableType.X87_1F:
                        case VariableType.X87_1D:
                            if (Util.IsX86)
                            {
                                if (i == argumentsCount + 1)
                                    var.Flags |= VarCallFlags.OUT_ST0;
                                else
                                    var.Flags |= VarCallFlags.OUT_ST1;
                            }
                            else if (Util.IsX64)
                            {
                                if (i == argumentsCount + 1)
                                    var.Flags |= VarCallFlags.OUT_XMM0;
                                else
                                    var.Flags |= VarCallFlags.OUT_XMM1;
                            }
                            else
                            {
                                throw new NotImplementedException();
                            }

                            break;

                        case VariableType.MM:
                            var.Flags |= VarCallFlags.OUT_MM0;
                            break;

                        case VariableType.XMM:
                        case VariableType.XMM_4F:
                        case VariableType.XMM_2D:
                            if (i == argumentsCount + 1)
                                var.Flags |= VarCallFlags.OUT_XMM0;
                            else
                                var.Flags |= VarCallFlags.OUT_XMM1;
                            break;

                        case VariableType.XMM_1F:
                        case VariableType.XMM_1D:
                            if (Util.IsX86)
                            {
                                if (i == argumentsCount + 1)
                                    var.Flags |= VarCallFlags.OUT_ST0;
                                else
                                    var.Flags |= VarCallFlags.OUT_ST1;
                            }
                            else if (Util.IsX64)
                            {
                                if (i == argumentsCount + 1)
                                    var.Flags |= VarCallFlags.OUT_XMM0;
                                else
                                    var.Flags |= VarCallFlags.OUT_XMM1;
                            }
                            else
                            {
                                throw new NotImplementedException();
                            }

                            break;

                        default:
                            throw new CompilerException("Invalid variable type in function call argument.");
                        }

                        vdata.RegisterWriteCount++;
                    }
                }
                else if (o.IsMem)
                {
                    if (i != argumentsCount)
                        throw new CompilerException();

                    if ((o.Id & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = Compiler.GetVarData(o.Id);
                        Contract.Assert(vdata != null);

                        vdata.MemoryReadCount++;
                    }
                    else if (((int)((Mem)o).Base & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = Compiler.GetVarData((int)((Mem)o).Base);
                        Contract.Assert(vdata != null);

                        vdata.RegisterReadCount++;

                        __GET_VARIABLE(vdata);
                        var.Flags |= VarCallFlags.CALL_OPERAND_REG | VarCallFlags.CALL_OPERAND_MEM;
                    }

                    if (((int)((Mem)o).Index & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = Compiler.GetVarData((int)((Mem)o).Index);
                        Contract.Assert(vdata != null);

                        vdata.RegisterReadCount++;

                        __GET_VARIABLE(vdata);
                        var.Flags |= VarCallFlags.CALL_OPERAND_REG | VarCallFlags.CALL_OPERAND_MEM;
                    }
                }
                else
                {
                    if (o != _target)
                        throw new NotImplementedException();
                }
            }

            // Traverse all variables and update firstEmittable / lastEmittable. This
            // function is called from iterator that scans emittables using forward
            // direction so we can use this knowledge to optimize the process.
            //
            // Same code is in EInstruction::prepare().
            for (i = 0; i < _variables.Length; i++)
            {
                VarData v = _variables[i].vdata;

                // First emittable (begin of variable scope).
                if (v.FirstEmittable == null)
                    v.FirstEmittable = this;

                // Last emittable (end of variable scope).
                v.LastEmittable = this;
            }

            cc.CurrentOffset++;
        }

        private const int FUNC_MAX_ARGS = 32;

        protected override Emittable TranslateImpl(CompilerContext cc)
        {
            RegisterMask preserved;

            RegIndex temporaryGpReg;
            RegIndex temporaryXmmReg;

            int offset = cc.CurrentOffset;
            Compiler compiler = cc.Compiler;

            // Constants.
            FunctionPrototype.Argument[] targs = Prototype.Arguments;

            int argumentsCount = Prototype.Arguments.Length;
            int variablesCount = _variables.Length;

            // Processed arguments.
            bool[] processed = new bool[FUNC_MAX_ARGS];

            compiler.Comment("Function Call");

            // These variables are used by the instruction and we set current offset
            // to their work offsets -> The SpillCandidate method never returns 
            // the variable used by this instruction.
            for (int i = 0; i < variablesCount; i++)
            {
                _variables[i].vdata.WorkOffset = offset;

                // Init back-reference to VarCallRecord.
                _variables[i].vdata.Temp = _variables[i];
            }

            // --------------------------------------------------------------------------
            // STEP 1:
            //
            // Spill variables which are not used by the function call and have to
            // be destroyed. These registers may be used by callee.
            // --------------------------------------------------------------------------

            preserved = Prototype.PreservedGP;
            for (int i = 0; i < RegNum.GP; i++)
            {
                RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                VarData vdata = cc.State.GP[i];
                if (vdata != null && vdata.WorkOffset != offset && (preserved & mask) == RegisterMask.Zero)
                {
                    cc.SpillGPVar(vdata);
                }
            }

            preserved = Prototype.PreservedMM;
            for (int i = 0; i < RegNum.MM; i++)
            {
                RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                VarData vdata = cc.State.MM[i];
                if (vdata != null && vdata.WorkOffset != offset && (preserved & mask) == RegisterMask.Zero)
                {
                    cc.SpillMMVar(vdata);
                }
            }

            preserved = Prototype.PreservedXMM;
            for (int i = 0; i < RegNum.XMM; i++)
            {
                RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                VarData vdata = cc.State.XMM[i];
                if (vdata != null && vdata.WorkOffset != offset && (preserved & mask) == RegisterMask.Zero)
                {
                    cc.SpillXMMVar(vdata);
                }
            }

            // --------------------------------------------------------------------------
            // STEP 2:
            //
            // Move all arguments to the stack which all already in registers.
            // --------------------------------------------------------------------------

            for (int i = 0; i < argumentsCount; i++)
            {
                if (processed[i])
                    continue;

                FunctionPrototype.Argument argType = targs[i];
                if (argType._registerIndex != RegIndex.Invalid)
                    continue;

                Operand operand = _args[i];

                if (operand.IsVar)
                {
                    VarCallRecord rec = _argumentToVarRecord[i];
                    VarData vdata = compiler.GetVarData(operand.Id);
                    Contract.Assert(vdata != null);

                    if (vdata.RegisterIndex != RegIndex.Invalid)
                    {
                        MoveAllocatedVariableToStack(cc, vdata, argType);

                        rec.InDone++;
                        processed[i] = true;
                    }
                }
            }

            // --------------------------------------------------------------------------
            // STEP 3:
            //
            // Spill all non-preserved variables we moved to stack in STEP #2.
            // --------------------------------------------------------------------------

            for (int i = 0; i < argumentsCount; i++)
            {
                VarCallRecord rec = _argumentToVarRecord[i];
                if (rec == null || processed[i])
                    continue;

                if (rec.InDone >= rec.InCount)
                {
                    VarData vdata = rec.vdata;
                    if (vdata.RegisterIndex == RegIndex.Invalid)
                        continue;

                    if (rec.OutCount != 0)
                    {
                        // Variable will be rewritten by function return value, it's not needed
                        // to spill it. It will be allocated again by ECall.
                        cc.UnuseVar(rec.vdata, VariableState.Unused);
                    }
                    else
                    {
                        switch (vdata.Type)
                        {
                        case VariableType.GPD:
                        case VariableType.GPQ:
                            if ((Prototype.PreservedGP & RegisterMask.FromIndex(vdata.RegisterIndex)) == RegisterMask.Zero)
                                cc.SpillGPVar(vdata);
                            break;
                        case VariableType.MM:
                            if ((Prototype.PreservedMM & RegisterMask.FromIndex(vdata.RegisterIndex)) == RegisterMask.Zero)
                                cc.SpillMMVar(vdata);
                            break;
                        case VariableType.XMM:
                        case VariableType.XMM_1F:
                        case VariableType.XMM_1D:
                        case VariableType.XMM_4F:
                        case VariableType.XMM_2D:
                            if ((Prototype.PreservedXMM & RegisterMask.FromIndex(vdata.RegisterIndex)) == RegisterMask.Zero)
                                cc.SpillXMMVar(vdata);
                            break;
                        default:
                            throw new CompilerException();
                        }
                    }
                }
            }

            // --------------------------------------------------------------------------
            // STEP 4:
            //
            // Get temporary register that we can use to pass input function arguments.
            // Now it's safe to do, because the non-needed variables should be spilled.
            // --------------------------------------------------------------------------

            temporaryGpReg = FindTemporaryGpRegister(cc);
            temporaryXmmReg = FindTemporaryXmmRegister(cc);

            // If failed to get temporary register then we need just to pick one.
            if (temporaryGpReg == RegIndex.Invalid)
            {
                // TODO.
                throw new NotImplementedException();
            }
            if (temporaryXmmReg == RegIndex.Invalid)
            {
                // TODO.
                throw new NotImplementedException();
            }

            // --------------------------------------------------------------------------
            // STEP 5:
            //
            // Move all remaining arguments to the stack (we can use temporary register).
            // or allocate it to the primary register. Also move immediates.
            // --------------------------------------------------------------------------

            for (int i = 0; i < argumentsCount; i++)
            {
                if (processed[i])
                    continue;

                FunctionPrototype.Argument argType = targs[i];
                if (argType._registerIndex != RegIndex.Invalid)
                    continue;

                Operand operand = _args[i];

                if (operand.IsVar)
                {
                    VarCallRecord rec = _argumentToVarRecord[i];
                    VarData vdata = compiler.GetVarData(operand.Id);
                    Contract.Assert(vdata != null);

                    MoveSpilledVariableToStack(cc, vdata, argType, temporaryGpReg, temporaryXmmReg);

                    rec.InDone++;
                    processed[i] = true;
                }
                else if (operand.IsImm)
                {
                    Mem dst;

                    switch (argType._variableType)
                    {
                    case VariableType.GPD:
                        dst = Mem.dword_ptr(Register.nsp, -IntPtr.Size + argType._stackOffset);
                        break;

                    case VariableType.GPQ:
                        dst = Mem.qword_ptr(Register.nsp, -IntPtr.Size + argType._stackOffset);
                        break;

                    case VariableType.X87:
                    case VariableType.X87_1D:
                    case VariableType.X87_1F:
                        throw new NotImplementedException();

                    case VariableType.MM:
                        dst = Mem.mmword_ptr(Register.nsp, -IntPtr.Size + argType._stackOffset);
                        break;

                    case VariableType.XMM:
                    case VariableType.XMM_1D:
                    case VariableType.XMM_1F:
                    case VariableType.XMM_2D:
                    case VariableType.XMM_4F:
                        dst = Mem.xmmword_ptr(Register.nsp, -IntPtr.Size + argType._stackOffset);
                        break;

                    default:
                        throw new NotImplementedException();
                    }

                    compiler.Mov(dst, (Imm)operand);
                }
            }

            // --------------------------------------------------------------------------
            // STEP 6:
            //
            // Allocate arguments to registers.
            // --------------------------------------------------------------------------

            bool didWork;

            do
            {
                didWork = false;

                for (int i = 0; i < argumentsCount; i++)
                {
                    if (processed[i])
                        continue;

                    VarCallRecord rsrc = _argumentToVarRecord[i];

                    Operand osrc = _args[i];
                    if (!osrc.IsVar)
                        throw new CompilerException();
                    VarData vsrc = compiler.GetVarData(osrc.Id);
                    Contract.Assert(vsrc != null);

                    FunctionPrototype.Argument srcArgType = targs[i];
                    VarData vdst = GetOverlappingVariable(cc, srcArgType);

                    if (vsrc == vdst)
                    {
                        rsrc.InDone++;
                        processed[i] = true;

                        didWork = true;
                        continue;
                    }
                    else if (vdst != null)
                    {
                        VarCallRecord rdst = (VarCallRecord)(vdst.Temp);

                        if (rdst.InDone >= rdst.InCount && (rdst.Flags & VarCallFlags.CALL_OPERAND_REG) == 0)
                        {
                            // Safe to spill.
                            if (rdst.OutCount != 0 || vdst.LastEmittable == this)
                                cc.UnuseVar(vdst, VariableState.Unused);
                            else
                                cc.SpillVar(vdst);
                            vdst = null;
                        }
                        else
                        {
                            int x = Prototype.FindArgumentByRegisterCode(VariableInfo.GetVariableRegisterCode(vsrc.Type, vsrc.RegisterIndex));

                            bool doSpill = true;

                            if ((VariableInfo.GetVariableClass(vdst.Type) & VariableClass.GP) != 0)
                            {
                                // Try to emit mov to register which is possible for call() operand.
                                if (x == InvalidValue && (rdst.Flags & VarCallFlags.CALL_OPERAND_REG) != 0)
                                {
                                    int rIndex;

                                    // The mask which contains registers which are not-preserved
                                    // (these that might be clobbered by the callee) and which are
                                    // not used to pass function arguments. Each register contained
                                    // in this mask is ideal to be used by call() instruction.
                                    RegisterMask possibleMask = ~Prototype.PreservedGP &
                                                            ~Prototype.PassedGP &
                                                            (RegisterMask.MaskToIndex(RegNum.GP) & ~RegisterMask.FromIndex(RegIndex.Eax));

                                    if (possibleMask != RegisterMask.Zero)
                                    {
                                        for (rIndex = 0; rIndex < RegNum.GP; rIndex++)
                                        {
                                            RegisterMask rBit = RegisterMask.FromIndex((RegIndex)rIndex);
                                            if ((possibleMask & rBit) != RegisterMask.Zero)
                                            {
                                                if (cc.State.GP[rIndex] == null)
                                                {
                                                    // This is the best possible solution, the register is
                                                    // free. We do not need to continue with this loop, the
                                                    // rIndex will be used by the call().
                                                    break;
                                                }
                                                else
                                                {
                                                    // Wait until the register is freed or try to find another.
                                                    doSpill = false;
                                                    didWork = true;
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        // Try to find a register which is free and which is not used
                                        // to pass a function argument.
                                        possibleMask = Prototype.PreservedGP;

                                        for (rIndex = 0; rIndex < RegNum.GP; rIndex++)
                                        {
                                            RegisterMask rBit = RegisterMask.FromIndex((RegIndex)rIndex);
                                            if ((possibleMask & rBit) != RegisterMask.Zero)
                                            {
                                                // Found one.
                                                if (cc.State.GP[rIndex] == null)
                                                    break;
                                            }
                                        }
                                    }

                                    if (rIndex < RegNum.GP)
                                    {
                                        if (temporaryGpReg == vsrc.RegisterIndex)
                                            temporaryGpReg = (RegIndex)rIndex;

                                        compiler.Emit(InstructionCode.Mov, Register.gpn((RegIndex)rIndex), Register.gpn(vsrc.RegisterIndex));

                                        cc.State.GP[(int)vsrc.RegisterIndex] = null;
                                        cc.State.GP[rIndex] = vsrc;

                                        vsrc.RegisterIndex = (RegIndex)rIndex;
                                        cc.AllocatedGPRegister((RegIndex)rIndex);

                                        doSpill = false;
                                        didWork = true;
                                    }
                                }
                                // Emit xchg instead of spill/alloc if possible.
                                else if (x != InvalidValue)
                                {
                                    FunctionPrototype.Argument dstArgType = targs[x];
                                    if (VariableInfo.GetVariableInfo(dstArgType._variableType).Class == VariableInfo.GetVariableInfo(srcArgType._variableType).Class)
                                    {
                                        RegIndex dstIndex = vdst.RegisterIndex;
                                        RegIndex srcIndex = vsrc.RegisterIndex;

                                        if (srcIndex == dstArgType._registerIndex)
                                        {
                                            compiler.Emit(InstructionCode.Xchg, Register.gpn(dstIndex), Register.gpd(srcIndex));
                                            if (Util.IsX64)
                                            {
                                                if (vdst.Type != VariableType.GPD || vsrc.Type != VariableType.GPD)
                                                    compiler.Emit(InstructionCode.Xchg, Register.gpq(dstIndex), Register.gpq(srcIndex));
                                                else
                                                    compiler.Emit(InstructionCode.Xchg, Register.gpd(dstIndex), Register.gpd(srcIndex));
                                            }
                                            else
                                            {
                                                compiler.Emit(InstructionCode.Xchg, Register.gpd(dstIndex), Register.gpd(srcIndex));
                                            }

                                            cc.State.GP[(int)srcIndex] = vdst;
                                            cc.State.GP[(int)dstIndex] = vsrc;

                                            vdst.RegisterIndex = srcIndex;
                                            vsrc.RegisterIndex = dstIndex;

                                            rdst.InDone++;
                                            rsrc.InDone++;

                                            processed[i] = true;
                                            processed[x] = true;

                                            doSpill = false;
                                        }
                                    }
                                }
                            }

                            if (doSpill)
                            {
                                cc.SpillVar(vdst);
                                vdst = null;
                            }
                        }
                    }

                    if (vdst == null)
                    {
                        VarCallRecord rec = (VarCallRecord)(vsrc.Temp);

                        MoveSrcVariableToRegister(cc, vsrc, srcArgType);

                        switch (srcArgType._variableType)
                        {
                        case VariableType.GPD:
                        case VariableType.GPQ:
                            cc.MarkGPRegisterModified(srcArgType._registerIndex);
                            break;

                        case VariableType.MM:
                            cc.MarkMMRegisterModified(srcArgType._registerIndex);
                            break;

                        case VariableType.XMM:
                        case VariableType.XMM_1F:
                        case VariableType.XMM_1D:
                        case VariableType.XMM_4F:
                        case VariableType.XMM_2D:
                            cc.MarkXMMRegisterModified(srcArgType._registerIndex);
                            break;
                        }

                        rec.InDone++;
                        processed[i] = true;
                    }
                }
            } while (didWork);

            // --------------------------------------------------------------------------
            // STEP 7:
            //
            // Allocate operand used by CALL instruction.
            // --------------------------------------------------------------------------

            for (int i = 0; i < variablesCount; i++)
            {
                VarCallRecord r = _variables[i];
                if ((r.Flags & VarCallFlags.CALL_OPERAND_REG) != 0 &&
                    (r.vdata.RegisterIndex == RegIndex.Invalid))
                {
                    // If the register is not allocated and the call form is 'call reg' then
                    // it's possible to keep it in memory.
                    if ((r.Flags & VarCallFlags.CALL_OPERAND_MEM) == 0)
                    {
                        _target = GPVar.FromData(r.vdata).ToMem();
                        break;
                    }

                    if (temporaryGpReg == RegIndex.Invalid)
                        temporaryGpReg = FindTemporaryGpRegister(cc);

                    cc.AllocGPVar(r.vdata, RegisterMask.FromIndex(temporaryGpReg),
                      VariableAlloc.Register | VariableAlloc.Read);
                }
            }

            {
                Operand[] operands = { _target };
                cc.TranslateOperands(operands);
                _target = operands[0];
            }

            // --------------------------------------------------------------------------
            // STEP 8:
            //
            // Spill all preserved variables.
            // --------------------------------------------------------------------------

            preserved = Prototype.PreservedGP;
            for (int i = 0; i < (int)RegNum.GP; i++)
            {
                RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                VarData vdata = cc.State.GP[i];
                if (vdata != null && (preserved & mask) == RegisterMask.Zero)
                {
                    VarCallRecord rec = (VarCallRecord)(vdata.Temp);
                    if (rec != null && (rec.OutCount != 0 || (rec.Flags & VarCallFlags.UnuseAfterUse) != 0 || vdata.LastEmittable == this))
                        cc.UnuseVar(vdata, VariableState.Unused);
                    else
                        cc.SpillGPVar(vdata);
                }
            }

            preserved = Prototype.PreservedMM;
            for (int i = 0; i < (int)RegNum.MM; i++)
            {
                RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                VarData vdata = cc.State.MM[i];
                if (vdata != null && (preserved & mask) == RegisterMask.Zero)
                {
                    VarCallRecord rec = (VarCallRecord)(vdata.Temp);
                    if (rec != null && (rec.OutCount != 0 || vdata.LastEmittable == this))
                        cc.UnuseVar(vdata, VariableState.Unused);
                    else
                        cc.SpillMMVar(vdata);
                }
            }

            preserved = Prototype.PreservedXMM;
            for (int i = 0; i < (int)RegNum.XMM; i++)
            {
                RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                VarData vdata = cc.State.XMM[i];
                if (vdata != null && (preserved & mask) == RegisterMask.Zero)
                {
                    VarCallRecord rec = (VarCallRecord)(vdata.Temp);
                    if (rec != null && (rec.OutCount != 0 || vdata.LastEmittable == this))
                        cc.UnuseVar(vdata, VariableState.Unused);
                    else
                        cc.SpillXMMVar(vdata);
                }
            }

            // --------------------------------------------------------------------------
            // STEP 9:
            //
            // Emit CALL instruction.
            // --------------------------------------------------------------------------

            compiler.Emit(InstructionCode.Call, _target);

            // Restore the stack offset.
            if (Prototype.CalleePopsStack)
            {
                int s = Prototype.ArgumentsStackSize;
                if (s != 0)
                    compiler.Emit(InstructionCode.Sub, Register.nsp, (Imm)s);
            }

            // --------------------------------------------------------------------------
            // STEP 10:
            //
            // Prepare others for return value(s) and cleanup.
            // --------------------------------------------------------------------------

            // Clear temp data, see AsmJit::VarData::temp why it's needed.
            for (int i = 0; i < variablesCount; i++)
            {
                VarCallRecord rec = _variables[i];
                VarData vdata = rec.vdata;

                if ((rec.Flags & (VarCallFlags.OUT_EAX | VarCallFlags.OUT_EDX)) != 0)
                {
                    if ((VariableInfo.GetVariableInfo(vdata.Type).Class & VariableClass.GP) != 0)
                    {
                        cc.AllocGPVar(vdata, (rec.Flags & VarCallFlags.OUT_EAX) != 0
                          ? RegisterMask.FromIndex(RegIndex.Eax)
                          : RegisterMask.FromIndex(RegIndex.Edx),
                          VariableAlloc.Register | VariableAlloc.Write);
                        vdata.Changed = true;
                    }
                }

                if ((rec.Flags & (VarCallFlags.OUT_MM0)) != 0)
                {
                    if ((VariableInfo.GetVariableInfo(vdata.Type).Class & VariableClass.MM) != 0)
                    {
                        cc.AllocMMVar(vdata, RegisterMask.FromIndex(RegIndex.Mm0),
                          VariableAlloc.Register | VariableAlloc.Write);
                        vdata.Changed = true;
                    }
                }

                if ((rec.Flags & (VarCallFlags.OUT_XMM0 | VarCallFlags.OUT_XMM1)) != 0)
                {
                    if ((VariableInfo.GetVariableInfo(vdata.Type).Class & VariableClass.XMM) != 0)
                    {
                        cc.AllocXMMVar(vdata, RegisterMask.FromIndex((rec.Flags & VarCallFlags.OUT_XMM0) != 0
                          ? RegIndex.Xmm0
                          : RegIndex.Xmm1),
                          VariableAlloc.Register | VariableAlloc.Write);
                        vdata.Changed = true;
                    }
                }

                if ((rec.Flags & (VarCallFlags.OUT_ST0 | VarCallFlags.OUT_ST1)) != 0)
                {
                    if ((VariableInfo.GetVariableInfo(vdata.Type).Class & VariableClass.XMM) != 0)
                    {
                        Mem mem = cc.GetVarMem(vdata);
                        cc.UnuseVar(vdata, VariableState.Memory);

                        switch (vdata.Type)
                        {
                        case VariableType.XMM_1F:
                        case VariableType.XMM_4F:
                            {
                                //mem.Size = 4;
                                if (mem.Size != 4)
                                    throw new NotImplementedException("Size is now an immutable property.");

                                compiler.Emit(InstructionCode.Fstp, mem);
                                break;
                            }
                        case VariableType.XMM_1D:
                        case VariableType.XMM_2D:
                            {
                                //mem.Size = 8;
                                if (mem.Size != 4)
                                    throw new NotImplementedException("Size is now an immutable property.");

                                compiler.Emit(InstructionCode.Fstp, mem);
                                break;
                            }
                        default:
                            {
                                compiler.Comment("*** WARNING: Can't convert float return value to untyped XMM\n");
                                break;
                            }
                        }
                    }
                }

                // Cleanup.
                vdata.Temp = null;
            }

            for (int i = 0; i < variablesCount; i++)
            {
                cc.UnuseVarOnEndOfScope(this, _variables[i]);
            }

            return Next;
        }

        protected override bool TryUnuseVarImpl(VarData v)
        {
            for (int i = 0; i < _variables.Length; i++)
            {
                if (_variables[i].vdata == v)
                {
                    _variables[i].Flags |= VarCallFlags.UnuseAfterUse;
                    return true;
                }
            }

            return false;
        }

        private void MoveAllocatedVariableToStack(CompilerContext cc, VarData vdata, FunctionPrototype.Argument argType)
        {
            Contract.Requires(cc != null);
            Contract.Requires(vdata != null);
            Contract.Requires(argType != null);

            if (argType._registerIndex != RegIndex.Invalid)
                throw new ArgumentException();
            if (vdata.RegisterIndex == RegIndex.Invalid)
                throw new ArgumentException();

            Compiler compiler = cc.Compiler;

            RegIndex src = vdata.RegisterIndex;
            Mem dst = Mem.ptr(Register.nsp, -IntPtr.Size + argType._stackOffset);

            switch (vdata.Type)
            {
            case VariableType.GPD:
                switch (argType._variableType)
                {
                case VariableType.GPD:
                    compiler.Emit(InstructionCode.Mov, dst, Register.gpd(src));
                    return;

                case VariableType.GPQ:
                case VariableType.MM:
                    if (!Util.IsX64)
                        throw new NotSupportedException();

                    compiler.Emit(InstructionCode.Mov, dst, Register.gpq(src));
                    return;
                }
                break;

            case VariableType.GPQ:
                if (!Util.IsX64)
                    throw new NotSupportedException();

                switch (argType._variableType)
                {
                case VariableType.GPD:
                    compiler.Emit(InstructionCode.Mov, dst, Register.gpd(src));
                    return;
                case VariableType.GPQ:
                    compiler.Emit(InstructionCode.Mov, dst, Register.gpq(src));
                    return;
                case VariableType.MM:
                    compiler.Emit(InstructionCode.Movq, dst, Register.gpq(src));
                    return;
                }
                break;

            case VariableType.MM:
                switch (argType._variableType)
                {
                case VariableType.GPD:
                case VariableType.X87_1F:
                case VariableType.XMM_1F:
                    compiler.Emit(InstructionCode.Movd, dst, Register.mm(src));
                    return;
                case VariableType.GPQ:
                case VariableType.MM:
                case VariableType.X87_1D:
                case VariableType.XMM_1D:
                    compiler.Emit(InstructionCode.Movq, dst, Register.mm(src));
                    return;
                }
                break;

            // We allow incompatible types here, because the called can convert them
            // to correct format before function is called.

            case VariableType.XMM:
            case VariableType.XMM_4F:
            case VariableType.XMM_2D:
                switch (argType._variableType)
                {
                case VariableType.XMM:
                    compiler.Emit(InstructionCode.Movdqu, dst, Register.xmm(src));
                    return;
                case VariableType.XMM_1F:
                case VariableType.XMM_4F:
                    compiler.Emit(InstructionCode.Movups, dst, Register.xmm(src));
                    return;
                case VariableType.XMM_1D:
                case VariableType.XMM_2D:
                    compiler.Emit(InstructionCode.Movupd, dst, Register.xmm(src));
                    return;
                }
                break;

            case VariableType.XMM_1F:
                switch (argType._variableType)
                {
                case VariableType.X87_1F:
                case VariableType.XMM:
                case VariableType.XMM_1F:
                case VariableType.XMM_4F:
                case VariableType.XMM_1D:
                case VariableType.XMM_2D:
                    compiler.Emit(InstructionCode.Movss, dst, Register.xmm(src));
                    return;
                }
                break;

            case VariableType.XMM_1D:
                switch (argType._variableType)
                {
                case VariableType.X87_1D:
                case VariableType.XMM:
                case VariableType.XMM_1F:
                case VariableType.XMM_4F:
                case VariableType.XMM_1D:
                case VariableType.XMM_2D:
                    compiler.Emit(InstructionCode.Movsd, dst, Register.xmm(src));
                    return;
                }
                break;
            }

            throw new ArgumentException("Incompatible argument.");
        }

        private RegIndex FindTemporaryXmmRegister(CompilerContext cc)
        {
            Contract.Requires(cc != null);

            RegisterMask passedXMM = Prototype.PassedXMM;
            RegIndex candidate = RegIndex.Invalid;

            // Find all registers used to pass function arguments. We shouldn't use these
            // if possible.
            for (int i = 0; i < RegNum.XMM; i++)
            {
                RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                if (cc.State.XMM[i] == null)
                {
                    // If this register is used to pass arguments to function, we will mark
                    // it and use it only if there is no other one.
                    if ((passedXMM & mask) != RegisterMask.Zero)
                        candidate = (RegIndex)i;
                    else
                        return (RegIndex)i;
                }
            }

            return candidate;
        }

        private void MoveSpilledVariableToStack(CompilerContext cc, VarData vdata, FunctionPrototype.Argument argType, RegIndex temporaryGpReg, RegIndex temporaryXmmReg)
        {
            Contract.Requires(cc != null);
            Contract.Requires(vdata != null);
            Contract.Requires(argType != null);

            if (argType._registerIndex != RegIndex.Invalid)
                throw new ArgumentException();
            if (vdata.RegisterIndex != RegIndex.Invalid)
                throw new ArgumentException();

            Compiler compiler = cc.Compiler;

            Mem src = cc.GetVarMem(vdata);
            Mem dst = Mem.ptr(Register.nsp, -IntPtr.Size + argType._stackOffset);

            switch (vdata.Type)
            {
            case VariableType.GPD:
                switch (argType._variableType)
                {
                case VariableType.GPD:
                    compiler.Emit(InstructionCode.Mov, Register.gpd(temporaryGpReg), src);
                    compiler.Emit(InstructionCode.Mov, dst, Register.gpd(temporaryGpReg));
                    return;

                case VariableType.GPQ:
                case VariableType.MM:
                    if (!Util.IsX64)
                        throw new NotSupportedException();

                    compiler.Emit(InstructionCode.Mov, Register.gpd(temporaryGpReg), src);
                    compiler.Emit(InstructionCode.Mov, dst, Register.gpq(temporaryGpReg));
                    return;

                default:
                    throw new CompilerException();
                }

            case VariableType.GPQ:
                if (!Util.IsX64)
                    throw new NotSupportedException();

                switch (argType._variableType)
                {
                case VariableType.GPD:
                    compiler.Emit(InstructionCode.Mov, Register.gpd(temporaryGpReg), src);
                    compiler.Emit(InstructionCode.Mov, dst, Register.gpd(temporaryGpReg));
                    return;

                case VariableType.GPQ:
                case VariableType.MM:
                    compiler.Emit(InstructionCode.Mov, Register.gpq(temporaryGpReg), src);
                    compiler.Emit(InstructionCode.Mov, dst, Register.gpq(temporaryGpReg));
                    return;

                default:
                    throw new CompilerException();
                }

            case VariableType.MM:
                switch (argType._variableType)
                {
                case VariableType.GPD:
                case VariableType.X87_1F:
                case VariableType.XMM_1F:
                    compiler.Emit(InstructionCode.Mov, Register.gpd(temporaryGpReg), src);
                    compiler.Emit(InstructionCode.Mov, dst, Register.gpd(temporaryGpReg));
                    return;

                case VariableType.GPQ:
                case VariableType.MM:
                case VariableType.X87_1D:
                case VariableType.XMM_1D:
                    // TODO
                    return;

                default:
                    throw new CompilerException();
                }

            // We allow incompatible types here, because the called can convert them
            // to correct format before function is called.

            case VariableType.XMM:
            case VariableType.XMM_4F:
            case VariableType.XMM_2D:
                switch (argType._variableType)
                {
                case VariableType.XMM:
                    compiler.Emit(InstructionCode.Movdqu, Register.xmm(temporaryXmmReg), src);
                    compiler.Emit(InstructionCode.Movdqu, dst, Register.xmm(temporaryXmmReg));
                    return;

                case VariableType.XMM_1F:
                case VariableType.XMM_4F:
                    compiler.Emit(InstructionCode.Movups, Register.xmm(temporaryXmmReg), src);
                    compiler.Emit(InstructionCode.Movups, dst, Register.xmm(temporaryXmmReg));
                    return;

                case VariableType.XMM_1D:
                case VariableType.XMM_2D:
                    compiler.Emit(InstructionCode.Movupd, Register.xmm(temporaryXmmReg), src);
                    compiler.Emit(InstructionCode.Movupd, dst, Register.xmm(temporaryXmmReg));
                    return;

                default:
                    throw new CompilerException();
                }

            case VariableType.XMM_1F:
                switch (argType._variableType)
                {
                case VariableType.X87_1F:
                case VariableType.XMM:
                case VariableType.XMM_1F:
                case VariableType.XMM_4F:
                case VariableType.XMM_1D:
                case VariableType.XMM_2D:
                    compiler.Emit(InstructionCode.Movss, Register.xmm(temporaryXmmReg), src);
                    compiler.Emit(InstructionCode.Movss, dst, Register.xmm(temporaryXmmReg));
                    return;

                default:
                    throw new CompilerException();
                }

            case VariableType.XMM_1D:
                switch (argType._variableType)
                {
                case VariableType.X87_1D:
                case VariableType.XMM:
                case VariableType.XMM_1F:
                case VariableType.XMM_4F:
                case VariableType.XMM_1D:
                case VariableType.XMM_2D:
                    compiler.Emit(InstructionCode.Movsd, Register.xmm(temporaryXmmReg), src);
                    compiler.Emit(InstructionCode.Movsd, dst, Register.xmm(temporaryXmmReg));
                    return;

                default:
                    throw new CompilerException();
                }

            default:
                throw new CompilerException("Incompatible argument.");
            }
        }

        private VarData GetOverlappingVariable(CompilerContext cc, FunctionPrototype.Argument argType)
        {
            Contract.Requires(cc != null);
            Contract.Requires(argType != null);

            if (argType._variableType == VariableType.Invalid)
                throw new ArgumentException();

            switch (argType._variableType)
            {
            case VariableType.GPD:
            case VariableType.GPQ:
                return cc.State.GP[(int)argType._registerIndex];

            case VariableType.MM:
                return cc.State.MM[(int)argType._registerIndex];

            case VariableType.XMM:
            case VariableType.XMM_1F:
            case VariableType.XMM_1D:
            case VariableType.XMM_4F:
            case VariableType.XMM_2D:
                return cc.State.XMM[(int)argType._registerIndex];

            default:
                throw new CompilerException();
            }
        }

        private void MoveSrcVariableToRegister(CompilerContext cc, VarData vdata, FunctionPrototype.Argument argType)
        {
            Contract.Requires(cc != null);
            Contract.Requires(vdata != null);
            Contract.Requires(argType != null);

            RegIndex dst = argType._registerIndex;
            RegIndex src = vdata.RegisterIndex;

            Compiler compiler = cc.Compiler;

            if (src != RegIndex.Invalid)
            {
                switch (argType._variableType)
                {
                case VariableType.GPD:
                    switch (vdata.Type)
                    {
                    case VariableType.GPD:
                        compiler.Emit(InstructionCode.Mov, Register.gpd(dst), Register.gpd(src));
                        return;

                    case VariableType.GPQ:
                        if (!Util.IsX64)
                            throw new NotSupportedException();

                        compiler.Emit(InstructionCode.Mov, Register.gpq(dst), Register.gpq(src));
                        return;

                    case VariableType.MM:
                        compiler.Emit(InstructionCode.Movd, Register.gpd(dst), Register.mm(src));
                        return;
                    }
                    break;

                case VariableType.GPQ:
                    if (!Util.IsX64)
                        throw new NotSupportedException();

                    switch (vdata.Type)
                    {
                    case VariableType.GPD:
                        compiler.Emit(InstructionCode.Mov, Register.gpd(dst), Register.gpd(src));
                        return;
                    case VariableType.GPQ:
                        compiler.Emit(InstructionCode.Mov, Register.gpq(dst), Register.gpq(src));
                        return;
                    case VariableType.MM:
                        compiler.Emit(InstructionCode.Movq, Register.gpq(dst), Register.mm(src));
                        return;
                    }
                    break;

                case VariableType.MM:
                    switch (vdata.Type)
                    {
                    case VariableType.GPD:
                        compiler.Emit(InstructionCode.Movd, Register.gpd(dst), Register.gpd(src));
                        return;

                    case VariableType.GPQ:
                        if (!Util.IsX64)
                            throw new NotSupportedException();

                        compiler.Emit(InstructionCode.Movq, Register.gpq(dst), Register.gpq(src));
                        return;

                    case VariableType.MM:
                        compiler.Emit(InstructionCode.Movq, Register.mm(dst), Register.mm(src));
                        return;
                    }
                    break;

                case VariableType.XMM:
                case VariableType.XMM_4F:
                case VariableType.XMM_2D:
                    switch (vdata.Type)
                    {
                    case VariableType.GPD:
                        compiler.Emit(InstructionCode.Movd, Register.xmm(dst), Register.gpd(src));
                        return;

                    case VariableType.GPQ:
                        if (!Util.IsX64)
                            throw new NotSupportedException();

                        compiler.Emit(InstructionCode.Movq, Register.xmm(dst), Register.gpq(src));
                        return;

                    case VariableType.MM:
                        compiler.Emit(InstructionCode.Movq, Register.xmm(dst), Register.mm(src));
                        return;
                    case VariableType.XMM:
                    case VariableType.XMM_1F:
                    case VariableType.XMM_4F:
                    case VariableType.XMM_1D:
                    case VariableType.XMM_2D:
                        compiler.Emit(InstructionCode.Movdqa, Register.xmm(dst), Register.xmm(src));
                        return;
                    }
                    break;

                case VariableType.XMM_1F:
                    switch (vdata.Type)
                    {
                    case VariableType.MM:
                        compiler.Emit(InstructionCode.Movq, Register.xmm(dst), Register.mm(src));
                        return;

                    case VariableType.XMM:
                        compiler.Emit(InstructionCode.Movdqa, Register.xmm(dst), Register.xmm(src));
                        return;
                    case VariableType.XMM_1F:
                    case VariableType.XMM_4F:
                        compiler.Emit(InstructionCode.Movss, Register.xmm(dst), Register.xmm(src));
                        return;
                    case VariableType.XMM_1D:
                    case VariableType.XMM_2D:
                        compiler.Emit(InstructionCode.Cvtsd2ss, Register.xmm(dst), Register.xmm(src));
                        return;
                    }
                    break;

                case VariableType.XMM_1D:
                    switch (vdata.Type)
                    {
                    case VariableType.MM:
                        compiler.Emit(InstructionCode.Movq, Register.xmm(dst), Register.mm(src));
                        return;

                    case VariableType.XMM:
                        compiler.Emit(InstructionCode.Movdqa, Register.xmm(dst), Register.xmm(src));
                        return;
                    case VariableType.XMM_1F:
                    case VariableType.XMM_4F:
                        compiler.Emit(InstructionCode.Cvtss2sd, Register.xmm(dst), Register.xmm(src));
                        return;
                    case VariableType.XMM_1D:
                    case VariableType.XMM_2D:
                        compiler.Emit(InstructionCode.Movsd, Register.xmm(dst), Register.xmm(src));
                        return;
                    }
                    break;
                }
            }
            else
            {
                Mem mem = cc.GetVarMem(vdata);

                switch (argType._variableType)
                {
                case VariableType.GPD:
                    switch (vdata.Type)
                    {
                    case VariableType.GPD:
                        compiler.Emit(InstructionCode.Mov, Register.gpd(dst), mem);
                        return;

                    case VariableType.GPQ:
                        if (!Util.IsX64)
                            throw new NotSupportedException();

                        compiler.Emit(InstructionCode.Mov, Register.gpq(dst), mem);
                        return;

                    case VariableType.MM:
                        compiler.Emit(InstructionCode.Movd, Register.gpd(dst), mem);
                        return;
                    }
                    break;

                case VariableType.GPQ:
                    if (!Util.IsX64)
                        throw new NotSupportedException();

                    switch (vdata.Type)
                    {
                    case VariableType.GPD:
                        compiler.Emit(InstructionCode.Mov, Register.gpd(dst), mem);
                        return;
                    case VariableType.GPQ:
                        compiler.Emit(InstructionCode.Mov, Register.gpq(dst), mem);
                        return;
                    case VariableType.MM:
                        compiler.Emit(InstructionCode.Movq, Register.gpq(dst), mem);
                        return;
                    }
                    break;

                case VariableType.MM:
                    switch (vdata.Type)
                    {
                    case VariableType.GPD:
                        compiler.Emit(InstructionCode.Movd, Register.gpd(dst), mem);
                        return;

                    case VariableType.GPQ:
                        if (!Util.IsX64)
                            throw new NotSupportedException();

                        compiler.Emit(InstructionCode.Movq, Register.gpq(dst), mem);
                        return;

                    case VariableType.MM:
                        compiler.Emit(InstructionCode.Movq, Register.mm(dst), mem);
                        return;
                    }
                    break;

                case VariableType.XMM:
                case VariableType.XMM_4F:
                case VariableType.XMM_2D:
                    switch (vdata.Type)
                    {
                    case VariableType.GPD:
                        compiler.Emit(InstructionCode.Movd, Register.xmm(dst), mem);
                        return;

                    case VariableType.GPQ:
                        if (!Util.IsX64)
                            throw new NotSupportedException();

                        compiler.Emit(InstructionCode.Movq, Register.xmm(dst), mem);
                        return;

                    case VariableType.MM:
                        compiler.Emit(InstructionCode.Movq, Register.xmm(dst), mem);
                        return;
                    case VariableType.XMM:
                    case VariableType.XMM_1F:
                    case VariableType.XMM_4F:
                    case VariableType.XMM_1D:
                    case VariableType.XMM_2D:
                        compiler.Emit(InstructionCode.Movdqa, Register.xmm(dst), mem);
                        return;
                    }
                    break;

                case VariableType.XMM_1F:
                    switch (vdata.Type)
                    {
                    case VariableType.MM:
                        compiler.Emit(InstructionCode.Movq, Register.xmm(dst), mem);
                        return;

                    case VariableType.XMM:
                        compiler.Emit(InstructionCode.Movdqa, Register.xmm(dst), mem);
                        return;
                    case VariableType.XMM_1F:
                    case VariableType.XMM_4F:
                        compiler.Emit(InstructionCode.Movss, Register.xmm(dst), mem);
                        return;
                    case VariableType.XMM_1D:
                    case VariableType.XMM_2D:
                        compiler.Emit(InstructionCode.Cvtsd2ss, Register.xmm(dst), mem);
                        return;
                    }
                    break;

                case VariableType.XMM_1D:
                    switch (vdata.Type)
                    {
                    case VariableType.MM:
                        compiler.Emit(InstructionCode.Movq, Register.xmm(dst), mem);
                        return;

                    case VariableType.XMM:
                        compiler.Emit(InstructionCode.Movdqa, Register.xmm(dst), mem);
                        return;
                    case VariableType.XMM_1F:
                    case VariableType.XMM_4F:
                        compiler.Emit(InstructionCode.Cvtss2sd, Register.xmm(dst), mem);
                        return;
                    case VariableType.XMM_1D:
                    case VariableType.XMM_2D:
                        compiler.Emit(InstructionCode.Movsd, Register.xmm(dst), mem);
                        return;
                    }
                    break;
                }
            }

            throw new ArgumentException("Incompatible argument.");
        }

        private RegIndex FindTemporaryGpRegister(CompilerContext cc)
        {
            RegisterMask passedGP = Prototype.PassedGP;
            RegIndex candidate = RegIndex.Invalid;

            // Find all registers used to pass function arguments. We shouldn't use these
            // if possible.
            for (int i = 0; i < (int)RegNum.GP; i++)
            {
                RegisterMask mask = RegisterMask.FromIndex((RegIndex)i);
                if (cc.State.GP[i] == null)
                {
                    // If this register is used to pass arguments to function, we will mark
                    // it and use it only if there is no other one.
                    if ((passedGP & mask) != RegisterMask.Zero)
                        candidate = (RegIndex)i;
                    else
                        return (RegIndex)i;
                }
            }

            return candidate;
        }
    }
}
