namespace NAsmJit
{
    using System;
    using System.Diagnostics.Contracts;

    public sealed class CompilerTarget : CompilerItem
    {
        private readonly Label _label;
        private CompilerJmpInstruction _from;
        private StateData _state;
        private int _jumpsCount;

        public CompilerTarget(Compiler compiler, Label label)
            : base(compiler)
        {
            if (label == null)
                throw new ArgumentNullException("label");
            Contract.Requires(compiler != null);
            Contract.EndContractBlock();

            _label = label;
        }

        public override ItemType ItemType
        {
            get
            {
                return ItemType.Target;
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

        public CompilerJmpInstruction From
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

        protected override void PrepareImpl(CompilerContext cc)
        {
            Offset = cc.CurrentOffset++;
        }

        protected override CompilerItem TranslateImpl(CompilerContext cc)
        {
            // If this Target was already translated, it's needed to change the current
            // state and return null to tell CompilerContext to process next untranslated
            // item.
            if (IsTranslated)
            {
                cc.RestoreState(_state);
                return null;
            }

            if (cc.Unreachable)
            {
                // If the context has "isUnreachable" flag set and there is no state then
                // it means that this code will be never called. This is a problem, because
                // we are unable to assign a state to current location so we can't allocate
                // registers for variables used inside. So instead of doing anything wrong
                // we remove the unreachable code.
                if (_state == null)
                    return RemoveUnreachableItems();

                // Assign state to the compiler context.
                cc.Unreachable = false;
                cc.AssignState(_state);
            }
            else
            {
                _state = cc.SaveState();
            }

            return Next;
        }

        protected override void EmitImpl(Assembler a)
        {
            a.MarkLabel(_label);
        }

        private CompilerTarget RemoveUnreachableItems()
        {
            Contract.Requires(Previous != null);
            Contract.Requires(Next != null);

            CompilerItem prev = Previous;
            CompilerItem item = Next;

            while (true)
            {
                CompilerItem next = item.Next;
                Contract.Assert(next != null);

                if (item.ItemType == ItemType.Target)
                    break;

                item.Previous = null;
                item.Next = null;
                item.IsUnreachable = true;

                item = next;
            }

            this.Previous = null;
            this.Next = null;

            prev.Next = item;
            item.Previous = prev;

            return (CompilerTarget)item;
        }
    }
}
