using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AsmJitNet2
{
    public abstract class Emittable
    {
        protected internal const int InvalidValue = -1;

        private Compiler _compiler;

        private int _offset;

        private Emittable _previous;

        private Emittable _next;

        private string _comment;

        public Emittable(Compiler compiler)
        {
            this._compiler = compiler;
            this._offset = InvalidValue;
        }

        public Compiler Compiler
        {
            get
            {
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

        public virtual void Prepare(CompilerContext cc)
        {
            _offset = cc.CurrentOffset;
        }

        public virtual void Translate(CompilerContext cc)
        {
        }

        public virtual void Emit(Assembler a)
        {
        }

        public virtual void Post(Assembler a)
        {
        }
    }
}
