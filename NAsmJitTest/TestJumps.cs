namespace AsmJitNetTest
{
    using System;
    using AsmJitNet;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Marshal = System.Runtime.InteropServices.Marshal;

    [TestClass]
    public class TestJumps
    {
        [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        [return: System.Runtime.InteropServices.MarshalAs(System.Runtime.InteropServices.UnmanagedType.Bool)]
        private delegate bool TestReverseJumpToUnreachableLabelFn();

        [TestMethod]
        public void TestReverseJumpToUnreachableLabel()
        {
            Compiler compiler = new Compiler()
            {
                Logger = new FileLogger(Console.Error)
            };

            compiler.NewFunction(CallingConvention.Default, typeof(Func<int>));

            Label a = compiler.DefineLabel();
            Label b = compiler.DefineLabel();
            Label c = compiler.DefineLabel();

            GPVar ret = compiler.NewGP();
            compiler.Mov(ret, 1);

            compiler.Jmp(b);
            compiler.MarkLabel(a);
            compiler.Jmp(c);
            compiler.MarkLabel(b);
            compiler.Jmp(a);
            compiler.MarkLabel(c);
            compiler.Ret(ret);
            compiler.EndFunction();

            IntPtr result = compiler.Make();
            var fn = (TestReverseJumpToUnreachableLabelFn)Marshal.GetDelegateForFunctionPointer(result, typeof(TestReverseJumpToUnreachableLabelFn));
            Assert.AreEqual(true, fn());

            compiler.CodeGenerator.Free(result);
        }
    }
}
