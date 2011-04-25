namespace AsmJitNet
{
    using System;
    using System.Collections.Generic;
    using Array = System.Array;
    using System.Diagnostics.Contracts;

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
            Contract.Assume(_registers.Length == other._registers.Length);

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
                Contract.Ensures(Contract.Result<IList<VarData>>() != null);

                return _registers;
            }
        }

        public IList<VarData> GP
        {
            get
            {
                Contract.Ensures(Contract.Result<IList<VarData>>() != null);

                return new ArraySegment<VarData>(_registers, 0, 16);
            }
        }

        public IList<VarData> MM
        {
            get
            {
                Contract.Ensures(Contract.Result<IList<VarData>>() != null);

                return new ArraySegment<VarData>(_registers, 16, 8);
            }
        }

        public IList<VarData> XMM
        {
            get
            {
                Contract.Ensures(Contract.Result<IList<VarData>>() != null);

                return new ArraySegment<VarData>(_registers, 24, 16);
            }
        }

        public RegisterMask UsedGP
        {
            get;
            set;
        }

        public RegisterMask UsedMM
        {
            get;
            set;
        }

        public RegisterMask UsedXMM
        {
            get;
            set;
        }

        public RegisterMask ChangedGP
        {
            get;
            set;
        }

        public RegisterMask ChangedMM
        {
            get;
            set;
        }

        public RegisterMask ChangedXMM
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
            UsedGP = RegisterMask.Zero;
            UsedMM = RegisterMask.Zero;
            UsedXMM = RegisterMask.Zero;
            ChangedGP = RegisterMask.Zero;
            ChangedMM = RegisterMask.Zero;
            ChangedXMM = RegisterMask.Zero;
        }

        internal void CopyFrom(StateData other)
        {
            Contract.Requires(other != null);

            Array.Copy(other._registers, _registers, _registers.Length);
            UsedGP = other.UsedGP;
            UsedMM = other.UsedMM;
            UsedXMM = other.UsedXMM;
            ChangedGP = other.ChangedGP;
            ChangedMM = other.ChangedMM;
            ChangedXMM = other.ChangedXMM;
        }

        [ContractInvariantMethod]
        private void ObjectInvariants()
        {
            Contract.Invariant(_registers != null);
            Contract.Invariant(_registers.Length == RegisterCount);
        }
    }
}
