namespace AsmJitNet
{
    using System;

    public abstract class BaseVar : Operand
    {
        private readonly RegType _registerCode;

        private readonly VariableType _variableType;

        protected BaseVar(RegType registerCode, VariableType variableType)
        {
            _registerCode = registerCode;
            _variableType = variableType;
        }

        protected BaseVar(int id, int size, RegType registerCode, VariableType variableType)
            : base(id, size)
        {
            _registerCode = registerCode;
            _variableType = variableType;
        }

        public override sealed OperandType OperandType
        {
            get
            {
                return OperandType.Var;
            }
        }

        public RegType RegisterCode
        {
            get
            {
                return _registerCode;
            }
        }

        public VariableType VariableType
        {
            get
            {
                return _variableType;
            }
        }

        public bool IsGPVar
        {
            get
            {
                return VariableType <= VariableType.GPQ;
            }
        }

        public bool IsX87Var
        {
            get
            {
                return VariableType >= VariableType.X87 && VariableType <= VariableType.X87_1D;
            }
        }

        public bool IsMMVar
        {
            get
            {
                return VariableType == VariableType.MM;
            }
        }

        public bool IsXMMVar
        {
            get
            {
                return VariableType >= VariableType.XMM && VariableType <= VariableType.XMM_2D;
            }
        }

        public Mem ToMem()
        {
            return BaseVarMem(this, InvalidValue);
        }

        internal static Mem BaseVarMem(BaseVar var, int ptrSize)
        {
            int memSize = (ptrSize == InvalidValue) ? var.Size : (byte)ptrSize;
            Mem m = new Mem(var.Id, memSize);
            return m;
        }
    }
}
