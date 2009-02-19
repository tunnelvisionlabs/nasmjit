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

// [Dependencies]
#include "AsmJitAssembler.h"
#include "AsmJitCompiler.h"
#include "AsmJitUtil.h"


#if defined(ASMJIT_X86)
# define NUM_REGS 8
#else
# define NUM_REGS 16
#endif // ASMJIT

namespace AsmJit {

// ----------------------------------------------------------------------------
// [Helpers]
// ----------------------------------------------------------------------------

static void delAll(EmittableList& buf)
{
  Emittable** emitters = buf.data();
  SysUInt i, len = buf.length();
  for (i = 0; i < len; i++) emitters[i]->~Emittable();
}

static void memset32(UInt32* p, UInt32 c, SysUInt len)
{
  SysUInt i;
  for (i = 0; i < len; i++) p[i] = c;
}

static bool isIntegerArgument(UInt32 arg)
{
  return 
    arg == ARGUMENT_INT     ||
    arg == ARGUMENT_UINT    ||
#if defined(ASMJIT_X64)
    arg == ARGUMENT_SYSINT  ||
    arg == ARGUMENT_SYSUINT ||
#endif
    arg == ARGUMENT_PTR ;
}

static bool isFloatArgument(UInt32 arg)
{
  return arg == ARGUMENT_FLOAT || arg == ARGUMENT_DOUBLE;
}

// ----------------------------------------------------------------------------
// [AsmJit::Emittable]
// ----------------------------------------------------------------------------

Emittable::Emittable(Compiler* c, UInt32 type) : _compiler(c), _type(type)
{
}

Emittable::~Emittable()
{
}

void Emittable::prepare()
{
  // ...nothing...
}

void Emittable::postEmit(Assembler& a)
{
  // ...nothing...
}

// ----------------------------------------------------------------------------
// [AsmJit::Align]
// ----------------------------------------------------------------------------

Align::Align(Compiler* c, SysInt size) : 
  Emittable(c, EMITTABLE_ALIGN), _size(size) 
{
}

Align::~Align()
{
}

void Align::emit(Assembler& a)
{
  a.align(size());
}

// ----------------------------------------------------------------------------
// [AsmJit::Instruction]
// ----------------------------------------------------------------------------

Instruction::Instruction(Compiler* c) :
  Emittable(c, EMITTABLE_INSTRUCTION)
{
}

Instruction::Instruction(Compiler* c, UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3) :
  Emittable(c, EMITTABLE_INSTRUCTION)
{
  _code = code;
  _o[0] = *o1;
  _o[1] = *o2;
  _o[2] = *o3;
}

Instruction::~Instruction()
{
}

void Instruction::emit(Assembler& a)
{
  a._emitX86(code(), &_o[0], &_o[1], &_o[2]);
}

// ----------------------------------------------------------------------------
// [AsmJit::Function]
// ----------------------------------------------------------------------------

Function::Function(Compiler* c) : 
  Emittable(c, EMITTABLE_FUNCTION),
  _alignmentSize(0),
  _variablesSize(0),
  _callingConvention(CALL_CONV_NONE),
  _calleePopsStack(false),
  _argumentsDirection(ARGUMENT_RIGHT_TO_LEFT),
  _argumentsStackSize(0),
  _usedGpnRegisters(0),
  _usedMmxRegisters(0),
  _usedSseRegisters(0),
  _changedGpnRegisters(0),
  _changedMmxRegisters(0),
  _changedSseRegisters(0),
  _preserveGpnRegisters(0),
  _preserveSseRegisters(0)
{
  memset32(_argumentsGpnRegisters, 0xFFFFFFFF, 16);
  memset32(_argumentsSseRegisters, 0xFFFFFFFF, 16);
}

Function::~Function()
{
}

bool Function::hasPrologEpilog()
{
  return false;
}

void Function::setCallingConvention(UInt32 cconv)
{
  // Safe defaults
  _callingConvention = cconv;
  _calleePopsStack = false;

  memset32(_argumentsGpnRegisters, 0xFFFFFFFF, 16);
  memset32(_argumentsSseRegisters, 0xFFFFFFFF, 16);

  _argumentsDirection = ARGUMENT_RIGHT_TO_LEFT;
  _argumentsStackSize = 0;

#if defined(ASMJIT_X86)
  // [X86 calling conventions]
  _preserveGpnRegisters =
    (1 << (REG_EBX & REGCODE_MASK)) |
    (1 << (REG_ESP & REGCODE_MASK)) |
    (1 << (REG_EBP & REGCODE_MASK)) |
    (1 << (REG_ESI & REGCODE_MASK)) |
    (1 << (REG_EDI & REGCODE_MASK)) ;
  _preserveSseRegisters = 0;

  switch (cconv)
  {
    case CALL_CONV_CDECL:
      break;
    case CALL_CONV_STDCALL:
      _calleePopsStack = true;
      break;
    case CALL_CONV_MSTHISCALL:
      _argumentsGpnRegisters[0] = (REG_ECX & REGCODE_MASK);
      _calleePopsStack = true;
      break;
    case CALL_CONV_MSFASTCALL:
      _argumentsGpnRegisters[0] = (REG_ECX & REGCODE_MASK);
      _argumentsGpnRegisters[1] = (REG_EDX & REGCODE_MASK);
      _calleePopsStack = true;
      break;
    case CALL_CONV_BORLANDFASTCALL:
      _argumentsGpnRegisters[0] = (REG_EAX & REGCODE_MASK);
      _argumentsGpnRegisters[1] = (REG_EDX & REGCODE_MASK);
      _argumentsGpnRegisters[2] = (REG_ECX & REGCODE_MASK);
      _argumentsDirection = ARGUMENT_LEFT_TO_RIGHT;
      _calleePopsStack = true;
      break;
    case CALL_CONV_GCCFASTCALL_2:
      _argumentsGpnRegisters[0] = (REG_ECX & REGCODE_MASK);
      _argumentsGpnRegisters[1] = (REG_EDX & REGCODE_MASK);
      _calleePopsStack = false;
      break;
    case CALL_CONV_GCCFASTCALL_3:
      _argumentsGpnRegisters[0] = (REG_EDX & REGCODE_MASK);
      _argumentsGpnRegisters[1] = (REG_ECX & REGCODE_MASK);
      _argumentsGpnRegisters[2] = (REG_EAX & REGCODE_MASK);
      _calleePopsStack = false;
      break;

    default:
      ASMJIT_CRASH();
  }
#else
  // [X64 calling conventions]
  switch(cconv)
  {
    case CALL_CONV_X64W:
      _preserveGpnRegisters =
        (1 << (REG_RBX   & REGCODE_MASK)) |
        (1 << (REG_RSP   & REGCODE_MASK)) |
        (1 << (REG_RBP   & REGCODE_MASK)) |
        (1 << (REG_RSI   & REGCODE_MASK)) |
        (1 << (REG_RDI   & REGCODE_MASK)) |
        (1 << (REG_R12   & REGCODE_MASK)) |
        (1 << (REG_R13   & REGCODE_MASK)) |
        (1 << (REG_R14   & REGCODE_MASK)) |
        (1 << (REG_R15   & REGCODE_MASK)) ;
      _preserveSseRegisters = 
        (1 << (REG_XMM6  & REGCODE_MASK)) |
        (1 << (REG_XMM7  & REGCODE_MASK)) |
        (1 << (REG_XMM8  & REGCODE_MASK)) |
        (1 << (REG_XMM9  & REGCODE_MASK)) |
        (1 << (REG_XMM10 & REGCODE_MASK)) |
        (1 << (REG_XMM11 & REGCODE_MASK)) |
        (1 << (REG_XMM12 & REGCODE_MASK)) |
        (1 << (REG_XMM13 & REGCODE_MASK)) |
        (1 << (REG_XMM14 & REGCODE_MASK)) |
        (1 << (REG_XMM15 & REGCODE_MASK)) ;

      _argumentsGpnRegisters[0] = (REG_RCX  & REGCODE_MASK);
      _argumentsGpnRegisters[1] = (REG_RDX  & REGCODE_MASK);
      _argumentsGpnRegisters[2] = (REG_R8   & REGCODE_MASK);
      _argumentsGpnRegisters[3] = (REG_R9   & REGCODE_MASK);
      _argumentsSseRegisters[0] = (REG_XMM0 & REGCODE_MASK);
      _argumentsSseRegisters[1] = (REG_XMM1 & REGCODE_MASK);
      _argumentsSseRegisters[2] = (REG_XMM2 & REGCODE_MASK);
      _argumentsSseRegisters[3] = (REG_XMM3 & REGCODE_MASK);
      break;

    case CALL_CONV_X64U:
      _preserveGpnRegisters =
        (1 << (REG_RBX   & REGCODE_MASK)) |
        (1 << (REG_RSP   & REGCODE_MASK)) |
        (1 << (REG_RBP   & REGCODE_MASK)) |
        (1 << (REG_R12   & REGCODE_MASK)) |
        (1 << (REG_R13   & REGCODE_MASK)) |
        (1 << (REG_R14   & REGCODE_MASK)) |
        (1 << (REG_R15   & REGCODE_MASK)) ;
      _preserveSseRegisters = 0;

      _argumentsGpnRegisters[0] = (REG_RDI  & REGCODE_MASK);
      _argumentsGpnRegisters[1] = (REG_RSI  & REGCODE_MASK);
      _argumentsGpnRegisters[2] = (REG_RDX  & REGCODE_MASK);
      _argumentsGpnRegisters[3] = (REG_RCX  & REGCODE_MASK);
      _argumentsGpnRegisters[4] = (REG_R8   & REGCODE_MASK);
      _argumentsGpnRegisters[5] = (REG_R9   & REGCODE_MASK);
      _argumentsSseRegisters[0] = (REG_XMM0 & REGCODE_MASK);
      _argumentsSseRegisters[1] = (REG_XMM1 & REGCODE_MASK);
      _argumentsSseRegisters[2] = (REG_XMM2 & REGCODE_MASK);
      _argumentsSseRegisters[3] = (REG_XMM3 & REGCODE_MASK);
      _argumentsSseRegisters[4] = (REG_XMM4 & REGCODE_MASK);
      _argumentsSseRegisters[5] = (REG_XMM5 & REGCODE_MASK);
      _argumentsSseRegisters[6] = (REG_XMM6 & REGCODE_MASK);
      _argumentsSseRegisters[7] = (REG_XMM7 & REGCODE_MASK);
      break;

    default:
      ASMJIT_CRASH();
  }
#endif
}

void Function::setArguments(const UInt32* _args, SysUInt len)
{
  ASMJIT_ASSERT(len <= 32);

  memset(_argumentsList, 0, sizeof(_argumentsList));

  SysInt i;

  SysInt gpnPos = 0;
  SysInt xmmPos = 0;
  SysInt stackOffset = 0;

  UInt32 args[32];
  memcpy(args, _args, len * sizeof(UInt32));

#if defined(ASMJIT_X86)
  // ==========================================================================
  // [X86 Calling Conventions]

  // Register arguments (Integer), always left-to-right
  for (i = 0; i != len; i++)
  {
    UInt32 a = args[i];
    if (isIntegerArgument(a) && gpnPos < 32 && _argumentsGpnRegisters[gpnPos] != 0xFFFFFFFF)
    {
      _argumentsList[i].type = a;
      _argumentsList[i].reg = _argumentsGpnRegisters[gpnPos++] | REG_GPN;
      _argumentsList[i].stackOffset = 0;
      args[i] = ARGUMENT_NONE;
    }
  }

  // Stack arguments
  bool ltr = _argumentsDirection == ARGUMENT_LEFT_TO_RIGHT;
  SysInt istart = ltr ? 0 : len-1;
  SysInt iend   = ltr ? len : -1;
  SysInt istep  = ltr ? 1 : -1;

  for (i = istart; i != iend; i += istep)
  {
    UInt32 a = args[i];
    if (isIntegerArgument(a))
    {
      stackOffset -= 4;

      _argumentsList[i].type = a;
      _argumentsList[i].reg = 0xFFFFFFFF;
      _argumentsList[i].stackOffset = stackOffset;

      args[i] = ARGUMENT_NONE;
    }
    else if (isFloatArgument(a))
    {
      if (a == ARGUMENT_FLOAT) 
        stackOffset -= 4;
      else
        stackOffset -= 8;

      _argumentsList[i].type = a;
      _argumentsList[i].reg = 0xFFFFFFFF;
      _argumentsList[i].stackOffset = stackOffset;

      args[i] = ARGUMENT_NONE;
    }
  }
  // ==========================================================================
#else
  // ==========================================================================
  // [X64 Calling Conventions]

  // Windows 64-bit specific
  if (callingConvention() == CALL_CONV_X64W)
  {
    SysInt max = len < 4 ? len : 4;

    // Register arguments (Integer / FP)
    for (i = 0; i != max; i++)
    {
      UInt32 a = args[i];
      if (isIntegerArgument(a))
      {
        _argumentsList[i].type = a;
        _argumentsList[i].reg = _argumentsGpnRegisters[i] | REG_GPN;
        _argumentsList[i].stackOffset = 0;
        args[i] = ARGUMENT_NONE;
      }
      else if (isFloatArgument(a))
      {
        _argumentsList[i].type = a;
        _argumentsList[i].reg = _argumentsSseRegisters[i] | REG_XMM;
        _argumentsList[i].stackOffset = 0;
        args[i] = ARGUMENT_NONE;
      }
    }

    // Stack arguments
    for (i = len-1; i != -1; i--)
    {
      UInt32 a = args[i];
      if (isIntegerArgument(a))
      {
        _argumentsList[i].type = a;
        _argumentsList[i].reg = 0xFFFFFFFF;
        _argumentsList[i].stackOffset = stackOffset;
        args[i] = ARGUMENT_NONE;
      }
    }
  }
  // All others
  else
  {
  }
  // ==========================================================================
#endif // ASMJIT_X86, ASMJIT_X64
}

void Function::emit(Assembler& a)
{
}

// ----------------------------------------------------------------------------
// [AsmJit::Prologue]
// ----------------------------------------------------------------------------

Prologue::Prologue(Compiler* c, Function* f) : 
  Emittable(c, EMITTABLE_PROLOGUE), 
  _function(f)
{
}

Prologue::~Prologue()
{
}

void Prologue::emit(Assembler& a)
{
  Function* f = function();
  ASMJIT_ASSERT(f);

  int i;

  if (f->hasPrologEpilog())
  {
    a.push(nbp);
    a.mov(nbp, nsp);
  }

  for (i = 0; i < NUM_REGS; i++)
  {
    if ((f->changedGpnRegisters () & (1U << i)) && 
        (f->preserveGpnRegisters() & (1U << i)) &&
        (i != (REG_NBP & REGCODE_MASK)) &&
        (i != (REG_NSP & REGCODE_MASK)) )
    {
      a.push(mk_gpn(i));
    }
  }
}

// ----------------------------------------------------------------------------
// [AsmJit::Epilogue]
// ----------------------------------------------------------------------------

Epilogue::Epilogue(Compiler* c, Function* f) : 
  Emittable(c, EMITTABLE_EPILOGUE),
  _function(f)
{
}

Epilogue::~Epilogue()
{
}

void Epilogue::emit(Assembler& a)
{
  Function* f = function();
  ASMJIT_ASSERT(f);

  int i;

  for (i = NUM_REGS-1; i >= 0; i--)
  {
    if ((f->changedGpnRegisters () & (1U << i)) && 
        (f->preserveGpnRegisters() & (1U << i)) &&
        (i != (REG_NBP & REGCODE_MASK)) &&
        (i != (REG_NSP & REGCODE_MASK)) )
    {
      a.pop(mk_gpn(i));
    }
  }

  if (f->hasPrologEpilog())
  {
    a.mov(nsp, nbp);
    a.pop(nbp);
  }

  if (f->calleePopsStack())
  {
    a.ret((Int16)f->argumentsStackSize());
  }
  else
  {
    a.ret();
  }
}

// ----------------------------------------------------------------------------
// [AsmJit::Variable]
// ----------------------------------------------------------------------------

Variable::Variable(Compiler* c, Function* f) :
  _compiler(c),
  _function(f)
{
}

Variable::~Variable()
{
}

// ----------------------------------------------------------------------------
// [AsmJit::Zone]
// ----------------------------------------------------------------------------

Zone::Zone(SysUInt chunkSize)
{
  _chunks = NULL;
  _total = 0;
  _chunkSize = chunkSize;
}

Zone::~Zone()
{
  freeAll();
}

void* Zone::alloc(SysUInt size)
{
  Chunk* cur = _chunks;

  if (!cur || cur->remain() < size)
  {
    cur = (Chunk*)ASMJIT_MALLOC(sizeof(Chunk) - (sizeof(UInt8)*4) + _chunkSize);
    if (!cur) return NULL;

    cur->prev = _chunks;
    cur->pos = 0;
    cur->size = _chunkSize;
    _chunks = cur;
  }

  UInt8* p = cur->data + cur->pos;
  cur->pos += size;
  return (void*)p;
}

void Zone::freeAll()
{
  Chunk* cur = _chunks;

  while (cur)
  {
    Chunk* prev = cur->prev;
    ASMJIT_FREE(cur);
    cur = prev;
  }

  _chunks = NULL;
  _total = 0;
}

// ----------------------------------------------------------------------------
// [AsmJit::Compiler - Construction / Destruction]
// ----------------------------------------------------------------------------

Compiler::Compiler() :
  _currentPosition(0),
  _currentFunction(NULL),
  _zone(65536 - sizeof(Zone::Chunk) - 32)
{
}

Compiler::~Compiler()
{
  delAll(_buffer);
}

// ----------------------------------------------------------------------------
// [AsmJit::Compiler - Internal Buffer]
// ----------------------------------------------------------------------------

void Compiler::clear()
{
  delAll(_buffer);
  _buffer.clear();
  _currentPosition = 0;
  _zone.freeAll();
}

void Compiler::free()
{
  delAll(_buffer);
  _buffer.free();
  _currentPosition = 0;
  _zone.freeAll();
}

// ----------------------------------------------------------------------------
// [AsmJit::Compiler - Function Builder]
// ----------------------------------------------------------------------------

Function* Compiler::beginFunction(UInt32 callingConvention)
{
  ASMJIT_ASSERT(_currentFunction == NULL);

  Function* f = _currentFunction = newObject<Function>();
  f->setCallingConvention(callingConvention);

  emit(f);
  return f;
}

Function* Compiler::endFunction()
{
  ASMJIT_ASSERT(_currentFunction != NULL);

  Function* f = _currentFunction;
  _currentFunction = NULL;
  return f;
}

Prologue* Compiler::prologue()
{
  Prologue* block = newObject<Prologue>(currentFunction());
  emit(block);
  return block;
}

Epilogue* Compiler::epilogue()
{
  Epilogue* block = newObject<Epilogue>(currentFunction());
  emit(block);
  return block;
}

// ----------------------------------------------------------------------------
// [AsmJit::Compiler - Emit]
// ----------------------------------------------------------------------------

void Compiler::emit(Emittable* emittable, bool endblock)
{
  if (endblock)
    _buffer.append(emittable);
  else
    _buffer.insert(_currentPosition++, emittable);
}

void Compiler::build(Assembler& a)
{
  Emittable** emitters = _buffer.data();
  SysUInt i, len;

  // Prepare (prepare action can append emittable)
  len = _buffer.length();
  for (i = 0; i < len; i++) emitters[i]->prepare();

  // Emit and postEmit
  len = _buffer.length();
  for (i = 0; i < len; i++) emitters[i]->emit(a);
  for (i = 0; i < len; i++) emitters[i]->postEmit(a);
}

// -------------------------------------------------------------------------
// [AsmJit::Compiler - EmitX86]
// -------------------------------------------------------------------------

void Compiler::_emitX86(UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3)
{
  if (o1->isLabel())
  {
    // TODO
  }
  else
  {
    emit(newObject<Instruction>(code, o1, o2, o3));
  }
}

// -------------------------------------------------------------------------
// [AsmJit::Compiler - Align]
// -------------------------------------------------------------------------

void Compiler::align(SysInt m)
{
  emit(newObject<Align>(m));
}

// -------------------------------------------------------------------------
// [AsmJit::Compiler - Bind]
// -------------------------------------------------------------------------

void Compiler::bind(Label* label)
{
}

} // AsmJit namespace
