namespace NAsmJit
{
    using System.Diagnostics.Contracts;

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

        /// <summary>
        /// Get whether this variable is general purpose BYTE register.
        /// </summary>
        public bool IsGPB
        {
            get
            {
                return (RegisterType & RegType.MASK) <= RegType.GPB_HI;
            }
        }

        /// <summary>
        /// Get whether this variable is general purpose BYTE.LO register.
        /// </summary>
        public bool IsGPBLo
        {
            get
            {
                return (RegisterType & RegType.MASK) == RegType.GPB_LO;
            }
        }

        /// <summary>
        /// Get whether this variable is general purpose BYTE.HI register.
        /// </summary>
        public bool IsGPBHi
        {
            get
            {
                return (RegisterType & RegType.MASK) == RegType.GPB_HI;
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
            Contract.Requires(var != null);

            int memSize = (ptrSize == InvalidValue) ? var.Size : (byte)ptrSize;
            Mem m = new Mem(var.Id, memSize);
            return m;
        }

        public override string ToString()
        {
            return string.Format("{0}:{1}", Id & OperandIdValueMask, VariableType);
        }
    }
}
