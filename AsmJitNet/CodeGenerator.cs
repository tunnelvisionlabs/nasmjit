namespace AsmJitNet
{
    using System;

    public abstract class CodeGenerator
    {
        private static readonly JitCodeGenerator _globalCodeGenerator = new JitCodeGenerator();

        public static CodeGenerator Global
        {
            get
            {
                return _globalCodeGenerator;
            }
        }

        public abstract void Generate(out IntPtr destination, Assembler assembler);

        public abstract void Free(IntPtr address);
    }
}
