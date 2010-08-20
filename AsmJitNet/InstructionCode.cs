namespace AsmJitNet2
{
    public enum InstructionCode
    {
        Adc,           // X86/X64
        Add,           // X86/X64
        Addpd,
        Addps,
        Addsd,
        Addss,
        Addsubpd,
        Addsubps,
        AmdPrefetch,
        AmdPrefetchw,
        And,           // X86/X64
        Andnpd,
        Andnps,
        Andpd,
        Andps,
        Blendpd,
        Blendps,
        Blendvpd,
        Blendvps,
        Bsf,           // X86/X64
        Bsr,           // X86/X64
        Bswap,         // X86/X64 (i486)
        Bt,            // X86/X64
        Btc,           // X86/X64
        Btr,           // X86/X64
        Bts,           // X86/X64
        Call,          // X86/X64
        Cbw,           // X86/X64
        Cdqe,          // X64 only
        Clc,           // X86/X64
        Cld,           // X86/X64
        Clflush,
        Cmc,           // X86/X64

        Cmov,          // Begin (cmovcc) (i586)
        Cmova = Cmov,  //X86/X64 (cmovcc) (i586)
        Cmovae,        // X86/X64 (cmovcc) (i586)
        Cmovb,         // X86/X64 (cmovcc) (i586)
        Cmovbe,        // X86/X64 (cmovcc) (i586)
        Cmovc,         // X86/X64 (cmovcc) (i586)
        Cmove,         // X86/X64 (cmovcc) (i586)
        Cmovg,         // X86/X64 (cmovcc) (i586)
        Cmovge,        // X86/X64 (cmovcc) (i586)
        Cmovl,         // X86/X64 (cmovcc) (i586)
        Cmovle,        // X86/X64 (cmovcc) (i586)
        Cmovna,        // X86/X64 (cmovcc) (i586)
        Cmovnae,       // X86/X64 (cmovcc) (i586)
        Cmovnb,        // X86/X64 (cmovcc) (i586)
        Cmovnbe,       // X86/X64 (cmovcc) (i586)
        Cmovnc,        // X86/X64 (cmovcc) (i586)
        Cmovne,        // X86/X64 (cmovcc) (i586)
        Cmovng,        // X86/X64 (cmovcc) (i586)
        Cmovnge,       // X86/X64 (cmovcc) (i586)
        Cmovnl,        // X86/X64 (cmovcc) (i586)
        Cmovnle,       // X86/X64 (cmovcc) (i586)
        Cmovno,        // X86/X64 (cmovcc) (i586)
        Cmovnp,        // X86/X64 (cmovcc) (i586)
        Cmovns,        // X86/X64 (cmovcc) (i586)
        Cmovnz,        // X86/X64 (cmovcc) (i586)
        Cmovo,         // X86/X64 (cmovcc) (i586)
        Cmovp,         // X86/X64 (cmovcc) (i586)
        Cmovpe,        // X86/X64 (cmovcc) (i586)
        Cmovpo,        // X86/X64 (cmovcc) (i586)
        Cmovs,         // X86/X64 (cmovcc) (i586)
        Cmovz,         // X86/X64 (cmovcc) (i586)

        Cmp,           // X86/X64
        Cmppd,
        Cmpps,
        Cmpsd,
        Cmpss,
        Cmpxchg,       // X86/X64 (i486)
        Cmpxchg16b,    // X64 only
        Cmpxchg8b,     // X86/X64 (i586)
        Comisd,
        Comiss,
        Cpuid,         // X86/X64 (i486)
        Crc32,
        Cvtdq2pd,
        Cvtdq2ps,
        Cvtpd2dq,
        Cvtpd2pi,
        Cvtpd2ps,
        Cvtpi2pd,
        Cvtpi2ps,
        Cvtps2dq,
        Cvtps2pd,
        Cvtps2pi,
        Cvtsd2si,
        Cvtsd2ss,
        Cvtsi2sd,
        Cvtsi2ss,
        Cvtss2sd,
        Cvtss2si,
        Cvttpd2dq,
        Cvttpd2pi,
        Cvttps2dq,
        Cvttps2pi,
        Cvttsd2si,
        Cvttss2si,
        Cwde,          // X86/X64
        Daa,           // X86 only
        Das,           // X86 only
        Dec,           // X86/X64
        Div,           // X86/X64
        Divpd,
        Divps,
        Divsd,
        Divss,
        Dppd,
        Dpps,
        Emms,          // MMX
        Enter,         // X86/X64
        Extractps,
        F2xm1,         // X87
        Fabs,          // X87
        Fadd,          // X87
        Faddp,         // X87
        Fbld,          // X87
        Fbstp,         // X87
        Fchs,          // X87
        Fclex,         // X87
        Fcmovb,        // X87
        Fcmovbe,       // X87
        Fcmove,        // X87
        Fcmovnb,       // X87
        Fcmovnbe,      // X87
        Fcmovne,       // X87
        Fcmovnu,       // X87
        Fcmovu,        // X87
        Fcom,          // X87
        Fcomi,         // X87
        Fcomip,        // X87
        Fcomp,         // X87
        Fcompp,        // X87
        Fcos,          // X87
        Fdecstp,       // X87
        Fdiv,          // X87
        Fdivp,         // X87
        Fdivr,         // X87
        Fdivrp,        // X87
        Femms,         // 3dNow!
        Ffree,         // X87
        Fiadd,         // X87
        Ficom,         // X87
        Ficomp,        // X87
        Fidiv,         // X87
        Fidivr,        // X87
        Fild,          // X87
        Fimul,         // X87
        Fincstp,       // X87
        Finit,         // X87
        Fist,          // X87
        Fistp,         // X87
        Fisttp,
        Fisub,         // X87
        Fisubr,        // X87
        Fld,           // X87
        Fld1,          // X87
        Fldcw,         // X87
        Fldenv,        // X87
        Fldl2e,        // X87
        Fldl2t,        // X87
        Fldlg2,        // X87
        Fldln2,        // X87
        Fldpi,         // X87
        Fldz,          // X87
        Fmul,          // X87
        Fmulp,         // X87
        Fnclex,        // X87
        Fninit,        // X87
        Fnop,          // X87
        Fnsave,        // X87
        Fnstcw,        // X87
        Fnstenv,       // X87
        Fnstsw,        // X87
        Fpatan,        // X87
        Fprem,         // X87
        Fprem1,        // X87
        Fptan,         // X87
        Frndint,       // X87
        Frstor,        // X87
        Fsave,         // X87
        Fscale,        // X87
        Fsin,          // X87
        Fsincos,       // X87
        Fsqrt,         // X87
        Fst,           // X87
        Fstcw,         // X87
        Fstenv,        // X87
        Fstp,          // X87
        Fstsw,         // X87
        Fsub,          // X87
        Fsubp,         // X87
        Fsubr,         // X87
        Fsubrp,        // X87
        Ftst,          // X87
        Fucom,         // X87
        Fucomi,        // X87
        Fucomip,       // X87
        Fucomp,        // X87
        Fucompp,       // X87
        Fwait,         // X87
        Fxam,          // X87
        Fxch,          // X87
        Fxrstor,       // X87
        Fxsave,        // X87
        Fxtract,       // X87
        Fyl2x,         // X87
        Fyl2xp1,       // X87
        Haddpd,
        Haddps,
        Hsubpd,
        Hsubps,
        Idiv,          // X86/X64
        Imul,          // X86/X64
        Inc,           // X86/X64
        Int3,          // X86/X64

        J,             // Begin (jcc)
        Ja = J,        // X86/X64 (jcc)
        Jae,           // X86/X64 (jcc)
        Jb,            // X86/X64 (jcc)
        Jbe,           // X86/X64 (jcc)
        Jc,            // X86/X64 (jcc)
        Je,            // X86/X64 (jcc)
        Jg,            // X86/X64 (jcc)
        Jge,           // X86/X64 (jcc)
        Jl,            // X86/X64 (jcc)
        Jle,           // X86/X64 (jcc)
        Jna,           // X86/X64 (jcc)
        Jnae,          // X86/X64 (jcc)
        Jnb,           // X86/X64 (jcc)
        Jnbe,          // X86/X64 (jcc)
        Jnc,           // X86/X64 (jcc)
        Jne,           // X86/X64 (jcc)
        Jng,           // X86/X64 (jcc)
        Jnge,          // X86/X64 (jcc)
        Jnl,           // X86/X64 (jcc)
        Jnle,          // X86/X64 (jcc)
        Jno,           // X86/X64 (jcc)
        Jnp,           // X86/X64 (jcc)
        Jns,           // X86/X64 (jcc)
        Jnz,           // X86/X64 (jcc)
        Jo,            // X86/X64 (jcc)
        Jp,            // X86/X64 (jcc)
        Jpe,           // X86/X64 (jcc)
        Jpo,           // X86/X64 (jcc)
        Js,            // X86/X64 (jcc)
        Jz,            // X86/X64 (jcc)
        Jmp,           // X86/X64 (jmp)

        JShort,             // Begin (jcc_short)
        JaShort = JShort,        // X86/X64 (jcc_short)
        JaeShort,           // X86/X64 (jcc_short)
        JbShort,            // X86/X64 (jcc_short)
        JbeShort,           // X86/X64 (jcc_short)
        JcShort,            // X86/X64 (jcc_short)
        JeShort,            // X86/X64 (jcc_short)
        JgShort,            // X86/X64 (jcc_short)
        JgeShort,           // X86/X64 (jcc_short)
        JlShort,            // X86/X64 (jcc_short)
        JleShort,           // X86/X64 (jcc_short)
        JnaShort,           // X86/X64 (jcc_short)
        JnaeShort,          // X86/X64 (jcc_short)
        JnbShort,           // X86/X64 (jcc_short)
        JnbeShort,          // X86/X64 (jcc_short)
        JncShort,           // X86/X64 (jcc_short)
        JneShort,           // X86/X64 (jcc_short)
        JngShort,           // X86/X64 (jcc_short)
        JngeShort,          // X86/X64 (jcc_short)
        JnlShort,           // X86/X64 (jcc_short)
        JnleShort,          // X86/X64 (jcc_short)
        JnoShort,           // X86/X64 (jcc_short)
        JnpShort,           // X86/X64 (jcc_short)
        JnsShort,           // X86/X64 (jcc_short)
        JnzShort,           // X86/X64 (jcc_short)
        JoShort,            // X86/X64 (jcc_short)
        JpShort,            // X86/X64 (jcc_short)
        JpeShort,           // X86/X64 (jcc_short)
        JpoShort,           // X86/X64 (jcc_short)
        JsShort,            // X86/X64 (jcc_short)
        JzShort,            // X86/X64 (jcc_short)
        JmpShort,           // X86/X64 (jmp_short)

        Lddqu,
        Ldmxcsr,
        Lahf,          // X86/X64 (CPUID NEEDED)
        Lea,           // X86/X64
        Leave,         // X86/X64
        Lfence,
        Maskmovdqu,
        Maskmovq,      // MMX Extensions
        Maxpd,
        Maxps,
        Maxsd,
        Maxss,
        Mfence,
        Minpd,
        Minps,
        Minsd,
        Minss,
        Monitor,
        Mov,           // X86/X64
        Movapd,
        Movaps,
        Movbe,
        Movd,
        Movddup,
        Movdq2q,
        Movdqa,
        Movdqu,
        Movhlps,
        Movhpd,
        Movhps,
        Movlhps,
        Movlpd,
        Movlps,
        Movmskpd,
        Movmskps,
        Movntdq,
        Movntdqa,
        Movnti,
        Movntpd,
        Movntps,
        Movntq,        // MMX Extensions
        Movq,
        Movq2dq,
        Movsd,
        Movshdup,
        Movsldup,
        Movss,
        Movsx,         // X86/X64
        Movsxd,        // X86/X64
        Movupd,
        Movups,
        Movzx,         // X86/X64
        MovPtr,       // X86/X64
        Mpsadbw,
        Mul,           // X86/X64
        Mulpd,
        Mulps,
        Mulsd,
        Mulss,
        Mwait,
        Neg,           // X86/X64
        Nop,           // X86/X64
        Not,           // X86/X64
        Or,            // X86/X64
        Orpd,
        Orps,
        Pabsb,
        Pabsd,
        Pabsw,
        Packssdw,
        Packsswb,
        Packusdw,
        Packuswb,
        Paddb,
        Paddd,
        Paddq,
        Paddsb,
        Paddsw,
        Paddusb,
        Paddusw,
        Paddw,
        Palignr,
        Pand,
        Pandn,
        Pause,
        Pavgb,         // MMX Extensions
        Pavgw,         // MMX Extensions
        Pblendvb,
        Pblendw,
        Pcmpeqb,
        Pcmpeqd,
        Pcmpeqq,
        Pcmpeqw,
        Pcmpestri,
        Pcmpestrm,
        Pcmpgtb,
        Pcmpgtd,
        Pcmpgtq,
        Pcmpgtw,
        Pcmpistri,
        Pcmpistrm,
        Pextrb,
        Pextrd,
        Pextrq,
        Pextrw,        // MMX Extensions
        Pf2id,         // 3dNow!
        Pf2iw,         // 3dNow! Extensions
        Pfacc,         // 3dNow!
        Pfadd,         // 3dNow!
        Pfcmpeq,       // 3dNow!
        Pfcmpge,       // 3dNow!
        Pfcmpgt,       // 3dNow!
        Pfmax,         // 3dNow!
        Pfmin,         // 3dNow!
        Pfmul,         // 3dNow!
        Pfnacc,        // 3dNow! Extensions
        Pfpnacc,       // 3dNow! Extensions
        Pfrcp,         // 3dNow!
        Pfrcpit1,      // 3dNow!
        Pfrcpit2,      // 3dNow!
        Pfrsqit1,      // 3dNow!
        Pfrsqrt,       // 3dNow!
        Pfsub,         // 3dNow!
        Pfsubr,        // 3dNow!
        Phaddd,
        Phaddsw,
        Phaddw,
        Phminposuw,
        Phsubd,
        Phsubsw,
        Phsubw,
        Pi2fd,         // 3dNow!
        Pi2fw,         // 3dNow! Extensions
        Pinsrb,
        Pinsrd,
        Pinsrq,
        Pinsrw,        // MMX Extensions
        Pmaddubsw,
        Pmaddwd,
        Pmaxsb,
        Pmaxsd,
        Pmaxsw,        // MMX Extensions
        Pmaxub,        // MMX Extensions
        Pmaxud,
        Pmaxuw,
        Pminsb,
        Pminsd,
        Pminsw,        // MMX Extensions
        Pminub,        // MMX Extensions
        Pminud,
        Pminuw,
        Pmovmskb,      // MMX Extensions
        Pmovsxbd,
        Pmovsxbq,
        Pmovsxbw,
        Pmovsxdq,
        Pmovsxwd,
        Pmovsxwq,
        Pmovzxbd,
        Pmovzxbq,
        Pmovzxbw,
        Pmovzxdq,
        Pmovzxwd,
        Pmovzxwq,
        Pmuldq,
        Pmulhrsw,
        Pmulhuw,       // MMX Extensions
        Pmulhw,
        Pmulld,
        Pmullw,
        Pmuludq,
        Pop,           // X86/X64
        Popad,         // X86 only
        Popcnt,
        Popfd,         // X86 only
        Popfq,         // X64 only
        Por,
        Prefetch,      // MMX Extensions
        Psadbw,        // MMX Extensions
        Pshufb,
        Pshufd,
        Pshufw,        // MMX Extensions
        Pshufhw,
        Pshuflw,
        Psignb,
        Psignd,
        Psignw,
        Pslld,
        Pslldq,
        Psllq,
        Psllw,
        Psrad,
        Psraw,
        Psrld,
        Psrldq,
        Psrlq,
        Psrlw,
        Psubb,
        Psubd,
        Psubq,
        Psubsb,
        Psubsw,
        Psubusb,
        Psubusw,
        Psubw,
        Pswapd,        // 3dNow! Extensions
        Ptest,
        Punpckhbw,
        Punpckhdq,
        Punpckhqdq,
        Punpckhwd,
        Punpcklbw,
        Punpckldq,
        Punpcklqdq,
        Punpcklwd,
        Push,          // X86/X64
        Pushad,        // X86 only
        Pushfd,        // X86 only
        Pushfq,        // X64 only
        Pxor,
        Rcl,           // X86/X64
        Rcpps,
        Rcpss,
        Rcr,           // X86/X64
        Rdtsc,         // X86/X64
        Rdtscp,        // X86/X64
        RepLodsb,     // X86/X64 (REP)
        RepLodsd,     // X86/X64 (REP)
        RepLodsq,     // X64 only (REP)
        RepLodsw,     // X86/X64 (REP)
        RepMovsb,     // X86/X64 (REP)
        RepMovsd,     // X86/X64 (REP)
        RepMovsq,     // X64 only (REP)
        RepMovsw,     // X86/X64 (REP)
        RepStosb,     // X86/X64 (REP)
        RepStosd,     // X86/X64 (REP)
        RepStosq,     // X64 only (REP)
        RepStosw,     // X86/X64 (REP)
        RepeCmpsb,    // X86/X64 (REP)
        RepeCmpsd,    // X86/X64 (REP)
        RepeCmpsq,    // X64 only (REP)
        RepeCmpsw,    // X86/X64 (REP)
        RepeScasb,    // X86/X64 (REP)
        RepeScasd,    // X86/X64 (REP)
        RepeScasq,    // X64 only (REP)
        RepeScasw,    // X86/X64 (REP)
        RepneCmpsb,   // X86/X64 (REP)
        RepneCmpsd,   // X86/X64 (REP)
        RepneCmpsq,   // X64 only (REP)
        RepneCmpsw,   // X86/X64 (REP)
        RepneScasb,   // X86/X64 (REP)
        RepneScasd,   // X86/X64 (REP)
        RepneScasq,   // X64 only (REP)
        RepneScasw,   // X86/X64 (REP)
        Ret,           // X86/X64
        Rol,           // X86/X64
        Ror,           // X86/X64
        Roundpd,
        Roundps,
        Roundsd,
        Roundss,
        Rsqrtps,
        Rsqrtss,
        Sahf,          // X86/X64 (CPUID NEEDED)
        Sal,           // X86/X64
        Sar,           // X86/X64
        Sbb,           // X86/X64
        Set,           // Begin (setcc)
        Seta = Set, // X86/X64 (setcc)
        Setae,         // X86/X64 (setcc)
        Setb,          // X86/X64 (setcc)
        Setbe,         // X86/X64 (setcc)
        Setc,          // X86/X64 (setcc)
        Sete,          // X86/X64 (setcc)
        Setg,          // X86/X64 (setcc)
        Setge,         // X86/X64 (setcc)
        Setl,          // X86/X64 (setcc)
        Setle,         // X86/X64 (setcc)
        Setna,         // X86/X64 (setcc)
        Setnae,        // X86/X64 (setcc)
        Setnb,         // X86/X64 (setcc)
        Setnbe,        // X86/X64 (setcc)
        Setnc,         // X86/X64 (setcc)
        Setne,         // X86/X64 (setcc)
        Setng,         // X86/X64 (setcc)
        Setnge,        // X86/X64 (setcc)
        Setnl,         // X86/X64 (setcc)
        Setnle,        // X86/X64 (setcc)
        Setno,         // X86/X64 (setcc)
        Setnp,         // X86/X64 (setcc)
        Setns,         // X86/X64 (setcc)
        Setnz,         // X86/X64 (setcc)
        Seto,          // X86/X64 (setcc)
        Setp,          // X86/X64 (setcc)
        Setpe,         // X86/X64 (setcc)
        Setpo,         // X86/X64 (setcc)
        Sets,          // X86/X64 (setcc)
        Setz,          // X86/X64 (setcc)
        Sfence,        // MMX Extensions
        Shl,           // X86/X64
        Shld,          // X86/X64
        Shr,           // X86/X64
        Shrd,          // X86/X64
        Shufpd,
        Shufps,
        Sqrtpd,
        Sqrtps,
        Sqrtsd,
        Sqrtss,
        Stc,           // X86/X64
        Std,           // X86/X64
        Stmxcsr,
        Sub,           // X86/X64
        Subpd,
        Subps,
        Subsd,
        Subss,
        Test,          // X86/X64
        Ucomisd,
        Ucomiss,
        Ud2,           // X86/X64
        Unpckhpd,
        Unpckhps,
        Unpcklpd,
        Unpcklps,
        Xadd,          // X86/X64 (i486)
        Xchg,          // X86/X64 (i386)
        Xor,           // X86/X64
        Xorpd,
        Xorps,
    }
}
