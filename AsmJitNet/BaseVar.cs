namespace AsmJitNet
{
    using System;

    public abstract class BaseVar : Operand
    {
        private readonly RegType _registerType;

        private readonly VariableType _variableType;

        protected BaseVar(RegType registerType, VariableType variableType)
        {
            _registerType = registerType;
            _variableType = variableType;
        }

        protected BaseVar(int id, int size, RegType registerType, VariableType variableType)
            : base(id, size)
        {
            _registerType = registerType;
            _variableType = variableType;
        }

        public override sealed OperandType OperandType
        {
            get
            {
                return OperandType.Var;
            }
        }

        public RegType RegisterType
        {
            get
            {
                return _registerType;
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
