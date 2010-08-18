namespace AsmJitNet2
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

        // Group for x87 FP instructions in format mem or st(i), st(i) (fadd, fsub, fdiv, ...)
        X87_FPU,
        // Group for x87 FP instructions in format st(i), st(i)
        X87_STI,
        // Group for fld/fst/fstp instruction, internally uses I_X87_MEM group.
        X87_MEM_STI,
        // Group for x87 FP instructions that uses Word, DWord, QWord or TWord memory pointer.
        X87_MEM,
        // Group for x87 FSTSW/FNSTSW instructions
        X87_FSTSW,

        // Group for movbe instruction
        MOVBE,

        // Group for MMX/SSE instructions in format (X)MM|Reg|Mem <- (X)MM|Reg|Mem,
        // 0x66 prefix must be set manually in opcodes.
        // - Primary opcode is used for instructions in (X)MM <- (X)MM/Mem format,
        // - Secondary opcode is used for instructions in (X)MM/Mem <- (X)MM format.
        MMU_MOV,

        // Group for movd and movq instructions.
        MMU_MOVD,
        MMU_MOVQ,

        // Group for pextrd, pextrq and pextrw instructions (it's special instruction
        // not similar to others)
        MMU_PEXTR,

        // Group for prefetch instruction
        MMU_PREFETCH,

        // Group for MMX/SSE instructions in format (X)MM|Reg <- (X)MM|Reg|Mem|Imm,
        // 0x66 prefix is added for MMX instructions that used by SSE2 registers.
        // - Primary opcode is used for instructions in (X)MM|Reg <- (X)MM|Reg|Mem format,
        // - Secondary opcode is iused for instructions in (X)MM|Reg <- Imm format.
        MMU_RMI,
        MMU_RM_IMM8,

        // Group for 3dNow instructions
        MMU_RM_3DNOW
    }
}
