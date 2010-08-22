namespace AsmJit.Sample
{
    using System;
    using AsmJitNet;
    using System.Collections.Generic;
    using Marshal = System.Runtime.InteropServices.Marshal;

    internal class Program
    {
        private static readonly Lazy<IntPtr> _compilerLaunchPad = new Lazy<IntPtr>(CreateCompilerLaunchPad);

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate int Method1();

        private static IntPtr CompilerLaunchPad
        {
            get
            {
                return _compilerLaunchPad.Value;
            }
        }

        private static void Main(string[] args)
        {
            Compiler compiler = new Compiler();
            CallingConvention callingConvention = CallingConvention.Default;
            compiler.NewFunction(callingConvention, typeof(Func<int>));
            GPVar var = compiler.NewGP();
            compiler.Mov(var, 3);
            compiler.Ret(var);
            compiler.EndFunction();
            IntPtr ptr = compiler.Make();
            Method1 method = (Method1)Marshal.GetDelegateForFunctionPointer(ptr, typeof(Method1));
            int result = method();
            Console.WriteLine(result);

            IntPtr method2Ptr = CreateMethodStub();
            TestMethod method2 = (TestMethod)Marshal.GetDelegateForFunctionPointer(method2Ptr, typeof(TestMethod));
            Console.WriteLine(method2());
        }

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate int TestMethod();

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate IntPtr MethodCompiler(IntPtr stub);

        private static readonly List<MethodCompiler> _unmanagedDelegates = new List<MethodCompiler>();

        private static IntPtr CreateMethodStub()
        {
            Assembler assembler = new Assembler();
            assembler.Logger = new FileLogger(Console.Error);
            assembler.Call(CompilerLaunchPad);
            assembler.Jmp(GPReg.Nax);
            return assembler.Make();
        }

        private static IntPtr CreateCompilerLaunchPad()
        {
            MethodCompiler methodCompiler = CreateMethod;
            _unmanagedDelegates.Add(methodCompiler);
            IntPtr methodPtr = Marshal.GetFunctionPointerForDelegate(methodCompiler);

            Assembler assembler = new Assembler();
            assembler.Logger = new FileLogger(Console.Error);
            assembler.Push(GPReg.Nbp);
            assembler.Mov(GPReg.Nbp, GPReg.Nsp);

            if (IntPtr.Size == 4)
            {
                assembler.Push(Mem.dword_ptr(GPReg.Nbp, 4));
                assembler.Sub(Mem.dword_ptr(GPReg.Nsp), 5);
            }
            else
            {
                assembler.Push(GPReg.Ncx);
                assembler.Mov(GPReg.Ncx, Mem.qword_ptr(GPReg.Nbp, 8));
                assembler.Sub(GPReg.Ncx, 5);
            }

            assembler.Call(methodPtr);

            if (IntPtr.Size == 4)
            {
                assembler.Add(GPReg.Nsp, 4);
            }
            else
            {
                assembler.Pop(GPReg.Ncx);
            }

            assembler.Pop(GPReg.Nbp);
            assembler.Ret();
            return assembler.Make();
        }

        private static IntPtr CreateMethod(IntPtr stubAddress)
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
