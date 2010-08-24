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

            Label a = compiler.NewLabel();
            Label b = compiler.NewLabel();
            Label c = compiler.NewLabel();

            compiler.Jmp(b);
            compiler.Bind(a);
            compiler.Jmp(c);
            compiler.Bind(b);
            compiler.Jmp(a);
            compiler.Bind(c);
            compiler.Ret();
            compiler.EndFunction();

            IntPtr result = compiler.Make();
        }
    }
}
