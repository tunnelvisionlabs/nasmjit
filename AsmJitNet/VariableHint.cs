namespace AsmJitNet2
{
    using System;

    public class VariableHint : Emittable
    {
        private VarData _varData;

        private VariableHintKind _hintKind;

        private int _hintValue;

        public VariableHint(Compiler compiler, VarData varData, VariableHintKind hintKind, int hintValue)
            : base(compiler)
        {
            if (varData == null)
                throw new ArgumentNullException("varData");

            _varData = varData;
            _hintKind = hintKind;
            _hintValue = hintValue;
        }

        public override EmittableType EmittableType
        {
            get
            {
                return EmittableType.VariableHint;
            }
        }

        public override int MaxSize
        {
            get
            {
                // Variable hint is NOP, but it can generate other emittables which can do something
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

        public override void Prepare(CompilerContext cc)
        {
            Offset = cc.CurrentOffset;

            // First emittable (begin of variable scope).
            if (_varData.FirstEmittable == null)
                _varData.FirstEmittable = this;

            // Last emittable (end of variable scope).
            _varData.LastEmittable = this;

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
                goto case VariableHintKind.Unuse;

            case VariableHintKind.Unuse:
                break;
            }
        }

        public override void Translate(CompilerContext cc)
        {
            switch (_hintKind)
            {
            case VariableHintKind.Alloc:
                cc.AllocVar(_varData, (RegIndex)_hintValue, VariableAlloc.ReadWrite);
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
                return;
            }

            cc.UnuseVarOnEndOfScope(this, _varData);
        }
    }
}
