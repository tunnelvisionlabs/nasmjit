﻿namespace AsmJitNet2
{
    using System;

    public sealed class Prolog : Emittable
    {
        private Function _function;

        public Prolog(Compiler compiler, Function function)
            : base(compiler)
        {
            _function = function;
        }

        public Function Function
        {
            get
            {
                return _function;
            }
        }

        public override EmittableType EmittableType
        {
            get
            {
                return EmittableType.Prolog;
            }
        }

        public override void Prepare(CompilerContext cc)
        {
            Offset = cc.CurrentOffset++;
            _function.PrepareVariables(this);
        }

        public override void Translate(CompilerContext cc)
        {
            _function.AllocVariables(cc);
        }
    }
}