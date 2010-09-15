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

            method.Dispose();
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

            method.Dispose();
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

            called.Dispose();
            caller.Dispose();
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidOperationException))]
        public void TestCompilerAssembler()
        {
            JitAction action = new JitAction();
            Compiler c = action.GetCompiler();
            Assert.IsNotNull(c);
            Assert.AreSame(c, action.GetCompiler());

            Assembler a = action.GetAssembler();
        }

        [TestMethod]
        [ExpectedException(typeof(InvalidOperationException))]
        public void TestAssemblerCompiler()
        {
            JitAction action = new JitAction();
            Assembler a = action.GetAssembler();
            Assert.IsNotNull(a);
            Assert.AreSame(a, action.GetAssembler());

            Compiler c = action.GetCompiler();
        }

        [TestMethod]
        public void TestCompileCaching()
        {
            JitAction action = new JitAction();
            Compiler c = action.GetCompiler();
            c.Ret();

            Assert.AreEqual(IntPtr.Zero, action.CompiledAddress);
            Assert.IsFalse(action.IsCompiled);

            action.Compile();

            Assert.AreNotEqual(IntPtr.Zero, action.CompiledAddress);
            Assert.IsTrue(action.IsCompiled);

            IntPtr ptr1 = action.CompiledAddress;
            action.Compile();
            Assert.AreEqual(ptr1, action.CompiledAddress);
        }
    }
}
