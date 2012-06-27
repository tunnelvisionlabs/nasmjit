namespace AsmJit.Sample
{
    using System;
    using Marshal = System.Runtime.InteropServices.Marshal;
    using AsmJitNet;

    public class TestExceptionHandling
    {
        [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate void WriteIntegerMethod(int value);

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate void TestMethod([System.Runtime.InteropServices.MarshalAs(System.Runtime.InteropServices.UnmanagedType.Bool)] bool throwException);

        [System.Runtime.InteropServices.UnmanagedFunctionPointer(System.Runtime.InteropServices.CallingConvention.Cdecl)]
        private delegate IntPtr AllocateExceptionMethod(int code);

        private static readonly WriteIntegerMethod WriteInteger = Console.WriteLine;
        private static readonly IntPtr WriteIntegerFunction = Marshal.GetFunctionPointerForDelegate(WriteInteger);

        private static readonly AllocateExceptionMethod AllocateException =
            code =>
            {
                ExceptionData exceptionData = new ExceptionData(code);
                IntPtr ptr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(ExceptionData)));
                Marshal.StructureToPtr(exceptionData, ptr, false);
                return ptr;
            };

        private static readonly IntPtr AllocateExceptionFunction = Marshal.GetFunctionPointerForDelegate(AllocateException);

        private static readonly int ThreadData_ExceptionDataOffset = (int)Marshal.OffsetOf(typeof(ThreadData), "_exceptionData");
        private static readonly int ExceptionData_CodeOffset = (int)Marshal.OffsetOf(typeof(ExceptionData), "_code");

        private unsafe ThreadData* _threadData;

        public void RunTest()
        {
            var action = GenerateCode();
            var method = action.GetDelegate<TestMethod>();
            method(true);
            method(false);
        }

        private JitAction<int> GenerateCode()
        {
            //Action<bool> sample =
            //    throwException =>
            //    {
            //        try
            //        {
            //            Console.WriteLine(0);
            //            if (throwException)
            //                throw new Exception("1");
            //        }
            //        catch (Exception e)
            //        {
            //            Console.WriteLine(e.Message);
            //        }
            //        finally
            //        {
            //            Console.WriteLine(2);
            //        }
            //    };

            JitFunction<IntPtr> getInstructionPointer = new JitFunction<IntPtr>(hints: FunctionHints.Naked);
            {
                Assembler assembler = getInstructionPointer.GetAssembler();
                assembler.Mov(Register.nax, Register.nbp);
                assembler.Ret();
                getInstructionPointer.Compile();
            }

            JitAction<int> testMethod = new JitAction<int>();

            unsafe
            {
                _threadData = (ThreadData*)Marshal.AllocHGlobal(Marshal.SizeOf(typeof(ThreadData)));
                Marshal.StructureToPtr(new ThreadData(), (IntPtr)_threadData, false);

                Compiler c = testMethod.GetCompiler();
                c.Logger = new FileLogger(Console.Error);

                // arguments
                GPVar argThrowException = c.ArgGP(0);

                // special variables
                GPVar currentException = c.NewGP();
                GPVar nextException = c.NewGP();
                GPVar position = c.NewGP();

                // special prolog
                GPVar varMethodData = c.NewGP();
                GPVar varThreadData = c.NewGP();
                c.Mov(varThreadData, (IntPtr)_threadData);

                // labels
                Label leaveTry = c.DefineLabel();
                Label enterCatch = c.DefineLabel();
                Label leaveCatch = c.DefineLabel();
                Label enterFinally = c.DefineLabel();
                Label leaveFinally = c.DefineLabel();
                Label enterThrowHandler = c.DefineLabel();
                Label enterRethrowHandler = c.DefineLabel();
                Label enterLeaveHandler = c.DefineLabel();
                Label enterEndFinallyHandler = c.DefineLabel();
                Label retLabel = c.DefineLabel();

                // try
                c.Comment("Enter try");
                GenerateWriteLine(c, (Imm)0);
                c.Test(argThrowException, argThrowException);
                c.Jz(leaveTry, Hint.Taken);
                GenerateNewException(c, nextException, (Imm)1);
                c.Call(getInstructionPointer, position);
                c.Jmp(enterThrowHandler);
                c.MarkLabel(leaveTry);
                c.Jmp(enterLeaveHandler);

                // catch
                c.Comment("Enter catch");
                c.MarkLabel(enterCatch);
                GPVar varErrorCode = c.NewGP();
                GenerateLoadCurrentException(c, varErrorCode, varThreadData);
                GenerateLoadExceptionMessage(c, varErrorCode, varErrorCode);
                GenerateWriteLine(c, varErrorCode);
                c.Xor(varErrorCode, varErrorCode);
                c.Mov(Mem.sysint_ptr(varThreadData, ThreadData_ExceptionDataOffset), varErrorCode);
                c.MarkLabel(leaveCatch);
                c.Jmp(enterLeaveHandler);

                // finally
                c.Comment("Enter finally");
                c.MarkLabel(enterFinally);
                GenerateWriteLine(c, (Imm)2);
                c.MarkLabel(leaveFinally);
                c.Jmp(enterEndFinallyHandler);

                //// special epilog (throw and leave implementations)
                //c.Jmp(retLabel);

                c.Comment("Handler for 'throw' instructions");
                c.MarkLabel(enterThrowHandler);
                c.Mov(Mem.sysint_ptr(varThreadData, ThreadData_ExceptionDataOffset), nextException);
                c.Jmp(enterCatch);

                c.Comment("Handler for 'rethrow' instructions");
                c.MarkLabel(enterRethrowHandler);
                // rethrow is not implemented and not currently used
                c.Int3();

                c.Comment("Handler for 'leave' instructions");
                c.MarkLabel(enterLeaveHandler);
                c.Jmp(enterFinally);
                //c.Mov(Mem.sysint_ptr(varThreadData, ThreadData_ExceptionDataOffset), 0);
                //throw new NotImplementedException("TODO: check for a finally block");
                //c.Jmp(leaveTarget);

                c.Comment("Handler for 'endfinally' instructions");
                c.MarkLabel(enterEndFinallyHandler);
                GenerateLoadCurrentException(c, currentException, varThreadData);
                c.Test(currentException, currentException);
                c.Jnz(enterRethrowHandler, Hint.NotTaken);
                c.Jmp(retLabel);

                // return
                c.Comment("Return");
                c.MarkLabel(retLabel);
                c.Ret();

                testMethod.Compile();
            }

            return testMethod;
        }

        private static void GenerateWriteLine(Compiler c, GPVar value)
        {
            Call call = c.Call(WriteIntegerFunction, CallingConvention.Default, typeof(Action<int>));
            call.SetArgument(0, value);
        }

        private static void GenerateWriteLine(Compiler c, Imm value)
        {
            GPVar var = c.NewGP();
            c.Mov(var, value);
            GenerateWriteLine(c, var);
        }

        private static void GenerateNewException(Compiler c, GPVar dst, Imm code)
        {
            GPVar var = c.NewGP();
            c.Mov(var, code);
            Call call = c.Call(AllocateExceptionFunction, CallingConvention.Default, typeof(Func<int, IntPtr>));
            call.SetArgument(0, var);
            call.SetReturn(dst);
        }

        private static void GenerateLoadThreadData(Compiler c, GPVar dst, GPVar threadDataVariable)
        {
            c.Mov(dst, threadDataVariable);
        }

        private static void GenerateLeave(Compiler c)
        {
        }

        private static void GenerateLoadCurrentException(Compiler c, GPVar dst, GPVar threadDataVariable)
        {
            GenerateLoadThreadData(c, dst, threadDataVariable);
            c.Mov(dst, Mem.sysint_ptr(dst, ThreadData_ExceptionDataOffset));
        }

        private static void GenerateLoadExceptionMessage(Compiler c, GPVar dst, GPVar exceptionVariable)
        {
            c.Mov(dst, Mem.dword_ptr(exceptionVariable, ExceptionData_CodeOffset));
        }

        private struct MethodData
        {
        }

        private struct ThreadData
        {
#pragma warning disable 649 // Field 'field' is never assigned to, and will always have its default value 'value'
            public unsafe ExceptionData* _exceptionData;
#pragma warning restore 649
        }

        private struct ExceptionData
        {
            public int _code;

            public ExceptionData(int code)
            {
                _code = code;
            }
        }
    }
}
