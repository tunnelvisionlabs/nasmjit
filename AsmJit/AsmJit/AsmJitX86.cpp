// AsmJit - Complete JIT Assembler for C++ Language.

// Copyright (c) 2006-2009, Petr Kobalicek <kobalicek.petr@gmail.com>
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

Register eax = { REG_EAX };
Register ecx = { REG_ECX };
Register edx = { REG_EDX };
Register ebx = { REG_EBX };
Register esp = { REG_ESP };
Register ebp = { REG_EBP };
Register esi = { REG_ESI };
Register edi = { REG_EDI };

Register ax = { REG_AX };
Register cx = { REG_CX };
Register dx = { REG_DX };
Register bx = { REG_BX };
Register sp = { REG_SP };
Register bp = { REG_BP };
Register si = { REG_SI };
Register di = { REG_DI };

Register al = { REG_AL };
Register cl = { REG_CL };
Register dl = { REG_DL };
Register bl = { REG_BL };
Register ah = { REG_AH };
Register ch = { REG_CH };
Register dh = { REG_DH };
Register bh = { REG_BH };

MMRegister mm0 = { REG_MM0 };
MMRegister mm1 = { REG_MM1 };
MMRegister mm2 = { REG_MM2 };
MMRegister mm3 = { REG_MM3 };
MMRegister mm4 = { REG_MM4 };
MMRegister mm5 = { REG_MM5 };
MMRegister mm6 = { REG_MM6 };
MMRegister mm7 = { REG_MM7 };

XMMRegister xmm0 = { REG_XMM0 };
XMMRegister xmm1 = { REG_XMM1 };
XMMRegister xmm2 = { REG_XMM2 };
XMMRegister xmm3 = { REG_XMM3 };
XMMRegister xmm4 = { REG_XMM4 };
XMMRegister xmm5 = { REG_XMM5 };
XMMRegister xmm6 = { REG_XMM6 };
XMMRegister xmm7 = { REG_XMM7 };

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

bool X86::realloc(size_t to)
{
  if (capacity() < to)
  {
    size_t len = codeSize();

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
  size_t to = _capacity;
  size_t pageSize = VM::pageSize();

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
