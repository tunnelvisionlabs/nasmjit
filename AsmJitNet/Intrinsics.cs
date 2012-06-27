namespace AsmJitNet
{
    using System;
    using System.Diagnostics.Contracts;

    public static class Intrinsics
    {
        #region X86 Instructions

        /// <summary>
        /// Add with carry.
        /// </summary>
        public static void Adc<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Adc, dst, src);
        }

        /// <summary>
        /// Add with carry.
        /// </summary>
        public static void Adc<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Adc, dst, src);
        }

        /// <summary>
        /// Add with carry.
        /// </summary>
        public static void Adc<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Adc, dst, src);
        }

        /// <summary>
        /// Add with carry.
        /// </summary>
        public static void Adc<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Adc, dst, src);
        }

        /// <summary>
        /// Add with carry.
        /// </summary>
        public static void Adc(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Adc, dst, src);
        }

        /// <summary>
        /// Add.
        /// </summary>
        public static void Add<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Add, dst, src);
        }

        /// <summary>
        /// Add.
        /// </summary>
        public static void Add<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Add, dst, src);
        }

        /// <summary>
        /// Add.
        /// </summary>
        public static void Add<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Add, dst, src);
        }

        /// <summary>
        /// Add.
        /// </summary>
        public static void Add<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Add, dst, src);
        }

        /// <summary>
        /// Add.
        /// </summary>
        public static void Add(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Add, dst, src);
        }

        /// <summary>
        /// Logical AND.
        /// </summary>
        public static void And<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.And, dst, src);
        }

        /// <summary>
        /// Logical AND.
        /// </summary>
        public static void And<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.And, dst, src);
        }

        /// <summary>
        /// Logical AND.
        /// </summary>
        public static void And<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.And, dst, src);
        }

        /// <summary>
        /// Logical AND.
        /// </summary>
        public static void And<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.And, dst, src);
        }

        /// <summary>
        /// Logical AND.
        /// </summary>
        public static void And(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.And, dst, src);
        }

        /// <summary>
        /// Bit scan forward
        /// </summary>
        public static void Bsf<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Bsf, dst, src);
        }

        /// <summary>
        /// Bit scan forward
        /// </summary>
        public static void Bsf<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Bsf, dst, src);
        }

        /// <summary>
        /// Bit scan reverse
        /// </summary>
        public static void Bsr<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Bsr, dst, src);
        }

        /// <summary>
        /// Bit scan reverse
        /// </summary>
        public static void Bsr<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Bsr, dst, src);
        }

        /// <summary>
        /// Byte swap (32 bit or 64 bit registers only) (i486)
        /// </summary>
        public static void Bswap<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Bswap, dst);
        }

        /// <summary>
        /// Bit test
        /// </summary>
        public static void Bt<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Bt, dst, src);
        }

        /// <summary>
        /// Bit test
        /// </summary>
        public static void Bt<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Bt, dst, src);
        }

        /// <summary>
        /// Bit test
        /// </summary>
        public static void Bt<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Bt, dst, src);
        }

        /// <summary>
        /// Bit test
        /// </summary>
        public static void Bt(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Bt, dst, src);
        }

        /// <summary>
        /// Bit test and complement
        /// </summary>
        public static void Btc<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Btc, dst, src);
        }

        /// <summary>
        /// Bit test and complement
        /// </summary>
        public static void Btc<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Btc, dst, src);
        }

        /// <summary>
        /// Bit test and complement
        /// </summary>
        public static void Btc<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Btc, dst, src);
        }

        /// <summary>
        /// Bit test and complement
        /// </summary>
        public static void Btc(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Btc, dst, src);
        }

        /// <summary>
        /// Bit test and reset
        /// </summary>
        public static void Btr<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Btr, dst, src);
        }

        /// <summary>
        /// Bit test and reset
        /// </summary>
        public static void Btr<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Btr, dst, src);
        }

        /// <summary>
        /// Bit test and reset
        /// </summary>
        public static void Btr<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Btr, dst, src);
        }

        /// <summary>
        /// Bit test and reset
        /// </summary>
        public static void Btr(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Btr, dst, src);
        }

        /// <summary>
        /// Bit test and set
        /// </summary>
        public static void Bts<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Bts, dst, src);
        }

        /// <summary>
        /// Bit test and set
        /// </summary>
        public static void Bts<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Bts, dst, src);
        }

        /// <summary>
        /// Bit test and set
        /// </summary>
        public static void Bts<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Bts, dst, src);
        }

        /// <summary>
        /// Bit test and set
        /// </summary>
        public static void Bts(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Bts, dst, src);
        }

        /// <summary>
        /// Convert byte to word (sign extend)
        /// </summary>
        public static void Cbw(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.Cbw);
        }

        /// <summary>
        /// Convert byte to word (sign extend)
        /// </summary>
        public static void Cbw(this Compiler compiler, GPVar dst)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst != null);

            compiler.EmitInstruction(InstructionCode.Cbw, dst);
        }

        /// <summary>
        /// Convert word to dword (sign extend)
        /// </summary>
        public static void Cwde(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.Cwde);
        }

        /// <summary>
        /// Convert word to dword (sign extend)
        /// </summary>
        public static void Cwde(this Compiler compiler, GPVar dst)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst != null);

            compiler.EmitInstruction(InstructionCode.Cwde, dst);
        }

        /// <summary>
        /// Convert dword to qword (sign extend)
        /// </summary>
        public static void Cdqe(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.Cdqe));

            assembler.EmitInstruction(InstructionCode.Cdqe);
        }

        /// <summary>
        /// Convert dword to qword (sign extend)
        /// </summary>
        public static void Cdqe(this Compiler compiler, GPVar dst)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.Cdqe));

            compiler.EmitInstruction(InstructionCode.Cdqe, dst);
        }

        /// <summary>
        /// Convert word to dword (sign extend)
        /// </summary>
        public static void Cwd(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.Cwd);
        }

        /// <summary>
        /// Convert word to dword (sign extend)
        /// </summary>
        public static void Cwd(this Compiler compiler, GPVar dstLo, GPVar dstHi)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dstLo != null);
            Contract.Requires(dstHi != null);

            compiler.EmitInstruction(InstructionCode.Cwd, dstLo, dstHi);
        }

        /// <summary>
        /// Convert dword to qword (sign extend)
        /// </summary>
        public static void Cdq(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.Cdq);
        }

        /// <summary>
        /// Convert dword to qword (sign extend)
        /// </summary>
        public static void Cdq(this Compiler compiler, GPVar dstLo, GPVar dstHi)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dstLo != null);
            Contract.Requires(dstHi != null);

            compiler.EmitInstruction(InstructionCode.Cdq, dstLo, dstHi);
        }

        /// <summary>
        /// Convert qword to dqword (sign extend)
        /// </summary>
        public static void Cqo(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.Cdqe));

            assembler.EmitInstruction(InstructionCode.Cqo);
        }

        /// <summary>
        /// Convert qword to dqword (sign extend)
        /// </summary>
        public static void Cqo(this Compiler compiler, GPVar dstLo, GPVar dstHi)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dstLo != null);
            Contract.Requires(dstHi != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.Cdqe));

            compiler.EmitInstruction(InstructionCode.Cqo, dstLo, dstHi);
        }

        /// <summary>
        /// Clear carry flag
        /// </summary>
        public static void Clc(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);
            intrinsicSupport.EmitInstruction(InstructionCode.Clc);
        }

        /// <summary>
        /// Clear direction flag
        /// </summary>
        public static void Cld(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);
            intrinsicSupport.EmitInstruction(InstructionCode.Cld);
        }

        /// <summary>
        /// Complement carry flag
        /// </summary>
        public static void Cmc(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);
            intrinsicSupport.EmitInstruction(InstructionCode.Cmc);
        }

        /// <summary>
        /// Conditional move
        /// </summary>
        public static void CMov<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Condition cc, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(Assembler.ConditionToMovCC(cc), dst, src);
        }

        /// <summary>
        /// Conditional move
        /// </summary>
        public static void CMov<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Condition cc, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(Assembler.ConditionToMovCC(cc), dst, src);
        }

        public static void Cmova<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmova, dst, src);
        }

        public static void Cmova<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmova, dst, src);
        }

        public static void Cmovae<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovae, dst, src);
        }

        public static void Cmovae<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovae, dst, src);
        }

        public static void Cmovb<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovb, dst, src);
        }

        public static void Cmovb<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovb, dst, src);
        }

        public static void Cmovbe<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovbe, dst, src);
        }

        public static void Cmovbe<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovbe, dst, src);
        }

        public static void Cmovc<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovc, dst, src);
        }

        public static void Cmovc<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovc, dst, src);
        }

        public static void Cmove<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmove, dst, src);
        }

        public static void Cmove<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmove, dst, src);
        }

        public static void Cmovg<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovg, dst, src);
        }

        public static void Cmovg<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovg, dst, src);
        }

        public static void Cmovge<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovge, dst, src);
        }

        public static void Cmovge<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovge, dst, src);
        }

        public static void Cmovl<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovl, dst, src);
        }

        public static void Cmovl<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovl, dst, src);
        }

        public static void Cmovle<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovle, dst, src);
        }

        public static void Cmovle<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovle, dst, src);
        }

        public static void Cmovna<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovna, dst, src);
        }

        public static void Cmovna<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovna, dst, src);
        }

        public static void Cmovnae<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnae, dst, src);
        }

        public static void Cmovnae<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnae, dst, src);
        }

        public static void Cmovnb<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnb, dst, src);
        }

        public static void Cmovnb<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnb, dst, src);
        }

        public static void Cmovnbe<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnbe, dst, src);
        }

        public static void Cmovnbe<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnbe, dst, src);
        }

        public static void Cmovnc<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnc, dst, src);
        }

        public static void Cmovnc<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnc, dst, src);
        }

        public static void Cmovne<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovne, dst, src);
        }

        public static void Cmovne<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovne, dst, src);
        }

        public static void Cmovng<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovng, dst, src);
        }

        public static void Cmovng<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovng, dst, src);
        }

        public static void Cmovnge<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnge, dst, src);
        }

        public static void Cmovnge<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnge, dst, src);
        }

        public static void Cmovnl<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnl, dst, src);
        }

        public static void Cmovnl<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnl, dst, src);
        }

        public static void Cmovnle<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnle, dst, src);
        }

        public static void Cmovnle<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnle, dst, src);
        }

        public static void Cmovno<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovno, dst, src);
        }

        public static void Cmovno<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovno, dst, src);
        }

        public static void Cmovnp<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnp, dst, src);
        }

        public static void Cmovnp<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnp, dst, src);
        }

        public static void Cmovns<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovns, dst, src);
        }

        public static void Cmovns<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovns, dst, src);
        }

        public static void Cmovnz<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnz, dst, src);
        }

        public static void Cmovnz<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovnz, dst, src);
        }

        public static void Cmovo<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovo, dst, src);
        }

        public static void Cmovo<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovo, dst, src);
        }

        public static void Cmovp<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovp, dst, src);
        }

        public static void Cmovp<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovp, dst, src);
        }

        public static void Cmovpe<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovpe, dst, src);
        }

        public static void Cmovpe<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovpe, dst, src);
        }

        public static void Cmovpo<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovpo, dst, src);
        }

        public static void Cmovpo<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovpo, dst, src);
        }

        public static void Cmovs<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovs, dst, src);
        }

        public static void Cmovs<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovs, dst, src);
        }

        public static void Cmovz<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovz, dst, src);
        }

        public static void Cmovz<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmovz, dst, src);
        }

        public static void Cmp<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmp, dst, src);
        }

        public static void Cmp<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmp, dst, src);
        }

        public static void Cmp<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmp, dst, src);
        }

        public static void Cmp<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmp, dst, src);
        }

        public static void Cmp(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmp, dst, src);
        }

        public static void Cmpxchg(this Assembler compiler, GPReg dst, GPReg src)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            compiler.EmitInstruction(InstructionCode.Cmpxchg, dst, src);
        }

        public static void Cmpxchg(this Assembler compiler, Mem dst, GPReg src)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            compiler.EmitInstruction(InstructionCode.Cmpxchg, dst, src);
        }

        public static void Cmpxchg(this Compiler compiler, GPVar cmp_1_eax, GPVar cmp_2, GPVar src)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp_1_eax != null);
            Contract.Requires(cmp_2 != null);
            Contract.Requires(src != null);

            if (cmp_1_eax.Id == src.Id)
                throw new ArgumentException();

            compiler.EmitInstruction(InstructionCode.Cmpxchg, cmp_1_eax, cmp_2, src);
        }

        public static void Cmpxchg(this Compiler compiler, GPVar cmp_1_eax, Mem cmp_2, GPVar src)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp_1_eax != null);
            Contract.Requires(cmp_2 != null);
            Contract.Requires(src != null);

            if (cmp_1_eax.Id == src.Id)
                throw new ArgumentException();

            compiler.EmitInstruction(InstructionCode.Cmpxchg, cmp_1_eax, cmp_2, src);
        }

        public static void Cmpxchg8b(this Assembler assembler, Mem dst)
        {
            Contract.Requires(assembler != null);
            Contract.Requires(dst != null);

            assembler.EmitInstruction(InstructionCode.Cmpxchg8b, dst);
        }

        public static void Cmpxchg8b(this Compiler compiler, GPVar cmp_edx, GPVar cmp_eax, GPVar cmp_ecx, GPVar cmp_ebx, Mem dst)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp_eax != null);
            Contract.Requires(cmp_ebx != null);
            Contract.Requires(cmp_ecx != null);
            Contract.Requires(cmp_edx != null);
            Contract.Requires(dst != null);

            compiler.EmitInstruction(InstructionCode.Cmpxchg8b, cmp_edx, cmp_eax, cmp_ecx, cmp_ebx, dst);
        }

        public static void Cmpxchg16b(this Assembler assembler, Mem dst)
        {
            Contract.Requires(assembler != null);
            Contract.Requires(dst != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.Cmpxchg16b));

            assembler.EmitInstruction(InstructionCode.Cmpxchg8b, dst);
        }

        public static void Cmpxchg16b(this Compiler compiler, GPVar cmp_rdx, GPVar cmp_rax, GPVar cmp_rcx, GPVar cmp_rbx, Mem dst)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp_rax != null);
            Contract.Requires(cmp_rbx != null);
            Contract.Requires(cmp_rcx != null);
            Contract.Requires(cmp_rdx != null);
            Contract.Requires(dst != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.Cmpxchg16b));

            compiler.EmitInstruction(InstructionCode.Cmpxchg8b, cmp_rdx, cmp_rax, cmp_rcx, cmp_rbx, dst);
        }

        public static void Cpuid(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.Cpuid);
        }

        public static void Cpuid(this Compiler compiler, GPVar inout_eax, GPVar out_ebx, GPVar out_ecx, GPVar out_edx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(inout_eax != null);
            Contract.Requires(out_ebx != null);
            Contract.Requires(out_ecx != null);
            Contract.Requires(out_edx != null);

            compiler.EmitInstruction(InstructionCode.Cpuid, inout_eax, out_ebx, out_ecx, out_edx);
        }

        public static void Daa<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            if (!Util.IsX86)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X86.", InstructionCode.Daa));

            intrinsicSupport.EmitInstruction(InstructionCode.Daa, dst);
        }

        public static void Das<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            if (!Util.IsX86)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X86.", InstructionCode.Das));

            intrinsicSupport.EmitInstruction(InstructionCode.Das, dst);
        }

        public static void Dec<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Dec, dst);
        }

        public static void Dec(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Dec, dst);
        }

        /// <summary>
        /// Unsigned divide
        /// </summary>
        public static void Div<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dstRem, TGP dstQuot, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dstRem != null);
            Contract.Requires(dstQuot != null);
            Contract.Requires(src != null);
            Contract.Requires(dstRem.Id != dstQuot.Id);

            intrinsicSupport.EmitInstruction(InstructionCode.Div, dstRem, dstQuot, src);
        }

        /// <summary>
        /// Unsigned divide
        /// </summary>
        public static void Div<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dstRem, TGP dstQuot, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dstRem != null);
            Contract.Requires(dstQuot != null);
            Contract.Requires(src != null);
            Contract.Requires(dstRem.Id != dstQuot.Id);

            intrinsicSupport.EmitInstruction(InstructionCode.Div, dstRem, dstQuot, src);
        }

        /// <summary>
        /// Make stack frame for procedure parameters
        /// </summary>
        public static void Enter(this Assembler intrinsicSupport, Imm imm16, Imm imm8)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(imm16 != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Enter, imm16, imm8);
        }

        /// <summary>
        /// Signed divide
        /// </summary>
        public static void Idiv<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dstRem, TGP dstQuot, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dstRem != null);
            Contract.Requires(dstQuot != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Idiv, dstRem, dstQuot, src);
        }

        /// <summary>
        /// Signed divide
        /// </summary>
        public static void Idiv<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dstRem, TGP dstQuot, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dstRem != null);
            Contract.Requires(dstQuot != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Idiv, dstRem, dstQuot, src);
        }

        /// <summary>
        /// Signed multiply
        /// </summary>
        public static void Imul<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dstHi, TGP dstLo, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dstHi != null);
            Contract.Requires(dstLo != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Imul, dstHi, dstLo, src);
        }

        /// <summary>
        /// Signed multiply
        /// </summary>
        public static void Imul<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dstHi, TGP dstLo, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dstHi != null);
            Contract.Requires(dstLo != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Imul, dstHi, dstLo, src);
        }

        public static void Imul<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Imul, dst, src);
        }

        public static void Imul<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Imul, dst, src);
        }

        public static void Imul<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Imul, dst, src);
        }

        public static void Imul<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src, Imm imm)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Imul, dst, src, imm);
        }

        public static void Imul<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src, Imm imm)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Imul, dst, src, imm);
        }

        public static void Inc<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Inc, dst);
        }

        public static void Inc(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Inc, dst);
        }

        public static void Int3(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Int3);
        }

        public static void J(this IIntrinsicSupport intrinsicSupport, Condition cc, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(Assembler.ConditionToJump(cc), label, hint);
        }

        public static void Ja(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Ja, label, hint);
        }

        public static void Jae(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jae, label, hint);
        }

        public static void Jb(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jb, label, hint);
        }

        public static void Jbe(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jbe, label, hint);
        }

        public static void Jc(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jc, label, hint);
        }

        public static void Je(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Je, label, hint);
        }

        public static void Jg(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jg, label, hint);
        }

        public static void Jge(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jge, label, hint);
        }

        public static void Jl(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jl, label, hint);
        }

        public static void Jle(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jle, label, hint);
        }

        public static void Jna(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jna, label, hint);
        }

        public static void Jnae(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jnae, label, hint);
        }

        public static void Jnb(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jnb, label, hint);
        }

        public static void Jnbe(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jnbe, label, hint);
        }

        public static void Jnc(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jnc, label, hint);
        }

        public static void Jne(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jne, label, hint);
        }

        public static void Jng(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jng, label, hint);
        }

        public static void Jnge(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jnge, label, hint);
        }

        public static void Jnl(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jnl, label, hint);
        }

        public static void Jnle(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jnle, label, hint);
        }

        public static void Jno(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jno, label, hint);
        }

        public static void Jnp(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jnp, label, hint);
        }

        public static void Jns(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jns, label, hint);
        }

        public static void Jnz(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jnz, label, hint);
        }

        public static void Jo(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jo, label, hint);
        }

        public static void Jp(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jp, label, hint);
        }

        public static void Jpe(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jpe, label, hint);
        }

        public static void Jpo(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jpo, label, hint);
        }

        public static void Js(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Js, label, hint);
        }

        public static void Jz(this IIntrinsicSupport intrinsicSupport, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitJcc(InstructionCode.Jz, label, hint);
        }

        public static void Jmp<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Jmp, dst);
        }

        public static void Jmp(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Jmp, dst);
        }

        public static void Jmp(this IIntrinsicSupport intrinsicSupport, Imm dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Jmp, dst);
        }

        public static void Jmp(this IIntrinsicSupport intrinsicSupport, IntPtr dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Jmp, (Imm)dst);
        }

        public static void Jmp(this IIntrinsicSupport intrinsicSupport, Label label)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(label != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Jmp, label);
        }

        public static void Lea<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Lea, dst, src);
        }

#if ASMJIT_NOT_SUPPORTED_BY_COMPILER
        /// <summary>
        /// High level procedure exit
        /// </summary>
        public static void Leave<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, )
        where TGP : Operand, IGpOperand where TX87 : Operand, IX87Operand where TMM : Operand, IMmOperand where TXMM : Operand, IXmmOperand { Contract.Requires(intrinsicSupport != null);
            intrinsicSupport.EmitInstruction(InstructionCode.Leave);
        }
#endif

        public static void Mov<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mov, dst, src);
        }

        public static void Mov<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mov, dst, src);
        }

        public static void Mov<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mov, dst, src);
        }

        public static void Mov(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mov, dst, src);
        }

        public static void Mov<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mov, dst, src);
        }

        public static void Mov(this IIntrinsicSupport intrinsicSupport, Mem dst, SegmentReg src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mov, dst, src);
        }

        public static void Mov<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, SegmentReg src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mov, dst, src);
        }

        public static void Mov(this IIntrinsicSupport intrinsicSupport, SegmentReg dst, Mem src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mov, dst, src);
        }

        public static void Mov<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, SegmentReg dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mov, dst, src);
        }

        public static void MovPtr<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, IntPtr src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            Imm imm = src;
            intrinsicSupport.EmitInstruction(InstructionCode.MovPtr, dst, imm);
        }

        public static void MovPtr<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, IntPtr dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(src != null);

            Imm imm = dst;
            intrinsicSupport.EmitInstruction(InstructionCode.MovPtr, imm, src);
        }

        public static void Movsx<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movsx, dst, src);
        }

        public static void Movsx<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movsx, dst, src);
        }

        public static void Movsxd<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.Movsxd));

            intrinsicSupport.EmitInstruction(InstructionCode.Movsxd, dst, src);
        }

        public static void Movsxd<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.Movsxd));

            intrinsicSupport.EmitInstruction(InstructionCode.Movsxd, dst, src);
        }

        public static void Movzx<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movzx, dst, src);
        }

        public static void Movzx<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movzx, dst, src);
        }

        /// <summary>
        /// Unsigned multiply
        /// </summary>
        public static void Mul<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dstHi, TGP dstLo, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dstHi != null);
            Contract.Requires(dstLo != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mul, dstHi, dstLo, src);
        }

        /// <summary>
        /// Unsigned multiply
        /// </summary>
        public static void Mul<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dstHi, TGP dstLo, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dstHi != null);
            Contract.Requires(dstLo != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mul, dstHi, dstLo, src);
        }

        /// <summary>
        /// Two's complement negation
        /// </summary>
        public static void Neg<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Neg, dst);
        }

        /// <summary>
        /// Two's complement negation
        /// </summary>
        public static void Neg(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Neg, dst);
        }

        /// <summary>
        /// No operation
        /// </summary>
        public static void Nop(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Nop);
        }

        /// <summary>
        /// One's complement negation
        /// </summary>
        public static void Not<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Not, dst);
        }

        /// <summary>
        /// One's complement negation
        /// </summary>
        public static void Not(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Not, dst);
        }

        /// <summary>
        /// Logical OR.
        /// </summary>
        public static void Or<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Or, dst, src);
        }

        /// <summary>
        /// Logical OR.
        /// </summary>
        public static void Or<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Or, dst, src);
        }

        /// <summary>
        /// Logical OR.
        /// </summary>
        public static void Or<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Or, dst, src);
        }

        /// <summary>
        /// Logical OR.
        /// </summary>
        public static void Or<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Or, dst, src);
        }

        /// <summary>
        /// Logical OR.
        /// </summary>
        public static void Or(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Or, dst, src);
        }

        public static void Pop<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pop, dst);
        }

        public static void Pop(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pop, dst);
        }

        public static void Popad(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            if (!Util.IsX86)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X86.", InstructionCode.Popad));

            intrinsicSupport.EmitInstruction(InstructionCode.Popad);
        }

        public static void Popf(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            if (Util.IsX86)
                intrinsicSupport.Popfd();
            else if (Util.IsX64)
                intrinsicSupport.Popfq();
            else
                throw new NotImplementedException();
        }

        public static void Popfd(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            if (!Util.IsX86)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X86.", InstructionCode.Popfd));

            intrinsicSupport.EmitInstruction(InstructionCode.Popfd);
        }

        public static void Popfq(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.Popfq));

            intrinsicSupport.EmitInstruction(InstructionCode.Popfq);
        }

        public static void Push<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Push, src);
        }

        public static void Push(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Push, src);
        }

        public static void Push(this IIntrinsicSupport intrinsicSupport, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Push, src);
        }

        public static void Pushad(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            if (!Util.IsX86)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X86.", InstructionCode.Pushad));

            intrinsicSupport.EmitInstruction(InstructionCode.Pushad);
        }

        public static void Pushf(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            if (Util.IsX86)
                intrinsicSupport.Pushfd();
            else if (Util.IsX64)
                intrinsicSupport.Pushfq();
            else
                throw new NotImplementedException();
        }

        public static void Pushfd(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            if (!Util.IsX86)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X86.", InstructionCode.Pushfd));

            intrinsicSupport.EmitInstruction(InstructionCode.Pushfd);
        }

        public static void Pushfq(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.Pushfq));

            intrinsicSupport.EmitInstruction(InstructionCode.Pushfq);
        }

        public static void Rcl<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rcl, dst, src);
        }

        public static void Rcl<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rcl, dst, src);
        }

        public static void Rcl<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rcl, dst, src);
        }

        public static void Rcl(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rcl, dst, src);
        }

        public static void Rcr<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rcr, dst, src);
        }

        public static void Rcr<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rcr, dst, src);
        }

        public static void Rcr<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rcr, dst, src);
        }

        public static void Rcr(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rcr, dst, src);
        }

        public static void Rdtsc(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.Rdtsc);
        }

        public static void Rdtsc(this Compiler compiler, GPVar dst_edx, GPVar dst_eax)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst_edx != null);
            Contract.Requires(dst_eax != null);

            if (dst_edx.Id == dst_eax.Id)
                throw new ArgumentException();

            compiler.EmitInstruction(InstructionCode.Rdtsc, dst_edx, dst_eax);
        }

        public static void Rdtscp(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.Rdtscp);
        }

        public static void Rdtscp(this Compiler compiler, GPVar dst_edx, GPVar dst_eax, GPVar dst_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst_edx != null);
            Contract.Requires(dst_eax != null);
            Contract.Requires(dst_ecx != null);

            compiler.EmitInstruction(InstructionCode.Rdtscp, dst_edx, dst_eax, dst_ecx);
        }

        public static void RepLodsb(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepLodsb);
        }

        public static void RepLodsb(this Compiler compiler, GPVar dst_val, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst_val != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepLodsb, dst_val, src_addr, cnt_ecx);
        }

        public static void RepLodsd(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepLodsd);
        }

        public static void RepLodsd(this Compiler compiler, GPVar dst_val, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst_val != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepLodsd, dst_val, src_addr, cnt_ecx);
        }

        public static void RepLodsq(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.RepLodsq));

            assembler.EmitInstruction(InstructionCode.RepLodsq);
        }

        public static void RepLodsq(this Compiler compiler, GPVar dst_val, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst_val != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.RepLodsq));

            compiler.EmitInstruction(InstructionCode.RepLodsq, dst_val, src_addr, cnt_ecx);
        }

        public static void RepLodsw(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepLodsw);
        }

        public static void RepLodsw(this Compiler compiler, GPVar dst_val, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst_val != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepLodsw, dst_val, src_addr, cnt_ecx);
        }

        public static void RepMovsb(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepMovsb);
        }

        public static void RepMovsb(this Compiler compiler, GPVar dst_addr, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepMovsb, dst_addr, src_addr, cnt_ecx);
        }

        public static void RepMovsd(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepMovsd);
        }

        public static void RepMovsd(this Compiler compiler, GPVar dst_addr, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepMovsd, dst_addr, src_addr, cnt_ecx);
        }

        public static void RepMovsq(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.RepMovsq));

            assembler.EmitInstruction(InstructionCode.RepMovsq);
        }

        public static void RepMovsq(this Compiler compiler, GPVar dst_addr, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.RepMovsq));

            compiler.EmitInstruction(InstructionCode.RepMovsq, dst_addr, src_addr, cnt_ecx);
        }

        public static void RepMovsw(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepMovsw);
        }

        public static void RepMovsw(this Compiler compiler, GPVar dst_addr, GPVar src_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_addr != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepMovsw, dst_addr, src_addr, cnt_ecx);
        }

        public static void RepStosb(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepStosb);
        }

        public static void RepStosb(this Compiler compiler, GPVar dst_addr, GPVar src_val, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_val != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepStosb, dst_addr, src_val, cnt_ecx);
        }

        public static void RepStosd(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepStosd);
        }

        public static void RepStosd(this Compiler compiler, GPVar dst_addr, GPVar src_val, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_val != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepStosd, dst_addr, src_val, cnt_ecx);
        }

        public static void RepStosq(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.RepStosq));

            assembler.EmitInstruction(InstructionCode.RepStosq);
        }

        public static void RepStosq(this Compiler compiler, GPVar dst_addr, GPVar src_val, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_val != null);
            Contract.Requires(cnt_ecx != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.RepStosq));

            compiler.EmitInstruction(InstructionCode.RepStosq, dst_addr, src_val, cnt_ecx);
        }

        public static void RepStosw(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepStosw);
        }

        public static void RepStosw(this Compiler compiler, GPVar dst_addr, GPVar src_val, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(dst_addr != null);
            Contract.Requires(src_val != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepStosw, dst_addr, src_val, cnt_ecx);
        }

        public static void RepeCmpsb(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepeCmpsb);
        }

        public static void RepeCmpsb(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepeCmpsb, cmp1_addr, cmp2_addr, cnt_ecx);
        }

        public static void RepeCmpsd(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepeCmpsd);
        }

        public static void RepeCmpsd(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepeCmpsd, cmp1_addr, cmp2_addr, cnt_ecx);
        }

        public static void RepeCmpsq(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.RepeCmpsq));

            assembler.EmitInstruction(InstructionCode.RepeCmpsq);
        }

        public static void RepeCmpsq(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.RepeCmpsq));

            compiler.EmitInstruction(InstructionCode.RepeCmpsq, cmp1_addr, cmp2_addr, cnt_ecx);
        }

        public static void RepeCmpsw(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepeCmpsw);
        }

        public static void RepeCmpsw(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepeCmpsw, cmp1_addr, cmp2_addr, cnt_ecx);
        }

        public static void RepeScasb(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepeScasb);
        }

        public static void RepeScasb(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepeScasb, cmp1_addr, cmp2_val, cnt_ecx);
        }

        public static void RepeScasd(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepeScasd);
        }

        public static void RepeScasd(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepeScasd, cmp1_addr, cmp2_val, cnt_ecx);
        }

        public static void RepeScasq(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.RepeScasq));

            assembler.EmitInstruction(InstructionCode.RepeScasq);
        }

        public static void RepeScasq(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.RepeScasq));

            compiler.EmitInstruction(InstructionCode.RepeScasq, cmp1_addr, cmp2_val, cnt_ecx);
        }

        public static void RepeScasw(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepeScasw);
        }

        public static void RepeScasw(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepeScasw, cmp1_addr, cmp2_val, cnt_ecx);
        }

        public static void RepneCmpsb(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepneCmpsb);
        }

        public static void RepneCmpsb(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepneCmpsb, cmp1_addr, cmp2_addr, cnt_ecx);
        }

        public static void RepneCmpsd(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepneCmpsd);
        }

        public static void RepneCmpsd(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepneCmpsd, cmp1_addr, cmp2_addr, cnt_ecx);
        }

        public static void RepneCmpsq(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.RepneCmpsq));

            assembler.EmitInstruction(InstructionCode.RepneCmpsq);
        }

        public static void RepneCmpsq(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.RepneCmpsq));

            compiler.EmitInstruction(InstructionCode.RepneCmpsq, cmp1_addr, cmp2_addr, cnt_ecx);
        }

        public static void RepneCmpsw(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepneCmpsw);
        }

        public static void RepneCmpsw(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_addr, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_addr != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepneCmpsw, cmp1_addr, cmp2_addr, cnt_ecx);
        }

        public static void RepneScasb(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepneScasb);
        }

        public static void RepneScasb(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepneScasb, cmp1_addr, cmp2_val, cnt_ecx);
        }

        public static void RepneScasd(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepneScasd);
        }

        public static void RepneScasd(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepneScasd, cmp1_addr, cmp2_val, cnt_ecx);
        }

        public static void RepneScasq(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.RepneScasq));

            assembler.EmitInstruction(InstructionCode.RepneScasq);
        }

        public static void RepneScasq(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.RepneScasq));

            compiler.EmitInstruction(InstructionCode.RepneScasq, cmp1_addr, cmp2_val, cnt_ecx);
        }

        public static void RepneScasw(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.RepneScasw);
        }

        public static void RepneScasw(this Compiler compiler, GPVar cmp1_addr, GPVar cmp2_val, GPVar cnt_ecx)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(cmp1_addr != null);
            Contract.Requires(cmp2_val != null);
            Contract.Requires(cnt_ecx != null);

            compiler.EmitInstruction(InstructionCode.RepneScasw, cmp1_addr, cmp2_val, cnt_ecx);
        }

        public static void Ret(this Assembler assembler)
        {
            Contract.Requires(assembler != null);

            assembler.EmitInstruction(InstructionCode.Ret);
        }

        public static void Ret(this Assembler assembler, Imm imm16)
        {
            Contract.Requires(assembler != null);
            Contract.Requires(imm16 != null);

            assembler.EmitInstruction(InstructionCode.Ret, imm16);
        }

        public static void Ret(this Compiler compiler)
        {
            Contract.Requires(compiler != null);

            compiler.EmitReturn(null, null);
        }

        public static void Ret(this Compiler compiler, GPVar first)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(first != null);

            compiler.EmitReturn(first, null);
        }

        public static void Ret(this Compiler compiler, GPVar first, GPVar second)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(first != null);
            Contract.Requires(second != null);
            Contract.Requires(compiler.Function != null);

            compiler.EmitReturn(first, second);
        }

        public static void Ret(this Compiler compiler, XMMVar first)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(first != null);

            compiler.EmitReturn(first, null);
        }

        public static void Ret(this Compiler compiler, XMMVar first, XMMVar second)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(first != null);
            Contract.Requires(second != null);
            Contract.Requires(compiler.Function != null);

            compiler.EmitReturn(first, second);
        }

        public static void Rol<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rol, dst, src);
        }

        public static void Rol<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rol, dst, src);
        }

        public static void Rol<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rol, dst, src);
        }

        public static void Rol(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rol, dst, src);
        }

        public static void Ror<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Ror, dst, src);
        }

        public static void Ror<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Ror, dst, src);
        }

        public static void Ror<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Ror, dst, src);
        }

        public static void Ror(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Ror, dst, src);
        }

        public static void Sahf<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP var)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(var != null);

            if (!Util.IsX86)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X86.", InstructionCode.Sahf));

            intrinsicSupport.EmitInstruction(InstructionCode.Sahf, var);
        }

        public static void Sal<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sal, dst, src);
        }

        public static void Sal<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sal, dst, src);
        }

        public static void Sal<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sal, dst, src);
        }

        public static void Sal(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sal, dst, src);
        }

        public static void Sar<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sar, dst, src);
        }

        public static void Sar<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sar, dst, src);
        }

        public static void Sar<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sar, dst, src);
        }

        public static void Sar(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sar, dst, src);
        }

        /// <summary>
        /// Integer subtraction with borrow.
        /// </summary>
        public static void Sbb<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sbb, dst, src);
        }

        /// <summary>
        /// Integer subtraction with borrow.
        /// </summary>
        public static void Sbb<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sbb, dst, src);
        }

        /// <summary>
        /// Integer subtraction with borrow.
        /// </summary>
        public static void Sbb<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sbb, dst, src);
        }

        /// <summary>
        /// Integer subtraction with borrow.
        /// </summary>
        public static void Sbb<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sbb, dst, src);
        }

        /// <summary>
        /// Integer subtraction with borrow.
        /// </summary>
        public static void Sbb(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sbb, dst, src);
        }

        /// <summary>
        /// Set byte on condition
        /// </summary>
        public static void Set<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Condition cc, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(Assembler.ConditionToSetCC(cc), dst);
        }

        /// <summary>
        /// Set byte on condition
        /// </summary>
        public static void Set(this IIntrinsicSupport intrinsicSupport, Condition cc, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(Assembler.ConditionToSetCC(cc), dst);
        }

        public static void Seta<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Seta, dst);
        }

        public static void Seta(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Seta, dst);
        }

        public static void Setae<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setae, dst);
        }

        public static void Setae(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setae, dst);
        }

        public static void Setb<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setb, dst);
        }

        public static void Setb(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setb, dst);
        }

        public static void Setbe<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setbe, dst);
        }

        public static void Setbe(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setbe, dst);
        }

        public static void Setc<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setc, dst);
        }

        public static void Setc(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setc, dst);
        }

        public static void Sete<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Sete, dst);
        }

        public static void Sete(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Sete, dst);
        }

        public static void Setg<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setg, dst);
        }

        public static void Setg(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setg, dst);
        }

        public static void Setge<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setge, dst);
        }

        public static void Setge(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setge, dst);
        }

        public static void Setl<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setl, dst);
        }

        public static void Setl(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setl, dst);
        }

        public static void Setle<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setle, dst);
        }

        public static void Setle(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setle, dst);
        }

        public static void Setna<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setna, dst);
        }

        public static void Setna(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setna, dst);
        }

        public static void Setnae<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnae, dst);
        }

        public static void Setnae(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnae, dst);
        }

        public static void Setnb<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnb, dst);
        }

        public static void Setnb(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnb, dst);
        }

        public static void Setnbe<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnbe, dst);
        }

        public static void Setnbe(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnbe, dst);
        }

        public static void Setnc<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnc, dst);
        }

        public static void Setnc(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnc, dst);
        }

        public static void Setne<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setne, dst);
        }

        public static void Setne(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setne, dst);
        }

        public static void Setng<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setng, dst);
        }

        public static void Setng(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setng, dst);
        }

        public static void Setnge<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnge, dst);
        }

        public static void Setnge(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnge, dst);
        }

        public static void Setnl<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnl, dst);
        }

        public static void Setnl(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnl, dst);
        }

        public static void Setnle<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnle, dst);
        }

        public static void Setnle(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnle, dst);
        }

        public static void Setno<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setno, dst);
        }

        public static void Setno(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setno, dst);
        }

        public static void Setnp<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnp, dst);
        }

        public static void Setnp(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnp, dst);
        }

        public static void Setns<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setns, dst);
        }

        public static void Setns(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setns, dst);
        }

        public static void Setnz<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnz, dst);
        }

        public static void Setnz(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setnz, dst);
        }

        public static void Seto<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Seto, dst);
        }

        public static void Seto(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Seto, dst);
        }

        public static void Setp<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setp, dst);
        }

        public static void Setp(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setp, dst);
        }

        public static void Setpe<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setpe, dst);
        }

        public static void Setpe(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setpe, dst);
        }

        public static void Setpo<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setpo, dst);
        }

        public static void Setpo(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setpo, dst);
        }

        public static void Sets<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Sets, dst);
        }

        public static void Sets(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Sets, dst);
        }

        public static void Setz<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size == 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setz, dst);
        }

        public static void Setz(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(dst.Size <= 1);

            intrinsicSupport.EmitInstruction(InstructionCode.Setz, dst);
        }

        public static void Shl<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shl, dst, src);
        }

        public static void Shl<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shl, dst, src);
        }

        public static void Shl<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shl, dst, src);
        }

        public static void Shl(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shl, dst, src);
        }

        public static void Shr<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shr, dst, src);
        }

        public static void Shr<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shr, dst, src);
        }

        public static void Shr<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shr, dst, src);
        }

        public static void Shr(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shr, dst, src);
        }

        public static void Shld<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src1, TGP src2)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shld, dst, src1, src2);
        }

        public static void Shld<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src1, Imm src2)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shld, dst, src1, src2);
        }

        public static void Shld<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src1, TGP src2)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shld, dst, src1, src2);
        }

        public static void Shld<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src1, Imm src2)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shld, dst, src1, src2);
        }

        public static void Shrd<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src1, TGP src2)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shrd, dst, src1, src2);
        }

        public static void Shrd<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src1, Imm src2)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shrd, dst, src1, src2);
        }

        public static void Shrd<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src1, TGP src2)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shrd, dst, src1, src2);
        }

        public static void Shrd<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src1, Imm src2)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src1 != null);
            Contract.Requires(src2 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shrd, dst, src1, src2);
        }

        public static void Stc(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Stc);
        }

        public static void Std(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Std);
        }

        public static void Sub<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sub, dst, src);
        }

        public static void Sub<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sub, dst, src);
        }

        public static void Sub<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sub, dst, src);
        }

        public static void Sub<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sub, dst, src);
        }

        public static void Sub(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sub, dst, src);
        }

        public static void Test<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP op1, TGP op2)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(op1 != null);
            Contract.Requires(op2 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Test, op1, op2);
        }

        public static void Test<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP op1, Imm op2)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(op1 != null);
            Contract.Requires(op2 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Test, op1, op2);
        }

        public static void Test<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem op1, TGP op2)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(op1 != null);
            Contract.Requires(op2 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Test, op1, op2);
        }

        public static void Test(this IIntrinsicSupport intrinsicSupport, Mem op1, Imm op2)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(op1 != null);
            Contract.Requires(op2 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Test, op1, op2);
        }

        public static void Ud2(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Ud2);
        }

        public static void Xadd<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Xadd, dst, src);
        }

        public static void Xadd<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Xadd, dst, src);
        }

        public static void Xchg<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Xchg, dst, src);
        }

        public static void Xchg<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Xchg, dst, src);
        }

        public static void Xchg<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Xchg, dst, src);
        }

        public static void Xor<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Xor, dst, src);
        }

        public static void Xor<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Xor, dst, src);
        }

        public static void Xor<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Imm src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Xor, dst, src);
        }

        public static void Xor<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Xor, dst, src);
        }

        public static void Xor(this IIntrinsicSupport intrinsicSupport, Mem dst, Imm src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Xor, dst, src);
        }

        #endregion

        #region X87 Instructions

        /// <summary>Compute 2^x - 1 (FPU).</summary>
        public static void F2xm1(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.F2xm1);
        }

        /// <summary>Absolute Value of st(0) (FPU).</summary>
        public static void Fabs(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fabs);
        }

        /// <summary>Add @a src to @a dst and store result in @a dst (FPU).</summary>
        ///
        /// <remarks>One of dst or src must be st(0).</remarks>
        public static void Fadd(this IX87IntrinsicSupport<X87Reg> intrinsicSupport, X87Reg dst, X87Reg src)
        {
            Contract.Assert(dst.RegisterIndex == 0 || src.RegisterIndex == 0);
            intrinsicSupport.EmitInstruction(InstructionCode.Fadd, dst, src);
        }

        /// <summary>Add @a src to st(0) and store result in st(0) (FPU).</summary>
        ///
        /// <remarks>SP-FP or DP-FP determined by @a adr size.</remarks>
        public static void Fadd(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fadd, src);
        }

        /// <summary>Add st(0) to @a dst and POP register stack (FPU).</summary>
        public static void Faddp(this IX87IntrinsicSupport<X87Reg> intrinsicSupport)
        {
            Faddp(intrinsicSupport, Register.st((RegIndex)1));
        }

        public static void Faddp<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 dst)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Faddp, dst);
        }

        /// <summary>Load Binary Coded Decimal (FPU).</summary>
        public static void Fbld(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fbld, src);
        }

        /// <summary>Store BCD Integer and Pop (FPU).</summary>
        public static void Fbstp(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fbstp, dst);
        }

        /// <summary>Change st(0) Sign (FPU).</summary>
        public static void Fchs(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fchs);
        }

        /// <summary>Clear Exceptions (FPU).</summary>
        /// <remarks>
        /// Clear floating-point exception flags after checking for pending unmasked
        /// floating-point exceptions.
        ///
        /// Clears the floating-point exception flags (PE, UE, OE, ZE, DE, and IE),
        /// the exception summary status flag (ES), the stack fault flag (SF), and
        /// the busy flag (B) in the FPU status word. The FCLEX instruction checks
        /// for and handles any pending unmasked floating-point exceptions before
        /// clearing the exception flags.
        /// </remarks>
        public static void Fclex(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fclex);
        }

        /// <summary>FP Conditional Move (FPU).</summary>
        public static void Fcmovb<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 src)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcmovb, src);
        }
        /// <summary>FP Conditional Move (FPU).</summary>
        public static void Fcmovbe<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 src)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcmovbe, src);
        }
        /// <summary>FP Conditional Move (FPU).</summary>
        public static void Fcmove<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 src)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcmove, src);
        }
        /// <summary>FP Conditional Move (FPU).</summary>
        public static void Fcmovnb<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 src)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcmovnb, src);
        }
        /// <summary>FP Conditional Move (FPU).</summary>
        public static void Fcmovnbe<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 src)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcmovnbe, src);
        }
        /// <summary>FP Conditional Move (FPU).</summary>
        public static void Fcmovne<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 src)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcmovne, src);
        }
        /// <summary>FP Conditional Move (FPU).</summary>
        public static void Fcmovnu<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 src)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcmovnu, src);
        }
        /// <summary>FP Conditional Move (FPU).</summary>
        public static void Fcmovu<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 src)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcmovu, src);
        }

        /// <summary>Compare st(0) with @a reg (FPU).</summary>
        public static void Fcom(this IX87IntrinsicSupport<X87Reg> intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcom, Register.st((RegIndex)1));
        }

        public static void Fcom<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcom, reg);
        }

        /// <summary>Compare st(0) with 4-byte or 8-byte FP at @a src (FPU).</summary>
        public static void Fcom(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcom, src);
        }

        /// <summary>Compare st(0) with @a reg and pop the stack (FPU).</summary>
        public static void Fcomp(this IX87IntrinsicSupport<X87Reg> intrinsicSupport)
        {
            Fcomp(intrinsicSupport, Register.st((RegIndex)1));
        }

        public static void Fcomp<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcomp, reg);
        }

        /// <summary>Compare st(0) with 4-byte or 8-byte FP at @a adr and pop the
        /// stack (FPU).</summary>
        public static void Fcomp(this IIntrinsicSupport intrinsicSupport, Mem mem)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcomp, mem);
        }

        /// <summary>Compare st(0) with st(1) and pop register stack twice (FPU).</summary>
        public static void Fcompp(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcompp);
        }

        /// <summary>Compare st(0) and @a reg and Set EFLAGS (FPU).</summary>
        public static void Fcomi<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcomi, reg);
        }

        /// <summary>Compare st(0) and @a reg and Set EFLAGS and pop the stack (FPU).</summary>
        public static void Fcomip<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcomip, reg);
        }

        /// <summary>Cosine (FPU).</summary>
        /// <remarks>
        /// This instruction calculates the cosine of the source operand in
        /// register st(0) and stores the result in st(0).
        /// </remarks>
        public static void Fcos(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fcos);
        }

        /// <summary>Decrement Stack-Top Pointer (FPU).</summary>
        /// <remarks>
        /// Subtracts one from the TOP field of the FPU status word (decrements
        /// the top-ofstack pointer). If the TOP field contains a 0, it is set
        /// to 7. The effect of this instruction is to rotate the stack by one
        /// position. The contents of the FPU data registers and tag register
        /// are not affected.
        /// </remarks>
        public static void Fdecstp(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fdecstp);
        }

        /// <summary>Divide @a dst by @a src (FPU).</summary>
        ///
        /// <remarks>One of @a dst or @a src register must be st(0).</remarks>
        public static void Fdiv(this IX87IntrinsicSupport<X87Reg> intrinsicSupport, X87Reg dst, X87Reg src)
        {
            Contract.Assert(dst.RegisterIndex == 0 || src.RegisterIndex == 0);
            intrinsicSupport.EmitInstruction(InstructionCode.Fdiv, dst, src);
        }
        /// <summary>Divide st(0) by 32-bit or 64-bit FP value (FPU).</summary>
        public static void Fdiv(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fdiv, src);
        }

        /// <summary>Divide @a reg by st(0) (FPU).</summary>
        public static void Fdivp(this IX87IntrinsicSupport<X87Reg> intrinsicSupport)
        {
            Fdivp(intrinsicSupport, Register.st((RegIndex)1));
        }

        public static void Fdivp<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fdivp, reg);
        }

        /// <summary>Reverse Divide @a dst by @a src (FPU).</summary>
        ///
        /// <remarks>One of @a dst or @a src register must be st(0).</remarks>
        public static void Fdivr(this IX87IntrinsicSupport<X87Reg> intrinsicSupport, X87Reg dst, X87Reg src)
        {
            Contract.Assert(dst.RegisterIndex == 0 || src.RegisterIndex == 0);
            intrinsicSupport.EmitInstruction(InstructionCode.Fdivr, dst, src);
        }
        /// <summary>Reverse Divide st(0) by 32-bit or 64-bit FP value (FPU).</summary>
        public static void Fdivr(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fdivr, src);
        }

        /// <summary>Reverse Divide @a reg by st(0) (FPU).</summary>
        public static void Fdivrp(this IX87IntrinsicSupport<X87Reg> intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fdivrp, Register.st((RegIndex)1));
        }

        public static void Fdivrp<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fdivrp, reg);
        }

        /// <summary>Free Floating-Point Register (FPU).</summary>
        /// <remarks>
        /// Sets the tag in the FPU tag register associated with register @a reg
        /// to empty (11B). The contents of @a reg and the FPU stack-top pointer
        /// (TOP) are not affected.
        /// </remarks>
        public static void Ffree<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Ffree, reg);
        }

        /// <summary>Add 16-bit or 32-bit integer to st(0) (FPU).</summary>
        public static void Fiadd(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            Contract.Assert(src.Size == 2 || src.Size == 4);
            intrinsicSupport.EmitInstruction(InstructionCode.Fiadd, src);
        }

        /// <summary>Compare st(0) with 16-bit or 32-bit Integer (FPU).</summary>
        public static void Ficom(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            Contract.Assert(src.Size == 2 || src.Size == 4);
            intrinsicSupport.EmitInstruction(InstructionCode.Ficom, src);
        }

        /// <summary>Compare st(0) with 16-bit or 32-bit Integer and pop the stack (FPU).</summary>
        public static void Ficomp(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            Contract.Assert(src.Size == 2 || src.Size == 4);
            intrinsicSupport.EmitInstruction(InstructionCode.Ficomp, src);
        }

        /// <summary>Divide st(0) by 32-bit or 16-bit integer (@a src) (FPU).</summary>
        public static void Fidiv(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            Contract.Assert(src.Size == 2 || src.Size == 4);
            intrinsicSupport.EmitInstruction(InstructionCode.Fidiv, src);
        }

        /// <summary>Reverse Divide st(0) by 32-bit or 16-bit integer (@a src) (FPU).</summary>
        public static void Fidivr(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            Contract.Assert(src.Size == 2 || src.Size == 4);
            intrinsicSupport.EmitInstruction(InstructionCode.Fidivr, src);
        }

        /// <summary>Load 16-bit, 32-bit or 64-bit Integer and push it to the stack (FPU).</summary>
        /// <remarks>
        /// Converts the signed-integer source operand into double extended-precision
        /// floating point format and pushes the value onto the FPU register stack.
        /// The source operand can be a word, doubleword, or quadword integer. It is
        /// loaded without rounding errors. The sign of the source operand is
        /// preserved.
        /// </remarks>
        public static void Fild(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            Contract.Assert(src.Size == 2 || src.Size == 4 || src.Size == 8);
            intrinsicSupport.EmitInstruction(InstructionCode.Fild, src);
        }

        /// <summary>Multiply st(0) by 16-bit or 32-bit integer and store it
        /// to st(0) (FPU).</summary>
        public static void Fimul(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            Contract.Assert(src.Size == 2 || src.Size == 4);
            intrinsicSupport.EmitInstruction(InstructionCode.Fimul, src);
        }

        /// <summary>Increment Stack-Top Pointer (FPU).</summary>
        /// <remarks>
        /// Adds one to the TOP field of the FPU status word (increments the
        /// top-of-stack pointer). If the TOP field contains a 7, it is set to 0.
        /// The effect of this instruction is to rotate the stack by one position.
        /// The contents of the FPU data registers and tag register are not affected.
        /// This operation is not equivalent to popping the stack, because the tag
        /// for the previous top-of-stack register is not marked empty.
        /// </remarks>
        public static void Fincstp(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fincstp);
        }

        /// <summary>Initialize Floating-Point Unit (FPU).</summary>
        /// <remarks>
        /// Initialize FPU after checking for pending unmasked floating-point
        /// exceptions.
        /// </remarks>
        public static void Finit(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Finit);
        }

        /// <summary>Subtract 16-bit or 32-bit integer from st(0) and store result to
        /// st(0) (FPU).</summary>
        public static void Fisub(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            Contract.Assert(src.Size == 2 || src.Size == 4);
            intrinsicSupport.EmitInstruction(InstructionCode.Fisub, src);
        }

        /// <summary>Reverse Subtract 16-bit or 32-bit integer from st(0) and
        /// store result to  st(0) (FPU).</summary>
        public static void Fisubr(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            Contract.Assert(src.Size == 2 || src.Size == 4);
            intrinsicSupport.EmitInstruction(InstructionCode.Fisubr, src);
        }

        /// <summary>Initialize Floating-Point Unit (FPU).</summary>
        /// <remarks>
        /// Initialize FPU without checking for pending unmasked floating-point
        /// exceptions.
        /// </remarks>
        public static void Fninit(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fninit);
        }

        /// <summary>Store st(0) as 16-bit or 32-bit Integer to @a dst (FPU).</summary>
        public static void Fist(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Assert(dst.Size == 2 || dst.Size == 4);
            intrinsicSupport.EmitInstruction(InstructionCode.Fist, dst);
        }

        /// <summary>Store st(0) as 16-bit, 32-bit or 64-bit Integer to @a dst and pop
        /// stack (FPU).</summary>
        public static void Fistp(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Assert(dst.Size == 2 || dst.Size == 4 || dst.Size == 8);
            intrinsicSupport.EmitInstruction(InstructionCode.Fistp, dst);
        }

        /// <summary>Push 32-bit, 64-bit or 80-bit Floating Point Value onto the FPU
        /// register stack (FPU).</summary>
        public static void Fld(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            Contract.Assert(src.Size == 4 || src.Size == 8 || src.Size == 10);
            intrinsicSupport.EmitInstruction(InstructionCode.Fld, src);
        }

        /// <summary>Push @a reg onto the FPU register stack (FPU).</summary>
        public static void Fld<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fld, reg);
        }

        /// <summary>Push +1.0 onto the FPU register stack (FPU).</summary>
        public static void Fld1(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fld1);
        }

        /// <summary>Push log2(10) onto the FPU register stack (FPU).</summary>
        public static void Fldl2t(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fldl2t);
        }

        /// <summary>Push log2(e) onto the FPU register stack (FPU).</summary>
        public static void Fldl2e(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fldl2e);
        }

        /// <summary>Push pi onto the FPU register stack (FPU).</summary>
        public static void Fldpi(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fldpi);
        }

        /// <summary>Push log10(2) onto the FPU register stack (FPU).</summary>
        public static void Fldlg2(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fldlg2);
        }

        /// <summary>Push ln(2) onto the FPU register stack (FPU).</summary>
        public static void Fldln2(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fldln2);
        }

        /// <summary>Push +0.0 onto the FPU register stack (FPU).</summary>
        public static void Fldz(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fldz);
        }

        /// <summary>Load x87 FPU Control Word (2 bytes) (FPU).</summary>
        public static void Fldcw(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fldcw, src);
        }

        /// <summary>Load x87 FPU Environment (14 or 28 bytes) (FPU).</summary>
        public static void Fldenv(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fldenv, src);
        }

        /// <summary>Multiply @a dst by @a src and store result in @a dst (FPU).</summary>
        ///
        /// <remarks>One of dst or src must be st(0).</remarks>
        public static void Fmul(this IX87IntrinsicSupport<X87Reg> intrinsicSupport, X87Reg dst, X87Reg src)
        {
            Contract.Assert(dst.RegisterIndex == 0 || src.RegisterIndex == 0);
            intrinsicSupport.EmitInstruction(InstructionCode.Fmul, dst, src);
        }
        /// <summary>Multiply st(0) by @a src and store result in st(0) (FPU).</summary>
        ///
        /// <remarks>SP-FP or DP-FP determined by @a adr size.</remarks>
        public static void Fmul(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fmul, src);
        }

        /// <summary>Multiply st(0) by @a dst and POP register stack (FPU).</summary>
        public static void Fmulp(this IX87IntrinsicSupport<X87Reg> intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fmulp, Register.st((RegIndex)1));
        }

        public static void Fmulp<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 dst)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fmulp, dst);
        }

        /// <summary>Clear Exceptions (FPU).</summary>
        /// <remarks>
        /// Clear floating-point exception flags without checking for pending
        /// unmasked floating-point exceptions.
        ///
        /// Clears the floating-point exception flags (PE, UE, OE, ZE, DE, and IE),
        /// the exception summary status flag (ES), the stack fault flag (SF), and
        /// the busy flag (B) in the FPU status word. The FCLEX instruction does
        /// not checks for and handles any pending unmasked floating-point exceptions
        /// before clearing the exception flags.
        /// </remarks>
        public static void Fnclex(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fnclex);
        }

        /// <summary>No Operation (FPU).</summary>
        public static void Fnop(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fnop);
        }

        /// <summary>Save FPU State (FPU).</summary>
        /// <remarks>
        /// Store FPU environment to m94byte or m108byte without
        /// checking for pending unmasked FP exceptions.
        /// Then re-initialize the FPU.
        /// </remarks>
        public static void Fnsave(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fnsave, dst);
        }

        /// <summary>Store x87 FPU Environment (FPU).</summary>
        /// <remarks>
        /// Store FPU environment to @a dst (14 or 28 Bytes) without checking for
        /// pending unmasked floating-point exceptions. Then mask all floating
        /// point exceptions.
        /// </remarks>
        public static void Fnstenv(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fnstenv, dst);
        }

        /// <summary>Store x87 FPU Control Word (FPU).</summary>
        /// <remarks>
        /// Store FPU control word to @a dst (2 Bytes) without checking for pending
        /// unmasked floating-point exceptions.
        /// </remarks>
        public static void Fnstcw(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fnstcw, dst);
        }

        /// <summary>Store x87 FPU Status Word (2 Bytes) (FPU).</summary>
        public static void Fnstsw<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Assert(dst.IsRegCode(RegCode.AX));
            intrinsicSupport.EmitInstruction(InstructionCode.Fnstsw, dst);
        }
        /// <summary>Store x87 FPU Status Word (2 Bytes) (FPU).</summary>
        public static void Fnstsw(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fnstsw, dst);
        }

        /// <summary>Partial Arctangent (FPU).</summary>
        /// <remarks>
        /// Replace st(1) with arctan(st(1)/st(0)) and pop the register stack.
        /// </remarks>
        public static void Fpatan(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fpatan);
        }

        /// <summary>Partial Remainder (FPU).</summary>
        /// <remarks>
        /// Replace st(0) with the remainder obtained from dividing st(0) by st(1).
        /// </remarks>
        public static void Fprem(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fprem);
        }

        /// <summary>Partial Remainder (FPU).</summary>
        /// <remarks>
        /// Replace st(0) with the IEEE remainder obtained from dividing st(0) by
        /// st(1).
        /// </remarks>
        public static void Fprem1(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fprem1);
        }

        /// <summary>Partial Tangent (FPU).</summary>
        /// <remarks>
        /// Replace st(0) with its tangent and push 1 onto the FPU stack.
        /// </remarks>
        public static void Fptan(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fptan);
        }

        /// <summary>Round to Integer (FPU).</summary>
        /// <remarks>
        /// Rount st(0) to an Integer.
        /// </remarks>
        public static void Frndint(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Frndint);
        }

        /// <summary>Restore FPU State (FPU).</summary>
        /// <remarks>
        /// Load FPU state from src (94 or 108 bytes).
        /// </remarks>
        public static void Frstor(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Frstor, src);
        }

        /// <summary>Save FPU State (FPU).</summary>
        /// <remarks>
        /// Store FPU state to 94 or 108-bytes after checking for
        /// pending unmasked FP exceptions. Then reinitialize
        /// the FPU.
        /// </remarks>
        public static void Fsave(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fsave, dst);
        }

        /// <summary>Scale (FPU).</summary>
        /// <remarks>
        /// Scale st(0) by st(1).
        /// </remarks>
        public static void Fscale(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fscale);
        }

        /// <summary>Sine (FPU).</summary>
        /// <remarks>
        /// This instruction calculates the sine of the source operand in
        /// register st(0) and stores the result in st(0).
        /// </remarks>
        public static void Fsin(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fsin);
        }

        /// <summary>Sine and Cosine (FPU).</summary>
        /// <remarks>
        /// Compute the sine and cosine of st(0); replace st(0) with
        /// the sine, and push the cosine onto the register stack.
        /// </remarks>
        public static void Fsincos(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fsincos);
        }

        /// <summary>Square Root (FPU).</summary>
        /// <remarks>
        /// Calculates square root of st(0) and stores the result in st(0).
        /// </remarks>
        public static void Fsqrt(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fsqrt);
        }

        /// <summary>Store Floating Point Value (FPU).</summary>
        /// <remarks>
        /// Store st(0) as 32-bit or 64-bit floating point value to @a dst.
        /// </remarks>
        public static void Fst(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Assert(dst.Size == 4 || dst.Size == 8);
            intrinsicSupport.EmitInstruction(InstructionCode.Fst, dst);
        }

        /// <summary>Store Floating Point Value (FPU).</summary>
        /// <remarks>
        /// Store st(0) to @a reg.
        /// </remarks>
        public static void Fst<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fst, reg);
        }

        /// <summary>Store Floating Point Value and Pop Register Stack (FPU).</summary>
        /// <remarks>
        /// Store st(0) as 32-bit or 64-bit floating point value to @a dst
        /// and pop register stack.
        /// </remarks>
        public static void Fstp(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            Contract.Assert(dst.Size == 4 || dst.Size == 8 || dst.Size == 10);
            intrinsicSupport.EmitInstruction(InstructionCode.Fstp, dst);
        }

        /// <summary>Store Floating Point Value and Pop Register Stack (FPU).</summary>
        /// <remarks>
        /// Store st(0) to @a reg and pop register stack.
        /// </remarks>
        public static void Fstp<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fstp, reg);
        }

        /// <summary>Store x87 FPU Control Word (FPU).</summary>
        /// <remarks>
        /// Store FPU control word to @a dst (2 Bytes) after checking for pending
        /// unmasked floating-point exceptions.
        /// </remarks>
        public static void Fstcw(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fstcw, dst);
        }

        /// <summary>Store x87 FPU Environment (FPU).</summary>
        /// <remarks>
        /// Store FPU environment to @a dst (14 or 28 Bytes) after checking for
        /// pending unmasked floating-point exceptions. Then mask all floating
        /// point exceptions.
        /// </remarks>
        public static void Fstenv(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fstenv, dst);
        }

        /// <summary>Store x87 FPU Status Word (2 Bytes) (FPU).</summary>
        public static void Fstsw<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst)
            where TGP : Operand, IGpOperand
        {
            Contract.Assert(dst.IsRegCode(RegCode.AX));
            intrinsicSupport.EmitInstruction(InstructionCode.Fstsw, dst);
        }

        /// <summary>Store x87 FPU Status Word (2 Bytes) (FPU).</summary>
        public static void Fstsw(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fstsw, dst);
        }

        /// <summary>Subtract @a src from @a dst and store result in @a dst (FPU).</summary>
        ///
        /// <remarks>One of dst or src must be st(0).</remarks>
        public static void Fsub(this IX87IntrinsicSupport<X87Reg> intrinsicSupport, X87Reg dst, X87Reg src)
        {
            Contract.Assert(dst.RegisterIndex == 0 || src.RegisterIndex == 0);
            intrinsicSupport.EmitInstruction(InstructionCode.Fsub, dst, src);
        }
        /// <summary>Subtract @a src from st(0) and store result in st(0) (FPU).</summary>
        ///
        /// <remarks>SP-FP or DP-FP determined by @a adr size.</remarks>
        public static void Fsub(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            Contract.Assert(src.Size == 4 || src.Size == 8);
            intrinsicSupport.EmitInstruction(InstructionCode.Fsub, src);
        }

        /// <summary>Subtract st(0) from @a dst and POP register stack (FPU).</summary>
        public static void Fsubp(this IX87IntrinsicSupport<X87Reg> intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fsubp, Register.st((RegIndex)1));
        }

        public static void Fsubp<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 dst)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fsubp, dst);
        }

        /// <summary>Reverse Subtract @a src from @a dst and store result in @a dst (FPU).</summary>
        ///
        /// <remarks>One of dst or src must be st(0).</remarks>
        public static void Fsubr(this IX87IntrinsicSupport<X87Reg> intrinsicSupport, X87Reg dst, X87Reg src)
        {
            Contract.Assert(dst.RegisterIndex == 0 || src.RegisterIndex == 0);
            intrinsicSupport.EmitInstruction(InstructionCode.Fsubr, dst, src);
        }

        /// <summary>Reverse Subtract @a src from st(0) and store result in st(0) (FPU).</summary>
        ///
        /// <remarks>SP-FP or DP-FP determined by @a adr size.</remarks>
        public static void Fsubr(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            Contract.Assert(src.Size == 4 || src.Size == 8);
            intrinsicSupport.EmitInstruction(InstructionCode.Fsubr, src);
        }

        /// <summary>Reverse Subtract st(0) from @a dst and POP register stack (FPU).</summary>
        public static void Fsubrp(this IX87IntrinsicSupport<X87Reg> intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fsubrp, Register.st((RegIndex)1));
        }

        public static void Fsubrp<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 dst)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fsubrp, dst);
        }

        /// <summary>Floating point test - Compare st(0) with 0.0. (FPU).</summary>
        public static void Ftst(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Ftst);
        }

        /// <summary>Unordered Compare st(0) with @a reg (FPU).</summary>
        public static void Fucom(this IX87IntrinsicSupport<X87Reg> intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fucom, Register.st((RegIndex)1));
        }

        public static void Fucom<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fucom, reg);
        }

        /// <summary>Unordered Compare st(0) and @a reg, check for ordered values
        /// and Set EFLAGS (FPU).</summary>
        public static void Fucomi<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fucomi, reg);
        }

        /// <summary>UnorderedCompare st(0) and @a reg, Check for ordered values
        /// and Set EFLAGS and pop the stack (FPU).</summary>
        public static void Fucomip(this IX87IntrinsicSupport<X87Reg> intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fucomip, Register.st((RegIndex)1));
        }

        public static void Fucomip<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fucomip, reg);
        }

        /// <summary>Unordered Compare st(0) with @a reg and pop register stack (FPU).</summary>
        public static void Fucomp(this IX87IntrinsicSupport<X87Reg> intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fucomp, Register.st((RegIndex)1));
        }

        public static void Fucomp<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fucomp, reg);
        }

        /// <summary>Unordered compare st(0) with st(1) and pop register stack twice
        /// (FPU).</summary>
        public static void Fucompp(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fucompp);
        }

        public static void Fwait(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fwait);
        }

        /// <summary>Examine st(0) (FPU).</summary>
        /// <remarks>
        /// Examines the contents of the ST(0) register and sets the condition code
        /// flags C0, C2, and C3 in the FPU status word to indicate the class of
        /// value or number in the register.
        /// </remarks>
        public static void Fxam(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fxam);
        }

        /// <summary>Exchange Register Contents (FPU).</summary>
        /// <remarks>
        /// Exchange content of st(0) with @a reg.
        /// </remarks>
        public static void Fxch(this IX87IntrinsicSupport<X87Reg> intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fxch, Register.st((RegIndex)1));
        }

        public static void Fxch<TX87>(this IX87IntrinsicSupport<TX87> intrinsicSupport, TX87 reg)
            where TX87 : Operand, IX87Operand
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fxch, reg);
        }

        /// <summary>Restore FP And MMX(tm) State And Streaming SIMD Extension State
        /// (FPU, MMX, SSE).</summary>
        /// <remarks>
        /// Load FP and MMX(tm) technology and Streaming SIMD Extension state from
        /// src (512 bytes).
        /// </remarks>
        public static void Fxrstor(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fxrstor, src);
        }

        /// <summary>Store FP and MMX(tm) State and Streaming SIMD Extension State
        /// (FPU, MMX, SSE).</summary>
        /// <remarks>
        /// Store FP and MMX(tm) technology state and Streaming SIMD Extension state
        /// to dst (512 bytes).
        /// </remarks>
        public static void Fxsave(this IIntrinsicSupport intrinsicSupport, Mem dst)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fxsave, dst);
        }

        /// <summary>Extract Exponent and Significand (FPU).</summary>
        /// <remarks>
        /// Separate value in st(0) into exponent and significand, store exponent
        /// in st(0), and push the significand onto the register stack.
        /// </remarks>
        public static void Fxtract(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fxtract);
        }

        /// <summary>Compute y * log2(x).</summary>
        /// <remarks>
        /// Replace st(1) with (st(1) * log2st(0)) and pop the register stack.
        /// </remarks>
        public static void Fyl2x(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fyl2x);
        }

        /// <summary>Compute y * log_2(x+1).</summary>
        /// <remarks>
        /// Replace st(1) with (st(1) * (log2st(0) + 1.0)) and pop the register stack.
        /// </remarks>
        public static void Fyl2xp1(this IIntrinsicSupport intrinsicSupport)
        {
            intrinsicSupport.EmitInstruction(InstructionCode.Fyl2xp1);
        }

        #endregion

        #region MMX Instructions

        public static void Emms(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Emms);
        }

        public static void Movd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, Mem dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movd, dst, src);
        }

        public static void Movd<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, TMM src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movd, dst, src);
        }

        public static void Movd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movd, dst, src);
        }

        public static void Movd<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TMM dst, TGP src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movd, dst, src);
        }

        public static void Movq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movq, dst, src);
        }

        public static void Movq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, Mem dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movq, dst, src);
        }

        public static void Movq<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, TMM src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.Movq));

            intrinsicSupport.EmitInstruction(InstructionCode.Movq, dst, src);
        }

        public static void Movq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movq, dst, src);
        }

        public static void Movq<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TMM dst, TGP src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.Movq));

            intrinsicSupport.EmitInstruction(InstructionCode.Movq, dst, src);
        }

        /// <summary>Pack with Signed Saturation (MMX).</summary>
        public static void packsswb<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TMM dst, TMM src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Packsswb, dst, src);
        }

        /// <summary>Pack with Signed Saturation (MMX).</summary>
        public static void packsswb<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TMM dst, Mem src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Packsswb, dst, src);
        }

        /// <summary>Pack with Signed Saturation (MMX).</summary>
        public static void packssdw<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TMM dst, TMM src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Packssdw, dst, src);
        }

        /// <summary>Pack with Signed Saturation (MMX).</summary>
        public static void packssdw<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TMM dst, Mem src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Packssdw, dst, src);
        }

        public static void Packuswb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Packuswb, dst, src);
        }

        public static void Packuswb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Packuswb, dst, src);
        }

        public static void Paddb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddb, dst, src);
        }

        public static void Paddb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddb, dst, src);
        }

        public static void Paddw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddw, dst, src);
        }

        public static void Paddw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddw, dst, src);
        }

        public static void Paddd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddd, dst, src);
        }

        public static void Paddd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddd, dst, src);
        }

        public static void Paddsb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddsb, dst, src);
        }

        public static void Paddsb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddsb, dst, src);
        }

        public static void Paddsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddsw, dst, src);
        }

        public static void Paddsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddsw, dst, src);
        }

        public static void Paddusb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddusb, dst, src);
        }

        public static void Paddusb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddusb, dst, src);
        }

        public static void Paddusw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddusw, dst, src);
        }

        public static void Paddusw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddusw, dst, src);
        }

        public static void Pand<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pand, dst, src);
        }

        public static void Pand<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pand, dst, src);
        }

        public static void Pandn<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pandn, dst, src);
        }

        public static void Pandn<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pandn, dst, src);
        }

        public static void Pcmpeqb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpeqb, dst, src);
        }

        public static void Pcmpeqb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpeqb, dst, src);
        }

        public static void Pcmpeqw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpeqw, dst, src);
        }

        public static void Pcmpeqw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpeqw, dst, src);
        }

        public static void Pcmpeqd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpeqd, dst, src);
        }

        public static void Pcmpeqd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpeqd, dst, src);
        }

        public static void Pcmpgtb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpgtb, dst, src);
        }

        public static void Pcmpgtb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpgtb, dst, src);
        }

        public static void Pcmpgtw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpgtw, dst, src);
        }

        public static void Pcmpgtw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpgtw, dst, src);
        }

        public static void Pcmpgtd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpgtd, dst, src);
        }

        public static void Pcmpgtd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpgtd, dst, src);
        }

        public static void Pmulhw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmulhw, dst, src);
        }

        public static void Pmulhw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmulhw, dst, src);
        }

        public static void Pmullw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmullw, dst, src);
        }

        public static void Pmullw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmullw, dst, src);
        }

        public static void Por<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Por, dst, src);
        }

        public static void Por<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Por, dst, src);
        }

        public static void Pmaddwd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaddwd, dst, src);
        }

        public static void Pmaddwd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaddwd, dst, src);
        }

        public static void Pslld<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pslld, dst, src);
        }

        public static void Pslld<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pslld, dst, src);
        }

        public static void Pslld<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Imm src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pslld, dst, src);
        }

        public static void Psllq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psllq, dst, src);
        }

        public static void Psllq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psllq, dst, src);
        }

        public static void Psllq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Imm src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psllq, dst, src);
        }

        public static void Psllw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psllw, dst, src);
        }

        public static void Psllw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psllw, dst, src);
        }

        public static void Psllw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Imm src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psllw, dst, src);
        }

        public static void Psrad<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrad, dst, src);
        }

        public static void Psrad<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrad, dst, src);
        }

        public static void Psrad<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Imm src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrad, dst, src);
        }

        public static void Psraw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psraw, dst, src);
        }

        public static void Psraw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psraw, dst, src);
        }

        public static void Psraw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Imm src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psraw, dst, src);
        }

        public static void Psrld<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrld, dst, src);
        }

        public static void Psrld<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrld, dst, src);
        }

        public static void Psrld<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Imm src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrld, dst, src);
        }

        public static void Psrlq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrlq, dst, src);
        }

        public static void Psrlq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrlq, dst, src);
        }

        public static void Psrlq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Imm src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrlq, dst, src);
        }

        public static void Psrlw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrlw, dst, src);
        }

        public static void Psrlw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrlw, dst, src);
        }

        public static void Psrlw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Imm src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrlw, dst, src);
        }

        public static void Psubb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubb, dst, src);
        }

        public static void Psubb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubb, dst, src);
        }

        public static void Psubw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubw, dst, src);
        }

        public static void Psubw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubw, dst, src);
        }

        public static void Psubd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubd, dst, src);
        }

        public static void Psubd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubd, dst, src);
        }

        public static void Psubsb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubsb, dst, src);
        }

        public static void Psubsb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubsb, dst, src);
        }

        public static void Psubsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubsw, dst, src);
        }

        public static void Psubsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubsw, dst, src);
        }

        public static void Psubusb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubusb, dst, src);
        }

        public static void Psubusb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubusb, dst, src);
        }

        public static void Psubusw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubusw, dst, src);
        }

        public static void Psubusw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubusw, dst, src);
        }

        public static void Punpckhbw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckhbw, dst, src);
        }

        public static void Punpckhbw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckhbw, dst, src);
        }

        public static void Punpckhwd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckhwd, dst, src);
        }

        public static void Punpckhwd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckhwd, dst, src);
        }

        public static void Punpckhdq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckhdq, dst, src);
        }

        public static void Punpckhdq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckhdq, dst, src);
        }

        public static void Punpcklbw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpcklbw, dst, src);
        }

        public static void Punpcklbw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpcklbw, dst, src);
        }

        public static void Punpcklwd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpcklwd, dst, src);
        }

        public static void Punpcklwd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpcklwd, dst, src);
        }

        public static void Punpckldq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckldq, dst, src);
        }

        public static void Punpckldq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckldq, dst, src);
        }

        public static void Pxor<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pxor, dst, src);
        }

        public static void Pxor<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pxor, dst, src);
        }

        #endregion

        #region AMD 3D-Now! Instructions

        public static void Femms(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Femms);
        }

        public static void Pf2id<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pf2id, dst, src);
        }

        public static void Pf2id<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pf2id, dst, src);
        }

        public static void Pf2iw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pf2iw, dst, src);
        }

        public static void Pf2iw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pf2iw, dst, src);
        }

        public static void Pfacc<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfacc, dst, src);
        }

        public static void Pfacc<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfacc, dst, src);
        }

        public static void Pfadd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfadd, dst, src);
        }

        public static void Pfadd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfadd, dst, src);
        }

        public static void Pfcmpeq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfcmpeq, dst, src);
        }

        public static void Pfcmpeq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfcmpeq, dst, src);
        }

        public static void Pfcmpge<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfcmpge, dst, src);
        }

        public static void Pfcmpge<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfcmpge, dst, src);
        }

        public static void Pfcmpgt<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfcmpgt, dst, src);
        }

        public static void Pfcmpgt<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfcmpgt, dst, src);
        }

        public static void Pfmax<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfmax, dst, src);
        }

        public static void Pfmax<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfmax, dst, src);
        }

        public static void Pfmin<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfmin, dst, src);
        }

        public static void Pfmin<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfmin, dst, src);
        }

        public static void Pfmul<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfmul, dst, src);
        }

        public static void Pfmul<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfmul, dst, src);
        }

        public static void Pfnacc<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfnacc, dst, src);
        }

        public static void Pfnacc<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfnacc, dst, src);
        }

        public static void Pfpnacc<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfpnacc, dst, src);
        }

        public static void Pfpnacc<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfpnacc, dst, src);
        }

        public static void Pfrcp<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfrcp, dst, src);
        }

        public static void Pfrcp<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfrcp, dst, src);
        }

        public static void Pfrcpit1<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfrcpit1, dst, src);
        }

        public static void Pfrcpit1<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfrcpit1, dst, src);
        }

        public static void Pfrcpit2<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfrcpit2, dst, src);
        }

        public static void Pfrcpit2<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfrcpit2, dst, src);
        }

        public static void Pfrsqit1<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfrsqit1, dst, src);
        }

        public static void Pfrsqit1<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfrsqit1, dst, src);
        }

        public static void Pfrsqrt<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfrsqrt, dst, src);
        }

        public static void Pfrsqrt<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfrsqrt, dst, src);
        }

        public static void Pfsub<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfsub, dst, src);
        }

        public static void Pfsub<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfsub, dst, src);
        }

        public static void Pfsubr<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfsubr, dst, src);
        }

        public static void Pfsubr<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pfsubr, dst, src);
        }

        public static void Pi2fd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pi2fd, dst, src);
        }

        public static void Pi2fd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pi2fd, dst, src);
        }

        public static void Pi2fw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pi2fw, dst, src);
        }

        public static void Pi2fw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pi2fw, dst, src);
        }

        public static void Pswapd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pswapd, dst, src);
        }

        public static void Pswapd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pswapd, dst, src);
        }

        #endregion

        #region SSE Instructions

        public static void Addps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Addps, dst, src);
        }

        public static void Addps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Addps, dst, src);
        }

        public static void Addss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Addss, dst, src);
        }

        public static void Addss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Addss, dst, src);
        }

        public static void Andnps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Andnps, dst, src);
        }

        public static void Andnps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Andnps, dst, src);
        }

        public static void Andps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Andps, dst, src);
        }

        public static void Andps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Andps, dst, src);
        }

        public static void Cmpps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmpps, dst, src, imm8);
        }

        public static void Cmpps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmpps, dst, src, imm8);
        }

        public static void Cmpss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmpss, dst, src, imm8);
        }

        public static void Cmpss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmpss, dst, src, imm8);
        }

        public static void Comiss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Comiss, dst, src);
        }

        public static void Comiss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Comiss, dst, src);
        }

        public static void Cvtpi2ps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtpi2ps, dst, src);
        }

        public static void Cvtpi2ps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtpi2ps, dst, src);
        }

        public static void Cvtps2pi<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtps2pi, dst, src);
        }

        public static void Cvtps2pi<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtps2pi, dst, src);
        }

        public static void Cvtsi2ss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtsi2ss, dst, src);
        }

        public static void Cvtsi2ss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtsi2ss, dst, src);
        }

        public static void Cvtss2si<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtss2si, dst, src);
        }

        public static void Cvtss2si<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtss2si, dst, src);
        }

        public static void Cvttps2pi<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvttps2pi, dst, src);
        }

        public static void Cvttps2pi<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvttps2pi, dst, src);
        }

        public static void Cvttss2si<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvttss2si, dst, src);
        }

        public static void Cvttss2si<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvttss2si, dst, src);
        }

        public static void Divps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Divps, dst, src);
        }

        public static void Divps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Divps, dst, src);
        }

        public static void Divss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Divss, dst, src);
        }

        public static void Divss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Divss, dst, src);
        }

        public static void Ldmxcsr(this IIntrinsicSupport intrinsicSupport, Mem src)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Ldmxcsr, src);
        }

        public static void Maskmovq<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst_ptr, TMM data, TMM mask)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst_ptr != null);
            Contract.Requires(data != null);
            Contract.Requires(mask != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Maskmovq, dst_ptr, data, mask);
        }

        public static void Maxps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Maxps, dst, src);
        }

        public static void Maxps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Maxps, dst, src);
        }

        public static void Maxss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Maxss, dst, src);
        }

        public static void Maxss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Maxss, dst, src);
        }

        public static void Minps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Minps, dst, src);
        }

        public static void Minps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Minps, dst, src);
        }

        public static void Minss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Minss, dst, src);
        }

        public static void Minss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Minss, dst, src);
        }

        public static void Movaps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movaps, dst, src);
        }

        public static void Movaps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movaps, dst, src);
        }

        public static void Movaps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movaps, dst, src);
        }

        public static void Movd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movd, dst, src);
        }

        public static void Movd<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movd, dst, src);
        }

        public static void Movd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movd, dst, src);
        }

        public static void Movd<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TXMM dst, TGP src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movd, dst, src);
        }

        public static void Movq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movq, dst, src);
        }

        public static void Movq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movq, dst, src);
        }

        public static void Movq<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, TXMM src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.Movq));

            intrinsicSupport.EmitInstruction(InstructionCode.Movq, dst, src);
        }

        public static void Movq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movq, dst, src);
        }

        public static void Movq<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TXMM dst, TGP src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            if (!Util.IsX64)
                throw new NotSupportedException(string.Format("The '{0}' instruction is only supported on X64.", InstructionCode.Movq));

            intrinsicSupport.EmitInstruction(InstructionCode.Movq, dst, src);
        }

        public static void Movntq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, Mem dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movntq, dst, src);
        }

        public static void Movhlps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movhlps, dst, src);
        }

        public static void Movhps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movhps, dst, src);
        }

        public static void Movhps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movhps, dst, src);
        }

        public static void Movlhps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movlhps, dst, src);
        }

        public static void Movlps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movlps, dst, src);
        }

        public static void Movlps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movlps, dst, src);
        }

        public static void Movntps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movntps, dst, src);
        }

        public static void Movss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movss, dst, src);
        }

        public static void Movss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movss, dst, src);
        }

        public static void Movss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movss, dst, src);
        }

        public static void Movups<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movups, dst, src);
        }

        public static void Movups<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movups, dst, src);
        }

        public static void Movups<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movups, dst, src);
        }

        public static void Mulps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mulps, dst, src);
        }

        public static void Mulps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mulps, dst, src);
        }

        public static void Mulss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mulss, dst, src);
        }

        public static void Mulss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mulss, dst, src);
        }

        public static void Orps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Orps, dst, src);
        }

        public static void Orps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Orps, dst, src);
        }

        public static void Pavgb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pavgb, dst, src);
        }

        public static void Pavgb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pavgb, dst, src);
        }

        public static void Pavgw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pavgw, dst, src);
        }

        public static void Pavgw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pavgw, dst, src);
        }

        public static void Pextrw<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, TMM src, Imm imm8)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pextrw, dst, src, imm8);
        }

        public static void Pinsrw<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TMM dst, TGP src, Imm imm8)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pinsrw, dst, src, imm8);
        }

        public static void Pinsrw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src, Imm imm8)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pinsrw, dst, src, imm8);
        }

        public static void Pextrw<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, TXMM src, Imm imm8)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pextrw, dst, src, imm8);
        }

        public static void Pextrw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pextrw, dst, src, imm8);
        }

        public static void Pmaxsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxsw, dst, src);
        }

        public static void Pmaxsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxsw, dst, src);
        }

        public static void Pmaxub<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxub, dst, src);
        }

        public static void Pmaxub<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxub, dst, src);
        }

        public static void Pminsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminsw, dst, src);
        }

        public static void Pminsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminsw, dst, src);
        }

        public static void Pminub<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminub, dst, src);
        }

        public static void Pminub<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminub, dst, src);
        }

        public static void Pmovmskb<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, TMM src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovmskb, dst, src);
        }

        public static void Pmulhuw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmulhuw, dst, src);
        }

        public static void Pmulhuw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmulhuw, dst, src);
        }

        public static void Psadbw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psadbw, dst, src);
        }

        public static void Psadbw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psadbw, dst, src);
        }

        public static void Pshufw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src, Imm imm8)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pshufw, dst, src, imm8);
        }

        public static void Pshufw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src, Imm imm8)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pshufw, dst, src, imm8);
        }

        public static void Rcpps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rcpps, dst, src);
        }

        public static void Rcpps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rcpps, dst, src);
        }

        public static void Rcpss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rcpss, dst, src);
        }

        public static void Rcpss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rcpss, dst, src);
        }

        public static void Prefetch(this IIntrinsicSupport intrinsicSupport, Mem mem, PrefetchHint hint)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(mem != null);
            Contract.Requires(hint >= PrefetchHint.NTA && hint <= PrefetchHint.T2);

            intrinsicSupport.EmitInstruction(InstructionCode.Prefetch, mem, (Imm)(int)hint);
        }

        public static void Psadbw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psadbw, dst, src);
        }

        public static void Psadbw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psadbw, dst, src);
        }

        public static void Rsqrtps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rsqrtps, dst, src);
        }

        public static void Rsqrtps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rsqrtps, dst, src);
        }

        public static void Rsqrtss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rsqrtss, dst, src);
        }

        public static void Rsqrtss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Rsqrtss, dst, src);
        }

        public static void Sfence(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sfence);
        }

        public static void Shufps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shufps, dst, src, imm8);
        }

        public static void Shufps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shufps, dst, src, imm8);
        }

        public static void Sqrtps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sqrtps, dst, src);
        }

        public static void Sqrtps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sqrtps, dst, src);
        }

        public static void Sqrtss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sqrtss, dst, src);
        }

        public static void Sqrtss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sqrtss, dst, src);
        }

        public static void Stmxcsr<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Stmxcsr, dst);
        }

        public static void Subps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Subps, dst, src);
        }

        public static void Subps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Subps, dst, src);
        }

        public static void Subss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Subss, dst, src);
        }

        public static void Subss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Subss, dst, src);
        }

        public static void Ucomiss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Ucomiss, dst, src);
        }

        public static void Ucomiss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Ucomiss, dst, src);
        }

        public static void Unpckhps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Unpckhps, dst, src);
        }

        public static void Unpckhps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Unpckhps, dst, src);
        }

        public static void Unpcklps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Unpcklps, dst, src);
        }

        public static void Unpcklps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Unpcklps, dst, src);
        }

        public static void Xorps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Xorps, dst, src);
        }

        public static void Xorps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Xorps, dst, src);
        }

        #endregion

        #region SSE2

        public static void Addpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Addpd, dst, src);
        }

        public static void Addpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Addpd, dst, src);
        }

        public static void Addsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Addsd, dst, src);
        }

        public static void Addsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Addsd, dst, src);
        }

        public static void Andnpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Andnpd, dst, src);
        }

        public static void Andnpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Andnpd, dst, src);
        }

        public static void Andpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Andpd, dst, src);
        }

        public static void Andpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Andpd, dst, src);
        }

        public static void Clflush(this IIntrinsicSupport intrinsicSupport, Mem mem)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(mem != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Clflush, mem);
        }

        public static void Cmppd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmppd, dst, src, imm8);
        }

        public static void Cmppd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmppd, dst, src, imm8);
        }

        public static void Cmpsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmpsd, dst, src, imm8);
        }

        public static void Cmpsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cmpsd, dst, src, imm8);
        }

        public static void Comisd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Comisd, dst, src);
        }

        public static void Comisd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Comisd, dst, src);
        }

        public static void Cvtdq2pd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtdq2pd, dst, src);
        }

        public static void Cvtdq2pd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtdq2pd, dst, src);
        }

        public static void Cvtdq2ps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtdq2ps, dst, src);
        }

        public static void Cvtdq2ps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtdq2ps, dst, src);
        }

        public static void Cvtpd2dq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtpd2dq, dst, src);
        }

        public static void Cvtpd2dq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtpd2dq, dst, src);
        }

        public static void Cvtpd2pi<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TMM dst, TXMM src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtpd2pi, dst, src);
        }

        public static void Cvtpd2pi<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtpd2pi, dst, src);
        }

        public static void Cvtpd2ps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtpd2ps, dst, src);
        }

        public static void Cvtpd2ps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtpd2ps, dst, src);
        }

        public static void Cvtpi2pd<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TXMM dst, TMM src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtpi2pd, dst, src);
        }

        public static void Cvtpi2pd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtpi2pd, dst, src);
        }

        public static void Cvtps2dq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtps2dq, dst, src);
        }

        public static void Cvtps2dq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtps2dq, dst, src);
        }

        public static void Cvtps2pd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtps2pd, dst, src);
        }

        public static void Cvtps2pd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtps2pd, dst, src);
        }

        public static void Cvtsd2si<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, TXMM src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtsd2si, dst, src);
        }

        public static void Cvtsd2si<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtsd2si, dst, src);
        }

        public static void Cvtsd2ss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtsd2ss, dst, src);
        }

        public static void Cvtsd2ss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtsd2ss, dst, src);
        }

        public static void Cvtsi2sd<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TXMM dst, TGP src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtsi2sd, dst, src);
        }

        public static void Cvtsi2sd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtsi2sd, dst, src);
        }

        public static void Cvtss2sd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtss2sd, dst, src);
        }

        public static void Cvtss2sd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvtss2sd, dst, src);
        }

        public static void Cvttpd2pi<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TMM dst, TXMM src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvttpd2pi, dst, src);
        }

        public static void Cvttpd2pi<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvttpd2pi, dst, src);
        }

        public static void Cvttpd2dq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvttpd2dq, dst, src);
        }

        public static void Cvttpd2dq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvttpd2dq, dst, src);
        }

        public static void Cvttps2dq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvttps2dq, dst, src);
        }

        public static void Cvttps2dq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvttps2dq, dst, src);
        }

        public static void Cvttsd2si<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, TXMM src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvttsd2si, dst, src);
        }

        public static void Cvttsd2si<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Cvttsd2si, dst, src);
        }

        public static void Divpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Divpd, dst, src);
        }

        public static void Divpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Divpd, dst, src);
        }

        public static void Divsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Divsd, dst, src);
        }

        public static void Divsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Divsd, dst, src);
        }

        public static void Lfence(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Lfence);
        }

        public static void Maskmovdqu<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst_ptr, TXMM src, TXMM mask)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst_ptr != null);
            Contract.Requires(src != null);
            Contract.Requires(mask != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Maskmovdqu, dst_ptr, src, mask);
        }

        public static void Maxpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Maxpd, dst, src);
        }

        public static void Maxpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Maxpd, dst, src);
        }

        public static void Maxsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Maxsd, dst, src);
        }

        public static void Maxsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Maxsd, dst, src);
        }

        public static void Mfence(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mfence);
        }

        public static void Minpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Minpd, dst, src);
        }

        public static void Minpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Minpd, dst, src);
        }

        public static void Minsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Minsd, dst, src);
        }

        public static void Minsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Minsd, dst, src);
        }

        public static void Movdqa<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movdqa, dst, src);
        }

        public static void Movdqa<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movdqa, dst, src);
        }

        public static void Movdqa<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movdqa, dst, src);
        }

        public static void Movdqu<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movdqu, dst, src);
        }

        public static void Movdqu<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movdqu, dst, src);
        }

        public static void Movdqu<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movdqu, dst, src);
        }

        public static void Movmskps<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, TXMM src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movmskps, dst, src);
        }

        public static void Movmskpd<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, TXMM src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movmskpd, dst, src);
        }

        public static void Movsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movsd, dst, src);
        }

        public static void Movsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movsd, dst, src);
        }

        public static void Movsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movsd, dst, src);
        }

        public static void Movapd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movapd, dst, src);
        }

        public static void Movapd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movapd, dst, src);
        }

        public static void Movapd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movapd, dst, src);
        }

        public static void Movdq2q<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movdq2q, dst, src);
        }

        public static void Movdq2q<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movdq2q, dst, src);
        }

        public static void Movhpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movhpd, dst, src);
        }

        public static void Movhpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movhpd, dst, src);
        }

        public static void Movlpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movlpd, dst, src);
        }

        public static void Movlpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movlpd, dst, src);
        }

        public static void Movntdq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movntdq, dst, src);
        }

        public static void Movnti<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movnti, dst, src);
        }

        public static void Movntpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movntpd, dst, src);
        }

        public static void Movupd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movupd, dst, src);
        }

        public static void Movupd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movupd, dst, src);
        }

        public static void Movupd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movupd, dst, src);
        }

        public static void Mulpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mulpd, dst, src);
        }

        public static void Mulpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mulpd, dst, src);
        }

        public static void Mulsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mulsd, dst, src);
        }

        public static void Mulsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mulsd, dst, src);
        }

        public static void Orpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Orpd, dst, src);
        }

        public static void Orpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Orpd, dst, src);
        }

        public static void Packsswb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Packsswb, dst, src);
        }

        public static void Packsswb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Packsswb, dst, src);
        }

        public static void Packssdw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Packssdw, dst, src);
        }

        public static void Packssdw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Packssdw, dst, src);
        }

        public static void Packuswb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Packuswb, dst, src);
        }

        public static void Packuswb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Packuswb, dst, src);
        }

        public static void Paddb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddb, dst, src);
        }

        public static void Paddb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddb, dst, src);
        }

        public static void Paddw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddw, dst, src);
        }

        public static void Paddw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddw, dst, src);
        }

        public static void Paddd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddd, dst, src);
        }

        public static void Paddd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddd, dst, src);
        }

        public static void Paddq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddq, dst, src);
        }

        public static void Paddq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddq, dst, src);
        }

        public static void Paddq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddq, dst, src);
        }

        public static void Paddq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddq, dst, src);
        }

        public static void Paddsb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddsb, dst, src);
        }

        public static void Paddsb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddsb, dst, src);
        }

        public static void Paddsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddsw, dst, src);
        }

        public static void Paddsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddsw, dst, src);
        }

        public static void Paddusb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddusb, dst, src);
        }

        public static void Paddusb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddusb, dst, src);
        }

        public static void Paddusw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddusw, dst, src);
        }

        public static void Paddusw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Paddusw, dst, src);
        }

        public static void Pand<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pand, dst, src);
        }

        public static void Pand<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pand, dst, src);
        }

        public static void Pandn<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pandn, dst, src);
        }

        public static void Pandn<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pandn, dst, src);
        }

        public static void Pause(this IIntrinsicSupport intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pause);
        }

        public static void Pavgb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pavgb, dst, src);
        }

        public static void Pavgb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pavgb, dst, src);
        }

        public static void Pavgw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pavgw, dst, src);
        }

        public static void Pavgw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pavgw, dst, src);
        }

        public static void Pcmpeqb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpeqb, dst, src);
        }

        public static void Pcmpeqb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpeqb, dst, src);
        }

        public static void Pcmpeqw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpeqw, dst, src);
        }

        public static void Pcmpeqw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpeqw, dst, src);
        }

        public static void Pcmpeqd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpeqd, dst, src);
        }

        public static void Pcmpeqd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpeqd, dst, src);
        }

        public static void Pcmpgtb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpgtb, dst, src);
        }

        public static void Pcmpgtb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpgtb, dst, src);
        }

        public static void Pcmpgtw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpgtw, dst, src);
        }

        public static void Pcmpgtw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpgtw, dst, src);
        }

        public static void Pcmpgtd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpgtd, dst, src);
        }

        public static void Pcmpgtd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpgtd, dst, src);
        }

        public static void Pmaxsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxsw, dst, src);
        }

        public static void Pmaxsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxsw, dst, src);
        }

        public static void Pmaxub<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxub, dst, src);
        }

        public static void Pmaxub<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxub, dst, src);
        }

        public static void Pminsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminsw, dst, src);
        }

        public static void Pminsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminsw, dst, src);
        }

        public static void Pminub<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminub, dst, src);
        }

        public static void Pminub<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminub, dst, src);
        }

        public static void Pmovmskb<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, TXMM src)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovmskb, dst, src);
        }

        public static void Pmulhw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmulhw, dst, src);
        }

        public static void Pmulhw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmulhw, dst, src);
        }

        public static void Pmulhuw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmulhuw, dst, src);
        }

        public static void Pmulhuw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmulhuw, dst, src);
        }

        public static void Pmullw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmullw, dst, src);
        }

        public static void Pmullw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmullw, dst, src);
        }

        public static void Pmuludq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmuludq, dst, src);
        }

        public static void Pmuludq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmuludq, dst, src);
        }

        public static void Pmuludq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmuludq, dst, src);
        }

        public static void Pmuludq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmuludq, dst, src);
        }

        public static void Por<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Por, dst, src);
        }

        public static void Por<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Por, dst, src);
        }

        public static void Pslld<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pslld, dst, src);
        }

        public static void Pslld<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pslld, dst, src);
        }

        public static void Pslld<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Imm src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pslld, dst, src);
        }

        public static void Psllq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psllq, dst, src);
        }

        public static void Psllq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psllq, dst, src);
        }

        public static void Psllq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Imm src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psllq, dst, src);
        }

        public static void Psllw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psllw, dst, src);
        }

        public static void Psllw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psllw, dst, src);
        }

        public static void Psllw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Imm src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psllw, dst, src);
        }

        public static void Pslldq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Imm src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pslldq, dst, src);
        }

        public static void Psrad<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrad, dst, src);
        }

        public static void Psrad<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrad, dst, src);
        }

        public static void Psrad<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Imm src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrad, dst, src);
        }

        public static void Psraw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psraw, dst, src);
        }

        public static void Psraw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psraw, dst, src);
        }

        public static void Psraw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Imm src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psraw, dst, src);
        }

        public static void Psubb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubb, dst, src);
        }

        public static void Psubb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubb, dst, src);
        }

        public static void Psubw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubw, dst, src);
        }

        public static void Psubw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubw, dst, src);
        }

        public static void Psubd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubd, dst, src);
        }

        public static void Psubd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubd, dst, src);
        }

        public static void Psubq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubq, dst, src);
        }

        public static void Psubq<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubq, dst, src);
        }

        public static void Psubq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubq, dst, src);
        }

        public static void Psubq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubq, dst, src);
        }

        public static void Pmaddwd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaddwd, dst, src);
        }

        public static void Pmaddwd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaddwd, dst, src);
        }

        public static void Pshufd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pshufd, dst, src, imm8);
        }

        public static void Pshufd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pshufd, dst, src, imm8);
        }

        public static void Pshufhw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pshufhw, dst, src, imm8);
        }

        public static void Pshufhw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pshufhw, dst, src, imm8);
        }

        public static void Pshuflw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pshuflw, dst, src, imm8);
        }

        public static void Pshuflw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pshuflw, dst, src, imm8);
        }

        public static void Psrld<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrld, dst, src);
        }

        public static void Psrld<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrld, dst, src);
        }

        public static void Psrld<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Imm src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrld, dst, src);
        }

        public static void Psrlq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrlq, dst, src);
        }

        public static void Psrlq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrlq, dst, src);
        }

        public static void Psrlq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Imm src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrlq, dst, src);
        }

        public static void Psrldq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Imm src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrldq, dst, src);
        }

        public static void Psrlw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrlw, dst, src);
        }

        public static void Psrlw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrlw, dst, src);
        }

        public static void Psrlw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Imm src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psrlw, dst, src);
        }

        public static void Psubsb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubsb, dst, src);
        }

        public static void Psubsb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubsb, dst, src);
        }

        public static void Psubsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubsw, dst, src);
        }

        public static void Psubsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubsw, dst, src);
        }

        public static void Psubusb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubusb, dst, src);
        }

        public static void Psubusb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubusb, dst, src);
        }

        public static void Psubusw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubusw, dst, src);
        }

        public static void Psubusw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psubusw, dst, src);
        }

        public static void Punpckhbw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckhbw, dst, src);
        }

        public static void Punpckhbw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckhbw, dst, src);
        }

        public static void Punpckhwd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckhwd, dst, src);
        }

        public static void Punpckhwd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckhwd, dst, src);
        }

        public static void Punpckhdq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckhdq, dst, src);
        }

        public static void Punpckhdq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckhdq, dst, src);
        }

        public static void Punpckhqdq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckhqdq, dst, src);
        }

        public static void Punpckhqdq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckhqdq, dst, src);
        }

        public static void Punpcklbw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpcklbw, dst, src);
        }

        public static void Punpcklbw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpcklbw, dst, src);
        }

        public static void Punpcklwd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpcklwd, dst, src);
        }

        public static void Punpcklwd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpcklwd, dst, src);
        }

        public static void Punpckldq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckldq, dst, src);
        }

        public static void Punpckldq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpckldq, dst, src);
        }

        public static void Punpcklqdq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpcklqdq, dst, src);
        }

        public static void Punpcklqdq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Punpcklqdq, dst, src);
        }

        public static void Pxor<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pxor, dst, src);
        }

        public static void Pxor<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pxor, dst, src);
        }

        public static void Shufpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shufpd, dst, src, imm8);
        }

        public static void Shufpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Shufpd, dst, src, imm8);
        }

        public static void Sqrtpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sqrtpd, dst, src);
        }

        public static void Sqrtpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sqrtpd, dst, src);
        }

        public static void Sqrtsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sqrtsd, dst, src);
        }

        public static void Sqrtsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Sqrtsd, dst, src);
        }

        public static void Subpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Subpd, dst, src);
        }

        public static void Subpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Subpd, dst, src);
        }

        public static void Subsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Subsd, dst, src);
        }

        public static void Subsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Subsd, dst, src);
        }

        public static void Ucomisd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Ucomisd, dst, src);
        }

        public static void Ucomisd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Ucomisd, dst, src);
        }

        public static void Unpckhpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Unpckhpd, dst, src);
        }

        public static void Unpckhpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Unpckhpd, dst, src);
        }

        public static void Unpcklpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Unpcklpd, dst, src);
        }

        public static void Unpcklpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Unpcklpd, dst, src);
        }

        public static void Xorpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Xorpd, dst, src);
        }

        public static void Xorpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Xorpd, dst, src);
        }

        #endregion

        #region SSE3

        public static void Addsubpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Addsubpd, dst, src);
        }

        public static void Addsubpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Addsubpd, dst, src);
        }

        public static void Addsubps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Addsubps, dst, src);
        }

        public static void Addsubps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Addsubps, dst, src);
        }

        public static void Fisttp(this Assembler intrinsicSupport, Mem dst)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Fisttp, dst);
        }

        public static void Haddpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Haddpd, dst, src);
        }

        public static void Haddpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Haddpd, dst, src);
        }

        public static void Haddps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Haddps, dst, src);
        }

        public static void Haddps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Haddps, dst, src);
        }

        public static void Hsubpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Hsubpd, dst, src);
        }

        public static void Hsubpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Hsubpd, dst, src);
        }

        public static void Hsubps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Hsubps, dst, src);
        }

        public static void Hsubps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Hsubps, dst, src);
        }

        public static void Lddqu<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Lddqu, dst, src);
        }

        public static void Monitor(this Assembler intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Monitor);
        }

        public static void Movddup<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movddup, dst, src);
        }

        public static void Movddup<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movddup, dst, src);
        }

        public static void Movshdup<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movshdup, dst, src);
        }

        public static void Movshdup<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movshdup, dst, src);
        }

        public static void Movsldup<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movsldup, dst, src);
        }

        public static void Movsldup<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movsldup, dst, src);
        }

        public static void Mwait(this Assembler intrinsicSupport)
        {
            Contract.Requires(intrinsicSupport != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mwait);
        }

        #endregion

        #region SSSE3

        public static void Psignb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psignb, dst, src);
        }

        public static void Psignb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psignb, dst, src);
        }

        public static void Psignb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psignb, dst, src);
        }

        public static void Psignb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psignb, dst, src);
        }

        public static void Psignw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psignw, dst, src);
        }

        public static void Psignw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psignw, dst, src);
        }

        public static void Psignw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psignw, dst, src);
        }

        public static void Psignw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psignw, dst, src);
        }

        public static void Psignd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psignd, dst, src);
        }

        public static void Psignd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psignd, dst, src);
        }

        public static void Psignd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psignd, dst, src);
        }

        public static void Psignd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Psignd, dst, src);
        }

        public static void Phaddw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phaddw, dst, src);
        }

        public static void Phaddw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phaddw, dst, src);
        }

        public static void Phaddw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phaddw, dst, src);
        }

        public static void Phaddw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phaddw, dst, src);
        }

        public static void Phaddd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phaddd, dst, src);
        }

        public static void Phaddd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phaddd, dst, src);
        }

        public static void Phaddd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phaddd, dst, src);
        }

        public static void Phaddd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phaddd, dst, src);
        }

        public static void Phaddsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phaddsw, dst, src);
        }

        public static void Phaddsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phaddsw, dst, src);
        }

        public static void Phaddsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phaddsw, dst, src);
        }

        public static void Phaddsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phaddsw, dst, src);
        }

        public static void Phsubw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phsubw, dst, src);
        }

        public static void Phsubw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phsubw, dst, src);
        }

        public static void Phsubw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phsubw, dst, src);
        }

        public static void Phsubw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phsubw, dst, src);
        }

        public static void Phsubd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phsubd, dst, src);
        }

        public static void Phsubd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phsubd, dst, src);
        }

        public static void Phsubd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phsubd, dst, src);
        }

        public static void Phsubd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phsubd, dst, src);
        }

        public static void Phsubsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phsubsw, dst, src);
        }

        public static void Phsubsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phsubsw, dst, src);
        }

        public static void Phsubsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phsubsw, dst, src);
        }

        public static void Phsubsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phsubsw, dst, src);
        }

        public static void Pmaddubsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaddubsw, dst, src);
        }

        public static void Pmaddubsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaddubsw, dst, src);
        }

        public static void Pmaddubsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaddubsw, dst, src);
        }

        public static void Pmaddubsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaddubsw, dst, src);
        }

        public static void Pabsb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pabsb, dst, src);
        }

        public static void Pabsb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pabsb, dst, src);
        }

        public static void Pabsb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pabsb, dst, src);
        }

        public static void Pabsb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pabsb, dst, src);
        }

        public static void Pabsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pabsw, dst, src);
        }

        public static void Pabsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pabsw, dst, src);
        }

        public static void Pabsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pabsw, dst, src);
        }

        public static void Pabsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pabsw, dst, src);
        }

        public static void Pabsd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pabsd, dst, src);
        }

        public static void Pabsd<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pabsd, dst, src);
        }

        public static void Pabsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pabsd, dst, src);
        }

        public static void Pabsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pabsd, dst, src);
        }

        public static void Pmulhrsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmulhrsw, dst, src);
        }

        public static void Pmulhrsw<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmulhrsw, dst, src);
        }

        public static void Pmulhrsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmulhrsw, dst, src);
        }

        public static void Pmulhrsw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmulhrsw, dst, src);
        }

        public static void Pshufb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pshufb, dst, src);
        }

        public static void Pshufb<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pshufb, dst, src);
        }

        public static void Pshufb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pshufb, dst, src);
        }

        public static void Pshufb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pshufb, dst, src);
        }

        public static void Palignr<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, TMM src, Imm imm8)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Palignr, dst, src, imm8);
        }

        public static void Palignr<TMM>(this IMmIntrinsicSupport<TMM> intrinsicSupport, TMM dst, Mem src, Imm imm8)
            where TMM : Operand, IMmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Palignr, dst, src, imm8);
        }

        public static void Palignr<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Palignr, dst, src, imm8);
        }

        public static void Palignr<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Palignr, dst, src, imm8);
        }

        #endregion

        #region SSE 4.1

        public static void Blendpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Blendpd, dst, src, imm8);
        }

        public static void Blendpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Blendpd, dst, src, imm8);
        }

        public static void Blendps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Blendps, dst, src, imm8);
        }

        public static void Blendps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Blendps, dst, src, imm8);
        }

        public static void Blendvpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Blendvpd, dst, src);
        }

        public static void Blendvpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Blendvpd, dst, src);
        }

        public static void Blendvps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Blendvps, dst, src);
        }

        public static void Blendvps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Blendvps, dst, src);
        }

        public static void Dppd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Dppd, dst, src, imm8);
        }

        public static void Dppd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Dppd, dst, src, imm8);
        }

        public static void Dpps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Dpps, dst, src, imm8);
        }

        public static void Dpps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Dpps, dst, src, imm8);
        }

        public static void Extractps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Extractps, dst, src, imm8);
        }

        public static void Extractps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Extractps, dst, src, imm8);
        }

        public static void Movntdqa<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movntdqa, dst, src);
        }

        public static void Mpsadbw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mpsadbw, dst, src, imm8);
        }

        public static void Mpsadbw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Mpsadbw, dst, src, imm8);
        }

        public static void Packusdw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Packusdw, dst, src);
        }

        public static void Packusdw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Packusdw, dst, src);
        }

        public static void Pblendvb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pblendvb, dst, src);
        }

        public static void Pblendvb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pblendvb, dst, src);
        }

        public static void Pblendw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pblendw, dst, src, imm8);
        }

        public static void Pblendw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pblendw, dst, src, imm8);
        }

        public static void Pcmpeqq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpeqq, dst, src);
        }

        public static void Pcmpeqq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpeqq, dst, src);
        }

        public static void Pextrb<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, TXMM src, Imm imm8)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pextrb, dst, src, imm8);
        }

        public static void Pextrb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pextrb, dst, src, imm8);
        }

        public static void Pextrd<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, TXMM src, Imm imm8)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pextrd, dst, src, imm8);
        }

        public static void Pextrd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pextrd, dst, src, imm8);
        }

        public static void Pextrq<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TGP dst, TXMM src, Imm imm8)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pextrq, dst, src, imm8);
        }

        public static void Pextrq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, Mem dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pextrq, dst, src, imm8);
        }

        public static void Phminposuw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phminposuw, dst, src);
        }

        public static void Phminposuw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Phminposuw, dst, src);
        }

        public static void Pinsrb<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TXMM dst, TGP src, Imm imm8)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pinsrb, dst, src, imm8);
        }

        public static void Pinsrb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pinsrb, dst, src, imm8);
        }

        public static void Pinsrw<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TXMM dst, TGP src, Imm imm8)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pinsrw, dst, src, imm8);
        }

        public static void Pinsrw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pinsrw, dst, src, imm8);
        }

        public static void Pinsrd<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TXMM dst, TGP src, Imm imm8)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pinsrd, dst, src, imm8);
        }

        public static void Pinsrd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pinsrd, dst, src, imm8);
        }

        public static void Pinsrq<TGP, TX87, TMM, TXMM>(this IIntrinsicSupport<TGP, TX87, TMM, TXMM> intrinsicSupport, TXMM dst, TGP src, Imm imm8)
            where TGP : Operand, IGpOperand
            where TX87 : Operand, IX87Operand
            where TMM : Operand, IMmOperand
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pinsrq, dst, src, imm8);
        }

        public static void Pinsrq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pinsrq, dst, src, imm8);
        }

        public static void Pmaxuw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxuw, dst, src);
        }

        public static void Pmaxuw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxuw, dst, src);
        }

        public static void Pmaxsb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxsb, dst, src);
        }

        public static void Pmaxsb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxsb, dst, src);
        }

        public static void Pmaxsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxsd, dst, src);
        }

        public static void Pmaxsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxsd, dst, src);
        }

        public static void Pmaxud<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxud, dst, src);
        }

        public static void Pmaxud<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmaxud, dst, src);
        }

        public static void Pminsb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminsb, dst, src);
        }

        public static void Pminsb<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminsb, dst, src);
        }

        public static void Pminuw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminuw, dst, src);
        }

        public static void Pminuw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminuw, dst, src);
        }

        public static void Pminud<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminud, dst, src);
        }

        public static void Pminud<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminud, dst, src);
        }

        public static void Pminsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminsd, dst, src);
        }

        public static void Pminsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pminsd, dst, src);
        }

        public static void Pmovsxbw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovsxbw, dst, src);
        }

        public static void Pmovsxbw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovsxbw, dst, src);
        }

        public static void Pmovsxbd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovsxbd, dst, src);
        }

        public static void Pmovsxbd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovsxbd, dst, src);
        }

        public static void Pmovsxbq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovsxbq, dst, src);
        }

        public static void Pmovsxbq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovsxbq, dst, src);
        }

        public static void Pmovsxwd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovsxwd, dst, src);
        }

        public static void Pmovsxwd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovsxwd, dst, src);
        }

        public static void Pmovsxwq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovsxwq, dst, src);
        }

        public static void Pmovsxwq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovsxwq, dst, src);
        }

        public static void Pmovsxdq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovsxdq, dst, src);
        }

        public static void Pmovsxdq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovsxdq, dst, src);
        }

        public static void Pmovzxbw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovzxbw, dst, src);
        }

        public static void Pmovzxbw<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovzxbw, dst, src);
        }

        public static void Pmovzxbd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovzxbd, dst, src);
        }

        public static void Pmovzxbd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovzxbd, dst, src);
        }

        public static void Pmovzxbq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovzxbq, dst, src);
        }

        public static void Pmovzxbq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovzxbq, dst, src);
        }

        public static void Pmovzxwd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovzxwd, dst, src);
        }

        public static void Pmovzxwd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovzxwd, dst, src);
        }

        public static void Pmovzxwq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovzxwq, dst, src);
        }

        public static void Pmovzxwq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovzxwq, dst, src);
        }

        public static void Pmovzxdq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovzxdq, dst, src);
        }

        public static void Pmovzxdq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmovzxdq, dst, src);
        }

        public static void Pmuldq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmuldq, dst, src);
        }

        public static void Pmuldq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmuldq, dst, src);
        }

        public static void Pmulld<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmulld, dst, src);
        }

        public static void Pmulld<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pmulld, dst, src);
        }

        public static void Ptest<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM op1, TXMM op2)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(op1 != null);
            Contract.Requires(op2 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Ptest, op1, op2);
        }

        public static void Ptest<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM op1, Mem op2)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(op1 != null);
            Contract.Requires(op2 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Ptest, op1, op2);
        }

        public static void Roundps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Roundps, dst, src, imm8);
        }

        public static void Roundps<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Roundps, dst, src, imm8);
        }

        public static void Roundss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Roundss, dst, src, imm8);
        }

        public static void Roundss<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Roundss, dst, src, imm8);
        }

        public static void Roundpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Roundpd, dst, src, imm8);
        }

        public static void Roundpd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Roundpd, dst, src, imm8);
        }

        public static void Roundsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Roundsd, dst, src, imm8);
        }

        public static void Roundsd<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Roundsd, dst, src, imm8);
        }

        #endregion

        #region SSE 4.2

        public static void Crc32<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Crc32, dst, src);
        }

        public static void Crc32<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Crc32, dst, src);
        }

        public static void Pcmpestri<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpestri, dst, src, imm8);
        }

        public static void Pcmpestri<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpestri, dst, src, imm8);
        }

        public static void Pcmpestrm<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpestrm, dst, src, imm8);
        }

        public static void Pcmpestrm<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpestrm, dst, src, imm8);
        }

        public static void Pcmpistri<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpistri, dst, src, imm8);
        }

        public static void Pcmpistri<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpistri, dst, src, imm8);
        }

        public static void Pcmpistrm<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpistrm, dst, src, imm8);
        }

        public static void Pcmpistrm<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src, Imm imm8)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);
            Contract.Requires(imm8 != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpistrm, dst, src, imm8);
        }

        public static void Pcmpgtq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, TXMM src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpgtq, dst, src);
        }

        public static void Pcmpgtq<TXMM>(this IXmmIntrinsicSupport<TXMM> intrinsicSupport, TXMM dst, Mem src)
            where TXMM : Operand, IXmmOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Pcmpgtq, dst, src);
        }

        public static void Popcnt<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Popcnt, dst, src);
        }

        public static void Popcnt<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Popcnt, dst, src);
        }

        #endregion

        #region AMD instructions

        public static void AmdPrefetch(this IIntrinsicSupport intrinsicSupport, Mem mem)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(mem != null);

            intrinsicSupport.EmitInstruction(InstructionCode.AmdPrefetch, mem);
        }

        public static void AmdPrefetchw(this IIntrinsicSupport intrinsicSupport, Mem mem)
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(mem != null);

            intrinsicSupport.EmitInstruction(InstructionCode.AmdPrefetchw, mem);
        }

        #endregion

        #region Intel instructions

        public static void Movbe<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, TGP dst, Mem src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movbe, dst, src);
        }

        public static void Movbe<TGP>(this IX86IntrinsicSupport<TGP> intrinsicSupport, Mem dst, TGP src)
            where TGP : Operand, IGpOperand
        {
            Contract.Requires(intrinsicSupport != null);
            Contract.Requires(dst != null);
            Contract.Requires(src != null);

            intrinsicSupport.EmitInstruction(InstructionCode.Movbe, dst, src);
        }

        #endregion
    }
}
