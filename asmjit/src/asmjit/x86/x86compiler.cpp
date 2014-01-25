// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// [Export]
#define ASMJIT_EXPORTS

// [Guard]
#include "../build.h"
#if defined(ASMJIT_BUILD_X86) || defined(ASMJIT_BUILD_X64)

// [Dependencies - AsmJit]
#include "../base/intutil.h"
#include "../base/string.h"
#include "../x86/x86assembler.h"
#include "../x86/x86compiler.h"
#include "../x86/x86context_p.h"

// [Api-Begin]
#include "../base/apibegin.h"

namespace asmjit {
namespace x86x64 {

// ============================================================================
// [asmjit::x86x64::X86X64CallNode - Prototype]
// ============================================================================

void X86X64CallNode::setPrototype(uint32_t convention, const FuncPrototype& prototype) {
  _x86Decl.setPrototype(convention, prototype);
}

// ============================================================================
// [asmjit::x86x64::X86X64CallNode - Arg / Ret]
// ============================================================================

bool X86X64CallNode::_setArg(uint32_t i, const Operand& op) {
  if ((i & ~kFuncArgHi) >= _x86Decl.getArgCount())
    return false;

  _args[i] = op;
  return true;
}

bool X86X64CallNode::_setRet(uint32_t i, const Operand& op) {
  if (i >= 2)
    return false;

  _ret[i] = op;
  return true;
}

// ============================================================================
// [asmjit::x86x64::X86X64Compiler - Construction / Destruction]
// ============================================================================

X86X64Compiler::X86X64Compiler(BaseRuntime* runtime) : BaseCompiler(runtime) {}
X86X64Compiler::~X86X64Compiler() {}

// ============================================================================
// [asmjit::x86x64::X86X64Compiler - Inst]
// ============================================================================

//! @brief Get compiler instruction item size without operands assigned.
static ASMJIT_INLINE size_t X86X64Compiler_getInstSize(uint32_t code) {
  return (IntUtil::inInterval<uint32_t>(code, _kInstJbegin, _kInstJend)) ? sizeof(JumpNode) : sizeof(InstNode);
}

static InstNode* X86X64Compiler_newInst(X86X64Compiler* self, void* p, uint32_t code, uint32_t options, Operand* opList, uint32_t opCount) {
  if (IntUtil::inInterval<uint32_t>(code, _kInstJbegin, _kInstJend)) {
    JumpNode* node = new(p) JumpNode(self, code, options, opList, opCount);
    TargetNode* jTarget = self->getTargetById(opList[0].getId());

    node->addFlags(code == kInstJmp ? kNodeFlagIsJmp | kNodeFlagIsTaken : kNodeFlagIsJcc);
    node->_target = jTarget;
    node->_jumpNext = static_cast<JumpNode*>(jTarget->_from);

    jTarget->_from = node;
    jTarget->addNumRefs();

    // The 'jmp' is always taken, conditional jump can contain hint, we detect it.
    if (code == kInstJmp)
      node->addFlags(kNodeFlagIsTaken);
    else if (options & kInstOptionTaken)
      node->addFlags(kNodeFlagIsTaken);

    node->addOptions(options);
    return node;
  }
  else {
    InstNode* node = new(p) InstNode(self, code, options, opList, opCount);
    node->addOptions(options);
    return node;
  }
}

InstNode* X86X64Compiler::newInst(uint32_t code) {
  size_t size = X86X64Compiler_getInstSize(code);
  InstNode* inst = static_cast<InstNode*>(_zoneAllocator.alloc(size));

  if (inst == NULL)
    goto _NoMemory;

  return X86X64Compiler_newInst(this, inst, code, getOptionsAndClear(), NULL, 0);

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

InstNode* X86X64Compiler::newInst(uint32_t code, const Operand& o0) {
  size_t size = X86X64Compiler_getInstSize(code);
  InstNode* inst = static_cast<InstNode*>(_zoneAllocator.alloc(size + 1 * sizeof(Operand)));

  if (inst == NULL)
    goto _NoMemory;

  {
    Operand* opList = reinterpret_cast<Operand*>(reinterpret_cast<uint8_t*>(inst) + size);
    opList[0] = o0;
    return X86X64Compiler_newInst(this, inst, code, getOptionsAndClear(), opList, 1);
  }

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

InstNode* X86X64Compiler::newInst(uint32_t code, const Operand& o0, const Operand& o1) {
  size_t size = X86X64Compiler_getInstSize(code);
  InstNode* inst = static_cast<InstNode*>(_zoneAllocator.alloc(size + 2 * sizeof(Operand)));

  if (inst == NULL)
    goto _NoMemory;

  {
    Operand* opList = reinterpret_cast<Operand*>(reinterpret_cast<uint8_t*>(inst) + size);
    opList[0] = o0;
    opList[1] = o1;
    return X86X64Compiler_newInst(this, inst, code, getOptionsAndClear(), opList, 2);
  }

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

InstNode* X86X64Compiler::newInst(uint32_t code, const Operand& o0, const Operand& o1, const Operand& o2) {
  size_t size = X86X64Compiler_getInstSize(code);
  InstNode* inst = static_cast<InstNode*>(_zoneAllocator.alloc(size + 3 * sizeof(Operand)));

  if (inst == NULL)
    goto _NoMemory;

  {
    Operand* opList = reinterpret_cast<Operand*>(reinterpret_cast<uint8_t*>(inst) + size);
    opList[0] = o0;
    opList[1] = o1;
    opList[2] = o2;
    return X86X64Compiler_newInst(this, inst, code, getOptionsAndClear(), opList, 3);
  }

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

InstNode* X86X64Compiler::newInst(uint32_t code, const Operand& o0, const Operand& o1, const Operand& o2, const Operand& o3) {
  size_t size = X86X64Compiler_getInstSize(code);
  InstNode* inst = static_cast<InstNode*>(_zoneAllocator.alloc(size + 4 * sizeof(Operand)));

  if (inst == NULL)
    goto _NoMemory;

  {
    Operand* opList = reinterpret_cast<Operand*>(reinterpret_cast<uint8_t*>(inst) + size);
    opList[0] = o0;
    opList[1] = o1;
    opList[2] = o2;
    opList[3] = o3;
    return X86X64Compiler_newInst(this, inst, code, getOptionsAndClear(), opList, 4);
  }

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

InstNode* X86X64Compiler::newInst(uint32_t code, const Operand& o0, const Operand& o1, const Operand& o2, const Operand& o3, const Operand& o4) {
  size_t size = X86X64Compiler_getInstSize(code);
  InstNode* inst = static_cast<InstNode*>(_zoneAllocator.alloc(size + 5 * sizeof(Operand)));

  if (inst == NULL)
    goto _NoMemory;

  {
    Operand* opList = reinterpret_cast<Operand*>(reinterpret_cast<uint8_t*>(inst) + size);
    opList[0] = o0;
    opList[1] = o1;
    opList[2] = o2;
    opList[3] = o3;
    opList[4] = o4;
    return X86X64Compiler_newInst(this, inst, code, getOptionsAndClear(), opList, 5);
  }

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

InstNode* X86X64Compiler::emit(uint32_t code) {
  InstNode* node = newInst(code);
  if (node == NULL)
    return NULL;
  return static_cast<InstNode*>(addNode(node));
}

InstNode* X86X64Compiler::emit(uint32_t code, const Operand& o0) {
  InstNode* node = newInst(code, o0);
  if (node == NULL)
    return NULL;
  return static_cast<InstNode*>(addNode(node));
}

InstNode* X86X64Compiler::emit(uint32_t code, const Operand& o0, const Operand& o1){
  InstNode* node = newInst(code, o0, o1);
  if (node == NULL)
    return NULL;
  return static_cast<InstNode*>(addNode(node));
}

InstNode* X86X64Compiler::emit(uint32_t code, const Operand& o0, const Operand& o1, const Operand& o2) {
  InstNode* node = newInst(code, o0, o1, o2);
  if (node == NULL)
    return NULL;
  return static_cast<InstNode*>(addNode(node));
}

InstNode* X86X64Compiler::emit(uint32_t code, const Operand& o0, const Operand& o1, const Operand& o2, const Operand& o3){
  InstNode* node = newInst(code, o0, o1, o2, o3);
  if (node == NULL)
    return NULL;
  return static_cast<InstNode*>(addNode(node));
}

InstNode* X86X64Compiler::emit(uint32_t code, const Operand& o0, const Operand& o1, const Operand& o2, const Operand& o3, const Operand& o4) {
  InstNode* node = newInst(code, o0, o1, o2, o3, o4);
  if (node == NULL)
    return NULL;
  return static_cast<InstNode*>(addNode(node));
}

InstNode* X86X64Compiler::emit(uint32_t code, int o0_) {
  Imm o0(o0_);
  InstNode* node = newInst(code, o0);
  if (node == NULL)
    return NULL;
  return static_cast<InstNode*>(addNode(node));
}

InstNode* X86X64Compiler::emit(uint32_t code, const Operand& o0, int o1_) {
  Imm o1(o1_);
  InstNode* node = newInst(code, o0, o1);
  if (node == NULL)
    return NULL;
  return static_cast<InstNode*>(addNode(node));
}

InstNode* X86X64Compiler::emit(uint32_t code, const Operand& o0, const Operand& o1, int o2_) {
  Imm o2(o2_);
  InstNode* node = newInst(code, o0, o1, o2);
  if (node == NULL)
    return NULL;
  return static_cast<InstNode*>(addNode(node));
}

// ============================================================================
// [asmjit::x86x64::X86X64Compiler - Func]
// ============================================================================

X86X64FuncNode* X86X64Compiler::newFunc(uint32_t convention, const FuncPrototype& prototype) {
  X86X64FuncNode* func = newNode<X86X64FuncNode>();
  if (func == NULL)
    goto _NoMemory;

  // Create helper nodes.
  func->_entryNode = newTarget();
  func->_exitNode = newTarget();
  func->_end = newNode<EndNode>();

  if (func->_entryNode == NULL || func->_exitNode == NULL || func->_end == NULL)
    goto _NoMemory;

  // Emit push/pop sequence by default.
  func->_funcHints |= IntUtil::mask(kFuncHintPushPop);

  // Function prototype.
  func->_x86Decl.setPrototype(convention, prototype);

  // Function arguments stack size. Since function requires _argStackSize to be
  // set, we have to copy it from X86X64FuncDecl.
  func->_argStackSize = func->_x86Decl.getArgStackSize();
  func->_redZoneSize = static_cast<uint16_t>(func->_x86Decl.getRedZoneSize());
  func->_spillZoneSize = static_cast<uint16_t>(func->_x86Decl.getSpillZoneSize());

  // Expected/Required stack alignment.
  func->_expectedStackAlignment = getRuntime()->getStackAlignment();
  func->_requiredStackAlignment = 0;

  // Allocate space for function arguments.
  func->_argList = NULL;
  if (func->getArgCount() != 0) {
    func->_argList = _zoneAllocator.allocT<VarData*>(func->getArgCount() * sizeof(VarData*));
    if (func->_argList == NULL)
      goto _NoMemory;
    ::memset(func->_argList, 0, func->getArgCount() * sizeof(VarData*));
  }

  return func;

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

X86X64FuncNode* X86X64Compiler::addFunc(uint32_t convention, const FuncPrototype& prototype) {
  X86X64FuncNode* func = newFunc(convention, prototype);

  if (func == NULL) {
    setError(kErrorNoHeapMemory);
    return NULL;
  }

  ASMJIT_ASSERT(_func == NULL);
  _func = func;

  addNode(func);
  addNode(func->getEntryNode());

  return func;
}

EndNode* X86X64Compiler::endFunc() {
  X86X64FuncNode* func = getFunc();
  ASMJIT_ASSERT(func != NULL);

  addNode(func->getExitNode());
  addNode(func->getEnd());

  func->addFuncFlags(kFuncFlagIsFinished);
  _func = NULL;

  return func->getEnd();
}

// ============================================================================
// [asmjit::x86x64::X86X64Compiler - Ret]
// ============================================================================

RetNode* X86X64Compiler::newRet(const Operand& o0, const Operand& o1) {
  RetNode* node = newNode<RetNode>(o0, o1);
  if (node == NULL)
    goto _NoMemory;
  return node;

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

RetNode* X86X64Compiler::addRet(const Operand& o0, const Operand& o1) {
  RetNode* node = newRet(o0, o1);
  if (node == NULL)
    return node;
  return static_cast<RetNode*>(addNode(node));
}

// ============================================================================
// [asmjit::x86x64::X86X64Compiler - Call]
// ============================================================================

X86X64CallNode* X86X64Compiler::newCall(const Operand& o0, uint32_t convention, const FuncPrototype& prototype) {
  X86X64CallNode* node = newNode<X86X64CallNode>(o0);
  if (node == NULL)
    goto _NoMemory;

  uint32_t nArgs = prototype.getArgCount();
  node->_x86Decl.setPrototype(convention, prototype);

  // If there are no arguments skip the allocation.
  if (!nArgs)
    return node;

  node->_args = static_cast<Operand*>(_zoneAllocator.alloc(nArgs * sizeof(Operand)));
  if (node->_args == NULL)
    goto _NoMemory;

  ::memset(node->_args, 0, sizeof(Operand) * nArgs);
  return node;

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

X86X64CallNode* X86X64Compiler::addCall(const Operand& o0, uint32_t convention, const FuncPrototype& prototype) {
  X86X64CallNode* node = newCall(o0, convention, prototype);
  if (node == NULL)
    return NULL;
  return static_cast<X86X64CallNode*>(addNode(node));
}

// ============================================================================
// [asmjit::x86x64::X86X64Compiler - Variables]
// ============================================================================

VarData* X86X64Compiler::newVd(const char* name, uint32_t type, uint32_t size) {
  VarData* vd = reinterpret_cast<VarData*>(_varAllocator.alloc(sizeof(VarData)));
  if (vd == NULL)
    goto _NoMemory;

  vd->_name = name ? _stringAllocator.sdup(name) : static_cast<char*>(NULL);
  vd->_id = OperandUtil::makeVarId(static_cast<uint32_t>(_vars.getLength()));

  vd->_contextId = kInvalidValue;

  vd->_type = static_cast<uint8_t>(type);
  vd->_class = static_cast<uint8_t>(_varInfo[type].getClass());
  vd->_size = static_cast<uint8_t>(size);
  vd->_flags = 0;

  vd->_priority = 10;
  vd->_state = kVarStateUnused;
  vd->_regIndex = kInvalidReg;

  vd->_isMemArg = false;
  vd->_isCalculated = false;
  vd->_saveOnUnuse = false;
  vd->_modified = false;

  vd->_reserved0 = 0;
  vd->_reserved1 = 0;

  vd->_memOffset = 0;
  vd->_memCell = NULL;

  vd->rReadCount = 0;
  vd->rWriteCount = 0;
  vd->mReadCount = 0;
  vd->mWriteCount = 0;

  vd->_va = NULL;

  if (_vars.append(vd) != kErrorOk)
    goto _NoMemory;
  return vd;

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

bool X86X64Compiler::setArg(uint32_t argIndex, BaseVar& var) {
  X86X64FuncNode* func = getFunc();

  if (func == NULL)
    return false;

  if (!isVarCreated(var))
    return false;

  VarData* vd = getVd(var);
  func->setArg(argIndex, vd);

  return true;
}

bool X86X64Compiler::_newVar(BaseVar* var, uint32_t vType, const char* name) {
  ASMJIT_ASSERT(vType < kVarTypeCount);

  if (getArch() == kArchX86 && IntUtil::inInterval<uint32_t>(vType, kVarTypeInt64, vType == kVarTypeUInt64)) {
    vType -= 2;
    if (_logger)
      _logger->logString(kLoggerStyleComment, "*** WARNING: 64-bit variable truncated to 32-bit. ***\n");
  }

  uint32_t size = _varInfo[vType].getSize();
  VarData* vd = newVd(name, vType, size);

  if (vd == NULL) {
    var->_init_packed_op_sz_r0_r1_id(kOperandTypeVar,
      0, 0, 0, kInvalidValue);
    var->_init_packed_u2_u3(kInvalidValue, kInvalidValue);
    return false;
  }
  else {
    var->_init_packed_op_sz_r0_r1_id(kOperandTypeVar,
      vd->_size, _varInfo[vType].getReg(), 0, vd->_id);
    var->_vreg.vType = vType;
    return true;
  }
}

GpVar X86X64Compiler::newGpVar(uint32_t vType, const char* name) {
  ASMJIT_ASSERT(_varInfo[vType].getClass() == kRegClassGp);

  GpVar var(DontInitialize);
  _newVar(&var, vType, name);
  return var;
}

MmVar X86X64Compiler::newMmVar(uint32_t vType, const char* name) {
  ASMJIT_ASSERT(_varInfo[vType].getClass() == kRegClassMm);

  MmVar var(DontInitialize);
  _newVar(&var, vType, name);
  return var;
}

XmmVar X86X64Compiler::newXmmVar(uint32_t vType, const char* name) {
  ASMJIT_ASSERT(_varInfo[vType].getClass() == kRegClassXy);

  XmmVar var(DontInitialize);
  _newVar(&var, vType, name);
  return var;
}

// ============================================================================
// [asmjit::x86x64::X86X64Compiler - Make]
// ============================================================================

template<typename Assembler>
static ASMJIT_INLINE void* X86X64Compiler_make(X86X64Compiler* self) {
  Assembler assembler(self->_runtime);
  BaseLogger* logger = self->_logger;

  if (logger)
    assembler.setLogger(logger);

  assembler._features = self->_features;

  if (self->serialize(assembler) != kErrorOk)
    return NULL;

  if (assembler.getError() != kErrorOk) {
    self->setError(assembler.getError());
    return NULL;
  }

  void* result = assembler.make();
  if (logger) {
    logger->logFormat(kLoggerStyleComment,
      "*** COMPILER SUCCESS - Wrote %u bytes, code: %u, trampolines: %u.\n\n",
      static_cast<unsigned int>(assembler.getCodeSize()),
      static_cast<unsigned int>(assembler.getOffset()),
      static_cast<unsigned int>(assembler.getTrampolineSize()));
  }

  return result;
}

// ============================================================================
// [asmjit::x86x64::X86X64Compiler - Assemble]
// ============================================================================

Error X86X64Compiler::serialize(BaseAssembler& assembler) {
  if (_firstNode == NULL)
    return kErrorOk;

  X86X64Context context(this);
  Error error = kErrorOk;

  BaseNode* node = _firstNode;
  BaseNode* start;

  // Find function and use the context to translate/emit.
  do {
    start = node;

    if (node->getType() == kNodeTypeFunc) {
      node = static_cast<X86X64FuncNode*>(start)->getEnd();
      error = context.compile(static_cast<X86X64FuncNode*>(start));

      if (error != kErrorOk)
        goto _Error;
    }

    do {
      node = node->getNext();
    } while (node != NULL && node->getType() != kNodeTypeFunc);

    error = context.serialize(&assembler, start, node);
    if (error != kErrorOk)
      goto _Error;
    context.cleanup();
  } while (node != NULL);
  return kErrorOk;

_Error:
  context.cleanup();
  return error;
}

} // x86x64 namespace
} // asmjit namespace

// ============================================================================
// [asmjit::x86]
// ============================================================================

#if defined(ASMJIT_BUILD_X86)

namespace asmjit {
namespace x86 {

Compiler::Compiler(BaseRuntime* runtime) : X86X64Compiler(runtime) {
  _arch = kArchX86;
  _regSize = 4;
}

Compiler::~Compiler() {}

void* Compiler::make() {
  return X86X64Compiler_make<x86::Assembler>(this);
}

} // x86 namespace
} // asmjit namespace

#endif // ASMJIT_BUILD_X86

// ============================================================================
// [asmjit::x64]
// ============================================================================

#if defined(ASMJIT_BUILD_X64)

namespace asmjit {
namespace x64 {

Compiler::Compiler(BaseRuntime* runtime) : X86X64Compiler(runtime) {
  _arch = kArchX64;
  _regSize = 8;
}

Compiler::~Compiler() {}

void* Compiler::make() {
  return X86X64Compiler_make<x64::Assembler>(this);
}

} // x64 namespace
} // asmjit namespace

#endif // ASMJIT_BUILD_X64

// [Api-End]
#include "../base/apiend.h"

// [Guard]
#endif // ASMJIT_BUILD_X86 || ASMJIT_BUILD_X64
