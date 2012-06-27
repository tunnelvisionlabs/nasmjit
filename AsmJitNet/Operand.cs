namespace AsmJitNet
{
    public abstract class Operand
    {
        public static readonly Operand[] EmptyOperands = new Operand[0];
        public static readonly Operand None = new NoneOperand();

        public const int InvalidValue = -1;
        public const int OperandIdValueMask = unchecked((int)0x3FFFFFFF);
        public const int OperandIdTypeMask = unchecked((int)0xC0000000);
        public const int OperandIdTypeLabel = unchecked((int)0x40000000);
        public const int OperandIdTypeVar = unchecked((int)0x80000000);

        private readonly byte _size;
        private readonly int _id;

        protected Operand(int id = InvalidValue, int size = 0)
        {
            _id = id;
            _size = checked((byte)size);
        }

        public abstract OperandType OperandType
        {
            get;
        }

        public int Id
        {
            get
            {
                return _id;
            }
        }

        public byte Size
        {
            get
            {
                return _size;
            }
        }

        public bool IsNone
        {
            get
            {
                return OperandType == OperandType.None;
            }
        }

        public bool IsReg
        {
            get
            {
                return OperandType == OperandType.Reg;
            }
        }

        public bool IsMem
        {
            get
            {
                return OperandType == OperandType.Mem;
            }
        }

        public bool IsImm
        {
            get
            {
                return OperandType == OperandType.Imm;
            }
        }

        public bool IsLabel
        {
            get
            {
                return OperandType == OperandType.Label;
            }
        }

        public bool IsVar
        {
            get
            {
                return OperandType == OperandType.Var;
            }
        }

        public bool IsVarMem
        {
            get
            {
                return IsVar || IsMem;
            }
        }

        public bool IsRegMem
        {
            get
            {
                return IsReg || IsMem;
            }
        }

        public bool IsExtendedRegisterUsed
        {
            get
            {
                // Hacky, but correct.
                // - If operand type is register then extended register is register with
                //   index 8 and greater (8 to 15 inclusive).
                // - If operand type is memory operand then we need to take care about
                //   label (in _mem.base) and INVALID_VALUE, we just decrement the value
                //   by 8 and check if it's at interval 0 to 7 inclusive (if it's there
                //   then it's extended register.
                return (IsReg && ((int)((BaseReg)this).Code & (int)RegIndex.Mask) >= 8) ||
                       (IsMem && ((((uint)((Mem)this).Base - 8U) < 8U) ||
                                   (((uint)((Mem)this).Index - 8U) < 8U)));
            }
        }

        public bool IsRegType(RegType regType)
        {
            return IsReg && ((BaseReg)this).RegisterType == regType;
        }

        public bool IsRegCode(RegCode code)
        {
            return IsReg && ((BaseReg)this).Code == code;
        }

        public bool IsRegIndex(RegIndex regIndex)
        {
            return IsReg && ((BaseReg)this).RegisterIndex == regIndex;
        }

        public bool IsRegTypeMem(RegType regType)
        {
            return IsMem || IsRegType(regType);
        }

        public abstract override string ToString();

        private sealed class NoneOperand : Operand
        {
            public override OperandType OperandType
            {
                get
                {
                    return OperandType.None;
                }
            }

            public override string ToString()
            {
                return "none";
            }
        }
    }
}
