namespace AsmJitNet
{
    using System.Diagnostics.Contracts;

    public abstract class Emittable
    {
        protected internal const int InvalidValue = -1;

        private readonly Compiler _compiler;

        private int _offset;

        private Emittable _previous;

        private Emittable _next;

        private string _comment;

        protected Emittable(Compiler compiler)
        {
            Contract.Requires(compiler != null);

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

        [ContractInvariantMethod]
        private void ObjectInvariants()
        {
            Contract.Invariant(_compiler != null);
        }

        public virtual void Prepare(CompilerContext cc)
        {
            Contract.Requires(cc != null);

            _offset = cc.CurrentOffset;
        }

        public virtual void Translate(CompilerContext cc)
        {
            Contract.Requires(cc != null);
        }

        public virtual void Emit(Assembler a)
        {
            Contract.Requires(a != null);
        }

        public virtual void Post(Assembler a)
        {
            Contract.Requires(a != null);
        }
    }
}
