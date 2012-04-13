namespace AsmJitNet
{
    public enum InstructionCode
    {
        Adc,           // X86/X64
        Add,           // X86/X64
        Addpd,         // SSE2
        Addps,         // SSE
        Addsd,         // SSE2
        Addss,         // SSE
        Addsubpd,      // SSE3
        Addsubps,      // SSE3
        AmdPrefetch,
        AmdPrefetchw,
        And,           // X86/X64
        Andnpd,        // SSE2
        Andnps,        // SSE
        Andpd,         // SSE2
        Andps,         // SSE
        Blendpd,       // SSE4.1
        Blendps,       // SSE4.1
        Blendvpd,      // SSE4.1
        Blendvps,      // SSE4.1
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
        Clflush,       // SSE2
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
        Cmppd,         // SSE2
        Cmpps,         // SSE
        Cmpsd,         // SSE2
        Cmpss,         // SSE
        Cmpxchg,       // X86/X64 (i486)
        Cmpxchg16b,    // X64 only
        Cmpxchg8b,     // X86/X64 (i586)
        Comisd,        // SSE2
        Comiss,        // SSE
        Cpuid,         // X86/X64 (i486)
        Crc32,         // SSE4.2
        Cvtdq2pd,      // SSE2
        Cvtdq2ps,      // SSE2
        Cvtpd2dq,      // SSE2
        Cvtpd2pi,      // SSE2
        Cvtpd2ps,      // SSE2
        Cvtpi2pd,      // SSE2
        Cvtpi2ps,      // SSE
        Cvtps2dq,      // SSE2
        Cvtps2pd,      // SSE2
        Cvtps2pi,      // SSE
        Cvtsd2si,      // SSE2
        Cvtsd2ss,      // SSE2
        Cvtsi2sd,      // SSE2
        Cvtsi2ss,      // SSE
        Cvtss2sd,      // SSE2
        Cvtss2si,      // SSE
        Cvttpd2dq,     // SSE2
        Cvttpd2pi,     // SSE2
        Cvttps2dq,     // SSE2
        Cvttps2pi,     // SSE
        Cvttsd2si,     // SSE2
        Cvttss2si,     // SSE
        Cwde,          // X86/X64
        Daa,           // X86 only
        Das,           // X86 only
        Dec,           // X86/X64
        Div,           // X86/X64
        Divpd,         // SSE2
        Divps,         // SSE
        Divsd,         // SSE2
        Divss,         // SSE
        Dppd,          // SSE4.1
        Dpps,          // SSE4.1
        Emms,          // MMX
        Enter,         // X86/X64
        Extractps,     // SSE4.1
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
        Fisttp,        // SSE3
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
        Haddpd,        // SSE3
        Haddps,        // SSE3
        Hsubpd,        // SSE3
        Hsubps,        // SSE3
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

        Lddqu,         // SSE3
        Ldmxcsr,       // SSE
        Lahf,          // X86/X64 (CPUID NEEDED)
        Lea,           // X86/X64
        Leave,         // X86/X64
        Lfence,        // SSE2
        Maskmovdqu,    // SSE2
        Maskmovq,      // MMX-Ext
        Maxpd,         // SSE2
        Maxps,         // SSE
        Maxsd,         // SSE2
        Maxss,         // SSE
        Mfence,        // SSE2
        Minpd,         // SSE2
        Minps,         // SSE
        Minsd,         // SSE2
        Minss,         // SSE
        Monitor,       // SSE3
        Mov,           // X86/X64
        Movapd,        // SSE2
        Movaps,        // SSE
        Movbe,         // SSE3 - Intel-Atom
        Movd,          // MMX/SSE2
        Movddup,       // SSE3
        Movdq2q,       // SSE2
        Movdqa,        // SSE2
        Movdqu,        // SSE2
        Movhlps,       // SSE
        Movhpd,        // SSE2
        Movhps,        // SSE
        Movlhps,       // SSE
        Movlpd,        // SSE2
        Movlps,        // SSE
        Movmskpd,      // SSE2
        Movmskps,      // SSE2
        Movntdq,       // SSE2
        Movntdqa,      // SSE4.1
        Movnti,        // SSE2
        Movntpd,       // SSE2
        Movntps,       // SSE
        Movntq,        // MMX-Ext
        Movq,          // MMX/SSE/SSE2
        Movq2dq,       // SSE2
        Movsd,         // SSE2
        Movshdup,      // SSE3
        Movsldup,      // SSE3
        Movss,         // SSE
        Movsx,         // X86/X64
        Movsxd,        // X86/X64
        Movupd,        // SSE2
        Movups,        // SSE
        Movzx,         // X86/X64
        MovPtr,       // X86/X64
        Mpsadbw,       // SSE4.1
        Mul,           // X86/X64
        Mulpd,         // SSE2
        Mulps,         // SSE
        Mulsd,         // SSE2
        Mulss,         // SSE
        Mwait,         // SSE3
        Neg,           // X86/X64
        Nop,           // X86/X64
        Not,           // X86/X64
        Or,            // X86/X64
        Orpd,          // SSE2
        Orps,          // SSE
        Pabsb,         // SSSE3
        Pabsd,         // SSSE3
        Pabsw,         // SSSE3
        Packssdw,      // MMX/SSE2
        Packsswb,      // MMX/SSE2
        Packusdw,      // SSE4.1
        Packuswb,      // MMX/SSE2
        Paddb,         // MMX/SSE2
        Paddd,         // MMX/SSE2
        Paddq,         // SSE2
        Paddsb,        // MMX/SSE2
        Paddsw,        // MMX/SSE2
        Paddusb,       // MMX/SSE2
        Paddusw,       // MMX/SSE2
        Paddw,         // MMX/SSE2
        Palignr,       // SSSE3
        Pand,          // MMX/SSE2
        Pandn,         // MMX/SSE2
        Pause,         // SSE2
        Pavgb,         // MMX-Ext
        Pavgw,         // MMX-Ext
        Pblendvb,      // SSE4.1
        Pblendw,       // SSE4.1
        Pcmpeqb,       // MMX/SSE2
        Pcmpeqd,       // MMX/SSE2
        Pcmpeqq,       // SSE4.1
        Pcmpeqw,       // MMX/SSE2
        Pcmpestri,     // SSE4.2
        Pcmpestrm,     // SSE4.2
        Pcmpgtb,       // MMX/SSE2
        Pcmpgtd,       // MMX/SSE2
        Pcmpgtq,       // SSE4.2
        Pcmpgtw,       // MMX/SSE2
        Pcmpistri,     // SSE4.2
        Pcmpistrm,     // SSE4.2
        Pextrb,        // SSE4.1
        Pextrd,        // SSE4.1
        Pextrq,        // SSE4.1
        Pextrw,        // MMX-Ext/SSE2
        Pf2id,         // 3dNow!
        Pf2iw,         // Enhanced 3dNow!
        Pfacc,         // 3dNow!
        Pfadd,         // 3dNow!
        Pfcmpeq,       // 3dNow!
        Pfcmpge,       // 3dNow!
        Pfcmpgt,       // 3dNow!
        Pfmax,         // 3dNow!
        Pfmin,         // 3dNow!
        Pfmul,         // 3dNow!
        Pfnacc,        // Enhanced 3dNow!
        Pfpnacc,       // Enhanced 3dNow!
        Pfrcp,         // 3dNow!
        Pfrcpit1,      // 3dNow!
        Pfrcpit2,      // 3dNow!
        Pfrsqit1,      // 3dNow!
        Pfrsqrt,       // 3dNow!
        Pfsub,         // 3dNow!
        Pfsubr,        // 3dNow!
        Phaddd,        // SSSE3
        Phaddsw,       // SSSE3
        Phaddw,        // SSSE3
        Phminposuw,    // SSE4.1
        Phsubd,        // SSSE3
        Phsubsw,       // SSSE3
        Phsubw,        // SSSE3
        Pi2fd,         // 3dNow!
        Pi2fw,         // Enhanced 3dNow!
        Pinsrb,        // SSE4.1
        Pinsrd,        // SSE4.1
        Pinsrq,        // SSE4.1
        Pinsrw,        // MMX-Ext
        Pmaddubsw,     // SSSE3
        Pmaddwd,       // MMX/SSE2
        Pmaxsb,        // SSE4.1
        Pmaxsd,        // SSE4.1
        Pmaxsw,        // MMX-Ext
        Pmaxub,        // MMX-Ext
        Pmaxud,        // SSE4.1
        Pmaxuw,        // SSE4.1
        Pminsb,        // SSE4.1
        Pminsd,        // SSE4.1
        Pminsw,        // MMX-Ext
        Pminub,        // MMX-Ext
        Pminud,        // SSE4.1
        Pminuw,        // SSE4.1
        Pmovmskb,      // MMX-Ext
        Pmovsxbd,      // SSE4.1
        Pmovsxbq,      // SSE4.1
        Pmovsxbw,      // SSE4.1
        Pmovsxdq,      // SSE4.1
        Pmovsxwd,      // SSE4.1
        Pmovsxwq,      // SSE4.1
        Pmovzxbd,      // SSE4.1
        Pmovzxbq,      // SSE4.1
        Pmovzxbw,      // SSE4.1
        Pmovzxdq,      // SSE4.1
        Pmovzxwd,      // SSE4.1
        Pmovzxwq,      // SSE4.1
        Pmuldq,        // SSE4.1
        Pmulhrsw,      // SSSE3
        Pmulhuw,       // MMX-Ext
        Pmulhw,        // MMX/SSE2
        Pmulld,        // SSE4.2
        Pmullw,        // MMX/SSE2
        Pmuludq,       // SSE2
        Pop,           // X86/X64
        Popad,         // X86 only
        Popcnt,        // SSE4.2
        Popfd,         // X86 only
        Popfq,         // X64 only
        Por,           // MMX/SSE2
        Prefetch,      // MMX-Ext
        Psadbw,        // MMX-Ext
        Pshufb,        // SSSE3
        Pshufd,        // SSE2
        Pshufw,        // MMX-Ext
        Pshufhw,       // SSE2
        Pshuflw,       // SSE2
        Psignb,        // SSSE3
        Psignd,        // SSSE3
        Psignw,        // SSSE3
        Pslld,         // MMX/SSE2
        Pslldq,        // SSE2
        Psllq,         // MMX/SSE2
        Psllw,         // MMX/SSE2
        Psrad,         // MMX/SSE2
        Psraw,         // MMX/SSE2
        Psrld,         // MMX/SSE2
        Psrldq,        // SSE2
        Psrlq,         // MMX/SSE2
        Psrlw,         // MMX/SSE2
        Psubb,         // MMX/SSE2
        Psubd,         // MMX/SSE2
        Psubq,         // SSE2
        Psubsb,        // MMX/SSE2
        Psubsw,        // MMX/SSE2
        Psubusb,       // MMX/SSE2
        Psubusw,       // MMX/SSE2
        Psubw,         // MMX/SSE2
        Pswapd,        // Enhanced 3dNow!
        Ptest,         // SSE4.1
        Punpckhbw,     // MMX/SSE2
        Punpckhdq,     // MMX/SSE2
        Punpckhqdq,    // SSE2
        Punpckhwd,     // MMX/SSE2
        Punpcklbw,     // MMX/SSE2
        Punpckldq,     // MMX/SSE2
        Punpcklqdq,    // SSE2
        Punpcklwd,     // MMX/SSE2
        Push,          // X86/X64
        Pushad,        // X86 only
        Pushfd,        // X86 only
        Pushfq,        // X64 only
        Pxor,          // MMX/SSE2
        Rcl,           // X86/X64
        Rcpps,         // SSE
        Rcpss,         // SSE
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
        Roundpd,       // SSE4.1
        Roundps,       // SSE4.1
        Roundsd,       // SSE4.1
        Roundss,       // SSE4.1
        Rsqrtps,       // SSE
        Rsqrtss,       // SSE
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
        Sfence,        // MMX-Ext/SSE
        Shl,           // X86/X64
        Shld,          // X86/X64
        Shr,           // X86/X64
        Shrd,          // X86/X64
        Shufpd,        // SSE2
        Shufps,        // SSE
        Sqrtpd,        // SSE2
        Sqrtps,        // SSE
        Sqrtsd,        // SSE2
        Sqrtss,        // SSE
        Stc,           // X86/X64
        Std,           // X86/X64
        Stmxcsr,       // SSE
        Sub,           // X86/X64
        Subpd,         // SSE2
        Subps,         // SSE
        Subsd,         // SSE2
        Subss,         // SSE
        Test,          // X86/X64
        Ucomisd,       // SSE2
        Ucomiss,       // SSE
        Ud2,           // X86/X64
        Unpckhpd,      // SSE2
        Unpckhps,      // SSE
        Unpcklpd,      // SSE2
        Unpcklps,      // SSE
        Xadd,          // X86/X64 (i486)
        Xchg,          // X86/X64 (i386)
        Xor,           // X86/X64
        Xorpd,         // SSE2
        Xorps,         // SSE
    }
}
