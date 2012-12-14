// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// This file is used to test opcodes generated by AsmJit. Output can be
// disassembled in your IDE or by your favourite disassembler. Instructions
// are sorted alphabetically.

// [Dependencies - AsmJit]
#include <asmjit/asmjit.h>

// [Dependencies - C]
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This is type of function we will generate
typedef void (*MyFn)();

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // Create assembler.
  X86Assembler a;

  // Log assembler output.
  FileLogger logger(stderr);
  logger.setLogBinary(true);
  a.setLogger(&logger);

  a.add(eax, 1);

  // We don't want to crash :)
  a.ret();

  // Instructions.
  a.adc(zax,zax);
  a.adc(zax,sysint_ptr(zax));
  a.adc(zax,0);
  a.adc(sysint_ptr(zax),zax);
  a.adc(sysint_ptr(zax),0);
  a.add(zax,zax);
  a.add(zax,sysint_ptr(zax));
  a.add(zax,0);
  a.add(sysint_ptr(zax),zax);
  a.add(sysint_ptr(zax),0);
  a.and_(zax,zax);
  a.and_(zax,sysint_ptr(zax));
  a.and_(zax,0);
  a.and_(sysint_ptr(zax),zax);
  a.and_(sysint_ptr(zax),0);
  a.bswap(zax);
  a.bt(zax,zax);
  a.bt(sysint_ptr(zax),zax);
  a.bt(zax,0);
  a.bt(sysint_ptr(zax),0);
  a.btc(zax,zax);
  a.btc(sysint_ptr(zax),zax);
  a.btc(zax,0);
  a.btc(sysint_ptr(zax),0);
  a.btr(zax,zax);
  a.btr(sysint_ptr(zax),zax);
  a.btr(zax,0);
  a.btr(sysint_ptr(zax),0);
  a.bts(zax,zax);
  a.bts(sysint_ptr(zax),zax);
  a.bts(zax,0);
  a.bts(sysint_ptr(zax),0);
  a.call(zax);
  a.call(sysint_ptr(zax));
  a.cbw();
  a.cwde();
  a.clc();
  a.cld();
  a.cmc();
  a.cmp(zax,zax);
  a.cmp(zax,sysint_ptr(zax));
  a.cmp(zax,0);
  a.cmp(sysint_ptr(zax),zax);
  a.cmp(sysint_ptr(zax),0);
  a.cmpxchg(zax,zax);
  a.cmpxchg(sysint_ptr(zax),zax);
  a.cmpxchg8b(ptr(zax));
  a.cpuid();
  a.dec(zax);
  a.dec(sysint_ptr(zax));
  a.div(zax);
  a.div(sysint_ptr(zax));
  a.idiv(zax);
  a.idiv(sysint_ptr(zax));
  a.imul(zax);
  a.imul(sysint_ptr(zax));
  a.imul(zax,zax);
  a.imul(zax,sysint_ptr(zax));
  a.imul(zax,0);
  a.imul(zax,zax,0);
  a.imul(zax,sysint_ptr(zax),0);
  a.inc(zax);
  a.inc(sysint_ptr(zax));
  a.int3();
  a.lea(zax,sysint_ptr(zax));
  a.mov(zax,zax);
  a.mov(zax,sysint_ptr(zax));
  a.mov(zax,0);
  a.mov(sysint_ptr(zax),zax);
  a.mov(sysint_ptr(zax),0);
  a.movsx(zax,al);
  a.movsx(zax,byte_ptr(zax));
  a.movzx(zax,al);
  a.movzx(zax,byte_ptr(zax));
  a.mul(zax);
  a.mul(sysint_ptr(zax));
  a.neg(zax);
  a.neg(sysint_ptr(zax));
  a.nop();
  a.not_(zax);
  a.not_(sysint_ptr(zax));
  a.or_(zax,zax);
  a.or_(zax,sysint_ptr(zax));
  a.or_(zax,0);
  a.or_(sysint_ptr(zax),zax);
  a.or_(sysint_ptr(zax),0);
  a.pop(zax);
  a.pop(sysint_ptr(zax));
  a.push(zax);
  a.push(sysint_ptr(zax));
  a.push(0);
  a.rcl(zax,cl);
  a.rcl(zax,0);
  a.rcl(sysint_ptr(zax),cl);
  a.rcl(sysint_ptr(zax),0);
  a.rcr(zax,cl);
  a.rcr(zax,0);
  a.rcr(sysint_ptr(zax),cl);
  a.rcr(sysint_ptr(zax),0);
  a.rdtsc();
  a.rdtscp();
  a.ret();
  a.ret(0);
  a.rol(zax,cl);
  a.rol(zax,0);
  a.rol(sysint_ptr(zax),cl);
  a.rol(sysint_ptr(zax),0);
  a.ror(zax,cl);
  a.ror(zax,0);
  a.ror(sysint_ptr(zax),cl);
  a.ror(sysint_ptr(zax),0);
  a.sbb(zax,zax);
  a.sbb(zax,sysint_ptr(zax));
  a.sbb(zax,0);
  a.sbb(sysint_ptr(zax),zax);
  a.sbb(sysint_ptr(zax),0);
  a.sal(zax,cl);
  a.sal(zax,0);
  a.sal(sysint_ptr(zax),cl);
  a.sal(sysint_ptr(zax),0);
  a.sar(zax,cl);
  a.sar(zax,0);
  a.sar(sysint_ptr(zax),cl);
  a.sar(sysint_ptr(zax),0);
  a.shl(zax,cl);
  a.shl(zax,0);
  a.shl(sysint_ptr(zax),cl);
  a.shl(sysint_ptr(zax),0);
  a.shr(zax,cl);
  a.shr(zax,0);
  a.shr(sysint_ptr(zax),cl);
  a.shr(sysint_ptr(zax),0);
  a.shld(zax,zax,cl);
  a.shld(zax,zax,0);
  a.shld(sysint_ptr(zax),zax,cl);
  a.shld(sysint_ptr(zax),zax,0);
  a.shrd(zax,zax,cl);
  a.shrd(zax,zax,0);
  a.shrd(sysint_ptr(zax),zax,cl);
  a.shrd(sysint_ptr(zax),zax,0);
  a.stc();
  a.std();
  a.sub(zax,zax);
  a.sub(zax,sysint_ptr(zax));
  a.sub(zax,0);
  a.sub(sysint_ptr(zax),zax);
  a.sub(sysint_ptr(zax),0);
  a.test(zax,zax);
  a.test(zax,0);
  a.test(sysint_ptr(zax),zax);
  a.test(sysint_ptr(zax),0);
  a.ud2();
  a.xadd(zax,zax);
  a.xadd(sysint_ptr(zax),zax);
  a.xchg(zax,zax);
  a.xchg(sysint_ptr(zax),zax);
  a.xchg(zax,sysint_ptr(zax));
  a.xor_(zax,zax);
  a.xor_(zax,sysint_ptr(zax));
  a.xor_(zax,0);
  a.xor_(sysint_ptr(zax),zax);
  a.xor_(sysint_ptr(zax),0);

  a.f2xm1();
  a.fabs();
  a.fadd(dword_ptr(zax));
  a.fbld(dword_ptr(zax));
  a.fbstp(dword_ptr(zax));
  a.fchs();
  a.fclex();
  a.fcom(dword_ptr(zax));
  a.fcomp(dword_ptr(zax));
  a.fcompp();
  a.fcos();
  a.fdecstp();
  a.fdiv(dword_ptr(zax));
  a.fdivr(dword_ptr(zax));
  a.fiadd(dword_ptr(zax));
  a.ficom(dword_ptr(zax));
  a.ficomp(dword_ptr(zax));
  a.fidiv(dword_ptr(zax));
  a.fidivr(dword_ptr(zax));
  a.fild(dword_ptr(zax));
  a.fimul(dword_ptr(zax));
  a.fincstp();
  a.finit();
  a.fisub(dword_ptr(zax));
  a.fisubr(dword_ptr(zax));
  a.fninit();
  a.fist(dword_ptr(zax));
  a.fistp(dword_ptr(zax));
  a.fld(dword_ptr(zax));
  a.fld1();
  a.fldl2t();
  a.fldl2e();
  a.fldpi();
  a.fldlg2();
  a.fldln2();
  a.fldz();
  a.fldcw(ptr(zax));
  a.fldenv(ptr(zax));
  a.fmul(dword_ptr(zax));
  a.fnclex();
  a.fnop();
  a.fnsave(ptr(zax));
  a.fnstenv(ptr(zax));
  a.fnstcw(ptr(zax));
  a.fpatan();
  a.fprem();
  a.fprem1();
  a.fptan();
  a.frndint();
  a.frstor(ptr(zax));
  a.fsave(ptr(zax));
  a.fscale();
  a.fsin();
  a.fsincos();
  a.fsqrt();
  a.fst(dword_ptr(zax));
  a.fstp(dword_ptr(zax));
  a.fstcw(ptr(zax));
  a.fstenv(ptr(zax));
  a.fsub(dword_ptr(zax));
  a.fsubr(dword_ptr(zax));
  a.ftst();
  a.fucompp();
  a.fxam();
  a.fxrstor(ptr(zax));
  a.fxsave(ptr(zax));
  a.fxtract();
  a.fyl2x();
  a.fyl2xp1();
  a.emms();

  a.movd(ptr(zax),mm0);
  a.movd(eax,mm0);
  a.movd(mm0,ptr(zax));
  a.movd(mm0,eax);
  a.movq(mm0,mm0);
  a.movq(ptr(zax),mm0);
  a.movq(mm0,ptr(zax));
  a.packuswb(mm0,mm0);
  a.packuswb(mm0,ptr(zax));
  a.paddb(mm0,mm0);
  a.paddb(mm0,ptr(zax));
  a.paddw(mm0,mm0);
  a.paddw(mm0,ptr(zax));
  a.paddd(mm0,mm0);
  a.paddd(mm0,ptr(zax));
  a.paddsb(mm0,mm0);
  a.paddsb(mm0,ptr(zax));
  a.paddsw(mm0,mm0);
  a.paddsw(mm0,ptr(zax));
  a.paddusb(mm0,mm0);
  a.paddusb(mm0,ptr(zax));
  a.paddusw(mm0,mm0);
  a.paddusw(mm0,ptr(zax));
  a.pand(mm0,mm0);
  a.pand(mm0,ptr(zax));
  a.pandn(mm0,mm0);
  a.pandn(mm0,ptr(zax));
  a.pcmpeqb(mm0,mm0);
  a.pcmpeqb(mm0,ptr(zax));
  a.pcmpeqw(mm0,mm0);
  a.pcmpeqw(mm0,ptr(zax));
  a.pcmpeqd(mm0,mm0);
  a.pcmpeqd(mm0,ptr(zax));
  a.pcmpgtb(mm0,mm0);
  a.pcmpgtb(mm0,ptr(zax));
  a.pcmpgtw(mm0,mm0);
  a.pcmpgtw(mm0,ptr(zax));
  a.pcmpgtd(mm0,mm0);
  a.pcmpgtd(mm0,ptr(zax));
  a.pmulhw(mm0,mm0);
  a.pmulhw(mm0,ptr(zax));
  a.pmullw(mm0,mm0);
  a.pmullw(mm0,ptr(zax));
  a.por(mm0,mm0);
  a.por(mm0,ptr(zax));
  a.pmaddwd(mm0,mm0);
  a.pmaddwd(mm0,ptr(zax));
  a.pslld(mm0,mm0);
  a.pslld(mm0,ptr(zax));
  a.pslld(mm0,0);
  a.psllq(mm0,mm0);
  a.psllq(mm0,ptr(zax));
  a.psllq(mm0,0);
  a.psllw(mm0,mm0);
  a.psllw(mm0,ptr(zax));
  a.psllw(mm0,0);
  a.psrad(mm0,mm0);
  a.psrad(mm0,ptr(zax));
  a.psrad(mm0,0);
  a.psraw(mm0,mm0);
  a.psraw(mm0,ptr(zax));
  a.psraw(mm0,0);
  a.psrld(mm0,mm0);
  a.psrld(mm0,ptr(zax));
  a.psrld(mm0,0);
  a.psrlq(mm0,mm0);
  a.psrlq(mm0,ptr(zax));
  a.psrlq(mm0,0);
  a.psrlw(mm0,mm0);
  a.psrlw(mm0,ptr(zax));
  a.psrlw(mm0,0);
  a.psubb(mm0,mm0);
  a.psubb(mm0,ptr(zax));
  a.psubw(mm0,mm0);
  a.psubw(mm0,ptr(zax));
  a.psubd(mm0,mm0);
  a.psubd(mm0,ptr(zax));
  a.psubsb(mm0,mm0);
  a.psubsb(mm0,ptr(zax));
  a.psubsw(mm0,mm0);
  a.psubsw(mm0,ptr(zax));
  a.psubusb(mm0,mm0);
  a.psubusb(mm0,ptr(zax));
  a.psubusw(mm0,mm0);
  a.psubusw(mm0,ptr(zax));
  a.punpckhbw(mm0,mm0);
  a.punpckhbw(mm0,ptr(zax));
  a.punpckhwd(mm0,mm0);
  a.punpckhwd(mm0,ptr(zax));
  a.punpckhdq(mm0,mm0);
  a.punpckhdq(mm0,ptr(zax));
  a.punpcklbw(mm0,mm0);
  a.punpcklbw(mm0,ptr(zax));
  a.punpcklwd(mm0,mm0);
  a.punpcklwd(mm0,ptr(zax));
  a.punpckldq(mm0,mm0);
  a.punpckldq(mm0,ptr(zax));
  a.pxor(mm0,mm0);
  a.pxor(mm0,ptr(zax));

  a.addps(xmm0,xmm0);
  a.addps(xmm0,ptr(zax));
  a.addss(xmm0,xmm0);
  a.addss(xmm0,ptr(zax));
  a.andnps(xmm0,xmm0);
  a.andnps(xmm0,ptr(zax));
  a.andps(xmm0,xmm0);
  a.andps(xmm0,ptr(zax));
  a.cmpps(xmm0,xmm0,0);
  a.cmpps(xmm0,ptr(zax),0);
  a.cmpss(xmm0,xmm0,0);
  a.cmpss(xmm0,ptr(zax),0);
  a.comiss(xmm0,xmm0);
  a.comiss(xmm0,ptr(zax));
  a.cvtpi2ps(xmm0,mm0);
  a.cvtpi2ps(xmm0,ptr(zax));
  a.cvtps2pi(mm0,xmm0);
  a.cvtps2pi(mm0,ptr(zax));
  a.cvtsi2ss(xmm0,zax);
  a.cvtsi2ss(xmm0,ptr(zax));
  a.cvtss2si(zax,xmm0);
  a.cvtss2si(zax,ptr(zax));
  a.cvttps2pi(mm0,xmm0);
  a.cvttps2pi(mm0,ptr(zax));
  a.cvttss2si(zax,xmm0);
  a.cvttss2si(zax,ptr(zax));
  a.divps(xmm0,xmm0);
  a.divps(xmm0,ptr(zax));
  a.divss(xmm0,xmm0);
  a.divss(xmm0,ptr(zax));
  a.ldmxcsr(ptr(zax));
  a.maskmovq(mm0,mm0);
  a.maxps(xmm0,xmm0);
  a.maxps(xmm0,ptr(zax));
  a.maxss(xmm0,xmm0);
  a.maxss(xmm0,ptr(zax));
  a.minps(xmm0,xmm0);
  a.minps(xmm0,ptr(zax));
  a.minss(xmm0,xmm0);
  a.minss(xmm0,ptr(zax));
  a.movaps(xmm0,xmm0);
  a.movaps(xmm0,ptr(zax));
  a.movaps(ptr(zax),xmm0);
  a.movd(ptr(zax),xmm0);
  a.movd(eax,xmm0);
  a.movd(xmm0,ptr(zax));
  a.movd(xmm0,eax);
  a.movq(mm0, mm0);
  a.movq(xmm0,xmm0);
  a.movq(ptr(zax),xmm0);
  a.movq(xmm0,ptr(zax));
  a.movntq(ptr(zax),mm0);
  a.movhlps(xmm0,xmm0);
  a.movhps(xmm0,ptr(zax));
  a.movhps(ptr(zax),xmm0);
  a.movlhps(xmm0,xmm0);
  a.movlps(xmm0,ptr(zax));
  a.movlps(ptr(zax),xmm0);
  a.movntps(ptr(zax),xmm0);
  a.movss(xmm0,ptr(zax));
  a.movss(ptr(zax),xmm0);
  a.movups(xmm0,xmm0);
  a.movups(xmm0,ptr(zax));
  a.movups(ptr(zax),xmm0);
  a.mulps(xmm0,xmm0);
  a.mulps(xmm0,ptr(zax));
  a.mulss(xmm0,xmm0);
  a.mulss(xmm0,ptr(zax));
  a.orps(xmm0,xmm0);
  a.orps(xmm0,ptr(zax));
  a.pavgb(mm0,mm0);
  a.pavgb(mm0,ptr(zax));
  a.pavgw(mm0,mm0);
  a.pavgw(mm0,ptr(zax));
  a.pextrw(zax,mm0,0);
  a.pinsrw(mm0,eax,0);
  a.pinsrw(mm0,ptr(zax),0);
  a.pmaxsw(mm0,mm0);
  a.pmaxsw(mm0,ptr(zax));
  a.pmaxub(mm0,mm0);
  a.pmaxub(mm0,ptr(zax));
  a.pminsw(mm0,mm0);
  a.pminsw(mm0,ptr(zax));
  a.pminub(mm0,mm0);
  a.pminub(mm0,ptr(zax));
  a.pmovmskb(zax,mm0);
  a.pmulhuw(mm0,mm0);
  a.pmulhuw(mm0,ptr(zax));
  a.psadbw(mm0,mm0);
  a.psadbw(mm0,ptr(zax));
  a.pshufw(mm0,mm0,0);
  a.pshufw(mm0,ptr(zax),0);
  a.rcpps(xmm0,xmm0);
  a.rcpps(xmm0,ptr(zax));
  a.rcpss(xmm0,xmm0);
  a.rcpss(xmm0,ptr(zax));
  a.prefetch(ptr(zax),0);
  a.psadbw(xmm0,xmm0);
  a.psadbw(xmm0,ptr(zax));
  a.rsqrtps(xmm0,xmm0);
  a.rsqrtps(xmm0,ptr(zax));
  a.rsqrtss(xmm0,xmm0);
  a.rsqrtss(xmm0,ptr(zax));
  a.sfence();
  a.shufps(xmm0,xmm0,0);
  a.shufps(xmm0,ptr(zax),0);
  a.sqrtps(xmm0,xmm0);
  a.sqrtps(xmm0,ptr(zax));
  a.sqrtss(xmm0,xmm0);
  a.sqrtss(xmm0,ptr(zax));
  a.stmxcsr(ptr(zax));
  a.subps(xmm0,xmm0);
  a.subps(xmm0,ptr(zax));
  a.subss(xmm0,xmm0);
  a.subss(xmm0,ptr(zax));
  a.ucomiss(xmm0,xmm0);
  a.ucomiss(xmm0,ptr(zax));
  a.unpckhps(xmm0,xmm0);
  a.unpckhps(xmm0,ptr(zax));
  a.unpcklps(xmm0,xmm0);
  a.unpcklps(xmm0,ptr(zax));
  a.xorps(xmm0,xmm0);
  a.xorps(xmm0,ptr(zax));

  a.addpd(xmm0,xmm0);
  a.addpd(xmm0,ptr(zax));
  a.addsd(xmm0,xmm0);
  a.addsd(xmm0,ptr(zax));
  a.andnpd(xmm0,xmm0);
  a.andnpd(xmm0,ptr(zax));
  a.andpd(xmm0,xmm0);
  a.andpd(xmm0,ptr(zax));
  a.clflush(ptr(zax));
  a.cmppd(xmm0,xmm0,0);
  a.cmppd(xmm0,ptr(zax),0);
  a.cmpsd(xmm0,xmm0,0);
  a.cmpsd(xmm0,ptr(zax),0);
  a.comisd(xmm0,xmm0);
  a.comisd(xmm0,ptr(zax));
  a.cvtdq2pd(xmm0,xmm0);
  a.cvtdq2pd(xmm0,ptr(zax));
  a.cvtdq2ps(xmm0,xmm0);
  a.cvtdq2ps(xmm0,ptr(zax));
  a.cvtpd2dq(xmm0,xmm0);
  a.cvtpd2dq(xmm0,ptr(zax));
  a.cvtpd2pi(mm0,xmm0);
  a.cvtpd2pi(mm0,ptr(zax));
  a.cvtpd2ps(xmm0,xmm0);
  a.cvtpd2ps(xmm0,ptr(zax));
  a.cvtpi2pd(xmm0,mm0);
  a.cvtpi2pd(xmm0,ptr(zax));
  a.cvtps2dq(xmm0,xmm0);
  a.cvtps2dq(xmm0,ptr(zax));
  a.cvtps2pd(xmm0,xmm0);
  a.cvtps2pd(xmm0,ptr(zax));
  a.cvtsd2si(zax,xmm0);
  a.cvtsd2si(zax,ptr(zax));
  a.cvtsd2ss(xmm0,xmm0);
  a.cvtsd2ss(xmm0,ptr(zax));
  a.cvtsi2sd(xmm0,zax);
  a.cvtsi2sd(xmm0,ptr(zax));
  a.cvtss2sd(xmm0,xmm0);
  a.cvtss2sd(xmm0,ptr(zax));
  a.cvtss2si(zax,xmm0);
  a.cvtss2si(zax,ptr(zax));
  a.cvttpd2pi(mm0,xmm0);
  a.cvttpd2pi(mm0,ptr(zax));
  a.cvttpd2dq(xmm0,xmm0);
  a.cvttpd2dq(xmm0,ptr(zax));
  a.cvttps2dq(xmm0,xmm0);
  a.cvttps2dq(xmm0,ptr(zax));
  a.cvttsd2si(zax,xmm0);
  a.cvttsd2si(zax,ptr(zax));
  a.divpd(xmm0,xmm0);
  a.divpd(xmm0,ptr(zax));
  a.divsd(xmm0,xmm0);
  a.divsd(xmm0,ptr(zax));
  a.lfence();
  a.maskmovdqu(xmm0,xmm0);
  a.maxpd(xmm0,xmm0);
  a.maxpd(xmm0,ptr(zax));
  a.maxsd(xmm0,xmm0);
  a.maxsd(xmm0,ptr(zax));
  a.mfence();
  a.minpd(xmm0,xmm0);
  a.minpd(xmm0,ptr(zax));
  a.minsd(xmm0,xmm0);
  a.minsd(xmm0,ptr(zax));
  a.movdqa(xmm0,xmm0);
  a.movdqa(xmm0,ptr(zax));
  a.movdqa(ptr(zax),xmm0);
  a.movdqu(xmm0,xmm0);
  a.movdqu(xmm0,ptr(zax));
  a.movdqu(ptr(zax),xmm0);
  a.movmskps(zax,xmm0);
  a.movmskpd(zax,xmm0);
  a.movsd(xmm0,xmm0);
  a.movsd(xmm0,ptr(zax));
  a.movsd(ptr(zax),xmm0);
  a.movapd(xmm0,ptr(zax));
  a.movapd(ptr(zax),xmm0);
  a.movdq2q(mm0,xmm0);
  a.movq2dq(xmm0,mm0);
  a.movhpd(xmm0,ptr(zax));
  a.movhpd(ptr(zax),xmm0);
  a.movlpd(xmm0,ptr(zax));
  a.movlpd(ptr(zax),xmm0);
  a.movntdq(ptr(zax),xmm0);
  a.movnti(ptr(zax),zax);
  a.movntpd(ptr(zax),xmm0);
  a.movupd(xmm0,ptr(zax));
  a.movupd(ptr(zax),xmm0);
  a.mulpd(xmm0,xmm0);
  a.mulpd(xmm0,ptr(zax));
  a.mulsd(xmm0,xmm0);
  a.mulsd(xmm0,ptr(zax));
  a.orpd(xmm0,xmm0);
  a.orpd(xmm0,ptr(zax));
  a.packsswb(xmm0,xmm0);
  a.packsswb(xmm0,ptr(zax));
  a.packssdw(xmm0,xmm0);
  a.packssdw(xmm0,ptr(zax));
  a.packuswb(xmm0,xmm0);
  a.packuswb(xmm0,ptr(zax));
  a.paddb(xmm0,xmm0);
  a.paddb(xmm0,ptr(zax));
  a.paddw(xmm0,xmm0);
  a.paddw(xmm0,ptr(zax));
  a.paddd(xmm0,xmm0);
  a.paddd(xmm0,ptr(zax));
  a.paddq(mm0,mm0);
  a.paddq(mm0,ptr(zax));
  a.paddq(xmm0,xmm0);
  a.paddq(xmm0,ptr(zax));
  a.paddsb(xmm0,xmm0);
  a.paddsb(xmm0,ptr(zax));
  a.paddsw(xmm0,xmm0);
  a.paddsw(xmm0,ptr(zax));
  a.paddusb(xmm0,xmm0);
  a.paddusb(xmm0,ptr(zax));
  a.paddusw(xmm0,xmm0);
  a.paddusw(xmm0,ptr(zax));
  a.pand(xmm0,xmm0);
  a.pand(xmm0,ptr(zax));
  a.pandn(xmm0,xmm0);
  a.pandn(xmm0,ptr(zax));
  a.pause();
  a.pavgb(xmm0,xmm0);
  a.pavgb(xmm0,ptr(zax));
  a.pavgw(xmm0,xmm0);
  a.pavgw(xmm0,ptr(zax));
  a.pcmpeqb(xmm0,xmm0);
  a.pcmpeqb(xmm0,ptr(zax));
  a.pcmpeqw(xmm0,xmm0);
  a.pcmpeqw(xmm0,ptr(zax));
  a.pcmpeqd(xmm0,xmm0);
  a.pcmpeqd(xmm0,ptr(zax));
  a.pcmpgtb(xmm0,xmm0);
  a.pcmpgtb(xmm0,ptr(zax));
  a.pcmpgtw(xmm0,xmm0);
  a.pcmpgtw(xmm0,ptr(zax));
  a.pcmpgtd(xmm0,xmm0);
  a.pcmpgtd(xmm0,ptr(zax));
  a.pmaxsw(xmm0,xmm0);
  a.pmaxsw(xmm0,ptr(zax));
  a.pmaxub(xmm0,xmm0);
  a.pmaxub(xmm0,ptr(zax));
  a.pminsw(xmm0,xmm0);
  a.pminsw(xmm0,ptr(zax));
  a.pminub(xmm0,xmm0);
  a.pminub(xmm0,ptr(zax));
  a.pmovmskb(zax,xmm0);
  a.pmulhw(xmm0,xmm0);
  a.pmulhw(xmm0,ptr(zax));
  a.pmulhuw(xmm0,xmm0);
  a.pmulhuw(xmm0,ptr(zax));
  a.pmullw(xmm0,xmm0);
  a.pmullw(xmm0,ptr(zax));
  a.pmuludq(mm0,mm0);
  a.pmuludq(mm0,ptr(zax));
  a.pmuludq(xmm0,xmm0);
  a.pmuludq(xmm0,ptr(zax));
  a.por(xmm0,xmm0);
  a.por(xmm0,ptr(zax));
  a.pslld(xmm0,xmm0);
  a.pslld(xmm0,ptr(zax));
  a.pslld(xmm0,0);
  a.psllq(xmm0,xmm0);
  a.psllq(xmm0,ptr(zax));
  a.psllq(xmm0,0);
  a.psllw(xmm0,xmm0);
  a.psllw(xmm0,ptr(zax));
  a.psllw(xmm0,0);
  a.pslldq(xmm0,0);
  a.psrad(xmm0,xmm0);
  a.psrad(xmm0,ptr(zax));
  a.psrad(xmm0,0);
  a.psraw(xmm0,xmm0);
  a.psraw(xmm0,ptr(zax));
  a.psraw(xmm0,0);
  a.psubb(xmm0,xmm0);
  a.psubb(xmm0,ptr(zax));
  a.psubw(xmm0,xmm0);
  a.psubw(xmm0,ptr(zax));
  a.psubd(xmm0,xmm0);
  a.psubd(xmm0,ptr(zax));
  a.psubq(mm0,mm0);
  a.psubq(mm0,ptr(zax));
  a.psubq(xmm0,xmm0);
  a.psubq(xmm0,ptr(zax));
  a.pmaddwd(xmm0,xmm0);
  a.pmaddwd(xmm0,ptr(zax));
  a.pshufd(xmm0,xmm0,0);
  a.pshufd(xmm0,ptr(zax),0);
  a.pshufhw(xmm0,xmm0,0);
  a.pshufhw(xmm0,ptr(zax),0);
  a.pshuflw(xmm0,xmm0,0);
  a.pshuflw(xmm0,ptr(zax),0);
  a.psrld(xmm0,xmm0);
  a.psrld(xmm0,ptr(zax));
  a.psrld(xmm0,0);
  a.psrlq(xmm0,xmm0);
  a.psrlq(xmm0,ptr(zax));
  a.psrlq(xmm0,0);
  a.psrldq(xmm0,0);
  a.psrlw(xmm0,xmm0);
  a.psrlw(xmm0,ptr(zax));
  a.psrlw(xmm0,0);
  a.psubsb(xmm0,xmm0);
  a.psubsb(xmm0,ptr(zax));
  a.psubsw(xmm0,xmm0);
  a.psubsw(xmm0,ptr(zax));
  a.psubusb(xmm0,xmm0);
  a.psubusb(xmm0,ptr(zax));
  a.psubusw(xmm0,xmm0);
  a.psubusw(xmm0,ptr(zax));
  a.punpckhbw(xmm0,xmm0);
  a.punpckhbw(xmm0,ptr(zax));
  a.punpckhwd(xmm0,xmm0);
  a.punpckhwd(xmm0,ptr(zax));
  a.punpckhdq(xmm0,xmm0);
  a.punpckhdq(xmm0,ptr(zax));
  a.punpckhqdq(xmm0,xmm0);
  a.punpckhqdq(xmm0,ptr(zax));
  a.punpcklbw(xmm0,xmm0);
  a.punpcklbw(xmm0,ptr(zax));
  a.punpcklwd(xmm0,xmm0);
  a.punpcklwd(xmm0,ptr(zax));
  a.punpckldq(xmm0,xmm0);
  a.punpckldq(xmm0,ptr(zax));
  a.punpcklqdq(xmm0,xmm0);
  a.punpcklqdq(xmm0,ptr(zax));
  a.pxor(xmm0,xmm0);
  a.pxor(xmm0,ptr(zax));
  a.sqrtpd(xmm0,xmm0);
  a.sqrtpd(xmm0,ptr(zax));
  a.sqrtsd(xmm0,xmm0);
  a.sqrtsd(xmm0,ptr(zax));
  a.subpd(xmm0,xmm0);
  a.subpd(xmm0,ptr(zax));
  a.subsd(xmm0,xmm0);
  a.subsd(xmm0,ptr(zax));
  a.ucomisd(xmm0,xmm0);
  a.ucomisd(xmm0,ptr(zax));
  a.unpckhpd(xmm0,xmm0);
  a.unpckhpd(xmm0,ptr(zax));
  a.unpcklpd(xmm0,xmm0);
  a.unpcklpd(xmm0,ptr(zax));
  a.xorpd(xmm0,xmm0);
  a.xorpd(xmm0,ptr(zax));

  a.addsubpd(xmm0,xmm0);
  a.addsubpd(xmm0,ptr(zax));
  a.addsubps(xmm0,xmm0);
  a.addsubps(xmm0,ptr(zax));
  a.fisttp(dword_ptr(zax));
  a.haddpd(xmm0,xmm0);
  a.haddpd(xmm0,ptr(zax));
  a.haddps(xmm0,xmm0);
  a.haddps(xmm0,ptr(zax));
  a.hsubpd(xmm0,xmm0);
  a.hsubpd(xmm0,ptr(zax));
  a.hsubps(xmm0,xmm0);
  a.hsubps(xmm0,ptr(zax));
  a.lddqu(xmm0,ptr(zax));
  a.monitor();
  a.movddup(xmm0,xmm0);
  a.movddup(xmm0,ptr(zax));
  a.movshdup(xmm0,xmm0);
  a.movshdup(xmm0,ptr(zax));
  a.movsldup(xmm0,xmm0);
  a.movsldup(xmm0,ptr(zax));
  a.mwait();
  a.psignb(mm0,mm0);
  a.psignb(mm0,ptr(zax));
  a.psignb(xmm0,xmm0);
  a.psignb(xmm0,ptr(zax));
  a.psignw(mm0,mm0);
  a.psignw(mm0,ptr(zax));
  a.psignw(xmm0,xmm0);
  a.psignw(xmm0,ptr(zax));
  a.psignd(mm0,mm0);
  a.psignd(mm0,ptr(zax));
  a.psignd(xmm0,xmm0);
  a.psignd(xmm0,ptr(zax));
  a.phaddw(mm0,mm0);
  a.phaddw(mm0,ptr(zax));
  a.phaddw(xmm0,xmm0);
  a.phaddw(xmm0,ptr(zax));
  a.phaddd(mm0,mm0);
  a.phaddd(mm0,ptr(zax));
  a.phaddd(xmm0,xmm0);
  a.phaddd(xmm0,ptr(zax));
  a.phaddsw(mm0,mm0);
  a.phaddsw(mm0,ptr(zax));
  a.phaddsw(xmm0,xmm0);
  a.phaddsw(xmm0,ptr(zax));
  a.phsubw(mm0,mm0);
  a.phsubw(mm0,ptr(zax));
  a.phsubw(xmm0,xmm0);
  a.phsubw(xmm0,ptr(zax));
  a.phsubd(mm0,mm0);
  a.phsubd(mm0,ptr(zax));
  a.phsubd(xmm0,xmm0);
  a.phsubd(xmm0,ptr(zax));
  a.phsubsw(mm0,mm0);
  a.phsubsw(mm0,ptr(zax));
  a.phsubsw(xmm0,xmm0);
  a.phsubsw(xmm0,ptr(zax));
  a.pmaddubsw(mm0,mm0);
  a.pmaddubsw(mm0,ptr(zax));
  a.pmaddubsw(xmm0,xmm0);
  a.pmaddubsw(xmm0,ptr(zax));
  a.pabsb(mm0,mm0);
  a.pabsb(mm0,ptr(zax));
  a.pabsb(xmm0,xmm0);
  a.pabsb(xmm0,ptr(zax));
  a.pabsw(mm0,mm0);
  a.pabsw(mm0,ptr(zax));
  a.pabsw(xmm0,xmm0);
  a.pabsw(xmm0,ptr(zax));
  a.pabsd(mm0,mm0);
  a.pabsd(mm0,ptr(zax));
  a.pabsd(xmm0,xmm0);
  a.pabsd(xmm0,ptr(zax));
  a.pmulhrsw(mm0,mm0);
  a.pmulhrsw(mm0,ptr(zax));
  a.pmulhrsw(xmm0,xmm0);
  a.pmulhrsw(xmm0,ptr(zax));
  a.pshufb(mm0,mm0);
  a.pshufb(mm0,ptr(zax));
  a.pshufb(xmm0,xmm0);
  a.pshufb(xmm0,ptr(zax));
  a.palignr(mm0,mm0,0);
  a.palignr(mm0,ptr(zax),0);
  a.palignr(xmm0,xmm0,0);
  a.palignr(xmm0,ptr(zax),0);
  a.blendpd(xmm0,xmm0,0);
  a.blendpd(xmm0,ptr(zax),0);
  a.blendps(xmm0,xmm0,0);
  a.blendps(xmm0,ptr(zax),0);
  a.blendvpd(xmm0,xmm0);
  a.blendvpd(xmm0,ptr(zax));
  a.blendvps(xmm0,xmm0);
  a.blendvps(xmm0,ptr(zax));
  a.dppd(xmm0,xmm0,0);
  a.dppd(xmm0,ptr(zax),0);
  a.dpps(xmm0,xmm0,0);
  a.dpps(xmm0,ptr(zax),0);
  a.extractps(xmm0,xmm0,0);
  a.extractps(xmm0,ptr(zax),0);
  a.movntdqa(xmm0,ptr(zax));
  a.mpsadbw(xmm0,xmm0,0);
  a.mpsadbw(xmm0,ptr(zax),0);
  a.packusdw(xmm0,xmm0);
  a.packusdw(xmm0,ptr(zax));
  a.pblendvb(xmm0,xmm0);
  a.pblendvb(xmm0,ptr(zax));
  a.pblendw(xmm0,xmm0,0);
  a.pblendw(xmm0,ptr(zax),0);
  a.pcmpeqq(xmm0,xmm0);
  a.pcmpeqq(xmm0,ptr(zax));
  a.pextrb(zax,xmm0,0);
  a.pextrb(ptr(zax),xmm0,0);
  a.pextrd(zax,xmm0,0);
  a.pextrd(ptr(zax),xmm0,0);
  a.pextrq(zax,xmm0,0);
  a.pextrq(ptr(zax),xmm0,0);
  a.pextrw(zax,xmm0,0);
  a.pextrw(ptr(zax),xmm0,0);
  a.phminposuw(xmm0,xmm0);
  a.phminposuw(xmm0,ptr(zax));
  a.pinsrb(xmm0,eax,0);
  a.pinsrb(xmm0,ptr(zax),0);
  a.pinsrd(xmm0,eax,0);
  a.pinsrd(xmm0,ptr(zax),0);
  a.pinsrw(xmm0,eax,0);
  a.pinsrw(xmm0,ptr(zax),0);
  a.pmaxuw(xmm0,xmm0);
  a.pmaxuw(xmm0,ptr(zax));
  a.pmaxsb(xmm0,xmm0);
  a.pmaxsb(xmm0,ptr(zax));
  a.pmaxsd(xmm0,xmm0);
  a.pmaxsd(xmm0,ptr(zax));
  a.pmaxud(xmm0,xmm0);
  a.pmaxud(xmm0,ptr(zax));
  a.pminsb(xmm0,xmm0);
  a.pminsb(xmm0,ptr(zax));
  a.pminuw(xmm0,xmm0);
  a.pminuw(xmm0,ptr(zax));
  a.pminud(xmm0,xmm0);
  a.pminud(xmm0,ptr(zax));
  a.pminsd(xmm0,xmm0);
  a.pminsd(xmm0,ptr(zax));
  a.pmovsxbw(xmm0,xmm0);
  a.pmovsxbw(xmm0,ptr(zax));
  a.pmovsxbd(xmm0,xmm0);
  a.pmovsxbd(xmm0,ptr(zax));
  a.pmovsxbq(xmm0,xmm0);
  a.pmovsxbq(xmm0,ptr(zax));
  a.pmovsxwd(xmm0,xmm0);
  a.pmovsxwd(xmm0,ptr(zax));
  a.pmovsxwq(xmm0,xmm0);
  a.pmovsxwq(xmm0,ptr(zax));
  a.pmovsxdq(xmm0,xmm0);
  a.pmovsxdq(xmm0,ptr(zax));
  a.pmovzxbw(xmm0,xmm0);
  a.pmovzxbw(xmm0,ptr(zax));
  a.pmovzxbd(xmm0,xmm0);
  a.pmovzxbd(xmm0,ptr(zax));
  a.pmovzxbq(xmm0,xmm0);
  a.pmovzxbq(xmm0,ptr(zax));
  a.pmovzxwd(xmm0,xmm0);
  a.pmovzxwd(xmm0,ptr(zax));
  a.pmovzxwq(xmm0,xmm0);
  a.pmovzxwq(xmm0,ptr(zax));
  a.pmovzxdq(xmm0,xmm0);
  a.pmovzxdq(xmm0,ptr(zax));
  a.pmuldq(xmm0,xmm0);
  a.pmuldq(xmm0,ptr(zax));
  a.pmulld(xmm0,xmm0);
  a.pmulld(xmm0,ptr(zax));
  a.ptest(xmm0,xmm0);
  a.ptest(xmm0,ptr(zax));
  a.roundps(xmm0,xmm0,0);
  a.roundps(xmm0,ptr(zax),0);
  a.roundss(xmm0,xmm0,0);
  a.roundss(xmm0,ptr(zax),0);
  a.roundpd(xmm0,xmm0,0);
  a.roundpd(xmm0,ptr(zax),0);
  a.roundsd(xmm0,xmm0,0);
  a.roundsd(xmm0,ptr(zax),0);
  a.crc32(zax,ptr(zax));
  a.pcmpestri(xmm0,xmm0,0);
  a.pcmpestri(xmm0,ptr(zax),0);
  a.pcmpestrm(xmm0,xmm0,0);
  a.pcmpestrm(xmm0,ptr(zax),0);
  a.pcmpistri(xmm0,xmm0,0);
  a.pcmpistri(xmm0,ptr(zax),0);
  a.pcmpistrm(xmm0,xmm0,0);
  a.pcmpistrm(xmm0,ptr(zax),0);
  a.pcmpgtq(xmm0,xmm0);
  a.pcmpgtq(xmm0,ptr(zax));
  a.popcnt(zax,ptr(zax));

  a.amd_prefetch(ptr(zax));
  a.amd_prefetchw(ptr(zax));

  a.movbe(zax,ptr(zax));
  a.movbe(ptr(zax),zax);
  // ==========================================================================

  // ==========================================================================
  // Make the function.
  MyFn fn = asmjit_cast<MyFn>(a.make());

  // Call function (This is convenience for IDEs to go directly into disassembly).
  fn();

  // Free the generated function if it's not needed anymore.
  MemoryManager::getGlobal()->free((void*)fn);
  // ==========================================================================

  return 0;
}
