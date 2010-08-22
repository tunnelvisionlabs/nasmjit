﻿namespace AsmJitNetTest
{
    using System;
    using System.Linq;
    using System.Text;
    using AsmJitNet;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Marshal = System.Runtime.InteropServices.Marshal;
    using UnmanagedFunctionPointer = System.Runtime.InteropServices.UnmanagedFunctionPointerAttribute;

    [TestClass]
    public sealed class TestVariables
    {
        [UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate void MyFn1();

        [TestMethod]
        public void TestVar1()
        {
            Compiler c = new Compiler();
            FileLogger logger = new FileLogger(Console.Error);
            c.Logger = logger;

            c.NewFunction(CallingConvention.Cdecl, typeof(Action<IntPtr, IntPtr, IntPtr, IntPtr, IntPtr, IntPtr, IntPtr, IntPtr>));

            GPVar p1 = c.ArgGP(0);
            GPVar p2 = c.ArgGP(1);
            GPVar p3 = c.ArgGP(2);
            GPVar p4 = c.ArgGP(3);
            GPVar p5 = c.ArgGP(4);
            GPVar p6 = c.ArgGP(5);
            GPVar p7 = c.ArgGP(6);
            GPVar p8 = c.ArgGP(7);

            GPVar v1 = c.NewGP();
            GPVar v2 = c.NewGP();
            GPVar v3 = c.NewGP();
            GPVar v4 = c.NewGP();
            GPVar v5 = c.NewGP();
            GPVar v6 = c.NewGP();
            GPVar v7 = c.NewGP();
            GPVar v8 = c.NewGP();

            GPVar r1 = c.NewGP();
            GPVar r2 = c.NewGP();
            GPVar r3 = c.NewGP();
            GPVar r4 = c.NewGP();
            GPVar r5 = c.NewGP();
            GPVar r6 = c.NewGP();

            c.Add(p1, 1);
            c.Add(p2, 2);
            c.Add(p3, 3);
            c.Add(p4, 4);
            c.Add(p5, 5);
            c.Add(p6, 6);
            c.Add(p7, 7);
            c.Add(p8, 8);

            c.Mov(v1, 10);
            c.Mov(v2, 20);
            c.Mov(v3, 30);
            c.Mov(v4, 40);
            c.Mov(v5, 50);
            c.Mov(v6, 60);
            c.Mov(v7, 70);
            c.Mov(v8, 80);

            c.Mov(r1, 100);
            c.Mov(r2, 200);
            c.Mov(r3, 300);
            c.Mov(r4, 400);
            c.Mov(r5, 500);
            c.Mov(r6, 600);

            c.Add(v1, 10);
            c.Add(v2, 20);
            c.Add(v3, 30);
            c.Add(v4, 40);
            c.Add(v5, 50);
            c.Add(v6, 60);
            c.Add(v7, 70);
            c.Add(v8, 80);

            c.Add(r1, 100);
            c.Add(r2, 200);
            c.Add(r3, 300);
            c.Add(r4, 400);
            c.Add(r5, 500);
            c.Add(r6, 600);

            c.Sub(p1, 1);
            c.Sub(p2, 2);
            c.Sub(p3, 3);
            c.Sub(p4, 4);
            c.Sub(p5, 5);
            c.Sub(p6, 6);
            c.Sub(p7, 7);
            c.Sub(p8, 8);

            c.EndFunction();

            MyFn1 fn = (MyFn1)Marshal.GetDelegateForFunctionPointer(c.Make(), typeof(MyFn1));
        }

        [UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate int TestDummyFn();

        [TestMethod]
        public void TestDummy()
        {
            Compiler c = new Compiler();

            FileLogger logger = new FileLogger(Console.Error);
            c.Logger = logger;

            c.NewFunction(CallingConvention.Cdecl, typeof(Func<int>));
            c.Function.SetHint(FunctionHints.Naked, true);

            GPVar var = c.NewGP(VariableType.GPD);
            c.Xor(var, var);
            c.Ret(var);

            c.EndFunction();

            TestDummyFn fn = FunctionCast<TestDummyFn>(c.Make());
            var result = fn();
            Assert.AreEqual(0, result);
        }

        [TestMethod]
        public void TestAlign()
        {
            TestAlign(false, false);
            TestAlign(false, true);

            if (CompilerUtil.IsStack16ByteAligned)
            {
                // If stack is 16-byte aligned by the operating system.
                TestAlign(true, false);
                TestAlign(true, true);
            }
        }

        [UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate IntPtr TestAlignFn0();

        [UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate IntPtr TestAlignFn1(IntPtr x);

        [UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate IntPtr TestAlignFn2(IntPtr x, IntPtr y);

        [UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate IntPtr TestAlignFn3(IntPtr x, IntPtr y, IntPtr z);

        public void TestAlign(bool naked, bool pushPopSequence)
        {
            for (int args = 0; args < 4; args++)
            {
                for (int vars = 0; vars < 4; vars++)
                {
                    TestAlign(args, vars, naked, pushPopSequence);
                }
            }
        }

        public void TestAlign(int args, int vars, bool naked, bool pushPopSequence)
        {
            IntPtr code = CompileFunction(args, vars, naked, pushPopSequence);
            IntPtr result = IntPtr.Zero;

            Console.WriteLine("", args, vars, naked, pushPopSequence);

            switch (args)
            {
            case 0:
                {
                    TestAlignFn0 fn = (TestAlignFn0)Marshal.GetDelegateForFunctionPointer(code, typeof(TestAlignFn0));
                    result = fn();
                }
                break;

            case 1:
                {
                    TestAlignFn1 fn = (TestAlignFn1)Marshal.GetDelegateForFunctionPointer(code, typeof(TestAlignFn1));
                    result = fn((IntPtr)1);
                }
                break;

            case 2:
                {
                    TestAlignFn2 fn = (TestAlignFn2)Marshal.GetDelegateForFunctionPointer(code, typeof(TestAlignFn2));
                    result = fn((IntPtr)1, (IntPtr)2);
                }
                break;

            case 3:
                {
                    TestAlignFn3 fn = (TestAlignFn3)Marshal.GetDelegateForFunctionPointer(code, typeof(TestAlignFn3));
                    result = fn((IntPtr)1, (IntPtr)2, (IntPtr)3);
                }
                break;
            }

            Assert.AreEqual(IntPtr.Zero, result);
        }

        private IntPtr CompileFunction(int args, int vars, bool naked, bool pushPopSequence)
        {
            Compiler compiler = new Compiler();

            FileLogger logger = new FileLogger(Console.Error);
            compiler.Logger = logger;

            switch (args)
            {
            case 0:
                compiler.NewFunction(CallingConvention.Cdecl, typeof(Func<IntPtr>));
                break;

            case 1:
                compiler.NewFunction(CallingConvention.Cdecl, typeof(Func<IntPtr, IntPtr>));
                break;

            case 2:
                compiler.NewFunction(CallingConvention.Cdecl, typeof(Func<IntPtr, IntPtr, IntPtr>));
                break;

            case 3:
                compiler.NewFunction(CallingConvention.Cdecl, typeof(Func<IntPtr, IntPtr, IntPtr, IntPtr>));
                break;
            }

            compiler.Function.SetHint(FunctionHints.Naked, naked);
            compiler.Function.SetHint(FunctionHints.PushPopSequence, pushPopSequence);

            GPVar gvar = compiler.NewGP();
            XMMVar xvar = compiler.NewXMM(VariableType.XMM);

            // Alloc, use and spill preserved registers.
            if (vars != 0)
            {
                int var = 0;
                RegIndex index = 0;
                int mask = 1;
                int preserved = compiler.Function.Prototype.PreservedGP;

                do
                {
                    if ((preserved & mask) != 0 && (index != RegIndex.Esp && index != RegIndex.Ebp))
                    {
                        GPVar somevar = compiler.NewGP(VariableType.GPD);
                        compiler.Alloc(somevar, index);
                        compiler.Mov(somevar, (Imm)0);
                        compiler.Spill(somevar);
                        var++;
                    }

                    index++;
                    mask <<= 1;
                } while (var < vars && (int)index < (int)RegNum.GP);
            }

            compiler.Alloc(gvar, GPReg.Nax);
            compiler.Lea(gvar, xvar.ToMem());
            compiler.And(gvar, (Imm)(15));
            compiler.Ret(gvar);
            compiler.EndFunction();

            return compiler.Make();
        }

        [UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate int TestFunctionCallFn(int a, int b, int c);

        [TestMethod]
        public void TestFunctionCall1()
        {
            IntPtr calledFn = TestFunctionCall1_CreateCalledFn();
            TestFunctionCallFn fn = (TestFunctionCallFn)Marshal.GetDelegateForFunctionPointer(calledFn, typeof(TestFunctionCallFn));
            // for now, the operation is (a+b)*c
            Assert.AreEqual(9, fn(1, 2, 3));

            // ==========================================================================
            // Create compiler.
            Compiler c = new Compiler();

            // Log compiler output.
            FileLogger logger = new FileLogger(Console.Error);
            c.Logger = logger;

            c.NewFunction(CallingConvention.Cdecl, typeof(Func<int, int, int, int>));

            GPVar v0 = c.ArgGP(0);
            GPVar v1 = c.ArgGP(1);
            GPVar v2 = c.ArgGP(2);

            // Just do something;)
            c.Shl(v0, (Imm)(1));
            c.Shl(v1, (Imm)(1));
            c.Shl(v2, (Imm)(1));

            // Call function
            GPVar address = c.NewGP();
            c.Mov(address, (Imm)calledFn);

            Call ctx = c.Call(address);
            ctx.SetPrototype(CallingConvention.Cdecl, typeof(Func<int, int, int, int>));
            ctx.SetArgument(0, v2);
            ctx.SetArgument(1, v1);
            ctx.SetArgument(2, v0);

            ctx.SetReturn(v0);
            c.Ret(v0);

            c.EndFunction();
            // ==========================================================================

            // ==========================================================================
            // Make the function.
            fn = (TestFunctionCallFn)Marshal.GetDelegateForFunctionPointer(c.Make(), typeof(TestFunctionCallFn));
            int result = fn(3, 2, 1);
            Assert.AreEqual(36, result);
            // ==========================================================================
        }

        private static IntPtr TestFunctionCall1_CreateCalledFn()
        {
            Compiler c = new Compiler();

            FileLogger logger = new FileLogger(Console.Error);
            c.Logger = logger;

            c.NewFunction(CallingConvention.Cdecl, typeof(Func<int, int, int, int>));
            GPVar v0 = c.ArgGP(0);
            GPVar v1 = c.ArgGP(1);
            GPVar v2 = c.ArgGP(2);

            c.Add(v0, v1);
            c.Imul(v0, v2);
            c.Ret(v0);

            c.EndFunction();
            return c.Make();
        }

        [UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate void TestFuncManyArgsFn(out byte arg0, out byte arg1, out byte arg2, out byte arg3, out byte arg4, out byte arg5, out byte arg6, out byte arg7);

        [TestMethod]
        public void TestFuncManyArgs()
        {
            // ==========================================================================
            // Create compiler.
            Compiler c = new Compiler();

            // Log compiler output.
            FileLogger logger = new FileLogger(Console.Error);
            c.Logger = logger;

            c.NewFunction(CallingConvention.Cdecl, typeof(Action<IntPtr, IntPtr, IntPtr, IntPtr, IntPtr, IntPtr, IntPtr, IntPtr>));

            GPVar p1 = c.ArgGP(0);
            GPVar p2 = c.ArgGP(1);
            GPVar p3 = c.ArgGP(2);
            GPVar p4 = c.ArgGP(3);
            GPVar p5 = c.ArgGP(4);
            GPVar p6 = c.ArgGP(5);
            GPVar p7 = c.ArgGP(6);
            GPVar p8 = c.ArgGP(7);

            //c.Add(p1, 1);
            //c.Add(p2, 2);
            //c.Add(p3, 3);
            //c.Add(p4, 4);
            //c.Add(p5, 5);
            //c.Add(p6, 6);
            //c.Add(p7, 7);
            //c.Add(p8, 8);

            // Move some data into buffer provided by arguments so we can verify if it
            // really works without looking into assembler output.
            c.Mov(Mem.byte_ptr(p1), (Imm)(1));
            c.Mov(Mem.byte_ptr(p2), (Imm)(2));
            c.Mov(Mem.byte_ptr(p3), (Imm)(3));
            c.Mov(Mem.byte_ptr(p4), (Imm)(4));
            c.Mov(Mem.byte_ptr(p5), (Imm)(5));
            c.Mov(Mem.byte_ptr(p6), (Imm)(6));
            c.Mov(Mem.byte_ptr(p7), (Imm)(7));
            c.Mov(Mem.byte_ptr(p8), (Imm)(8));

            c.EndFunction();
            // ==========================================================================

            // ==========================================================================
            // Make the function.
            byte[] var = { 0, 0, 0, 0, 0, 0, 0, 0 };

            TestFuncManyArgsFn fn = FunctionCast<TestFuncManyArgsFn>(c.Make());
            fn(out var[0], out var[1], out var[2], out var[3], out var[4], out var[5], out var[6], out var[7]);

            for (int i = 0; i < var.Length; i++)
                Assert.AreEqual(i + 1, var[i]);
            // ==========================================================================
        }

        [UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate int TestFuncRetFn(int x, int y, int op);

        [UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate int TestFuncRetFnA(int x, int y);

        [TestMethod]
        public void TestFuncRet()
        {
            TestFuncRetFnA funcAdel = TestFuncRet_FuncA;
            TestFuncRetFnA funcBdel = TestFuncRet_FuncB;
            IntPtr funcA = Marshal.GetFunctionPointerForDelegate(funcAdel);
            IntPtr funcB = Marshal.GetFunctionPointerForDelegate(funcBdel);

            Compiler c = new Compiler();

            FileLogger logger = new FileLogger(Console.Error);
            c.Logger = logger;

            c.NewFunction(CallingConvention.Cdecl, typeof(Func<int, int, int, int>));

            {
                GPVar x = c.ArgGP(0);
                GPVar y = c.ArgGP(1);
                GPVar op = c.ArgGP(2);

                Label opAdd = c.NewLabel();
                Label opMul = c.NewLabel();

                c.Cmp(op, 0);
                c.Jz(opAdd);

                c.Cmp(op, 1);
                c.Jz(opMul);

                {
                    GPVar result = c.NewGP();
                    c.Mov(result, 0);
                    c.Ret(result);
                }

                {
                    c.Bind(opAdd);

                    GPVar result = c.NewGP();
                    Call call = c.Call(funcA);
                    call.SetPrototype(CallingConvention.Cdecl, typeof(Func<int, int, int>));
                    call.SetArgument(0, x);
                    call.SetArgument(1, y);
                    call.SetReturn(result);
                    c.Ret(result);
                }

                {
                    c.Bind(opMul);

                    GPVar result = c.NewGP();
                    Call call = c.Call(funcB);
                    call.SetPrototype(CallingConvention.Cdecl, typeof(Func<int, int, int>));
                    call.SetArgument(0, x);
                    call.SetArgument(1, y);
                    call.SetReturn(result);
                    c.Ret(result);
                }
            }

            c.EndFunction();

            TestFuncRetFn fn = FunctionCast<TestFuncRetFn>(c.Make());
            Assert.AreEqual(12, fn(4, 8, 0));
            Assert.AreEqual(32, fn(4, 8, 1));
            Assert.AreEqual(0, fn(4, 8, 2));
        }

        private static int TestFuncRet_FuncA(int x, int y)
        {
            return x + y;
        }

        private static int TestFuncRet_FuncB(int x, int y)
        {
            return x * y;
        }

        private IntPtr TestFuncRet_CreateFuncA()
        {
            throw new NotImplementedException();
        }

        private IntPtr TestFuncRet_CreateFuncB()
        {
            throw new NotImplementedException();
        }

        [UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate void TestRepFn([System.Runtime.InteropServices.Out] byte[] destination, byte[] source, IntPtr length);

        [TestMethod]
        public void TestRep()
        {
            Compiler c = new Compiler();

            FileLogger logger = new FileLogger(Console.Error);
            c.Logger = logger;

            {
                c.NewFunction(CallingConvention.Cdecl, typeof(Action<IntPtr, IntPtr, IntPtr>));
                c.Function.SetHint(FunctionHints.Naked, true);

                GPVar dst = c.ArgGP(0);
                GPVar src = c.ArgGP(1);
                GPVar cnt = c.ArgGP(2);

                c.RepMovsb(dst, src, cnt);
                c.EndFunction();
            }

            {
                TestRepFn copy = FunctionCast<TestRepFn>(c.Make());

                string text = "Hello AsmJit";
                byte[] src = Encoding.ASCII.GetBytes(text);
                byte[] dst = new byte[src.Length];

                copy(dst, src, (IntPtr)src.Length);
                Assert.AreEqual(text, Encoding.ASCII.GetString(dst));
            }
        }

        private static TFunction FunctionCast<TFunction>(IntPtr ptr)
        {
            return (TFunction)(object)Marshal.GetDelegateForFunctionPointer(ptr, typeof(TFunction));
        }

        [UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate void TestFuncMemCpyFn([System.Runtime.InteropServices.Out] int[] destination, int[] source, IntPtr length);

        [TestMethod]
        public void TestFuncMemCpy()
        {
            // ==========================================================================
            // Part 1:

            // Create Compiler.
            Compiler c = new Compiler();

            FileLogger logger = new FileLogger(Console.Error);
            c.Logger = logger;

            // Tell compiler the function prototype we want. It allocates variables representing
            // function arguments that can be accessed through Compiler or Function instance.
            c.NewFunction(CallingConvention.Cdecl, typeof(Action<IntPtr, IntPtr, IntPtr>));

            // Try to generate function without prolog/epilog code:
            c.Function.SetHint(FunctionHints.Naked, true);

            // Create labels.
            Label L_Loop = c.NewLabel();
            Label L_Exit = c.NewLabel();

            // Function arguments.
            GPVar dst = c.ArgGP(0);
            GPVar src = c.ArgGP(1);
            GPVar cnt = c.ArgGP(2);

            // Allocate loop variables registers (if they are not allocated already).
            c.Alloc(dst);
            c.Alloc(src);
            c.Alloc(cnt);

            // Exit if length is zero.
            c.Test(cnt, cnt);
            c.Jz(L_Exit);

            // Loop.
            c.Bind(L_Loop);

            // Copy DWORD (4 bytes).
            GPVar tmp = c.NewGP(VariableType.GPD);
            c.Mov(tmp, Mem.dword_ptr(src));
            c.Mov(Mem.dword_ptr(dst), tmp);

            // Increment dst/src pointers.
            c.Add(src, 4);
            c.Add(dst, 4);

            // Loop until cnt is not zero.
            c.Dec(cnt);
            c.Jnz(L_Loop);

            // Exit.
            c.Bind(L_Exit);
            c.Ret();

            // Finish.
            c.EndFunction();
            // ==========================================================================

            // ==========================================================================
            // Part 2:

            // Make JIT function.
            TestFuncMemCpyFn fn = FunctionCast<TestFuncMemCpyFn>(c.Make());

            Assert.IsNotNull(fn);

            // Create some data.
            const int count = 128;
            int[] dstBuffer = new int[count];
            int[] srcBuffer = new int[count];

            for (int i = 0; i < srcBuffer.Length; i++)
            {
                srcBuffer[i] = (byte)i;
            }

            // Call the JIT function.
            fn(dstBuffer, srcBuffer, (IntPtr)count);

            Assert.IsTrue(srcBuffer.SequenceEqual(dstBuffer));
        }

        private static readonly Tuple<CpuFeatures, string>[] _bitDescriptions =
            {
                Tuple.Create( CpuFeatures.RDTSC                       , "RDTSC" ),
                Tuple.Create( CpuFeatures.RDTSCP                      , "RDTSCP" ),
                Tuple.Create( CpuFeatures.CMOV                        , "CMOV" ),
                Tuple.Create( CpuFeatures.CMPXCHG8B                   , "CMPXCHG8B" ),
                Tuple.Create( CpuFeatures.CMPXCHG16B                  , "CMPXCHG16B" ),
                Tuple.Create( CpuFeatures.CLFLUSH                     , "CLFLUSH" ),
                Tuple.Create( CpuFeatures.PREFETCH                    , "PREFETCH" ),
                Tuple.Create( CpuFeatures.LAHF_SAHF                   , "LAHF/SAHF" ),
                Tuple.Create( CpuFeatures.FXSR                        , "FXSAVE/FXRSTOR" ),
                Tuple.Create( CpuFeatures.FFXSR                       , "FXSAVE/FXRSTOR Optimizations" ),
                Tuple.Create( CpuFeatures.MMX                         , "MMX" ),
                Tuple.Create( CpuFeatures.MMX_EXT                     , "MMX Extensions" ),
                Tuple.Create( CpuFeatures.AMD3DNOW                    , "3dNow!" ),
                Tuple.Create( CpuFeatures.AMD3DNOW_EXT                , "3dNow! Extensions" ),
                Tuple.Create( CpuFeatures.SSE                         , "SSE" ),
                Tuple.Create( CpuFeatures.MSSE                        , "Misaligned SSE" ),
                Tuple.Create( CpuFeatures.SSE2                        , "SSE2" ),
                Tuple.Create( CpuFeatures.SSE3                        , "SSE3" ),
                Tuple.Create( CpuFeatures.SSSE3                       , "Suplemental SSE3 (SSSE3)" ),
                Tuple.Create( CpuFeatures.SSE4_A                      , "SSE4A" ),
                Tuple.Create( CpuFeatures.SSE4_1                      , "SSE4.1" ),
                Tuple.Create( CpuFeatures.SSE4_2                      , "SSE4.2" ),
                Tuple.Create( CpuFeatures.SSE5                        , "SSE5" ),
                Tuple.Create( CpuFeatures.MONITOR_MWAIT               , "MONITOR/MWAIT" ),
                Tuple.Create( CpuFeatures.MOVBE                       , "MOVBE" ),
                Tuple.Create( CpuFeatures.POPCNT                      , "POPCNT" ),
                Tuple.Create( CpuFeatures.LZCNT                       , "LZCNT" ),
                Tuple.Create( CpuFeatures.MULTI_THREADING             , "MultiThreading" ),
                Tuple.Create( CpuFeatures.EXECUTE_DISABLE_BIT         , "Execute Disable Bit" ),
                Tuple.Create( CpuFeatures.X64_BIT                     , "64 Bit Processor" ),
            };

        [TestMethod]
        public void TestCpuInfo()
        {
            CpuInfo info = CpuInfo.Instance;

            Console.Error.WriteLine("Basic Info:");
            Console.Error.WriteLine("  Vendor String         : {0}", info.Vendor);
            Console.Error.WriteLine("  Family                : {0}", info.Family);
            Console.Error.WriteLine("  Model                 : {0}", info.Model);
            Console.Error.WriteLine("  Stepping              : {0}", info.Stepping);
            Console.Error.WriteLine("  Number of Processors  : {0}", info.NumberOfProcessors);
            Console.Error.WriteLine("  Features              : {0:x}", info.Features);
            Console.Error.WriteLine("  Bugs                  : {0:x}", info.Bugs);

            Console.Error.WriteLine("Extended Info (X86/X64):");
            Console.Error.WriteLine("  Processor Type        : {0}", info.ExtendedInfo.ProcessorType);
            Console.Error.WriteLine("  Brand Index           : {0}", info.ExtendedInfo.BrandIndex);
            Console.Error.WriteLine("  CL Flush Cache Line   : {0}", info.ExtendedInfo.FlushCacheLineSize);
            Console.Error.WriteLine("  Max Logical Processors: {0}", info.ExtendedInfo.MaxLogicalProcessors);
            Console.Error.WriteLine("  APIC Physical ID      : {0}", info.ExtendedInfo.ApicPhysicalId);

            Console.Error.WriteLine("CPU Features:");
            PrintBits("  ", info.Features);
        }

        private void PrintBits(string message, CpuFeatures features)
        {
            foreach (var description in _bitDescriptions)
            {
                if ((features & description.Item1) != 0)
                    Console.Error.WriteLine("{0}{1}", message, description.Item2);
            }
        }
    }
}
