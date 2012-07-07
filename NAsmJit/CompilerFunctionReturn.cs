namespace NAsmJit
{
    using System;
    using System.Diagnostics.Contracts;

    public class CompilerFunctionReturn : CompilerItem
    {
        private readonly CompilerFunction _function;
        private readonly Operand _first;
        private readonly Operand _second;

        public CompilerFunctionReturn(Compiler compiler, CompilerFunction function, Operand first, Operand second)
            : base(compiler)
        {
            Contract.Requires(compiler != null);
            Contract.Requires(function != null);

            _function = function;
            _first = first;
            _second = second;
        }

        public override ItemType ItemType
        {
            get
            {
                return ItemType.Return;
            }
        }

        public override int MaxSize
        {
            get
            {
                return ShouldEmitJumpToEpilog() ? 15 : 0;
            }
        }

        public CompilerFunction Function
        {
            get
            {
                Contract.Ensures(Contract.Result<CompilerFunction>() != null);

                return _function;
            }
        }

        public Operand First
        {
            get
            {
                return _first;
            }
        }

        public Operand Second
        {
            get
            {
                return _second;
            }
        }

        [ContractInvariantMethod]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance", "CA1822:MarkMembersAsStatic", Justification = "Required for code contracts.")]
        private void ObjectInvariant()
        {
            Contract.Invariant(_function != null);
        }

        protected override void PrepareImpl(CompilerContext cc)
        {
            Offset = cc.CurrentOffset;

            VariableType retValType = Function.Declaration.ReturnValue;
            if (retValType != VariableType.Invalid)
            {
                for (int i = 0; i < 2; i++)
                {
                    Operand o = (i == 0 ? _first : _second);
                    if (o == null)
                        continue;

                    if (o.IsVar)
                    {
                        if (o.Id == Operand.InvalidValue)
                            throw new CompilerException();

                        CompilerVar vdata = Compiler.GetVarData(o.Id);
                        Contract.Assert(vdata != null);

                        // First item (begin of variable scope).
                        if (vdata.FirstItem == null)
                            vdata.FirstItem = this;

                        // Last item (end of variable scope).
                        vdata.LastItem = this;

                        if (vdata.WorkOffset == Offset)
                            continue;
                        if (!cc.IsActive(vdata))
                            cc.AddActive(vdata);

                        vdata.WorkOffset = Offset;
                        vdata.RegisterReadCount++;

                        if (VariableInfo.IsVariableInteger(vdata.Type) && VariableInfo.IsVariableInteger(retValType))
                        {
                            cc.NewRegisterHomeIndex(vdata, (i == 0) ? RegIndex.Eax : RegIndex.Edx);
                        }
                    }
                }
            }

            cc.CurrentOffset++;
        }

        protected override CompilerItem TranslateImpl(CompilerContext cc)
        {
            Compiler compiler = cc.Compiler;
            Operand[] ret = { _first, _second };

            // Check whether the return value is compatible.
            VariableType retValType = Function.Declaration.ReturnValue;
            int i;

            switch (retValType)
            {
            case VariableType.GPD:
            case VariableType.GPQ:
                for (i = 0; i < 2; i++)
                {
                    if (ret[i] == null)
                        continue;

                    RegIndex dsti = (i == 0) ? RegIndex.Eax : RegIndex.Edx;
                    RegIndex srci;

                    if (ret[i].IsVar)
                    {
                        if (((BaseVar)ret[i]).IsGPVar)
                        {
                            CompilerVar vdata = compiler.GetVarData(ret[i].Id);
                            Contract.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            if (srci == RegIndex.Invalid)
                                compiler.Emit(InstructionCode.Mov, Register.gpn(dsti), cc.GetVarMem(vdata));
                            else if (dsti != srci)
                                compiler.Emit(InstructionCode.Mov, Register.gpn(dsti), Register.gpn(srci));
                        }
                    }
                    else if (ret[i].IsImm)
                    {
                        compiler.Emit(InstructionCode.Mov, Register.gpn(dsti), ret[i]);
                    }
                }
                break;

            case VariableType.X87:
            case VariableType.X87_1F:
            case VariableType.X87_1D:
                // There is case that we need to return two values (Unix-ABI specific):
                // - FLD #2
                //-  FLD #1
                i = 2;
                do
                {
                    i--;
                    int dsti = i;
                    RegIndex srci;

                    if (ret[i] == null)
                        continue;

                    if (ret[i].IsVar)
                    {
                        if (((BaseVar)ret[i]).IsX87Var)
                        {
                            // TODO: X87.
                            throw new NotImplementedException("X87 support is not yet implemented.");
                        }
                        else if (((BaseVar)ret[i]).IsXMMVar)
                        {
                            CompilerVar vdata = compiler.GetVarData(ret[i].Id);
                            Contract.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            if (srci != RegIndex.Invalid)
                                cc.SaveVar(vdata);

                            switch (vdata.Type)
                            {
                            case VariableType.XMM_1F:
                            case VariableType.XMM_4F:
                                compiler.Emit(InstructionCode.Fld, BaseVarMem(((BaseVar)ret[i]), 4));
                                break;
                            case VariableType.XMM_1D:
                            case VariableType.XMM_2D:
                                compiler.Emit(InstructionCode.Fld, BaseVarMem(((BaseVar)ret[i]), 8));
                                break;
                            }
                        }
                    }
                } while (i != 0);
                break;

            case VariableType.MM:
                for (i = 0; i < 2; i++)
                {
                    RegIndex dsti = (RegIndex)i;
                    RegIndex srci;

                    if (ret[i] == null)
                        continue;

                    if (ret[i].IsVar)
                    {
                        if (((BaseVar)ret[i]).IsGPVar)
                        {
                            CompilerVar vdata = compiler.GetVarData(ret[i].Id);
                            Contract.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            InstructionCode inst = ret[i].IsRegType(RegType.GPQ) ? InstructionCode.Movq : InstructionCode.Movd;

                            if (srci == RegIndex.Invalid)
                            {
                                compiler.Emit(inst, Register.mm(dsti), cc.GetVarMem(vdata));
                            }
                            else
                            {
                                if (Util.IsX86)
                                    compiler.Emit(inst, Register.mm(dsti), Register.gpd(srci));
                                else if (Util.IsX64)
                                    compiler.Emit(inst, Register.mm(dsti), ret[i].IsRegType(RegType.GPQ) ? Register.gpq(srci) : Register.gpd(srci));
                                else
                                    throw new NotImplementedException();
                            }
                        }
                        else if (((BaseVar)ret[i]).IsMMVar)
                        {
                            CompilerVar vdata = compiler.GetVarData(ret[i].Id);
                            Contract.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            InstructionCode inst = InstructionCode.Movq;

                            if (srci == RegIndex.Invalid)
                                compiler.Emit(inst, Register.mm(dsti), cc.GetVarMem(vdata));
                            else if (dsti != srci)
                                compiler.Emit(inst, Register.mm(dsti), Register.mm(srci));
                        }
                        else if (((BaseVar)ret[i]).IsXMMVar)
                        {
                            CompilerVar vdata = compiler.GetVarData(ret[i].Id);
                            Contract.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            InstructionCode inst = InstructionCode.Movq;
                            if (((BaseVar)ret[i]).VariableType == VariableType.XMM_1F)
                                inst = InstructionCode.Movd;

                            if (srci == RegIndex.Invalid)
                                compiler.Emit(inst, Register.mm(dsti), cc.GetVarMem(vdata));
                            else
                                compiler.Emit(inst, Register.mm(dsti), Register.xmm(srci));
                        }
                    }
                }
                break;

            case VariableType.XMM:
            case VariableType.XMM_4F:
            case VariableType.XMM_2D:
                for (i = 0; i < 2; i++)
                {
                    RegIndex dsti = (RegIndex)i;
                    RegIndex srci;

                    if (ret[i] == null)
                        continue;

                    if (ret[i].IsVar)
                    {
                        if (((BaseVar)ret[i]).IsGPVar)
                        {
                            CompilerVar vdata = compiler.GetVarData(ret[i].Id);
                            Contract.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            InstructionCode inst = ret[i].IsRegType(RegType.GPQ) ? InstructionCode.Movq : InstructionCode.Movd;

                            if (srci == RegIndex.Invalid)
                            {
                                compiler.Emit(inst, Register.xmm(dsti), cc.GetVarMem(vdata));
                            }
                            else
                            {
                                if (Util.IsX86)
                                    compiler.Emit(inst, Register.xmm(dsti), Register.gpd(srci));
                                else if (Util.IsX64)
                                    compiler.Emit(inst, Register.xmm(dsti), ret[i].IsRegType(RegType.GPQ) ? Register.gpq(srci) : Register.gpd(srci));
                                else
                                    throw new NotImplementedException();
                            }
                        }
                        else if (((BaseVar)ret[i]).IsX87Var)
                        {
                            // TODO: X87.
                            throw new NotImplementedException("X87 support is not yet implemented.");
                        }
                        else if (((BaseVar)ret[i]).IsMMVar)
                        {
                            CompilerVar vdata = compiler.GetVarData(ret[i].Id);
                            Contract.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            if (srci == RegIndex.Invalid)
                                compiler.Emit(InstructionCode.Movq, Register.xmm(dsti), cc.GetVarMem(vdata));
                            else
                                compiler.Emit(InstructionCode.Movq, Register.xmm(dsti), Register.mm(srci));
                        }
                        else if (((BaseVar)ret[i]).IsXMMVar)
                        {
                            CompilerVar vdata = compiler.GetVarData(ret[i].Id);
                            Contract.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            if (srci == RegIndex.Invalid)
                                compiler.Emit(InstructionCode.Movdqa, Register.xmm(dsti), cc.GetVarMem(vdata));
                            else if (dsti != srci)
                                compiler.Emit(InstructionCode.Movdqa, Register.xmm(dsti), Register.xmm(srci));
                        }
                    }
                }
                break;

            case VariableType.XMM_1F:
                for (i = 0; i < 2; i++)
                {
                    RegIndex dsti = (RegIndex)i;
                    RegIndex srci;

                    if (ret[i] == null)
                        continue;

                    if (ret[i].IsVar)
                    {
                        if (((BaseVar)ret[i]).IsX87Var)
                        {
                            // TODO: X87.
                        }
                        else if (((BaseVar)ret[i]).IsXMMVar)
                        {
                            CompilerVar vdata = compiler.GetVarData(ret[i].Id);
                            Contract.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            switch (vdata.Type)
                            {
                            case VariableType.XMM:
                                if (srci == RegIndex.Invalid)
                                    compiler.Emit(InstructionCode.Movdqa, Register.xmm(dsti), cc.GetVarMem(vdata));
                                else if (dsti != srci)
                                    compiler.Emit(InstructionCode.Movdqa, Register.xmm(dsti), Register.xmm(srci));
                                break;
                            case VariableType.XMM_1F:
                            case VariableType.XMM_4F:
                                if (srci == RegIndex.Invalid)
                                    compiler.Emit(InstructionCode.Movss, Register.xmm(dsti), cc.GetVarMem(vdata));
                                else
                                    compiler.Emit(InstructionCode.Movss, Register.xmm(dsti), Register.xmm(srci));
                                break;
                            case VariableType.XMM_1D:
                            case VariableType.XMM_2D:
                                if (srci == RegIndex.Invalid)
                                    compiler.Emit(InstructionCode.Cvtsd2ss, Register.xmm(dsti), cc.GetVarMem(vdata));
                                else if (dsti != srci)
                                    compiler.Emit(InstructionCode.Cvtsd2ss, Register.xmm(dsti), Register.xmm(srci));
                                break;
                            }
                        }
                    }
                }
                break;

            case VariableType.XMM_1D:
                for (i = 0; i < 2; i++)
                {
                    RegIndex dsti = (RegIndex)i;
                    RegIndex srci;

                    if (ret[i] == null)
                        continue;

                    if (ret[i].IsVar)
                    {
                        if (((BaseVar)ret[i]).IsX87Var)
                        {
                            // TODO: X87.
                        }
                        else if (((BaseVar)ret[i]).IsXMMVar)
                        {
                            CompilerVar vdata = compiler.GetVarData(ret[i].Id);
                            Contract.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            switch (vdata.Type)
                            {
                            case VariableType.XMM:
                                if (srci == RegIndex.Invalid)
                                    compiler.Emit(InstructionCode.Movdqa, Register.xmm(dsti), cc.GetVarMem(vdata));
                                else if (dsti != srci)
                                    compiler.Emit(InstructionCode.Movdqa, Register.xmm(dsti), Register.xmm(srci));
                                break;
                            case VariableType.XMM_1F:
                            case VariableType.XMM_4F:
                                if (srci == RegIndex.Invalid)
                                    compiler.Emit(InstructionCode.Cvtss2sd, Register.xmm(dsti), cc.GetVarMem(vdata));
                                else
                                    compiler.Emit(InstructionCode.Cvtss2sd, Register.xmm(dsti), Register.xmm(srci));
                                break;
                            case VariableType.XMM_1D:
                            case VariableType.XMM_2D:
                                if (srci == RegIndex.Invalid)
                                    compiler.Emit(InstructionCode.Movsd, Register.xmm(dsti), cc.GetVarMem(vdata));
                                else
                                    compiler.Emit(InstructionCode.Movsd, Register.xmm(dsti), Register.xmm(srci));
                                break;
                            }
                        }
                    }
                }
                break;

            case VariableType.Invalid:
            default:
                break;
            }

            if (ShouldEmitJumpToEpilog())
            {
                cc.Unreachable = true;
            }

            for (i = 0; i < 2; i++)
            {
                if (ret[i] != null && ret[i].IsVar)
                {
                    CompilerVar vdata = compiler.GetVarData(ret[i].Id);
                    Contract.Assert(vdata != null);
                    cc.UnuseVarOnEndOfScope(this, vdata);
                }
            }

            return Next;
        }

        private Mem BaseVarMem(BaseVar var, int ptrSize)
        {
            Contract.Requires(var != null);

            int memSize = (ptrSize == Operand.InvalidValue ? var.Size : (byte)ptrSize);
            Mem m = new Mem(var.Id, memSize);
            return m;
        }

        protected override void EmitImpl(Assembler a)
        {
            if (ShouldEmitJumpToEpilog())
                a.Jmp(Function.ExitLabel);
        }

        public bool ShouldEmitJumpToEpilog()
        {
            // Iterate over next items. If we found item that emits real 
            // instruction then we must return @c true.
            CompilerItem e = this.Next;

            while (e != null)
            {
                switch (e.ItemType)
                {
                // Interesting items.
                case ItemType.EmbeddedData:
                case ItemType.Instruction:
                case ItemType.Call:
                case ItemType.Return:
                    return true;

                // Non-interesting items.
                case ItemType.Comment:
                case ItemType.Mark:
                case ItemType.Align:
                case ItemType.VariableHint:
                    break;

                case ItemType.Target:
                    if (((CompilerTarget)e).Label.Id == Function.ExitLabel.Id)
                        return false;

                    break;

                // These items shouldn't be here. We are inside the function, after
                // prolog.
                case ItemType.Function:
                    throw new InvalidOperationException();

                // We can't go forward from here.
                case ItemType.FunctionEnd:
                    return false;

                default:
                    throw new NotImplementedException();
                }

                e = e.Next;
            }

            return false;
        }
    }
}
