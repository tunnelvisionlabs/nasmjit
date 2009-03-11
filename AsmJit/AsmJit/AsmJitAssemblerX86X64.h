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
#include "AsmJitBuild.h"
#include "AsmJitSerializer.h"
#include "AsmJitUtil.h"

// [AsmJit::]
namespace AsmJit {

//! @addtogroup AsmJit_Assembler
//! @{

//! @brief Assembler - low level code generation.
//!
//! @c Assembler is the main class in AsmJit for generating low level x86/x64 
//! binary. It creates internal buffer, where opcodes are stored and contains 
//! methods that mimics x86/x64 assembler instructions. Because 
//! @c AsmJit::Assembler is based on @c AsmJit::Serializer it supports also
//! type checking by C++ compiler. It's nearly impossible to create invalid 
//! instruction (for example <code>mov [eax], [eax]</code>).
//!
//! Each call to assembler intrinsics directly emits instruction to internal
//! binary stream. Instruction emitting also contains runtime checks so it's
//! impossible to create instruction that is not valid.
//!
//! @c AsmJit::Assembler contains internal buffer where all emitted 
//! instructions are stored. Look at @c AsmJit::Buffer for buffer 
//! implementation. You should always use @c relocCode() method to relocate
//! emitted code into location allocated by AsmJit::VM::alloc(), but you can
//! also use @c AsmJit::Assembler::take() method to take this code.
//!
//! <b>Code Generation</b>
//!
//! To generate code is only needed to create instance of @c AsmJit::Assembler
//! and to use intrinsics. See example how to do that:
//! 
//! @verbatim
//! // Use AsmJit namespace
//! using namespace AsmJit;
//!
//! // Create Assembler instance
//! Assembler a;
//!
//! // Prolog
//! a.push(ebp);
//! a.mov(ebp, esp);
//!
//! // Mov 1024 to EAX, EAX is also return value.
//! a.mov(eax, imm(1024));
//!
//! // Epilog
//! a.mov(esp, ebp);
//! a.pop(ebp);
//! a.ret();
//! @endverbatim
//!
//! You can see that syntax is very close to Intel one. Only difference is that
//! you are calling functions that emits the binary code for you. All registers
//! are in @c AsmJit namespace, so it's very comfortable to use it (look at
//! first line). There is also used method @c AsmJit::imm() to create immediate
//! value. Use @c AsmJit::uimm() to create unsigned immediate value.
//!
//! There is also possibility for use memory addresses and immediates. To build
//! memory address use @c ptr(), @c byte_ptr(), @c word_ptr(), @c dword_ptr()
//! and similar methods. In most cases you needs only @c ptr() method, but 
//! there are instructions where you must specify address size.
//!
//! for example (a is @c AsmJit::Assembler instance):
//!
//! @verbatim
//! a.mov(ptr(eax), imm(0));                   // mov ptr [eax], 0
//! a.mov(ptr(eax), edx);                      // mov ptr [eax], edx
//! @endverbatim
//!
//! But it's also possible to create complex addresses: 
//!
//! @verbatim
//! // eax + ecx*x addresses
//! a.mov(ptr(eax, ecx, TIMES_1), imm(0));     // mov ptr [eax + ecx], 0
//! a.mov(ptr(eax, ecx, TIMES_2), imm(0));     // mov ptr [eax + ecx * 2], 0
//! a.mov(ptr(eax, ecx, TIMES_3), imm(0));     // mov ptr [eax + ecx * 4], 0
//! a.mov(ptr(eax, ecx, TIMES_4), imm(0));     // mov ptr [eax + ecx * 8], 0
//! // eax + ecx*x + disp addresses
//! a.mov(ptr(eax, ecx, TIMES_1,  4), imm(0)); // mov ptr [eax + ecx     +  4], 0
//! a.mov(ptr(eax, ecx, TIMES_2,  8), imm(0)); // mov ptr [eax + ecx * 2 +  8], 0
//! a.mov(ptr(eax, ecx, TIMES_3, 12), imm(0)); // mov ptr [eax + ecx * 4 + 12], 0
//! a.mov(ptr(eax, ecx, TIMES_4, 16), imm(0)); // mov ptr [eax + ecx * 8 + 16], 0
//! @endverbatim
//!
//! All addresses shown are using @c AsmJit::ptr() to make memory operand.
//! Some assembler instructions (single operand ones) needs to specify memory
//! operand size. For example calling <code>a.inc(ptr(eax))</code> can't be
//! used. @c AsmJit::Assembler::inc(), @c AsmJit::Assembler::dec() and similar
//! instructions can't be serialized without specifying how bytes they are
//! operating on. See next code how assembler works:
//!
//! @verbatim
//! // [byte] address
//! a.inc(byte_ptr(eax));                      // inc byte ptr [eax]
//! a.dec(byte_ptr(eax));                      // dec byte ptr [eax]
//! // [word] address
//! a.inc(word_ptr(eax));                      // inc word ptr [eax]
//! a.dec(word_ptr(eax));                      // dec word ptr [eax]
//! // [dword] address
//! a.inc(dword_ptr(eax));                     // inc dword ptr [eax]
//! a.dec(dword_ptr(eax));                     // dec dword ptr [eax]
//! @endverbatim
//!
//! <b>Calling Code</b>
//!
//! While you are over from emitting instructions, you can get size of code
//! by @c codeSize() or @c offset() methods. These methods returns you code
//! size (or more precisely current code offset) in bytes. Use takeCode() to
//! take internal buffer (all pointers in @c AsmJit::Assembler instance will
//! be zeroed and current buffer returned) to use it. If you don't take it, 
//! @c AsmKit::Assembler destructor will free it. To run code, don't use 
//! @c malloc()'ed memory, but instead use @c AsmJit::VM::alloc() to get 
//! memory for executing (specify @c canExecute to @c true). Code generated 
//! by @c AsmJit::Assembler can be relocated to that buffer and called.
//!
//! See next example how to allocate memory where you can execute code created
//! by @c AsmJit::Assembler:
//!
//! @verbatim
//! using namespace AsmJit;
//!
//! Assembler a;
//!
//! // ... your code generation 
//!
//! // Alloc execute enabled memory and call generated function, vsize will
//! // contain size of allocated virtual memory block.
//! SysInt vsize;
//! void *vmem = VM::alloc(a.codeSize(), &vsize, true /* canExecute */);
//!
//! // Relocate code to vmem.
//! a.relocCode(vmem);
//!
//! // Cast vmem to void() function and call it. If you have different function
//! // type, you must use it instead
//! function_cast<void (*)()>(vmem)();
//!
//! // Memory should be freed, but use VM::free() to do that.
//! VM::free(vmem, vsize);
//! @endverbatim
//!
//! @c note This was very primitive example how to call generated code.
//! In real production code you will never alloc and free code for one run,
//! you will alloc memory, where you copy generated code, and you will run
//! it many times from that place.
//!
//! <b>Using labels</b>
//!
//! While generating assembler code, you will usually need to create complex
//! code with labels. Labels are fully supported and you can call @c jmp or 
//! @c je (and similar) instructions to initialized or yet uninitialized label.
//! Each label expects to be bound into offset. To bind label to specific 
//! offset, use @c bind() method.
//!
//! See next example that contains complete code that creates simple memory
//! copy function (in DWORD entities).
//!
//! @verbatim
//! // Example: Usage of Label (32 bit code)
//! //
//! // Create simple DWORD memory copy function:
//! // ASMJIT_STDCALL void copy32(UInt32* dst, const UInt32* src, sysuint_t count);
//! using namespace AsmJit;
//!
//! // Assembler instance
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
//! a.mov(esi, dword_ptr(ebp, arg_offset + 0)); // get dst
//! a.mov(edi, dword_ptr(ebp, arg_offset + 4)); // get src
//! a.mov(ecx, dword_ptr(ebp, arg_offset + 8)); // get count
//!
//! // Bind L_Loop label to here
//! a.bind(&L_Loop);
//!
//! Copy 4 bytes
//! a.mov(eax, dword_ptr(esi));
//! a.mov(dword_ptr(edi), eax);
//!
//! // Increment pointers
//! a.add(esi, 4);
//! a.add(edi, 4);
//! 
//! // Repeat loop until ecx != 0
//! a.dec(ecx);
//! a.jz(&L_Loop);
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
//!
//! If you need more abstraction for generating assembler code and you want
//! to hide calling conventions between 32 bit and 64 bit operating systems,
//! look at @c Compiler class that is designed for higher level code 
//! generation.
//!
//! @sa @c Compiler.
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
  inline UInt8 getByteAt(SysInt pos) const { return _buffer.getByteAt(pos); }
  //! @brief Set word at position @a pos.
  inline UInt16 getWordAt(SysInt pos) const { return _buffer.getWordAt(pos); }
  //! @brief Set word at position @a pos.
  inline UInt32 getDWordAt(SysInt pos) const { return _buffer.getDWordAt(pos); }
  //! @brief Set word at position @a pos.
  inline UInt64 getQWordAt(SysInt pos) const { return _buffer.getQWordAt(pos); }

  //! @brief Set byte at position @a pos.
  inline void setByteAt(SysInt pos, UInt8 x) { _buffer.setByteAt(pos, x); }
  //! @brief Set word at position @a pos.
  inline void setWordAt(SysInt pos, UInt16 x) { _buffer.setWordAt(pos, x); }
  //! @brief Set word at position @a pos.
  inline void setDWordAt(SysInt pos, UInt32 x) { _buffer.setDWordAt(pos, x); }
  //! @brief Set word at position @a pos.
  inline void setQWordAt(SysInt pos, UInt64 x) { _buffer.setQWordAt(pos, x); }

  //! @brief Set word at position @a pos.
  inline Int32 getInt32At(SysInt pos) const { return (Int32)_buffer.getDWordAt(pos); }
  //! @brief Set int32 at position @a pos.
  inline void setInt32At(SysInt pos, Int32 x) { _buffer.setDWordAt(pos, (Int32)x); }

  //! @brief Set custom variable @a imm at position @a pos.
  //!
  //! @note This function is used to patch existing code.
  void setVarAt(SysInt pos, SysInt i, UInt8 isUnsigned, UInt32 size);

  // -------------------------------------------------------------------------
  // [Assembler Emitters]
  //
  // These emitters are not protecting buffer from overrun, this must be 
  // done is _emitX86() methods by:
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

  //! @brief Emit Int32 (4 bytes) to internal buffer.
  inline void _emitInt32(Int32 x) { _buffer.emitDWord((UInt32)x); }

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
  inline void _emitRexRM(UInt8 w, UInt8 opReg, const BaseRegMem& rm)
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
  void _emitModRM(UInt8 opReg, const BaseRegMem& op);

  void _emitX86Inl(UInt32 opCode, UInt8 i16bit, UInt8 rexw, UInt8 reg);
  void _emitX86RM(UInt32 opCode, UInt8 i16bit, UInt8 rexw, UInt8 o, const BaseRegMem& op);

  void _emitFpu(UInt32 opCode);
  void _emitFpuSTI(UInt32 opCode, UInt32 sti);
  void _emitFpuMEM(UInt32 opCode, UInt8 opReg, const Mem& mem);

  void _emitMmu(UInt32 opCode, UInt8 rexw, UInt8 opReg, const BaseRegMem& src);

  void _emitDisplacement(Label* label);

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
  // [EmitX86]
  // -------------------------------------------------------------------------

  virtual void _emitX86(UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3);

  // -------------------------------------------------------------------------
  // [Align]
  // -------------------------------------------------------------------------

  virtual void align(SysInt m);

  // -------------------------------------------------------------------------
  // [Bind]
  // -------------------------------------------------------------------------

  virtual void bind(Label* label);

  //! @brief Bind label to pos - called from bind(Label* label).
  void bindTo(Label* label, SysInt pos);

  // -------------------------------------------------------------------------
  // [Logging]
  // -------------------------------------------------------------------------

  //! @brief Logger
  struct ASMJIT_API Logger
  {
    //! @brief Destroy logger.
    virtual ~Logger();
    //! @brief Log instruction with operands.
    virtual void logInstruction(UInt32 code, const Operand* o1, const Operand* o2, const Operand* o3) = 0;
    //! @brief Log .align directive.
    virtual void logAlign(SysInt m) = 0;
    //! @brief Log label.
    virtual void logLabel(const Label* label) = 0;
    //! @brief Log printf like message.
    virtual void logFormat(const char* fmt, ...) = 0;
    //! @brief Abstract method to log output.
    virtual void log(const char* buf) = 0;
  };

  //! @brief Return logger or @c NULL (if none).
  inline Logger* logger() const { return _logger; }
  //! @brief Set logger to @a logger.
  inline void setLogger(Logger* logger) { _logger = logger; }

  // -------------------------------------------------------------------------
  // [Variables]
  // -------------------------------------------------------------------------

  //! @brief Binary code buffer.
  Buffer _buffer;

  //! @brief List of relocations.
  PodVector<RelocInfo> _relocations;

  //! @brief Logger.
  Logger* _logger;
};

//! @}

} // AsmJit namespace

// [Guard]
#endif // _ASMJITASSEMBLERX86X64_H
