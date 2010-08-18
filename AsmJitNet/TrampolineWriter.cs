#if ASMJIT_X64
namespace AsmJitNet2
{
    using System;

    internal static class TrampolineWriter
    {
        public static readonly int TRAMPOLINE_JMP = 6;
        public static readonly int TRAMPOLINE_ADDR = IntPtr.Size;
        public static readonly int TRAMPOLINE_SIZE = TRAMPOLINE_JMP + TRAMPOLINE_ADDR;

        public static void WriteTrampoline(IntPtr code, IntPtr target)
        {
            unsafe
            {
                byte* codePtr = (byte*)code.ToPointer();
                codePtr[0] = 0xFF;
                codePtr[1] = 0x25;
                ((uint*)(codePtr + 2))[0] = 0;
                ((IntPtr*)(codePtr + TRAMPOLINE_JMP))[0] = target;
            }
        }
    }
}
#endif
