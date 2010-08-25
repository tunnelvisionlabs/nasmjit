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
                return NewFunction(callingConvention, new VariableType[0], VariableType.Invalid);

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
            if (arguments == null)
                throw new ArgumentNullException("arguments");
            Contract.Ensures(Contract.Result<Function>() != null);
            Contract.EndContractBlock();

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
        /// Bit scan forward
        /// </summary>
        public void Bsf(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Bsf, dst, src);
        }

        /// <summary>
        /// Bit scan forward
        /// </summary>
        public void Bsf(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Bsf, dst, src);
        }

        /// <summary>
        /// Bit scan reverse
        /// </summary>
        public void Bsr(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Bsr, dst, src);
        }

        /// <summary>
        /// Bit scan reverse
        /// </summary>
        public void Bsr(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Bsr, dst, src);
        }

        /// <summary>
        /// Byte swap (32 bit or 64 bit registers only) (i486)
        /// </summary>
        public void Bswap(GPVar dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Bswap, dst);
        }

        /// <summary>
        /// Bit test
        /// </summary>
        public void Bt(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Bt, dst, src);
        }

        /// <summary>
        /// Bit test
        /// </summary>
        public void Bt(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Bt, dst, src);
        }

        /// <summary>
        /// Bit test
        /// </summary>
        public void Bt(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Bt, dst, src);
        }

        /// <summary>
        /// Bit test
        /// </summary>
        public void Bt(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Bt, dst, src);
        }

        /// <summary>
        /// Bit test and complement
        /// </summary>
        public void Btc(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Btc, dst, src);
        }

        /// <summary>
        /// Bit test and complement
        /// </summary>
        public void Btc(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Btc, dst, src);
        }

        /// <summary>
        /// Bit test and complement
        /// </summary>
        public void Btc(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Btc, dst, src);
        }

        /// <summary>
        /// Bit test and complement
        /// </summary>
        public void Btc(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Btc, dst, src);
        }

        /// <summary>
        /// Bit test and reset
        /// </summary>
        public void Btr(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Btr, dst, src);
        }

        /// <summary>
        /// Bit test and reset
        /// </summary>
        public void Btr(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Btr, dst, src);
        }

        /// <summary>
        /// Bit test and reset
        /// </summary>
        public void Btr(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Btr, dst, src);
        }

        /// <summary>
        /// Bit test and reset
        /// </summary>
        public void Btr(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Btr, dst, src);
        }

        /// <summary>
        /// Bit test and set
        /// </summary>
        public void Bts(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Bts, dst, src);
        }

        /// <summary>
        /// Bit test and set
        /// </summary>
        public void Bts(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Bts, dst, src);
        }

        /// <summary>
        /// Bit test and set
        /// </summary>
        public void Bts(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Bts, dst, src);
        }

        /// <summary>
        /// Bit test and set
        /// </summary>
        public void Bts(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Bts, dst, src);
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

        /// <summary>
        /// Convert byte to word (sign extend)
        /// </summary>
        public void Cbw(GPVar dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Cbw, dst);
        }

        /// <summary>
        /// Convert word to dword (sign extend)
        /// </summary>
        public void Cwde(GPVar dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Cwde, dst);
        }

#if ASMJIT_X64
        /// <summary>
        /// Convert dword to qword (sign extend)
        /// </summary>
        public void Cdqe(GPVar dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Cdqe, dst);
        }
#endif

        /// <summary>
        /// Clear carry flag
        /// </summary>
        public void Clc()
        {
            _emitInstruction(InstructionCode.Clc);
        }

        /// <summary>
        /// Clear direction flag
        /// </summary>
        public void Cld()
        {
            _emitInstruction(InstructionCode.Cld);
        }

        /// <summary>
        /// Complement carry flag
        /// </summary>
        public void Cmc()
        {
            _emitInstruction(InstructionCode.Cmc);
        }

        /// <summary>
        /// Conditional move
        /// </summary>
        public void CMov(Condition cc, GPVar dst, GPVar src)
        {
            _emitInstruction(Assembler.ConditionToMovCC(cc), dst, src);
        }

        /// <summary>
        /// Conditional move
        /// </summary>
        public void CMov(Condition cc, GPVar dst, Mem src)
        {
            _emitInstruction(Assembler.ConditionToMovCC(cc), dst, src);
        }

        public void Cmova(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmova, dst, src);
        }
        public void Cmova(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmova, dst, src);
        }
        public void Cmovae(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovae, dst, src);
        }
        public void Cmovae(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovae, dst, src);
        }
        public void Cmovb(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovb, dst, src);
        }
        public void Cmovb(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovb, dst, src);
        }
        public void Cmovbe(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovbe, dst, src);
        }
        public void Cmovbe(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovbe, dst, src);
        }
        public void Cmovc(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovc, dst, src);
        }
        public void Cmovc(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovc, dst, src);
        }
        public void Cmove(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmove, dst, src);
        }
        public void Cmove(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmove, dst, src);
        }
        public void Cmovg(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovg, dst, src);
        }
        public void Cmovg(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovg, dst, src);
        }
        public void Cmovge(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovge, dst, src);
        }
        public void Cmovge(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovge, dst, src);
        }
        public void Cmovl(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovl, dst, src);
        }
        public void Cmovl(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovl, dst, src);
        }
        public void Cmovle(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovle, dst, src);
        }
        public void Cmovle(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovle, dst, src);
        }
        public void Cmovna(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovna, dst, src);
        }
        public void Cmovna(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovna, dst, src);
        }
        public void Cmovnae(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovnae, dst, src);
        }
        public void Cmovnae(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovnae, dst, src);
        }
        public void Cmovnb(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovnb, dst, src);
        }
        public void Cmovnb(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovnb, dst, src);
        }
        public void Cmovnbe(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovnbe, dst, src);
        }
        public void Cmovnbe(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovnbe, dst, src);
        }
        public void Cmovnc(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovnc, dst, src);
        }
        public void Cmovnc(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovnc, dst, src);
        }
        public void Cmovne(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovne, dst, src);
        }
        public void Cmovne(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovne, dst, src);
        }
        public void Cmovng(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovng, dst, src);
        }
        public void Cmovng(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovng, dst, src);
        }
        public void Cmovnge(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovnge, dst, src);
        }
        public void Cmovnge(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovnge, dst, src);
        }
        public void Cmovnl(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovnl, dst, src);
        }
        public void Cmovnl(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovnl, dst, src);
        }
        public void Cmovnle(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovnle, dst, src);
        }
        public void Cmovnle(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovnle, dst, src);
        }
        public void Cmovno(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovno, dst, src);
        }
        public void Cmovno(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovno, dst, src);
        }
        public void Cmovnp(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovnp, dst, src);
        }
        public void Cmovnp(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovnp, dst, src);
        }
        public void Cmovns(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovns, dst, src);
        }
        public void Cmovns(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovns, dst, src);
        }
        public void Cmovnz(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovnz, dst, src);
        }
        public void Cmovnz(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovnz, dst, src);
        }
        public void Cmovo(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovo, dst, src);
        }
        public void Cmovo(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovo, dst, src);
        }
        public void Cmovp(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovp, dst, src);
        }
        public void Cmovp(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovp, dst, src);
        }
        public void Cmovpe(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovpe, dst, src);
        }
        public void Cmovpe(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovpe, dst, src);
        }
        public void Cmovpo(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovpo, dst, src);
        }
        public void Cmovpo(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovpo, dst, src);
        }
        public void Cmovs(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovs, dst, src);
        }
        public void Cmovs(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovs, dst, src);
        }
        public void Cmovz(GPVar dst, GPVar src)
        {
            _emitInstruction(InstructionCode.Cmovz, dst, src);
        }
        public void Cmovz(GPVar dst, Mem src)
        {
            _emitInstruction(InstructionCode.Cmovz, dst, src);
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

        public void Cmpxchg(GPVar cmp_1_eax, GPVar cmp_2, GPVar src)
        {
            Contract.Requires(cmp_1_eax != null);
            Contract.Requires(cmp_2 != null);
            Contract.Requires(src != null);

            if (cmp_1_eax.Id == src.Id)
                throw new ArgumentException();

            _emitInstruction(InstructionCode.Cmpxchg, cmp_1_eax, cmp_2, src);
        }

        public void Cmpxchg(GPVar cmp_1_eax, Mem cmp_2, GPVar src)
        {
            Contract.Requires(cmp_1_eax != null);
            Contract.Requires(cmp_2 != null);
            Contract.Requires(src != null);

            if (cmp_1_eax.Id == src.Id)
                throw new ArgumentException();

            _emitInstruction(InstructionCode.Cmpxchg, cmp_1_eax, cmp_2, src);
        }

        public void Cmpxchg8b(GPVar cmp_edx, GPVar cmp_eax, GPVar cmp_ecx, GPVar cmp_ebx, Mem dst)
        {
            Contract.Requires(cmp_eax != null);
            Contract.Requires(cmp_ebx != null);
            Contract.Requires(cmp_ecx != null);
            Contract.Requires(cmp_edx != null);
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Cmpxchg8b, cmp_edx, cmp_eax, cmp_ecx, cmp_ebx, dst);
        }

#if ASMJIT_X64
        public void Cmpxchg16b(GPVar cmp_rdx, GPVar cmp_rax, GPVar cmp_rcx, GPVar cmp_rbx, Mem dst)
        {
            Contract.Requires(cmp_rax != null);
            Contract.Requires(cmp_rbx != null);
            Contract.Requires(cmp_rcx != null);
            Contract.Requires(cmp_rdx != null);
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Cmpxchg8b, cmp_rdx, cmp_rax, cmp_rcx, cmp_rbx, dst);
        }
#endif

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

#if ASMJIT_X86
        public void Daa(GPVar dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Daa, dst);
        }

        public void Das(GPVar dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Das, dst);
        }
#endif

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

        /// <summary>
        /// Unsigned divide
        /// </summary>
        public void DivLoHi(GPVar dstLo, GPVar dstHi, GPVar src)
        {
            Contract.Requires(dstLo != null);
            Contract.Requires(dstHi != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Div, dstLo, dstHi, src);
        }

        /// <summary>
        /// Unsigned divide
        /// </summary>
        public void DivLoHi(GPVar dstLo, GPVar dstHi, Mem src)
        {
            Contract.Requires(dstLo != null);
            Contract.Requires(dstHi != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Div, dstLo, dstHi, src);
        }

#if ASMJIT_NOT_SUPPORTED_BY_COMPILER
        /// <summary>
        /// Make stack frame for procedure parameters
        /// </summary>
        public void Enter(Imm imm16, Imm imm8)
        {
            Contract.Requires(imm16 != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Enter, imm16, imm8);
        }
#endif

        /// <summary>
        /// Signed divide
        /// </summary>
        public void IdivLoHi(GPVar dstLo, GPVar dstHi, GPVar src)
        {
            Contract.Requires(dstLo != null);
            Contract.Requires(dstHi != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Idiv, dstLo, dstHi, src);
        }

        /// <summary>
        /// Signed divide
        /// </summary>
        public void IdivLoHi(GPVar dstLo, GPVar dstHi, Mem src)
        {
            Contract.Requires(dstLo != null);
            Contract.Requires(dstHi != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Idiv, dstLo, dstHi, src);
        }

        /// <summary>
        /// Signed multiply
        /// </summary>
        public void ImulLoHi(GPVar dstLo, GPVar dstHi, GPVar src)
        {
            Contract.Requires(dstLo != null);
            Contract.Requires(dstHi != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Imul, dstLo, dstHi, src);
        }

        /// <summary>
        /// Signed multiply
        /// </summary>
        public void ImulLoHi(GPVar dstLo, GPVar dstHi, Mem src)
        {
            Contract.Requires(dstLo != null);
            Contract.Requires(dstHi != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Imul, dstLo, dstHi, src);
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

        public void J(Condition cc, Label label, Hint hint = Hint.None)
        {
            _emitJcc(Assembler.ConditionToJump(cc), label, hint);
        }

        public void Ja(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Ja, label, hint);
        }
        public void Jae(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jae, label, hint);
        }
        public void Jb(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jb, label, hint);
        }
        public void Jbe(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jbe, label, hint);
        }
        public void Jc(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jc, label, hint);
        }
        public void Je(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Je, label, hint);
        }
        public void Jg(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jg, label, hint);
        }
        public void Jge(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jge, label, hint);
        }
        public void Jl(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jl, label, hint);
        }
        public void Jle(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jle, label, hint);
        }
        public void Jna(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jna, label, hint);
        }
        public void Jnae(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jnae, label, hint);
        }
        public void Jnb(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jnb, label, hint);
        }
        public void Jnbe(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jnbe, label, hint);
        }
        public void Jnc(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jnc, label, hint);
        }
        public void Jne(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jne, label, hint);
        }
        public void Jng(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jng, label, hint);
        }
        public void Jnge(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jnge, label, hint);
        }
        public void Jnl(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jnl, label, hint);
        }
        public void Jnle(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jnle, label, hint);
        }
        public void Jno(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jno, label, hint);
        }
        public void Jnp(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jnp, label, hint);
        }
        public void Jns(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jns, label, hint);
        }
        public void Jnz(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jnz, label, hint);
        }
        public void Jo(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jo, label, hint);
        }
        public void Jp(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jp, label, hint);
        }
        public void Jpe(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jpe, label, hint);
        }
        public void Jpo(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jpo, label, hint);
        }
        public void Js(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Js, label, hint);
        }
        public void Jz(Label label, Hint hint = Hint.None)
        {
            _emitJcc(InstructionCode.Jz, label, hint);
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

        public void Lea(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Lea, dst, src);
        }

#if ASMJIT_NOT_SUPPORTED_BY_COMPILER
        /// <summary>
        /// High level procedure exit
        /// </summary>
        public void Leave()
        {
            _emitInstruction(InstructionCode.Leave);
        }
#endif

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

        public void MovPtr(GPVar dst, IntPtr src)
        {
            Contract.Requires(dst != null);

            Imm imm = src;
            _emitInstruction(InstructionCode.MovPtr, dst, imm);
        }

        public void MovPtr(IntPtr dst, GPVar src)
        {
            Contract.Requires(src != null);

            Imm imm = dst;
            _emitInstruction(InstructionCode.MovPtr, imm, src);
        }

        public void Movsx(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movsx, dst, src);
        }

        public void Movsx(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movsx, dst, src);
        }

#if ASMJIT_X64
        public void Movsxd(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movsxd, dst, src);
        }

        public void Movsxd(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movsxd, dst, src);
        }
#endif

        public void Movzx(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movzx, dst, src);
        }

        public void Movzx(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movzx, dst, src);
        }

        /// <summary>
        /// Unsigned multiply
        /// </summary>
        public void MulLoHi(GPVar dstLo, GPVar dstHi, GPVar src)
        {
            Contract.Requires(dstLo != null);
            Contract.Requires(dstHi != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mul, dstLo, dstHi, src);
        }

        /// <summary>
        /// Unsigned multiply
        /// </summary>
        public void MulLoHi(GPVar dstLo, GPVar dstHi, Mem src)
        {
            Contract.Requires(dstLo != null);
            Contract.Requires(dstHi != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mul, dstLo, dstHi, src);
        }

        /// <summary>
        /// Two's complement negation
        /// </summary>
        public void Neg(GPVar dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Neg, dst);
        }

        /// <summary>
        /// Two's complement negation
        /// </summary>
        public void Neg(Mem dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Neg, dst);
        }

        /// <summary>
        /// No operation
        /// </summary>
        public void Nop()
        {
            _emitInstruction(InstructionCode.Nop);
        }

        /// <summary>
        /// One's complement negation
        /// </summary>
        public void Not(GPVar dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Not, dst);
        }

        /// <summary>
        /// One's complement negation
        /// </summary>
        public void Not(Mem dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Not, dst);
        }

        /// <summary>
        /// Logical OR.
        /// </summary>
        public void Or(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Or, dst, src);
        }

        /// <summary>
        /// Logical OR.
        /// </summary>
        public void Or(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Or, dst, src);
        }

        /// <summary>
        /// Logical OR.
        /// </summary>
        public void Or(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Or, dst, src);
        }

        /// <summary>
        /// Logical OR.
        /// </summary>
        public void Or(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Or, dst, src);
        }

        /// <summary>
        /// Logical OR.
        /// </summary>
        public void Or(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Or, dst, src);
        }

        public void Pop(GPVar dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Pop, dst);
        }

        public void Pop(Mem dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Pop, dst);
        }

#if ASMJIT_X86
        public void Popad()
        {
            _emitInstruction(InstructionCode.Popad);
        }
#endif

        public void Popf()
        {
#if ASMJIT_X86
            Popfd();
#elif ASMJIT_X64
            Popfq();
#else
            throw new NotImplementedException();
#endif
        }

#if ASMJIT_X86
        public void Popfd()
        {
            _emitInstruction(InstructionCode.Popfd);
        }
#endif

#if ASMJIT_X64
        public void Popfq()
        {
            _emitInstruction(InstructionCode.Popfq);
        }
#endif

        public void Push(GPVar src)
        {
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Push, src);
        }

        public void Push(Mem src)
        {
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Push, src);
        }

        public void Push(Imm src)
        {
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Push, src);
        }

#if ASMJIT_X86
        public void Pushad()
        {
            _emitInstruction(InstructionCode.Pushad);
        }
#endif

        public void Pushf()
        {
#if ASMJIT_X86
            Pushfd();
#elif ASMJIT_X64
            Pushfq();
#else
            throw new NotImplementedException();
#endif
        }

#if ASMJIT_X86
        public void Pushfd()
        {
            _emitInstruction(InstructionCode.Pushfd);
        }
#endif

#if ASMJIT_X64
        public void Pushfq()
        {
            _emitInstruction(InstructionCode.Pushfq);
        }
#endif

        public void Rcl(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rcl, dst, src);
        }

        public void Rcl(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rcl, dst, src);
        }

        public void Rcl(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rcl, dst, src);
        }

        public void Rcl(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rcl, dst, src);
        }

        public void Rcr(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rcr, dst, src);
        }

        public void Rcr(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rcr, dst, src);
        }

        public void Rcr(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rcr, dst, src);
        }

        public void Rcr(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rcr, dst, src);
        }

        public void Rdtsc(GPVar dst_edx, GPVar dst_eax)
        {
            Contract.Requires(dst_edx != null);
            Contract.Requires(dst_eax != null);

            if (dst_edx.Id == dst_eax.Id)
                throw new ArgumentException();

            _emitInstruction(InstructionCode.Rdtsc, dst_edx, dst_eax);
        }

        public void Rdtscp(GPVar dst_edx, GPVar dst_eax, GPVar dst_ecx)
        {
            Contract.Requires(dst_edx != null);
            Contract.Requires(dst_eax != null);
            Contract.Requires(dst_ecx != null);

            _emitInstruction(InstructionCode.Rdtscp, dst_edx, dst_eax, dst_ecx);
        }

        public void RepLodsb(GPVar dst_val, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(dst_val != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepLodsb, dst_val, src_addr, cnt_ecx);
        }

        public void RepLodsd(GPVar dst_val, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(dst_val != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepLodsd, dst_val, src_addr, cnt_ecx);
        }

#if ASMJIT_X64
        public void RepLodsq(GPVar dst_val, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(dst_val != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepLodsq, dst_val, src_addr, cnt_ecx);
        }
#endif

        public void RepLodsw(GPVar dst_val, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(dst_val != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepLodsw, dst_val, src_addr, cnt_ecx);
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

        public void RepMovsd(GPVar dst_addr, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepMovsd, dst_addr, src_addr, cnt_ecx);
        }

#if ASMJIT_X64
        public void RepMovsq(GPVar dst_addr, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepMovsq, dst_addr, src_addr, cnt_ecx);
        }
#endif

        public void RepMovsw(GPVar dst_addr, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepMovsw, dst_addr, src_addr, cnt_ecx);
        }

        public void RepStosb(GPVar dst_addr, GPVar src_val, GPVar cnt_ecx)
        {
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_val != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepStosb, dst_addr, src_val, cnt_ecx);
        }

        public void RepStosd(GPVar dst_addr, GPVar src_val, GPVar cnt_ecx)
        {
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_val != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepStosd, dst_addr, src_val, cnt_ecx);
        }

#if ASMJIT_X64
        public void RepStosq(GPVar dst_addr, GPVar src_val, GPVar cnt_ecx)
        {
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_val != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepStosq, dst_addr, src_val, cnt_ecx);
        }
#endif

        public void RepStosw(GPVar dst_addr, GPVar src_val, GPVar cnt_ecx)
        {
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_val != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepStosw, dst_addr, src_val, cnt_ecx);
        }

        public void RepeCmpsb(GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepeCmpsb, cmp1_addr, cmp2_addr, cnt_ecx);
        }

        public void RepeCmpsd(GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepeCmpsd, cmp1_addr, cmp2_addr, cnt_ecx);
        }

#if ASMJIT_X64
        public void RepeCmpsq(GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepeCmpsq, cmp1_addr, cmp2_addr, cnt_ecx);
        }
#endif

        public void RepeCmpsw(GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepeCmpsw, cmp1_addr, cmp2_addr, cnt_ecx);
        }

        public void RepeScasb(GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepeScasb, cmp1_addr, cmp2_val, cnt_ecx);
        }

        public void RepeScasd(GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepeScasd, cmp1_addr, cmp2_val, cnt_ecx);
        }

#if ASMJIT_X64
        public void RepeScasq(GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepeScasq, cmp1_addr, cmp2_val, cnt_ecx);
        }
#endif

        public void RepeScasw(GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepeScasw, cmp1_addr, cmp2_val, cnt_ecx);
        }

        public void RepneCmpsb(GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepneCmpsb, cmp1_addr, cmp2_addr, cnt_ecx);
        }

        public void RepneCmpsd(GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepneCmpsd, cmp1_addr, cmp2_addr, cnt_ecx);
        }

#if ASMJIT_X64
        public void RepneCmpsq(GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepneCmpsq, cmp1_addr, cmp2_addr, cnt_ecx);
        }
#endif

        public void RepneCmpsw(GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepneCmpsw, cmp1_addr, cmp2_addr, cnt_ecx);
        }

        public void RepneScasb(GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepneScasb, cmp1_addr, cmp2_val, cnt_ecx);
        }

        public void RepneScasd(GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepneScasd, cmp1_addr, cmp2_val, cnt_ecx);
        }

#if ASMJIT_X64
        public void RepneScasq(GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepneScasq, cmp1_addr, cmp2_val, cnt_ecx);
        }
#endif

        public void RepneScasw(GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            _emitInstruction(InstructionCode.RepneScasw, cmp1_addr, cmp2_val, cnt_ecx);
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

        public void Rol(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rol, dst, src);
        }

        public void Rol(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rol, dst, src);
        }

        public void Rol(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rol, dst, src);
        }

        public void Rol(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rol, dst, src);
        }

        public void Ror(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Ror, dst, src);
        }

        public void Ror(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Ror, dst, src);
        }

        public void Ror(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Ror, dst, src);
        }

        public void Ror(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Ror, dst, src);
        }

#if ASMJIT_X86
        public void Sahf(GPVar var)
        {
            _emitInstruction(InstructionCode.Sahf, var);
        }
#endif

        public void Sal(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sal, dst, src);
        }

        public void Sal(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sal, dst, src);
        }

        public void Sal(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sal, dst, src);
        }

        public void Sal(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sal, dst, src);
        }

        public void Sar(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sar, dst, src);
        }

        public void Sar(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sar, dst, src);
        }

        public void Sar(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sar, dst, src);
        }

        public void Sar(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sar, dst, src);
        }

        /// <summary>
        /// Integer subtraction with borrow.
        /// </summary>
        public void Sbb(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sbb, dst, src);
        }

        /// <summary>
        /// Integer subtraction with borrow.
        /// </summary>
        public void Sbb(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sbb, dst, src);
        }

        /// <summary>
        /// Integer subtraction with borrow.
        /// </summary>
        public void Sbb(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sbb, dst, src);
        }

        /// <summary>
        /// Integer subtraction with borrow.
        /// </summary>
        public void Sbb(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sbb, dst, src);
        }

        /// <summary>
        /// Integer subtraction with borrow.
        /// </summary>
        public void Sbb(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sbb, dst, src);
        }

        /// <summary>
        /// Set byte on condition
        /// </summary>
        public void Set(Condition cc, GPVar dst)
        {
            _emitInstruction(Assembler.ConditionToSetCC(cc), dst);
        }

        /// <summary>
        /// Set byte on condition
        /// </summary>
        public void Set(Condition cc, Mem dst)
        {
            _emitInstruction(Assembler.ConditionToSetCC(cc), dst);
        }

        public void Seta(GPVar dst)
        {
            _emitInstruction(InstructionCode.Seta, dst);
        }
        public void Seta(Mem dst)
        {
            _emitInstruction(InstructionCode.Seta, dst);
        }
        public void Setae(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setae, dst);
        }
        public void Setae(Mem dst)
        {
            _emitInstruction(InstructionCode.Setae, dst);
        }
        public void Setb(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setb, dst);
        }
        public void Setb(Mem dst)
        {
            _emitInstruction(InstructionCode.Setb, dst);
        }
        public void Setbe(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setbe, dst);
        }
        public void Setbe(Mem dst)
        {
            _emitInstruction(InstructionCode.Setbe, dst);
        }
        public void Setc(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setc, dst);
        }
        public void Setc(Mem dst)
        {
            _emitInstruction(InstructionCode.Setc, dst);
        }
        public void Sete(GPVar dst)
        {
            _emitInstruction(InstructionCode.Sete, dst);
        }
        public void Sete(Mem dst)
        {
            _emitInstruction(InstructionCode.Sete, dst);
        }
        public void Setg(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setg, dst);
        }
        public void Setg(Mem dst)
        {
            _emitInstruction(InstructionCode.Setg, dst);
        }
        public void Setge(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setge, dst);
        }
        public void Setge(Mem dst)
        {
            _emitInstruction(InstructionCode.Setge, dst);
        }
        public void Setl(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setl, dst);
        }
        public void Setl(Mem dst)
        {
            _emitInstruction(InstructionCode.Setl, dst);
        }
        public void Setle(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setle, dst);
        }
        public void Setle(Mem dst)
        {
            _emitInstruction(InstructionCode.Setle, dst);
        }
        public void Setna(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setna, dst);
        }
        public void Setna(Mem dst)
        {
            _emitInstruction(InstructionCode.Setna, dst);
        }
        public void Setnae(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setnae, dst);
        }
        public void Setnae(Mem dst)
        {
            _emitInstruction(InstructionCode.Setnae, dst);
        }
        public void Setnb(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setnb, dst);
        }
        public void Setnb(Mem dst)
        {
            _emitInstruction(InstructionCode.Setnb, dst);
        }
        public void Setnbe(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setnbe, dst);
        }
        public void Setnbe(Mem dst)
        {
            _emitInstruction(InstructionCode.Setnbe, dst);
        }
        public void Setnc(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setnc, dst);
        }
        public void Setnc(Mem dst)
        {
            _emitInstruction(InstructionCode.Setnc, dst);
        }
        public void Setne(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setne, dst);
        }
        public void Setne(Mem dst)
        {
            _emitInstruction(InstructionCode.Setne, dst);
        }
        public void Setng(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setng, dst);
        }
        public void Setng(Mem dst)
        {
            _emitInstruction(InstructionCode.Setng, dst);
        }
        public void Setnge(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setnge, dst);
        }
        public void Setnge(Mem dst)
        {
            _emitInstruction(InstructionCode.Setnge, dst);
        }
        public void Setnl(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setnl, dst);
        }
        public void Setnl(Mem dst)
        {
            _emitInstruction(InstructionCode.Setnl, dst);
        }
        public void Setnle(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setnle, dst);
        }
        public void Setnle(Mem dst)
        {
            _emitInstruction(InstructionCode.Setnle, dst);
        }
        public void Setno(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setno, dst);
        }
        public void Setno(Mem dst)
        {
            _emitInstruction(InstructionCode.Setno, dst);
        }
        public void Setnp(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setnp, dst);
        }
        public void Setnp(Mem dst)
        {
            _emitInstruction(InstructionCode.Setnp, dst);
        }
        public void Setns(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setns, dst);
        }
        public void Setns(Mem dst)
        {
            _emitInstruction(InstructionCode.Setns, dst);
        }
        public void Setnz(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setnz, dst);
        }
        public void Setnz(Mem dst)
        {
            _emitInstruction(InstructionCode.Setnz, dst);
        }
        public void Seto(GPVar dst)
        {
            _emitInstruction(InstructionCode.Seto, dst);
        }
        public void Seto(Mem dst)
        {
            _emitInstruction(InstructionCode.Seto, dst);
        }
        public void Setp(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setp, dst);
        }
        public void Setp(Mem dst)
        {
            _emitInstruction(InstructionCode.Setp, dst);
        }
        public void Setpe(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setpe, dst);
        }
        public void Setpe(Mem dst)
        {
            _emitInstruction(InstructionCode.Setpe, dst);
        }
        public void Setpo(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setpo, dst);
        }
        public void Setpo(Mem dst)
        {
            _emitInstruction(InstructionCode.Setpo, dst);
        }
        public void Sets(GPVar dst)
        {
            _emitInstruction(InstructionCode.Sets, dst);
        }
        public void Sets(Mem dst)
        {
            _emitInstruction(InstructionCode.Sets, dst);
        }
        public void Setz(GPVar dst)
        {
            _emitInstruction(InstructionCode.Setz, dst);
        }
        public void Setz(Mem dst)
        {
            _emitInstruction(InstructionCode.Setz, dst);
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

        public void Shr(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Shr, dst, src);
        }

        public void Shr(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Shr, dst, src);
        }

        public void Shr(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Shr, dst, src);
        }

        public void Shr(Mem dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Shr, dst, src);
        }

        public void Shld(GPVar dst, GPVar src1, GPVar src2)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            _emitInstruction(InstructionCode.Shld, dst, src1, src2);
        }

        public void Shld(GPVar dst, GPVar src1, Imm src2)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            _emitInstruction(InstructionCode.Shld, dst, src1, src2);
        }

        public void Shld(Mem dst, GPVar src1, GPVar src2)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            _emitInstruction(InstructionCode.Shld, dst, src1, src2);
        }

        public void Shld(Mem dst, GPVar src1, Imm src2)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            _emitInstruction(InstructionCode.Shld, dst, src1, src2);
        }

        public void Shrd(GPVar dst, GPVar src1, GPVar src2)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            _emitInstruction(InstructionCode.Shrd, dst, src1, src2);
        }

        public void Shrd(GPVar dst, GPVar src1, Imm src2)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            _emitInstruction(InstructionCode.Shrd, dst, src1, src2);
        }

        public void Shrd(Mem dst, GPVar src1, GPVar src2)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            _emitInstruction(InstructionCode.Shrd, dst, src1, src2);
        }

        public void Shrd(Mem dst, GPVar src1, Imm src2)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            _emitInstruction(InstructionCode.Shrd, dst, src1, src2);
        }

        public void Stc()
        {
            _emitInstruction(InstructionCode.Stc);
        }

        public void Std()
        {
            _emitInstruction(InstructionCode.Std);
        }

        public void Sub(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sub, dst, src);
        }

        public void Sub(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sub, dst, src);
        }

        public void Sub(GPVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sub, dst, src);
        }

        public void Sub(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sub, dst, src);
        }

        public void Sub(Mem dst, Imm src)
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

        public void Ud2()
        {
            _emitInstruction(InstructionCode.Ud2);
        }

        public void Xadd(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Xadd, dst, src);
        }

        public void Xadd(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Xadd, dst, src);
        }

        public void Xchg(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Xchg, dst, src);
        }

        public void Xchg(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Xchg, dst, src);
        }

        public void Xchg(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Xchg, dst, src);
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

        #region X87 Instructions

        public void Emms()
        {
            _emitInstruction(InstructionCode.Emms);
        }

        #endregion

        #region MMX Instructions

        public void Movd(Mem dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movd, dst, src);
        }

        public void Movd(GPVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movd, dst, src);
        }

        public void Movd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movd, dst, src);
        }

        public void Movd(MMVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movd, dst, src);
        }

        public void Movq(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movq, dst, src);
        }

        public void Movq(Mem dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movq, dst, src);
        }

#if ASMJIT_X64
        public void Movq(GPVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movq, dst, src);
        }
#endif

        public void Movq(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movq, dst, src);
        }

#if ASMJIT_X64
        public void Movq(MMVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movq, dst, src);
        }
#endif

        public void Packuswb(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Packuswb, dst, src);
        }

        public void Packuswb(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Packuswb, dst, src);
        }

        public void Paddb(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddb, dst, src);
        }

        public void Paddb(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddb, dst, src);
        }

        public void Paddw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddw, dst, src);
        }

        public void Paddw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddw, dst, src);
        }

        public void Paddd(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddd, dst, src);
        }

        public void Paddd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddd, dst, src);
        }

        public void Paddsb(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddsb, dst, src);
        }

        public void Paddsb(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddsb, dst, src);
        }

        public void Paddsw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddsw, dst, src);
        }

        public void Paddsw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddsw, dst, src);
        }

        public void Paddusb(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddusb, dst, src);
        }

        public void Paddusb(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddusb, dst, src);
        }

        public void Paddusw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddusw, dst, src);
        }

        public void Paddusw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddusw, dst, src);
        }

        public void Pand(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pand, dst, src);
        }

        public void Pand(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pand, dst, src);
        }

        public void Pandn(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pandn, dst, src);
        }

        public void Pandn(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pandn, dst, src);
        }

        public void Pcmpeqb(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpeqb, dst, src);
        }

        public void Pcmpeqb(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpeqb, dst, src);
        }

        public void Pcmpeqw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpeqw, dst, src);
        }

        public void Pcmpeqw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpeqw, dst, src);
        }

        public void Pcmpeqd(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpeqd, dst, src);
        }

        public void Pcmpeqd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpeqd, dst, src);
        }

        public void Pcmpgtb(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpgtb, dst, src);
        }

        public void Pcmpgtb(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpgtb, dst, src);
        }

        public void Pcmpgtw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpgtw, dst, src);
        }

        public void Pcmpgtw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpgtw, dst, src);
        }

        public void Pcmpgtd(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpgtd, dst, src);
        }

        public void Pcmpgtd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpgtd, dst, src);
        }

        public void Pmulhw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmulhw, dst, src);
        }

        public void Pmulhw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmulhw, dst, src);
        }

        public void Pmullw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmullw, dst, src);
        }

        public void Pmullw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmullw, dst, src);
        }

        public void Por(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Por, dst, src);
        }

        public void Por(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Por, dst, src);
        }

        public void Pmaddwd(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaddwd, dst, src);
        }

        public void Pmaddwd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaddwd, dst, src);
        }

        public void Pslld(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pslld, dst, src);
        }

        public void Pslld(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pslld, dst, src);
        }

        public void Pslld(MMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pslld, dst, src);
        }

        public void Psllq(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psllq, dst, src);
        }

        public void Psllq(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psllq, dst, src);
        }

        public void Psllq(MMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psllq, dst, src);
        }

        public void Psllw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psllw, dst, src);
        }

        public void Psllw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psllw, dst, src);
        }

        public void Psllw(MMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psllw, dst, src);
        }

        public void Psrad(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrad, dst, src);
        }

        public void Psrad(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrad, dst, src);
        }

        public void Psrad(MMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrad, dst, src);
        }

        public void Psraw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psraw, dst, src);
        }

        public void Psraw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psraw, dst, src);
        }

        public void Psraw(MMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psraw, dst, src);
        }

        public void Psrld(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrld, dst, src);
        }

        public void Psrld(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrld, dst, src);
        }

        public void Psrld(MMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrld, dst, src);
        }

        public void Psrlq(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrlq, dst, src);
        }

        public void Psrlq(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrlq, dst, src);
        }

        public void Psrlq(MMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrlq, dst, src);
        }

        public void Psrlw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrlw, dst, src);
        }

        public void Psrlw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrlw, dst, src);
        }

        public void Psrlw(MMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrlw, dst, src);
        }

        public void Psubb(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubb, dst, src);
        }

        public void Psubb(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubb, dst, src);
        }

        public void Psubw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubw, dst, src);
        }

        public void Psubw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubw, dst, src);
        }

        public void Psubd(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubd, dst, src);
        }

        public void Psubd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubd, dst, src);
        }

        public void Psubsb(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubsb, dst, src);
        }

        public void Psubsb(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubsb, dst, src);
        }

        public void Psubsw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubsw, dst, src);
        }

        public void Psubsw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubsw, dst, src);
        }

        public void Psubusb(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubusb, dst, src);
        }

        public void Psubusb(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubusb, dst, src);
        }

        public void Psubusw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubusw, dst, src);
        }

        public void Psubusw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubusw, dst, src);
        }

        public void Punpckhbw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckhbw, dst, src);
        }

        public void Punpckhbw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckhbw, dst, src);
        }

        public void Punpckhwd(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckhwd, dst, src);
        }

        public void Punpckhwd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckhwd, dst, src);
        }

        public void Punpckhdq(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckhdq, dst, src);
        }

        public void Punpckhdq(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckhdq, dst, src);
        }

        public void Punpcklbw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpcklbw, dst, src);
        }

        public void Punpcklbw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpcklbw, dst, src);
        }

        public void Punpcklwd(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpcklwd, dst, src);
        }

        public void Punpcklwd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpcklwd, dst, src);
        }

        public void Punpckldq(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckldq, dst, src);
        }

        public void Punpckldq(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckldq, dst, src);
        }

        public void Pxor(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pxor, dst, src);
        }

        public void Pxor(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pxor, dst, src);
        }

        #endregion

        #region AMD 3D-Now! Instructions

        public void Femms()
        {
            _emitInstruction(InstructionCode.Femms);
        }

        public void Pf2id(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pf2id, dst, src);
        }

        public void Pf2id(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pf2id, dst, src);
        }

        public void Pf2iw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pf2iw, dst, src);
        }

        public void Pf2iw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pf2iw, dst, src);
        }

        public void Pfacc(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfacc, dst, src);
        }

        public void Pfacc(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfacc, dst, src);
        }

        public void Pfadd(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfadd, dst, src);
        }

        public void Pfadd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfadd, dst, src);
        }

        public void Pfcmpeq(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfcmpeq, dst, src);
        }

        public void Pfcmpeq(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfcmpeq, dst, src);
        }

        public void Pfcmpge(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfcmpge, dst, src);
        }

        public void Pfcmpge(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfcmpge, dst, src);
        }

        public void Pfcmpgt(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfcmpgt, dst, src);
        }

        public void Pfcmpgt(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfcmpgt, dst, src);
        }

        public void Pfmax(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfmax, dst, src);
        }

        public void Pfmax(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfmax, dst, src);
        }

        public void Pfmin(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfmin, dst, src);
        }

        public void Pfmin(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfmin, dst, src);
        }

        public void Pfmul(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfmul, dst, src);
        }

        public void Pfmul(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfmul, dst, src);
        }

        public void Pfnacc(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfnacc, dst, src);
        }

        public void Pfnacc(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfnacc, dst, src);
        }

        public void Pfpnacc(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfpnacc, dst, src);
        }

        public void Pfpnacc(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfpnacc, dst, src);
        }

        public void Pfrcp(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfrcp, dst, src);
        }

        public void Pfrcp(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfrcp, dst, src);
        }

        public void Pfrcpit1(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfrcpit1, dst, src);
        }

        public void Pfrcpit1(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfrcpit1, dst, src);
        }

        public void Pfrcpit2(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfrcpit2, dst, src);
        }

        public void Pfrcpit2(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfrcpit2, dst, src);
        }

        public void Pfrsqit1(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfrsqit1, dst, src);
        }

        public void Pfrsqit1(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfrsqit1, dst, src);
        }

        public void Pfrsqrt(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfrsqrt, dst, src);
        }

        public void Pfrsqrt(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfrsqrt, dst, src);
        }

        public void Pfsub(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfsub, dst, src);
        }

        public void Pfsub(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfsub, dst, src);
        }

        public void Pfsubr(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfsubr, dst, src);
        }

        public void Pfsubr(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pfsubr, dst, src);
        }

        public void Pi2fd(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pi2fd, dst, src);
        }

        public void Pi2fd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pi2fd, dst, src);
        }

        public void Pi2fw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pi2fw, dst, src);
        }

        public void Pi2fw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pi2fw, dst, src);
        }

        public void Pswapd(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pswapd, dst, src);
        }

        public void Pswapd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pswapd, dst, src);
        }

        #endregion

        #region SSE Instructions

        public void Addps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Addps, dst, src);
        }

        public void Addps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Addps, dst, src);
        }

        public void Addss(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Addss, dst, src);
        }

        public void Addss(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Addss, dst, src);
        }

        public void Andnps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Andnps, dst, src);
        }

        public void Andnps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Andnps, dst, src);
        }

        public void Andps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Andps, dst, src);
        }

        public void Andps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Andps, dst, src);
        }

        public void Cmpps(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Cmpps, dst, src, imm8);
        }

        public void Cmpps(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Cmpps, dst, src, imm8);
        }

        public void Cmpss(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Cmpss, dst, src, imm8);
        }

        public void Cmpss(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Cmpss, dst, src, imm8);
        }

        public void Comiss(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Comiss, dst, src);
        }

        public void Comiss(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Comiss, dst, src);
        }

        public void Cvtpi2ps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtpi2ps, dst, src);
        }

        public void Cvtpi2ps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtpi2ps, dst, src);
        }

        public void Cvtps2pi(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtps2pi, dst, src);
        }

        public void Cvtps2pi(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtps2pi, dst, src);
        }

        public void Cvtsi2ss(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtsi2ss, dst, src);
        }

        public void Cvtsi2ss(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtsi2ss, dst, src);
        }

        public void Cvtss2si(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtss2si, dst, src);
        }

        public void Cvtss2si(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtss2si, dst, src);
        }

        public void Cvttps2pi(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvttps2pi, dst, src);
        }

        public void Cvttps2pi(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvttps2pi, dst, src);
        }

        public void Cvttss2si(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvttss2si, dst, src);
        }

        public void Cvttss2si(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvttss2si, dst, src);
        }

        public void Divps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Divps, dst, src);
        }

        public void Divps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Divps, dst, src);
        }

        public void Divss(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Divss, dst, src);
        }

        public void Divss(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Divss, dst, src);
        }

        public void Ldmxcsr(Mem src)
        {
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Ldmxcsr, src);
        }

        public void Maskmovq(GPVar dst_ptr, MMVar data, MMVar mask)
        {
            Contract.Requires(dst_ptr != null);
            Contract.Requires(data != null);
            Contract.Requires(mask != null);

            _emitInstruction(InstructionCode.Maskmovq, dst_ptr, data, mask);
        }

        public void Maxps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Maxps, dst, src);
        }

        public void Maxps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Maxps, dst, src);
        }

        public void Maxss(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Maxss, dst, src);
        }

        public void Maxss(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Maxss, dst, src);
        }

        public void Minps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Minps, dst, src);
        }

        public void Minps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Minps, dst, src);
        }

        public void Minss(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Minss, dst, src);
        }

        public void Minss(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Minss, dst, src);
        }

        public void Movaps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movaps, dst, src);
        }

        public void Movaps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movaps, dst, src);
        }

        public void Movaps(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movaps, dst, src);
        }

        public void Movd(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movd, dst, src);
        }

        public void Movd(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movd, dst, src);
        }

        public void Movd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movd, dst, src);
        }

        public void Movd(XMMVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movd, dst, src);
        }

        public void Movq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movq, dst, src);
        }

        public void Movq(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movq, dst, src);
        }

#if ASMJIT_X64
        public void Movq(GPVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movq, dst, src);
        }
#endif

        public void Movq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movq, dst, src);
        }

#if ASMJIT_X64
        public void Movq(XMMVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movq, dst, src);
        }
#endif

        public void Movntq(Mem dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movntq, dst, src);
        }

        public void Movhlps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movhlps, dst, src);
        }

        public void Movhps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movhps, dst, src);
        }

        public void Movhps(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movhps, dst, src);
        }

        public void Movlhps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movlhps, dst, src);
        }

        public void Movlps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movlps, dst, src);
        }

        public void Movlps(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movlps, dst, src);
        }

        public void Movntps(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movntps, dst, src);
        }

        public void Movss(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movss, dst, src);
        }

        public void Movss(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movss, dst, src);
        }

        public void Movss(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movss, dst, src);
        }

        public void Movups(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movups, dst, src);
        }

        public void Movups(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movups, dst, src);
        }

        public void Movups(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movups, dst, src);
        }

        public void Mulps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mulps, dst, src);
        }

        public void Mulps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mulps, dst, src);
        }

        public void Mulss(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mulss, dst, src);
        }

        public void Mulss(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mulss, dst, src);
        }

        public void Orps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Orps, dst, src);
        }

        public void Orps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Orps, dst, src);
        }

        public void Pavgb(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pavgb, dst, src);
        }

        public void Pavgb(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pavgb, dst, src);
        }

        public void Pavgw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pavgw, dst, src);
        }

        public void Pavgw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pavgw, dst, src);
        }

        public void Pextrw(GPVar dst, MMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pextrw, dst, src, imm8);
        }

        public void Pinsrw(MMVar dst, GPVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pinsrw, dst, src, imm8);
        }

        public void Pinsrw(MMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pinsrw, dst, src, imm8);
        }

        public void Pmaxsw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxsw, dst, src);
        }

        public void Pmaxsw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxsw, dst, src);
        }

        public void Pmaxub(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxub, dst, src);
        }

        public void Pmaxub(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxub, dst, src);
        }

        public void Pminsw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminsw, dst, src);
        }

        public void Pminsw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminsw, dst, src);
        }

        public void Pminub(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminub, dst, src);
        }

        public void Pminub(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminub, dst, src);
        }

        public void Pmovmskb(GPVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovmskb, dst, src);
        }

        public void Pmulhuw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmulhuw, dst, src);
        }

        public void Pmulhuw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmulhuw, dst, src);
        }

        public void Psadbw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psadbw, dst, src);
        }

        public void Psadbw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psadbw, dst, src);
        }

        public void Pshufw(MMVar dst, MMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pshufw, dst, src, imm8);
        }

        public void Pshufw(MMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pshufw, dst, src, imm8);
        }

        public void Rcpps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rcpps, dst, src);
        }

        public void Rcpps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rcpps, dst, src);
        }

        public void Rcpss(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rcpss, dst, src);
        }

        public void Rcpss(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rcpss, dst, src);
        }

        public void Prefetch(Mem mem, Imm hint)
        {
            Contract.Requires(mem != null);
            Contract.Requires(hint != null);

            _emitInstruction(InstructionCode.Prefetch, mem, hint);
        }

        public void Psadbw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psadbw, dst, src);
        }

        public void Psadbw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psadbw, dst, src);
        }

        public void Rsqrtps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rsqrtps, dst, src);
        }

        public void Rsqrtps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rsqrtps, dst, src);
        }

        public void Rsqrtss(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rsqrtss, dst, src);
        }

        public void Rsqrtss(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Rsqrtss, dst, src);
        }

        public void Sfence()
        {
            _emitInstruction(InstructionCode.Sfence);
        }

        public void Shufps(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Shufps, dst, src, imm8);
        }

        public void Shufps(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Shufps, dst, src, imm8);
        }

        public void Sqrtps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sqrtps, dst, src);
        }

        public void Sqrtps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sqrtps, dst, src);
        }

        public void Sqrtss(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sqrtss, dst, src);
        }

        public void Sqrtss(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sqrtss, dst, src);
        }

        public void Stmxcsr(Mem dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Stmxcsr, dst);
        }

        public void Subps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Subps, dst, src);
        }

        public void Subps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Subps, dst, src);
        }

        public void Subss(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Subss, dst, src);
        }

        public void Subss(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Subss, dst, src);
        }

        public void Ucomiss(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Ucomiss, dst, src);
        }

        public void Ucomiss(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Ucomiss, dst, src);
        }

        public void Unpckhps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Unpckhps, dst, src);
        }

        public void Unpckhps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Unpckhps, dst, src);
        }

        public void Unpcklps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Unpcklps, dst, src);
        }

        public void Unpcklps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Unpcklps, dst, src);
        }

        public void Xorps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Xorps, dst, src);
        }

        public void Xorps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Xorps, dst, src);
        }

        #endregion

        #region SSE2

        public void Addpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Addpd, dst, src);
        }

        public void Addpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Addpd, dst, src);
        }

        public void Addsd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Addsd, dst, src);
        }

        public void Addsd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Addsd, dst, src);
        }

        public void Andnpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Andnpd, dst, src);
        }

        public void Andnpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Andnpd, dst, src);
        }

        public void Andpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Andpd, dst, src);
        }

        public void Andpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Andpd, dst, src);
        }

        public void Clflush(Mem mem)
        {
            Contract.Requires(mem != null);

            _emitInstruction(InstructionCode.Clflush, mem);
        }

        public void Cmppd(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Cmppd, dst, src, imm8);
        }

        public void Cmppd(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Cmppd, dst, src, imm8);
        }

        public void Cmpsd(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Cmpsd, dst, src, imm8);
        }

        public void Cmpsd(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Cmpsd, dst, src, imm8);
        }

        public void Comisd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Comisd, dst, src);
        }

        public void Comisd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Comisd, dst, src);
        }

        public void Cvtdq2pd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtdq2pd, dst, src);
        }

        public void Cvtdq2pd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtdq2pd, dst, src);
        }

        public void Cvtdq2ps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtdq2ps, dst, src);
        }

        public void Cvtdq2ps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtdq2ps, dst, src);
        }

        public void Cvtpd2dq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtpd2dq, dst, src);
        }

        public void Cvtpd2dq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtpd2dq, dst, src);
        }

        public void Cvtpd2pi(MMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtpd2pi, dst, src);
        }

        public void Cvtpd2pi(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtpd2pi, dst, src);
        }

        public void Cvtpd2ps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtpd2ps, dst, src);
        }

        public void Cvtpd2ps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtpd2ps, dst, src);
        }

        public void Cvtpi2pd(XMMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtpi2pd, dst, src);
        }

        public void Cvtpi2pd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtpi2pd, dst, src);
        }

        public void Cvtps2dq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtps2dq, dst, src);
        }

        public void Cvtps2dq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtps2dq, dst, src);
        }

        public void Cvtps2pd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtps2pd, dst, src);
        }

        public void Cvtps2pd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtps2pd, dst, src);
        }

        public void Cvtsd2si(GPVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtsd2si, dst, src);
        }

        public void Cvtsd2si(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtsd2si, dst, src);
        }

        public void Cvtsd2ss(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtsd2ss, dst, src);
        }

        public void Cvtsd2ss(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtsd2ss, dst, src);
        }

        public void Cvtsi2sd(XMMVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtsi2sd, dst, src);
        }

        public void Cvtsi2sd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtsi2sd, dst, src);
        }

        public void Cvtss2sd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtss2sd, dst, src);
        }

        public void Cvtss2sd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvtss2sd, dst, src);
        }

        public void Cvttpd2pi(MMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvttpd2pi, dst, src);
        }

        public void Cvttpd2pi(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvttpd2pi, dst, src);
        }

        public void Cvttpd2dq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvttpd2dq, dst, src);
        }

        public void Cvttpd2dq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvttpd2dq, dst, src);
        }

        public void Cvttps2dq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvttps2dq, dst, src);
        }

        public void Cvttps2dq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvttps2dq, dst, src);
        }

        public void Cvttsd2si(GPVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvttsd2si, dst, src);
        }

        public void Cvttsd2si(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Cvttsd2si, dst, src);
        }

        public void Divpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Divpd, dst, src);
        }

        public void Divpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Divpd, dst, src);
        }

        public void Divsd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Divsd, dst, src);
        }

        public void Divsd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Divsd, dst, src);
        }

        public void Lfence()
        {
            _emitInstruction(InstructionCode.Lfence);
        }

        public void Maskmovdqu(GPVar dst_ptr, XMMVar src, XMMVar mask)
        {
            Contract.Requires(dst_ptr != null);
            Contract.Requires(src != null);
            Contract.Requires(mask != null);

            _emitInstruction(InstructionCode.Maskmovdqu, dst_ptr, src, mask);
        }

        public void Maxpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Maxpd, dst, src);
        }

        public void Maxpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Maxpd, dst, src);
        }

        public void Maxsd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Maxsd, dst, src);
        }

        public void Maxsd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Maxsd, dst, src);
        }

        public void Mfence()
        {
            _emitInstruction(InstructionCode.Mfence);
        }

        public void Minpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Minpd, dst, src);
        }

        public void Minpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Minpd, dst, src);
        }

        public void Minsd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Minsd, dst, src);
        }

        public void Minsd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Minsd, dst, src);
        }

        public void Movdqa(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movdqa, dst, src);
        }

        public void Movdqa(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movdqa, dst, src);
        }

        public void Movdqa(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movdqa, dst, src);
        }

        public void Movdqu(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movdqu, dst, src);
        }

        public void Movdqu(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movdqu, dst, src);
        }

        public void Movdqu(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movdqu, dst, src);
        }

        public void Movmskps(GPVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movmskps, dst, src);
        }

        public void Movmskpd(GPVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movmskpd, dst, src);
        }

        public void Movsd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movsd, dst, src);
        }

        public void Movsd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movsd, dst, src);
        }

        public void Movsd(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movsd, dst, src);
        }

        public void Movapd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movapd, dst, src);
        }

        public void Movapd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movapd, dst, src);
        }

        public void Movapd(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movapd, dst, src);
        }

        public void Movdq2q(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movdq2q, dst, src);
        }

        public void Movdq2q(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movdq2q, dst, src);
        }

        public void Movhpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movhpd, dst, src);
        }

        public void Movhpd(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movhpd, dst, src);
        }

        public void Movlpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movlpd, dst, src);
        }

        public void Movlpd(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movlpd, dst, src);
        }

        public void Movntdq(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movntdq, dst, src);
        }

        public void Movnti(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movnti, dst, src);
        }

        public void Movntpd(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movntpd, dst, src);
        }

        public void Movupd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movupd, dst, src);
        }

        public void Movupd(Mem dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movupd, dst, src);
        }

        public void Mulpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mulpd, dst, src);
        }

        public void Mulpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mulpd, dst, src);
        }

        public void Mulsd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mulsd, dst, src);
        }

        public void Mulsd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Mulsd, dst, src);
        }

        public void Orpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Orpd, dst, src);
        }

        public void Orpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Orpd, dst, src);
        }

        public void Packsswb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Packsswb, dst, src);
        }

        public void Packsswb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Packsswb, dst, src);
        }

        public void Packssdw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Packssdw, dst, src);
        }

        public void Packssdw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Packssdw, dst, src);
        }

        public void Packuswb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Packuswb, dst, src);
        }

        public void Packuswb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Packuswb, dst, src);
        }

        public void Paddb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddb, dst, src);
        }

        public void Paddb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddb, dst, src);
        }

        public void Paddw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddw, dst, src);
        }

        public void Paddw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddw, dst, src);
        }

        public void Paddd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddd, dst, src);
        }

        public void Paddd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddd, dst, src);
        }

        public void Paddq(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddq, dst, src);
        }

        public void Paddq(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddq, dst, src);
        }

        public void Paddq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddq, dst, src);
        }

        public void Paddq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddq, dst, src);
        }

        public void Paddsb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddsb, dst, src);
        }

        public void Paddsb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddsb, dst, src);
        }

        public void Paddsw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddsw, dst, src);
        }

        public void Paddsw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddsw, dst, src);
        }

        public void Paddusb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddusb, dst, src);
        }

        public void Paddusb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddusb, dst, src);
        }

        public void Paddusw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddusw, dst, src);
        }

        public void Paddusw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Paddusw, dst, src);
        }

        public void Pand(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pand, dst, src);
        }

        public void Pand(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pand, dst, src);
        }

        public void Pandn(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pandn, dst, src);
        }

        public void Pandn(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pandn, dst, src);
        }

        public void Pause()
        {
            _emitInstruction(InstructionCode.Pause);
        }

        public void Pavgb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pavgb, dst, src);
        }

        public void Pavgb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pavgb, dst, src);
        }

        public void Pavgw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pavgw, dst, src);
        }

        public void Pavgw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pavgw, dst, src);
        }

        public void Pcmpeqb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpeqb, dst, src);
        }

        public void Pcmpeqb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpeqb, dst, src);
        }

        public void Pcmpeqw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpeqw, dst, src);
        }

        public void Pcmpeqw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpeqw, dst, src);
        }

        public void Pcmpeqd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpeqd, dst, src);
        }

        public void Pcmpeqd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpeqd, dst, src);
        }

        public void Pcmpgtb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpgtb, dst, src);
        }

        public void Pcmpgtb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpgtb, dst, src);
        }

        public void Pcmpgtw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpgtw, dst, src);
        }

        public void Pcmpgtw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpgtw, dst, src);
        }

        public void Pcmpgtd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpgtd, dst, src);
        }

        public void Pcmpgtd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpgtd, dst, src);
        }

        public void Pmaxsw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxsw, dst, src);
        }

        public void Pmaxsw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxsw, dst, src);
        }

        public void Pmaxub(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxub, dst, src);
        }

        public void Pmaxub(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxub, dst, src);
        }

        public void Pminsw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminsw, dst, src);
        }

        public void Pminsw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminsw, dst, src);
        }

        public void Pminub(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminub, dst, src);
        }

        public void Pminub(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminub, dst, src);
        }

        public void Pmovmskb(GPVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovmskb, dst, src);
        }

        public void Pmulhw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmulhw, dst, src);
        }

        public void Pmulhw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmulhw, dst, src);
        }

        public void Pmulhuw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmulhuw, dst, src);
        }

        public void Pmulhuw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmulhuw, dst, src);
        }

        public void Pmullw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmullw, dst, src);
        }

        public void Pmullw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmullw, dst, src);
        }

        public void Pmuludq(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmuludq, dst, src);
        }

        public void Pmuludq(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmuludq, dst, src);
        }

        public void Pmuludq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmuludq, dst, src);
        }

        public void Pmuludq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmuludq, dst, src);
        }

        public void Por(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Por, dst, src);
        }

        public void Por(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Por, dst, src);
        }

        public void Pslld(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pslld, dst, src);
        }

        public void Pslld(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pslld, dst, src);
        }

        public void Pslld(XMMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pslld, dst, src);
        }

        public void Psllq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psllq, dst, src);
        }

        public void Psllq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psllq, dst, src);
        }

        public void Psllq(XMMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psllq, dst, src);
        }

        public void Psllw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psllw, dst, src);
        }

        public void Psllw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psllw, dst, src);
        }

        public void Psllw(XMMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psllw, dst, src);
        }

        public void Pslldq(XMMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pslldq, dst, src);
        }

        public void Psrad(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrad, dst, src);
        }

        public void Psrad(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrad, dst, src);
        }

        public void Psrad(XMMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrad, dst, src);
        }

        public void Psraw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psraw, dst, src);
        }

        public void Psraw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psraw, dst, src);
        }

        public void Psraw(XMMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psraw, dst, src);
        }

        public void Psubb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubb, dst, src);
        }

        public void Psubb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubb, dst, src);
        }

        public void Psubw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubw, dst, src);
        }

        public void Psubw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubw, dst, src);
        }

        public void Psubd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubd, dst, src);
        }

        public void Psubd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubd, dst, src);
        }

        public void Psubq(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubq, dst, src);
        }

        public void Psubq(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubq, dst, src);
        }

        public void Psubq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubq, dst, src);
        }

        public void Psubq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubq, dst, src);
        }

        public void Pmaddwd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaddwd, dst, src);
        }

        public void Pmaddwd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaddwd, dst, src);
        }

        public void Pshufd(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pshufd, dst, src, imm8);
        }

        public void Pshufd(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pshufd, dst, src, imm8);
        }

        public void Pshufhw(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pshufhw, dst, src, imm8);
        }

        public void Pshufhw(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pshufhw, dst, src, imm8);
        }

        public void Pshuflw(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pshuflw, dst, src, imm8);
        }

        public void Pshuflw(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pshuflw, dst, src, imm8);
        }

        public void Psrld(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrld, dst, src);
        }

        public void Psrld(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrld, dst, src);
        }

        public void Psrld(XMMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrld, dst, src);
        }

        public void Psrlq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrlq, dst, src);
        }

        public void Psrlq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrlq, dst, src);
        }

        public void Psrlq(XMMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrlq, dst, src);
        }

        public void Psrldq(XMMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrldq, dst, src);
        }

        public void Psrlw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrlw, dst, src);
        }

        public void Psrlw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrlw, dst, src);
        }

        public void Psrlw(XMMVar dst, Imm src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psrlw, dst, src);
        }

        public void Psubsb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubsb, dst, src);
        }

        public void Psubsb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubsb, dst, src);
        }

        public void Psubsw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubsw, dst, src);
        }

        public void Psubsw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubsw, dst, src);
        }

        public void Psubusb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubusb, dst, src);
        }

        public void Psubusb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubusb, dst, src);
        }

        public void Psubusw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubusw, dst, src);
        }

        public void Psubusw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psubusw, dst, src);
        }

        public void Punpckhbw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckhbw, dst, src);
        }

        public void Punpckhbw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckhbw, dst, src);
        }

        public void Punpckhwd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckhwd, dst, src);
        }

        public void Punpckhwd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckhwd, dst, src);
        }

        public void Punpckhdq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckhdq, dst, src);
        }

        public void Punpckhdq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckhdq, dst, src);
        }

        public void Punpckhqdq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckhqdq, dst, src);
        }

        public void Punpckhqdq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckhqdq, dst, src);
        }

        public void Punpcklbw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpcklbw, dst, src);
        }

        public void Punpcklbw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpcklbw, dst, src);
        }

        public void Punpcklwd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpcklwd, dst, src);
        }

        public void Punpcklwd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpcklwd, dst, src);
        }

        public void Punpckldq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckldq, dst, src);
        }

        public void Punpckldq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpckldq, dst, src);
        }

        public void Punpcklqdq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpcklqdq, dst, src);
        }

        public void Punpcklqdq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Punpcklqdq, dst, src);
        }

        public void Pxor(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pxor, dst, src);
        }

        public void Pxor(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pxor, dst, src);
        }

        public void Shufpd(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Shufpd, dst, src, imm8);
        }

        public void Shufpd(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Shufpd, dst, src, imm8);
        }

        public void Sqrtpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sqrtpd, dst, src);
        }

        public void Sqrtpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sqrtpd, dst, src);
        }

        public void Sqrtsd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sqrtsd, dst, src);
        }

        public void Sqrtsd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Sqrtsd, dst, src);
        }

        public void Subpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Subpd, dst, src);
        }

        public void Subpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Subpd, dst, src);
        }

        public void Subsd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Subsd, dst, src);
        }

        public void Subsd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Subsd, dst, src);
        }

        public void Ucomisd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Ucomisd, dst, src);
        }

        public void Ucomisd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Ucomisd, dst, src);
        }

        public void Unpckhpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Unpckhpd, dst, src);
        }

        public void Unpckhpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Unpckhpd, dst, src);
        }

        public void Unpcklpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Unpcklpd, dst, src);
        }

        public void Unpcklpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Unpcklpd, dst, src);
        }

        public void Xorpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Xorpd, dst, src);
        }

        public void Xorpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Xorpd, dst, src);
        }

        #endregion

        #region SSE3

        public void Addsubpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Addsubpd, dst, src);
        }

        public void Addsubpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Addsubpd, dst, src);
        }

        public void Addsubps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Addsubps, dst, src);
        }

        public void Addsubps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Addsubps, dst, src);
        }

#if ASMJIT_NOT_SUPPORTED_BY_COMPILER
        public void Fisttp(Mem dst)
        {
            Contract.Requires(dst != null);

            _emitInstruction(InstructionCode.Fisttp, dst);
        }
#endif

        public void Haddpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Haddpd, dst, src);
        }

        public void Haddpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Haddpd, dst, src);
        }

        public void Haddps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Haddps, dst, src);
        }

        public void Haddps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Haddps, dst, src);
        }

        public void Hsubpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Hsubpd, dst, src);
        }

        public void Hsubpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Hsubpd, dst, src);
        }

        public void Hsubps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Hsubps, dst, src);
        }

        public void Hsubps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Hsubps, dst, src);
        }

        public void Lddqu(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Lddqu, dst, src);
        }

#if ASMJIT_NOT_SUPPORTED_BY_COMPILER
        public void Monitor()
        {
            _emitInstruction(InstructionCode.Monitor);
        }
#endif

        public void Movddup(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movddup, dst, src);
        }

        public void Movddup(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movddup, dst, src);
        }

        public void Movshdup(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movshdup, dst, src);
        }

        public void Movshdup(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movshdup, dst, src);
        }

        public void Movsldup(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movsldup, dst, src);
        }

        public void Movsldup(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movsldup, dst, src);
        }

#if ASMJIT_NOT_SUPPORTED_BY_COMPILER
        public void Mwait()
        {
            _emitInstruction(InstructionCode.Mwait);
        }
#endif

        #endregion

        #region SSSE3

        public void Psignb(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psignb, dst, src);
        }

        public void Psignb(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psignb, dst, src);
        }

        public void Psignb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psignb, dst, src);
        }

        public void Psignb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psignb, dst, src);
        }

        public void Psignw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psignw, dst, src);
        }

        public void Psignw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psignw, dst, src);
        }

        public void Psignw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psignw, dst, src);
        }

        public void Psignw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psignw, dst, src);
        }

        public void Psignd(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psignd, dst, src);
        }

        public void Psignd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psignd, dst, src);
        }

        public void Psignd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psignd, dst, src);
        }

        public void Psignd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Psignd, dst, src);
        }

        public void Phaddw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phaddw, dst, src);
        }

        public void Phaddw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phaddw, dst, src);
        }

        public void Phaddw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phaddw, dst, src);
        }

        public void Phaddw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phaddw, dst, src);
        }

        public void Phaddd(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phaddd, dst, src);
        }

        public void Phaddd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phaddd, dst, src);
        }

        public void Phaddd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phaddd, dst, src);
        }

        public void Phaddd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phaddd, dst, src);
        }

        public void Phaddsw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phaddsw, dst, src);
        }

        public void Phaddsw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phaddsw, dst, src);
        }

        public void Phaddsw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phaddsw, dst, src);
        }

        public void Phaddsw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phaddsw, dst, src);
        }

        public void Phsubw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phsubw, dst, src);
        }

        public void Phsubw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phsubw, dst, src);
        }

        public void Phsubw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phsubw, dst, src);
        }

        public void Phsubw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phsubw, dst, src);
        }

        public void Phsubd(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phsubd, dst, src);
        }

        public void Phsubd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phsubd, dst, src);
        }

        public void Phsubd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phsubd, dst, src);
        }

        public void Phsubd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phsubd, dst, src);
        }

        public void Phsubsw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phsubsw, dst, src);
        }

        public void Phsubsw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phsubsw, dst, src);
        }

        public void Phsubsw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phsubsw, dst, src);
        }

        public void Phsubsw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phsubsw, dst, src);
        }

        public void Pmaddubsw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaddubsw, dst, src);
        }

        public void Pmaddubsw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaddubsw, dst, src);
        }

        public void Pmaddubsw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaddubsw, dst, src);
        }

        public void Pmaddubsw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaddubsw, dst, src);
        }

        public void Pabsb(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pabsb, dst, src);
        }

        public void Pabsb(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pabsb, dst, src);
        }

        public void Pabsb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pabsb, dst, src);
        }

        public void Pabsb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pabsb, dst, src);
        }

        public void Pabsw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pabsw, dst, src);
        }

        public void Pabsw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pabsw, dst, src);
        }

        public void Pabsw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pabsw, dst, src);
        }

        public void Pabsw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pabsw, dst, src);
        }

        public void Pabsd(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pabsd, dst, src);
        }

        public void Pabsd(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pabsd, dst, src);
        }

        public void Pabsd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pabsd, dst, src);
        }

        public void Pabsd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pabsd, dst, src);
        }

        public void Pmulhrsw(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmulhrsw, dst, src);
        }

        public void Pmulhrsw(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmulhrsw, dst, src);
        }

        public void Pmulhrsw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmulhrsw, dst, src);
        }

        public void Pmulhrsw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmulhrsw, dst, src);
        }

        public void Pshufb(MMVar dst, MMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pshufb, dst, src);
        }

        public void Pshufb(MMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pshufb, dst, src);
        }

        public void Pshufb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pshufb, dst, src);
        }

        public void Pshufb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pshufb, dst, src);
        }

        public void Palignr(MMVar dst, MMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Palignr, dst, src, imm8);
        }

        public void Palignr(MMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Palignr, dst, src, imm8);
        }

        public void Palignr(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Palignr, dst, src, imm8);
        }

        public void Palignr(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Palignr, dst, src, imm8);
        }

        #endregion

        #region SSE 4.1

        public void Blendpd(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Blendpd, dst, src, imm8);
        }

        public void Blendpd(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Blendpd, dst, src, imm8);
        }

        public void Blendps(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Blendps, dst, src, imm8);
        }

        public void Blendps(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Blendps, dst, src, imm8);
        }

        public void Blendvpd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Blendvpd, dst, src);
        }

        public void Blendvpd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Blendvpd, dst, src);
        }

        public void Blendvps(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Blendvps, dst, src);
        }

        public void Blendvps(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Blendvps, dst, src);
        }

        public void Dppd(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Dppd, dst, src, imm8);
        }

        public void Dppd(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Dppd, dst, src, imm8);
        }

        public void Dpps(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Dpps, dst, src, imm8);
        }

        public void Dpps(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Dpps, dst, src, imm8);
        }

        public void Extractps(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Extractps, dst, src, imm8);
        }

        public void Extractps(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Extractps, dst, src, imm8);
        }

        public void Movntdqa(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movntdqa, dst, src);
        }

        public void Mpsadbw(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Mpsadbw, dst, src, imm8);
        }

        public void Mpsadbw(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Mpsadbw, dst, src, imm8);
        }

        public void Packusdw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Packusdw, dst, src);
        }

        public void Packusdw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Packusdw, dst, src);
        }

        public void Pblendvb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pblendvb, dst, src);
        }

        public void Pblendvb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pblendvb, dst, src);
        }

        public void Pblendw(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pblendw, dst, src, imm8);
        }

        public void Pblendw(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pblendw, dst, src, imm8);
        }

        public void Pcmpeqq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpeqq, dst, src);
        }

        public void Pcmpeqq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpeqq, dst, src);
        }

        public void Pextrb(GPVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pextrb, dst, src, imm8);
        }

        public void Pextrb(Mem dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pextrb, dst, src, imm8);
        }

        public void Pextrw(GPVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pextrw, dst, src, imm8);
        }

        public void Pextrw(Mem dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pextrw, dst, src, imm8);
        }

        public void Pextrd(GPVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pextrd, dst, src, imm8);
        }

        public void Pextrd(Mem dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pextrd, dst, src, imm8);
        }

        public void Pextrq(GPVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pextrq, dst, src, imm8);
        }

        public void Pextrq(Mem dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pextrq, dst, src, imm8);
        }

        public void Phminposuw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phminposuw, dst, src);
        }

        public void Phminposuw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Phminposuw, dst, src);
        }

        public void Pinsrb(XMMVar dst, GPVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pinsrb, dst, src, imm8);
        }

        public void Pinsrb(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pinsrb, dst, src, imm8);
        }

        public void Pinsrw(XMMVar dst, GPVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pinsrw, dst, src, imm8);
        }

        public void Pinsrw(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pinsrw, dst, src, imm8);
        }

        public void Pinsrd(XMMVar dst, GPVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pinsrd, dst, src, imm8);
        }

        public void Pinsrd(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pinsrd, dst, src, imm8);
        }

        public void Pinsrq(XMMVar dst, GPVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pinsrq, dst, src, imm8);
        }

        public void Pinsrq(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pinsrq, dst, src, imm8);
        }

        public void Pmaxuw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxuw, dst, src);
        }

        public void Pmaxuw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxuw, dst, src);
        }

        public void Pmaxsb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxsb, dst, src);
        }

        public void Pmaxsb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxsb, dst, src);
        }

        public void Pmaxsd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxsd, dst, src);
        }

        public void Pmaxsd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxsd, dst, src);
        }

        public void Pmaxud(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxud, dst, src);
        }

        public void Pmaxud(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmaxud, dst, src);
        }

        public void Pminsb(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminsb, dst, src);
        }

        public void Pminsb(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminsb, dst, src);
        }

        public void Pminuw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminuw, dst, src);
        }

        public void Pminuw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminuw, dst, src);
        }

        public void Pminud(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminud, dst, src);
        }

        public void Pminud(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminud, dst, src);
        }

        public void Pminsd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminsd, dst, src);
        }

        public void Pminsd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pminsd, dst, src);
        }

        public void Pmovsxbw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovsxbw, dst, src);
        }

        public void Pmovsxbw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovsxbw, dst, src);
        }

        public void Pmovsxbd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovsxbd, dst, src);
        }

        public void Pmovsxbd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovsxbd, dst, src);
        }

        public void Pmovsxbq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovsxbq, dst, src);
        }

        public void Pmovsxbq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovsxbq, dst, src);
        }

        public void Pmovsxwd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovsxwd, dst, src);
        }

        public void Pmovsxwd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovsxwd, dst, src);
        }

        public void Pmovsxwq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovsxwq, dst, src);
        }

        public void Pmovsxwq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovsxwq, dst, src);
        }

        public void Pmovsxdq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovsxdq, dst, src);
        }

        public void Pmovsxdq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovsxdq, dst, src);
        }

        public void Pmovzxbw(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovzxbw, dst, src);
        }

        public void Pmovzxbw(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovzxbw, dst, src);
        }

        public void Pmovzxbd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovzxbd, dst, src);
        }

        public void Pmovzxbd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovzxbd, dst, src);
        }

        public void Pmovzxbq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovzxbq, dst, src);
        }

        public void Pmovzxbq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovzxbq, dst, src);
        }

        public void Pmovzxwd(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovzxwd, dst, src);
        }

        public void Pmovzxwd(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovzxwd, dst, src);
        }

        public void Pmovzxwq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovzxwq, dst, src);
        }

        public void Pmovzxwq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovzxwq, dst, src);
        }

        public void Pmovzxdq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovzxdq, dst, src);
        }

        public void Pmovzxdq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmovzxdq, dst, src);
        }

        public void Pmuldq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmuldq, dst, src);
        }

        public void Pmuldq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmuldq, dst, src);
        }

        public void Pmulld(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmulld, dst, src);
        }

        public void Pmulld(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pmulld, dst, src);
        }

        public void Ptest(XMMVar op1, XMMVar op2)
        {
            Contract.Requires(op1 != null);
            Contract.Requires(op2 != null);

            _emitInstruction(InstructionCode.Ptest, op1, op2);
        }

        public void Ptest(XMMVar op1, Mem op2)
        {
            Contract.Requires(op1 != null);
            Contract.Requires(op2 != null);

            _emitInstruction(InstructionCode.Ptest, op1, op2);
        }

        public void Roundps(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Roundps, dst, src, imm8);
        }

        public void Roundps(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Roundps, dst, src, imm8);
        }

        public void Roundss(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Roundss, dst, src, imm8);
        }

        public void Roundss(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Roundss, dst, src, imm8);
        }

        public void Roundpd(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Roundpd, dst, src, imm8);
        }

        public void Roundpd(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Roundpd, dst, src, imm8);
        }

        public void Roundsd(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Roundsd, dst, src, imm8);
        }

        public void Roundsd(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Roundsd, dst, src, imm8);
        }

        #endregion

        #region SSE 4.2

        public void Crc32(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Crc32, dst, src);
        }

        public void Crc32(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Crc32, dst, src);
        }

        public void Pcmpestri(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pcmpestri, dst, src, imm8);
        }

        public void Pcmpestri(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pcmpestri, dst, src, imm8);
        }

        public void Pcmpestrm(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pcmpestrm, dst, src, imm8);
        }

        public void Pcmpestrm(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pcmpestrm, dst, src, imm8);
        }

        public void Pcmpistri(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pcmpistri, dst, src, imm8);
        }

        public void Pcmpistri(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pcmpistri, dst, src, imm8);
        }

        public void Pcmpistrm(XMMVar dst, XMMVar src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pcmpistrm, dst, src, imm8);
        }

        public void Pcmpistrm(XMMVar dst, Mem src, Imm imm8)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            _emitInstruction(InstructionCode.Pcmpistrm, dst, src, imm8);
        }

        public void Pcmpgtq(XMMVar dst, XMMVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpgtq, dst, src);
        }

        public void Pcmpgtq(XMMVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Pcmpgtq, dst, src);
        }

        public void Popcnt(GPVar dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Popcnt, dst, src);
        }

        public void Popcnt(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Popcnt, dst, src);
        }

        #endregion

        #region AMD instructions

        public void AmdPrefetch(Mem mem)
        {
            _emitInstruction(InstructionCode.AmdPrefetch, mem);
        }

        public void AmdPrefetchw(Mem mem)
        {
            _emitInstruction(InstructionCode.AmdPrefetchw, mem);
        }

        #endregion

        #region Intel instructions

        public void Movbe(GPVar dst, Mem src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movbe, dst, src);
        }

        public void Movbe(Mem dst, GPVar src)
        {
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            _emitInstruction(InstructionCode.Movbe, dst, src);
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
