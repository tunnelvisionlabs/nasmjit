namespace AsmJitNet
{
    using System.Collections.Generic;

    public class VariableInfo
    {
        private RegType _code;
        private byte _size;
        private VariableClass _class;
        private VariableFlags _flags;
        private string _name;

        private VariableInfo(RegType code, int size, VariableClass @class, VariableFlags flags, string name)
        {
            this._code = code;
            this._size = (byte)size;
            this._class = @class;
            this._flags = flags;
            this._name = name;
        }

        public RegType Code
        {
            get
            {
                return _code;
            }
        }

        public int Size
        {
            get
            {
                return _size;
            }
        }

        public VariableClass Class
        {
            get
            {
                return _class;
            }
        }

        public VariableFlags Flags
        {
            get
            {
                return _flags;
            }
        }

        public string Name
        {
            get
            {
                return _name;
            }
        }

        public static bool IsVariableFloat(VariableType variableType)
        {
            return (variableInfo[variableType].Flags & (VariableFlags.DpFp | VariableFlags.SpFp)) != 0;
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
            return (int)variableInfo[variableType].Code | (int)regIndex;
        }

        private static readonly Dictionary<VariableType, VariableInfo> variableInfo =
            new Dictionary<VariableType, VariableInfo>()
            {
                { VariableType.GPD, new VariableInfo(RegType.GPD, 4, VariableClass.GP, 0, "GP.D") },
                { VariableType.GPQ, new VariableInfo(RegType.GPQ, 8, VariableClass.GP, 0, "GP.Q") },
                { VariableType.X87, new VariableInfo(RegType.X87, 4, VariableClass.X87, VariableFlags.SpFp, "X87") },
                { VariableType.X87_1F, new VariableInfo(RegType.X87, 4, VariableClass.X87, VariableFlags.SpFp, "X87.1F") },
                { VariableType.X87_1D, new VariableInfo(RegType.X87, 8, VariableClass.X87, VariableFlags.DpFp, "X87.1D") },
                { VariableType.MM, new VariableInfo(RegType.MM, 8, VariableClass.MM, VariableFlags.Vector, "MM") },
                { VariableType.XMM, new VariableInfo(RegType.XMM, 16, VariableClass.XMM, 0, "XMM") },
                { VariableType.XMM_1F, new VariableInfo(RegType.XMM, 4, VariableClass.XMM, VariableFlags.SpFp, "XMM.1F") },
                { VariableType.XMM_1D, new VariableInfo(RegType.XMM, 8, VariableClass.XMM, VariableFlags.DpFp, "XMM.1D") },
                { VariableType.XMM_4F, new VariableInfo(RegType.XMM, 16, VariableClass.XMM, VariableFlags.SpFp | VariableFlags.Vector, "XMM.4F") },
                { VariableType.XMM_2D, new VariableInfo(RegType.XMM, 16, VariableClass.XMM, VariableFlags.DpFp | VariableFlags.Vector, "XMM.2D") },
            };
    }
}
