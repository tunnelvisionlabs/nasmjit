using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AsmJitNet2
{
    public sealed class VarData
    {
        private string _name;



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
            get;
            set;
        }

        public string Name
        {
            get;
            set;
        }

        public int Size
        {
            get;
            set;
        }

        public VariableType Type
        {
            get;
            set;
        }

        public int Id
        {
            get;
            set;
        }

        public RegIndex PreferredRegisterIndex
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

        public Emittable FirstEmittable
        {
            get;
            set;
        }

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
    }
}
