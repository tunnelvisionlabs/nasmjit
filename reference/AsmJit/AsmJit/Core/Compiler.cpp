// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

#define _ASMJIT_BEING_COMPILED

// [Dependencies - AsmJit]
#include "../Core/Assembler.h"
#include "../Core/Compiler.h"
#include "../Core/CompilerContext.h"
#include "../Core/CompilerFunc.h"
#include "../Core/CompilerItem.h"
#include "../Core/CpuInfo.h"
#include "../Core/IntUtil.h"
#include "../Core/Logger.h"

// [Api-Begin]
#include "../Core/ApiBegin.h"

namespace AsmJit {

// ============================================================================
// [AsmJit::Compiler - Construction / Destruction]
// ============================================================================

Compiler::Compiler(Context* context) ASMJIT_NOTHROW :
  _zoneMemory(16384 - sizeof(ZoneChunk) - 32),
  _linkMemory(1024 - 32),
  _context(context != NULL ? context : static_cast<Context*>(JitContext::getGlobal())),
  _logger(NULL),
  _error(0),
  _properties(0),
  _emitOptions(0),
  _finished(false),
  _first(NULL),
  _last(NULL),
  _current(NULL),
  _func(NULL),
  _cc(NULL),
  _varNameId(0)
{
}

Compiler::~Compiler() ASMJIT_NOTHROW
{
  reset();
}

// ============================================================================
// [AsmJit::Compiler - Logging]
// ============================================================================

void Compiler::setLogger(Logger* logger) ASMJIT_NOTHROW
{
  _logger = logger;
}

// ============================================================================
// [AsmJit::Compiler - Error Handling]
// ============================================================================

void Compiler::setError(uint32_t error) ASMJIT_NOTHROW
{
  _error = error;
  if (_error == kErrorOk)
    return;

  if (_logger)
    _logger->logFormat("*** COMPILER ERROR: %s (%u).\n", getErrorString(error), (unsigned int)error);
}

// ============================================================================
// [AsmJit::Compiler - Properties]
// ============================================================================

uint32_t Compiler::getProperty(uint32_t propertyId)
{
  if (propertyId > 31)
    return 0;

  return (_properties & IntUtil::maskFromIndex(propertyId)) != 0;
}

void Compiler::setProperty(uint32_t propertyId, uint32_t value)
{
  if (propertyId > 31)
    return;

  if (value)
    _properties |= IntUtil::maskFromIndex(propertyId);
  else
    _properties &= ~IntUtil::maskFromIndex(propertyId);
}

// ============================================================================
// [AsmJit::Compiler - Clear / Reset]
// ============================================================================

void Compiler::clear() ASMJIT_NOTHROW
{
  _purge();

  if (_error != kErrorOk)
    setError(kErrorOk);
}

void Compiler::reset() ASMJIT_NOTHROW
{
  _purge();

  _zoneMemory.reset();
  _linkMemory.reset();

  _targets.reset();
  _vars.reset();

  if (_error != kErrorOk)
    setError(kErrorOk);
}

void Compiler::_purge() ASMJIT_NOTHROW
{
  _zoneMemory.clear();
  _linkMemory.clear();

  _emitOptions = 0;
  _finished = false;

  _first = NULL;
  _last = NULL;
  _current = NULL;
  _func = NULL;

  _targets.clear();
  _vars.clear();

  _cc = NULL;
  _varNameId = 0;
}

// ============================================================================
// [AsmJit::Compiler - Item Management]
// ============================================================================

CompilerItem* Compiler::setCurrentItem(CompilerItem* item) ASMJIT_NOTHROW
{
  CompilerItem* old = _current;
  _current = item;
  return old;
}

void Compiler::addItem(CompilerItem* item) ASMJIT_NOTHROW
{
  ASMJIT_ASSERT(item != NULL);
  ASMJIT_ASSERT(item->_prev == NULL);
  ASMJIT_ASSERT(item->_next == NULL);

  if (_current == NULL)
  {
    if (_first == NULL)
    {
      _first = item;
      _last = item;
    }
    else
    {
      item->_next = _first;
      _first->_prev = item;
      _first = item;
    }
  }
  else
  {
    CompilerItem* prev = _current;
    CompilerItem* next = _current->_next;

    item->_prev = prev;
    item->_next = next;

    prev->_next = item;
    if (next)
      next->_prev = item;
    else
      _last = item;
  }

  _current = item;
}

void Compiler::addItemAfter(CompilerItem* item, CompilerItem* ref) ASMJIT_NOTHROW
{
  ASMJIT_ASSERT(item != NULL);
  ASMJIT_ASSERT(item->_prev == NULL);
  ASMJIT_ASSERT(item->_next == NULL);
  ASMJIT_ASSERT(ref != NULL);

  CompilerItem* prev = ref;
  CompilerItem* next = ref->_next;

  item->_prev = prev;
  item->_next = next;

  prev->_next = item;
  if (next)
    next->_prev = item;
  else
    _last = item;
}

void Compiler::removeItem(CompilerItem* item) ASMJIT_NOTHROW
{
  CompilerItem* prev = item->_prev;
  CompilerItem* next = item->_next;

  if (_first == item) { _first = next; } else { prev->_next = next; }
  if (_last  == item) { _last  = prev; } else { next->_prev = prev; }

  item->_prev = NULL;
  item->_next = NULL;

  if (_current == item)
    _current = prev;
}

// ============================================================================
// [AsmJit::Compiler - Comment]
// ============================================================================

void Compiler::comment(const char* fmt, ...) ASMJIT_NOTHROW
{
  char buf[128];
  char* p = buf;

  if (fmt)
  {
    *p++ = ';';
    *p++ = ' ';

    va_list ap;
    va_start(ap, fmt);
    p += vsnprintf(p, 100, fmt, ap);
    va_end(ap);
  }

  *p++ = '\n';
  *p   = '\0';

  CompilerComment* item = Compiler_newItem<CompilerComment>(this, buf);
  addItem(item);
}

// ============================================================================
// [AsmJit::Compiler - Embed]
// ============================================================================

void Compiler::embed(const void* data, size_t len) ASMJIT_NOTHROW
{
  // Align length to 16 bytes.
  size_t alignedSize = IntUtil::align<size_t>(len, sizeof(uintptr_t));
  void* p = _zoneMemory.alloc(sizeof(CompilerEmbed) - sizeof(void*) + alignedSize);

  if (p == NULL)
    return;

  CompilerEmbed* item = new(p) CompilerEmbed(this, data, len);
  addItem(item);
}

} // AsmJit namespace

// [Api-Begin]
#include "../Core/ApiBegin.h"
