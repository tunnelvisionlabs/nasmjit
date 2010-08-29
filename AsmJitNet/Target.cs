namespace AsmJitNet
{
    using System;
    using System.Diagnostics.Contracts;

    public sealed class Target : Emittable
    {
        private readonly Label _label;
        private Jmp _from;
        private StateData _state;
        private int _jumpsCount;

        public Target(Compiler compiler, Label label)
            : base(compiler)
        {
            if (label == null)
                throw new ArgumentNullException("label");
            Contract.Requires(compiler != null);
            Contract.EndContractBlock();

            _label = label;
        }

        public override EmittableType EmittableType
        {
            get
            {
                return EmittableType.Target;
            }
        }

        public override int MaxSize
        {
            get
            {
                return 0;
            }
        }

        public Label Label
        {
            get
            {
                return _label;
            }
        }

        public Jmp From
        {
            get
            {
                return _from;
            }

            internal set
            {
                _from = value;
            }
        }

        public StateData State
        {
            get
            {
                return _state;
            }

            internal set
            {
                _state = value;
            }
        }

        public int JumpsCount
        {
            get
            {
                return _jumpsCount;
            }

            internal set
            {
                _jumpsCount = value;
            }
        }

        public override void Prepare(CompilerContext cc)
        {
            Offset = cc.CurrentOffset++;
        }

        public override void Translate(CompilerContext cc)
        {
            if (cc.Unreachable)
            {
                cc.Unreachable = false;

                // Assign state to the compiler context. 
                if (_state == null)
                    throw new CompilerException();

                cc.AssignState(_state);
            }
            else
            {
                _state = cc.SaveState();
            }
        }

        public override void Emit(Assembler a)
        {
            a.Bind(_label);
        }
    }
}
