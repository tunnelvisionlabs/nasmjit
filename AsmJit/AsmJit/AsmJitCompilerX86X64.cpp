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

// [Count of registers is different in 32 bit or 64 bit mode]
#if defined(ASMJIT_X86)
# define NUM_REGS 8
#else
# define NUM_REGS 16
#endif // ASMJIT

// VARIABLE_TYPE_INT64 not declared in 32 bit mode, in future this should 
// change
#if defined(ASMJIT_X86)
#define VARIABLE_TYPE_INT64 2
#endif // ASMJIT_X86

namespace AsmJit {

// ============================================================================
// [Helpers]
// ============================================================================

static void delAll(Compiler::EmittableList& buf)
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
    arg == VARIABLE_TYPE_INT32   ||
    arg == VARIABLE_TYPE_UINT32  ||
#if defined(ASMJIT_X64)
    arg == VARIABLE_TYPE_SYSINT  ||
    arg == VARIABLE_TYPE_SYSUINT ||
#endif
    arg == VARIABLE_TYPE_PTR ;
}

static bool isFloatArgument(UInt32 arg)
{
  return arg == VARIABLE_TYPE_FLOAT || arg == VARIABLE_TYPE_DOUBLE;
}

static const UInt32 variableSizeData[] = {
  0,               // VARIABLE_TYPE_NONE
  4,               // VARIABLE_TYPE_INT32, VARIABLE_TYPE_UINT32
  8,               // VARIABLE_TYPE_INT64, VARIABLE_TYPE_UINT64
  4,               // VARIABLE_TYPE_FLOAT
  8,               // VARIABLE_TYPE_DOUBLE
  8,               // VARIABLE_TYPE_MM
  16               // VARIABLE_TYPE_XMM
};

#define ASMJIT_ARRAY_SIZE(A) (sizeof(A) / sizeof(*A))

static UInt32 getVariableSize(UInt32 type)
{
  ASMJIT_ASSERT(type < ASMJIT_ARRAY_SIZE(variableSizeData));
  return variableSizeData[type];
}

// ============================================================================
// [AsmJit::Variable]
// ============================================================================

Variable::Variable(Compiler* c, Function* f, UInt8 type) :
  _compiler(c),
  _function(f),
  _refCount(0),
  _spillCount(0),
  _reuseCount(0),
  _type(type),
  _size(getVariableSize(type)),
  _state(VARIABLE_STATE_UNUSED),
  _priority(10),
  _registerCode(NO_REG),
  _preferredRegister(0xFF),
  _stackOffset(0)
{
  ASMJIT_ASSERT(f != NULL);

  _memoryOperand = new(c->_allocObject(sizeof(Mem))) Mem(ebp, 0, _size);
  c->_registerOperand(_memoryOperand);
}

Variable::~Variable()
{
}

Variable* Variable::ref()
{
  _refCount++;
  return this;
}

void Variable::deref()
{
  _refCount--;
  if (_refCount == 0) unuse();
}

// ============================================================================
// [AsmJit::Emittable]
// ============================================================================

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

// ============================================================================
// [AsmJit::Align]
// ============================================================================

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

// ============================================================================
// [AsmJit::Instruction]
// ============================================================================

Instruction::Instruction(Compiler* c) :
  Emittable(c, EMITTABLE_INSTRUCTION)
{
  _o[0] = &_ocache[0];
  _o[1] = &_ocache[1];
  _o[2] = &_ocache[2];
  memset(_ocache, 0, sizeof(_ocache));
}

Instruction::Instruction(Compiler* c, UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3) :
  Emittable(c, EMITTABLE_INSTRUCTION)
{
  _code = code;

  UInt32 oid;
  if ((oid = o1->operandId()) != 0) { ASMJIT_ASSERT(oid < c->_operands.length()); _o[0] = c->_operands[oid]; } else { _o[0] = &_ocache[0]; _ocache[0] = *o1; }
  if ((oid = o2->operandId()) != 0) { ASMJIT_ASSERT(oid < c->_operands.length()); _o[1] = c->_operands[oid]; } else { _o[1] = &_ocache[1]; _ocache[1] = *o2; }
  if ((oid = o3->operandId()) != 0) { ASMJIT_ASSERT(oid < c->_operands.length()); _o[2] = c->_operands[oid]; } else { _o[2] = &_ocache[2]; _ocache[2] = *o3; }
}

Instruction::~Instruction()
{
}

void Instruction::emit(Assembler& a)
{
  a._emitX86(code(), _o[0], _o[1], _o[2]);
}

// ============================================================================
// [AsmJit::Function]
// ============================================================================

Function::Function(Compiler* c) : 
  Emittable(c, EMITTABLE_FUNCTION),
  _maxAlignmentStackSize(0),
  _variablesStackSize(0),
  _cconv(CALL_CONV_NONE),
  _naked(false),
  _calleePopsStack(false),
  _cconvArgumentsDirection(ARGUMENT_DIR_RIGHT_TO_LEFT),
  _argumentsStackSize(0),
  _usedGpRegisters(0),
  _usedMmRegisters(0),
  _usedXmmRegisters(0),
  _modifiedGpRegisters(0),
  _modifiedMmRegisters(0),
  _modifiedXmmRegisters(0),
  _cconvPreservedGp(0),
  _cconvPreservedXmm(0),
  _entryLabel(c->newLabel()),
  _prologLabel(c->newLabel()),
  _exitLabel(c->newLabel())
{
  memset32(_cconvArgumentsGp, 0xFFFFFFFF, 16);
  memset32(_cconvArgumentsXmm, 0xFFFFFFFF, 16);
}

Function::~Function()
{
}

void Function::prepare()
{
  // Prepare variables
  static const UInt32 sizes[] = { 16, 8, 4, 2, 1 };

  SysUInt i, v;

  SysInt sp = 0;       // Stack offset
  SysInt pe = 0;       // Prolog / epilog size

  SysInt disp = 0;     // Displacement (based on prolog / epilog size)
  UInt8 membase;       // Address base register for variables

  UInt32 maxAlign = 0; // Maximum alignment stack size

  // This is simple optimization to do 16 byte aligned variables first and
  // all others next.
  for (i = 0; i < ASMJIT_ARRAY_SIZE(sizes); i++)
  {
    UInt32 size = sizes[i];

    for (v = 0; v < _variables.length(); v++)
    {
      // Use only variable with size 'size' and variable not mapped to the 
      // function arguments
      if (_variables[v]->size() == size && _variables[v]->_stackOffset <= 0)
      {
        // X86 stack is aligned to 32 bits (4 bytes). For MMX and SSE 
        // programming we need 8 or 16 bytes alignment. For MMX memory
        // operands we can use 4 bytes and for SSE 12 bytes.
#if defined(ASMJIT_X86)
        if (size == 8  && maxAlign < 4) maxAlign = 4;
        if (size == 16 && maxAlign < 12) maxAlign = 12;
#endif // ASMJIT_X86

        sp -= size;
        _variables[v]->_stackOffset = sp;
      }
    }
  }

  // Get prolog/epilog push/pop size on the stack
  for (i = 0; i < NUM_REGS; i++)
  {
    if ((modifiedGpRegisters () & (1U << i)) && 
        (cconvPreservedGp() & (1U << i)) &&
        (i != (REG_NBP & REGCODE_MASK)) &&
        (i != (REG_NSP & REGCODE_MASK)) )
    {
      pe += sizeof(SysInt);
    }
  }

  if (sp) pe += sizeof(SysInt); // push/pop ebp/rpb

  _prologEpilogStackSize = pe;
  _variablesStackSize = -sp;
  _maxAlignmentStackSize = maxAlign;

  // Calculate displacement
  if (!naked() || maxAlign)
  {
    // Functions with prolog/epilog are using ebp/rbp
    membase = RID_EBP | 0x10;
    disp = -pe + (Int32)sizeof(SysInt);
  }
  else
  {
    // Naked functions are using esp/rsp
    membase = RID_ESP | 0x10;
    disp = -pe + (Int32)sizeof(SysInt) /* first push */;
  }

  // Patch all variables to point to correct address in memory
  for (v = 0; v < _variables.length(); v++)
  {
    Variable* var = _variables[v];
    Mem* memop = var->_memoryOperand;

    if (var->stackOffset() > 0)
    {
      memop->_mem.base = RID_ESP | 0x10;
      memop->_mem.displacement = var->stackOffset() + sizeof(SysInt);
    }
    else
    {
      memop->_mem.base = membase;
      memop->_mem.displacement = var->stackOffset() + disp;
    }
  }
}

void Function::emit(Assembler& a)
{
  a.bind(_entryLabel);
}

void Function::setPrototype(UInt32 cconv, const UInt32* args, SysUInt count)
{
  _setCallingConvention(cconv);
  _setArguments(args, count);
}

void Function::setNaked(UInt32 naked)
{
  if (_naked == naked) return;

  _naked = naked;
}

void Function::_setCallingConvention(UInt32 cconv)
{
  // Safe defaults
  _cconv = cconv;
  _calleePopsStack = false;

  memset32(_cconvArgumentsGp, 0xFFFFFFFF, 16);
  memset32(_cconvArgumentsXmm, 0xFFFFFFFF, 16);

  _cconvArgumentsDirection = ARGUMENT_DIR_RIGHT_TO_LEFT;
  _argumentsStackSize = 0;

#if defined(ASMJIT_X86)
  // [X86 calling conventions]
  _cconvPreservedGp =
    (1 << (REG_EBX & REGCODE_MASK)) |
    (1 << (REG_ESP & REGCODE_MASK)) |
    (1 << (REG_EBP & REGCODE_MASK)) |
    (1 << (REG_ESI & REGCODE_MASK)) |
    (1 << (REG_EDI & REGCODE_MASK)) ;
  _cconvPreservedXmm = 0;

  switch (cconv)
  {
    case CALL_CONV_CDECL:
      break;
    case CALL_CONV_STDCALL:
      _calleePopsStack = true;
      break;
    case CALL_CONV_MSTHISCALL:
      _cconvArgumentsGp[0] = (REG_ECX & REGCODE_MASK);
      _calleePopsStack = true;
      break;
    case CALL_CONV_MSFASTCALL:
      _cconvArgumentsGp[0] = (REG_ECX & REGCODE_MASK);
      _cconvArgumentsGp[1] = (REG_EDX & REGCODE_MASK);
      _calleePopsStack = true;
      break;
    case CALL_CONV_BORLANDFASTCALL:
      _cconvArgumentsGp[0] = (REG_EAX & REGCODE_MASK);
      _cconvArgumentsGp[1] = (REG_EDX & REGCODE_MASK);
      _cconvArgumentsGp[2] = (REG_ECX & REGCODE_MASK);
      _cconvArgumentsDirection = ARGUMENT_DIR_LEFT_TO_RIGHT;
      _calleePopsStack = true;
      break;
    case CALL_CONV_GCCFASTCALL_2:
      _cconvArgumentsGp[0] = (REG_ECX & REGCODE_MASK);
      _cconvArgumentsGp[1] = (REG_EDX & REGCODE_MASK);
      _calleePopsStack = false;
      break;
    case CALL_CONV_GCCFASTCALL_3:
      _cconvArgumentsGp[0] = (REG_EDX & REGCODE_MASK);
      _cconvArgumentsGp[1] = (REG_ECX & REGCODE_MASK);
      _cconvArgumentsGp[2] = (REG_EAX & REGCODE_MASK);
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
      _cconvPreservedGp =
        (1 << (REG_RBX   & REGCODE_MASK)) |
        (1 << (REG_RSP   & REGCODE_MASK)) |
        (1 << (REG_RBP   & REGCODE_MASK)) |
        (1 << (REG_RSI   & REGCODE_MASK)) |
        (1 << (REG_RDI   & REGCODE_MASK)) |
        (1 << (REG_R12   & REGCODE_MASK)) |
        (1 << (REG_R13   & REGCODE_MASK)) |
        (1 << (REG_R14   & REGCODE_MASK)) |
        (1 << (REG_R15   & REGCODE_MASK)) ;
      _cconvPreservedXmm = 
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

      _cconvArgumentsGp[0] = (REG_RCX  & REGCODE_MASK);
      _cconvArgumentsGp[1] = (REG_RDX  & REGCODE_MASK);
      _cconvArgumentsGp[2] = (REG_R8   & REGCODE_MASK);
      _cconvArgumentsGp[3] = (REG_R9   & REGCODE_MASK);
      _cconvArgumentsXmm[0] = (REG_XMM0 & REGCODE_MASK);
      _cconvArgumentsXmm[1] = (REG_XMM1 & REGCODE_MASK);
      _cconvArgumentsXmm[2] = (REG_XMM2 & REGCODE_MASK);
      _cconvArgumentsXmm[3] = (REG_XMM3 & REGCODE_MASK);
      break;

    case CALL_CONV_X64U:
      _cconvPreservedGp =
        (1 << (REG_RBX   & REGCODE_MASK)) |
        (1 << (REG_RSP   & REGCODE_MASK)) |
        (1 << (REG_RBP   & REGCODE_MASK)) |
        (1 << (REG_R12   & REGCODE_MASK)) |
        (1 << (REG_R13   & REGCODE_MASK)) |
        (1 << (REG_R14   & REGCODE_MASK)) |
        (1 << (REG_R15   & REGCODE_MASK)) ;
      _cconvPreservedXmm = 0;

      _cconvArgumentsGp[0] = (REG_RDI  & REGCODE_MASK);
      _cconvArgumentsGp[1] = (REG_RSI  & REGCODE_MASK);
      _cconvArgumentsGp[2] = (REG_RDX  & REGCODE_MASK);
      _cconvArgumentsGp[3] = (REG_RCX  & REGCODE_MASK);
      _cconvArgumentsGp[4] = (REG_R8   & REGCODE_MASK);
      _cconvArgumentsGp[5] = (REG_R9   & REGCODE_MASK);
      _cconvArgumentsXmm[0] = (REG_XMM0 & REGCODE_MASK);
      _cconvArgumentsXmm[1] = (REG_XMM1 & REGCODE_MASK);
      _cconvArgumentsXmm[2] = (REG_XMM2 & REGCODE_MASK);
      _cconvArgumentsXmm[3] = (REG_XMM3 & REGCODE_MASK);
      _cconvArgumentsXmm[4] = (REG_XMM4 & REGCODE_MASK);
      _cconvArgumentsXmm[5] = (REG_XMM5 & REGCODE_MASK);
      _cconvArgumentsXmm[6] = (REG_XMM6 & REGCODE_MASK);
      _cconvArgumentsXmm[7] = (REG_XMM7 & REGCODE_MASK);
      break;

    default:
      ASMJIT_CRASH();
  }
#endif
}

void Function::_setArguments(const UInt32* _args, SysUInt count)
{
  ASMJIT_ASSERT(count <= 32);

  SysInt i;

  SysInt gpnPos = 0;
  SysInt xmmPos = 0;
  SysInt stackOffset = 0;

  UInt32 args[32];
  memcpy(args, _args, count * sizeof(UInt32));

  _variables.clear();
  for (i = 0; i < (SysInt)count; i++)
  {
    Variable* v = _compiler->newObject<Variable>(this, args[i]);
    v->_refCount = 1; // Arguments are never freed or reused.
    _variables.append(v);
  }

  if (!_args) return;

#if defined(ASMJIT_X86)
  // ==========================================================================
  // [X86 Calling Conventions]

  // Register arguments (Integer), always left-to-right
  for (i = 0; i != count; i++)
  {
    UInt32 a = args[i];
    if (isIntegerArgument(a) && gpnPos < 32 && _cconvArgumentsGp[gpnPos] != 0xFFFFFFFF)
    {
      _variables[i]->setAll(a, 0, VARIABLE_STATE_REGISTER, 10, _cconvArgumentsGp[gpnPos++] | REG_GPN, NO_REG, 0);
      args[i] = VARIABLE_TYPE_NONE;
    }
  }

  // Stack arguments
  bool ltr = _cconvArgumentsDirection == ARGUMENT_DIR_LEFT_TO_RIGHT;
  SysInt istart = ltr ? 0 : count-1;
  SysInt iend   = ltr ? count : -1;
  SysInt istep  = ltr ? 1 : -1;

  for (i = istart; i != iend; i += istep)
  {
    UInt32 a = args[i];
    if (isIntegerArgument(a))
    {
      stackOffset -= 4;

      _variables[i]->setAll(a, 0, VARIABLE_STATE_MEMORY, 20, NO_REG, NO_REG, stackOffset);
      args[i] = VARIABLE_TYPE_NONE;
    }
    else if (isFloatArgument(a))
    {
      if (a == VARIABLE_TYPE_FLOAT) 
        stackOffset -= 4;
      else
        stackOffset -= 8;

      _variables[i]->setAll(a, 0, VARIABLE_STATE_MEMORY, 20, NO_REG, NO_REG, stackOffset);
      args[i] = VARIABLE_TYPE_NONE;
    }
  }
  // ==========================================================================
#else
  // ==========================================================================
  // [X64 Calling Conventions]

  // Windows 64-bit specific
  if (cconv() == CALL_CONV_X64W)
  {
    SysInt max = count < 4 ? count : 4;

    // Register arguments (Integer / FP)
    for (i = 0; i != max; i++)
    {
      UInt32 a = args[i];
      if (isIntegerArgument(a))
      {
        _variables[i]->setAll(a, 0, VARIABLE_STATE_REGISTER, _cconvArgumentsGp[i] | REG_GPN, NO_REG, 0);
        args[i] = VARIABLE_TYPE_NONE;
      }
      else if (isFloatArgument(a))
      {
        _variables[i]->setAll(a, 0, VARIABLE_STATE_REGISTER, _cconvArgumentsXmm[i] | REG_XMM, NO_REG, 0);
        args[i] = VARIABLE_TYPE_NONE;
      }
    }

    // Stack arguments
    for (i = count-1; i != -1; i--)
    {
      UInt32 a = args[i];
      if (isIntegerArgument(a))
      {
        _variables[i]->setAll(a, 0, VARIABLE_STATE_MEMORY, NO_REG, NO_REG, stackOffset);
        args[i] = VARIABLE_TYPE_NONE;
      }
    }
  }
  // All others
  else
  {
  }
  // ==========================================================================
#endif // ASMJIT_X86, ASMJIT_X64

  // Modify stack offset (all function parameters will be in positive stack
  // offset that is never zero).
  for (i = 0; i < (SysInt)count; i++)
  {
    _variables[i]->_stackOffset += sizeof(SysInt) - stackOffset;
  }
}

Variable* Function::newVariable(UInt8 type)
{
  Variable* v;

  // First look to unused variables
  SysUInt i;
  for (i = 0; i < _variables.length(); i++)
  {
    v = _variables[i];
    if (v->refCount() == 0 && v->type() == type) return v;
  }

  // If variable can't be reused, create new one.
  v = compiler()->newObject<Variable>(this, type);
  _variables.append(v);
  return v;
}

void Function::alloc(Variable& v)
{
  ASMJIT_ASSERT(compiler() == v.compiler());
  if (v.state() == VARIABLE_STATE_REGISTER) return;

  UInt32 i;

  // New register code.
  UInt8 code = NO_REG;

  // true if we must copy content from memory to register before we can use it
  bool copy = v.state() == VARIABLE_STATE_MEMORY;

  // TODO:
  // if (v._preferredRegister != NO_REG)

  // First try to find unused registers
  if (v.type() == VARIABLE_TYPE_INT32 || v.type() == VARIABLE_TYPE_INT64)
  {
    // We start from 1, because EAX/RAX registers are sometimes explicitly 
    // needed
    for (i = 1; i < NUM_REGS; i++)
    {
      if ((_usedGpRegisters & (1U << i)) == 0 && i != RID_EBP && i != RID_ESP) 
      {
        if (v.type() == VARIABLE_TYPE_INT32)
          code = i | REG_GPD;
        else
          code = i | REG_GPQ;
        break;
      }
    }

    // If not found, try EAX/RAX
    if (code == NO_REG && (_usedGpRegisters & 1) != 0) 
    {
      if (v.type() == VARIABLE_TYPE_INT32)
        code = RID_EAX | REG_GPD;
      else
        code = RID_EAX | REG_GPQ;
    }
  }
  else if (v.type() == VARIABLE_TYPE_MM)
  {
    for (i = 0; i < 8; i++)
    {
      if ((_usedMmRegisters & (1U << i)) == 0)
      {
        code = i | REG_MM;
        break;
      }
    }
  }
  else if (v.type() == VARIABLE_TYPE_XMM)
  {
    for (i = 0; i < NUM_REGS; i++)
    {
      if ((_usedXmmRegisters & (1U << i)) == 0)
      {
        code = i | REG_XMM;
        break;
      }
    }
  }

  // If register is still not found, spill some variable
  if (code == NO_REG)
  {
    Variable* candidate = _getSpillCandidate(v.type());
    ASMJIT_ASSERT(candidate);

    code = candidate->_registerCode;
    spill(*candidate);
  }

  _allocReg(code);

  v._state = VARIABLE_STATE_REGISTER;
  v._registerCode = code;

  if (copy)
  {
    compiler()->mov(mk_gpn(v._registerCode), *v._memoryOperand);
  }
}

void Function::spill(Variable& v)
{
  ASMJIT_ASSERT(compiler() == v.compiler());
  if (v.state() == VARIABLE_STATE_UNUSED) return;
  if (v.state() == VARIABLE_STATE_MEMORY) return;

  if (v.state() == VARIABLE_STATE_REGISTER)
  {
    // FIXME: Dependent, Incorrect
    compiler()->mov(*v._memoryOperand, mk_gpd(v.registerCode()));

    _freeReg(v.registerCode());
    v._registerCode = NO_REG;

    v._state = VARIABLE_STATE_MEMORY;
    v._spillCount++;
  }
}

void Function::unuse(Variable& v)
{
  ASMJIT_ASSERT(compiler() == v.compiler());
  if (v.state() == VARIABLE_STATE_UNUSED) return;

  if (v.state() == VARIABLE_STATE_REGISTER)
  {
    _freeReg(v.registerCode());
    v._registerCode = NO_REG;
  }

  v._state = VARIABLE_STATE_UNUSED;
  v._reuseCount++;
}

static UInt32 getSpillScore(Variable* v)
{
  return v->priority();
}

Variable* Function::_getSpillCandidate(UInt8 type)
{
  Variable* candidate = NULL;
  Variable* v;
  SysUInt i, len = _variables.length();

  UInt32 candidateScore = 0;
  UInt32 variableScore;

  if (type == VARIABLE_TYPE_INT32 || type == VARIABLE_TYPE_INT64)
  {
    for (i = 0; i < len; i++)
    {
      v = _variables[i];
      if ((v->type() == VARIABLE_TYPE_INT32 || v->type() == VARIABLE_TYPE_INT64) &&
        v->state() == VARIABLE_STATE_REGISTER && v->priority() > 0)
      {
        variableScore = getSpillScore(v);
        if (variableScore > candidateScore) { candidateScore = variableScore; candidate = v; }
      }
    }
  }
  else if (type == VARIABLE_TYPE_MM)
  {
    // TODO
  }
  else if (type == VARIABLE_TYPE_XMM)
  {
    // TODO
  }

  return candidate;
}

void Function::_allocReg(UInt8 code)
{
  UInt32 type = code & REGTYPE_MASK;
  UInt32 mask = 1U << (code & REGCODE_MASK);

  switch (type)
  {
    case REG_GPB:
    case REG_GPW:
    case REG_GPD:
    case REG_GPQ:
      useGpRegisters(mask);
      modifyGpRegisters(mask);
      break;
    case REG_MM:
      useMmRegisters(mask);
      modifyMmRegisters(mask);
      break;
    case REG_XMM:
      useXmmRegisters(mask);
      modifyXmmRegisters(mask);
      break;
  }
}

void Function::_freeReg(UInt8 code)
{
  UInt32 type = code & REGTYPE_MASK;
  UInt32 mask = 1U << (code & REGCODE_MASK);

  switch (type)
  {
    case REG_GPB:
    case REG_GPW:
    case REG_GPD:
    case REG_GPQ:
      unuseGpRegisters(mask);
      break;
    case REG_MM:
      unuseMmRegisters(mask);
      break;
    case REG_XMM:
      unuseXmmRegisters(mask);
      break;
  }
}

// ============================================================================
// [AsmJit::Prolog]
// ============================================================================

Prolog::Prolog(Compiler* c, Function* f) : 
  Emittable(c, EMITTABLE_PROLOGUE), 
  _function(f)
{
}

Prolog::~Prolog()
{
}

void Prolog::emit(Assembler& a)
{
  Function* f = function();
  ASMJIT_ASSERT(f);

  int i;

  // Emit prolog if needed (prolog is not emitted for pure naked functions
  if (!f->naked() || f->maxAlignmentStackSize())
  {
    a.push(nbp);
    a.mov(nbp, nsp);

#if defined(ASMJIT_X86)
    // Alignment
    if (f->maxAlignmentStackSize())
    {
      // maxAlignmentStackSize can be 4 or 12, we build 8 or 16 from it.
      a.and_(nbp, -((Int32)(f->maxAlignmentStackSize() + 4)));
    }
#endif // ASMJIT_X86
  }

  // Save registers if needed
  for (i = 0; i < NUM_REGS; i++)
  {
    if ((f->modifiedGpRegisters () & (1U << i)) && 
        (f->cconvPreservedGp() & (1U << i)) &&
        (i != (REG_NBP & REGCODE_MASK)) &&
        (i != (REG_NSP & REGCODE_MASK)) )
    {
      a.push(mk_gpn(i));
    }
  }

  // After prolog, bind label
  if (_label) a.bind(_label);
}

// ============================================================================
// [AsmJit::Epilog]
// ============================================================================

Epilog::Epilog(Compiler* c, Function* f) : 
  Emittable(c, EMITTABLE_EPILOGUE),
  _function(f)
{
}

Epilog::~Epilog()
{
}

void Epilog::emit(Assembler& a)
{
  Function* f = function();
  ASMJIT_ASSERT(f);

  // First bind label (Function::_exitLabel) before the epilog
  if (_label) a.bind(_label);

  // Add variables and register cleanup code
  int i;
  for (i = NUM_REGS-1; i >= 0; i--)
  {
    if ((f->modifiedGpRegisters() & (1U << i)) && 
        (f->cconvPreservedGp() & (1U << i)) &&
        (i != (REG_NBP & REGCODE_MASK)) &&
        (i != (REG_NSP & REGCODE_MASK)) )
    {
      a.pop(mk_gpn(i));
    }
  }

  // Use epilog code (if needed)
  if (!f->naked() || f->maxAlignmentStackSize())
  {
    a.mov(nsp, nbp);
    a.pop(nbp);
  }

  // Return using correct instruction
  if (f->calleePopsStack())
  {
    a.ret((Int16)f->argumentsStackSize());
  }
  else
  {
    a.ret();
  }
}

// ============================================================================
// [AsmJit::Target]
// ============================================================================

Target::Target(Compiler* c, Label* l) : 
  Emittable(c, EMITTABLE_TARGET), 
  _label(l)
{
}

Target::~Target()
{
}

void Target::emit(Assembler& a)
{
  a.bind(_label);
}

// ============================================================================
// [AsmJit::Zone]
// ============================================================================

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

// ============================================================================
// [AsmJit::Compiler - Construction / Destruction]
// ============================================================================

Compiler::Compiler() :
  _currentPosition(0),
  _currentFunction(NULL),
  _zone(65536 - sizeof(Zone::Chunk) - 32),
  _labelIdCounter(1)
{
}

Compiler::~Compiler()
{
  delAll(_buffer);
}

// ============================================================================
// [AsmJit::Compiler - Buffer]
// ============================================================================

void Compiler::clear()
{
  delAll(_buffer);
  _buffer.clear();
  _operands.clear();
  _currentPosition = 0;
  _zone.freeAll();
}

void Compiler::free()
{
  clear();
  _buffer.free();
  _operands.free();
}

// ============================================================================
// [AsmJit::Compiler - Function Builder]
// ============================================================================

Function* Compiler::newFunction_(UInt32 cconv, const UInt32* args, SysUInt count)
{
  ASMJIT_ASSERT(_currentFunction == NULL);

  Function* f = _currentFunction = newObject<Function>();
  f->setPrototype(cconv, args, count);

  emit(f);

  Prolog* e = prolog();
  e->_label = f->_prologLabel;

  return f;
}

Function* Compiler::endFunction()
{
  ASMJIT_ASSERT(_currentFunction != NULL);
  Function* f = _currentFunction;

  Epilog* e = epilog();
  e->_label = f->_exitLabel;

  _currentFunction = NULL;
  return f;
}

Prolog* Compiler::prolog()
{
  Prolog* block = newObject<Prolog>(currentFunction());
  emit(block);
  return block;
}

Epilog* Compiler::epilog()
{
  Epilog* block = newObject<Epilog>(currentFunction());
  emit(block);
  return block;
}

// -------------------------------------------------------------------------
// [AsmJit::Compiler - Labels]
// -------------------------------------------------------------------------

Label* Compiler::newLabel()
{
  Label* label = new(_allocObject(sizeof(Label))) Label((UInt16)(_labelIdCounter++));
  _registerOperand(label);
  return label;
}

// ============================================================================
// [AsmJit::Compiler - Emit]
// ============================================================================

void Compiler::emit(Emittable* emittable, bool endblock)
{
  if (endblock)
  {
    _buffer.append(emittable);
  }
  else
  {
    _buffer.insert(_currentPosition, emittable);
    _currentPosition++;
  }
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
// [AsmJit::Compiler - Memory Management]
// -------------------------------------------------------------------------

void Compiler::_registerOperand(Operand* op)
{
  op->_operandId = _operands.length();
  _operands.append(op);
}

// -------------------------------------------------------------------------
// [AsmJit::Compiler - Variables Management / Register Allocation]
// -------------------------------------------------------------------------

Variable* Compiler::newVariable(UInt8 type)
{
  Function* f = currentFunction();
  ASMJIT_ASSERT(f != NULL);

  return f->newVariable(type);
}

// -------------------------------------------------------------------------
// [AsmJit::Compiler - EmitX86]
// -------------------------------------------------------------------------

void Compiler::_emitX86(UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3)
{
  emit(newObject<Instruction>(code, o1, o2, o3));
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
  emit(newObject<Target>(label));
}

} // AsmJit namespace
