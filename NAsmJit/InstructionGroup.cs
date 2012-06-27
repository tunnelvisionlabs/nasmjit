namespace AsmJitNet
{
    public enum InstructionGroup : byte
    {
        EMIT,

        ALU,
        BSWAP,
        BT,
        CALL,
        CRC32,
        ENTER,
        IMUL,
        INC_DEC,
        J,
        JMP,
        LEA,
        M,
        MOV,
        MOV_PTR,
        MOVSX_MOVZX,
        MOVSXD,
        PUSH, // I_PUSH is implemented before I_POP
        POP,
        R_RM,
        RM_B,
        RM,
        RM_R,
        REP,
        RET,
        ROT,
        SHLD_SHRD,
        TEST,
        XCHG,

        /// <summary>
        /// Group for x87 FP instructions in format mem or st(i), st(i) (fadd, fsub, fdiv, ...)
        /// </summary>
        X87_FPU,

        /// <summary>
        /// Group for x87 FP instructions in format st(i), st(i)
        /// </summary>
        X87_STI,

        // Group for fld/fst/fstp instruction, internally uses I_X87_MEM group.
        X87_MEM_STI,

        /// <summary>
        /// Group for x87 FP instructions that uses Word, DWord, QWord or TWord memory pointer
        /// </summary>
        X87_MEM,

        /// <summary>
        /// Group for x87 FSTSW/FNSTSW instructions
        /// </summary>
        X87_FSTSW,

        /// <summary>
        /// Group for movbe instruction
        /// </summary>
        MOVBE,

        // Group for MMX/SSE instructions in format (X)MM|Reg|Mem <- (X)MM|Reg|Mem,
        // 0x66 prefix must be set manually in opcodes.
        // - Primary opcode is used for instructions in (X)MM <- (X)MM/Mem format,
        // - Secondary opcode is used for instructions in (X)MM/Mem <- (X)MM format.
        MMU_MOV,

        /// <summary>
        /// Group for movd instructions
        /// </summary>
        MMU_MOVD,

        /// <summary>
        /// Group for movq instructions
        /// </summary>
        MMU_MOVQ,

        /// <summary>
        /// Group for pextrd, pextrq and pextrw instructions (it's special instruction not similar to others)
        /// </summary>
        MMU_PEXTR,

        /// <summary>
        /// Group for prefetch instruction
        /// </summary>
        MMU_PREFETCH,

        // Group for MMX/SSE instructions in format (X)MM|Reg <- (X)MM|Reg|Mem|Imm,
        // 0x66 prefix is added for MMX instructions that used by SSE2 registers.
        // - Primary opcode is used for instructions in (X)MM|Reg <- (X)MM|Reg|Mem format,
        // - Secondary opcode is iused for instructions in (X)MM|Reg <- Imm format.
        MMU_RMI,
        MMU_RM_IMM8,

        /// <summary>
        /// Group for 3dNow instructions
        /// </summary>
        MMU_RM_3DNOW
    }
}
