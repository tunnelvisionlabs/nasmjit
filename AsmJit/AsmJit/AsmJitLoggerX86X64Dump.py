#!/usr/bin/env python

# This Python script will regenerate AsmJit::Logger::dump... methods. It 
# should be called only if instruction order or count of instructions 
# were modified. It will overwrite the AsmJitLoggerX86X64Dump.cpp file.

import os, re, string

fh = open("./AsmJitAssemblerX86X64.cpp", "rb")
din = fh.read()
fh.close()

r = re.compile(r"x86instructions\[\][\s]*=[\s]*{(?P<BLOCK>[^}])*}")
m = r.search(din)

if not m:
  print "Cannot match instruction data"
  exit(0)

din = din[m.start():m.end()]
dout = ""
dinst = []
daddr = []
dpos = 0

r = re.compile(r'\"(?P<INST>[A-Za-z0-9_]+)\"')
for m in r.finditer(din):
  dinst.append('\"' + m.group("INST") + '\\0\"');
  daddr.append(dpos)
  dpos += len(m.group("INST")) + 1

dout += \
"""// AsmJit - Complete JIT Assembler for C++ Language.

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

// ----------------------------------------------------------------------------
// WARNING:
// This file was generated by AsmJitLoggerX86X64Dump.py script, do not
// modify it, instead modify the script.
// ----------------------------------------------------------------------------

// We are using sprintf() here.
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

// [Dependencies]
#include "AsmJitLogger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

namespace AsmJit {

// [Constants]

"""

dout += "static const char instructionName[] =\n"
dout +=   "".join(["  " + str(a) + "\n" for a in dinst[0:len(dinst)-1]])
dout +=   "  " + str(dinst[-1]) + ";\n"

dout += "\n"

dout += "static const UInt16 instructionIndex[] =\n"
dout += "{\n"
dout +=   "".join(["  " + str(a) + ", \n" for a in daddr[0:len(daddr)-1]])
dout +=   "  " + str(daddr[-1]) + "\n"
dout += "};\n"

dout += \
"""
static const char* operandSize[] =
{
  NULL,
  "byte ptr ",
  "word ptr ",
  NULL,
  "dword ptr ",
  NULL,
  NULL,
  NULL,
  "qword ptr ",
  NULL,
  "tword ptr ",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  "dqword ptr "
};

static const char segmentName[] =
  "\\0\\0\\0\\0"
  "cs:\\0"
  "ss:\\0"
  "ds:\\0"
  "es:\\0"
  "fs:\\0"
  "gs:\\0";

// [String Functions]

static char* mycpy(char* dst, const char* src, SysUInt len = (SysUInt)-1)
{
  if (src == NULL) return dst;

  if (len == (SysUInt)-1)
  {
    while (*src) *dst++ = *src++;
  }
  else
  {
    memcpy(dst, src, len);
    dst += len;
  }
 
  return dst;
}

// not too effective, but this is debug logger:)
static char* myutoa(char* dst, SysUInt i, SysUInt base = 10)
{
  static const char letters[] = "0123456789ABCDEF";

  char buf[128];
  char* p = buf + 128;

  do {
    SysInt b = i % base;
    *--p = letters[(Int8)b];
    i /= base;
  } while (i);

  return mycpy(dst, p, (SysUInt)(buf + 128 - p));
}

static char* myitoa(char* dst, SysInt i, SysUInt base = 10)
{
  if (i < 0)
  {
    *dst++ = '-';
    i = -i;
  }

  return myutoa(dst, (SysUInt)i, base);
}

// [AsmJit::Logger]

char* Logger::dumpInstruction(char* buf, UInt32 code)
{
  ASMJIT_ASSERT(code < _INST_COUNT);
  return mycpy(buf, &instructionName[instructionIndex[code]]);
}

char* Logger::dumpOperand(char* buf, const Operand* op)
{
  if (op->isReg())
  {
    const BaseReg& reg = operand_cast<const BaseReg&>(*op);
    return dumpRegister(buf, reg.type(), reg.index());
  }
  else if (op->isMem())
  {
    bool isAbsolute = false;
    const Mem& mem = operand_cast<const Mem&>(*op);

    if (op->size() <= 16) 
    {
      buf = mycpy(buf, operandSize[op->size()]);
    }

    buf = mycpy(buf, &segmentName[mem.segmentPrefix() * 4]);

    *buf++ = '[';

    // [base + index*scale + displacement]
    if (mem.hasBase())
    {
      buf = dumpRegister(buf, REG_GPN, mem.base());
    }
    // [label + index*scale + displacement]
    else if (mem.label())
    {
      buf = dumpLabel(buf, mem.label());
    }
    // [absolute]
    else
    {
      isAbsolute = true;
      buf = myutoa(buf, (SysUInt)mem._mem.target, 16);
    }

    if (mem.hasIndex())
    {
      buf = mycpy(buf, " + ");
      buf = dumpRegister(buf, REG_GPN, mem.index());

      if (mem.shift())
      {
        buf = mycpy(buf, " * ");
        *buf++ = "1248"[mem.shift() & 3];
      }
    }

    if (mem.displacement() && !isAbsolute)
    {
      SysInt d = mem.displacement();
      *buf++ = (d < 0) ? '-' : '+';
      buf = myitoa(buf, d);
    }

    *buf++ = ']';
    return buf;
  }
  else if (op->isImm())
  {
    const Immediate& i = operand_cast<const Immediate&>(*op);
    return myitoa(buf, (SysInt)i.value());
  }
  else if (op->isLabel())
  {
    return dumpLabel(buf, (const Label*)op);
  }
  else
    return buf;
}

char* Logger::dumpRegister(char* buf, UInt8 type, UInt8 index)
{
  const char regs1[] = "al\\0" "cl\\0" "dl\\0" "bl\\0" "ah\\0" "ch\\0" "dh\\0" "bh\\0";
  const char regs2[] = "ax\\0" "cx\\0" "dx\\0" "bx\\0" "sp\\0" "bp\\0" "si\\0" "di\\0";
  
  switch (type)
  {
    case REG_GPB:
      if (index < 8)
        return buf + sprintf(buf, "%s", &regs1[index*3]);
      else
        return buf + sprintf(buf, "r%ub", (UInt32)index);
    case REG_GPW:
      if (index < 8)
        return buf + sprintf(buf, "%s", &regs2[index*3]);
      else
        return buf + sprintf(buf, "r%uw", (UInt32)index);
    case REG_GPD:
      if (index < 8)
        return buf + sprintf(buf, "e%s", &regs2[index*3]);
      else
        return buf + sprintf(buf, "r%ud", (UInt32)index);
    case REG_GPQ:
      if (index < 8)
        return buf + sprintf(buf, "r%s", &regs2[index*3]);
      else
        return buf + sprintf(buf, "r%u", (UInt32)index);
    case REG_X87:
      return buf + sprintf(buf, "st%u", (UInt32)index);
    case REG_MM:
      return buf + sprintf(buf, "mm%u", (UInt32)index);
    case REG_XMM:
      return buf + sprintf(buf, "xmm%u", (UInt32)index);
    default:
      return buf;
  }
}

char* Logger::dumpLabel(char* buf, const Label* label)
{
  char* beg = buf;
  *buf++ = 'L';
  
  if (label->labelId())
    buf = myutoa(buf, label->labelId());
  else
    *buf++ = 'x';

  return buf;
}

} // AsmJit namespace
"""

fh = open("./AsmJitLoggerX86X64Dump.cpp", "wb")
fh.truncate()
fh.write(dout)
fh.close()
