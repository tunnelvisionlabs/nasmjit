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
        private readonly ReadOnlyCollection<RegIndex> _argumentsGPList;
        private readonly ReadOnlyCollection<RegIndex> _argumentsXMMList;

        //! @brief Bitmask for preserved GP registers.
        private readonly uint _argumentsGP;
        //! @brief Bitmask for preserved MM registers.
        private readonly uint _argumentsMM;
        //! @brief Bitmask for preserved XMM registers.
        private readonly uint _argumentsXMM;

        static CallingConventionInfo()
        {
            _info = new CallingConventionInfo[Enum.GetValues(typeof(CallingConvention)).Cast<int>().Max() + 1];

            if (Util.IsX86)
            {
                _info[(int)CallingConvention.Cdecl] = new CallingConventionInfo(CallingConvention.Cdecl);
                _info[(int)CallingConvention.StdCall] = new CallingConventionInfo(CallingConvention.StdCall);
                _info[(int)CallingConvention.MsThisCall] = new CallingConventionInfo(CallingConvention.MsThisCall);
                _info[(int)CallingConvention.MsFastCall] = new CallingConventionInfo(CallingConvention.MsFastCall);
                _info[(int)CallingConvention.BorlandFastCall] = new CallingConventionInfo(CallingConvention.BorlandFastCall);
                _info[(int)CallingConvention.GccFastCall2] = new CallingConventionInfo(CallingConvention.GccFastCall2);
                _info[(int)CallingConvention.GccFastCall3] = new CallingConventionInfo(CallingConvention.GccFastCall3);
            }
            else if (Util.IsX64)
            {
                _info[(int)CallingConvention.X64U] = new CallingConventionInfo(CallingConvention.X64U);
                _info[(int)CallingConvention.X64W] = new CallingConventionInfo(CallingConvention.X64W);
            }
            else
            {
                throw new NotSupportedException();
            }
        }

        private CallingConventionInfo(CallingConvention callingConvention)
        {
            _callingConvention = callingConvention;
            _argumentsDirection = ArgumentsDirection.RightToLeft;

            RegIndex[] argumentsGPList = new RegIndex[(int)RegNum.GP];
            RegIndex[] argumentsXMMList = new RegIndex[(int)RegNum.XMM];

            for (int i = 0; i < argumentsGPList.Length; i++)
                argumentsGPList[i] = RegIndex.Invalid;
            for (int i = 0; i < argumentsXMMList.Length; i++)
                argumentsXMMList[i] = RegIndex.Invalid;

            if (Util.IsX86)
            {
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
                    _calleePopsStack = false;
                    break;

                case AsmJitNet.CallingConvention.StdCall:
                    _calleePopsStack = true;
                    break;

                case AsmJitNet.CallingConvention.MsThisCall:
                    _calleePopsStack = true;
                    argumentsGPList[0] = RegIndex.Ecx;
                    _argumentsGP = 1 << (int)RegIndex.Ecx;
                    break;

                case AsmJitNet.CallingConvention.MsFastCall:
                    _calleePopsStack = true;
                    argumentsGPList[0] = RegIndex.Ecx;
                    argumentsGPList[1] = RegIndex.Edx;
                    _argumentsGP = (1 << (int)RegIndex.Ecx) |
                                    (1 << (int)RegIndex.Edx);
                    break;

                case AsmJitNet.CallingConvention.BorlandFastCall:
                    _calleePopsStack = true;
                    _argumentsDirection = ArgumentsDirection.LeftToRight;
                    argumentsGPList[0] = RegIndex.Eax;
                    argumentsGPList[1] = RegIndex.Edx;
                    argumentsGPList[2] = RegIndex.Ecx;

                    _argumentsGP = (1 << (int)RegIndex.Eax) |
                                    (1 << (int)RegIndex.Edx) |
                                    (1 << (int)RegIndex.Ecx);
                    break;

                case AsmJitNet.CallingConvention.GccFastCall2:
                    _calleePopsStack = false;
                    argumentsGPList[0] = RegIndex.Ecx;
                    argumentsGPList[1] = RegIndex.Edx;

                    _argumentsGP = (1 << (int)RegIndex.Ecx) |
                                    (1 << (int)RegIndex.Edx);
                    break;

                case AsmJitNet.CallingConvention.GccFastCall3:
                    _calleePopsStack = false;
                    argumentsGPList[0] = RegIndex.Edx;
                    argumentsGPList[1] = RegIndex.Ecx;
                    argumentsGPList[2] = RegIndex.Eax;

                    _argumentsGP = (1 << (int)RegIndex.Edx) |
                                    (1 << (int)RegIndex.Ecx) |
                                    (1 << (int)RegIndex.Eax);
                    break;

                default:
                    throw new NotSupportedException(string.Format("The calling convention '{0}' is not supported on this platform.", callingConvention));
                }
            }
            else if (Util.IsX64)
            {
                switch (_callingConvention)
                {
                case CallingConvention.X64W:
                    argumentsGPList[0] = RegIndex.Rcx;
                    argumentsGPList[1] = RegIndex.Rdx;
                    argumentsGPList[2] = RegIndex.R8;
                    argumentsGPList[3] = RegIndex.R9;

                    argumentsXMMList[0] = RegIndex.Xmm0;
                    argumentsXMMList[1] = RegIndex.Xmm1;
                    argumentsXMMList[2] = RegIndex.Xmm2;
                    argumentsXMMList[3] = RegIndex.Xmm3;

                    _argumentsGP = (1 << (int)RegIndex.Rcx) |
                                    (1 << (int)RegIndex.Rdx) |
                                    (1 << (int)RegIndex.R8) |
                                    (1 << (int)RegIndex.R9);

                    _argumentsXMM = (1 << (int)RegIndex.Xmm0) |
                                    (1 << (int)RegIndex.Xmm1) |
                                    (1 << (int)RegIndex.Xmm2) |
                                    (1 << (int)RegIndex.Xmm3);

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
                    argumentsGPList[0] = RegIndex.Rdi;
                    argumentsGPList[1] = RegIndex.Rsi;
                    argumentsGPList[2] = RegIndex.Rdx;
                    argumentsGPList[3] = RegIndex.Rcx;
                    argumentsGPList[4] = RegIndex.R8;
                    argumentsGPList[5] = RegIndex.R9;

                    argumentsXMMList[0] = RegIndex.Xmm0;
                    argumentsXMMList[1] = RegIndex.Xmm1;
                    argumentsXMMList[2] = RegIndex.Xmm2;
                    argumentsXMMList[3] = RegIndex.Xmm3;
                    argumentsXMMList[4] = RegIndex.Xmm4;
                    argumentsXMMList[5] = RegIndex.Xmm5;
                    argumentsXMMList[6] = RegIndex.Xmm6;
                    argumentsXMMList[7] = RegIndex.Xmm7;

                    _argumentsGP = (1 << (int)RegIndex.Rdi) |
                                    (1 << (int)RegIndex.Rsi) |
                                    (1 << (int)RegIndex.Rdx) |
                                    (1 << (int)RegIndex.Rcx) |
                                    (1 << (int)RegIndex.R8) |
                                    (1 << (int)RegIndex.R9);

                    _argumentsXMM = (1 << (int)RegIndex.Xmm0) |
                                    (1 << (int)RegIndex.Xmm1) |
                                    (1 << (int)RegIndex.Xmm2) |
                                    (1 << (int)RegIndex.Xmm3) |
                                    (1 << (int)RegIndex.Xmm4) |
                                    (1 << (int)RegIndex.Xmm5) |
                                    (1 << (int)RegIndex.Xmm6) |
                                    (1 << (int)RegIndex.Xmm7);

                    _preservedGP =
                      (1 << (int)RegIndex.Rbx) |
                      (1 << (int)RegIndex.Rsp) |
                      (1 << (int)RegIndex.Rbp) |
                      (1 << (int)RegIndex.R12) |
                      (1 << (int)RegIndex.R13) |
                      (1 << (int)RegIndex.R14) |
                      (1 << (int)RegIndex.R15);
                    break;

                default:
                    throw new NotSupportedException(string.Format("The calling convention '{0}' is not supported on this platform.", callingConvention));
                }
            }
            else
            {
                throw new NotImplementedException();
            }

            _argumentsGPList = new ReadOnlyCollection<RegIndex>(argumentsGPList);
            _argumentsXMMList = new ReadOnlyCollection<RegIndex>(argumentsXMMList);
        }

        public static CallingConvention DefaultCallingConvention
        {
            get
            {
                if (Util.IsX86)
                    return CallingConvention.Cdecl;
                else if (Util.IsX64)
                    return CallingConvention.X64W;
                else
                    throw new NotSupportedException();
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

        public ReadOnlyCollection<RegIndex> ArgumentsGPList
        {
            get
            {
                return _argumentsGPList;
            }
        }

        public ReadOnlyCollection<RegIndex> ArgumentsXMMList
        {
            get
            {
                return _argumentsXMMList;
            }
        }

        public static CallingConventionInfo GetCallingConventionInfo(CallingConvention callingConvention)
        {
            if (callingConvention == CallingConvention.Default)
                callingConvention = DefaultCallingConvention;

            return _info[(int)callingConvention];
        }
    }
}
