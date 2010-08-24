﻿namespace AsmJitNet
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using Marshal = System.Runtime.InteropServices.Marshal;
    using System.Collections.ObjectModel;

    public abstract class JitMethodBase
    {
        private readonly object _compilerLock = new object();
        private readonly CallingConvention _callingConvention;
        private readonly FunctionHints _hints;
        private readonly CodeGenerator _codeGenerator;
        private Assembler _assembler;
        private Compiler _compiler;
        private IntPtr _result;
        private readonly List<Delegate> _delegates = new List<Delegate>();

        protected JitMethodBase(CallingConvention callingConvention, FunctionHints hints, CodeGenerator codeGenerator)
        {
            _callingConvention = callingConvention;
            _hints = hints;
            _codeGenerator = codeGenerator;
        }

        public CallingConvention CallingConvention
        {
            get
            {
                return _callingConvention;
            }
        }

        public FunctionHints Hints
        {
            get
            {
                return _hints;
            }
        }

        public CodeGenerator CodeGenerator
        {
            get
            {
                return _codeGenerator;
            }
        }

        public bool IsCompiled
        {
            get
            {
                return _result != IntPtr.Zero;
            }
        }

        public IntPtr CompiledAddress
        {
            get
            {
                return _result;
            }
        }

        public abstract ReadOnlyCollection<VariableType> ArgumentTypes
        {
            get;
        }

        public abstract VariableType ReturnType
        {
            get;
        }

        protected Assembler Assembler
        {
            get
            {
                return _assembler;
            }
        }

        protected Compiler Compiler
        {
            get
            {
                return _compiler;
            }
        }

        public Assembler GetAssembler()
        {
            lock (_compilerLock)
            {
                if (_assembler != null)
                    return _assembler;

                if (_compiler != null)
                    throw new InvalidOperationException("This function already has a compiler.");

                _assembler = CreateAssembler();
                return _assembler;
            }
        }

        public Compiler GetCompiler()
        {
            lock (_compilerLock)
            {
                if (_compiler != null)
                    return _compiler;

                if (_assembler != null)
                    throw new InvalidOperationException("This function already has an assembler.");

                _compiler = CreateCompiler();
                return _compiler;
            }
        }

        public void Compile()
        {
            lock (_compilerLock)
            {
                if (_result != IntPtr.Zero)
                    return;

                _result = CompileCore();
            }
        }

        public TDelegate GetDelegate<TDelegate>()
        {
            lock (_compilerLock)
            {
                if (_result == IntPtr.Zero)
                    _result = CompileCore();

                TDelegate existing = _delegates.OfType<TDelegate>().FirstOrDefault();
                if (existing != null)
                    return existing;

                existing = (TDelegate)(object)Marshal.GetDelegateForFunctionPointer(_result, typeof(TDelegate));
                _delegates.Add((Delegate)(object)existing);
                return existing;
            }
        }

        protected virtual Assembler CreateAssembler()
        {
            Assembler assembler = new Assembler(CodeGenerator);
            return assembler;
        }

        protected virtual Compiler CreateCompiler()
        {
            Compiler compiler = new Compiler(CodeGenerator);
            compiler.NewFunction(this.CallingConvention, ArgumentTypes.ToArray(), ReturnType);
            compiler.Function.SetHints(Hints);
            return compiler;
        }

        protected virtual IntPtr CompileCore()
        {
            if (_compiler != null)
            {
                _compiler.EndFunction();
                return _compiler.Make();
            }
            else if (_assembler != null)
            {
                return _assembler.Make();
            }
            else
            {
                throw new InvalidOperationException();
            }
        }
    }
}