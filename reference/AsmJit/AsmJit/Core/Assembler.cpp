// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

#define _ASMJIT_BEING_COMPILED

// [Dependencies - AsmJit]
#include "../Core/Assembler.h"
#include "../Core/MemoryManager.h"
#include "../Core/IntUtil.h"

// [Dependenceis - C]
#include <stdarg.h>

// [Api-Begin]
#include "../Core/ApiBegin.h"

namespace AsmJit {

// ============================================================================
// [AsmJit::Assembler - Construction / Destruction]
// ============================================================================

Assembler::Assembler(Context* context) ASMJIT_NOTHROW :
  _zoneMemory(16384 - sizeof(ZoneChunk) - 32),
  _buffer(),
  _context(context != NULL 
    ? context
    : static_cast<Context*>(JitContext::getGlobal())),
  _logger(NULL),
  _error(kErrorOk),
  _properties(0),
  _emitOptions(0),
  _trampolineSize(0),
  _inlineComment(NULL),
  _unusedLinks(NULL)
{
}

Assembler::~Assembler() ASMJIT_NOTHROW
{
}

// ============================================================================
// [AsmJit::Assembler - Logging]
// ============================================================================

void Assembler::setLogger(Logger* logger) ASMJIT_NOTHROW
{
  _logger = logger;
}

// ============================================================================
// [AsmJit::Assembler - Error Handling]
// ============================================================================

void Assembler::setError(uint32_t error) ASMJIT_NOTHROW
{
  _error = error;
  if (_error == kErrorOk)
    return;

  if (_logger)
    _logger->logFormat("*** ASSEMBLER ERROR: %s (%u).\n", getErrorString(error), (unsigned int)error);
}

// ============================================================================
// [AsmJit::Assembler - Properties]
// ============================================================================

uint32_t Assembler::getProperty(uint32_t propertyId) const ASMJIT_NOTHROW
{
  if (propertyId > 31)
    return 0;

  return (_properties & (IntUtil::maskFromIndex(propertyId))) != 0;
}

void Assembler::setProperty(uint32_t propertyId, uint32_t value) ASMJIT_NOTHROW
{
  if (propertyId > 31)
    return;

  if (value)
    _properties |= IntUtil::maskFromIndex(propertyId);
  else
    _properties &= ~IntUtil::maskFromIndex(propertyId);
}

// ============================================================================
// [AsmJit::Assembler - TakeCode]
// ============================================================================

uint8_t* Assembler::takeCode() ASMJIT_NOTHROW
{
  uint8_t* code = _buffer.take();
  _relocData.clear();
  _zoneMemory.clear();

  if (_error != kErrorOk)
    setError(kErrorOk);

  return code;
}

// ============================================================================
// [AsmJit::Assembler - Clear / Reset]
// ============================================================================

void Assembler::clear() ASMJIT_NOTHROW
{
  _purge();

  if (_error != kErrorOk)
    setError(kErrorOk);
}

void Assembler::reset() ASMJIT_NOTHROW
{
  _purge();

  _zoneMemory.reset();
  _buffer.reset();

  _labels.reset();
  _relocData.reset();

  if (_error != kErrorOk)
    setError(kErrorOk);
}

void Assembler::_purge() ASMJIT_NOTHROW
{
  _zoneMemory.clear();
  _buffer.clear();
 
  _emitOptions = 0;
  _trampolineSize = 0;

  _inlineComment = NULL;
  _unusedLinks = NULL;
  
  _labels.clear();
  _relocData.clear();
}

// ============================================================================
// [AsmJit::Assembler - Emit]
// ============================================================================

void Assembler::embed(const void* data, size_t len) ASMJIT_NOTHROW
{
  if (!canEmit())
    return;

  if (_logger)
  {
    size_t i, j;
    size_t max;

    char buf[128];
    char dot[] = ".data ";
    char* p;

    memcpy(buf, dot, ASMJIT_ARRAY_SIZE(dot) - 1);

    for (i = 0; i < len; i += 16)
    {
      max = (len - i < 16) ? len - i : 16;
      p = buf + ASMJIT_ARRAY_SIZE(dot) - 1;

      for (j = 0; j < max; j++)
        p += sprintf(p, "%02X", reinterpret_cast<const uint8_t *>(data)[i+j]);

      *p++ = '\n';
      *p = '\0';

      _logger->logString(buf);
    }
  }

  _buffer.emitData(data, len);
}

// ============================================================================
// [AsmJit::Assembler - Helpers]
// ============================================================================

Assembler::LabelLink* Assembler::_newLabelLink() ASMJIT_NOTHROW
{
  LabelLink* link = _unusedLinks;

  if (link)
  {
    _unusedLinks = link->prev;
  }
  else
  {
    link = (LabelLink*)_zoneMemory.alloc(sizeof(LabelLink));
    if (link == NULL) return NULL;
  }

  // clean link
  link->prev = NULL;
  link->offset = 0;
  link->displacement = 0;
  link->relocId = -1;

  return link;
}

} // AsmJit namespace

// [Api-End]
#include "../Core/ApiEnd.h"
