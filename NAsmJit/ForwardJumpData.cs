namespace NAsmJit
{
    public sealed class ForwardJumpData
    {
        public CompilerJmpInstruction Inst
        {
            get;
            set;
        }

        public StateData State
        {
            get;
            set;
        }

        public ForwardJumpData Next
        {
            get;
            set;
        }
    }
}
