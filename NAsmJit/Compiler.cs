namespace NAsmJit
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics.Contracts;
    using System.Linq;

    public class Compiler
        : IIntrinsicSupport<GPVar, X87Var, MMVar, XMMVar>
        , IX86IntrinsicSupport<GPVar>
        , IX87IntrinsicSupport<X87Var>
        , IMmIntrinsicSupport<MMVar>
        , IXmmIntrinsicSupport<XMMVar>
    {
        [ContractPublicPropertyName("CodeGenerator")]
        private readonly CodeGenerator _codeGenerator;

        [ContractPublicPropertyName("Logger")]
        private Logger _logger;

        private readonly CompilerProperties _properties;
        private EmitOptions _emitOptions;
        private Emittable _first;
        private Emittable _last;
        private Emittable _current;

        [ContractPublicPropertyName("Function")]
        private Function _function;

        private readonly List<Target> _targetData = new List<Target>();
        private readonly List<VarData> _varData = new List<VarData>();
        private int _varNameId;
        private CompilerContext _cc;

        public Compiler(CodeGenerator codeGenerator = null, CompilerProperties properties = CompilerProperties.None)
        {
            _codeGenerator = codeGenerator ?? CodeGenerator.Global;
            _properties = properties;
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
            Contract.Requires(Function == null);
            Contract.Ensures(_function != null);
            Contract.Ensures(Contract.Result<Function>() != null);
            Contract.EndContractBlock();

            if (delegateType == typeof(Action))
                return NewFunction(callingConvention, new VariableType[0], VariableType.Invalid);

            Function f = new Function(this, callingConvention, delegateType);
            _function = f;

            AddEmittable(f);
            MarkLabel(f.EntryLabel);
            AddEmittable(f.Prolog);

            _varNameId = 0;
            f.CreateVariables();
            return f;
        }

        public Function NewFunction(CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            if (arguments == null)
                throw new ArgumentNullException("arguments");
            Contract.Requires(Function == null);
            Contract.Ensures(_function != null);
            Contract.Ensures(Contract.Result<Function>() != null);
            Contract.EndContractBlock();

            if (_function != null)
                throw new InvalidOperationException();

            Function f = new Function(this, callingConvention, arguments, returnValue);
            _function = f;

            AddEmittable(f);
            MarkLabel(f.EntryLabel);
            AddEmittable(f.Prolog);

            _varNameId = 0;
            f.CreateVariables();
            return f;
        }

        public Label DefineLabel()
        {
            Contract.Ensures(Contract.Result<Label>() != null);

            Label label = new Label(_targetData.Count);
            _targetData.Add(new Target(this, label));
            return label;
        }

        public void MarkLabel(Label label)
        {
            Contract.Requires(label != null);

            int id = label.Id & Operand.OperandIdValueMask;
            if (id >= _targetData.Count)
                throw new ArgumentException();

            AddEmittable(_targetData[id]);
        }

        public VarData NewVarData(string name, VariableType variableType, int size)
        {
            Contract.Requires(Function != null);
            Contract.Ensures(Contract.Result<VarData>() != null);

            if (name == null)
            {
                name = "var_" + _varNameId;
                _varNameId++;
            }

            VarData varData = new VarData(Function, _varData.Count | Operand.OperandIdTypeVar, variableType, size, name);
            _varData.Add(varData);
            return varData;
        }

        public GPVar ArgGP(int index)
        {
            if (index < 0)
                throw new ArgumentOutOfRangeException("index");
            Contract.Requires(Function != null);
            Contract.Requires(Function.Prototype != null);
            Contract.Requires(Function.Prototype != null);
            Contract.Requires(Function.Prototype.Arguments != null);
            Contract.Ensures(Contract.Result<GPVar>() != null);
            Contract.EndContractBlock();

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

        public GPVar NewGP(string name = null)
        {
            Contract.Requires(Function != null);
            Contract.Ensures(Contract.Result<GPVar>() != null);

            return NewGP(VariableInfo.NativeVariableType, name);
        }

        public GPVar NewGP(VariableType variableType, string name = null)
        {
            Contract.Requires(Function != null);
            Contract.Ensures(Contract.Result<GPVar>() != null);

            if ((VariableInfo.GetVariableInfo(variableType).Class & VariableClass.GP) == 0)
                throw new ArgumentException();

            if (Util.IsX86 && VariableInfo.GetVariableInfo(variableType).Size > 4)
            {
                variableType = VariableType.GPD;
                if (_logger != null)
                    _logger.LogString("*** COMPILER WARNING: Translated QWORD variable to DWORD, FIX YOUR CODE! ***" + Environment.NewLine);
            }

            VarData varData = NewVarData(name, variableType, VariableInfo.GetVariableInfo(variableType).Size);
            return GPVar.FromData(varData);
        }

        public MMVar ArgMM(int index)
        {
            if (index < 0)
                throw new ArgumentOutOfRangeException("index");
            Contract.Requires(Function != null);
            Contract.Requires(Function.Prototype != null);
            Contract.Requires(Function.Prototype.Arguments != null);
            Contract.Ensures(Contract.Result<MMVar>() != null);
            Contract.EndContractBlock();

            Function f = Function;
            if (f == null)
                throw new InvalidOperationException("No function.");

            FunctionPrototype prototype = f.Prototype;
            if (index < prototype.Arguments.Length)
                throw new ArgumentException();

            VarData vdata = Function._argumentVariables[index];
            if (vdata.Size != 12)
                throw new NotSupportedException();

            MMVar var = new MMVar(vdata.Id, VariableInfo.GetVariableInfo(vdata.Type).RegisterType, vdata.Type);
            return var;
        }

        public MMVar NewMM(VariableType variableType = VariableType.MM, string name = null)
        {
            Contract.Requires(Function != null);
            Contract.Ensures(Contract.Result<MMVar>() != null);

            if ((VariableInfo.GetVariableInfo(variableType).Class & VariableClass.MM) == 0)
                throw new ArgumentException();

            VarData vdata = NewVarData(name, variableType, 8);
            if (vdata.Size != 8)
                throw new NotSupportedException();

            return MMVar.FromData(vdata);
        }

        public XMMVar ArgXMM(int index)
        {
            if (index < 0)
                throw new ArgumentOutOfRangeException("index");
            Contract.Requires(Function != null);
            Contract.Requires(Function.Prototype != null);
            Contract.Requires(Function.Prototype.Arguments != null);
            Contract.Ensures(Contract.Result<XMMVar>() != null);
            Contract.EndContractBlock();

            Function f = Function;
            if (f == null)
                throw new InvalidOperationException("No function.");

            FunctionPrototype prototype = f.Prototype;
            if (index < prototype.Arguments.Length)
                throw new ArgumentException();

            VarData vdata = Function._argumentVariables[index];
            if (vdata.Size != 16)
                throw new NotSupportedException();

            XMMVar var = new XMMVar(vdata.Id, VariableInfo.GetVariableInfo(vdata.Type).RegisterType, vdata.Type);
            return var;
        }

        public XMMVar NewXMM(VariableType variableType = VariableType.XMM, string name = null)
        {
            Contract.Requires(Function != null);
            Contract.Ensures(Contract.Result<XMMVar>() != null);

            if ((VariableInfo.GetVariableInfo(variableType).Class & VariableClass.XMM) == 0)
                throw new ArgumentException();

            VarData vdata = NewVarData(name, variableType, 16);
            if (vdata.Size != 16)
                throw new NotSupportedException();

            return XMMVar.FromData(vdata);
        }

        public Function EndFunction()
        {
            Contract.Requires(Function != null);
            Contract.Ensures(Function == null);

            if (_function == null)
                throw new InvalidOperationException("No function.");

            Function f = _function;
            MarkLabel(f.ExitLabel);
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

            IntPtr result = a.Make();
            if (_logger != null)
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
            Contract.Requires(a != null);

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
                cc.Function = (Function)start;
                cc.Start = start;
                cc.Stop = stop = cc.Function.End;
                cc.ExtraBlock = stop.Previous;

                // Detect whether the function generation was finished.
                if (!cc.Function.Finished || cc.Function.End.Previous == null)
                {
                    throw new CompilerException("The compiler encountered an incomplete function.");
                }
                // ------------------------------------------------------------------------

                // ------------------------------------------------------------------------
                // Step 1:
                // - Assign/increment offset to each emittable.
                // - Extract variables from instructions.
                // - Prepare variables for register allocator:
                //   - Update read(r) / write(w) / overwrite(x) statistics.
                //   - Update register / memory usage statistics.
                //   - Find scope (first / last emittable) of variables.
                for (Emittable cur = start; ; cur = cur.Next)
                {
                    cur.Prepare(cc);
                    if (cur == stop)
                        break;
                }
                // ------------------------------------------------------------------------

                // We set compiler context also to Compiler so new emitted instructions
                // can call prepare() to itself.
                _cc = cc;

                /* ------------------------------------------------------------------------
                 * Step 2:
                 * - Translate special instructions (imul, cmpxchg8b, ...).
                 * - Alloc registers.
                 * - Translate forward jumps.
                 * - Alloc memory operands (variables related).
                 * - Emit function prolog.
                 * - Emit function epilog.
                 * - Patch memory operands (variables related).
                 * - Dump function prototype and variable statistics (if enabled).
                 */

                // translate special instructions and run alloc registers
                {
                    Emittable cur = start;
                    do
                    {
                        do
                        {
                            // assign current offset for each emittable back to CompilerContext.
                            cc.CurrentOffset = cur.Offset;
                            // assign previous emittable to compiler so each variable spill/alloc will be emitted before
                            _current = cur.Previous;
                            cur = cur.Translate(cc);
                        } while (cur != null);

                        cc.Unreachable = true;

                        int len = cc.BackwardsCode.Count;
                        while (cc.BackwardsPosition < len)
                        {
                            cur = cc.BackwardsCode[cc.BackwardsPosition++].Next;
                            if (!cur.IsTranslated)
                                break;

                            cur = null;
                        }
                    } while (cur != null);
                }

                // Translate forward jumps.
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

                // Alloc memory operands (variables related).
                cc.AllocMemoryOperands();

                // Emit function prolog / epilog
                cc.Function.PreparePrologEpilog(cc);

                _current = cc.Function.Prolog;
                cc.Function.EmitProlog(cc);

                _current = cc.Function.Epilog;
                cc.Function.EmitEpilog(cc);

                // Patch memory operands (variables related)
                _current = _last;
                cc.PatchMemoryOperands(start, stop);

                // Dump function prototype and variable statistics (if enabled).
                if (_logger != null)
                {
                    cc.Function.DumpFunction(cc);
                }
                // ------------------------------------------------------------------------

                // ------------------------------------------------------------------------
                // Hack: need to register labels that was created by the Step 2.
                if (a.LabelData.Count < _targetData.Count)
                {
                    a.RegisterLabels(_targetData.Count - a.LabelData.Count);
                }

                Emittable extraBlock = cc.ExtraBlock;

                // Step 3:
                // - Emit instructions to Assembler stream.
                for (Emittable cur = start; ; cur = cur.Next)
                {
                    cur.Emit(a);
                    if (cur == extraBlock)
                        break;
                }
                // ------------------------------------------------------------------------

                // ------------------------------------------------------------------------
                // Step 4:
                // - Emit everything else (post action).
                for (Emittable cur = start; ; cur = cur.Next)
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

        public void Alloc(BaseVar var)
        {
            Contract.Requires(var != null);

            Vhint(var, VariableHintKind.Alloc, Operand.InvalidValue);
        }

        public void Alloc(BaseVar var, RegIndex regIndex)
        {
            Contract.Requires(var != null);

            if ((int)regIndex > 31)
                return;

            Vhint(var, VariableHintKind.Alloc, 1 << (int)regIndex);
        }

        public void Alloc(BaseVar var, BaseReg reg)
        {
            Contract.Requires(var != null);
            Contract.Requires(reg != null);

            Vhint(var, VariableHintKind.Alloc, 1 << (int)reg.RegisterIndex);
        }

        public void Spill(BaseVar var)
        {
            Contract.Requires(var != null);

            Vhint(var, VariableHintKind.Spill, Operand.InvalidValue);
        }

        private void Vhint(BaseVar var, VariableHintKind hintId, int hintValue)
        {
            Contract.Requires(var != null);

            if (var.Id == Operand.InvalidValue)
                return;

            VarData vdata = GetVarData(var.Id);
            Contract.Assert(vdata != null);

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
            EmitInstructionImpl(code, Operand.EmptyOperands);
        }

        public void Emit(InstructionCode code, Operand operand0)
        {
            if (operand0 == null)
                throw new ArgumentNullException("operand0");
            Contract.EndContractBlock();

            EmitInstructionImpl(code, operand0);
        }

        public void Emit(InstructionCode code, Operand operand0, Operand operand1)
        {
            if (operand0 == null)
                throw new ArgumentNullException("operand0");
            if (operand1 == null)
                throw new ArgumentNullException("operand1");
            Contract.EndContractBlock();

            EmitInstructionImpl(code, operand0, operand1);
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

            EmitInstructionImpl(code, operand0, operand1, operand2);
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

            EmitInstructionImpl(code, operand0, operand1, operand2, operand3);
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

            EmitInstructionImpl(code, operand0, operand1, operand2, operand3, operand4);
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
            Contract.Requires(arguments != null);
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
            Contract.Requires(arguments != null);
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
            Contract.Requires(arguments != null);
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
            Contract.Requires(arguments != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall((Imm)dst, callingConvention, arguments, returnValue);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(Label label, CallingConvention callingConvention, Type delegateType)
        {
            Contract.Requires(label != null);
            Contract.Requires(delegateType != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall(label, callingConvention, delegateType);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(Label label, CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            Contract.Requires(label != null);
            Contract.Requires(arguments != null);
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
            Contract.Requires(Contract.ForAll(operands, i => i != null));
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

        public void EmitInstruction(InstructionCode code)
        {
            EmitInstructionImpl(code, Operand.EmptyOperands);
        }

        public void EmitInstruction(InstructionCode code, Operand operand0)
        {
            if (operand0 == null)
                throw new ArgumentNullException("operand0");

            EmitInstructionImpl(code, operand0);
        }

        public void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1)
        {
            if (operand0 == null)
                throw new ArgumentNullException("operand0");
            if (operand1 == null)
                throw new ArgumentNullException("operand1");

            EmitInstructionImpl(code, operand0, operand1);
        }

        public void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1, Operand operand2)
        {
            if (operand0 == null)
                throw new ArgumentNullException("operand0");
            if (operand1 == null)
                throw new ArgumentNullException("operand1");
            if (operand2 == null)
                throw new ArgumentNullException("operand2");

            EmitInstructionImpl(code, operand0, operand1, operand2);
        }

        public void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1, Operand operand2, Operand operand3)
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

            EmitInstructionImpl(code, operand0, operand1, operand2, operand3);
        }

        public void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1, Operand operand2, Operand operand3, Operand operand4)
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

            EmitInstructionImpl(code, operand0, operand1, operand2, operand3, operand4);
        }

        private void EmitInstructionImpl(InstructionCode instructionCode, params Operand[] operands)
        {
            Contract.Requires(operands != null);
            Contract.Requires(Contract.ForAll(operands, i => i != null));

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
            if (label == null)
                throw new ArgumentNullException("label");

            if (hint == Hint.None)
            {
                EmitInstructionImpl(code, label);
            }
            else
            {
                Imm imm = (int)hint;
                EmitInstructionImpl(code, label, imm);
            }
        }

        private Call _emitCall(Operand o0, CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            Contract.Requires(o0 != null);
            Contract.Requires(arguments != null);
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
            Contract.Requires(Function != null);

            Function fn = Function;
            if (fn == null)
                throw new InvalidOperationException("There is no function defined for the compiler.");

            Ret eRet = new Ret(this, fn, first, second);
            AddEmittable(eRet);
        }

        internal static VariableType TypeToId(Type type)
        {
            switch (Type.GetTypeCode(type))
            {
            case TypeCode.Byte:
            case TypeCode.SByte:
            case TypeCode.Int16:
            case TypeCode.Int32:
            case TypeCode.UInt16:
            case TypeCode.UInt32:
                return VariableType.GPD;

            case TypeCode.Int64:
            case TypeCode.UInt64:
                return VariableType.GPQ;

            case TypeCode.Single:
                return VariableInfo.FloatVariableType;

            case TypeCode.Double:
                return VariableInfo.DoubleVariableType;

            case TypeCode.Boolean:
                throw new NotImplementedException();

            default:
                if (type == typeof(IntPtr))
                    return VariableInfo.NativeVariableType;
                else if (type == typeof(void))
                    return VariableType.Invalid;
                else
                    throw new ArgumentException();
            }
        }

        internal static Type IdToType(VariableType type)
        {
            Contract.Ensures(Contract.Result<Type>() != null);

            switch (type)
            {
            case VariableType.Invalid:
                return typeof(void);

            case VariableType.GPD:
                return typeof(int);
            case VariableType.GPQ:
                return typeof(long);

            case VariableType.X87:
            case VariableType.X87_1F:
            case VariableType.X87_1D:
            case VariableType.MM:
            case VariableType.XMM:
            case VariableType.XMM_1F:
            case VariableType.XMM_4F:
            case VariableType.XMM_1D:
            case VariableType.XMM_2D:
                throw new NotImplementedException();

            default:
                throw new ArgumentException();
            }
        }

        internal VarData GetVarData(int id)
        {
            if (id == Operand.InvalidValue)
                throw new ArgumentException();
            Contract.Ensures(Contract.Result<VarData>() != null);

            int index = id & Operand.OperandIdValueMask;
            if (index >= _varData.Count)
                throw new ArgumentException();

            return _varData[id & Operand.OperandIdValueMask];
        }

        [ContractInvariantMethod]
        private void ObjectInvariants()
        {
            Contract.Invariant(Contract.ForAll(_targetData, i => i != null));
            Contract.Invariant(Contract.ForAll(_varData, i => i != null));
        }
    }
}
