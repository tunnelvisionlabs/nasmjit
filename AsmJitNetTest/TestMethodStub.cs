namespace AsmJitNetTest
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using AsmJitNet;
    using Marshal = System.Runtime.InteropServices.Marshal;

    [TestClass]
    public class TestMethodStub
    {
        [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate int TestMethod();

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate IntPtr MethodCompiler(IntPtr stub);

        [TestMethod]
        public void TestBasicMethodStub()
        {
            IntPtr methodPtr = CreateMethodStub(CreateMethod);
            TestMethod method = (TestMethod)Marshal.GetDelegateForFunctionPointer(methodPtr, typeof(TestMethod));
            Assert.AreEqual(2, method());
        }

        private readonly List<MethodCompiler> _unmanagedDelegates = new List<MethodCompiler>();

        private IntPtr CreateMethodStub(MethodCompiler methodCompiler)
        {
            if (methodCompiler == null)
                throw new ArgumentNullException("methodCompiler");

            _unmanagedDelegates.Add(methodCompiler);
            IntPtr methodPtr = Marshal.GetFunctionPointerForDelegate(methodCompiler);

            Assembler assembler = new Assembler();
            Label label = assembler.NewLabel();
            assembler.Call(label);
            assembler.Bind(label);
            assembler.Sub(Mem.sysint_ptr(GPReg.Nsp), 5);
            assembler.Call(methodPtr);
            assembler.Add(GPReg.Nsp, IntPtr.Size);
            assembler.Jmp(GPReg.Nax);
            return assembler.Make();
        }

        private IntPtr CreateMethod(IntPtr stub)
        {
            Compiler compiler = new Compiler();
            compiler.NewFunction(CallingConvention.Default, typeof(Func<int>));
            GPVar x = compiler.NewGP(VariableType.INT32);
            compiler.Mov(x, 2);
            compiler.Ret(x);
            compiler.EndFunction();
            return compiler.Make();
        }
    }
}
