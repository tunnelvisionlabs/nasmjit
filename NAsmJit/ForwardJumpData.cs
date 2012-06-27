namespace AsmJitNet
{
    public sealed class ForwardJumpData
    {
        public Jmp Inst
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
