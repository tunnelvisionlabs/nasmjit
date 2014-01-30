// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// [Export]
#define ASMJIT_EXPORTS

// [Dependencies - AsmJit]
#include "../base/assembler.h"
#include "../base/compiler.h"
#include "../base/context_p.h"
#include "../base/cpu.h"
#include "../base/intutil.h"
#include "../base/logger.h"

// [Dependencies - C]
#include <stdarg.h>

// [Api-Begin]
#include "../base/apibegin.h"

namespace asmjit {

// ============================================================================
// [Constants]
// ============================================================================

static const char noName[1] = { '\0' };
enum { kBaseCompilerDefaultLookAhead = 64 };

// ============================================================================
// [asmjit::BaseCompiler - Construction / Destruction]
// ============================================================================

BaseCompiler::BaseCompiler(BaseRuntime* runtime) :
  CodeGen(runtime),
  _nodeFlowId(0),
  _nodeFlags(0),
  _maxLookAhead(kBaseCompilerDefaultLookAhead),
  _targetVarMapping(NULL),
  _firstNode(NULL),
  _lastNode(NULL),
  _cursor(NULL),
  _func(NULL),
  _varAllocator(4096 - kMemAllocOverhead),
  _stringAllocator(4096 - kMemAllocOverhead) {}

BaseCompiler::~BaseCompiler() {
  reset();
}

// ============================================================================
// [asmjit::BaseCompiler - Clear / Reset]
// ============================================================================

void BaseCompiler::clear() {
  _purge();
}

void BaseCompiler::reset() {
  _purge();
  _zoneAllocator.reset();

  _varAllocator.reset();
  _stringAllocator.reset();

  _targets.reset();
  _vars.reset();
}

void BaseCompiler::_purge() {
  _zoneAllocator.clear();

  _varAllocator.clear();
  _stringAllocator.clear();

  _options = 0;

  _firstNode = NULL;
  _lastNode = NULL;

  _cursor = NULL;
  _func = NULL;

  _targets.clear();
  _vars.clear();

  clearError();
}

// ============================================================================
// [asmjit::BaseCompiler - Node Management]
// ============================================================================

BaseNode* BaseCompiler::setCursor(BaseNode* node) {
  BaseNode* old = _cursor;
  _cursor = node;
  return old;
}

BaseNode* BaseCompiler::addNode(BaseNode* node) {
  ASMJIT_ASSERT(node != NULL);
  ASMJIT_ASSERT(node->_prev == NULL);
  ASMJIT_ASSERT(node->_next == NULL);

  if (_cursor == NULL) {
    if (_firstNode == NULL) {
      _firstNode = node;
      _lastNode = node;
    }
    else {
      node->_next = _firstNode;
      _firstNode->_prev = node;
      _firstNode = node;
    }
  }
  else {
    BaseNode* prev = _cursor;
    BaseNode* next = _cursor->_next;

    node->_prev = prev;
    node->_next = next;

    prev->_next = node;
    if (next)
      next->_prev = node;
    else
      _lastNode = node;
  }

  _cursor = node;
  return node;
}

BaseNode* BaseCompiler::addNodeBefore(BaseNode* node, BaseNode* ref) {
  ASMJIT_ASSERT(node != NULL);
  ASMJIT_ASSERT(node->_prev == NULL);
  ASMJIT_ASSERT(node->_next == NULL);
  ASMJIT_ASSERT(ref != NULL);

  BaseNode* prev = ref->_prev;
  BaseNode* next = ref;

  node->_prev = prev;
  node->_next = next;

  next->_prev = node;
  if (prev)
    prev->_next = node;
  else
    _firstNode = node;

  return node;
}

BaseNode* BaseCompiler::addNodeAfter(BaseNode* node, BaseNode* ref) {
  ASMJIT_ASSERT(node != NULL);
  ASMJIT_ASSERT(node->_prev == NULL);
  ASMJIT_ASSERT(node->_next == NULL);
  ASMJIT_ASSERT(ref != NULL);

  BaseNode* prev = ref;
  BaseNode* next = ref->_next;

  node->_prev = prev;
  node->_next = next;

  prev->_next = node;
  if (next)
    next->_prev = node;
  else
    _lastNode = node;

  return node;
}

BaseNode* BaseCompiler::removeNode(BaseNode* node) {
  BaseNode* prev = node->_prev;
  BaseNode* next = node->_next;

  if (_firstNode == node)
    _firstNode = next;
  else
    prev->_next = next;

  if (_lastNode == node)
    _lastNode  = prev;
  else
    next->_prev = prev;

  node->_prev = NULL;
  node->_next = NULL;

  if (_cursor == node)
    _cursor = prev;

  return node;
}

void BaseCompiler::removeNodes(BaseNode* first, BaseNode* last) {
  if (first == last) {
    removeNode(first);
    return;
  }

  BaseNode* prev = first->_prev;
  BaseNode* next = last->_next;

  if (_firstNode == first)
    _firstNode = next;
  else
    prev->_next = next;

  if (_lastNode == last)
    _lastNode  = prev;
  else
    next->_prev = prev;

  BaseNode* node = first;
  for (;;) {
    BaseNode* next = node->getNext();
    ASMJIT_ASSERT(next != NULL);

    node->_prev = NULL;
    node->_next = NULL;

    if (_cursor == node)
      _cursor = prev;

    if (node == last)
      break;
    node = next;
  }
}

// ============================================================================
// [asmjit::BaseCompiler - Align]
// ============================================================================

AlignNode* BaseCompiler::newAlign(uint32_t m) {
  AlignNode* node = newNode<AlignNode>(m);
  if (node == NULL)
    goto _NoMemory;
  return node;

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

AlignNode* BaseCompiler::addAlign(uint32_t m) {
  AlignNode* node = newAlign(m);
  if (node == NULL)
    return NULL;
  return static_cast<AlignNode*>(addNode(node));
}

// ============================================================================
// [asmjit::BaseCompiler - Target]
// ============================================================================

TargetNode* BaseCompiler::newTarget() {
  TargetNode* node = newNode<TargetNode>(
    OperandUtil::makeLabelId(static_cast<uint32_t>(_targets.getLength())));

  if (node == NULL || _targets.append(node) != kErrorOk)
    goto _NoMemory;
  return node;

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

TargetNode* BaseCompiler::addTarget() {
  TargetNode* node = newTarget();
  if (node == NULL)
    return NULL;
  return static_cast<TargetNode*>(addNode(node));
}

// ============================================================================
// [asmjit::BaseCompiler - Label]
// ============================================================================

Error BaseCompiler::_newLabel(Label* dst) {
  dst->_init_packed_op_sz_b0_b1_id(kOperandTypeLabel, 0, 0, 0, kInvalidValue);
  dst->_init_packed_d2_d3(0, 0);

  TargetNode* node = newTarget();
  if (node == NULL)
    goto _NoMemory;

  dst->_label.id = node->getLabelId();
  return kErrorOk;

_NoMemory:
  return setError(kErrorNoHeapMemory);
}

void BaseCompiler::bind(const Label& label) {
  uint32_t index = label.getId();
  ASMJIT_ASSERT(index < _targets.getLength());

  addNode(_targets[index]);
}

// ============================================================================
// [asmjit::BaseCompiler - Embed]
// ============================================================================

EmbedNode* BaseCompiler::newEmbed(const void* data, uint32_t size) {
  EmbedNode* node;

  if (size > EmbedNode::kInlineBufferSize) {
    void* clonedData = _stringAllocator.alloc(size);
    if (clonedData == NULL)
      goto _NoMemory;

    ::memcpy(clonedData, data, size);
    data = clonedData;
  }

  node = newNode<EmbedNode>(const_cast<void*>(data), size);
  if (node == NULL)
    goto _NoMemory;
  return node;

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

EmbedNode* BaseCompiler::addEmbed(const void* data, uint32_t size) {
  EmbedNode* node = newEmbed(data, size);
  if (node == NULL)
    return node;
  return static_cast<EmbedNode*>(addNode(node));
}

// ============================================================================
// [asmjit::BaseCompiler - Comment]
// ============================================================================

CommentNode* BaseCompiler::newComment(const char* str) {
  CommentNode* node;

  if (str != NULL && str[0]) {
    str = _stringAllocator.sdup(str);
    if (str == NULL)
      goto _NoMemory;
  }

  node = newNode<CommentNode>(str);
  if (node == NULL)
    goto _NoMemory;
  return node;

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

CommentNode* BaseCompiler::addComment(const char* str) {
  CommentNode* node = newComment(str);
  if (node == NULL)
    return NULL;
  return static_cast<CommentNode*>(addNode(node));
}

CommentNode* BaseCompiler::comment(const char* fmt, ...) {
  char buf[256];
  char* p = buf;

  if (fmt) {
    *p++ = ';';
    *p++ = ' ';

    va_list ap;
    va_start(ap, fmt);
    p += vsnprintf(p, 254, fmt, ap);
    va_end(ap);
  }

  p[0] = '\n';
  p[1] = '\0';

  return addComment(fmt);
}

// ============================================================================
// [asmjit::BaseCompiler - Hint]
// ============================================================================

HintNode* BaseCompiler::newHint(BaseVar& var, uint32_t hint, uint32_t value) {
  if (var.getId() == kInvalidValue)
    return NULL;
  VarData* vd = getVd(var);

  HintNode* node = newNode<HintNode>(vd, hint, value);
  if (node == NULL)
    goto _NoMemory;
  return node;

_NoMemory:
  setError(kErrorNoHeapMemory);
  return NULL;
}

HintNode* BaseCompiler::addHint(BaseVar& var, uint32_t hint, uint32_t value) {
  if (var.getId() == kInvalidValue)
    return NULL;

  HintNode* node = newHint(var, hint, value);
  if (node == NULL)
    return NULL;
  return static_cast<HintNode*>(addNode(node));
}

// ============================================================================
// [asmjit::BaseCompiler - Vars]
// ============================================================================

VarData* BaseCompiler:: _newVd(uint32_t type, uint32_t size, uint32_t c, const char* name) {
  VarData* vd = reinterpret_cast<VarData*>(_varAllocator.alloc(sizeof(VarData)));
  if (vd == NULL)
    goto _NoMemory;

  vd->_name = noName;
  vd->_id = OperandUtil::makeVarId(static_cast<uint32_t>(_vars.getLength()));
  vd->_contextId = kInvalidValue;

  if (name != NULL && name[0] != '\0') {
    vd->_name = _stringAllocator.sdup(name);
  }

  vd->_type = static_cast<uint8_t>(type);
  vd->_class = static_cast<uint8_t>(c);
  vd->_flags = 0;
  vd->_priority = 10;

  vd->_state = kVarStateUnused;
  vd->_regIndex = kInvalidReg;
  vd->_isStack = false;
  vd->_isMemArg = false;
  vd->_isCalculated = false;
  vd->_saveOnUnuse = false;
  vd->_modified = false;
  vd->_reserved0 = 0;
  vd->_alignment = static_cast<uint8_t>(IntUtil::iMin<uint32_t>(size, 64));

  vd->_size = size;

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

void BaseCompiler::alloc(BaseVar& var) {
  addHint(var, kVarHintAlloc, kInvalidValue);
}

void BaseCompiler::alloc(BaseVar& var, uint32_t regIndex) {
  addHint(var, kVarHintAlloc, regIndex);
}

void BaseCompiler::alloc(BaseVar& var, const BaseReg& reg) {
  addHint(var, kVarHintAlloc, reg.getRegIndex());
}

void BaseCompiler::save(BaseVar& var) {
  addHint(var, kVarHintSave, kInvalidValue);
}

void BaseCompiler::spill(BaseVar& var) {
  addHint(var, kVarHintSpill, kInvalidValue);
}

void BaseCompiler::unuse(BaseVar& var) {
  addHint(var, kVarHintUnuse, kInvalidValue);
}

uint32_t BaseCompiler::getPriority(BaseVar& var) const {
  if (var.getId() == kInvalidValue)
    return kInvalidValue;

  VarData* vd = getVdById(var.getId());
  return vd->getPriority();
}

void BaseCompiler::setPriority(BaseVar& var, uint32_t priority) {
  if (var.getId() == kInvalidValue)
    return;

  if (priority > 255)
    priority = 255;

  VarData* vd = getVdById(var.getId());
  vd->_priority = static_cast<uint8_t>(priority);
}

bool BaseCompiler::getSaveOnUnuse(BaseVar& var) const {
  if (var.getId() == kInvalidValue)
    return false;

  VarData* vd = getVdById(var.getId());
  return static_cast<bool>(vd->_saveOnUnuse);
}

void BaseCompiler::setSaveOnUnuse(BaseVar& var, bool value) {
  if (var.getId() == kInvalidValue)
    return;

  VarData* vd = getVdById(var.getId());
  vd->_saveOnUnuse = value;
}

void BaseCompiler::rename(BaseVar& var, const char* name) {
  if (var.getId() == kInvalidValue)
    return;

  VarData* vd = getVdById(var.getId());
  vd->_name = noName;

  if (name != NULL && name[0] != '\0') {
    vd->_name = _stringAllocator.sdup(name);
  }
}

} // asmjit namespace

// [Api-End]
#include "../base/apiend.h"
