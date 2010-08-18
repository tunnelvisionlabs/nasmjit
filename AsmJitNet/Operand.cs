﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AsmJitNet2
{
    public abstract class Operand
    {
        public static readonly Operand[] EmptyOperands = new Operand[0];
        public static readonly Operand None = new NoneOperand();

        internal const int InvalidValue = -1;
        internal const int OperandIdValueMask = unchecked((int)0x3FFFFFFF);
        internal const int OperandIdTypeMask = unchecked((int)0xC0000000);
        internal const int OperandIdTypeLabel = unchecked((int)0x40000000);
        internal const int OperandIdTypeVar = unchecked((int)0x80000000);

        private byte _size;
        private int _id;

        protected Operand()
        {
            _id = InvalidValue;
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

            protected internal set
            {
                _id = value;
            }
        }

        public byte Size
        {
            get
            {
                return _size;
            }

            protected internal set
            {
                _size = value;
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

        public bool IsRegType(RegType regType)
        {
            return IsReg && ((BaseReg)this).RegisterType == regType;
        }

        public bool IsRegCode(RegCode code)
        {
            return IsReg && ((BaseReg)this).Code == (int)code;
        }

        public bool IsRegIndex(RegIndex regIndex)
        {
            return IsReg && ((BaseReg)this).RegisterIndex == regIndex;
        }

        public bool IsRegTypeMem(RegType regType)
        {
            return IsMem || IsRegType(regType);
        }

        private sealed class NoneOperand : Operand
        {
            public override OperandType OperandType
            {
                get
                {
                    return OperandType.None;
                }
            }
        }
    }
}
