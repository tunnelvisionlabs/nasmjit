namespace AsmJitNetTest
{
    using System;
    using AsmJitNet;
    using Microsoft.VisualStudio.TestTools.UnitTesting;

    [TestClass]
    public class TestJumps
    {
        [TestMethod]
        public void TestReverseJumpToUnreachableLabel()
        {
            Compiler compiler = new Compiler();
            compiler.NewFunction(CallingConvention.Default, typeof(Action));

            Label a = compiler.DefineLabel();
            Label b = compiler.DefineLabel();
            Label c = compiler.DefineLabel();

            compiler.Jmp(b);
            compiler.MarkLabel(a);
            compiler.Jmp(c);
            compiler.MarkLabel(b);
            compiler.Jmp(a);
            compiler.MarkLabel(c);
            compiler.Ret();
            compiler.EndFunction();

            IntPtr result = compiler.Make();
        }
    }
}
