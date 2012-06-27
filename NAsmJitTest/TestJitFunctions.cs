namespace NAsmJitTest
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using NAsmJit;

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

        [TestMethod]
        public void TestRecursiveFunction()
        {
            JitFunction<int, int> function = new JitFunction<int, int>(CallingConvention.Default, FunctionHints.Naked);
            Compiler c = function.GetCompiler();
            c.Logger = new FileLogger(Console.Error);

            Label skip = c.DefineLabel();

            GPVar var = c.ArgGP(0);
            c.Cmp(var, 1);
            c.Jle(skip);

            GPVar tmp = c.NewGP(VariableType.INT32);
            c.Mov(tmp, var);
            c.Dec(tmp);

            Call call = c.Call(c.Function.EntryLabel, CallingConvention.Default, typeof(Func<int, int>));
            call.SetArgument(0, tmp);
            call.SetReturn(tmp);
            c.Mul(c.NewGP(VariableType.INT32), var, tmp);

            c.MarkLabel(skip);
            c.Ret(var);

            TestJitFunction2Fn fn = function.GetDelegate<TestJitFunction2Fn>();
            Assert.AreEqual(5 * 4 * 3 * 2 * 1, fn(5));
        }
    }
}
