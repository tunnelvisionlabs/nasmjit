namespace AsmJitNet
{
    using System;
    using System.Diagnostics.Contracts;

    public sealed class VarData
    {
        private readonly Function _scope;
        private readonly int _id;
        private readonly VariableType _type;
        private readonly int _size;
        private readonly string _name;

        public VarData(Function scope, int id, VariableType type, int size, string name)
        {
            if (scope == null)
                throw new ArgumentNullException("scope");
            Contract.EndContractBlock();

            _scope = scope;
            _id = id;
            _type = type;
            _size = size;
            _name = name;

            HomeRegisterIndex = RegIndex.Invalid;
            PreferredRegisterMask = RegisterMask.All;
            RegisterIndex = RegIndex.Invalid;
            WorkOffset = Operand.InvalidValue;
            Priority = 10;
            State = VariableState.Unused;
        }

        public bool IsRegArgument
        {
            get;
            set;
        }

        public RegIndex RegisterIndex
        {
            get;
            set;
        }

        public RegIndex HomeRegisterIndex
        {
            get;
            set;
        }

        public bool IsMemArgument
        {
            get;
            set;
        }

        public int HomeMemoryOffset
        {
            get;
            set;
        }

        public Function Scope
        {
            get
            {
                Contract.Ensures(Contract.Result<Function>() != null);

                return _scope;
            }
        }

        public string Name
        {
            get
            {
                return _name;
            }
        }

        public int Size
        {
            get
            {
                return _size;
            }
        }

        public VariableType Type
        {
            get
            {
                return _type;
            }
        }

        public int Id
        {
            get
            {
                return _id;
            }
        }

        public RegisterMask PreferredRegisterMask
        {
            get;
            set;
        }

        public int WorkOffset
        {
            get;
            set;
        }

        public int Priority
        {
            get;
            set;
        }

        public bool Calculated
        {
            get;
            set;
        }

        public bool Changed
        {
            get;
            set;
        }

        public bool SaveOnUnuse
        {
            get;
            set;
        }

        public int RegisterReadCount
        {
            get;
            set;
        }

        public int RegisterWriteCount
        {
            get;
            set;
        }

        public int RegisterRWCount
        {
            get;
            set;
        }

        public int RegisterGPBLoCount
        {
            get;
            set;
        }

        public int RegisterGPBHiCount
        {
            get;
            set;
        }

        public int MemoryReadCount
        {
            get;
            set;
        }

        public int MemoryWriteCount
        {
            get;
            set;
        }

        public int MemoryRWCount
        {
            get;
            set;
        }

        public object Temp
        {
            get;
            set;
        }

        public VariableState State
        {
            get;
            set;
        }

        /// <summary>
        /// The first emittable where the variable is accessed.
        /// </summary>
        /// <remarks>
        /// If this member is @c NULL then variable is unused.
        /// </remarks>
        public Emittable FirstEmittable
        {
            get;
            set;
        }

        /// <summary>
        /// The first callable (ECall) which is after the @c FirstEmittable.
        /// </summary>
        public Emittable FirstCallable
        {
            get;
            set;
        }

        /// <summary>
        /// The last emittable where the variable is accessed.
        /// </summary>
        public Emittable LastEmittable
        {
            get;
            set;
        }

        public VarData NextActive
        {
            get;
            set;
        }

        public VarData PreviousActive
        {
            get;
            set;
        }

        public VarMemBlock HomeMemoryData
        {
            get;
            set;
        }

        [ContractInvariantMethod]
        private void ObjectInvariants()
        {
            Contract.Invariant(_scope != null);
        }
    }
}
