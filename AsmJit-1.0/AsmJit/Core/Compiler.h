// [AsmJit]
// Complete JIT Assembler for C++ Language.
//
// [License]
// Zlib - See COPYING file in this package.

// [Guard]
#ifndef _ASMJIT_CORE_COMPILER_H
#define _ASMJIT_CORE_COMPILER_H

// [Dependencies - AsmJit]
#include "../Core/Assembler.h"
#include "../Core/Context.h"
#include "../Core/Func.h"
#include "../Core/Operand.h"
#include "../Core/PodVector.h"

// [Api-Begin]
#include "../Core/ApiBegin.h"

namespace AsmJit {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Compiler;
struct CompilerAlign;
struct CompilerComment;
struct CompilerContext;
struct CompilerEmbed;
struct CompilerFuncCall;
struct CompilerFuncDecl;
struct CompilerFuncEnd;
struct CompilerInst;
struct CompilerItem;
struct CompilerMark;
struct CompilerState;
struct CompilerTarget;
struct CompilerVar;

// ============================================================================
// [AsmJit::CompilerState]
// ============================================================================

//! @brief Compiler state base.
struct CompilerState
{
};

// ============================================================================
// [AsmJit::CompilerVar]
// ============================================================================

//! @brief Compiler variable base.
struct CompilerVar
{
};

// ============================================================================
// [AsmJit::Compiler]
// ============================================================================

//! @brief Compiler.
//!
//! @sa @ref Assembler.
struct Compiler
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a new @ref Compiler instance.
  ASMJIT_API Compiler(Context* context) ASMJIT_NOTHROW;
  //! @brief Destroy the @ref Compiler instance.
  ASMJIT_API virtual ~Compiler() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Context]
  // --------------------------------------------------------------------------

  //! @brief Get code generator.
  inline Context* getContext() const ASMJIT_NOTHROW
  { return _context; }

  // --------------------------------------------------------------------------
  // [Memory Management]
  // --------------------------------------------------------------------------

  //! @brief Get zone memory manager.
  inline ZoneMemory& getZoneMemory() ASMJIT_NOTHROW
  { return _zoneMemory; }

  //! @brief Get link memory manager.
  inline ZoneMemory& getLinkMemory() ASMJIT_NOTHROW
  { return _linkMemory; }

  // --------------------------------------------------------------------------
  // [Logging]
  // --------------------------------------------------------------------------

  //! @brief Get logger.
  inline Logger* getLogger() const ASMJIT_NOTHROW
  { return _logger; }

  //! @brief Set logger to @a logger.
  ASMJIT_API virtual void setLogger(Logger* logger) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Error Handling]
  // --------------------------------------------------------------------------

  //! @brief Get error code.
  inline uint32_t getError() const ASMJIT_NOTHROW
  { return _error; }

  //! @brief Set error code.
  //!
  //! This method is virtual, because higher classes can use it to catch all
  //! errors.
  ASMJIT_API virtual void setError(uint32_t error) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  //! @brief Get compiler property.
  ASMJIT_API virtual uint32_t getProperty(uint32_t propertyId);
  //! @brief Set compiler property.
  ASMJIT_API virtual void setProperty(uint32_t propertyId, uint32_t value);

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  //! @brief Clear everything, but not deallocate buffers.
  //!
  //! @note This method will destroy your code.
  ASMJIT_API void clear() ASMJIT_NOTHROW;

  //! @brief Free internal buffer, all emitters and NULL all pointers.
  //!
  //! @note This method will destroy your code.
  ASMJIT_API void reset() ASMJIT_NOTHROW;

  //! @brief Called by clear() and reset() to clear all data related to derived
  //! class implementation.
  ASMJIT_API virtual void _purge() ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Item Management]
  // --------------------------------------------------------------------------

  //! @brief Get first item.
  inline CompilerItem* getFirstItem() const ASMJIT_NOTHROW
  { return _first; }

  //! @brief Get last item.
  inline CompilerItem* getLastItem() const ASMJIT_NOTHROW
  { return _last; }

  //! @brief Get current item.
  //!
  //! @note If this method returns @c NULL it means that nothing has been 
  //! emitted yet.
  inline CompilerItem* getCurrentItem() const ASMJIT_NOTHROW
  { return _current; }

  //! @brief Get current function.
  inline CompilerFuncDecl* getFunc() const ASMJIT_NOTHROW
  { return _func; }

  //! @brief Set current item to @a item and return the previous current one.
  ASMJIT_API CompilerItem* setCurrentItem(CompilerItem* item) ASMJIT_NOTHROW;

  //! @brief Add item after current item to @a item and set current item to 
  //! @a item.
  ASMJIT_API void addItem(CompilerItem* item) ASMJIT_NOTHROW;

  //! @brief Add item after @a ref.
  ASMJIT_API void addItemAfter(CompilerItem* item, CompilerItem* ref) ASMJIT_NOTHROW;

  //! @brief Remove item @a item.
  ASMJIT_API void removeItem(CompilerItem* item) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Comment]
  // --------------------------------------------------------------------------

  //! @brief Emit a single comment line.
  //!
  //! @note Comment is not directly sent to logger, but instead it's stored as
  //! @ref CompilerComment item emitted when @c serialize() method is called.
  ASMJIT_API void comment(const char* fmt, ...) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Embed]
  // --------------------------------------------------------------------------

  //! @brief Embed data.
  ASMJIT_API void embed(const void* data, size_t len) ASMJIT_NOTHROW;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief ZoneMemory allocator, used to allocate compiler items.
  ZoneMemory _zoneMemory;
  //! @brief ZoneMemory allocator, used to alloc small data structures like
  //! linked lists.
  ZoneMemory _linkMemory;

  //! @brief Context.
  Context* _context;
  //! @brief Logger.
  Logger* _logger;

  //! @brief Error code.
  uint32_t _error;
  //! @brief Properties.
  uint32_t _properties;
  //! @brief Contains options for next emitted instruction, clear after each emit.
  uint32_t _emitOptions;
  //! @brief Whether compiler was finished the job (register allocator, etc...).
  uint32_t _finished;

  //! @brief First item.
  CompilerItem* _first;
  //! @brief Last item.
  CompilerItem* _last;
  //! @brief Current item.
  CompilerItem* _current;
  //! @brief Current function.
  CompilerFuncDecl* _func;

  //! @brief Targets.
  PodVector<CompilerTarget*> _targets;
  //! @brief Variables.
  PodVector<CompilerVar*> _vars;

  //! @brief Compiler context instance, only available after prepare().
  CompilerContext* _cc;

  //! @brief Variable name id (used to generate unique names per function).
  int _varNameId;
};

// ============================================================================
// [AsmJit::Compiler - Helpers]
// ============================================================================

template<typename T, typename Compiler>
inline T* Compiler_newObject(Compiler* self) ASMJIT_NOTHROW
{
  void* addr = self->getZoneMemory().alloc(sizeof(T));
  return new(addr) T(self);
}

template<typename T, typename Compiler, typename P1>
inline T* Compiler_newObject(Compiler* self, P1 p1) ASMJIT_NOTHROW
{
  void* addr = self->getZoneMemory().alloc(sizeof(T));
  return new(addr) T(self, p1);
}

template<typename T, typename Compiler, typename P1, typename P2>
inline T* Compiler_newObject(Compiler* self, P1 p1, P2 p2) ASMJIT_NOTHROW
{
  void* addr = self->getZoneMemory().alloc(sizeof(T));
  return new(addr) T(self, p1, p2);
}

template<typename T, typename Compiler, typename P1, typename P2, typename P3>
inline T* Compiler_newObject(Compiler* self, P1 p1, P2 p2, P3 p3) ASMJIT_NOTHROW
{
  void* addr = self->getZoneMemory().alloc(sizeof(T));
  return new(addr) T(self, p1, p2, p3);
}

template<typename T, typename Compiler, typename P1, typename P2, typename P3, typename P4>
inline T* Compiler_newObject(Compiler* self, P1 p1, P2 p2, P3 p3, P4 p4) ASMJIT_NOTHROW
{
  void* addr = self->getZoneMemory().alloc(sizeof(T));
  return new(addr) T(self, p1, p2, p3, p4);
}

template<typename T, typename Compiler, typename P1, typename P2, typename P3, typename P4, typename P5>
inline T* Compiler_newObject(Compiler* self, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) ASMJIT_NOTHROW
{
  void* addr = self->getZoneMemory().alloc(sizeof(T));
  return new(addr) T(self, p1, p2, p3, p4, p5);
}

} // AsmJit namespace

// [Api-End]
#include "../Core/ApiEnd.h"

// [Guard]
#endif // _ASMJIT_CORE_COMPILER_H
