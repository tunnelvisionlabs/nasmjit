﻿namespace AsmJitNet
{
    using System;
    using System.Collections.Generic;
    using Array = System.Array;

    public class StateData
    {
        public const int RegisterCount = 16 + 8 + 16;

        private readonly VarData[] _registers;

        public StateData(int memVarsCount)
        {
            if (memVarsCount < 0)
                throw new ArgumentOutOfRangeException("memVarsCount");

            _registers = new VarData[RegisterCount];
            MemVarsData = new VarData[memVarsCount];
        }

        public StateData(StateData other, int memVarsCount)
        {
            if (other == null)
                throw new ArgumentNullException("other");
            if (memVarsCount < 0)
                throw new ArgumentOutOfRangeException("memVarsCount");

            _registers = (VarData[])other._registers.Clone();
            UsedGP = other.UsedGP;
            UsedMM = other.UsedMM;
            UsedXMM = other.UsedXMM;
            ChangedGP = other.ChangedGP;
            ChangedMM = other.ChangedMM;
            ChangedXMM = other.ChangedXMM;
            MemVarsData = new VarData[memVarsCount];
        }

        public IList<VarData> Registers
        {
            get
            {
                return _registers;
            }
        }

        public IList<VarData> GP
        {
            get
            {
                return new ArraySegment<VarData>(_registers, 0, 16);
            }
        }

        public IList<VarData> MM
        {
            get
            {
                return new ArraySegment<VarData>(_registers, 16, 8);
            }
        }

        public IList<VarData> XMM
        {
            get
            {
                return new ArraySegment<VarData>(_registers, 24, 16);
            }
        }

        public int UsedGP
        {
            get;
            set;
        }

        public int UsedMM
        {
            get;
            set;
        }

        public int UsedXMM
        {
            get;
            set;
        }

        public int ChangedGP
        {
            get;
            set;
        }

        public int ChangedMM
        {
            get;
            set;
        }

        public int ChangedXMM
        {
            get;
            set;
        }

        public VarData[] MemVarsData
        {
            get;
            set;
        }

        internal void Clear()
        {
            UsedGP = 0;
            UsedMM = 0;
            UsedXMM = 0;
            ChangedGP = 0;
            ChangedMM = 0;
            ChangedXMM = 0;
        }

        internal void CopyFrom(StateData other)
        {
            Array.Copy(other._registers, _registers, _registers.Length);
            UsedGP = other.UsedGP;
            UsedMM = other.UsedMM;
            UsedXMM = other.UsedXMM;
            ChangedGP = other.ChangedGP;
            ChangedMM = other.ChangedMM;
            ChangedXMM = other.ChangedXMM;
        }
    }
}
