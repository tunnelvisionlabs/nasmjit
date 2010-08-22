namespace AsmJit.Sample
{
    using System;
    using AsmJitNet;

    internal class Program
    {
        [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate int Method1();

        private static void Main(string[] args)
        {
            Compiler compiler = new Compiler();
            CallingConvention callingConvention = (IntPtr.Size == 4) ? CallingConvention.Cdecl : CallingConvention.X64W;
            compiler.NewFunction(callingConvention, typeof(Func<int>));
            GPVar var = compiler.NewGP();
            compiler.Mov(var, 3);
            compiler.Ret(var);
            compiler.EndFunction();
            IntPtr ptr = compiler.Make();
            Method1 method = (Method1)System.Runtime.InteropServices.Marshal.GetDelegateForFunctionPointer(ptr, typeof(Method1));
            int result = method();
            Console.WriteLine(result);
        }
    }
}
