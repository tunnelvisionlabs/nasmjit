namespace AsmJitNet
{
    using System;
    using Debug = System.Diagnostics.Debug;
    using System.Diagnostics.Contracts;

    public class Call : Emittable
    {
        private readonly Function _caller;
        private Operand _target;
        private readonly Operand[] _ret = new Operand[2];
        private Operand[] _args;
        private FunctionPrototype _functionPrototype;
        /// <summary>
        /// Mask of GP registers used as function arguments
        /// </summary>
        private int _gpParams;
        /// <summary>
        /// Mask of MM registers used as function arguments
        /// </summary>
        private int _mmParams;
        /// <summary>
        /// Mask of XMM registers used as function arguments
        /// </summary>
        private int _xmmParams;
        /// <summary>
        /// Variables (extracted from operands)
        /// </summary>
        private VarCallRecord[] _variables;

        private readonly VarCallRecord[] _argumentToVarRecord = new VarCallRecord[32];

        public Call(Compiler compiler, Function caller, Operand target)
            : base(compiler)
        {
            _caller = caller;
            _target = target;
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
                return _functionPrototype;
            }
        }

        public void SetPrototype(CallingConvention callingConvention, Type delegateType)
        {
            if (delegateType == null)
                throw new ArgumentNullException("delegateType");

            if (delegateType == typeof(Action))
            {
                SetPrototype(callingConvention, new VariableType[0], VariableType.Invalid);
            }

            if (!delegateType.IsGenericType)
                throw new ArgumentException();

            VariableType[] arguments = null;
            VariableType returnValue = VariableType.Invalid;
            Type genericType = delegateType.GetGenericTypeDefinition();
            if (genericType.FullName.StartsWith("System.Action`"))
            {
                arguments = Array.ConvertAll(delegateType.GetGenericArguments(), Compiler.TypeToId);
            }
            else if (genericType.FullName.StartsWith("System.Func`"))
            {
                Type[] typeArguments = delegateType.GetGenericArguments();
                returnValue = Compiler.TypeToId(typeArguments[typeArguments.Length - 1]);
                Array.Resize(ref typeArguments, typeArguments.Length - 1);
                arguments = Array.ConvertAll(typeArguments, Compiler.TypeToId);
            }
            else
            {
                throw new ArgumentException();
            }

            SetPrototype(callingConvention, arguments, returnValue);
        }

        public void SetPrototype(CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            Contract.Requires(arguments != null);

            _functionPrototype = new FunctionPrototype(callingConvention, arguments, returnValue);
            if (arguments != null && arguments.Length > 0)
                _args = new Operand[arguments.Length];
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
            _ret[0] = first;
            _ret[1] = second;
            return true;
        }

        public override void Prepare(CompilerContext cc)
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
                        _gpParams |= (1 << (int)fArg._registerIndex);
                        break;
                    case VariableType.MM:
                        _mmParams |= (1 << (int)fArg._registerIndex);
                        break;
                    case VariableType.XMM:
                    case VariableType.XMM_1F:
                    case VariableType.XMM_4F:
                    case VariableType.XMM_1D:
                    case VariableType.XMM_2D:
                        _xmmParams |= (1 << (int)fArg._registerIndex);
                        break;
                    default:
                        Debug.Assert(false);
                        return;
                    }
                }
            }

            // Call address.
            operandsCount++;

            // First return value.
            if (_ret[0] != null && !_ret[0].IsNone)
                operandsCount++;

            // Second return value.
            if (_ret[1] != null && !_ret[1].IsNone)
                operandsCount++;

            for (i = 0; i < operandsCount; i++)
            {
                Operand o = (i < argumentsCount)
                  ? (_args[i])
                  : (i == argumentsCount ? _target : _ret[i - argumentsCount - 1]);

                if (o.IsVar)
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
                else if (o.IsMem)
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
                    else if (((int)((Mem)o).Base & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = Compiler.GetVarData((int)((Mem)o).Base);
                        Debug.Assert(vdata != null);

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

            _variables = new VarCallRecord[variablesCount];

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

            //#define __GET_VARIABLE(__vardata__) \
            //  { \
            //    VarData* _candidate = __vardata__; \
            //    \
            //    for (var = cur; ;) \
            //    { \
            //      if (var == _variables) \
            //      { \
            //        var = cur++; \
            //        var->vdata = _candidate; \
            //        break; \
            //      } \
            //      \
            //      var--; \
            //      \
            //      if (var->vdata == _candidate) \
            //      { \
            //        break; \
            //      } \
            //    } \
            //    \
            //    ASMJIT_ASSERT(var != NULL); \
            //  }
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
                    Debug.Assert(var != null);
                };

            for (i = 0; i < operandsCount; i++)
            {
                Operand o = (i < argumentsCount)
                  ? (_args[i])
                  : (i == argumentsCount ? _target : _ret[i - argumentsCount - 1]);

                if (o.IsVar)
                {
                    VarData vdata = Compiler.GetVarData(o.Id);
                    Debug.Assert(vdata != null);

                    __GET_VARIABLE(vdata);
                    _argumentToVarRecord[i] = var;

                    if (i < argumentsCount)
                    {
                        FunctionPrototype.Argument fArg = Prototype.Arguments[i];

                        if (fArg._registerIndex != RegIndex.Invalid)
                        {
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
                                Debug.Assert(false);
                                return;
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
                        var.Flags |= VarCallFlags.CALL_OPERAND;
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
#if ASMJIT_X86
                            if (i == argumentsCount + 1)
                                var.Flags |= VarCallFlags.OUT_ST0;
                            else
                                var.Flags |= VarCallFlags.OUT_ST1;
#else
                            if (i == argumentsCount + 1)
                                var.Flags |= VarCallFlags.OUT_XMM0;
                            else
                                var.Flags |= VarCallFlags.OUT_XMM1;
#endif
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
#if ASMJIT_X86
                            if (i == argumentsCount + 1)
                                var.Flags |= VarCallFlags.OUT_ST0;
                            else
                                var.Flags |= VarCallFlags.OUT_ST1;
#else
                            if (i == argumentsCount + 1)
                                var.Flags |= VarCallFlags.OUT_XMM0;
                            else
                                var.Flags |= VarCallFlags.OUT_XMM1;
#endif
                            break;

                        default:
                            Debug.Assert(false);
                            return;
                        }

                        vdata.RegisterWriteCount++;
                    }
                }
                else if (o.IsMem)
                {
                    Debug.Assert(i == argumentsCount);

                    if ((o.Id & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = Compiler.GetVarData(o.Id);
                        Debug.Assert(vdata != null);

                        vdata.MemoryReadCount++;
                    }
                    else if (((int)((Mem)o).Base & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = Compiler.GetVarData((int)((Mem)o).Base);
                        Debug.Assert(vdata != null);

                        vdata.RegisterReadCount++;

                        __GET_VARIABLE(vdata);
                        var.Flags |= VarCallFlags.CALL_OPERAND;
                    }

                    if (((int)((Mem)o).Index & Operand.OperandIdTypeMask) == Operand.OperandIdTypeVar)
                    {
                        VarData vdata = Compiler.GetVarData((int)((Mem)o).Index);
                        Debug.Assert(vdata != null);

                        vdata.RegisterReadCount++;

                        __GET_VARIABLE(vdata);
                        var.Flags |= VarCallFlags.CALL_OPERAND;
                    }
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

        public override void Translate(CompilerContext cc)
        {
            int i;
            int preserved;
            int mask;

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
            // to their work offsets -> getSpillCandidate never return the variable
            // used this instruction.
            for (i = 0; i < variablesCount; i++)
            {
                _variables[i].vdata.WorkOffset = offset;

                // Init back-reference to VarCallRecord.
                _variables[i].vdata.Temp = _variables[i];
            }


            // STEP 1:
            //
            // Spill variables which are not used by the function call and have to
            // be destroyed. These registers may be used by callee.

            preserved = Prototype.PreservedGP;
            for (i = 0, mask = 1; i < (int)RegNum.GP; i++, mask <<= 1)
            {
                VarData vdata = cc.State.GP[i];
                if (vdata != null && vdata.WorkOffset != offset && (preserved & mask) == 0)
                {
                    cc.SpillGPVar(vdata);
                }
            }

            preserved = Prototype.PreservedMM;
            for (i = 0, mask = 1; i < (int)RegNum.MM; i++, mask <<= 1)
            {
                VarData vdata = cc.State.MM[i];
                if (vdata != null && vdata.WorkOffset != offset && (preserved & mask) == 0)
                {
                    cc.SpillMMVar(vdata);
                }
            }

            preserved = Prototype.PreservedXMM;
            for (i = 0, mask = 1; i < (int)RegNum.XMM; i++, mask <<= 1)
            {
                VarData vdata = cc.State.XMM[i];
                if (vdata != null && vdata.WorkOffset != offset && (preserved & mask) == 0)
                {
                    cc.SpillXMMVar(vdata);
                }
            }


            // STEP 2:
            //
            // Move all arguments to the stack which all already in registers.

            for (i = 0; i < argumentsCount; i++)
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

                    if (vdata.RegisterIndex != RegIndex.Invalid)
                    {
                        MoveAllocatedVariableToStack(cc, vdata, argType);

                        rec.InDone++;
                        processed[i] = true;
                    }
                }
            }


            // STEP 3:
            //
            // Spill all non-preserved variables we moved to stack in STEP #2.

            for (i = 0; i < argumentsCount; i++)
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
                            if ((Prototype.PreservedGP & (1 << (int)vdata.RegisterIndex)) == 0)
                                cc.SpillGPVar(vdata);
                            break;
                        case VariableType.MM:
                            if ((Prototype.PreservedMM & (1 << (int)vdata.RegisterIndex)) == 0)
                                cc.SpillMMVar(vdata);
                            break;
                        case VariableType.XMM:
                        case VariableType.XMM_1F:
                        case VariableType.XMM_1D:
                        case VariableType.XMM_4F:
                        case VariableType.XMM_2D:
                            if ((Prototype.PreservedXMM & (1 << (int)vdata.RegisterIndex)) == 0)
                                cc.SpillXMMVar(vdata);
                            break;
                        }
                    }
                }
            }


            // STEP 4:
            //
            // Get temporary register that we can use to pass input function arguments.
            // Now it's safe to do, because we spilled some variables (I hope:)).

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


            // STEP 5:
            //
            // Move all remaining arguments to the stack (we can use temporary register).
            // or allocate it to the primary register. Also move immediates.

            for (i = 0; i < argumentsCount; i++)
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

                    MoveSpilledVariableToStack(cc, vdata, argType, temporaryGpReg, temporaryXmmReg);

                    rec.InDone++;
                    processed[i] = true;
                }
                else if (operand.IsImm)
                {
                    // TODO.
                    throw new NotImplementedException();
                }
            }


            // STEP 6:
            //
            // Allocate arguments to registers.

            bool didWork;

            do
            {
                didWork = false;

                for (i = 0; i < argumentsCount; i++)
                {
                    if (processed[i])
                        continue;

                    VarCallRecord rsrc = _argumentToVarRecord[i];

                    Operand osrc = _args[i];
                    Debug.Assert(osrc.IsVar);
                    VarData vsrc = compiler.GetVarData(osrc.Id);

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

                        if (rdst.InDone >= rdst.InCount)
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

                            // Emit xchg instead of spill/alloc if possible (only GP registers).
                            if (x != InvalidValue && (VariableInfo.GetVariableInfo(vdst.Type).Class & VariableClass.GP) != 0)
                            {
                                FunctionPrototype.Argument dstArgType = targs[x];
                                if (VariableInfo.GetVariableInfo(dstArgType._variableType).Class == VariableInfo.GetVariableInfo(srcArgType._variableType).Class)
                                {
                                    RegIndex dstIndex = vdst.RegisterIndex;
                                    RegIndex srcIndex = vsrc.RegisterIndex;

                                    if (srcIndex == dstArgType._registerIndex)
                                    {
                                        compiler.Emit(InstructionCode.Xchg, Register.gpn(dstIndex), Register.gpd(srcIndex));

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

                        rec.InDone++;
                        processed[i] = true;
                    }
                }
            } while (didWork);


            // STEP 7:
            //
            // Allocate operand used by CALL instruction.

            for (i = 0; i < variablesCount; i++)
            {
                VarCallRecord r = _variables[i];
                if ((r.Flags & VarCallFlags.CALL_OPERAND) != 0 &&
                    (r.vdata.RegisterIndex == RegIndex.Invalid))
                {
                    if (temporaryGpReg == RegIndex.Invalid)
                        temporaryGpReg = FindTemporaryGpRegister(cc);

                    cc.AllocGPVar(r.vdata, (RegIndex)temporaryGpReg,
                      VariableAlloc.Register | VariableAlloc.Read);
                }
            }

            {
                Operand[] operands = { _target };
                cc.TranslateOperands(operands);
                _target = operands[0];
            }

            // STEP 8:
            //
            // Spill all preserved variables.

            preserved = Prototype.PreservedGP;
            for (i = 0, mask = 1; i < (int)RegNum.GP; i++, mask <<= 1)
            {
                VarData vdata = cc.State.GP[i];
                if (vdata != null && (preserved & mask) == 0)
                {
                    VarCallRecord rec = (VarCallRecord)(vdata.Temp);
                    if (rec != null && (rec.OutCount != 0 || vdata.LastEmittable == this))
                        cc.UnuseVar(vdata, VariableState.Unused);
                    else
                        cc.SpillGPVar(vdata);
                }
            }

            preserved = Prototype.PreservedMM;
            for (i = 0, mask = 1; i < (int)RegNum.MM; i++, mask <<= 1)
            {
                VarData vdata = cc.State.MM[i];
                if (vdata != null && (preserved & mask) == 0)
                {
                    VarCallRecord rec = (VarCallRecord)(vdata.Temp);
                    if (rec != null && (rec.OutCount != 0 || vdata.LastEmittable == this))
                        cc.UnuseVar(vdata, VariableState.Unused);
                    else
                        cc.SpillMMVar(vdata);
                }
            }

            preserved = Prototype.PreservedXMM;
            for (i = 0, mask = 1; i < (int)RegNum.XMM; i++, mask <<= 1)
            {
                VarData vdata = cc.State.XMM[i];
                if (vdata != null && (preserved & mask) == 0)
                {
                    VarCallRecord rec = (VarCallRecord)(vdata.Temp);
                    if (rec != null && (rec.OutCount != 0 || vdata.LastEmittable == this))
                        cc.UnuseVar(vdata, VariableState.Unused);
                    else
                        cc.SpillXMMVar(vdata);
                }
            }


            // STEP 9:
            //
            // Emit CALL instruction.

            compiler.Emit(InstructionCode.Call, _target);


            // Restore stack offset if needed. This is mainly for STDCALL calling
            // convention used by Windows. Standard 32-bit and 64-bit calling
            // conventions don't need this.
            if (Prototype.CalleePopsStack)
            {
                int s = Prototype.ArgumentsStackSize;
                if (s != 0)
                    compiler.Emit(InstructionCode.Sub, Register.nsp, (Imm)s);
            }

            // STEP 10:
            //
            // Prepare others for return value(s) and cleanup.

            // Clear temp data, see AsmJit::VarData::temp why it's needed.
            for (i = 0; i < variablesCount; i++)
            {
                VarCallRecord rec = _variables[i];
                VarData vdata = rec.vdata;

                if ((rec.Flags & (VarCallFlags.OUT_EAX | VarCallFlags.OUT_EDX)) != 0)
                {
                    if ((VariableInfo.GetVariableInfo(vdata.Type).Class & VariableClass.GP) != 0)
                    {
                        cc.AllocGPVar(vdata, (rec.Flags & VarCallFlags.OUT_EAX) != 0
                          ? RegIndex.Eax
                          : RegIndex.Edx,
                          VariableAlloc.Register | VariableAlloc.Write);
                        vdata.Changed = true;
                    }
                }

                if ((rec.Flags & (VarCallFlags.OUT_MM0)) != 0)
                {
                    if ((VariableInfo.GetVariableInfo(vdata.Type).Class & VariableClass.MM) != 0)
                    {
                        cc.AllocMMVar(vdata, RegIndex.Mm0,
                          VariableAlloc.Register | VariableAlloc.Write);
                        vdata.Changed = true;
                    }
                }

                if ((rec.Flags & (VarCallFlags.OUT_XMM0 | VarCallFlags.OUT_XMM1)) != 0)
                {
                    if ((VariableInfo.GetVariableInfo(vdata.Type).Class & VariableClass.XMM) != 0)
                    {
                        cc.AllocXMMVar(vdata, (rec.Flags & VarCallFlags.OUT_XMM0) != 0
                          ? RegIndex.Xmm0
                          : RegIndex.Xmm1,
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

            for (i = 0; i < variablesCount; i++)
            {
                VarData v = _variables[i].vdata;
                cc.UnuseVarOnEndOfScope(this, _variables[i].vdata);
            }
        }

        private void MoveAllocatedVariableToStack(CompilerContext cc, VarData vdata, FunctionPrototype.Argument argType)
        {
            Debug.Assert(argType._registerIndex == RegIndex.Invalid);
            Debug.Assert(vdata.RegisterIndex != RegIndex.Invalid);

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
#if ASMJIT_X64
                case VariableType.GPQ:
                case VariableType.MM:
                    compiler.Emit(InstructionCode.Mov, dst, Register.gpq(src));
                    return;
#endif // ASMJIT_X64
                }
                break;

#if ASMJIT_X64
            case VariableType.GPQ:
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
#endif // ASMJIT_X64

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

            compiler.Error = Errors.IncompatibleArgument;
        }

        private RegIndex FindTemporaryXmmRegister(CompilerContext cc)
        {
            int i;
            int mask;

            int passedXMM = Prototype.PassedXMM;
            RegIndex candidate = RegIndex.Invalid;

            // Find all registers used to pass function arguments. We shouldn't use these
            // if possible.
            for (i = 0, mask = 1; i < (int)RegNum.XMM; i++, mask <<= 1)
            {
                if (cc.State.XMM[i] == null)
                {
                    // If this register is used to pass arguments to function, we will mark
                    // it and use it only if there is no other one.
                    if ((passedXMM & mask) != 0)
                        candidate = (RegIndex)i;
                    else
                        return (RegIndex)i;
                }
            }

            return candidate;
        }

        private void MoveSpilledVariableToStack(CompilerContext cc, VarData vdata, FunctionPrototype.Argument argType, RegIndex temporaryGpReg, RegIndex temporaryXmmReg)
        {
            Debug.Assert(argType._registerIndex == RegIndex.Invalid);
            Debug.Assert(vdata.RegisterIndex == RegIndex.Invalid);

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
#if ASMJIT_X64
                case VariableType.GPQ:
                case VariableType.MM:
                    compiler.Emit(InstructionCode.Mov, Register.gpd(temporaryGpReg), src);
                    compiler.Emit(InstructionCode.Mov, dst, Register.gpq(temporaryGpReg));
                    return;
#endif // ASMJIT_X64
                }
                break;

#if ASMJIT_X64
            case VariableType.GPQ:
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
                }
                break;
#endif // ASMJIT_X64

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
                    compiler.Emit(InstructionCode.Movss, Register.xmm(temporaryXmmReg), src);
                    compiler.Emit(InstructionCode.Movss, dst, Register.xmm(temporaryXmmReg));
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
                    compiler.Emit(InstructionCode.Movsd, Register.xmm(temporaryXmmReg), src);
                    compiler.Emit(InstructionCode.Movsd, dst, Register.xmm(temporaryXmmReg));
                    return;
                }
                break;
            }

            compiler.Error = Errors.IncompatibleArgument;
        }

        private VarData GetOverlappingVariable(CompilerContext cc, FunctionPrototype.Argument argType)
        {
            Debug.Assert(argType._variableType != VariableType.Invalid);

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
            }

            return null;
        }

        private void MoveSrcVariableToRegister(CompilerContext cc, VarData vdata, FunctionPrototype.Argument argType)
        {
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
#if ASMJIT_X64
                    case VariableType.GPQ:
#endif // ASMJIT_X64
                        compiler.Emit(InstructionCode.Mov, Register.gpd(dst), Register.gpd(src));
                        return;
                    case VariableType.MM:
                        compiler.Emit(InstructionCode.Movd, Register.gpd(dst), Register.mm(src));
                        return;
                    }
                    break;

#if ASMJIT_X64
                case VariableType.GPQ:
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
#endif // ASMJIT_X64

                case VariableType.MM:
                    switch (vdata.Type)
                    {
                    case VariableType.GPD:
                        compiler.Emit(InstructionCode.Movd, Register.gpd(dst), Register.gpd(src));
                        return;
#if ASMJIT_X64
                    case VariableType.GPQ:
                        compiler.Emit(InstructionCode.Movq, Register.gpq(dst), Register.gpq(src));
                        return;
#endif // ASMJIT_X64
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
#if ASMJIT_X64
                    case VariableType.GPQ:
                        compiler.Emit(InstructionCode.Movq, Register.xmm(dst), Register.gpq(src));
                        return;
#endif // ASMJIT_X64
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
#if ASMJIT_X64
                    case VariableType.GPQ:
#endif // ASMJIT_X64
                        compiler.Emit(InstructionCode.Mov, Register.gpd(dst), mem);
                        return;
                    case VariableType.MM:
                        compiler.Emit(InstructionCode.Movd, Register.gpd(dst), mem);
                        return;
                    }
                    break;

#if ASMJIT_X64
                case VariableType.GPQ:
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
#endif // ASMJIT_X64

                case VariableType.MM:
                    switch (vdata.Type)
                    {
                    case VariableType.GPD:
                        compiler.Emit(InstructionCode.Movd, Register.gpd(dst), mem);
                        return;
#if ASMJIT_X64
                    case VariableType.GPQ:
                        compiler.Emit(InstructionCode.Movq, Register.gpq(dst), mem);
                        return;
#endif // ASMJIT_X64
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
#if ASMJIT_X64
                    case VariableType.GPQ:
                        compiler.Emit(InstructionCode.Movq, Register.xmm(dst), mem);
                        return;
#endif // ASMJIT_X64
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

            compiler.Error = Errors.IncompatibleArgument;
        }

        private RegIndex FindTemporaryGpRegister(CompilerContext cc)
        {
            int i;
            int mask;

            int passedGP = Prototype.PassedGP;
            RegIndex candidate = RegIndex.Invalid;

            // Find all registers used to pass function arguments. We shouldn't use these
            // if possible.
            for (i = 0, mask = 1; i < (int)RegNum.GP; i++, mask <<= 1)
            {
                if (cc.State.GP[i] == null)
                {
                    // If this register is used to pass arguments to function, we will mark
                    // it and use it only if there is no other one.
                    if ((passedGP & mask) != 0)
                        candidate = (RegIndex)i;
                    else
                        return (RegIndex)i;
                }
            }

            return candidate;
        }
    }
}
