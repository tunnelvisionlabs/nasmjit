namespace NAsmJitTest
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

            MemoryManager.Global.Free(methodPtr);
        }

        private readonly List<MethodCompiler> _unmanagedDelegates = new List<MethodCompiler>();

        private IntPtr CreateMethodStub(MethodCompiler methodCompiler)
        {
            if (methodCompiler == null)
                throw new ArgumentNullException("methodCompiler");

            _unmanagedDelegates.Add(methodCompiler);
            IntPtr methodPtr = Marshal.GetFunctionPointerForDelegate(methodCompiler);

            Assembler assembler = new Assembler();
            Label label = assembler.DefineLabel();
            assembler.Call(label);
            assembler.MarkLabel(label);
            assembler.Sub(Mem.sysint_ptr(Register.nsp), 5);
            assembler.Call(methodPtr);
            assembler.Add(Register.nsp, IntPtr.Size);
            assembler.Jmp(Register.nax);
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
