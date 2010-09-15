namespace AsmJitNet
{
    using System;
    using System.Diagnostics.Contracts;

    public abstract class Emittable
    {
        protected internal const int InvalidValue = -1;

        private readonly Compiler _compiler;

        private int _offset;

        private Emittable _previous;

        private Emittable _next;

        private string _comment;

        private bool _translated;

        protected Emittable(Compiler compiler)
        {
            if (compiler == null)
                throw new ArgumentNullException("compiler");
            Contract.EndContractBlock();

            this._compiler = compiler;
            this._offset = InvalidValue;
        }

        public Compiler Compiler
        {
            get
            {
                Contract.Ensures(Contract.Result<Compiler>() != null);

                return _compiler;
            }
        }

        public string Comment
        {
            get
            {
                return _comment;
            }

            set
            {
                _comment = value;
            }
        }

        public Emittable Previous
        {
            get
            {
                return _previous;
            }

            internal set
            {
                _previous = value;
            }
        }

        public Emittable Next
        {
            get
            {
                return _next;
            }

            internal set
            {
                _next = value;
            }
        }

        public int Offset
        {
            get
            {
                return _offset;
            }

            internal set
            {
                _offset = value;
            }
        }

        public abstract EmittableType EmittableType
        {
            get;
        }

        public virtual int MaxSize
        {
            get
            {
                // Default maximum size is -1 which means that it's not known
                return -1;
            }
        }

        public bool IsTranslated
        {
            get
            {
                return _translated;
            }
        }

        [ContractInvariantMethod]
        private void ObjectInvariants()
        {
            Contract.Invariant(_compiler != null);
        }

        public void Prepare(CompilerContext cc)
        {
            Contract.Requires(cc != null);

            PrepareImpl(cc);
        }

        public Emittable Translate(CompilerContext cc)
        {
            Contract.Requires(cc != null);

            Emittable next = TranslateImpl(cc);
            _translated = true;
            return next;
        }

        public void Emit(Assembler a)
        {
            Contract.Requires(a != null);

            EmitImpl(a);
        }

        public void Post(Assembler a)
        {
            Contract.Requires(a != null);

            PostImpl(a);
        }

        protected virtual void PrepareImpl(CompilerContext cc)
        {
            Contract.Requires(cc != null);

            _offset = cc.CurrentOffset;
        }

        protected virtual Emittable TranslateImpl(CompilerContext cc)
        {
            Contract.Requires(cc != null);
            return Next;
        }

        protected virtual void EmitImpl(Assembler a)
        {
            Contract.Requires(a != null);
        }

        protected virtual void PostImpl(Assembler a)
        {
            Contract.Requires(a != null);
        }
    }
}
