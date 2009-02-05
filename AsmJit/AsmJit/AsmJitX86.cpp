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

// The AsmJit compiler is also based on V8 JIT compiler:
//
// Copyright (c) 1994-2006 Sun Microsystems Inc.
// All Rights Reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// - Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// - Redistribution in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the
// distribution.
//
// - Neither the name of Sun Microsystems or the names of contributors may
// be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

// The original source code covered by the above license above has been modified
// significantly by Google Inc.
// Copyright 2006-2008 the V8 project authors. All rights reserved.

#include "AsmJitConfig.h"
#include "AsmJitVM.h"
#include "AsmJitX86.h"

namespace AsmJit {

// [Registers]

// 8 bit general purpose registers
const Register al = { REG_AL };
const Register cl = { REG_CL };
const Register dl = { REG_DL };
const Register bl = { REG_BL };
const Register ah = { REG_AH };
const Register ch = { REG_CH };
const Register dh = { REG_DH };
const Register bh = { REG_BH };

#if defined(ASMJIT_X64)
const Register r8b = { REG_R8B };
const Register r9b = { REG_R9B };
const Register r10b = { REG_R10B };
const Register r11b = { REG_R11B };
const Register r12b = { REG_R12B };
const Register r13b = { REG_R13B };
const Register r14b = { REG_R14B };
const Register r15b = { REG_R15B };
#endif // ASMJIT_X64

// 16 bit general purpose registers
const Register ax = { REG_AX };
const Register cx = { REG_CX };
const Register dx = { REG_DX };
const Register bx = { REG_BX };
const Register sp = { REG_SP };
const Register bp = { REG_BP };
const Register si = { REG_SI };
const Register di = { REG_DI };

// 32 bit general purpose registers
const Register eax = { REG_EAX };
const Register ecx = { REG_ECX };
const Register edx = { REG_EDX };
const Register ebx = { REG_EBX };
const Register esp = { REG_ESP };
const Register ebp = { REG_EBP };
const Register esi = { REG_ESI };
const Register edi = { REG_EDI };

#if defined(ASMJIT_X64)
const Register r8d = { REG_R8D };
const Register r9d = { REG_R9D };
const Register r10d = { REG_R10D };
const Register r11d = { REG_R11D };
const Register r12d = { REG_R12D };
const Register r13d = { REG_R13D };
const Register r14d = { REG_R14D };
const Register r15d = { REG_R15D };
#endif // ASMJIT_X64

// mmx registers
const MMRegister mm0 = { REG_MM0 };
const MMRegister mm1 = { REG_MM1 };
const MMRegister mm2 = { REG_MM2 };
const MMRegister mm3 = { REG_MM3 };
const MMRegister mm4 = { REG_MM4 };
const MMRegister mm5 = { REG_MM5 };
const MMRegister mm6 = { REG_MM6 };
const MMRegister mm7 = { REG_MM7 };

// sse registers
const XMMRegister xmm0 = { REG_XMM0 };
const XMMRegister xmm1 = { REG_XMM1 };
const XMMRegister xmm2 = { REG_XMM2 };
const XMMRegister xmm3 = { REG_XMM3 };
const XMMRegister xmm4 = { REG_XMM4 };
const XMMRegister xmm5 = { REG_XMM5 };
const XMMRegister xmm6 = { REG_XMM6 };
const XMMRegister xmm7 = { REG_XMM7 };

// 64 bit mode
#if defined(ASMJIT_X64)
const XMMRegister xmm8 = { REG_XMM8 };
const XMMRegister xmm9 = { REG_XMM9 };
const XMMRegister xmm10 = { REG_XMM10 };
const XMMRegister xmm11 = { REG_XMM11 };
const XMMRegister xmm12 = { REG_XMM12 };
const XMMRegister xmm13 = { REG_XMM13 };
const XMMRegister xmm14 = { REG_XMM14 };
const XMMRegister xmm15 = { REG_XMM15 };
#endif // ASMJIT_X64

// 64 bit mode
#if defined(ASMJIT_X64)
// 64 bit mode general purpose registers
const Register rax = { REG_RAX };
const Register rcx = { REG_RCX };
const Register rdx = { REG_RDX };
const Register rbx = { REG_RBX };
const Register rsp = { REG_RSP };
const Register rbp = { REG_RBP };
const Register rsi = { REG_RSI };
const Register rdi = { REG_RDI };
const Register r8  = { REG_R8  };
const Register r9  = { REG_R9  };
const Register r10 = { REG_R10 };
const Register r11 = { REG_R11 };
const Register r12 = { REG_R12 };
const Register r13 = { REG_R13 };
const Register r14 = { REG_R14 };
const Register r15 = { REG_R15 };
#endif // ASMJIT_X64

// native general purpose registers
// (shared between 32 bit mode and 64 bit mode)

const Register nax = { REG_NAX };
const Register ncx = { REG_NCX };
const Register ndx = { REG_NDX };
const Register nbx = { REG_NBX };
const Register nsp = { REG_NSP };
const Register nbp = { REG_NBP };
const Register nsi = { REG_NSI };
const Register ndi = { REG_NDI };

// [AsmJit::X86]

X86::X86() : 
  pData(NULL),
  pCur(NULL),
  pMax(NULL),
  _capacity(0)
{
}

X86::~X86()
{
  free();
}

bool X86::realloc(SysInt to)
{
  if (capacity() < to)
  {
    SysInt len = codeSize();

    UInt8 *newdata;
    if (pData)
      newdata = (UInt8*)ASMJIT_REALLOC(pData, to);
    else
      newdata = (UInt8*)ASMJIT_MALLOC(to);
    if (!newdata) return false;

    pData = newdata;
    pCur = newdata + len;
    pMax = pData + to;
    pMax -= (to >= 16) ? 16 : to;

    _capacity = to;
  }

  return true;
}

bool X86::grow()
{
  SysInt to = _capacity;
  SysInt pageSize = VM::pageSize();

  if (to < pageSize) 
    to = pageSize;
  else if (to > 65536)
    to += 65536;
  else
    to <<= 1;

  return realloc(to);
}

void X86::free()
{
  if (pData)
  {
    ASMJIT_FREE(pData);

    pData = NULL;
    pCur = NULL;
    pMax = NULL;

    _capacity = 0;
  }
}

UInt8* X86::takeCode()
{
  UInt8* result = pData;

  pData = NULL;
  pCur = NULL;
  pMax = NULL;

  _capacity = 0;

  return result;
}

} // AsmJit namespace
