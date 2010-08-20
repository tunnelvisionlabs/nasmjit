﻿namespace AsmJitNet2
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

        public abstract int Generate(out IntPtr destination, Assembler assembler);
    }
}