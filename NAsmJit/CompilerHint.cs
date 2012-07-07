namespace NAsmJit
{
    using System;
    using System.Diagnostics.Contracts;

    public class CompilerHint : CompilerItem
    {
        private readonly CompilerVar _varData;

        private readonly VariableHintKind _hintKind;

        private readonly int _hintValue;

        public CompilerHint(Compiler compiler, CompilerVar varData, VariableHintKind hintKind, int hintValue)
            : base(compiler)
        {
            if (varData == null)
                throw new ArgumentNullException("varData");
            Contract.Requires(compiler != null);
            Contract.EndContractBlock();

            _varData = varData;
            _hintKind = hintKind;
            _hintValue = hintValue;
        }

        public override ItemType ItemType
        {
            get
            {
                return ItemType.VariableHint;
            }
        }

        public override int MaxSize
        {
            get
            {
                // Variable hint is NOP, but it can generate other items which can do something
                return 0;
            }
        }

        public VariableHintKind HintKind
        {
            get
            {
                return _hintKind;
            }
        }

        public int HintValue
        {
            get
            {
                return _hintValue;
            }
        }

        protected override void PrepareImpl(CompilerContext cc)
        {
            Offset = cc.CurrentOffset;

            // First item (begin of variable scope).
            if (_varData.FirstItem == null)
                _varData.FirstItem = this;

            CompilerItem oldLast = _varData.LastItem;

            // Last item (end of variable scope).
            _varData.LastItem = this;

            switch (_hintKind)
            {
            case VariableHintKind.Alloc:
            case VariableHintKind.Spill:
            case VariableHintKind.Save:
                if (!cc.IsActive(_varData))
                    cc.AddActive(_varData);
                break;

            case VariableHintKind.SaveAndUnuse:
                if (!cc.IsActive(_varData))
                    cc.AddActive(_varData);
                break;

            case VariableHintKind.Unuse:
                if (oldLast != null)
                    oldLast.TryUnuseVar(_varData);
                break;
            }
        }

        protected override CompilerItem TranslateImpl(CompilerContext cc)
        {
            switch (_hintKind)
            {
            case VariableHintKind.Alloc:
                cc.AllocVar(_varData, new RegisterMask(_hintValue), VariableAlloc.Read);
                break;

            case VariableHintKind.Spill:
                if (_varData.State == VariableState.Register)
                    cc.SpillVar(_varData);
                break;

            case VariableHintKind.Save:
            case VariableHintKind.SaveAndUnuse:
                if (_varData.State == VariableState.Register && _varData.Changed)
                {
                    cc.EmitSaveVar(_varData, _varData.RegisterIndex);
                    _varData.Changed = false;
                }

                if (_hintKind == VariableHintKind.SaveAndUnuse)
                    goto case VariableHintKind.Unuse;

                break;

            case VariableHintKind.Unuse:
                cc.UnuseVar(_varData, VariableState.Unused);
                goto end;
            }

            cc.UnuseVarOnEndOfScope(this, _varData);

        end:
            return Next;
        }
    }
}
