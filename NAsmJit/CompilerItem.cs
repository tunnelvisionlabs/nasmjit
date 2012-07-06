namespace NAsmJit
{
    using System;
    using System.Diagnostics.Contracts;

    public abstract class CompilerItem
    {
        protected internal const int InvalidValue = -1;

        private readonly Compiler _compiler;

        private int _offset;

        private CompilerItem _previous;

        private CompilerItem _next;

        private string _comment;

        private bool _isTranslated;

        private bool _isUnreachable;

        protected CompilerItem(Compiler compiler)
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

        public CompilerItem Previous
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

        public CompilerItem Next
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

        public abstract ItemType ItemType
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

        /// <summary>
        /// Get whether the item was translated.
        /// </summary>
        public bool IsTranslated
        {
            get
            {
                return _isTranslated;
            }
        }

        /// <summary>
        /// Get whether the item is unreachable.
        /// </summary>
        public bool IsUnreachable
        {
            get
            {
                return _isUnreachable;
            }

            set
            {
                _isUnreachable = value;
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

        public CompilerItem Translate(CompilerContext cc)
        {
            Contract.Requires(cc != null);

            CompilerItem next = TranslateImpl(cc);
            Contract.Assert(!_isTranslated || next == null);
            _isTranslated = true;
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

        /// <summary>
        /// Try to unuse the variable <paramref name="v"/>.
        /// </summary>
        /// <param name="v"></param>
        /// <returns>
        /// @c true only if the variable will be unused by the instruction,
        /// otherwise @c false is returned.
        /// </returns>
        public bool TryUnuseVar(CompilerVar v)
        {
            Contract.Requires(v != null);

            return TryUnuseVarImpl(v);
        }

        protected virtual void PrepareImpl(CompilerContext cc)
        {
            Contract.Requires(cc != null);

            _offset = cc.CurrentOffset;
        }

        protected virtual CompilerItem TranslateImpl(CompilerContext cc)
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

        protected virtual bool TryUnuseVarImpl(CompilerVar v)
        {
            Contract.Requires(v != null);
            return false;
        }
    }
}
