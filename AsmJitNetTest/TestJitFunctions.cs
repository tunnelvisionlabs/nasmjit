namespace AsmJitNetTest
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using AsmJitNet;

    [TestClass]
    public class TestJitFunctions
    {
        [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate int TestJitFunction1Fn();

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate int TestJitFunction2Fn(int x);

        [TestMethod]
        public void TestJitFunction1()
        {
            JitFunction<int> method = new JitFunction<int>();
            Compiler compiler = method.GetCompiler();
            compiler.Logger = new FileLogger(Console.Error);
            GPVar var = compiler.NewGP();
            compiler.Mov(var, 1);
            compiler.Ret(var);

            var f = method.GetDelegate<TestJitFunction1Fn>();
            int result = f();
            Assert.AreEqual(1, result);
        }

        [TestMethod]
        public void TestJitFunction2()
        {
            JitFunction<int, int> method = new JitFunction<int, int>();
            Compiler compiler = method.GetCompiler();
            compiler.Logger = new FileLogger(Console.Error);
            GPVar arg = compiler.ArgGP(0);
            GPVar var = compiler.NewGP();
            compiler.Mov(var, arg);
            compiler.Imul(var, 2);
            compiler.Ret(var);

            var f = method.GetDelegate<TestJitFunction2Fn>();
            int result = f(1);
            Assert.AreEqual(2, result);
        }

        [TestMethod]
        public void TestJitFunctionCall1()
        {
            JitFunction<int, int> caller = new JitFunction<int, int>();
            JitFunction<int, int> called = new JitFunction<int, int>(CallingConvention.MsFastCall);

            {
                Compiler compiler = called.GetCompiler();
                compiler.Logger = new FileLogger(Console.Error);
                GPVar arg = compiler.ArgGP(0);
                GPVar var = compiler.NewGP();
                compiler.Mov(var, arg);
                compiler.Imul(var, 2);
                compiler.Ret(var);
                called.Compile();
            }

            {
                Compiler compiler = caller.GetCompiler();
                compiler.Logger = new FileLogger(Console.Error);
                GPVar arg = compiler.ArgGP(0);
                GPVar var = compiler.NewGP();
                compiler.Call(called, arg, var);
                compiler.Ret(var);
            }

            var f = caller.GetDelegate<TestJitFunction2Fn>();
            int result = f(1);
            Assert.AreEqual(2, result);
        }
    }
}
