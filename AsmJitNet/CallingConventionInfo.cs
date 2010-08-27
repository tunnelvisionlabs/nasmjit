namespace AsmJitNet
{
    using System.Collections.ObjectModel;
    using System;
    using System.Linq;

    public sealed class CallingConventionInfo
    {
        private static readonly CallingConventionInfo[] _info;

        private readonly CallingConvention _callingConvention;
        private readonly bool _calleePopsStack;
        private readonly ArgumentsDirection _argumentsDirection;
        private readonly int _preservedGP;
        private readonly int _preservedMM;
        private readonly int _preservedXMM;
        private readonly ReadOnlyCollection<RegIndex> _argumentsGP;
        private readonly ReadOnlyCollection<RegIndex> _argumentsXMM;

        static CallingConventionInfo()
        {
            _info = new CallingConventionInfo[Enum.GetValues(typeof(CallingConvention)).Cast<int>().Max() + 1];

#if ASMJIT_X86
            _info[(int)CallingConvention.Cdecl] = new CallingConventionInfo(CallingConvention.Cdecl);
            _info[(int)CallingConvention.StdCall] = new CallingConventionInfo(CallingConvention.StdCall);
            _info[(int)CallingConvention.MsThisCall] = new CallingConventionInfo(CallingConvention.MsThisCall);
            _info[(int)CallingConvention.MsFastCall] = new CallingConventionInfo(CallingConvention.MsFastCall);
            _info[(int)CallingConvention.BorlandFastCall] = new CallingConventionInfo(CallingConvention.BorlandFastCall);
            _info[(int)CallingConvention.GccFastCall2] = new CallingConventionInfo(CallingConvention.GccFastCall2);
            _info[(int)CallingConvention.GccFastCall3] = new CallingConventionInfo(CallingConvention.GccFastCall3);
#elif ASMJIT_X64
            _info[(int)CallingConvention.X64U] = new CallingConventionInfo(CallingConvention.X64U);
            _info[(int)CallingConvention.X64W] = new CallingConventionInfo(CallingConvention.X64W);
#else
            throw new NotSupportedException();
#endif
        }

        private CallingConventionInfo(CallingConvention callingConvention)
        {
            _callingConvention = callingConvention;
            _argumentsDirection = ArgumentsDirection.RightToLeft;

            RegIndex[] argumentsGP = new RegIndex[(int)RegNum.GP];
            RegIndex[] argumentsXMM = new RegIndex[(int)RegNum.XMM];

            for (int i = 0; i < argumentsGP.Length; i++)
                argumentsGP[i] = RegIndex.Invalid;
            for (int i = 0; i < argumentsXMM.Length; i++)
                argumentsXMM[i] = RegIndex.Invalid;

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
                _calleePopsStack = true;
                break;

            case AsmJitNet.CallingConvention.MsThisCall:
                _calleePopsStack = true;
                argumentsGP[0] = RegIndex.Ecx;
                break;

            case AsmJitNet.CallingConvention.MsFastCall:
                _calleePopsStack = true;
                argumentsGP[0] = RegIndex.Ecx;
                argumentsGP[1] = RegIndex.Edx;
                break;

            case AsmJitNet.CallingConvention.BorlandFastCall:
                _calleePopsStack = true;
                _argumentsDirection = ArgumentsDirection.LeftToRight;
                argumentsGP[0] = RegIndex.Eax;
                argumentsGP[1] = RegIndex.Edx;
                argumentsGP[2] = RegIndex.Ecx;
                break;

            case AsmJitNet.CallingConvention.GccFastCall2:
                _calleePopsStack = false;
                argumentsGP[0] = RegIndex.Ecx;
                argumentsGP[1] = RegIndex.Edx;
                break;

            case AsmJitNet.CallingConvention.GccFastCall3:
                _calleePopsStack = false;
                argumentsGP[0] = RegIndex.Edx;
                argumentsGP[1] = RegIndex.Ecx;
                argumentsGP[2] = RegIndex.Eax;
                break;

            default:
                throw new ArgumentException();
            }
#elif ASMJIT_X64
            switch (_callingConvention)
            {
            case CallingConvention.X64W:
                argumentsGP[0] = RegIndex.Rcx;
                argumentsGP[1] = RegIndex.Rdx;
                argumentsGP[2] = RegIndex.R8;
                argumentsGP[3] = RegIndex.R9;

                argumentsXMM[0] = RegIndex.Xmm0;
                argumentsXMM[1] = RegIndex.Xmm1;
                argumentsXMM[2] = RegIndex.Xmm2;
                argumentsXMM[3] = RegIndex.Xmm3;

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
                argumentsGP[0] = RegIndex.Rdi;
                argumentsGP[1] = RegIndex.Rsi;
                argumentsGP[2] = RegIndex.Rdx;
                argumentsGP[3] = RegIndex.Rcx;
                argumentsGP[4] = RegIndex.R8;
                argumentsGP[5] = RegIndex.R9;

                argumentsXMM[0] = RegIndex.Xmm0;
                argumentsXMM[1] = RegIndex.Xmm1;
                argumentsXMM[2] = RegIndex.Xmm2;
                argumentsXMM[3] = RegIndex.Xmm3;
                argumentsXMM[4] = RegIndex.Xmm4;
                argumentsXMM[5] = RegIndex.Xmm5;
                argumentsXMM[6] = RegIndex.Xmm6;
                argumentsXMM[7] = RegIndex.Xmm7;

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

            _argumentsGP = new ReadOnlyCollection<RegIndex>(argumentsGP);
            _argumentsXMM = new ReadOnlyCollection<RegIndex>(argumentsXMM);
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

        public ArgumentsDirection ArgumentsDirection
        {
            get
            {
                return _argumentsDirection;
            }
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
                return _preservedXMM;
            }
        }

        public int PreservedXMM
        {
            get
            {
                return _preservedXMM;
            }
        }

        public ReadOnlyCollection<RegIndex> ArgumentsGP
        {
            get
            {
                return _argumentsGP;
            }
        }

        public ReadOnlyCollection<RegIndex> ArgumentsXMM
        {
            get
            {
                return _argumentsXMM;
            }
        }

        public static CallingConventionInfo GetCallingConventionInfo(CallingConvention callingConvention)
        {
            return _info[(int)callingConvention];
        }
    }
}
