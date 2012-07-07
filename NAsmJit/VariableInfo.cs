namespace NAsmJit
{
    using System;
    using System.Collections.Generic;

    /// <summary>
    /// X86 variable information.
    /// </summary>
    public class VariableInfo
    {
        private readonly RegType _registerType;
        private readonly byte _size;
        private readonly VariableClass _class;
        private readonly VariableFlags _flags;
        private readonly string _name;

        private VariableInfo(RegType registerType, int size, VariableClass @class, VariableFlags flags, string name)
        {
            this._registerType = registerType;
            this._size = (byte)size;
            this._class = @class;
            this._flags = flags;
            this._name = name;
        }

        public static VariableType NativeVariableType
        {
            get
            {
                if (Util.IsX86)
                    return VariableType.GPD;
                else if (Util.IsX64)
                    return VariableType.GPQ;
                else
                    throw new NotImplementedException();
            }
        }

        public static VariableType FloatVariableType
        {
            get
            {
                if (Util.IsX86)
                    return VariableType.X87_1F;
                else if (Util.IsX64)
                    return VariableType.XMM_1F;
                else
                    throw new NotImplementedException();
            }
        }

        public static VariableType DoubleVariableType
        {
            get
            {
                if (Util.IsX86)
                    return VariableType.X87_1D;
                else if (Util.IsX64)
                    return VariableType.XMM_1D;
                else
                    throw new NotImplementedException();
            }
        }

        public RegType RegisterType
        {
            get
            {
                return _registerType;
            }
        }

        /// <summary>
        /// Get register size in bytes.
        /// </summary>
        public int Size
        {
            get
            {
                return _size;
            }
        }

        /// <summary>
        /// Get variable class, see @ref kX86VarClass.
        /// </summary>
        public VariableClass Class
        {
            get
            {
                return _class;
            }
        }

        /// <summary>
        /// Get variable flags, see @ref kX86VarFlags.
        /// </summary>
        public VariableFlags Flags
        {
            get
            {
                return _flags;
            }
        }

        /// <summary>
        /// Get variable type name.
        /// </summary>
        public string Name
        {
            get
            {
                return _name;
            }
        }

        public static bool IsVariableFloat(VariableType variableType)
        {
            return (variableInfo[variableType].Flags & (VariableFlags.DoublePrecision | VariableFlags.SinglePrecision)) != 0;
        }

        public static bool IsVariableInteger(VariableType variableType)
        {
            return (variableInfo[variableType].Class & VariableClass.GP) != 0;
        }

        public static VariableInfo GetVariableInfo(VariableType variableType)
        {
            return variableInfo[variableType];
        }

        public static int GetVariableSize(VariableType variableType)
        {
            return variableInfo[variableType].Size;
        }

        public static VariableClass GetVariableClass(VariableType variableType)
        {
            return variableInfo[variableType].Class;
        }

        public static int GetVariableRegisterCode(VariableType variableType, RegIndex regIndex)
        {
            return (int)variableInfo[variableType].RegisterType | (int)regIndex;
        }

        private static readonly Dictionary<VariableType, VariableInfo> variableInfo =
            new Dictionary<VariableType, VariableInfo>()
            {
                { VariableType.GPD, new VariableInfo(RegType.GPD, 4, VariableClass.GP, 0, "Gpd") },
                { VariableType.GPQ, new VariableInfo(RegType.GPQ, 8, VariableClass.GP, 0, "Gpq") },
                { VariableType.X87, new VariableInfo(RegType.X87, 4, VariableClass.X87, VariableFlags.SinglePrecision, "X87") },
                { VariableType.X87_1F, new VariableInfo(RegType.X87, 4, VariableClass.X87, VariableFlags.SinglePrecision, "X87.SS") },
                { VariableType.X87_1D, new VariableInfo(RegType.X87, 8, VariableClass.X87, VariableFlags.DoublePrecision, "X87.SD") },
                { VariableType.MM, new VariableInfo(RegType.MM, 8, VariableClass.MM, VariableFlags.Packed, "Mm") },
                { VariableType.XMM, new VariableInfo(RegType.XMM, 16, VariableClass.XMM, 0, "Xmm") },
                { VariableType.XMM_1F, new VariableInfo(RegType.XMM, 4, VariableClass.XMM, VariableFlags.SinglePrecision, "Xmm.SS") },
                { VariableType.XMM_1D, new VariableInfo(RegType.XMM, 8, VariableClass.XMM, VariableFlags.DoublePrecision, "Xmm.SD") },
                { VariableType.XMM_4F, new VariableInfo(RegType.XMM, 16, VariableClass.XMM, VariableFlags.SinglePrecision | VariableFlags.Packed, "Xmm.PS") },
                { VariableType.XMM_2D, new VariableInfo(RegType.XMM, 16, VariableClass.XMM, VariableFlags.DoublePrecision | VariableFlags.Packed, "Xmm.PD") },
            };
    }
}
