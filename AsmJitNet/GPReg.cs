namespace AsmJitNet2
{
    public class GPReg : BaseReg
    {
        static GPReg()
        {
            al = new GPReg((int)RegCode.AL);
            cl = new GPReg((int)RegCode.CL);
            dl = new GPReg((int)RegCode.DL);
            bl = new GPReg((int)RegCode.BL);

            ah = new GPReg((int)RegCode.AH);
            ch = new GPReg((int)RegCode.CH);
            dh = new GPReg((int)RegCode.DH);
            bh = new GPReg((int)RegCode.BH);

            eax = new GPReg((int)RegCode.EAX);
            ecx = new GPReg((int)RegCode.ECX);
            edx = new GPReg((int)RegCode.EDX);
            ebx = new GPReg((int)RegCode.EBX);
            esp = new GPReg((int)RegCode.ESP);
            ebp = new GPReg((int)RegCode.EBP);
            esi = new GPReg((int)RegCode.ESI);
            edi = new GPReg((int)RegCode.EDI);

            Nax = new GPReg((int)RegCode.NAX);
            Ncx = new GPReg((int)RegCode.NCX);
            Ndx = new GPReg((int)RegCode.NDX);
            Nbx = new GPReg((int)RegCode.NBX);
            Nsp = new GPReg((int)RegCode.NSP);
            Nbp = new GPReg((int)RegCode.NBP);
            Nsi = new GPReg((int)RegCode.NSI);
            Ndi = new GPReg((int)RegCode.NDI);
        }

        public GPReg()
            : base(InvalidValue, 0)
        {
        }

        public GPReg(int code)
            : base(code, 1 << ((code & (int)RegType.MASK) >> 12))
        {
        }

        public static readonly GPReg al;
        public static readonly GPReg cl;
        public static readonly GPReg dl;
        public static readonly GPReg bl;

        public static readonly GPReg ah;
        public static readonly GPReg ch;
        public static readonly GPReg dh;
        public static readonly GPReg bh;

        public static readonly GPReg eax;
        public static readonly GPReg ecx;
        public static readonly GPReg edx;
        public static readonly GPReg ebx;
        public static readonly GPReg esp;
        public static readonly GPReg ebp;
        public static readonly GPReg esi;
        public static readonly GPReg edi;

        public static readonly GPReg Nax;
        public static readonly GPReg Ncx;
        public static readonly GPReg Ndx;
        public static readonly GPReg Nbx;
        public static readonly GPReg Nsp;
        public static readonly GPReg Nbp;
        public static readonly GPReg Nsi;
        public static readonly GPReg Ndi;
    }
}
