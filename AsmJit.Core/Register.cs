namespace AsmJitNet
{
    using System.Diagnostics.Contracts;

    public static class Register
    {
        /// <summary>
        /// 8-bit General purpose register.
        /// </summary>
        public static readonly GPReg al = new GPReg(RegCode.AL);

        /// <summary>
        /// 8-bit General purpose register.
        /// </summary>
        public static readonly GPReg cl = new GPReg(RegCode.CL);

        /// <summary>
        /// 8-bit General purpose register.
        /// </summary>
        public static readonly GPReg dl = new GPReg(RegCode.DL);

        /// <summary>
        /// 8-bit General purpose register.
        /// </summary>
        public static readonly GPReg bl = new GPReg(RegCode.BL);

        /// <summary>
        /// 8-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg spl = new GPReg(RegCode.SPL);

        /// <summary>
        /// 8-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg bpl = new GPReg(RegCode.BPL);

        /// <summary>
        /// 8-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg sil = new GPReg(RegCode.SIL);

        /// <summary>
        /// 8-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg dil = new GPReg(RegCode.DIL);

        /// <summary>
        /// 8-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r8b = new GPReg(RegCode.R8B);

        /// <summary>
        /// 8-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r9b = new GPReg(RegCode.R9B);

        /// <summary>
        /// 8-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r10b = new GPReg(RegCode.R10B);

        /// <summary>
        /// 8-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r11b = new GPReg(RegCode.R11B);

        /// <summary>
        /// 8-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r12b = new GPReg(RegCode.R12B);

        /// <summary>
        /// 8-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r13b = new GPReg(RegCode.R13B);

        /// <summary>
        /// 8-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r14b = new GPReg(RegCode.R14B);

        /// <summary>
        /// 8-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r15b = new GPReg(RegCode.R15B);

        /// <summary>
        /// 8-bit General purpose register.
        /// </summary>
        public static readonly GPReg ah = new GPReg(RegCode.AH);

        /// <summary>
        /// 8-bit General purpose register.
        /// </summary>
        public static readonly GPReg ch = new GPReg(RegCode.CH);

        /// <summary>
        /// 8-bit General purpose register.
        /// </summary>
        public static readonly GPReg dh = new GPReg(RegCode.DH);

        /// <summary>
        /// 8-bit General purpose register.
        /// </summary>
        public static readonly GPReg bh = new GPReg(RegCode.BH);

        /// <summary>
        /// 16-bit General purpose register.
        /// </summary>
        public static readonly GPReg ax = new GPReg(RegCode.AX);

        /// <summary>
        /// 16-bit General purpose register.
        /// </summary>
        public static readonly GPReg cx = new GPReg(RegCode.CX);

        /// <summary>
        /// 16-bit General purpose register.
        /// </summary>
        public static readonly GPReg dx = new GPReg(RegCode.DX);

        /// <summary>
        /// 16-bit General purpose register.
        /// </summary>
        public static readonly GPReg bx = new GPReg(RegCode.BX);

        /// <summary>
        /// 16-bit General purpose register.
        /// </summary>
        public static readonly GPReg sp = new GPReg(RegCode.SP);

        /// <summary>
        /// 16-bit General purpose register.
        /// </summary>
        public static readonly GPReg bp = new GPReg(RegCode.BP);

        /// <summary>
        /// 16-bit General purpose register.
        /// </summary>
        public static readonly GPReg si = new GPReg(RegCode.SI);

        /// <summary>
        /// 16-bit General purpose register.
        /// </summary>
        public static readonly GPReg di = new GPReg(RegCode.DI);

        /// <summary>
        /// 16-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r8w = new GPReg(RegCode.R8W);

        /// <summary>
        /// 16-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r9w = new GPReg(RegCode.R9W);

        /// <summary>
        /// 16-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r10w = new GPReg(RegCode.R10W);

        /// <summary>
        /// 16-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r11w = new GPReg(RegCode.R11W);

        /// <summary>
        /// 16-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r12w = new GPReg(RegCode.R12W);

        /// <summary>
        /// 16-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r13w = new GPReg(RegCode.R13W);

        /// <summary>
        /// 16-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r14w = new GPReg(RegCode.R14W);

        /// <summary>
        /// 16-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r15w = new GPReg(RegCode.R15W);

        /// <summary>
        /// 32-bit General purpose register.
        /// </summary>
        public static readonly GPReg eax = new GPReg(RegCode.EAX);

        /// <summary>
        /// 32-bit General purpose register.
        /// </summary>
        public static readonly GPReg ecx = new GPReg(RegCode.ECX);

        /// <summary>
        /// 32-bit General purpose register.
        /// </summary>
        public static readonly GPReg edx = new GPReg(RegCode.EDX);

        /// <summary>
        /// 32-bit General purpose register.
        /// </summary>
        public static readonly GPReg ebx = new GPReg(RegCode.EBX);

        /// <summary>
        /// 32-bit General purpose register.
        /// </summary>
        public static readonly GPReg esp = new GPReg(RegCode.ESP);

        /// <summary>
        /// 32-bit General purpose register.
        /// </summary>
        public static readonly GPReg ebp = new GPReg(RegCode.EBP);

        /// <summary>
        /// 32-bit General purpose register.
        /// </summary>
        public static readonly GPReg esi = new GPReg(RegCode.ESI);

        /// <summary>
        /// 32-bit General purpose register.
        /// </summary>
        public static readonly GPReg edi = new GPReg(RegCode.EDI);

        /// <summary>
        /// 32-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r8d = new GPReg(RegCode.R8D);

        /// <summary>
        /// 32-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r9d = new GPReg(RegCode.R9D);

        /// <summary>
        /// 32-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r10d = new GPReg(RegCode.R10D);

        /// <summary>
        /// 32-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r11d = new GPReg(RegCode.R11D);

        /// <summary>
        /// 32-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r12d = new GPReg(RegCode.R12D);

        /// <summary>
        /// 32-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r13d = new GPReg(RegCode.R13D);

        /// <summary>
        /// 32-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r14d = new GPReg(RegCode.R14D);

        /// <summary>
        /// 32-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r15d = new GPReg(RegCode.R15D);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg rax = new GPReg(RegCode.RAX);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg rcx = new GPReg(RegCode.RCX);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg rdx = new GPReg(RegCode.RDX);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg rbx = new GPReg(RegCode.RBX);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg rsp = new GPReg(RegCode.RSP);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg rbp = new GPReg(RegCode.RBP);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg rsi = new GPReg(RegCode.RSI);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg rdi = new GPReg(RegCode.RDI);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r8 = new GPReg(RegCode.R8);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r9 = new GPReg(RegCode.R9);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r10 = new GPReg(RegCode.R10);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r11 = new GPReg(RegCode.R11);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r12 = new GPReg(RegCode.R12);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r13 = new GPReg(RegCode.R13);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r14 = new GPReg(RegCode.R14);

        /// <summary>
        /// 64-bit General purpose register (64-bit mode only).
        /// </summary>
        public static readonly GPReg r15 = new GPReg(RegCode.R15);

        /// <summary>
        /// Native-size (platform specific) general purpose register.
        /// </summary>
        public static readonly GPReg nax = new GPReg(RegCode.NAX);

        /// <summary>
        /// Native-size (platform specific) general purpose register.
        /// </summary>
        public static readonly GPReg ncx = new GPReg(RegCode.NCX);

        /// <summary>
        /// Native-size (platform specific) general purpose register.
        /// </summary>
        public static readonly GPReg ndx = new GPReg(RegCode.NDX);

        /// <summary>
        /// Native-size (platform specific) general purpose register.
        /// </summary>
        public static readonly GPReg nbx = new GPReg(RegCode.NBX);

        /// <summary>
        /// Native-size (platform specific) general purpose register.
        /// </summary>
        public static readonly GPReg nsp = new GPReg(RegCode.NSP);

        /// <summary>
        /// Native-size (platform specific) general purpose register.
        /// </summary>
        public static readonly GPReg nbp = new GPReg(RegCode.NBP);

        /// <summary>
        /// Native-size (platform specific) general purpose register.
        /// </summary>
        public static readonly GPReg nsi = new GPReg(RegCode.NSI);

        /// <summary>
        /// Native-size (platform specific) general purpose register.
        /// </summary>
        public static readonly GPReg ndi = new GPReg(RegCode.NDI);

        /// <summary>
        /// 64-bit MM register.
        /// </summary>
        public static readonly MMReg mm0 = new MMReg(RegCode.MM0);

        /// <summary>
        /// 64-bit MM register.
        /// </summary>
        public static readonly MMReg mm1 = new MMReg(RegCode.MM1);

        /// <summary>
        /// 64-bit MM register.
        /// </summary>
        public static readonly MMReg mm2 = new MMReg(RegCode.MM2);

        /// <summary>
        /// 64-bit MM register.
        /// </summary>
        public static readonly MMReg mm3 = new MMReg(RegCode.MM3);

        /// <summary>
        /// 64-bit MM register.
        /// </summary>
        public static readonly MMReg mm4 = new MMReg(RegCode.MM4);

        /// <summary>
        /// 64-bit MM register.
        /// </summary>
        public static readonly MMReg mm5 = new MMReg(RegCode.MM5);

        /// <summary>
        /// 64-bit MM register.
        /// </summary>
        public static readonly MMReg mm6 = new MMReg(RegCode.MM6);

        /// <summary>
        /// 64-bit MM register.
        /// </summary>
        public static readonly MMReg mm7 = new MMReg(RegCode.MM7);

        /// <summary>
        /// 128-bit XMM register.
        /// </summary>
        public static readonly XMMReg xmm0 = new XMMReg(RegCode.XMM0);

        /// <summary>
        /// 128-bit XMM register.
        /// </summary>
        public static readonly XMMReg xmm1 = new XMMReg(RegCode.XMM1);

        /// <summary>
        /// 128-bit XMM register.
        /// </summary>
        public static readonly XMMReg xmm2 = new XMMReg(RegCode.XMM2);

        /// <summary>
        /// 128-bit XMM register.
        /// </summary>
        public static readonly XMMReg xmm3 = new XMMReg(RegCode.XMM3);

        /// <summary>
        /// 128-bit XMM register.
        /// </summary>
        public static readonly XMMReg xmm4 = new XMMReg(RegCode.XMM4);

        /// <summary>
        /// 128-bit XMM register.
        /// </summary>
        public static readonly XMMReg xmm5 = new XMMReg(RegCode.XMM5);

        /// <summary>
        /// 128-bit XMM register.
        /// </summary>
        public static readonly XMMReg xmm6 = new XMMReg(RegCode.XMM6);

        /// <summary>
        /// 128-bit XMM register.
        /// </summary>
        public static readonly XMMReg xmm7 = new XMMReg(RegCode.XMM7);

        /// <summary>
        /// 128-bit XMM register (64-bit mode only).
        /// </summary>
        public static readonly XMMReg xmm8 = new XMMReg(RegCode.XMM8);

        /// <summary>
        /// 128-bit XMM register (64-bit mode only).
        /// </summary>
        public static readonly XMMReg xmm9 = new XMMReg(RegCode.XMM9);

        /// <summary>
        /// 128-bit XMM register (64-bit mode only).
        /// </summary>
        public static readonly XMMReg xmm10 = new XMMReg(RegCode.XMM10);

        /// <summary>
        /// 128-bit XMM register (64-bit mode only).
        /// </summary>
        public static readonly XMMReg xmm11 = new XMMReg(RegCode.XMM11);

        /// <summary>
        /// 128-bit XMM register (64-bit mode only).
        /// </summary>
        public static readonly XMMReg xmm12 = new XMMReg(RegCode.XMM12);

        /// <summary>
        /// 128-bit XMM register (64-bit mode only).
        /// </summary>
        public static readonly XMMReg xmm13 = new XMMReg(RegCode.XMM13);

        /// <summary>
        /// 128-bit XMM register (64-bit mode only).
        /// </summary>
        public static readonly XMMReg xmm14 = new XMMReg(RegCode.XMM14);

        /// <summary>
        /// 128-bit XMM register (64-bit mode only).
        /// </summary>
        public static readonly XMMReg xmm15 = new XMMReg(RegCode.XMM15);

        /// <summary>
        /// Get general purpose register of byte size.
        /// </summary>
        public static GPReg gpb_lo(RegIndex index)
        {
            Contract.Ensures(Contract.Result<GPReg>() != null);

            return new GPReg(RegType.GPB_LO, index);
        }

        /// <summary>
        /// Get general purpose register of byte size.
        /// </summary>
        public static GPReg gpb_hi(RegIndex index)
        {
            Contract.Ensures(Contract.Result<GPReg>() != null);

            return new GPReg(RegType.GPB_HI, index);
        }

        /// <summary>
        /// Get general purpose register of word size.
        /// </summary>
        public static GPReg gpw(RegIndex index)
        {
            Contract.Ensures(Contract.Result<GPReg>() != null);

            return new GPReg(RegType.GPW, index);
        }

        /// <summary>
        /// Get general purpose register of dword size.
        /// </summary>
        public static GPReg gpd(RegIndex index)
        {
            Contract.Ensures(Contract.Result<GPReg>() != null);

            return new GPReg(RegType.GPD, index);
        }

        /// <summary>
        /// Get general purpose register of qword size (64-bit only).
        /// </summary>
        public static GPReg gpq(RegIndex index)
        {
            Contract.Ensures(Contract.Result<GPReg>() != null);

            return new GPReg(RegType.GPQ, index);
        }

        /// <summary>
        /// Get general purpose register of native (platform specific) size.
        /// </summary>
        public static GPReg gpn(RegIndex index)
        {
            Contract.Ensures(Contract.Result<GPReg>() != null);

            return new GPReg(RegType.GPN, index);
        }

        /// <summary>
        /// Get X87 register.
        /// </summary>
        public static X87Reg st(RegIndex index)
        {
            Contract.Ensures(Contract.Result<X87Reg>() != null);

            return new X87Reg(RegType.X87, index);
        }

        /// <summary>
        /// Get MMX register.
        /// </summary>
        public static MMReg mm(RegIndex index)
        {
            Contract.Ensures(Contract.Result<MMReg>() != null);

            return new MMReg(RegType.MM, index);
        }

        /// <summary>
        /// Get SSE register.
        /// </summary>
        public static XMMReg xmm(RegIndex index)
        {
            Contract.Ensures(Contract.Result<XMMReg>() != null);

            return new XMMReg(RegType.XMM, index);
        }
    }
}
