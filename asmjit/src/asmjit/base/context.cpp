// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// [Export]
#define ASMJIT_EXPORTS

// [Dependencies - AsmJit]
#include "../base/context_p.h"
#include "../base/intutil.h"

// [Api-Begin]
#include "../base/apibegin.h"

namespace asmjit {

// ============================================================================
// [asmjit::BaseContext - Construction / Destruction]
// ============================================================================

BaseContext::BaseContext(BaseCompiler* compiler) :
  _compiler(compiler),
  _func(NULL),
  _zoneAllocator(8192 - sizeof(Zone::Chunk) - kMemAllocOverhead),
  _start(NULL),
  _end(NULL),
  _extraBlock(NULL),
  _stop(NULL),
  _unreachableList(),
  _jccList(),
  _contextVd(),
  _memCells(NULL),
  _numStackCells(0),
  _num64ByteCells(0),
  _num32ByteCells(0),
  _num16ByteCells(0),
  _num8ByteCells(0),
  _num4ByteCells(0),
  _num2ByteCells(0),
  _num1ByteCells(0),
  _memSize(0),
  _state(NULL) {}

BaseContext::~BaseContext() {}

// ============================================================================
// [asmjit::BaseContext - Reset]
// ============================================================================

void BaseContext::reset() {
  _zoneAllocator.clear();

  _func = NULL;
  _start = NULL;
  _end = NULL;

  _unreachableList.reset();
  _jccList.reset();

  _contextVd.clear();
  _memCells = NULL;

  _num64ByteCells = 0;
  _num32ByteCells = 0;
  _num16ByteCells = 0;
  _num8ByteCells = 0;
  _num4ByteCells = 0;
  _num2ByteCells = 0;
  _num1ByteCells = 0;

  _memSize = 0;
  _maxCellSize = 0;
}

// ============================================================================
// [asmjit::BaseContext - MemCell]
// ============================================================================

static ASMJIT_INLINE uint32_t BaseContext_getVarAlignment(uint32_t size) {
  if (size > 32)
    return 64;
  else if (size > 16)
    return 32;
  else if (size > 8)
    return 16;
  else if (size > 4)
    return 8;
  else if (size > 2)
    return 4;
  else if (size > 1)
    return 2;
  else
    return 1;
}

MemCell* BaseContext::_newMemCell(uint32_t size, uint32_t alignment) {
  ASMJIT_ASSERT(size != 0);

  if (alignment > 1)
    size = IntUtil::alignTo<uint32_t>(size, alignment);

  // First try to find free block.
  MemCell* cell;

  // If there is no free cell we create a new one.
  cell = static_cast<MemCell*>(_zoneAllocator.alloc(sizeof(MemCell)));
  if (cell == NULL)
    goto _NoMemory;

  cell->_next = _memCells;
  cell->_offset = 0;
  cell->_size = size;
  cell->_occupied = true;

  _memCells = cell;
  _memSize += size;
  _maxCellSize = IntUtil::iMax(_maxCellSize, size);

  switch (size) {
    case 64: _num64ByteCells++; break;
    case 32: _num32ByteCells++; break;
    case 16: _num16ByteCells++; break;
    case  8: _num8ByteCells++ ; break;
    case  4: _num4ByteCells++ ; break;
    case  2: _num2ByteCells++ ; break;
    case  1: _num1ByteCells++ ; break;
  }

  return cell;

_NoMemory:
  _compiler->setError(kErrorNoHeapMemory);
  return NULL;
}

MemCell* BaseContext::_newVarCell(VarData* vd) {
  ASMJIT_ASSERT(vd->_memCell == NULL);

  uint32_t size = vd->getSize();
  MemCell* cell = _newMemCell(size, BaseContext_getVarAlignment(size));

  if (cell == NULL)
    return NULL;

  vd->_memCell = cell;
  return cell;
}

// ============================================================================
// [asmjit::BaseContext - RemoveUnreachableCode]
// ============================================================================

Error BaseContext::removeUnreachableCode() {
  PodList<BaseNode*>::Link* link = _unreachableList.getFirst();
  BaseNode* stop = getStop();

  while (link != NULL) {
    BaseNode* node = link->getValue();
    if (node != NULL) {
      // Locate all unreachable nodes.
      BaseNode* first = node;
      do {
        if (node->isFetched() || (node->getType() == kNodeTypeTarget && static_cast<TargetNode*>(node)->getNumRefs() > 0))
          break;
        node = node->getNext();
      } while (node != stop);

      // Remove.
      if (node != first) {
        BaseNode* last = (node != NULL) ? node->getPrev() : getCompiler()->getLastNode();
        getCompiler()->removeNodes(first, last);
      }
    }

    link = link->getNext();
  }

  return kErrorOk;
}

// ============================================================================
// [asmjit::BaseContext - Cleanup]
// ============================================================================

//! @internal
//!
//! @brief Translate the given function @a func.
void BaseContext::cleanup() {
  VarData** array = _contextVd.getData();
  size_t length = _contextVd.getLength();
  
  for (size_t i = 0; i < length; i++) {
    VarData* vd = array[i];
    vd->resetContextId();
    vd->resetRegIndex();
  }

  _contextVd.clear();
  _extraBlock = NULL;
}

// ============================================================================
// [asmjit::BaseContext - CompileFunc]
// ============================================================================

Error BaseContext::compile(FuncNode* func) {
  BaseNode* end = func->getEnd();
  BaseNode* stop = end->getNext();

  _func = func;
  _stop = stop;
  _extraBlock = end;

  ASMJIT_PROPAGATE_ERROR(fetch());
  ASMJIT_PROPAGATE_ERROR(removeUnreachableCode());
  ASMJIT_PROPAGATE_ERROR(analyze());
  ASMJIT_PROPAGATE_ERROR(translate());

  // We alter the compiler cursor, because it doesn't make sense to reference
  // it after compilation - some nodes may disappear and it's forbidden to add
  // new code after the compilation is done.
  _compiler->_setCursor(NULL);

  return kErrorOk;
}

} // asmjit namespace

// [Api-End]
#include "../base/apiend.h"
