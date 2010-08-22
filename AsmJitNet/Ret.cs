namespace AsmJitNet
{
    using System;
    using Debug = System.Diagnostics.Debug;

    public class Ret : Emittable
    {
        private readonly Function _function;
        private readonly Operand _first;
        private readonly Operand _second;

        public Ret(Compiler compiler, Function function, Operand first, Operand second)
            : base(compiler)
        {
            _function = function;
            _first = first;
            _second = second;
        }

        public override EmittableType EmittableType
        {
            get
            {
                return EmittableType.Return;
            }
        }

        public override int MaxSize
        {
            get
            {
                return ShouldEmitJumpToEpilog() ? 15 : 0;
            }
        }

        public Function Function
        {
            get
            {
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

        public override void Prepare(CompilerContext cc)
        {
            Offset = cc.CurrentOffset;

            VariableType retValType = Function.Prototype.ReturnValue;
            if (retValType != VariableType.Invalid)
            {
                for (int i = 0; i < 2; i++)
                {
                    Operand o = (i == 0 ? _first : _second);
                    if (o == null)
                        continue;

                    if (o.IsVar)
                    {
                        Debug.Assert(o.Id != Operand.InvalidValue);
                        VarData vdata = Compiler.GetVarData(o.Id);
                        Debug.Assert(vdata != null);

                        // First emittable (begin of variable scope).
                        if (vdata.FirstEmittable == null)
                            vdata.FirstEmittable = this;

                        // Last emittable (end of variable scope).
                        vdata.LastEmittable = this;

                        if (vdata.WorkOffset == Offset)
                            continue;
                        if (!cc.IsActive(vdata))
                            cc.AddActive(vdata);

                        vdata.WorkOffset = Offset;
                        vdata.RegisterReadCount++;

                        if (vdata.HomeRegisterIndex == RegIndex.Invalid)
                        {
                            if (VariableInfo.IsVariableInteger(vdata.Type) && VariableInfo.IsVariableInteger(retValType))
                            {
                                vdata.HomeRegisterIndex = (i == 0) ? RegIndex.Eax : RegIndex.Edx;
                            }
                        }
                    }
                }
            }

            cc.CurrentOffset++;
        }

        public override void Translate(CompilerContext cc)
        {
            Compiler compiler = cc.Compiler;
            Operand[] ret = { _first, _second };

            // Check whether the return value is compatible.
            VariableType retValType = Function.Prototype.ReturnValue;
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
                            VarData vdata = compiler.GetVarData(ret[i].Id);
                            Debug.Assert(vdata != null);

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
                            throw new NotImplementedException();
                        }
                        else if (((BaseVar)ret[i]).IsXMMVar)
                        {
                            VarData vdata = compiler.GetVarData(ret[i].Id);
                            Debug.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            if (srci != RegIndex.Invalid)
                                cc.SaveXMMVar(vdata);

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
                            VarData vdata = compiler.GetVarData(ret[i].Id);
                            Debug.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            InstructionCode inst = ret[i].IsRegType(RegType.GPQ) ? InstructionCode.Movq : InstructionCode.Movd;

                            if (srci == RegIndex.Invalid)
                            {
                                compiler.Emit(inst, Register.mm(dsti), cc.GetVarMem(vdata));
                            }
                            else
                            {
#if ASMJIT_X86
                                compiler.Emit(inst, Register.mm(dsti), Register.gpd(srci));
#else
                                compiler.Emit(inst, Register.mm(dsti), ret[i].IsRegType(RegType.GPQ) ? Register.gpq(srci) : Register.gpd(srci));
#endif
                            }
                        }
                        else if (((BaseVar)ret[i]).IsMMVar)
                        {
                            VarData vdata = compiler.GetVarData(ret[i].Id);
                            Debug.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            InstructionCode inst = InstructionCode.Movq;

                            if (srci == RegIndex.Invalid)
                                compiler.Emit(inst, Register.mm(dsti), cc.GetVarMem(vdata));
                            else if (dsti != srci)
                                compiler.Emit(inst, Register.mm(dsti), Register.mm(srci));
                        }
                        else if (((BaseVar)ret[i]).IsXMMVar)
                        {
                            VarData vdata = compiler.GetVarData(ret[i].Id);
                            Debug.Assert(vdata != null);

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
                            VarData vdata = compiler.GetVarData(ret[i].Id);
                            Debug.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            InstructionCode inst = ret[i].IsRegType(RegType.GPQ) ? InstructionCode.Movq : InstructionCode.Movd;

                            if (srci == RegIndex.Invalid)
                            {
                                compiler.Emit(inst, Register.xmm(dsti), cc.GetVarMem(vdata));
                            }
                            else
                            {
#if ASMJIT_X86
                                compiler.Emit(inst, Register.xmm(dsti), Register.gpd(srci));
#else
                                compiler.Emit(inst, Register.xmm(dsti), ret[i].IsRegType(RegType.GPQ) ? Register.gpq(srci) : Register.gpd(srci));
#endif
                            }
                        }
                        else if (((BaseVar)ret[i]).IsX87Var)
                        {
                            // TODO: X87.
                            throw new NotImplementedException();
                        }
                        else if (((BaseVar)ret[i]).IsMMVar)
                        {
                            VarData vdata = compiler.GetVarData(ret[i].Id);
                            Debug.Assert(vdata != null);

                            srci = vdata.RegisterIndex;
                            if (srci == RegIndex.Invalid)
                                compiler.Emit(InstructionCode.Movq, Register.xmm(dsti), cc.GetVarMem(vdata));
                            else
                                compiler.Emit(InstructionCode.Movq, Register.xmm(dsti), Register.mm(srci));
                        }
                        else if (((BaseVar)ret[i]).IsXMMVar)
                        {
                            VarData vdata = compiler.GetVarData(ret[i].Id);
                            Debug.Assert(vdata != null);

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
                            VarData vdata = compiler.GetVarData(ret[i].Id);
                            Debug.Assert(vdata != null);

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
                            VarData vdata = compiler.GetVarData(ret[i].Id);
                            Debug.Assert(vdata != null);

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
                    VarData vdata = compiler.GetVarData(ret[i].Id);
                    cc.UnuseVarOnEndOfScope(this, vdata);
                }
            }
        }

        private Mem BaseVarMem(BaseVar var, int ptrSize)
        {
            Mem m = new Mem();

            m.Size = (ptrSize == Operand.InvalidValue ? var.Size : (byte)ptrSize);
            m.MemoryType = MemoryType.Native;
            m.SegmentPrefix = SegmentPrefix.None;

            m.Id = var.Id;

            m.Base = RegIndex.Invalid;
            m.Index = RegIndex.Invalid;
            m.Shift = 0;

            m.Target = IntPtr.Zero;
            m.Displacement = IntPtr.Zero;

            return m;
        }

        public override void Emit(Assembler a)
        {
            if (ShouldEmitJumpToEpilog())
                a.Jmp(Function.ExitLabel);
        }

        public bool ShouldEmitJumpToEpilog()
        {
            // Iterate over next emittables. If we found emittable that emits real 
            // instruction then we must return @c true.
            Emittable e = this.Next;

            while (e != null)
            {
                switch (e.EmittableType)
                {
                // Non-interesting emittables.
                case EmittableType.Comment:
                case EmittableType.Dummy:
                case EmittableType.Align:
                case EmittableType.Block:
                case EmittableType.VariableHint:
                case EmittableType.Target:
                    break;

                // Interesting emittables.
                case EmittableType.EmbeddedData:
                case EmittableType.Instruction:
                case EmittableType.JumpTable:
                case EmittableType.Call:
                case EmittableType.Return:
                    return true;

                // These emittables shouldn't be here. We are inside function, after
                // prolog.
                case EmittableType.Function:
                case EmittableType.Prolog:
                    break;

                // Stop station, we can't next.
                case EmittableType.Epilog:
                    return false;
                }
                e = e.Next;
            }

            return false;
        }
    }
}
