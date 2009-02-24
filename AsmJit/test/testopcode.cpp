// AsmJit - Complete JIT Assembler for C++ Language.

// Copyright (c) 2008-2009, Petr Kobalicek <kobalicek.petr@gmail.com>
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

// This file is used to test opcodes generated by AsmJit. Output can be
// disassembled in your IDE or by your favourite disassembler. Instructions
// are sorted alphabetically.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AsmJit/AsmJitAssembler.h>
#include <AsmJit/AsmJitVM.h>

// This is type of function we will generate
typedef void (*MyFn)();

static void dump(const AsmJit::UInt8* data, AsmJit::SysInt size)
{
  AsmJit::SysInt i;
  for (i = 0; i < size; i++) 
  {
    printf("%0.2X ", data[i]);
    if (i % 16 == 0) printf("\n");
  }
}

int main(int argc, char* argv[])
{
  using namespace AsmJit;

  // ==========================================================================
  // STEP 1: Create function.
  Assembler a;

  // We don't want to crash :)
  a.ret();

  // Instructions
  a.adc(nax,nax);
  a.adc(nax,sysint_ptr(nax));
  a.adc(nax,0);
  a.adc(sysint_ptr(nax),nax);
  a.adc(sysint_ptr(nax),0);
  a.add(nax,nax);
  a.add(nax,sysint_ptr(nax));
  a.add(nax,0);
  a.add(sysint_ptr(nax),nax);
  a.add(sysint_ptr(nax),0);
  a.and_(nax,nax);
  a.and_(nax,sysint_ptr(nax));
  a.and_(nax,0);
  a.and_(sysint_ptr(nax),nax);
  a.and_(sysint_ptr(nax),0);
  a.bswap(nax);
  a.bt(nax,nax);
  a.bt(sysint_ptr(nax),nax);
  a.bt(nax,0);
  a.bt(sysint_ptr(nax),0);
  a.btc(nax,nax);
  a.btc(sysint_ptr(nax),nax);
  a.btc(nax,0);
  a.btc(sysint_ptr(nax),0);
  a.btr(nax,nax);
  a.btr(sysint_ptr(nax),nax);
  a.btr(nax,0);
  a.btr(sysint_ptr(nax),0);
  a.bts(nax,nax);
  a.bts(sysint_ptr(nax),nax);
  a.bts(nax,0);
  a.bts(sysint_ptr(nax),0);
  a.call(nax);
  a.call(sysint_ptr(nax));
  a.cbw();
  a.cwde();
  a.clc();
  a.cld();
  a.cmc();
  a.cmp(nax,nax);
  a.cmp(nax,sysint_ptr(nax));
  a.cmp(nax,0);
  a.cmp(sysint_ptr(nax),nax);
  a.cmp(sysint_ptr(nax),0);
  a.cmpxchg(nax,nax);
  a.cmpxchg(sysint_ptr(nax),nax);
  a.cmpxchg8b(ptr(nax));
  a.cpuid();
  a.dec(nax);
  a.dec(sysint_ptr(nax));
  a.div(nax);
  a.div(sysint_ptr(nax));
  a.idiv(nax);
  a.idiv(sysint_ptr(nax));
  a.imul(nax);
  a.imul(sysint_ptr(nax));
  a.imul(nax,nax);
  a.imul(nax,sysint_ptr(nax));
  a.imul(nax,0);
  a.imul(nax,nax,0);
  a.imul(nax,sysint_ptr(nax),0);
  a.inc(nax);
  a.inc(sysint_ptr(nax));
  a.int3();
  a.lea(nax,sysint_ptr(nax));
  a.lock();
  a.mov(nax,nax);
  a.mov(nax,sysint_ptr(nax));
  a.mov(nax,0);
  a.mov(sysint_ptr(nax),nax);
  a.mov(sysint_ptr(nax),0);
  a.movsx(nax,al);
  a.movsx(nax,byte_ptr(nax));
  a.movzx(nax,al);
  a.movzx(nax,byte_ptr(nax));
  a.mul(nax);
  a.mul(sysint_ptr(nax));
  a.neg(nax);
  a.neg(sysint_ptr(nax));
  a.nop();
  a.not_(nax);
  a.not_(sysint_ptr(nax));
  a.or_(nax,nax);
  a.or_(nax,sysint_ptr(nax));
  a.or_(nax,0);
  a.or_(sysint_ptr(nax),nax);
  a.or_(sysint_ptr(nax),0);
  a.pop(nax);
  a.pop(sysint_ptr(nax));
  a.push(nax);
  a.push(sysint_ptr(nax));
  a.push(0);
  a.rcl(nax,cl);
  a.rcl(nax,0);
  a.rcl(sysint_ptr(nax),cl);
  a.rcl(sysint_ptr(nax),0);
  a.rcr(nax,cl);
  a.rcr(nax,0);
  a.rcr(sysint_ptr(nax),cl);
  a.rcr(sysint_ptr(nax),0);
  a.rdtsc();
  a.rdtscp();
  a.ret();
  a.ret(0);
  a.rol(nax,cl);
  a.rol(nax,0);
  a.rol(sysint_ptr(nax),cl);
  a.rol(sysint_ptr(nax),0);
  a.ror(nax,cl);
  a.ror(nax,0);
  a.ror(sysint_ptr(nax),cl);
  a.ror(sysint_ptr(nax),0);
  a.sbb(nax,nax);
  a.sbb(nax,sysint_ptr(nax));
  a.sbb(nax,0);
  a.sbb(sysint_ptr(nax),nax);
  a.sbb(sysint_ptr(nax),0);
  a.sal(nax,cl);
  a.sal(nax,0);
  a.sal(sysint_ptr(nax),cl);
  a.sal(sysint_ptr(nax),0);
  a.sar(nax,cl);
  a.sar(nax,0);
  a.sar(sysint_ptr(nax),cl);
  a.sar(sysint_ptr(nax),0);
  a.shl(nax,cl);
  a.shl(nax,0);
  a.shl(sysint_ptr(nax),cl);
  a.shl(sysint_ptr(nax),0);
  a.shr(nax,cl);
  a.shr(nax,0);
  a.shr(sysint_ptr(nax),cl);
  a.shr(sysint_ptr(nax),0);
  a.shld(nax,nax,cl);
  a.shld(nax,nax,0);
  a.shld(sysint_ptr(nax),nax,cl);
  a.shld(sysint_ptr(nax),nax,0);
  a.shrd(nax,nax,cl);
  a.shrd(nax,nax,0);
  a.shrd(sysint_ptr(nax),nax,cl);
  a.shrd(sysint_ptr(nax),nax,0);
  a.stc();
  a.std();
  a.sub(nax,nax);
  a.sub(nax,sysint_ptr(nax));
  a.sub(nax,0);
  a.sub(sysint_ptr(nax),nax);
  a.sub(sysint_ptr(nax),0);
  a.test(nax,nax);
  a.test(nax,0);
  a.test(sysint_ptr(nax),nax);
  a.test(sysint_ptr(nax),0);
  a.ud2();
  a.xadd(nax,nax);
  a.xadd(sysint_ptr(nax),nax);
  a.xchg(nax,nax);
  a.xchg(sysint_ptr(nax),nax);
  a.xchg(nax,sysint_ptr(nax));
  a.xor_(nax,nax);
  a.xor_(nax,sysint_ptr(nax));
  a.xor_(nax,0);
  a.xor_(sysint_ptr(nax),nax);
  a.xor_(sysint_ptr(nax),0);

  a.f2xm1();
  a.fabs();
  a.fadd(dword_ptr(nax));
  a.fbld(dword_ptr(nax));
  a.fbstp(dword_ptr(nax));
  a.fchs();
  a.fclex();
  a.fcom(dword_ptr(nax));
  a.fcomp(dword_ptr(nax));
  a.fcompp();
  a.fcos();
  a.fdecstp();
  a.fdiv(dword_ptr(nax));
  a.fdivr(dword_ptr(nax));
  a.fiadd(dword_ptr(nax));
  a.ficom(dword_ptr(nax));
  a.ficomp(dword_ptr(nax));
  a.fidiv(dword_ptr(nax));
  a.fidivr(dword_ptr(nax));
  a.fild(dword_ptr(nax));
  a.fimul(dword_ptr(nax));
  a.fincstp();
  a.finit();
  a.fisub(dword_ptr(nax));
  a.fisubr(dword_ptr(nax));
  a.fninit();
  a.fist(dword_ptr(nax));
  a.fistp(dword_ptr(nax));
  a.fld(dword_ptr(nax));
  a.fld1();
  a.fldl2t();
  a.fldl2e();
  a.fldpi();
  a.fldlg2();
  a.fldln2();
  a.fldz();
  a.fldcw(ptr(nax));
  a.fldenv(ptr(nax));
  a.fmul(dword_ptr(nax));
  a.fnclex();
  a.fnop();
  a.fnsave(ptr(nax));
  a.fnstenv(ptr(nax));
  a.fnstcw(ptr(nax));
  a.fpatan();
  a.fprem();
  a.fprem1();
  a.fptan();
  a.frndint();
  a.frstor(ptr(nax));
  a.fsave(ptr(nax));
  a.fscale();
  a.fsin();
  a.fsincos();
  a.fsqrt();
  a.fst(dword_ptr(nax));
  a.fstp(dword_ptr(nax));
  a.fstcw(ptr(nax));
  a.fstenv(ptr(nax));
  a.fsub(dword_ptr(nax));
  a.fsubr(dword_ptr(nax));
  a.ftst();
  a.fucompp();
  a.fxam();
  a.fxrstor(ptr(nax));
  a.fxsave(ptr(nax));
  a.fxtract();
  a.fyl2x();
  a.fyl2xp1();
  a.emms();

  a.movd(ptr(nax),mm0);
  a.movd(eax,mm0);
  a.movd(mm0,ptr(nax));
  a.movd(mm0,eax);
  a.movq(mm0,mm0);
  a.movq(ptr(nax),mm0);
  a.movq(mm0,ptr(nax));
  a.packuswb(mm0,mm0);
  a.packuswb(mm0,ptr(nax));
  a.paddb(mm0,mm0);
  a.paddb(mm0,ptr(nax));
  a.paddw(mm0,mm0);
  a.paddw(mm0,ptr(nax));
  a.paddd(mm0,mm0);
  a.paddd(mm0,ptr(nax));
  a.paddsb(mm0,mm0);
  a.paddsb(mm0,ptr(nax));
  a.paddsw(mm0,mm0);
  a.paddsw(mm0,ptr(nax));
  a.paddusb(mm0,mm0);
  a.paddusb(mm0,ptr(nax));
  a.paddusw(mm0,mm0);
  a.paddusw(mm0,ptr(nax));
  a.pand(mm0,mm0);
  a.pand(mm0,ptr(nax));
  a.pandn(mm0,mm0);
  a.pandn(mm0,ptr(nax));
  a.pcmpeqb(mm0,mm0);
  a.pcmpeqb(mm0,ptr(nax));
  a.pcmpeqw(mm0,mm0);
  a.pcmpeqw(mm0,ptr(nax));
  a.pcmpeqd(mm0,mm0);
  a.pcmpeqd(mm0,ptr(nax));
  a.pcmpgtb(mm0,mm0);
  a.pcmpgtb(mm0,ptr(nax));
  a.pcmpgtw(mm0,mm0);
  a.pcmpgtw(mm0,ptr(nax));
  a.pcmpgtd(mm0,mm0);
  a.pcmpgtd(mm0,ptr(nax));
  a.pmulhw(mm0,mm0);
  a.pmulhw(mm0,ptr(nax));
  a.pmullw(mm0,mm0);
  a.pmullw(mm0,ptr(nax));
  a.por(mm0,mm0);
  a.por(mm0,ptr(nax));
  a.pmaddwd(mm0,mm0);
  a.pmaddwd(mm0,ptr(nax));
  a.pslld(mm0,mm0);
  a.pslld(mm0,ptr(nax));
  a.pslld(mm0,0);
  a.psllq(mm0,mm0);
  a.psllq(mm0,ptr(nax));
  a.psllq(mm0,0);
  a.psllw(mm0,mm0);
  a.psllw(mm0,ptr(nax));
  a.psllw(mm0,0);
  a.psrad(mm0,mm0);
  a.psrad(mm0,ptr(nax));
  a.psrad(mm0,0);
  a.psraw(mm0,mm0);
  a.psraw(mm0,ptr(nax));
  a.psraw(mm0,0);
  a.psrld(mm0,mm0);
  a.psrld(mm0,ptr(nax));
  a.psrld(mm0,0);
  a.psrlq(mm0,mm0);
  a.psrlq(mm0,ptr(nax));
  a.psrlq(mm0,0);
  a.psrlw(mm0,mm0);
  a.psrlw(mm0,ptr(nax));
  a.psrlw(mm0,0);
  a.psubb(mm0,mm0);
  a.psubb(mm0,ptr(nax));
  a.psubw(mm0,mm0);
  a.psubw(mm0,ptr(nax));
  a.psubd(mm0,mm0);
  a.psubd(mm0,ptr(nax));
  a.psubsb(mm0,mm0);
  a.psubsb(mm0,ptr(nax));
  a.psubsw(mm0,mm0);
  a.psubsw(mm0,ptr(nax));
  a.psubusb(mm0,mm0);
  a.psubusb(mm0,ptr(nax));
  a.psubusw(mm0,mm0);
  a.psubusw(mm0,ptr(nax));
  a.punpckhbw(mm0,mm0);
  a.punpckhbw(mm0,ptr(nax));
  a.punpckhwd(mm0,mm0);
  a.punpckhwd(mm0,ptr(nax));
  a.punpckhdq(mm0,mm0);
  a.punpckhdq(mm0,ptr(nax));
  a.punpcklbw(mm0,mm0);
  a.punpcklbw(mm0,ptr(nax));
  a.punpcklwd(mm0,mm0);
  a.punpcklwd(mm0,ptr(nax));
  a.punpckldq(mm0,mm0);
  a.punpckldq(mm0,ptr(nax));
  a.pxor(mm0,mm0);
  a.pxor(mm0,ptr(nax));

  a.addps(xmm0,xmm0);
  a.addps(xmm0,ptr(nax));
  a.addss(xmm0,xmm0);
  a.addss(xmm0,ptr(nax));
  a.andnps(xmm0,xmm0);
  a.andnps(xmm0,ptr(nax));
  a.andps(xmm0,xmm0);
  a.andps(xmm0,ptr(nax));
  a.cmpps(xmm0,xmm0,0);
  a.cmpps(xmm0,ptr(nax),0);
  a.cmpss(xmm0,xmm0,0);
  a.cmpss(xmm0,ptr(nax),0);
  a.comiss(xmm0,xmm0);
  a.comiss(xmm0,ptr(nax));
  a.cvtpi2ps(xmm0,mm0);
  a.cvtpi2ps(xmm0,ptr(nax));
  a.cvtps2pi(mm0,xmm0);
  a.cvtps2pi(mm0,ptr(nax));
  a.cvtsi2ss(xmm0,nax);
  a.cvtsi2ss(xmm0,ptr(nax));
  a.cvtss2si(nax,xmm0);
  a.cvtss2si(nax,ptr(nax));
  a.cvttps2pi(mm0,xmm0);
  a.cvttps2pi(mm0,ptr(nax));
  a.cvttss2si(nax,xmm0);
  a.cvttss2si(nax,ptr(nax));
  a.divps(xmm0,xmm0);
  a.divps(xmm0,ptr(nax));
  a.divss(xmm0,xmm0);
  a.divss(xmm0,ptr(nax));
  a.ldmxcsr(ptr(nax));
  a.maskmovq(mm0,mm0);
  a.maxps(xmm0,xmm0);
  a.maxps(xmm0,ptr(nax));
  a.maxss(xmm0,xmm0);
  a.maxss(xmm0,ptr(nax));
  a.minps(xmm0,xmm0);
  a.minps(xmm0,ptr(nax));
  a.minss(xmm0,xmm0);
  a.minss(xmm0,ptr(nax));
  a.movaps(xmm0,xmm0);
  a.movaps(xmm0,ptr(nax));
  a.movaps(ptr(nax),xmm0);
  a.movd(ptr(nax),xmm0);
  a.movd(eax,xmm0);
  a.movd(xmm0,ptr(nax));
  a.movd(xmm0,eax);
  a.movq(mm0, mm0);
  a.movq(xmm0,xmm0);
  a.movq(ptr(nax),xmm0);
  a.movq(xmm0,ptr(nax));
  a.movntq(ptr(nax),mm0);
  a.movhlps(xmm0,xmm0);
  a.movhps(xmm0,ptr(nax));
  a.movhps(ptr(nax),xmm0);
  a.movlhps(xmm0,xmm0);
  a.movlps(xmm0,ptr(nax));
  a.movlps(ptr(nax),xmm0);
  a.movntps(ptr(nax),xmm0);
  a.movss(xmm0,ptr(nax));
  a.movss(ptr(nax),xmm0);
  a.movups(xmm0,xmm0);
  a.movups(xmm0,ptr(nax));
  a.movups(ptr(nax),xmm0);
  a.mulps(xmm0,xmm0);
  a.mulps(xmm0,ptr(nax));
  a.mulss(xmm0,xmm0);
  a.mulss(xmm0,ptr(nax));
  a.orps(xmm0,xmm0);
  a.orps(xmm0,ptr(nax));
  a.pavgb(mm0,mm0);
  a.pavgb(mm0,ptr(nax));
  a.pavgw(mm0,mm0);
  a.pavgw(mm0,ptr(nax));
  a.pextrw(nax,mm0,0);
  a.pinsrw(mm0,mm0,0);
  a.pinsrw(mm0,ptr(nax),0);
  a.pmaxsw(mm0,mm0);
  a.pmaxsw(mm0,ptr(nax));
  a.pmaxub(mm0,mm0);
  a.pmaxub(mm0,ptr(nax));
  a.pminsw(mm0,mm0);
  a.pminsw(mm0,ptr(nax));
  a.pminub(mm0,mm0);
  a.pminub(mm0,ptr(nax));
  a.pmovmskb(nax,mm0);
  a.pmulhuw(mm0,mm0);
  a.pmulhuw(mm0,ptr(nax));
  a.psadbw(mm0,mm0);
  a.psadbw(mm0,ptr(nax));
  a.pshufw(mm0,mm0,0);
  a.pshufw(mm0,ptr(nax),0);
  a.rcpps(xmm0,xmm0);
  a.rcpps(xmm0,ptr(nax));
  a.rcpss(xmm0,xmm0);
  a.rcpss(xmm0,ptr(nax));
  a.prefetch(ptr(nax),0);
  a.psadbw(xmm0,xmm0);
  a.psadbw(xmm0,ptr(nax));
  a.rsqrtps(xmm0,xmm0);
  a.rsqrtps(xmm0,ptr(nax));
  a.rsqrtss(xmm0,xmm0);
  a.rsqrtss(xmm0,ptr(nax));
  a.sfence(ptr(nax));
  a.shufps(xmm0,xmm0,0);
  a.shufps(xmm0,ptr(nax),0);
  a.sqrtps(xmm0,xmm0);
  a.sqrtps(xmm0,ptr(nax));
  a.sqrtss(xmm0,xmm0);
  a.sqrtss(xmm0,ptr(nax));
  a.stmxcsr(ptr(nax));
  a.subps(xmm0,xmm0);
  a.subps(xmm0,ptr(nax));
  a.subss(xmm0,xmm0);
  a.subss(xmm0,ptr(nax));
  a.ucomiss(xmm0,xmm0);
  a.ucomiss(xmm0,ptr(nax));
  a.unpckhps(xmm0,xmm0);
  a.unpckhps(xmm0,ptr(nax));
  a.unpcklps(xmm0,xmm0);
  a.unpcklps(xmm0,ptr(nax));
  a.xorps(xmm0,xmm0);
  a.xorps(xmm0,ptr(nax));

  a.addpd(xmm0,xmm0);
  a.addpd(xmm0,ptr(nax));
  a.addsd(xmm0,xmm0);
  a.addsd(xmm0,ptr(nax));
  a.andnpd(xmm0,xmm0);
  a.andnpd(xmm0,ptr(nax));
  a.andpd(xmm0,xmm0);
  a.andpd(xmm0,ptr(nax));
  a.clflush(ptr(nax));
  a.cmppd(xmm0,xmm0,0);
  a.cmppd(xmm0,ptr(nax),0);
  a.cmpsd(xmm0,xmm0,0);
  a.cmpsd(xmm0,ptr(nax),0);
  a.comisd(xmm0,xmm0);
  a.comisd(xmm0,ptr(nax));
  a.cvtdq2pd(xmm0,xmm0);
  a.cvtdq2pd(xmm0,ptr(nax));
  a.cvtdq2ps(xmm0,xmm0);
  a.cvtdq2ps(xmm0,ptr(nax));
  a.cvtpd2dq(xmm0,xmm0);
  a.cvtpd2dq(xmm0,ptr(nax));
  a.cvtpd2pi(mm0,xmm0);
  a.cvtpd2pi(mm0,ptr(nax));
  a.cvtpd2ps(xmm0,xmm0);
  a.cvtpd2ps(xmm0,ptr(nax));
  a.cvtpi2pd(xmm0,mm0);
  a.cvtpi2pd(xmm0,ptr(nax));
  a.cvtps2dq(xmm0,xmm0);
  a.cvtps2dq(xmm0,ptr(nax));
  a.cvtps2pd(xmm0,xmm0);
  a.cvtps2pd(xmm0,ptr(nax));
  a.cvtsd2si(nax,xmm0);
  a.cvtsd2si(nax,ptr(nax));
  a.cvtsd2ss(xmm0,xmm0);
  a.cvtsd2ss(xmm0,ptr(nax));
  a.cvtsi2sd(xmm0,nax);
  a.cvtsi2sd(xmm0,ptr(nax));
  a.cvtss2sd(xmm0,xmm0);
  a.cvtss2sd(xmm0,ptr(nax));
  a.cvtss2si(nax,xmm0);
  a.cvtss2si(nax,ptr(nax));
  a.cvttpd2pi(mm0,xmm0);
  a.cvttpd2pi(mm0,ptr(nax));
  a.cvttpd2dq(xmm0,xmm0);
  a.cvttpd2dq(xmm0,ptr(nax));
  a.cvttps2dq(xmm0,xmm0);
  a.cvttps2dq(xmm0,ptr(nax));
  a.cvttsd2si(nax,xmm0);
  a.cvttsd2si(nax,ptr(nax));
  a.divpd(xmm0,xmm0);
  a.divpd(xmm0,ptr(nax));
  a.divsd(xmm0,xmm0);
  a.divsd(xmm0,ptr(nax));
  a.lfence();
  a.maskmovdqu(xmm0,xmm0);
  a.maxpd(xmm0,xmm0);
  a.maxpd(xmm0,ptr(nax));
  a.maxsd(xmm0,xmm0);
  a.maxsd(xmm0,ptr(nax));
  a.mfence();
  a.minpd(xmm0,xmm0);
  a.minpd(xmm0,ptr(nax));
  a.minsd(xmm0,xmm0);
  a.minsd(xmm0,ptr(nax));
  a.movdqa(xmm0,xmm0);
  a.movdqa(xmm0,ptr(nax));
  a.movdqa(ptr(nax),xmm0);
  a.movdqu(xmm0,xmm0);
  a.movdqu(xmm0,ptr(nax));
  a.movdqu(ptr(nax),xmm0);
  a.movmskps(nax,xmm0);
  a.movmskpd(nax,xmm0);
  a.movsd(xmm0,xmm0);
  a.movsd(xmm0,ptr(nax));
  a.movsd(ptr(nax),xmm0);
  a.movapd(xmm0,ptr(nax));
  a.movapd(ptr(nax),xmm0);
  a.movdq2q(mm0,xmm0);
  a.movq2dq(xmm0,mm0);
  a.movhpd(xmm0,ptr(nax));
  a.movhpd(ptr(nax),xmm0);
  a.movlpd(xmm0,ptr(nax));
  a.movlpd(ptr(nax),xmm0);
  a.movntdq(ptr(nax),xmm0);
  a.movnti(ptr(nax),nax);
  a.movntpd(ptr(nax),xmm0);
  a.movupd(xmm0,ptr(nax));
  a.movupd(ptr(nax),xmm0);
  a.mulpd(xmm0,xmm0);
  a.mulpd(xmm0,ptr(nax));
  a.mulsd(xmm0,xmm0);
  a.mulsd(xmm0,ptr(nax));
  a.orpd(xmm0,xmm0);
  a.orpd(xmm0,ptr(nax));
  a.packsswb(xmm0,xmm0);
  a.packsswb(xmm0,ptr(nax));
  a.packssdw(xmm0,xmm0);
  a.packssdw(xmm0,ptr(nax));
  a.packuswb(xmm0,xmm0);
  a.packuswb(xmm0,ptr(nax));
  a.paddb(xmm0,xmm0);
  a.paddb(xmm0,ptr(nax));
  a.paddw(xmm0,xmm0);
  a.paddw(xmm0,ptr(nax));
  a.paddd(xmm0,xmm0);
  a.paddd(xmm0,ptr(nax));
  a.paddq(mm0,mm0);
  a.paddq(mm0,ptr(nax));
  a.paddq(xmm0,xmm0);
  a.paddq(xmm0,ptr(nax));
  a.paddsb(xmm0,xmm0);
  a.paddsb(xmm0,ptr(nax));
  a.paddsw(xmm0,xmm0);
  a.paddsw(xmm0,ptr(nax));
  a.paddusb(xmm0,xmm0);
  a.paddusb(xmm0,ptr(nax));
  a.paddusw(xmm0,xmm0);
  a.paddusw(xmm0,ptr(nax));
  a.pand(xmm0,xmm0);
  a.pand(xmm0,ptr(nax));
  a.pandn(xmm0,xmm0);
  a.pandn(xmm0,ptr(nax));
  a.pause();
  a.pavgb(xmm0,xmm0);
  a.pavgb(xmm0,ptr(nax));
  a.pavgw(xmm0,xmm0);
  a.pavgw(xmm0,ptr(nax));
  a.pcmpeqb(xmm0,xmm0);
  a.pcmpeqb(xmm0,ptr(nax));
  a.pcmpeqw(xmm0,xmm0);
  a.pcmpeqw(xmm0,ptr(nax));
  a.pcmpeqd(xmm0,xmm0);
  a.pcmpeqd(xmm0,ptr(nax));
  a.pcmpgtb(xmm0,xmm0);
  a.pcmpgtb(xmm0,ptr(nax));
  a.pcmpgtw(xmm0,xmm0);
  a.pcmpgtw(xmm0,ptr(nax));
  a.pcmpgtd(xmm0,xmm0);
  a.pcmpgtd(xmm0,ptr(nax));
  a.pmaxsw(xmm0,xmm0);
  a.pmaxsw(xmm0,ptr(nax));
  a.pmaxub(xmm0,xmm0);
  a.pmaxub(xmm0,ptr(nax));
  a.pminsw(xmm0,xmm0);
  a.pminsw(xmm0,ptr(nax));
  a.pminub(xmm0,xmm0);
  a.pminub(xmm0,ptr(nax));
  a.pmovmskb(nax,xmm0);
  a.pmulhw(xmm0,xmm0);
  a.pmulhw(xmm0,ptr(nax));
  a.pmulhuw(xmm0,xmm0);
  a.pmulhuw(xmm0,ptr(nax));
  a.pmullw(xmm0,xmm0);
  a.pmullw(xmm0,ptr(nax));
  a.pmuludq(mm0,mm0);
  a.pmuludq(mm0,ptr(nax));
  a.pmuludq(xmm0,xmm0);
  a.pmuludq(xmm0,ptr(nax));
  a.por(xmm0,xmm0);
  a.por(xmm0,ptr(nax));
  a.pslld(xmm0,xmm0);
  a.pslld(xmm0,ptr(nax));
  a.pslld(xmm0,0);
  a.psllq(xmm0,xmm0);
  a.psllq(xmm0,ptr(nax));
  a.psllq(xmm0,0);
  a.psllw(xmm0,xmm0);
  a.psllw(xmm0,ptr(nax));
  a.psllw(xmm0,0);
  a.pslldq(xmm0,0);
  a.psrad(xmm0,xmm0);
  a.psrad(xmm0,ptr(nax));
  a.psrad(xmm0,0);
  a.psraw(xmm0,xmm0);
  a.psraw(xmm0,ptr(nax));
  a.psraw(xmm0,0);
  a.psubb(xmm0,xmm0);
  a.psubb(xmm0,ptr(nax));
  a.psubw(xmm0,xmm0);
  a.psubw(xmm0,ptr(nax));
  a.psubd(xmm0,xmm0);
  a.psubd(xmm0,ptr(nax));
  a.psubq(mm0,mm0);
  a.psubq(mm0,ptr(nax));
  a.psubq(xmm0,xmm0);
  a.psubq(xmm0,ptr(nax));
  a.pmaddwd(xmm0,xmm0);
  a.pmaddwd(xmm0,ptr(nax));
  a.pshufd(xmm0,xmm0,0);
  a.pshufd(xmm0,ptr(nax),0);
  a.pshuhw(xmm0,xmm0,0);
  a.pshuhw(xmm0,ptr(nax),0);
  a.pshulw(xmm0,xmm0,0);
  a.pshulw(xmm0,ptr(nax),0);
  a.psrld(xmm0,xmm0);
  a.psrld(xmm0,ptr(nax));
  a.psrld(xmm0,0);
  a.psrlq(xmm0,xmm0);
  a.psrlq(xmm0,ptr(nax));
  a.psrlq(xmm0,0);
  a.psrldq(xmm0,0);
  a.psrlw(xmm0,xmm0);
  a.psrlw(xmm0,ptr(nax));
  a.psrlw(xmm0,0);
  a.psubsb(xmm0,xmm0);
  a.psubsb(xmm0,ptr(nax));
  a.psubsw(xmm0,xmm0);
  a.psubsw(xmm0,ptr(nax));
  a.psubusb(xmm0,xmm0);
  a.psubusb(xmm0,ptr(nax));
  a.psubusw(xmm0,xmm0);
  a.psubusw(xmm0,ptr(nax));
  a.punpckhbw(xmm0,xmm0);
  a.punpckhbw(xmm0,ptr(nax));
  a.punpckhwd(xmm0,xmm0);
  a.punpckhwd(xmm0,ptr(nax));
  a.punpckhdq(xmm0,xmm0);
  a.punpckhdq(xmm0,ptr(nax));
  a.punpckhqdq(xmm0,xmm0);
  a.punpckhqdq(xmm0,ptr(nax));
  a.punpcklbw(xmm0,xmm0);
  a.punpcklbw(xmm0,ptr(nax));
  a.punpcklwd(xmm0,xmm0);
  a.punpcklwd(xmm0,ptr(nax));
  a.punpckldq(xmm0,xmm0);
  a.punpckldq(xmm0,ptr(nax));
  a.punpcklqdq(xmm0,xmm0);
  a.punpcklqdq(xmm0,ptr(nax));
  a.pxor(xmm0,xmm0);
  a.pxor(xmm0,ptr(nax));
  a.sqrtpd(xmm0,xmm0);
  a.sqrtpd(xmm0,ptr(nax));
  a.sqrtsd(xmm0,xmm0);
  a.sqrtsd(xmm0,ptr(nax));
  a.subpd(xmm0,xmm0);
  a.subpd(xmm0,ptr(nax));
  a.subsd(xmm0,xmm0);
  a.subsd(xmm0,ptr(nax));
  a.ucomisd(xmm0,xmm0);
  a.ucomisd(xmm0,ptr(nax));
  a.unpckhpd(xmm0,xmm0);
  a.unpckhpd(xmm0,ptr(nax));
  a.unpcklpd(xmm0,xmm0);
  a.unpcklpd(xmm0,ptr(nax));
  a.xorpd(xmm0,xmm0);
  a.xorpd(xmm0,ptr(nax));

  a.addsubpd(xmm0,xmm0);
  a.addsubpd(xmm0,ptr(nax));
  a.addsubps(xmm0,xmm0);
  a.addsubps(xmm0,ptr(nax));
  a.fisttp(dword_ptr(nax));
  a.haddpd(xmm0,xmm0);
  a.haddpd(xmm0,ptr(nax));
  a.haddps(xmm0,xmm0);
  a.haddps(xmm0,ptr(nax));
  a.hsubpd(xmm0,xmm0);
  a.hsubpd(xmm0,ptr(nax));
  a.hsubps(xmm0,xmm0);
  a.hsubps(xmm0,ptr(nax));
  a.lddqu(xmm0,ptr(nax));
  a.monitor();
  a.movddup(xmm0,xmm0);
  a.movddup(xmm0,ptr(nax));
  a.movshdup(xmm0,xmm0);
  a.movshdup(xmm0,ptr(nax));
  a.movsldup(xmm0,xmm0);
  a.movsldup(xmm0,ptr(nax));
  a.mwait();
  a.psignb(mm0,mm0);
  a.psignb(mm0,ptr(nax));
  a.psignb(xmm0,xmm0);
  a.psignb(xmm0,ptr(nax));
  a.psignw(mm0,mm0);
  a.psignw(mm0,ptr(nax));
  a.psignw(xmm0,xmm0);
  a.psignw(xmm0,ptr(nax));
  a.psignd(mm0,mm0);
  a.psignd(mm0,ptr(nax));
  a.psignd(xmm0,xmm0);
  a.psignd(xmm0,ptr(nax));
  a.phaddw(mm0,mm0);
  a.phaddw(mm0,ptr(nax));
  a.phaddw(xmm0,xmm0);
  a.phaddw(xmm0,ptr(nax));
  a.phaddd(mm0,mm0);
  a.phaddd(mm0,ptr(nax));
  a.phaddd(xmm0,xmm0);
  a.phaddd(xmm0,ptr(nax));
  a.phaddsw(mm0,mm0);
  a.phaddsw(mm0,ptr(nax));
  a.phaddsw(xmm0,xmm0);
  a.phaddsw(xmm0,ptr(nax));
  a.phsubw(mm0,mm0);
  a.phsubw(mm0,ptr(nax));
  a.phsubw(xmm0,xmm0);
  a.phsubw(xmm0,ptr(nax));
  a.phsubd(mm0,mm0);
  a.phsubd(mm0,ptr(nax));
  a.phsubd(xmm0,xmm0);
  a.phsubd(xmm0,ptr(nax));
  a.phsubsw(mm0,mm0);
  a.phsubsw(mm0,ptr(nax));
  a.phsubsw(xmm0,xmm0);
  a.phsubsw(xmm0,ptr(nax));
  a.pmaddubsw(mm0,mm0);
  a.pmaddubsw(mm0,ptr(nax));
  a.pmaddubsw(xmm0,xmm0);
  a.pmaddubsw(xmm0,ptr(nax));
  a.pabsb(mm0,mm0);
  a.pabsb(mm0,ptr(nax));
  a.pabsb(xmm0,xmm0);
  a.pabsb(xmm0,ptr(nax));
  a.pabsw(mm0,mm0);
  a.pabsw(mm0,ptr(nax));
  a.pabsw(xmm0,xmm0);
  a.pabsw(xmm0,ptr(nax));
  a.pabsd(mm0,mm0);
  a.pabsd(mm0,ptr(nax));
  a.pabsd(xmm0,xmm0);
  a.pabsd(xmm0,ptr(nax));
  a.pmulhrsw(mm0,mm0);
  a.pmulhrsw(mm0,ptr(nax));
  a.pmulhrsw(xmm0,xmm0);
  a.pmulhrsw(xmm0,ptr(nax));
  a.pshufb(mm0,mm0);
  a.pshufb(mm0,ptr(nax));
  a.pshufb(xmm0,xmm0);
  a.pshufb(xmm0,ptr(nax));
  a.palignr(mm0,mm0,0);
  a.palignr(mm0,ptr(nax),0);
  a.palignr(xmm0,xmm0,0);
  a.palignr(xmm0,ptr(nax),0);
  a.blendpd(xmm0,xmm0,0);
  a.blendpd(xmm0,ptr(nax),0);
  a.blendps(xmm0,xmm0,0);
  a.blendps(xmm0,ptr(nax),0);
  a.blendvpd(xmm0,xmm0);
  a.blendvpd(xmm0,ptr(nax));
  a.blendvps(xmm0,xmm0);
  a.blendvps(xmm0,ptr(nax));
  a.dppd(xmm0,xmm0,0);
  a.dppd(xmm0,ptr(nax),0);
  a.dpps(xmm0,xmm0,0);
  a.dpps(xmm0,ptr(nax),0);
  a.extractps(xmm0,xmm0,0);
  a.extractps(xmm0,ptr(nax),0);
  a.movntdqa(xmm0,ptr(nax));
  a.mpsadbw(xmm0,xmm0,0);
  a.mpsadbw(xmm0,ptr(nax),0);
  a.packusdw(xmm0,xmm0);
  a.packusdw(xmm0,ptr(nax));
  a.pblendvb(xmm0,xmm0);
  a.pblendvb(xmm0,ptr(nax));
  a.pblendw(xmm0,xmm0,0);
  a.pblendw(xmm0,ptr(nax),0);
  a.pcmpeqq(xmm0,xmm0);
  a.pcmpeqq(xmm0,ptr(nax));
  a.pextrb(nax,xmm0,0);
  a.pextrb(ptr(nax),xmm0,0);
  a.pextrd(nax,xmm0,0);
  a.pextrd(ptr(nax),xmm0,0);
  a.pextrq(nax,xmm0,0);
  a.pextrq(ptr(nax),xmm0,0);
  a.pextrw(nax,xmm0,0);
  a.pextrw(ptr(nax),xmm0,0);
  a.phminposuw(xmm0,xmm0);
  a.phminposuw(xmm0,ptr(nax));
  a.pinsrb(xmm0,xmm0,0);
  a.pinsrb(xmm0,ptr(nax),0);
  a.pinsrd(xmm0,xmm0,0);
  a.pinsrd(xmm0,ptr(nax),0);
  a.pinsrw(xmm0,xmm0,0);
  a.pinsrw(xmm0,ptr(nax),0);
  a.pmaxuw(xmm0,xmm0);
  a.pmaxuw(xmm0,ptr(nax));
  a.pmaxsb(xmm0,xmm0);
  a.pmaxsb(xmm0,ptr(nax));
  a.pmaxsd(xmm0,xmm0);
  a.pmaxsd(xmm0,ptr(nax));
  a.pmaxud(xmm0,xmm0);
  a.pmaxud(xmm0,ptr(nax));
  a.pminsb(xmm0,xmm0);
  a.pminsb(xmm0,ptr(nax));
  a.pminuw(xmm0,xmm0);
  a.pminuw(xmm0,ptr(nax));
  a.pminud(xmm0,xmm0);
  a.pminud(xmm0,ptr(nax));
  a.pminsd(xmm0,xmm0);
  a.pminsd(xmm0,ptr(nax));
  a.pmovsxbw(xmm0,xmm0);
  a.pmovsxbw(xmm0,ptr(nax));
  a.pmovsxbd(xmm0,xmm0);
  a.pmovsxbd(xmm0,ptr(nax));
  a.pmovsxbq(xmm0,xmm0);
  a.pmovsxbq(xmm0,ptr(nax));
  a.pmovsxwd(xmm0,xmm0);
  a.pmovsxwd(xmm0,ptr(nax));
  a.pmovsxwq(xmm0,xmm0);
  a.pmovsxwq(xmm0,ptr(nax));
  a.pmovsxdq(xmm0,xmm0);
  a.pmovsxdq(xmm0,ptr(nax));
  a.pmovzxbw(xmm0,xmm0);
  a.pmovzxbw(xmm0,ptr(nax));
  a.pmovzxbd(xmm0,xmm0);
  a.pmovzxbd(xmm0,ptr(nax));
  a.pmovzxbq(xmm0,xmm0);
  a.pmovzxbq(xmm0,ptr(nax));
  a.pmovzxwd(xmm0,xmm0);
  a.pmovzxwd(xmm0,ptr(nax));
  a.pmovzxwq(xmm0,xmm0);
  a.pmovzxwq(xmm0,ptr(nax));
  a.pmovzxdq(xmm0,xmm0);
  a.pmovzxdq(xmm0,ptr(nax));
  a.pmuldq(xmm0,xmm0);
  a.pmuldq(xmm0,ptr(nax));
  a.pmulld(xmm0,xmm0);
  a.pmulld(xmm0,ptr(nax));
  a.ptest(xmm0,xmm0);
  a.ptest(xmm0,ptr(nax));
  a.roundps(xmm0,xmm0,0);
  a.roundps(xmm0,ptr(nax),0);
  a.roundss(xmm0,xmm0,0);
  a.roundss(xmm0,ptr(nax),0);
  a.roundpd(xmm0,xmm0,0);
  a.roundpd(xmm0,ptr(nax),0);
  a.roundsd(xmm0,xmm0,0);
  a.roundsd(xmm0,ptr(nax),0);
  a.crc32(nax,ptr(nax));
  a.pcmpestri(xmm0,xmm0,0);
  a.pcmpestri(xmm0,ptr(nax),0);
  a.pcmpestrm(xmm0,xmm0,0);
  a.pcmpestrm(xmm0,ptr(nax),0);
  a.pcmpistri(xmm0,xmm0,0);
  a.pcmpistri(xmm0,ptr(nax),0);
  a.pcmpistrm(xmm0,xmm0,0);
  a.pcmpistrm(xmm0,ptr(nax),0);
  a.pcmpgtq(xmm0,xmm0);
  a.pcmpgtq(xmm0,ptr(nax));
  a.popcnt(nax,ptr(nax));

  a.amd_prefetch(ptr(nax));
  a.amd_prefetchw(ptr(nax));

  a.movbe(nax,ptr(nax));
  a.movbe(ptr(nax),nax);
  // ==========================================================================

  // ==========================================================================
  // STEP 2: Alloc execution-enabled memory
  SysUInt vsize;
  void *vmem = VM::alloc(a.codeSize(), &vsize, true);
  if (!vmem) 
  {
    printf("AsmJit::VM::alloc() - Failed to allocate execution-enabled memory.\n");
    return 1;
  }

  // Dump instruction stream
  dump(a.code(), a.codeSize());

  // Relocate generated code to vmem.
  a.relocCode(vmem);

  // Cast vmem to our function and call the code.
  // (This is convenience for IDEs to go directly to instruction stream)
  function_cast<MyFn>(vmem)();

  // Memory should be freed, but use VM::free() to do that.
  VM::free(vmem, vsize);
  // ==========================================================================

  return 0;
}
