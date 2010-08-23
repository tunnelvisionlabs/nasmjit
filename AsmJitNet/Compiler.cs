namespace AsmJitNet
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics.Contracts;
    using System.Linq;
    using Debug = System.Diagnostics.Debug;

    public class Compiler
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
            Contract.Ensures(Contract.Result<Function>() != null);

            if (delegateType == null)
                throw new ArgumentNullException("delegateType");

            if (delegateType == typeof(Action))
                return NewFunction(callingConvention, null, VariableType.Invalid);

            if (!delegateType.IsGenericType)
                throw new ArgumentException();

            VariableType[] arguments = null;
            VariableType returnValue = VariableType.Invalid;
            Type genericType = delegateType.GetGenericTypeDefinition();
            if (genericType.FullName.StartsWith("System.Action`"))
            {
                arguments = Array.ConvertAll(delegateType.GetGenericArguments(), TypeToId);
            }
            else if (genericType.FullName.StartsWith("System.Func`"))
            {
                Type[] typeArguments = delegateType.GetGenericArguments();
                returnValue = TypeToId(typeArguments[typeArguments.Length - 1]);
                Array.Resize(ref typeArguments, typeArguments.Length - 1);
                arguments = Array.ConvertAll(typeArguments, TypeToId);
            }
            else
            {
                throw new ArgumentException();
            }

            return NewFunction(callingConvention, arguments, returnValue);
        }

        public Function NewFunction(CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            Contract.Ensures(Contract.Result<Function>() != null);

            if (_function != null)
                throw new InvalidOperationException();

            Function f = new Function(this);
            _function = f;

            f.SetPrototype(callingConvention, arguments, returnValue);
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
            GPVar var = new GPVar(vdata.Id, vdata.Size, VariableInfo.GetVariableInfo(vdata.Type).Code, vdata.Type);
            return var;
        }

        public GPVar NewGP(VariableType variableType = VariableType.GPN, string name = null)
        {
            if ((VariableInfo.GetVariableInfo(variableType).Class & VariableClass.GP) == 0)
                throw new ArgumentException();

            VarData varData = NewVarData(name, variableType, IntPtr.Size);
            GPVar var = new GPVar(varData.Id, varData.Size, VariableInfo.GetVariableInfo(varData.Type).Code, varData.Type);
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
            MMVar var = new MMVar(vdata.Id, vdata.Size, VariableInfo.GetVariableInfo(vdata.Type).Code, vdata.Type);
            return var;
        }

        public MMVar NewMM(VariableType variableType = VariableType.MM, string name = null)
        {
            if ((VariableInfo.GetVariableInfo(variableType).Class & VariableClass.MM) == 0)
                throw new ArgumentException();

            VarData vdata = NewVarData(name, variableType, 16);
            MMVar var = new MMVar(vdata.Id, vdata.Size, VariableInfo.GetVariableInfo(vdata.Type).Code, vdata.Type);
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
            XMMVar var = new XMMVar(vdata.Id, vdata.Size, VariableInfo.GetVariableInfo(vdata.Type).Code, vdata.Type);
            return var;
        }

        public XMMVar NewXMM(VariableType variableType = VariableType.XMM, string name = null)
        {
            if ((VariableInfo.GetVariableInfo(variableType).Class & VariableClass.XMM) == 0)
                throw new ArgumentException();

            VarData vdata = NewVarData(name, variableType, 16);
            XMMVar var = new XMMVar(vdata.Id, vdata.Size, VariableInfo.GetVariableInfo(vdata.Type).Code, vdata.Type);
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
                // Find function.
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
            if (operand0 == null)
                throw new ArgumentNullException("operand1");
            if (operand0 == null)
                throw new ArgumentNullException("operand2");
            Contract.EndContractBlock();

            _emitInstruction(code, operand0, operand1, operand2);
        }

        #endregion

        #region X86 Instructions

        /// <summary>
        /// Add with carry.
        /// </summary>
        public void Adc(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Adc, dst, src);
        }

        /// <summary>
        /// Add with carry.
        /// </summary>
        public void Adc(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Adc, dst, src);
        }

        /// <summary>
        /// Add with carry.
        /// </summary>
        public void Adc(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Adc, dst, src);
        }

        /// <summary>
        /// Add with carry.
        /// </summary>
        public void Adc(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Adc, dst, src);
        }

        /// <summary>
        /// Add with carry.
        /// </summary>
        public void Adc(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Adc, dst, src);
        }

        /// <summary>
        /// Add.
        /// </summary>
        public void Add(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Add, dst, src);
        }

        /// <summary>
        /// Add.
        /// </summary>
        public void Add(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Add, dst, src);
        }

        /// <summary>
        /// Add.
        /// </summary>
        public void Add(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Add, dst, src);
        }

        /// <summary>
        /// Add.
        /// </summary>
        public void Add(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Add, dst, src);
        }

        /// <summary>
        /// Add.
        /// </summary>
        public void Add(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Add, dst, src);
        }

        /// <summary>
        /// Logical AND.
        /// </summary>
        public void And(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.And, dst, src);
        }

        /// <summary>
        /// Logical AND.
        /// </summary>
        public void And(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.And, dst, src);
        }

        /// <summary>
        /// Logical AND.
        /// </summary>
        public void And(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.And, dst, src);
        }

        /// <summary>
        /// Logical AND.
        /// </summary>
        public void And(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.And, dst, src);
        }

        /// <summary>
        /// Logical AND.
        /// </summary>
        public void And(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.And, dst, src);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(GPVar dst)
        {
            Contract.Requires(dst != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall(dst);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(Mem dst)
        {
            Contract.Requires(dst != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall(dst);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(Imm dst)
        {
            Contract.Requires(dst != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall(dst);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(IntPtr dst)
        {
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall((Imm)dst);
        }

        /// <summary>
        /// Call procedure.
        /// </summary>
        public Call Call(Label label)
        {
            Contract.Requires(label != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            return _emitCall(label);
        }

        public void Call(JitAction function)
        {
            Contract.Requires(function != null);

            if (!function.IsCompiled)
                throw new NotImplementedException("The compiler doesn't support late bindings.");

            Call call = Call(function.CompiledAddress);
            call.SetPrototype(function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
        }

        public void Call<T>(JitAction<T> function, Operand arg0)
        {
            Contract.Requires(function != null);
            Contract.Requires(arg0 != null);

            if (!function.IsCompiled)
                throw new NotImplementedException("The compiler doesn't support late bindings.");

            Call call = Call(function.CompiledAddress);
            call.SetPrototype(function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
            call.SetArgument(0, arg0);
        }

        public void Call<T1, T2>(JitAction<T1, T2> function, Operand arg0, Operand arg1)
        {
            Contract.Requires(function != null);
            Contract.Requires(arg0 != null);
            Contract.Requires(arg1 != null);

            if (!function.IsCompiled)
                throw new NotImplementedException("The compiler doesn't support late bindings.");

            Call call = Call(function.CompiledAddress);
            call.SetPrototype(function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
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

            Call call = Call(function.CompiledAddress);
            call.SetPrototype(function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
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

            Call call = Call(function.CompiledAddress);
            call.SetPrototype(function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
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

            Call call = Call(function.CompiledAddress);
            call.SetPrototype(function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
            call.SetReturn(returnOperand0, returnOperand1 ?? Operand.None);
        }

        public void Call<T, TResult>(JitFunction<T, TResult> function, Operand arg0, Operand returnOperand0, Operand returnOperand1 = null)
        {
            Contract.Requires(function != null);
            Contract.Requires(arg0 != null);
            Contract.Requires(returnOperand0 != null);

            if (!function.IsCompiled)
                throw new NotImplementedException("The compiler doesn't support late bindings.");

            Call call = Call(function.CompiledAddress);
            call.SetPrototype(function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
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

            Call call = Call(function.CompiledAddress);
            call.SetPrototype(function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
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

            Call call = Call(function.CompiledAddress);
            call.SetPrototype(function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
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

            Call call = Call(function.CompiledAddress);
            call.SetPrototype(function.CallingConvention, function.ArgumentTypes.ToArray(), function.ReturnType);
            call.SetArgument(0, arg0);
            call.SetArgument(1, arg1);
            call.SetArgument(2, arg2);
            call.SetArgument(3, arg3);
            call.SetReturn(returnOperand0, returnOperand1 ?? Operand.None);
        }

        public void Cmp(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cmp, dst, src);
        }

        public void Cmp(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cmp, dst, src);
        }

        public void Cmp(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cmp, dst, src);
        }

        public void Cmp(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cmp, dst, src);
        }

        public void Cmp(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cmp, dst, src);
        }

        public void Cpuid(GPVar inout_eax, GPVar out_ebx, GPVar out_ecx, GPVar out_edx)
        {
            Contract.Requires(inout_eax != null);
            Contract.Requires(out_ebx != null);
            Contract.Requires(out_ecx != null);
            Contract.Requires(out_edx != null);

            // Destination variables must be different
            if (inout_eax.Id == out_ebx.Id
                || out_ebx.Id == out_ecx.Id
                || out_ecx.Id == out_edx.Id)
            {
                throw new ArgumentException("Destination variables must be different.");
            }

            _emitInstruction(InstructionCode.Cpuid, inout_eax, out_ebx, out_ecx, out_edx);
        }

        public void Dec(GPVar dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Dec, dst);
        }

        public void Dec(Mem dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Dec, dst);
        }

        public void Imul(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Imul, dst, src);
        }

        public void Imul(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Imul, dst, src);
        }

        public void Imul(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Imul, dst, src);
        }

        public void Imul(GPVar dst, GPVar src, Imm imm)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm != null);

            _emitInstruction(InstructionCode.Imul, dst, src, imm);
        }

        public void Imul(GPVar dst, Mem src, Imm imm)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm != null);

            _emitInstruction(InstructionCode.Imul, dst, src, imm);
        }

        public void Inc(GPVar dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Inc, dst);
        }

        public void Inc(Mem dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Inc, dst);
        }

        public void Int3()
        {
            _emitInstruction(InstructionCode.Int3);
        }

        public void Jmp(GPVar dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Jmp, dst);
        }

        public void Jmp(Mem dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Jmp, dst);
        }

        public void Jmp(Imm dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Jmp, dst);
        }

        public void Jmp(IntPtr dst)
        {
            _emitInstruction(InstructionCode.Jmp, (Imm)dst);
        }

        public void Jmp(Label label)
        {
            Contract.Requires(label != null);

            _emitInstruction(InstructionCode.Jmp, label);
        }

        public void Jnz(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            _emitJcc(InstructionCode.Jnz, label, hint);
        }

        public void Jz(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            _emitJcc(InstructionCode.Jz, label, hint);
        }

        public void Lea(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Lea, dst, src);
        }

        public void Maskmovdqu(GPVar dst_ptr, XMMVar src, XMMVar mask)
        {
            Contract.Requires(dst_ptr != null);
            Contract.Requires(src != null);
            Contract.Requires(mask != null);

            _emitInstruction(InstructionCode.Maskmovdqu, dst_ptr, src, mask);
        }

        public void Maskmovq(GPVar dst_ptr, MMVar data, MMVar mask)
        {
            Contract.Requires(dst_ptr != null);
            Contract.Requires(data != null);
            Contract.Requires(mask != null);

            _emitInstruction(InstructionCode.Maskmovq, dst_ptr, data, mask);
        }

        public void Mov(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mov, dst, src);
        }

        public void Mov(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mov, dst, src);
        }

        public void Mov(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mov, dst, src);
        }

        public void Mov(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mov, dst, src);
        }

        public void Mov(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mov, dst, src);
        }

        public void RepMovsb(GPVar dst_addr, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            // All registers must be unique, they will be reallocated to dst=ES:EDI,RDI, src=DS:ESI/RSI, cnt=ECX/RCX.
            Debug.Assert(dst_addr.Id != src_addr.Id && src_addr.Id != cnt_ecx.Id);

            _emitInstruction(InstructionCode.RepMovsb, dst_addr, src_addr, cnt_ecx);
        }

        public void Ret()
        {
            _emitReturn(null, null);
        }

        public void Ret(GPVar first)
        {
            Contract.Requires(first != null);

            _emitReturn(first, null);
        }

        public void Ret(GPVar first, GPVar second)
        {
            Contract.Requires(first != null);
            Contract.Requires(second != null);

            _emitReturn(first, second);
        }

        public void Ret(XMMVar first)
        {
            Contract.Requires(first != null);

            _emitReturn(first, null);
        }

        public void Ret(XMMVar first, XMMVar second)
        {
            Contract.Requires(first != null);
            Contract.Requires(second != null);

            _emitReturn(first, second);
        }

        public void Shl(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Shl, dst, src);
        }

        public void Shl(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Shl, dst, src);
        }

        public void Shl(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Shl, dst, src);
        }

        public void Shl(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Shl, dst, src);
        }

        public void Sub(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sub, dst, src);
        }

        public void Test(GPVar op1, GPVar op2)
        {
            Contract.Requires(op1 != null);
            Contract.Requires(op2 != null);

            _emitInstruction(InstructionCode.Test, op1, op2);
        }

        public void Test(GPVar op1, Imm op2)
        {
            Contract.Requires(op1 != null);
            Contract.Requires(op2 != null);

            _emitInstruction(InstructionCode.Test, op1, op2);
        }

        public void Test(Mem op1, GPVar op2)
        {
            Contract.Requires(op1 != null);
            Contract.Requires(op2 != null);

            _emitInstruction(InstructionCode.Test, op1, op2);
        }

        public void Test(Mem op1, Imm op2)
        {
            Contract.Requires(op1 != null);
            Contract.Requires(op2 != null);

            _emitInstruction(InstructionCode.Test, op1, op2);
        }

        public void Xor(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Xor, dst, src);
        }

        public void Xor(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Xor, dst, src);
        }

        public void Xor(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Xor, dst, src);
        }

        public void Xor(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Xor, dst, src);
        }

        public void Xor(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Xor, dst, src);
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

            if (code >= InstructionDescription.JumpAnyBegin && code <= InstructionDescription.JumpAnyEnd)
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

        private void _emitJcc(InstructionCode code, Label label, Hint hint)
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

        private Call _emitCall(Operand o0)
        {
            Contract.Requires(o0 != null);
            Contract.Ensures(Contract.Result<Call>() != null);

            Function fn = Function;
            if (fn == null)
                throw new InvalidOperationException("No function.");

            Call call = new Call(this, fn, o0);
            AddEmittable(call);
            return call;
        }

        private void _emitReturn(Operand first, Operand second)
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
