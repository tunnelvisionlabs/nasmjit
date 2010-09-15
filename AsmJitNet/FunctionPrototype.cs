namespace AsmJitNet
{
    using System;
    using System.Diagnostics.Contracts;
    using System.Linq;
    using System.Collections.Concurrent;
    using System.Collections.Generic;
    using Expression = System.Linq.Expressions.Expression;

    public sealed class FunctionPrototype
    {
        private const int InvalidValue = -1;

        private static readonly ConcurrentDictionary<Type, List<FunctionPrototype>> _prototypes =
            new ConcurrentDictionary<Type, List<FunctionPrototype>>();

        private readonly CallingConvention _callingConvention;
        private Argument[] _arguments;
        private VariableType _returnValue = VariableType.Invalid;
        private int _argumentsStackSize;

        private int _passedGP;
        private int _passedMM;
        private int _passedXMM;

        internal FunctionPrototype(CallingConvention callingConvention, Type delegateType)
        {
            if (delegateType == null)
                throw new ArgumentNullException("delegateType");

            if (callingConvention == CallingConvention.Default)
                callingConvention = CallingConventionInfo.DefaultCallingConvention;

            _callingConvention = callingConvention;

            if (delegateType == typeof(Action))
            {
                SetPrototype(new VariableType[0], VariableType.Invalid);
            }
            else
            {
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

                SetPrototype(arguments, returnValue);
            }
        }

        internal FunctionPrototype(CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            Contract.Requires(arguments != null);

            if (callingConvention == CallingConvention.Default)
                callingConvention = CallingConventionInfo.DefaultCallingConvention;

            _callingConvention = callingConvention;
            if (arguments.Length > 32)
                throw new ArgumentException();

            SetPrototype(arguments, returnValue);
        }

        public static FunctionPrototype GetFunctionPrototype(CallingConvention callingConvention, Type delegateType)
        {
            if (delegateType == null)
                throw new ArgumentNullException("delegateType");
            Contract.Ensures(Contract.Result<FunctionPrototype>() != null);
            Contract.EndContractBlock();

            List<FunctionPrototype> prototypes = _prototypes.GetOrAdd(delegateType, key => new List<FunctionPrototype>());
            lock (prototypes)
            {
                FunctionPrototype prototype = prototypes.FirstOrDefault(i => i.CallingConvention == callingConvention);
                if (prototype == null)
                {
                    prototype = new FunctionPrototype(callingConvention, delegateType);
                    prototypes.Add(prototype);
                }

                return prototype;
            }
        }

        public static FunctionPrototype GetFunctionPrototype(CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            if (arguments == null)
                throw new ArgumentNullException("arguments");
            Contract.Ensures(Contract.Result<FunctionPrototype>() != null);
            Contract.EndContractBlock();

            bool isAction = returnValue == VariableType.Invalid;
            Type delegateType;

            if (isAction)
            {
                delegateType = Expression.GetActionType(Array.ConvertAll(arguments, i => Compiler.IdToType(i)));
            }
            else
            {
                List<Type> typeArgs = arguments.Select(i => Compiler.IdToType(i)).ToList();
                typeArgs.Add(Compiler.IdToType(returnValue));
                delegateType = Expression.GetFuncType(typeArgs.ToArray());
            }

            List<FunctionPrototype> prototypes = _prototypes.GetOrAdd(delegateType, key => new List<FunctionPrototype>());
            lock (prototypes)
            {
                FunctionPrototype prototype = prototypes.FirstOrDefault(i => i.CallingConvention == callingConvention);
                if (prototype == null)
                {
                    prototype = new FunctionPrototype(callingConvention, arguments, returnValue);
                    prototypes.Add(prototype);
                }

                return prototype;
            }
        }

        public int PassedGP
        {
            get
            {
                return _passedGP;
            }
        }

        public int PassedMM
        {
            get
            {
                return _passedMM;
            }
        }

        public int PassedXMM
        {
            get
            {
                return _passedXMM;
            }
        }

        public CallingConvention CallingConvention
        {
            get
            {
                return _callingConvention;
            }
        }

        public CallingConventionInfo CallingConventionInfo
        {
            get
            {
                return CallingConventionInfo.GetCallingConventionInfo(_callingConvention);
            }
        }

        public bool CalleePopsStack
        {
            get
            {
                return CallingConventionInfo.CalleePopsStack;
            }
        }

        public ArgumentsDirection ArgumentsDirection
        {
            get
            {
                return CallingConventionInfo.ArgumentsDirection;
            }
        }

        public int PreservedGP
        {
            get
            {
                return CallingConventionInfo.PreservedGP;
            }
        }

        public int PreservedMM
        {
            get
            {
                return CallingConventionInfo.PreservedMM;
            }
        }

        public int PreservedXMM
        {
            get
            {
                return CallingConventionInfo.PreservedXMM;
            }
        }

        public Argument[] Arguments
        {
            get
            {
                return _arguments;
            }
        }

        public VariableType ReturnValue
        {
            get
            {
                return _returnValue;
            }
        }

        public int ArgumentsStackSize
        {
            get
            {
                return _argumentsStackSize;
            }
        }

        public override string ToString()
        {
            string callingConvention = GetCallingConventionName(CallingConvention);
            string returnValue = GetVariableTypeName(ReturnValue);
            string arguments = string.Join(", ", Arguments.Select(i => GetVariableTypeName(i._variableType)));
            return string.Format("[{0}] {1} function({2})", callingConvention, returnValue, arguments);
        }

        private static string GetCallingConventionName(CallingConvention callingConvention)
        {
            switch (callingConvention)
            {
            case CallingConvention.X64W:
                return "win64";
            case CallingConvention.X64U:
                return "64-bit unix";
            case CallingConvention.Cdecl:
                return "cdecl";
            case CallingConvention.StdCall:
                return "stdcall";
            case CallingConvention.MsThisCall:
                return "thiscall";
            case CallingConvention.MsFastCall:
                return "fastcall";
            case CallingConvention.BorlandFastCall:
                return "borland fastcall";
            case CallingConvention.GccFastCall2:
                return "gcc fastcall 2";
            case CallingConvention.GccFastCall3:
                return "gcc fastcall 3";
            case CallingConvention.Default:
                return GetCallingConventionName(CallingConventionInfo.DefaultCallingConvention);
            case CallingConvention.None:
            default:
                return "invalid";
            }
        }

        private static string GetVariableTypeName(VariableType variableType)
        {
            switch (variableType)
            {
            case VariableType.GPD:
                return "int";
            case VariableType.GPQ:
                return "long";
            case VariableType.X87:
                return "x87";
            case VariableType.X87_1F:
                return "float";
            case VariableType.X87_1D:
                return "double";
            case VariableType.MM:
                return "mm";
            case VariableType.XMM:
                return "xmm";
            case VariableType.XMM_1F:
                return "float";
            case VariableType.XMM_4F:
                return "floatv4";
            case VariableType.XMM_1D:
                return "double";
            case VariableType.XMM_2D:
                return "doublev2";
            case VariableType.Invalid:
                return "void";
            default:
                return "invalid";
            }
        }

        private void SetPrototype(VariableType[] arguments, VariableType returnValue)
        {
            if (arguments == null)
                throw new ArgumentNullException("arguments");
            if (arguments.Length > 32)
                throw new ArgumentException();
            Contract.EndContractBlock();

            int i;

            int posGP = 0;
            int posXMM = 0;
            int stackOffset = 0;

            _returnValue = returnValue;

            if (arguments.Length == 0)
            {
                _arguments = new Argument[0];
                return;
            }

            ArgumentData[] argumentData;
            argumentData = Array.ConvertAll(arguments, a => new ArgumentData(a, RegIndex.Invalid, InvalidValue));

            // --------------------------------------------------------------------------
            // [X86 Calling Conventions (32-bit)]
            // --------------------------------------------------------------------------

            if (Util.IsX86)
            {
                // Register arguments (Integer), always left-to-right.
                for (i = 0; i != arguments.Length; i++)
                {
                    ArgumentData a = argumentData[i];
                    if (VariableInfo.IsVariableInteger(a._variableType) && posGP < 16 && CallingConventionInfo.ArgumentsGP[posGP] != RegIndex.Invalid)
                    {
                        a._registerIndex = CallingConventionInfo.ArgumentsGP[posGP++];
                        _passedGP |= (1 << (int)a._registerIndex);
                    }
                }

                // Stack arguments.
                bool ltr = CallingConventionInfo.ArgumentsDirection == ArgumentsDirection.LeftToRight;
                int istart = ltr ? 0 : arguments.Length - 1;
                int iend = ltr ? arguments.Length : -1;
                int istep = ltr ? 1 : -1;

                for (i = istart; i != iend; i += istep)
                {
                    ArgumentData a = argumentData[i];

                    if (VariableInfo.IsVariableInteger(a._variableType))
                    {
                        stackOffset -= 4;
                        a._stackOffset = stackOffset;
                    }
                    else if (VariableInfo.IsVariableFloat(a._variableType))
                    {
                        int size = VariableInfo.GetVariableInfo(a._variableType).Size;
                        stackOffset -= size;
                        a._stackOffset = stackOffset;
                    }
                }
            }

            // --------------------------------------------------------------------------
            // [X64 Calling Conventions (64-bit)]
            // --------------------------------------------------------------------------

            if (Util.IsX64)
            {
                // Windows 64-bit specific.
                if (_callingConvention == CallingConvention.X64W)
                {
                    int max = Math.Min(arguments.Length, 4);

                    // Register arguments (Integer / FP), always left to right.
                    for (i = 0; i != max; i++)
                    {
                        ArgumentData a = argumentData[i];

                        if (VariableInfo.IsVariableInteger(a._variableType))
                        {
                            a._registerIndex = CallingConventionInfo.ArgumentsGP[i];
                            _passedGP |= (1 << (int)a._registerIndex);
                        }
                        else if (VariableInfo.IsVariableFloat(a._variableType))
                        {
                            a._registerIndex = CallingConventionInfo.ArgumentsXMM[i];
                            _passedXMM |= (1 << (int)a._registerIndex);
                        }
                    }

                    // Stack arguments (always right-to-left).
                    for (i = arguments.Length - 1; i != -1; i--)
                    {
                        ArgumentData a = argumentData[i];
                        if (a.IsAssigned)
                            continue;

                        if (VariableInfo.IsVariableInteger(a._variableType))
                        {
                            stackOffset -= 8; // Always 8 bytes.
                            a._stackOffset = stackOffset;
                        }
                        else if (VariableInfo.IsVariableFloat(a._variableType))
                        {
                            int size = VariableInfo.GetVariableInfo(a._variableType).Size;
                            stackOffset -= size;
                            a._stackOffset = stackOffset;
                        }
                    }

                    // 32 bytes shadow space (X64W calling convention specific).
                    stackOffset -= 4 * 8;
                }
                // Linux/Unix 64-bit (AMD64 calling convention).
                else
                {
                    // Register arguments (Integer), always left to right.
                    for (i = 0; i != arguments.Length; i++)
                    {
                        ArgumentData a = argumentData[i];
                        if (VariableInfo.IsVariableInteger(a._variableType) && posGP < 32 && CallingConventionInfo.ArgumentsGP[posGP] != RegIndex.Invalid)
                        {
                            a._registerIndex = CallingConventionInfo.ArgumentsGP[posGP++];
                            _passedGP |= (1 << (int)a._registerIndex);
                        }
                    }

                    // Register arguments (FP), always left to right.
                    for (i = 0; i != arguments.Length; i++)
                    {
                        ArgumentData a = argumentData[i];
                        if (VariableInfo.IsVariableFloat(a._variableType))
                        {
                            a._registerIndex = CallingConventionInfo.ArgumentsXMM[posXMM++];
                            _passedXMM |= (1 << (int)a._registerIndex);
                        }
                    }

                    // Stack arguments.
                    for (i = arguments.Length - 1; i != -1; i--)
                    {
                        ArgumentData a = argumentData[i];
                        if (a.IsAssigned)
                            continue;

                        if (VariableInfo.IsVariableInteger(a._variableType))
                        {
                            stackOffset -= 8;
                            a._stackOffset = stackOffset;
                        }
                        else if (VariableInfo.IsVariableFloat(a._variableType))
                        {
                            int size = VariableInfo.GetVariableInfo(a._variableType).Size;

                            stackOffset -= size;
                            a._stackOffset = stackOffset;
                        }
                    }
                }
            }

            // Modify stack offset (all function parameters will be in positive stack
            // offset that is never zero).
            for (i = 0; i < arguments.Length; i++)
            {
                if (argumentData[i]._registerIndex == RegIndex.Invalid)
                    argumentData[i]._stackOffset += IntPtr.Size - stackOffset;
            }

            _argumentsStackSize = -stackOffset;

            _arguments = Array.ConvertAll(argumentData, data => new Argument(data._variableType, data._registerIndex, data._stackOffset));
        }

        internal int FindArgumentByRegisterCode(int regCode)
        {
            RegType type = (RegType)(regCode & (int)RegType.MASK);
            RegIndex idx = (RegIndex)(regCode & (int)RegIndex.Mask);

            VariableClass @class;
            int i;

            switch (type)
            {
            case RegType.GPD:
            case RegType.GPQ:
                @class = VariableClass.GP;
                break;

            case RegType.MM:
                @class = VariableClass.MM;
                break;

            case RegType.XMM:
                @class = VariableClass.XMM;
                break;

            default:
                return InvalidValue;
            }

            for (i = 0; i < _arguments.Length; i++)
            {
                Argument arg = _arguments[i];
                if ((VariableInfo.GetVariableClass(arg._variableType) & @class) != 0 && (arg._registerIndex == idx))
                    return i;
            }

            return InvalidValue;
        }

        [ContractInvariantMethod]
        private void ObjectInvariants()
        {
            Contract.Invariant(_arguments != null);
            Contract.Invariant(_argumentsStackSize >= 0);
        }

        public sealed class Argument
        {
            public readonly VariableType _variableType;

            public readonly RegIndex _registerIndex;

            public readonly int _stackOffset;

            public Argument(VariableType variableType, RegIndex registerIndex, int stackOffset)
            {
                _variableType = variableType;
                _registerIndex = registerIndex;
                _stackOffset = stackOffset;
            }

            public bool IsAssigned
            {
                get
                {
                    return _registerIndex != RegIndex.Invalid || _stackOffset != InvalidValue;
                }
            }
        }

        private sealed class ArgumentData
        {
            public readonly VariableType _variableType;
            public RegIndex _registerIndex;
            public int _stackOffset;

            public ArgumentData(VariableType variableType, RegIndex registerIndex, int stackOffset)
            {
                _variableType = variableType;
                _registerIndex = registerIndex;
                _stackOffset = stackOffset;
            }

            public bool IsAssigned
            {
                get
                {
                    return _registerIndex != RegIndex.Invalid || _stackOffset != InvalidValue;
                }
            }
        }
    }
}
