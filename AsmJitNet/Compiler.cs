namespace AsmJitNet
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics.Contracts;
    using System.Linq;
    using Debug = System.Diagnostics.Debug;

    public class Compiler
        : IIntrinsicSupport<GPVar, X87Var, MMVar, XMMVar>
        , IX86IntrinsicSupport<GPVar>
        , IX87IntrinsicSupport<X87Var>
        , IMmIntrinsicSupport<MMVar>
        , IXmmIntrinsicSupport<XMMVar>
    {
        private CodeGenerator _codeGenerator;
        private Logger _logger;
        private int _error;
        private CompilerProperties _properties;
        private EmitOptions _emitOptions;
        private Emittable _first;
        private Emittable _last;
        private Emittable _current;
        private Function _function;
        private readonly List<Target> _targetData = new List<Target>();
        private readonly List<VarData> _varData = new List<VarData>();
        private int _varNameId;
        private CompilerContext _cc;

        public Compiler(CodeGenerator codeGenerator = null)
        {
            _codeGenerator = codeGenerator ?? CodeGenerator.Global;
        }

        public Logger Logger
        {
            get
            {
                return _logger;
            }

            set
            {
                _logger = value;
            }
        }

        public CodeGenerator CodeGenerator
        {
            get
            {
                return _codeGenerator;
            }
        }

        public EmitOptions EmitOptions
        {
            get
            {
                return _emitOptions;
            }

            set
            {
                _emitOptions = value;
            }
        }

        public Function Function
        {
            get
            {
                return _function;
            }
        }

        public int Error
        {
            get
            {
                return _error;
            }

            set
            {
                _error = value;
                if (_error != 0 && _logger != null && _logger.IsUsed)
                {
                    _logger.LogFormat("*** COMPILER ERROR: {0} ({1})." + Environment.NewLine, Errors.GetErrorCodeAsString(value), value);
                }
            }
        }

        public Emittable CurrentEmittable
        {
            get
            {
                return _current;
            }

            set
            {
                _current = value;
            }
        }

        internal IList<VarData> Variables
        {
            get
            {
                return _varData;
            }
        }

        public Function NewFunction(CallingConvention callingConvention, Type delegateType)
        {
            if (delegateType == null)
                throw new ArgumentNullException("delegateType");
            Contract.Ensures(Contract.Result<Function>() != null);
            Contract.EndContractBlock();

            if (delegateType == typeof(Action))
                return NewFunction(callingConvention, new VariableType[0], VariableType.Invalid);

            Function f = new Function(this, callingConvention, delegateType);
            _function = f;

            AddEmittable(f);
            Bind(f.EntryLabel);
            AddEmittable(f.Prolog);

            _varNameId = 0;
            f.CreateVariables();
            return f;
        }

        public Function NewFunction(CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            if (arguments == null)
                throw new ArgumentNullException("arguments");
            Contract.Ensures(Contract.Result<Function>() != null);
            Contract.EndContractBlock();

            if (_function != null)
                throw new InvalidOperationException();

            Function f = new Function(this, callingConvention, arguments, returnValue);
            _function = f;

            AddEmittable(f);
            Bind(f.EntryLabel);
            AddEmittable(f.Prolog);

            _varNameId = 0;
            f.CreateVariables();
            return f;
        }

        public Label NewLabel()
        {
            Contract.Ensures(Contract.Result<Label>() != null);

            Label label = new Label(_targetData.Count);
            _targetData.Add(new Target(this, label));
            return label;
        }

        public VarData NewVarData(string name, VariableType variableType, int size)
        {
            Contract.Ensures(Contract.Result<VarData>() != null);

            VarData varData = new VarData();

            if (name == null)
            {
                name = "var_" + _varNameId;
                _varNameId++;
            }

            varData.Scope = Function;
            varData.FirstEmittable = default(Emittable);
            varData.LastEmittable = default(Emittable);

            varData.Name = name;
            varData.Id = _varData.Count | Operand.OperandIdTypeVar;
            varData.Type = variableType;
            varData.Size = size;

            varData.HomeRegisterIndex = RegIndex.Invalid;
            varData.PreferredRegisterIndex = RegIndex.Invalid;

            varData.HomeMemoryData = null;

            varData.RegisterIndex = RegIndex.Invalid;
            varData.WorkOffset = Operand.InvalidValue;

            varData.NextActive = default(VarData);
            varData.PreviousActive = default(VarData);

            varData.Priority = 10;
            varData.Calculated = false;
            varData.IsRegArgument = false;
            varData.IsMemArgument = false;

            varData.State = VariableState.Unused;
            varData.Changed = false;
            varData.SaveOnUnuse = false;

            varData.RegisterReadCount = 0;
            varData.RegisterWriteCount = 0;
            varData.RegisterRWCount = 0;

            varData.RegisterGPBLoCount = 0;
            varData.RegisterGPBHiCount = 0;

            varData.MemoryReadCount = 0;
            varData.MemoryWriteCount = 0;
            varData.MemoryRWCount = 0;

            _varData.Add(varData);
            return varData;
        }

        public GPVar ArgGP(int index)
        {
            if (index < 0)
                throw new ArgumentOutOfRangeException("index");

            Function f = Function;
            if (f == null)
                throw new InvalidOperationException("No function.");

            FunctionPrototype prototype = f.Prototype;
            if (index >= prototype.Arguments.Length)
                throw new ArgumentException();

            VarData vdata = Function._argumentVariables[index];
            GPVar var = new GPVar(vdata.Id, vdata.Size, VariableInfo.GetVariableInfo(vdata.Type).RegisterType, vdata.Type);
            return var;
        }

        public GPVar NewGP(VariableType variableType = VariableType.GPN, string name = null)
        {
            if ((VariableInfo.GetVariableInfo(variableType).Class & VariableClass.GP) == 0)
                throw new ArgumentException();

            VarData varData = NewVarData(name, variableType, IntPtr.Size);
            GPVar var = new GPVar(varData.Id, varData.Size, VariableInfo.GetVariableInfo(varData.Type).RegisterType, varData.Type);
            return var;
        }

        public MMVar ArgMM(int index)
        {
            if (index < 0)
                throw new ArgumentOutOfRangeException("index");

            Function f = Function;
            if (f == null)
                throw new InvalidOperationException("No function.");

            FunctionPrototype prototype = f.Prototype;
            if (index >= prototype.Arguments.Length)
                throw new ArgumentException();

            VarData vdata = Function._argumentVariables[index];
            if (vdata.Size != 12)
                throw new NotSupportedException();

            MMVar var = new MMVar(vdata.Id, VariableInfo.GetVariableInfo(vdata.Type).RegisterType, vdata.Type);
            return var;
        }

        public MMVar NewMM(VariableType variableType = VariableType.MM, string name = null)
        {
            if ((VariableInfo.GetVariableInfo(variableType).Class & VariableClass.MM) == 0)
                throw new ArgumentException();

            VarData vdata = NewVarData(name, variableType, 8);
            if (vdata.Size != 8)
                throw new NotSupportedException();

            MMVar var = new MMVar(vdata.Id, VariableInfo.GetVariableInfo(vdata.Type).RegisterType, vdata.Type);
            return var;
        }

        public XMMVar ArgXMM(int index)
        {
            if (index < 0)
                throw new ArgumentOutOfRangeException("index");

            Function f = Function;
            if (f == null)
                throw new InvalidOperationException("No function.");

            FunctionPrototype prototype = f.Prototype;
            if (index >= prototype.Arguments.Length)
                throw new ArgumentException();

            VarData vdata = Function._argumentVariables[index];
            if (vdata.Size != 16)
                throw new NotSupportedException();

            XMMVar var = new XMMVar(vdata.Id, VariableInfo.GetVariableInfo(vdata.Type).RegisterType, vdata.Type);
            return var;
        }

        public XMMVar NewXMM(VariableType variableType = VariableType.XMM, string name = null)
        {
            if ((VariableInfo.GetVariableInfo(variableType).Class & VariableClass.XMM) == 0)
                throw new ArgumentException();

            VarData vdata = NewVarData(name, variableType, 16);
            if (vdata.Size != 16)
                throw new NotSupportedException();

            XMMVar var = new XMMVar(vdata.Id, VariableInfo.GetVariableInfo(vdata.Type).RegisterType, vdata.Type);
            return var;
        }

        public Function EndFunction()
        {
            if (_function == null)
                throw new InvalidOperationException("No function.");

            Function f = _function;
            Bind(f.ExitLabel);
            AddEmittable(f.Epilog);
            AddEmittable(f.End);

            _function.Finished = true;
            _function = null;
            return f;
        }

        public IntPtr Make()
        {
            Assembler a = new Assembler(_codeGenerator);
            a.Properties = _properties;
            a.Logger = _logger;

            Serialize(a);

            if (Error != 0)
                return IntPtr.Zero;

            if (a.Error != 0)
            {
                Error = a.Error;
                return IntPtr.Zero;
            }

            IntPtr result = a.Make();
            if (_logger != null && _logger.IsUsed)
            {
                _logger.LogFormat("*** COMPILER SUCCESS - Wrote {0} bytes, code: {1}, trampolines: {2}." + Environment.NewLine + Environment.NewLine,
                    a.CodeSize,
                    a.Offset,
                    a.TrampolineSize);
            }

            return result;
        }

        public void Serialize(Assembler a)
        {
            // context
            CompilerContext cc = new CompilerContext(this);

            Emittable start = _first;
            Emittable stop = null;

            // register all labels
            a.RegisterLabels(_targetData.Count);

            // make code
            while (true)
            {
                _cc = null;

                // ------------------------------------------------------------------------
                // Find a function.
                while (true)
                {
                    if (start == null)
                        return;

                    if (start.EmittableType == EmittableType.Function)
                        break;
                    else
                        start.Emit(a);

                    start = start.Next;
                }
                // ------------------------------------------------------------------------

                // ------------------------------------------------------------------------
                // Setup code generation context.
                Emittable cur;

                cc.Function = (Function)start;
                cc.Start = start;
                cc.Stop = stop = cc.Function.End;
                cc.ExtraBlock = stop.Previous;

                // Detect whether the function generation was finished.
                if (!cc.Function.Finished || cc.Function.End.Previous == null)
                {
                    Error = Errors.IncompleteFunction;
                    return;
                }
                // ------------------------------------------------------------------------

                // ------------------------------------------------------------------------
                // Step 1:
                // - Assign offset to each emittable.
                // - Extract variables from instructions.
                // - Prepare variables for register allocator, doing:
                //   - Update read/write statistics.
                //   - Find scope (first/last emittable) where variable is used.
                for (cur = start; ; cur = cur.Next)
                {
                    cur.Prepare(cc);
                    if (cur == stop)
                        break;
                }

                /*
                // Step 1.b:
                // - Add "VARIABLE_HINT_UNUSE" hint to the end of each variable scope.
                if (cc.Active != null)
                {
                    VarData vdata = cc.Active;

                    do
                    {
                        if (vdata.LastEmittable != null)
                        {
                            VariableHint e;
                            e = new VariableHint(this, vdata, VariableHintKind.Unuse, Emittable.InvalidValue);
                            e.Offset = vdata.LastEmittable.Offset;
                            AddEmittableAfter(e, vdata.LastEmittable);
                        }

                        vdata = vdata.NextActive;
                    } while (vdata != cc.Active);
                }
                */
                // ------------------------------------------------------------------------

                // We set compiler context also to Compiler so new emitted instructions
                // can call prepare() to itself.
                _cc = cc;

                // ------------------------------------------------------------------------
                // Step 2.a:
                // - Alloc registers.
                // - Translate special instructions (imul, cmpxchg8b, ...).
                Emittable prev = null;
                for (cur = start; ; prev = cur, cur = cur.Next)
                {
                    _current = prev;
                    cc.CurrentOffset = cur.Offset;
                    cur.Translate(cc);
                    if (cur == stop)
                        break;
                }

                // Step 2.b:
                // - Translate forward jumps.
                {
                    ForwardJumpData j = cc.ForwardJumps;
                    while (j != null)
                    {
                        cc.AssignState(j.State);
                        _current = j.Inst.Previous;
                        j.Inst.DoJump(cc);
                        j = j.Next;
                    }
                }

                // Step 2.c:
                // - Alloc memory operands (variables related).
                cc.AllocMemoryOperands();

                // Step 2.d:
                // - Emit function prolog.
                // - Emit function epilog.
                // - Patch memory operands (variables related).
                cc.Function.PreparePrologEpilog(cc);

                _current = cc.Function.Prolog;
                cc.Function.EmitProlog(cc);

                _current = cc.Function.Epilog;
                cc.Function.EmitEpilog(cc);

                _current = _last;
                cc.PatchMemoryOperands(start, stop);

                // Step 2.e:
                // - Dump function prototype and variable statistics if logging is enabled.
                if (_logger != null && _logger.IsUsed)
                {
                    cc.Function.DumpFunction(cc);
                }
                // ------------------------------------------------------------------------

                // ------------------------------------------------------------------------
                // Hack, need to register labels that was created by the Step 2.
                if (a.LabelData.Count < _targetData.Count)
                {
                    a.RegisterLabels(_targetData.Count - a.LabelData.Count);
                }

                Emittable extraBlock = cc.ExtraBlock;

                // Step 3:
                // - Emit instructions to Assembler stream.
                for (cur = start; ; cur = cur.Next)
                {
                    cur.Emit(a);
                    if (cur == extraBlock)
                        break;
                }
                // ------------------------------------------------------------------------

                // ------------------------------------------------------------------------
                // Step 4:
                // - Emit everything else (post action).
                for (cur = start; ; cur = cur.Next)
                {
                    cur.Post(a);
                    if (cur == extraBlock)
                        break;
                }
                // ------------------------------------------------------------------------

                start = extraBlock.Next;
                cc.Clear();
            }
        }

        public void AddEmittable(Emittable emittable)
        {
            Contract.Requires(emittable != null);

            if (emittable == null)
                throw new ArgumentNullException("emittable");
            if (emittable.Previous != null)
                throw new ArgumentException();
            if (emittable.Next != null)
                throw new ArgumentException();

            if (_current == null)
            {
                if (_first == null)
                {
                    _first = emittable;
                    _last = emittable;
                }
                else
                {
                    emittable.Next = _first;
                    _first.Previous = emittable;
                    _first = emittable;
                }
            }
            else
            {
                Emittable previous = _current;
                Emittable next = _current.Next;

                emittable.Previous = previous;
                emittable.Next = next;

                previous.Next = emittable;
                if (next != null)
                    next.Previous = emittable;
                else
                    _last = emittable;
            }

            _current = emittable;
        }

        public void AddEmittableAfter(Emittable emittable, Emittable reference)
        {
            if (emittable == null)
                throw new ArgumentNullException("emittable");
            if (reference == null)
                throw new ArgumentNullException("reference");
            if (emittable.Previous != null || emittable.Next != null)
                throw new ArgumentException("The emittable is already linked.");

            Emittable previous = reference;
            Emittable next = reference.Next;
            emittable.Previous = previous;
            emittable.Next = next;

            previous.Next = emittable;
            if (next != null)
                next.Previous = emittable;
            else
                _last = emittable;
        }

        public void Comment(string format, params object[] args)
        {
            Contract.Requires(format != null);
            Contract.Requires(args != null);

            string text = string.Format(format, args);
            AddEmittable(new Comment(this, text));
        }

        public void Bind(Label label)
        {
            int id = label.Id & Operand.OperandIdValueMask;
            //Debug.Assert(id != InvalidValue);
            Debug.Assert(id < _targetData.Count);

            AddEmittable(_targetData[id]);
        }

        public void Alloc(BaseVar var)
        {
            Vhint(var, VariableHintKind.Alloc, Operand.InvalidValue);
        }

        public void Alloc(BaseVar var, RegIndex regIndex)
        {
            Vhint(var, VariableHintKind.Alloc, (int)regIndex);
        }

        public void Alloc(BaseVar var, BaseReg reg)
        {
            Vhint(var, VariableHintKind.Alloc, (int)reg.RegisterIndex);
        }

        public void Spill(BaseVar var)
        {
            Vhint(var, VariableHintKind.Spill, Operand.InvalidValue);
        }

        private void Vhint(BaseVar var, VariableHintKind hintId, int hintValue)
        {
            if (var.Id == Operand.InvalidValue)
                return;

            VarData vdata = GetVarData(var.Id);
            Debug.Assert(vdata != null);

            VariableHint e = new VariableHint(this, vdata, hintId, hintValue);
            AddEmittable(e);
        }

        internal Target GetTarget(int id)
        {
            if ((id & Operand.OperandIdTypeMask) != Operand.OperandIdTypeLabel)
                throw new ArgumentException("The ID is not a label.");

            return _targetData[id & Operand.OperandIdValueMask];
        }

        #region Custom instructions

        public void Emit(InstructionCode code)
        {
            _emitInstruction(code);
        }

        public void Emit(InstructionCode code, Operand operand0)
        {
            if (operand0 == null)
                throw new ArgumentNullException("operand0");
            Contract.EndContractBlock();

            _emitInstruction(code, operand0);
        }

        public void Emit(InstructionCode code, Operand operand0, Operand operand1)
        {
            if (operand0 == null)
                throw new ArgumentNullException("operand0");
            if (operand1 == null)
                throw new ArgumentNullException("operand1");
            Contract.EndContractBlock();

            _emitInstruction(code, operand0, operand1);
        }

        public void Emit(InstructionCode code, Operand operand0, Operand operand1, Operand operand2)
        {
            if (operand0 == null)
                throw new ArgumentNullException("operand0");
            if (operand1 == null)
                throw new ArgumentNullException("operand1");
            if (operand2 == null)
                throw new ArgumentNullException("operand2");
            Contract.EndContractBlock();

            _emitInstruction(code, operand0, operand1, operand2);
        }

        public void Emit(InstructionCode code, Operand operand0, Operand operand1, Operand operand2, Operand operand3)
        {
            if (operand0 == null)
                throw new ArgumentNullException("operand0");
            if (operand1 == null)
                throw new ArgumentNullException("operand1");
            if (operand2 == null)
                throw new ArgumentNullException("operand2");
            if (operand3 == null)
                throw new ArgumentNullException("operand3");
            Contract.EndContractBlock();

            _emitInstruction(code, operand0, operand1, operand2, operand3);
        }

        public void Emit(InstructionCode code, Operand operand0, Operand operand1, Operand operand2, Operand operand3, Operand operand4)
        {
            if (operand0 == null)
                throw new ArgumentNullException("operand0");
            if (operand1 == null)
                throw new ArgumentNullException("operand1");
            if (operand2 == null)
                throw new ArgumentNullException("operand2");
            if (operand3 == null)
                throw new ArgumentNullException("operand3");
            if (operand4 == null)
                throw new ArgumentNullException("operand4");
            Contract.EndContractBlock();

            _emitInstruction(code, operand0, operand1, operand2, operand3, operand4);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(GPVar dst, CallingConvention callingConvention, Type delegateType)
        {
            Contract.Requires(dst != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall(dst, callingConvention, delegateType);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(GPVar dst, CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            Contract.Requires(dst != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall(dst, callingConvention, arguments, returnValue);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(Mem dst, CallingConvention callingConvention, Type delegateType)
        {
            Contract.Requires(dst != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall(dst, callingConvention, delegateType);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(Mem dst, CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            Contract.Requires(dst != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall(dst, callingConvention, arguments, returnValue);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(Imm dst, CallingConvention callingConvention, Type delegateType)
        {
            Contract.Requires(dst != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall(dst, callingConvention, delegateType);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(Imm dst, CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            Contract.Requires(dst != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall(dst, callingConvention, arguments, returnValue);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(IntPtr dst, CallingConvention callingConvention, Type delegateType)
        {
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall((Imm)dst, callingConvention, delegateType);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(IntPtr dst, CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall((Imm)dst, callingConvention, arguments, returnValue);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(Label label, CallingConvention callingConvention, Type delegateType)
        {
            Contract.Requires(label != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall(label, callingConvention, delegateType);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(Label label, CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            Contract.Requires(label != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall(label, callingConvention, arguments, returnValue);
        }

        public void Call(JitAction function)
        {
            Contract.Requires(function != null);

            if (!function.IsCompiled)
                throw new NotImplementedException("The compiler doesn't support late bindings.");

            Call call = Call(function.CompiledAddress, function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
        }

        public void Call<T>(JitAction<T> function, Operand arg0)
        {
            Contract.Requires(function != null);
            Contract.Requires(arg0 != null);

            if (!function.IsCompiled)
                throw new NotImplementedException("The compiler doesn't support late bindings.");

            Call call = Call(function.CompiledAddress, function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
            call.SetArgument(0, arg0);
        }

        public void Call<T1, T2>(JitAction<T1, T2> function, Operand arg0, Operand arg1)
        {
            Contract.Requires(function != null);
            Contract.Requires(arg0 != null);
            Contract.Requires(arg1 != null);

            if (!function.IsCompiled)
                throw new NotImplementedException("The compiler doesn't support late bindings.");

            Call call = Call(function.CompiledAddress, function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
            call.SetArgument(0, arg0);
            call.SetArgument(1, arg1);
        }

        public void Call<T1, T2, T3>(JitAction<T1, T2, T3> function, Operand arg0, Operand arg1, Operand arg2)
        {
            Contract.Requires(function != null);
            Contract.Requires(arg0 != null);
            Contract.Requires(arg1 != null);
            Contract.Requires(arg2 != null);

            if (!function.IsCompiled)
                throw new NotImplementedException("The compiler doesn't support late bindings.");

            Call call = Call(function.CompiledAddress, function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
            call.SetArgument(0, arg0);
            call.SetArgument(1, arg1);
            call.SetArgument(2, arg2);
        }

        public void Call<T1, T2, T3, T4>(JitAction<T1, T2, T3, T4> function, Operand arg0, Operand arg1, Operand arg2, Operand arg3)
        {
            Contract.Requires(function != null);
            Contract.Requires(arg0 != null);
            Contract.Requires(arg1 != null);
            Contract.Requires(arg2 != null);
            Contract.Requires(arg3 != null);

            if (!function.IsCompiled)
                throw new NotImplementedException("The compiler doesn't support late bindings.");

            Call call = Call(function.CompiledAddress, function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
            call.SetArgument(0, arg0);
            call.SetArgument(1, arg1);
            call.SetArgument(2, arg2);
            call.SetArgument(3, arg3);
        }

        public void Call<TResult>(JitFunction<TResult> function, Operand returnOperand0, Operand returnOperand1 = null)
        {
            Contract.Requires(function != null);
            Contract.Requires(returnOperand0 != null);

            if (!function.IsCompiled)
                throw new NotImplementedException("The compiler doesn't support late bindings.");

            Call call = Call(function.CompiledAddress, function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
            call.SetReturn(returnOperand0, returnOperand1 ?? Operand.None);
        }

        public void Call<T, TResult>(JitFunction<T, TResult> function, Operand arg0, Operand returnOperand0, Operand returnOperand1 = null)
        {
            Contract.Requires(function != null);
            Contract.Requires(arg0 != null);
            Contract.Requires(returnOperand0 != null);

            if (!function.IsCompiled)
                throw new NotImplementedException("The compiler doesn't support late bindings.");

            Call call = Call(function.CompiledAddress, function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
            call.SetArgument(0, arg0);
            call.SetReturn(returnOperand0, returnOperand1 ?? Operand.None);
        }

        public void Call<T1, T2, TResult>(JitFunction<T1, T2, TResult> function, Operand arg0, Operand arg1, Operand returnOperand0, Operand returnOperand1 = null)
        {
            Contract.Requires(function != null);
            Contract.Requires(arg0 != null);
            Contract.Requires(arg1 != null);
            Contract.Requires(returnOperand0 != null);

            if (!function.IsCompiled)
                throw new NotImplementedException("The compiler doesn't support late bindings.");

            Call call = Call(function.CompiledAddress, function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
            call.SetArgument(0, arg0);
            call.SetArgument(1, arg1);
            call.SetReturn(returnOperand0, returnOperand1 ?? Operand.None);
        }

        public void Call<T1, T2, T3, TResult>(JitFunction<T1, T2, T3, TResult> function, Operand arg0, Operand arg1, Operand arg2, Operand returnOperand0, Operand returnOperand1 = null)
        {
            Contract.Requires(function != null);
            Contract.Requires(arg0 != null);
            Contract.Requires(arg1 != null);
            Contract.Requires(arg2 != null);
            Contract.Requires(returnOperand0 != null);

            if (!function.IsCompiled)
                throw new NotImplementedException("The compiler doesn't support late bindings.");

            Call call = Call(function.CompiledAddress, function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
            call.SetArgument(0, arg0);
            call.SetArgument(1, arg1);
            call.SetArgument(2, arg2);
            call.SetReturn(returnOperand0, returnOperand1 ?? Operand.None);
        }

        public void Call<T1, T2, T3, T4, TResult>(JitFunction<T1, T2, T3, T4, TResult> function, Operand arg0, Operand arg1, Operand arg2, Operand arg3, Operand returnOperand0, Operand returnOperand1 = null)
        {
            Contract.Requires(function != null);
            Contract.Requires(arg0 != null);
            Contract.Requires(arg1 != null);
            Contract.Requires(arg2 != null);
            Contract.Requires(arg3 != null);
            Contract.Requires(returnOperand0 != null);

            if (!function.IsCompiled)
                throw new NotImplementedException("The compiler doesn't support late bindings.");

            Call call = Call(function.CompiledAddress, function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
            call.SetArgument(0, arg0);
            call.SetArgument(1, arg1);
            call.SetArgument(2, arg2);
            call.SetArgument(3, arg3);
            call.SetReturn(returnOperand0, returnOperand1 ?? Operand.None);
        }

        #endregion

        public void Lock()
        {
            _emitOptions |= EmitOptions.LockPrefix;
        }

        public void Rex()
        {
            _emitOptions |= EmitOptions.RexPrefix;
        }

        private Instruction NewInstruction(InstructionCode code, Operand[] operands)
        {
            Contract.Requires(operands != null);
            //Contract.Requires(operands.All(i => i != null));
            Contract.Ensures(Contract.Result<Instruction>() != null);

            if (code >= InstructionDescription.JumpBegin && code <= InstructionDescription.JumpEnd)
            {
                return new Jmp(this, code, operands);
            }
            else
            {
                return new Instruction(this, code, operands);
            }
        }

        private void _emitInstruction(InstructionCode instructionCode)
        {
            Instruction e = NewInstruction(instructionCode, Operand.EmptyOperands);
            AddEmittable(e);
            if (_cc != null)
            {
                e.Offset = _cc.CurrentOffset;
                e.Prepare(_cc);
            }
        }

        public void EmitInstruction(InstructionCode code)
        {
            _emitInstruction(code);
        }

        public void EmitInstruction(InstructionCode code, Operand operand0)
        {
            Contract.Requires(operand0 != null);

            _emitInstruction(code, operand0);
        }

        public void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1)
        {
            Contract.Requires(operand0 != null);
            Contract.Requires(operand1 != null);

            _emitInstruction(code, operand0, operand1);
        }

        public void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1, Operand operand2)
        {
            Contract.Requires(operand0 != null);
            Contract.Requires(operand1 != null);
            Contract.Requires(operand2 != null);

            _emitInstruction(code, operand0, operand1, operand2);
        }

        public void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1, Operand operand2, Operand operand3)
        {
            Contract.Requires(operand0 != null);
            Contract.Requires(operand1 != null);
            Contract.Requires(operand2 != null);
            Contract.Requires(operand3 != null);

            _emitInstruction(code, operand0, operand1, operand2, operand3);
        }

        public void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1, Operand operand2, Operand operand3, Operand operand4)
        {
            Contract.Requires(operand0 != null);
            Contract.Requires(operand1 != null);
            Contract.Requires(operand2 != null);
            Contract.Requires(operand3 != null);
            Contract.Requires(operand4 != null);

            _emitInstruction(code, operand0, operand1, operand2, operand3, operand4);
        }

        private void _emitInstruction(InstructionCode instructionCode, params Operand[] operands)
        {
            Contract.Requires(operands != null);
            //Contract.Requires(operands.All(i => i != null));

            Instruction e = NewInstruction(instructionCode, operands);
            AddEmittable(e);
            if (_cc != null)
            {
                e.Offset = _cc.CurrentOffset;
                e.Prepare(_cc);
            }
        }

        public void EmitJcc(InstructionCode code, Label label, Hint hint)
        {
            if (hint == Hint.None)
            {
                _emitInstruction(code, label);
            }
            else
            {
                Imm imm = (int)hint;
                _emitInstruction(code, label, imm);
            }
        }

        private Call _emitCall(Operand o0, CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            Contract.Requires(o0 != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            Function fn = Function;
            if (fn == null)
                throw new InvalidOperationException("No function.");

            Call call = new Call(this, fn, o0, callingConvention, arguments, returnValue);
            AddEmittable(call);
            return call;
        }

        private Call _emitCall(Operand o0, CallingConvention callingConvention, Type delegateType)
        {
            Contract.Requires(o0 != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            Function fn = Function;
            if (fn == null)
                throw new InvalidOperationException("No function.");

            Call call = new Call(this, fn, o0, callingConvention, delegateType);
            AddEmittable(call);
            return call;
        }

        internal void EmitReturn(Operand first, Operand second)
        {
            Function fn = Function;
            if (fn == null)
            {
                Error = Errors.NoFunction;
                return;
            }

            Ret eRet;
            try
            {
                eRet = new Ret(this, fn, first, second);
            }
            catch (OutOfMemoryException)
            {
                Error = Errors.NoHeapMemory;
                return;
            }

            AddEmittable(eRet);
        }

        private static readonly Dictionary<Type, VariableType> _jitTypes = new Dictionary<Type, VariableType>()
            {
                { typeof(void), (VariableType)(-1) },
                { typeof(sbyte), VariableType.GPD },
                { typeof(byte), VariableType.GPD },
                { typeof(short), VariableType.GPD },
                { typeof(ushort), VariableType.GPD },
                { typeof(int), VariableType.GPD },
                { typeof(uint), VariableType.GPD },
                { typeof(long), VariableType.GPQ },
                { typeof(ulong), VariableType.GPQ },
                { typeof(float), VariableType.FLOAT },
                { typeof(double), VariableType.DOUBLE },
                { typeof(IntPtr), VariableType.INTPTR },
            };

        internal static VariableType TypeToId(Type type)
        {
            VariableType value;
            if (_jitTypes.TryGetValue(type, out value))
                return value;

            return VariableType.Invalid;
        }

        internal VarData GetVarData(int id)
        {
            if (id == Operand.InvalidValue)
                throw new ArgumentException();

            return _varData[id & Operand.OperandIdValueMask];
        }
    }
}
