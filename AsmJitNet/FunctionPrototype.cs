namespace AsmJitNet
{
    using System;

    public class FunctionPrototype
    {
        private const int InvalidValue = -1;

        private CallingConvention _callingConvention;
        public bool _calleePopsStack;
        private Argument[] _arguments;
        private VariableType _returnValue;
        private ArgumentsDirection _argumentsDirection;
        private int _argumentsStackSize;

        private readonly RegIndex[] _argumentsGP = new RegIndex[16];

        private readonly RegIndex[] _argumentsXMM = new RegIndex[16];

        private int _preservedGP;
        private int _preservedMM;
        private int _preservedXMM;

        private int _passedGP;
        private int _passedMM;
        private int _passedXMM;

        public FunctionPrototype(CallingConvention callingConvention, VariableType[] arguments, VariableType returnValue)
        {
            Clear();
            SetCallingConvention(callingConvention);
            if (arguments.Length > 32)
                throw new ArgumentException();

            SetPrototype(arguments, returnValue);
        }

        public int PreservedGP
        {
            get
            {
                return _preservedGP;
            }
        }

        public int PreservedMM
        {
            get
            {
                return _preservedMM;
            }
        }

        public int PreservedXMM
        {
            get
            {
                return _preservedXMM;
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

        public bool CalleePopsStack
        {
            get
            {
                return _calleePopsStack;
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

        public ArgumentsDirection ArgumentsDirection
        {
            get
            {
                return _argumentsDirection;
            }
        }

        public int ArgumentsStackSize
        {
            get
            {
                return _argumentsStackSize;
            }
        }

        private void Clear()
        {
            _callingConvention = CallingConvention.None;
            _calleePopsStack = false;

            _arguments = null;
            _argumentsDirection = ArgumentsDirection.RightToLeft;
            _argumentsStackSize = 0;

            _returnValue = VariableType.Invalid;

            for (int i = 0; i < _argumentsGP.Length; i++)
                _argumentsGP[i] = RegIndex.Invalid;
            for (int i = 0; i < _argumentsXMM.Length; i++)
                _argumentsXMM[i] = RegIndex.Invalid;

            _preservedGP = 0;
            _preservedMM = 0;
            _preservedXMM = 0;

            _passedGP = 0;
            _passedMM = 0;
            _passedXMM = 0;
        }

        private void SetCallingConvention(CallingConvention callingConvention)
        {
            _callingConvention = callingConvention;

#if ASMJIT_X86
            _preservedGP =
              (1 << (int)RegIndex.Ebx) |
              (1 << (int)RegIndex.Esp) |
              (1 << (int)RegIndex.Ebp) |
              (1 << (int)RegIndex.Esi) |
              (1 << (int)RegIndex.Edi);
            _preservedXMM = 0;

            switch (_callingConvention)
            {
            case AsmJitNet.CallingConvention.Cdecl:
                break;

            case AsmJitNet.CallingConvention.StdCall:
                _calleePopsStack = true;break;

            case AsmJitNet.CallingConvention.MsThisCall:
                _calleePopsStack = true;
                _argumentsGP[0] = RegIndex.Ecx;
                break;

            case AsmJitNet.CallingConvention.MsFastCall:
                _calleePopsStack = true;
                _argumentsGP[0] = RegIndex.Ecx;
                _argumentsGP[1] = RegIndex.Edx;
                break;

            case AsmJitNet.CallingConvention.BorlandFastCall:
                _calleePopsStack = true;
                _argumentsDirection = ArgumentsDirection.LeftToRight;
                _argumentsGP[0] = RegIndex.Eax;
                _argumentsGP[1] = RegIndex.Edx;
                _argumentsGP[2] = RegIndex.Ecx;
                break;

            case AsmJitNet.CallingConvention.GccFastCall2:
                _calleePopsStack = false;
                _argumentsGP[0] = RegIndex.Ecx;
                _argumentsGP[1] = RegIndex.Edx;
                break;

            case AsmJitNet.CallingConvention.GccFastCall3:
                _calleePopsStack = false;
                _argumentsGP[0] = RegIndex.Edx;
                _argumentsGP[1] = RegIndex.Ecx;
                _argumentsGP[2] = RegIndex.Eax;
                break;

            default:
                throw new ArgumentException();
            }
#elif ASMJIT_X64
            switch (_callingConvention)
            {
            case CallingConvention.X64W:
                _argumentsGP[0] = RegIndex.Rcx;
                _argumentsGP[1] = RegIndex.Rdx;
                _argumentsGP[2] = RegIndex.R8;
                _argumentsGP[3] = RegIndex.R9;

                _argumentsXMM[0] = RegIndex.Xmm0;
                _argumentsXMM[1] = RegIndex.Xmm1;
                _argumentsXMM[2] = RegIndex.Xmm2;
                _argumentsXMM[3] = RegIndex.Xmm3;

                _preservedGP =
                  (1 << (int)RegIndex.Rbx) |
                  (1 << (int)RegIndex.Rsp) |
                  (1 << (int)RegIndex.Rbp) |
                  (1 << (int)RegIndex.Rsi) |
                  (1 << (int)RegIndex.Rdi) |
                  (1 << (int)RegIndex.R12) |
                  (1 << (int)RegIndex.R13) |
                  (1 << (int)RegIndex.R14) |
                  (1 << (int)RegIndex.R15);
                _preservedXMM =
                  (1 << (int)RegIndex.Xmm6) |
                  (1 << (int)RegIndex.Xmm7) |
                  (1 << (int)RegIndex.Xmm8) |
                  (1 << (int)RegIndex.Xmm9) |
                  (1 << (int)RegIndex.Xmm10) |
                  (1 << (int)RegIndex.Xmm11) |
                  (1 << (int)RegIndex.Xmm12) |
                  (1 << (int)RegIndex.Xmm13) |
                  (1 << (int)RegIndex.Xmm14) |
                  (1 << (int)RegIndex.Xmm15);
                break;

            case CallingConvention.X64U:
                _argumentsGP[0] = RegIndex.Rdi;
                _argumentsGP[1] = RegIndex.Rsi;
                _argumentsGP[2] = RegIndex.Rdx;
                _argumentsGP[3] = RegIndex.Rcx;
                _argumentsGP[4] = RegIndex.R8;
                _argumentsGP[5] = RegIndex.R9;

                _argumentsXMM[0] = RegIndex.Xmm0;
                _argumentsXMM[1] = RegIndex.Xmm1;
                _argumentsXMM[2] = RegIndex.Xmm2;
                _argumentsXMM[3] = RegIndex.Xmm3;
                _argumentsXMM[4] = RegIndex.Xmm4;
                _argumentsXMM[5] = RegIndex.Xmm5;
                _argumentsXMM[6] = RegIndex.Xmm6;
                _argumentsXMM[7] = RegIndex.Xmm7;

                _preservedGP =
                  (1 << (int)RegIndex.Rbx) |
                  (1 << (int)RegIndex.Rsp) |
                  (1 << (int)RegIndex.Rbp) |
                  (1 << (int)RegIndex.R12) |
                  (1 << (int)RegIndex.R13) |
                  (1 << (int)RegIndex.R14) |
                  (1 << (int)RegIndex.R15);
                _preservedXMM = 0;
                break;

            default:
                throw new ArgumentException("Illegal calling convention.");
            }
#else
            throw new NotImplementedException();
#endif
        }

        private void SetPrototype(VariableType[] arguments, VariableType returnValue)
        {
            if (arguments == null)
                throw new ArgumentNullException("arguments");
            if (arguments.Length > 32)
                throw new ArgumentException();

            int i;

            int posGP = 0;
            int posXMM = 0;
            int stackOffset = 0;

            _returnValue = returnValue;

            _arguments = Array.ConvertAll(arguments,
                a => new Argument()
                {
                    _variableType = a,
                    _registerIndex = RegIndex.Invalid,
                    _stackOffset = InvalidValue
                });

            if (_arguments.Length == 0)
                return;

            // --------------------------------------------------------------------------
            // [X86 Calling Conventions (32-bit)]
            // --------------------------------------------------------------------------

#if ASMJIT_X86
            // Register arguments (Integer), always left-to-right.
            for (i = 0; i != arguments.Length; i++)
            {
                Argument a = _arguments[i];
                if (VariableInfo.IsVariableInteger(a._variableType) && posGP < 16 && _argumentsGP[posGP] != RegIndex.Invalid)
                {
                    a._registerIndex = _argumentsGP[posGP++];
                    _passedGP |= (1 << (int)a._registerIndex);
                }
            }

            // Stack arguments.
            bool ltr = _argumentsDirection == ArgumentsDirection.LeftToRight;
            int istart = ltr ? 0 : arguments.Length - 1;
            int iend = ltr ? arguments.Length : -1;
            int istep = ltr ? 1 : -1;

            for (i = istart; i != iend; i += istep)
            {
                Argument a = _arguments[i];

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
#endif // ASMJIT_X86

            // --------------------------------------------------------------------------
            // [X64 Calling Conventions (64-bit)]
            // --------------------------------------------------------------------------

#if ASMJIT_X64
            // Windows 64-bit specific.
            if (_callingConvention == CallingConvention.X64W)
            {
                int max = Math.Min(arguments.Length, 4);

                // Register arguments (Integer / FP), always left to right.
                for (i = 0; i != max; i++)
                {
                    Argument a = _arguments[i];

                    if (VariableInfo.IsVariableInteger(a._variableType))
                    {
                        a._registerIndex = _argumentsGP[i];
                        _passedGP |= (1 << (int)a._registerIndex);
                    }
                    else if (VariableInfo.IsVariableFloat(a._variableType))
                    {
                        a._registerIndex = _argumentsXMM[i];
                        _passedXMM |= (1 << (int)a._registerIndex);
                    }
                }

                // Stack arguments (always right-to-left).
                for (i = arguments.Length - 1; i != -1; i--)
                {
                    Argument a = _arguments[i];
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
                    Argument a = _arguments[i];
                    if (VariableInfo.IsVariableInteger(a._variableType) && posGP < 32 && _argumentsGP[posGP] != RegIndex.Invalid)
                    {
                        a._registerIndex = _argumentsGP[posGP++];
                        _passedGP |= (1 << (int)a._registerIndex);
                    }
                }

                // Register arguments (FP), always left to right.
                for (i = 0; i != arguments.Length; i++)
                {
                    Argument a = _arguments[i];
                    if (VariableInfo.IsVariableFloat(a._variableType))
                    {
                        a._registerIndex = _argumentsXMM[posXMM++];
                        _passedXMM |= (1 << (int)a._registerIndex);
                    }
                }

                // Stack arguments.
                for (i = arguments.Length - 1; i != -1; i--)
                {
                    Argument a = _arguments[i];
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
#endif // ASMJIT_X64

            // Modify stack offset (all function parameters will be in positive stack
            // offset that is never zero).
            for (i = 0; i < arguments.Length; i++)
            {
                if (_arguments[i]._registerIndex == RegIndex.Invalid)
                    _arguments[i]._stackOffset += IntPtr.Size - stackOffset;
            }

            _argumentsStackSize = -stackOffset;
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

        public class Argument
        {
            public VariableType _variableType;

            public RegIndex _registerIndex;

            public int _stackOffset;

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
