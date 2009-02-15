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

// [Guard]
#ifndef _ASMJITASSEMBLERX86X64_H
#define _ASMJITASSEMBLERX86X64_H

// [Dependencies]
#include "AsmJitConfig.h"
#include "AsmJitSerializer.h"
#include "AsmJitUtil.h"

// [AsmJit::]
namespace AsmJit {

//! @addtogroup AsmJit_Assembler
//! @{

// Forward Declarations
struct Displacement;
struct Label;
struct Assembler; 

// Helpers

//! @brief BitField is a help template for encoding and decode bitfield with
//! unsigned content.
//!
//! @internal
template<class T, int shift, int size>
struct BitField
{
  // Tells whether the provided value fits into the bit field.
  static bool isValid(T value) {
    return (static_cast<SysUInt>(value) & ~(((SysUInt)1U << (size)) - 1)) == 0;
  }

  // Returns a UInt32 mask of bit field.
  static SysUInt mask() {
    return (1U << (size + shift)) - ((SysUInt)1U << shift);
  }

  // Returns a UInt32 with the bit field value encoded.
  static SysUInt encode(T value)
  {
    ASMJIT_ASSERT(isValid(value));
    return static_cast<SysUInt>(value) << shift;
  }

  // Extracts the bit field from the value.
  static T decode(SysUInt value)
  {
    return static_cast<T>((value >> shift) & (((SysUInt)1U << (size)) - (SysUInt)1));
  }
};

//! @brief Displacement.
//!
//! A Displacement describes the 32bit immediate field of an instruction which
//! may be used together with a Label in order to refer to a yet unknown code
//! position. Displacements stored in the instruction stream are used to describe
//! the instruction and to chain a list of instructions using the same Label.
//! A Displacement contains 2 different fields:
//!
//! next field: position of next displacement in the chain (0 = end of list)
//! type field: instruction type
//!
//! A next value of null (0) indicates the end of a chain (note that there can
//! be no displacement at position zero, because there is always at least one
//! instruction byte before the displacement).
//!
//! Displacement _data field layout
//!
//! |31.....1| ......0|
//! [  next  |  type  |
//!
//! @internal
struct Displacement
{
  enum Type
  {
    UNCONDITIONAL_JUMP,
    OTHER
  };

  explicit Displacement(SysInt data)
  {
    _data = data;
  }

  Displacement(Label* L, Type type)
  {
    init(L, type);
  }

  inline SysInt data() const
  {
    return _data;
  }

  inline Type type() const
  {
    return TypeField::decode(_data);
  }

  inline void next(Label* L) const;
  inline void linkTo(Label* L);

private:
  SysInt _data;

  struct TypeField: public BitField<Type, 0, 1> {};
  struct NextField: public BitField<SysInt,  1, ((sizeof(SysInt)*8)-1)> {};

  inline void init(Label* L, Type type);

  friend struct Label;
  friend struct Assembler;
};

//! @brief Labels represent pc locations.
//!
//! They are typically jump or call targets. After declaration, a label 
//! can be freely used to denote known or (yet) unknown pc location. 
//! AsmJit::Assembler::bind() is used to bind a label to the current pc.
//!
//! @note A label can be bound only once.
struct Label
{
  //! @brief Creates new unused label.
  inline Label() { unuse(); }
  //! @brief Destroys label. If label is linked to some location, assertion is raised.
  inline ~Label() { ASMJIT_ASSERT(!isLinked()); }

  //! @brief Returns Unuse label (unbound or unlink).
  inline void unuse() { _pos = 0; }

  //! @brief Returns @c true if label is bound.
  inline bool isBound()  const { return _pos <  0; }
  //! @brief Returns @c true if label is unused (not bound or linked).
  inline bool isUnused() const { return _pos == 0; }
  //! @brief Returns @c true if label is linked.
  inline bool isLinked() const { return _pos >  0; }

  //! @brief Returns the position of bound or linked labels. Cannot be 
  //! used for unused labels.
  inline SysInt pos() const
  {
    if (_pos < 0) return -_pos - 1;
    if (_pos > 0) return  _pos - 1;
    ASMJIT_ASSERT(0);
    return 0;
  }

private:
  //! pos_ encodes both the binding state (via its sign)
  //! and the binding position (via its value) of a label.
  //!
  //! pos_ <  0  bound label, pos() returns the jump target position
  //! pos_ == 0  unused label
  //! pos_ >  0  linked label, pos() returns the last reference position
  SysInt _pos;

  //! @brief Bind label to position @a pos.
  inline void bindTo(SysInt pos) 
  {
    _pos = -pos - 1;
    ASMJIT_ASSERT(isBound());
  }

  //! @brief Link label to position @a pos.
  inline void linkTo(SysInt pos)
  {
    _pos =  pos + 1;
    ASMJIT_ASSERT(isLinked());
  }

  friend struct Displacement;
  friend struct Assembler;
};

inline void Displacement::next(Label* L) const
{
  SysInt n = NextField::decode(_data);
  n > 0 ? L->linkTo(n) : L->unuse();
}

inline void Displacement::linkTo(Label* L)
{
  init(L, type());
}

inline void Displacement::init(Label* L, Type type)
{
  ASMJIT_ASSERT(!L->isBound());

  SysInt next = 0;
  if (L->isLinked())
  {
    next = L->pos();
    // Displacements must be at positions > 0
    ASMJIT_ASSERT(next > 0);
  }

  // Ensure that we _never_ overflow the next field.
  // ASMJIT_ASSERT(NextField::isValid(Assembler::kMaximalBufferSize));
  _data = NextField::encode(next) | TypeField::encode(type);
}

//! @brief X86/X64 Assembler.
//!
//! This class is the main class in AsmJit for generating X86/X64 binary. In
//! creates internal buffer, where opcodes are stored (don't worry about it,
//! it's auto growing buffer) and contains methods for generating opcodes
//! with compile time and runtime time (DEBUG) checks. Buffer is allocated 
//! first time you emit instruction. Newly constructed instance never allocates
//! any memory (in constructor).
//!
//! Buffer is allocated by @c ASMJIT_MALLOC, @c ASMJIT_REALLOC and 
//! freed by @c ASMJIT_FREE macros. It's designed to fit your memory 
//! management model and not to dictate you own. Default functions are from
//! standard C library - malloc, realloc and free.
//!
//! While you are over from emitting instructions, you can get size of code
//! by codeSize() or offset() methods. These methods returns you code size
//! (or more precisely current code offset) in bytes. Use takeCode() to take
//! internal buffer (all pointers in Assembler instance will be zeroed and 
//! current buffer returned) to use it. If you don't take it, Assembler 
//! destructor will free it. To run code, don't use malloc()'ed memory, but
//! use @c AsmJit::VM::alloc() to get memory for executing (specify
//! canExecute to true). Code generated by Assembler can be relocated to that 
//! buffer and called.
//!
//! To generate instruction stream, use methods provided by @c Assembler 
//! class. For example to generate X86 function, you need to do this:
//!
//! @verbatim
//! using namespace AsmJit;
//!
//! // X86/X64 assembler
//! Assembler a;
//!
//! // int f() - prolog
//! a.push(ebp);
//! a.mov(ebp, esp);
//!
//! // Mov 1024 to EAX, EAX is also return value.
//! a.mov(eax, imm(1024));
//!
//! // int f() - epilog
//! a.mov(esp, ebp);
//! a.pop(ebp);
//! a.ret();
//!
//! @endverbatim
//!
//! Code generated by this function can be executed by this way:
//!
//! @verbatim
//! // Alloc execute enabled memory and call generated function, vsize will
//! // contain size of allocated virtual memory block.
//! SysInt vsize;
//! void *vmem = VM::alloc(a.codeSize(), &vsize, true /* canExecute */);
//!
//! // Relocate code to vmem.
//! a.relocCode(vmem);
//!
//! // Cast vmem to void() function and call it
//! reinterpret_cast<void (*)()>(vmem)();
//!
//! // Memory should be freed, but use VM::free() to do that.
//! VM::free(vmem, vsize);
//! @endverbatim
//!
//! NOTE: This was very primitive example how to call generated code. In
//! real production code you will never alloc and free code for one run,
//! you will alloc memory, where you copy generated code, and you will run
//! it many times from that place.
//!
//! Using labels
//!
//! While generating assembler code, you will usually need to create complex
//! code with labels. Labels are fully supported and you can call jump or j..
//! instructions to yet uninitialized label. Each label expects to be bound
//! into offset. To bind label to specific offset, use @c bind() methos.
//!
//! @verbatim
//! // Example: Usage of Label (32 bit code)
//! //
//! // Create simple DWORD memory copy function:
//! // STDCALL void copy32(void* DST, const void* SRC, sysuint_t COUNT);
//! using namespace AsmJit;
//!
//! // X86/X64 assembler
//! Assembler a;
//! 
//! // Constants
//! const int arg_offset = 8; // Arguments offset (STDCALL EBP)
//! const int arg_size = 12;  // Arguments size
//! 
//! // Labels
//! Label L_Loop;
//! 
//! // Prolog
//! a.push(ebp);
//! a.mov(ebp, esp);
//! a.push(esi);
//! a.push(edi);
//! 
//! // Fetch arguments
//! a.mov(esi, dword_ptr(ebp, arg_offset + 0)); // get DST
//! a.mov(edi, dword_ptr(ebp, arg_offset + 4)); // get SRC
//! a.mov(ecx, dword_ptr(ebp, arg_offset + 8)); // get COUNT
//! 
//! // Bind L_Loop label to here
//! a.bind(&L_Loop);
//! 
//! a.mov(eax, dword_ptr(edi));
//! a.mov(dword_ptr(esi), eax);
//! 
//! // Repeat loop until ecx == 0
//! a.dec(ecx);
//! // See @c CONDITION codes, if ecx != 0, jump to L_Loop
//! a.j(C_ZERO, &L_Loop);
//! 
//! // Epilog
//! a.pop(edi);
//! a.pop(esi);
//! a.mov(esp, ebp);
//! a.pop(ebp);
//! 
//! // Return: STDCALL convention is to pop stack in called function
//! a.ret(arg_size);
//! @endverbatim
struct ASMJIT_API Assembler : public Serializer
{
  // -------------------------------------------------------------------------
  // [Construction / Destruction]
  // -------------------------------------------------------------------------

  //! @brief Creates Assembler instance.
  Assembler();
  //! @brief Destroys Assembler instance
  virtual ~Assembler();

  // -------------------------------------------------------------------------
  // [Buffer Getters / Setters]
  // -------------------------------------------------------------------------

  //! @brief Return start of assembler code buffer.
  inline UInt8* code() const { return _buffer.data(); }

  //! @brief Ensure space for next instruction
  inline bool ensureSpace() { return _buffer.ensureSpace(); }

  //! @brief Return size of currently generated code.
  inline SysInt codeSize() const { return _buffer.offset(); }

  //! @brief Return current offset in buffer (same as codeSize()).
  inline SysInt offset() const { return _buffer.offset(); }

  //! @brief Sets offset to @a o and returns previous offset.
  //!
  //! This method can be used to truncate code (previous offset is not
  //! recorded) or to overwrite instruction stream at position @a o.
  inline SysInt toOffset(SysInt o) { return _buffer.toOffset(o); }

  //! @brief Return capacity of internal code buffer.
  inline SysInt capacity() const { return _buffer.capacity(); }

  //! @brief Reallocate internal buffer.
  //!
  //! It's only used for growing, buffer is never reallocated to smaller 
  //! number than current capacity() is.
  bool realloc(SysInt to);

  //! @brief Used to grow the buffer.
  //!
  //! It will typically realloc to twice size of capacity(), but if capacity()
  //! is large, it will use smaller steps.
  bool grow();

  //! @brief Clear everything, but not deallocate buffers.
  void clear();

  //! @brief Free internal buffer and NULL all pointers.
  void free();

  //! @brief Return internal buffer and NULL all pointers.
  UInt8* takeCode();

  // -------------------------------------------------------------------------
  // [Stream Setters / Getters]
  // -------------------------------------------------------------------------

  //! @brief Set byte at position @a pos.
  inline UInt8 getByteAt(SysInt pos) { return _buffer.getByteAt(pos); }
  //! @brief Set word at position @a pos.
  inline UInt16 getWordAt(SysInt pos) { return _buffer.getWordAt(pos); }
  //! @brief Set word at position @a pos.
  inline UInt32 getDWordAt(SysInt pos) { return _buffer.getDWordAt(pos); }
  //! @brief Set word at position @a pos.
  inline UInt64 getQWordAt(SysInt pos) { return _buffer.getQWordAt(pos); }

  // TODO: Remove
  //! @brief Return integer (dword size) at position @a pos.
  inline UInt32 getIntAt(SysInt pos)
  {
    return *reinterpret_cast<UInt32*>(_buffer._data + pos);
  }

  //! @brief Displacement at position @a pos.
  inline Displacement getDispAt(Label* L)
  {
    return Displacement(getIntAt(L->pos()));
  }

  //! @brief Set byte at position @a pos.
  inline void setByteAt(SysInt pos, UInt8 x) { _buffer.setByteAt(pos, x); }
  //! @brief Set word at position @a pos.
  inline void setWordAt(SysInt pos, UInt16 x) { _buffer.setWordAt(pos, x); }
  //! @brief Set word at position @a pos.
  inline void setDWordAt(SysInt pos, UInt32 x) { _buffer.setDWordAt(pos, x); }
  //! @brief Set word at position @a pos.
  inline void setQWordAt(SysInt pos, UInt64 x) { _buffer.setQWordAt(pos, x); }

  //! @brief Set custom variable @a imm at position @a pos.
  //!
  //! @note This function is used to patch existing code.
  void setVarAt(SysInt pos, SysInt i, UInt8 isUnsigned, UInt32 size);

  //! @brief Set displacement at label @a L position.
  inline void setDispAt(Label* L, Displacement disp)
  {
    setDWordAt(L->pos(), (UInt32)disp.data());
  }

  // -------------------------------------------------------------------------
  // [Assembler Emitters]
  //
  // These emitters are not protecting buffer from overrun, this must be 
  // done for each instruction by this:
  //   if (!ensureSpace()) return;
  // -------------------------------------------------------------------------

  //! @brief Emit Byte to internal buffer.
  inline void _emitByte(UInt8 x) { _buffer.emitByte(x); }
  //! @brief Emit Word (2 bytes) to internal buffer.
  inline void _emitWord(UInt16 x) { _buffer.emitWord(x); }
  //! @brief Emit DWord (4 bytes) to internal buffer.
  inline void _emitDWord(UInt32 x) { _buffer.emitDWord(x); }
  //! @brief Emit QWord (8 bytes) to internal buffer.
  inline void _emitQWord(UInt64 x) { _buffer.emitQWord(x); }

  void _emitImmediate(const Immediate& imm, UInt32 size);

  //! @brief Emit single @a opCode without operands.
  inline void _emitOpCode(UInt32 opCode)
  {
    // instruction prefix
    if (opCode & 0xFF000000) _emitByte((UInt8)((opCode & 0xFF000000) >> 24));
    // instruction opcodes
    if (opCode & 0x00FF0000) _emitByte((UInt8)((opCode & 0x00FF0000) >> 16));
    if (opCode & 0x0000FF00) _emitByte((UInt8)((opCode & 0x0000FF00) >>  8));
    // last opcode is always emitted (can be also 0x00)
    _emitByte((UInt8)(opCode & 0x000000FF));
  }

  //! @brief Emit MODR/M byte.
  inline void _emitMod(UInt8 m, UInt8 o, UInt8 r)
  { _emitByte(((m & 0x03) << 6) | ((o & 0x07) << 3) | (r & 0x07)); }

  //! @brief Emit SIB byte.
  inline void _emitSib(UInt8 s, UInt8 i, UInt8 b)
  { _emitByte(((s & 0x03) << 6) | ((i & 0x07) << 3) | (b & 0x07)); }

  //! @brief Emit REX prefix (64 bit mode only).
  inline void _emitRexR(UInt8 w, UInt8 opReg, UInt8 regCode)
  {
#if defined(ASMJIT_X64)
    UInt8 r = (opReg & 0x8) != 0;
    UInt8 b = (regCode & 0x8) != 0;

    // w Default operand size(0=Default, 1=64 bits).
    // r Register field (1=high bit extension of the ModR/M REG field).
    // x Index field not used in RexR
    // b Base field (1=high bit extension of the ModR/M or SIB Base field).
    if (w || r || b)
    {
      _emitByte(0x40 | (w << 3) | (r << 2) | b);
    }
#else
    ASMJIT_USE(w);
    ASMJIT_USE(opReg);
    ASMJIT_USE(regCode);
#endif // ASMJIT_X64
  }

  //! @brief Emit REX prefix (64 bit mode only).
  inline void _emitRexRM(UInt8 w, UInt8 opReg, const RegMem& rm)
  {
#if defined(ASMJIT_X64)
    UInt8 r = (opReg & 0x8) != 0;
    UInt8 x = 0;
    UInt8 b = 0;

    if (rm.isReg())
    {
      b = (reinterpret_cast<const BaseReg&>(rm).code() & 0x8) != 0;
    }
    else if (rm.isMem())
    {
      x = (reinterpret_cast<const Mem&>(rm).index() & 0x8) != 0;
      b = (reinterpret_cast<const Mem&>(rm).base() & 0x8) != 0;
    }

    // w Default operand size(0=Default, 1=64 bits).
    // r Register field (1=high bit extension of the ModR/M REG field).
    // x Index field (1=high bit extension of the SIB Index field).
    // b Base field (1=high bit extension of the ModR/M or SIB Base field).
    if (w || r || x || b)
    {
      _emitByte(0x40 | (w << 3) | (r << 2) | (x << 1) | b);
    }
#else
    ASMJIT_USE(w);
    ASMJIT_USE(opReg);
    ASMJIT_USE(rm);
#endif // ASMJIT_X64
  }

  //! @brief Emit Register / Register - calls _emitMod(3, opReg, r)
  inline void _emitModR(UInt8 opReg, UInt8 r)
  { _emitMod(3, opReg, r); }

  //! @brief Emit Register / Register - calls _emitMod(3, opReg, r.code())
  inline void _emitModR(UInt8 opReg, const BaseReg& r)
  { _emitMod(3, opReg, r.code()); }

  //! @brief Emit register / memory address combination to buffer.
  //!
  //! This method can hangle addresses from simple to complex ones with
  //! index and displacement.
  void _emitModM(UInt8 opReg, const Mem& mem);

  //! @brief Emit Reg<-Reg or Reg<-Reg|Mem ModRM (can be followed by SIB 
  //! and displacement) to buffer.
  //!
  //! This function internally calls @c _emitModM() or _emitModR() that depends
  //! to @a op type.
  //!
  //! @note @a opReg is usually real register ID (see @c R) but some instructions
  //! have specific format and in that cases @a opReg is part of opcode.
  void _emitModRM(UInt8 opReg, const RegMem& op);

  void _emitX86Inl(UInt32 opCode, UInt8 i16bit, UInt8 rexw, UInt8 reg);
  void _emitX86RM(UInt32 opCode, UInt8 i16bit, UInt8 rexw, UInt8 o, const RegMem& op);

  void _emitFpu(UInt32 opCode);
  void _emitFpuSTI(UInt32 opCode, UInt32 sti);
  void _emitFpuMEM(UInt32 opCode, UInt8 opReg, const Mem& mem);

  void _emitMmu(UInt32 opCode, UInt8 rexw, UInt8 opReg, const RegMem& src);

  void _emitDisp(Label* L, Displacement::Type type);

  // -------------------------------------------------------------------------
  // [Bind and Labels Declaration]
  // -------------------------------------------------------------------------

  //! @brief Bind label to the current offset.
  //!
  //! @note Label can be bound only once!
  void bind(Label* L);

  //! @brief Bind label to pos - called from bind(Label*L).
  void bindTo(Label* L, SysInt pos);

  //! @brief link label, called internally from jumpers.
  void linkTo(Label* L, Label* appendix);

  // -------------------------------------------------------------------------
  // [Relocation helpers]
  // -------------------------------------------------------------------------

  //! @brief Relocate code to a given address @a dst.
  //!
  //! A given buffer will be overwritten, to get number of bytes required use
  //! @c codeSize() or @c offset() methods.
  void relocCode(void* dst) const;

  //! @internal
  bool writeRelocInfo(const Relocable& immediate, SysUInt relocOffset, UInt8 relocSize);

  //! @brief Overwrites emitted immediate to a new value.
  void overwrite(const Relocable& immediate);

  // -------------------------------------------------------------------------
  // [Abstract Emitters]
  // -------------------------------------------------------------------------

  virtual void _emitX86(UInt32 code, const Operand* o1 = NULL, const Operand* o2 = NULL, const Operand* o3 = NULL);
  virtual void _emitX86Align(SysInt m);
  virtual void _emitX86Call(Label* L);
  virtual void _emitX86J(CONDITION cc, Label* L, HINT hint);
  virtual void _emitX86Jmp(Label* L);

  // -------------------------------------------------------------------------
  // [Variables]
  // -------------------------------------------------------------------------

  //! @brief Binary code buffer.
  Buffer _buffer;

  //! @brief List of relocations.
  PodVector<RelocInfo> _relocations;
};

//! @}

} // AsmJit namespace

// [Guard]
#endif // _ASMJITASSEMBLERX86X64_H
