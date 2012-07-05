namespace NAsmJit
{
    using System;
    using System.Collections.ObjectModel;
    using System.Diagnostics.Contracts;

    public sealed class InstructionDescription
    {
        public const InstructionCode JumpBegin = InstructionCode.J;
        public const InstructionCode JumpEnd = InstructionCode.Jmp;

        private static readonly InstructionDescription[] instructionDescriptions;

        /// <summary>
        /// Instruction code
        /// </summary>
        private readonly InstructionCode _code;
        private readonly string _name;
        private readonly byte _group;
        private readonly byte _flags;
        private readonly ReadOnlyCollection<OperandFlags> _operandFlags;
        private readonly short _opcodeR;
        private readonly int _opcode0;
        private readonly int _opcode1;

        #region Array initializer
        static InstructionDescription()
        {
instructionDescriptions =
    new InstructionDescription[]
{
  // Instruction code (enum)      | instruction name   | instruction group| instruction flags| oflags[0]           | oflags[1]           | r| opCode[0] | opcode[1]
  MAKE_INST(InstructionCode.Adc              , "adc"              , InstructionGroup.ALU           , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GQDWB_MEM|NAsmJit.OperandFlags.IMM , 2, 0x00000010, 0x00000080),
  MAKE_INST(InstructionCode.Add              , "add"              , InstructionGroup.ALU           , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GQDWB_MEM|NAsmJit.OperandFlags.IMM , 0, 0x00000000, 0x00000080),
  MAKE_INST(InstructionCode.Addpd            , "addpd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F58, 0),
  MAKE_INST(InstructionCode.Addps            , "addps"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F58, 0),
  MAKE_INST(InstructionCode.Addsd            , "addsd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000F58, 0),
  MAKE_INST(InstructionCode.Addss            , "addss"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F58, 0),
  MAKE_INST(InstructionCode.Addsubpd         , "addsubpd"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000FD0, 0),
  MAKE_INST(InstructionCode.Addsubps         , "addsubps"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000FD0, 0),
  MAKE_INST(InstructionCode.AmdPrefetch      , "amd_prefetch"     , InstructionGroup.M             , InstructionFlags.NONE          , NAsmJit.OperandFlags.MEM              , 0                   , 0, 0x00000F0D, 0),
  MAKE_INST(InstructionCode.AmdPrefetchw     , "amd_prefetchw"    , InstructionGroup.M             , InstructionFlags.NONE          , NAsmJit.OperandFlags.MEM              , 0                   , 1, 0x00000F0D, 0),
  MAKE_INST(InstructionCode.And              , "and"              , InstructionGroup.ALU           , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GQDWB_MEM|NAsmJit.OperandFlags.IMM , 4, 0x00000020, 0x00000080),
  MAKE_INST(InstructionCode.Andnpd           , "andnpd"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F55, 0),
  MAKE_INST(InstructionCode.Andnps           , "andnps"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F55, 0),
  MAKE_INST(InstructionCode.Andpd            , "andpd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F54, 0),
  MAKE_INST(InstructionCode.Andps            , "andps"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F54, 0),
  MAKE_INST(InstructionCode.Blendpd          , "blendpd"          , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x660F3A0D, 0),
  MAKE_INST(InstructionCode.Blendps          , "blendps"          , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x660F3A0C, 0),
  MAKE_INST(InstructionCode.Blendvpd         , "blendvpd"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x660F3815, 0),
  MAKE_INST(InstructionCode.Blendvps         , "blendvps"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x660F3814, 0),
  MAKE_INST(InstructionCode.Bsf              , "bsf"              , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000FBC, 0),
  MAKE_INST(InstructionCode.Bsr              , "bsr"              , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000FBD, 0),
  MAKE_INST(InstructionCode.Bswap            , "bswap"            , InstructionGroup.BSWAP         , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQD              , 0                   , 0, 0         , 0),
  MAKE_INST(InstructionCode.Bt               , "bt"               , InstructionGroup.BT            , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW|NAsmJit.OperandFlags.MEM      , NAsmJit.OperandFlags.GQDW|NAsmJit.OperandFlags.IMM      , 4, 0x00000FA3, 0x00000FBA),
  MAKE_INST(InstructionCode.Btc              , "btc"              , InstructionGroup.BT            , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDW|NAsmJit.OperandFlags.MEM      , NAsmJit.OperandFlags.GQDW|NAsmJit.OperandFlags.IMM      , 7, 0x00000FBB, 0x00000FBA),
  MAKE_INST(InstructionCode.Btr              , "btr"              , InstructionGroup.BT            , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDW|NAsmJit.OperandFlags.MEM      , NAsmJit.OperandFlags.GQDW|NAsmJit.OperandFlags.IMM      , 6, 0x00000FB3, 0x00000FBA),
  MAKE_INST(InstructionCode.Bts              , "bts"              , InstructionGroup.BT            , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDW|NAsmJit.OperandFlags.MEM      , NAsmJit.OperandFlags.GQDW|NAsmJit.OperandFlags.IMM      , 5, 0x00000FAB, 0x00000FBA),
  MAKE_INST(InstructionCode.Call             , "call"             , InstructionGroup.CALL          , InstructionFlags.JUMP          , NAsmJit.OperandFlags.GQD|NAsmJit.OperandFlags.MEM       , 0                   , 0, 0         , 0),
  MAKE_INST(InstructionCode.Cbw              , "cbw"              , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x66000098, 0),
  MAKE_INST(InstructionCode.Cdq              , "cdq"              , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x48000099, 0),
  MAKE_INST(InstructionCode.Cdqe             , "cdqe"             , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x48000098, 0),
  MAKE_INST(InstructionCode.Clc              , "clc"              , InstructionGroup.EMIT          , InstructionFlags.NONE          , 0                   , 0                   , 0, 0x000000F8, 0),
  MAKE_INST(InstructionCode.Cld              , "cld"              , InstructionGroup.EMIT          , InstructionFlags.NONE          , 0                   , 0                   , 0, 0x000000FC, 0),
  MAKE_INST(InstructionCode.Clflush          , "clflush"          , InstructionGroup.M             , InstructionFlags.NONE          , NAsmJit.OperandFlags.MEM              , 0                   , 7, 0x00000FAE, 0),
  MAKE_INST(InstructionCode.Cmc              , "cmc"              , InstructionGroup.EMIT          , InstructionFlags.NONE          , 0                   , 0                   , 0, 0x000000F5, 0),
  MAKE_INST(InstructionCode.Cmova            , "cmova"            , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F47, 0),
  MAKE_INST(InstructionCode.Cmovae           , "cmovae"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F43, 0),
  MAKE_INST(InstructionCode.Cmovb            , "cmovb"            , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F42, 0),
  MAKE_INST(InstructionCode.Cmovbe           , "cmovbe"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F46, 0),
  MAKE_INST(InstructionCode.Cmovc            , "cmovc"            , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F42, 0),
  MAKE_INST(InstructionCode.Cmove            , "cmove"            , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F44, 0),
  MAKE_INST(InstructionCode.Cmovg            , "cmovg"            , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F4F, 0),
  MAKE_INST(InstructionCode.Cmovge           , "cmovge"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F4D, 0),
  MAKE_INST(InstructionCode.Cmovl            , "cmovl"            , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F4C, 0),
  MAKE_INST(InstructionCode.Cmovle           , "cmovle"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F4E, 0),
  MAKE_INST(InstructionCode.Cmovna           , "cmovna"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F46, 0),
  MAKE_INST(InstructionCode.Cmovnae          , "cmovnae"          , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F42, 0),
  MAKE_INST(InstructionCode.Cmovnb           , "cmovnb"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F43, 0),
  MAKE_INST(InstructionCode.Cmovnbe          , "cmovnbe"          , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F47, 0),
  MAKE_INST(InstructionCode.Cmovnc           , "cmovnc"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F43, 0),
  MAKE_INST(InstructionCode.Cmovne           , "cmovne"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F45, 0),
  MAKE_INST(InstructionCode.Cmovng           , "cmovng"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F4E, 0),
  MAKE_INST(InstructionCode.Cmovnge          , "cmovnge"          , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F4C, 0),
  MAKE_INST(InstructionCode.Cmovnl           , "cmovnl"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F4D, 0),
  MAKE_INST(InstructionCode.Cmovnle          , "cmovnle"          , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F4F, 0),
  MAKE_INST(InstructionCode.Cmovno           , "cmovno"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F41, 0),
  MAKE_INST(InstructionCode.Cmovnp           , "cmovnp"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F4B, 0),
  MAKE_INST(InstructionCode.Cmovns           , "cmovns"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F49, 0),
  MAKE_INST(InstructionCode.Cmovnz           , "cmovnz"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F45, 0),
  MAKE_INST(InstructionCode.Cmovo            , "cmovo"            , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F40, 0),
  MAKE_INST(InstructionCode.Cmovp            , "cmovp"            , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F4A, 0),
  MAKE_INST(InstructionCode.Cmovpe           , "cmovpe"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F4A, 0),
  MAKE_INST(InstructionCode.Cmovpo           , "cmovpo"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F4B, 0),
  MAKE_INST(InstructionCode.Cmovs            , "cmovs"            , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F48, 0),
  MAKE_INST(InstructionCode.Cmovz            , "cmovz"            , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0x00000F44, 0),
  MAKE_INST(InstructionCode.Cmp              , "cmp"              , InstructionGroup.ALU           , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GQDWB_MEM|NAsmJit.OperandFlags.IMM , 7, 0x00000038, 0x00000080),
  MAKE_INST(InstructionCode.Cmppd            , "cmppd"            , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000FC2, 0),
  MAKE_INST(InstructionCode.Cmpps            , "cmpps"            , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000FC2, 0),
  MAKE_INST(InstructionCode.Cmpsd            , "cmpsd"            , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000FC2, 0),
  MAKE_INST(InstructionCode.Cmpss            , "cmpss"            , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000FC2, 0),
  MAKE_INST(InstructionCode.Cmpxchg          , "cmpxchg"          , InstructionGroup.RM_R          , InstructionFlags.SPECIAL | InstructionFlags.LOCKABLE       , 0                   , 0                   , 0, 0x00000FB0, 0),
  MAKE_INST(InstructionCode.Cmpxchg16b       , "cmpxchg16b"       , InstructionGroup.M             , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , 0                   , 1, 0x00000FC7, 1 /* RexW */),
  MAKE_INST(InstructionCode.Cmpxchg8b        , "cmpxchg8b"        , InstructionGroup.M             , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , 0                   , 1, 0x00000FC7, 0),
  MAKE_INST(InstructionCode.Comisd           , "comisd"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F2F, 0),
  MAKE_INST(InstructionCode.Comiss           , "comiss"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F2F, 0),
  MAKE_INST(InstructionCode.Cpuid            , "cpuid"            , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x00000FA2, 0),
  MAKE_INST(InstructionCode.Cqo              , "cqo"              , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x48000099, 0),
  MAKE_INST(InstructionCode.Crc32            , "crc32"            , InstructionGroup.CRC32         , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQD              , NAsmJit.OperandFlags.GQDWB_MEM        , 0, 0xF20F38F0, 0),
  MAKE_INST(InstructionCode.Cvtdq2pd         , "cvtdq2pd"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000FE6, 0),
  MAKE_INST(InstructionCode.Cvtdq2ps         , "cvtdq2ps"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F5B, 0),
  MAKE_INST(InstructionCode.Cvtpd2dq         , "cvtpd2dq"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000FE6, 0),
  MAKE_INST(InstructionCode.Cvtpd2pi         , "cvtpd2pi"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F2D, 0),
  MAKE_INST(InstructionCode.Cvtpd2ps         , "cvtpd2ps"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F5A, 0),
  MAKE_INST(InstructionCode.Cvtpi2pd         , "cvtpi2pd"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.MM_MEM           , 0, 0x66000F2A, 0),
  MAKE_INST(InstructionCode.Cvtpi2ps         , "cvtpi2ps"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F2A, 0),
  MAKE_INST(InstructionCode.Cvtps2dq         , "cvtps2dq"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F5B, 0),
  MAKE_INST(InstructionCode.Cvtps2pd         , "cvtps2pd"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F5A, 0),
  MAKE_INST(InstructionCode.Cvtps2pi         , "cvtps2pi"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F2D, 0),
  MAKE_INST(InstructionCode.Cvtsd2si         , "cvtsd2si"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.GQD              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000F2D, 0),
  MAKE_INST(InstructionCode.Cvtsd2ss         , "cvtsd2ss"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000F5A, 0),
  MAKE_INST(InstructionCode.Cvtsi2sd         , "cvtsi2sd"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.GQD|NAsmJit.OperandFlags.MEM       , 0, 0xF2000F2A, 0),
  MAKE_INST(InstructionCode.Cvtsi2ss         , "cvtsi2ss"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.GQD|NAsmJit.OperandFlags.MEM       , 0, 0xF3000F2A, 0),
  MAKE_INST(InstructionCode.Cvtss2sd         , "cvtss2sd"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F5A, 0),
  MAKE_INST(InstructionCode.Cvtss2si         , "cvtss2si"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQD              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F2D, 0),
  MAKE_INST(InstructionCode.Cvttpd2dq        , "cvttpd2dq"        , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000FE6, 0),
  MAKE_INST(InstructionCode.Cvttpd2pi        , "cvttpd2pi"        , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F2C, 0),
  MAKE_INST(InstructionCode.Cvttps2dq        , "cvttps2dq"        , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F5B, 0),
  MAKE_INST(InstructionCode.Cvttps2pi        , "cvttps2pi"        , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F2C, 0),
  MAKE_INST(InstructionCode.Cvttsd2si        , "cvttsd2si"        , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQD              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000F2C, 0),
  MAKE_INST(InstructionCode.Cvttss2si        , "cvttss2si"        , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQD              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F2C, 0),
  MAKE_INST(InstructionCode.Cwd              , "cwd"              , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x00000099, 0),
  MAKE_INST(InstructionCode.Cwde             , "cwde"             , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x00000098, 0),
  MAKE_INST(InstructionCode.Daa              , "daa"              , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x00000027, 0),
  MAKE_INST(InstructionCode.Das              , "das"              , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x0000002F, 0),
  MAKE_INST(InstructionCode.Dec              , "dec"              , InstructionGroup.INC_DEC       , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDWB_MEM        , 0                   , 1, 0x00000048, 0x000000FE),
  MAKE_INST(InstructionCode.Div              , "div"              , InstructionGroup.RM            , InstructionFlags.SPECIAL       , 0                   , 0                   , 6, 0x000000F6, 0),
  MAKE_INST(InstructionCode.Divpd            , "divpd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F5E, 0),
  MAKE_INST(InstructionCode.Divps            , "divps"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F5E, 0),
  MAKE_INST(InstructionCode.Divsd            , "divsd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000F5E, 0),
  MAKE_INST(InstructionCode.Divss            , "divss"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F5E, 0),
  MAKE_INST(InstructionCode.Dppd             , "dppd"             , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x660F3A41, 0),
  MAKE_INST(InstructionCode.Dpps             , "dpps"             , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x660F3A40, 0),
  MAKE_INST(InstructionCode.Emms             , "emms"             , InstructionGroup.EMIT          , InstructionFlags.NONE          , 0                   , 0                   , 0, 0x00000F77, 0),
  MAKE_INST(InstructionCode.Enter            , "enter"            , InstructionGroup.ENTER         , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x000000C8, 0),
  MAKE_INST(InstructionCode.Extractps        , "extractps"        , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x660F3A17, 0),
  MAKE_INST(InstructionCode.F2xm1            , "f2xm1"            , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9F0, 0),
  MAKE_INST(InstructionCode.Fabs             , "fabs"             , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9E1, 0),
  MAKE_INST(InstructionCode.Fadd             , "fadd"             , InstructionGroup.X87_FPU       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0xD8DCC0C0, 0),
  MAKE_INST(InstructionCode.Faddp            , "faddp"            , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DEC0, 0),
  MAKE_INST(InstructionCode.Fbld             , "fbld"             , InstructionGroup.M             , InstructionFlags.FPU           , NAsmJit.OperandFlags.MEM              , 0                   , 4, 0x000000DF, 0),
  MAKE_INST(InstructionCode.Fbstp            , "fbstp"            , InstructionGroup.M             , InstructionFlags.FPU           , NAsmJit.OperandFlags.MEM              , 0                   , 6, 0x000000DF, 0),
  MAKE_INST(InstructionCode.Fchs             , "fchs"             , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9E0, 0),
  MAKE_INST(InstructionCode.Fclex            , "fclex"            , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x9B00DBE2, 0),
  MAKE_INST(InstructionCode.Fcmovb           , "fcmovb"           , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DAC0, 0),
  MAKE_INST(InstructionCode.Fcmovbe          , "fcmovbe"          , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DAD0, 0),
  MAKE_INST(InstructionCode.Fcmove           , "fcmove"           , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DAC8, 0),
  MAKE_INST(InstructionCode.Fcmovnb          , "fcmovnb"          , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DBC0, 0),
  MAKE_INST(InstructionCode.Fcmovnbe         , "fcmovnbe"         , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DBD0, 0),
  MAKE_INST(InstructionCode.Fcmovne          , "fcmovne"          , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DBC8, 0),
  MAKE_INST(InstructionCode.Fcmovnu          , "fcmovnu"          , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DBD8, 0),
  MAKE_INST(InstructionCode.Fcmovu           , "fcmovu"           , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DAD8, 0),
  MAKE_INST(InstructionCode.Fcom             , "fcom"             , InstructionGroup.X87_FPU       , InstructionFlags.FPU           , 0                   , 0                   , 2, 0xD8DCD0D0, 0),
  MAKE_INST(InstructionCode.Fcomi            , "fcomi"            , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DBF0, 0),
  MAKE_INST(InstructionCode.Fcomip           , "fcomip"           , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DFF0, 0),
  MAKE_INST(InstructionCode.Fcomp            , "fcomp"            , InstructionGroup.X87_FPU       , InstructionFlags.FPU           , 0                   , 0                   , 3, 0xD8DCD8D8, 0),
  MAKE_INST(InstructionCode.Fcompp           , "fcompp"           , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DED9, 0),
  MAKE_INST(InstructionCode.Fcos             , "fcos"             , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9FF, 0),
  MAKE_INST(InstructionCode.Fdecstp          , "fdecstp"          , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9F6, 0),
  MAKE_INST(InstructionCode.Fdiv             , "fdiv"             , InstructionGroup.X87_FPU       , InstructionFlags.FPU           , 0                   , 0                   , 6, 0xD8DCF0F8, 0),
  MAKE_INST(InstructionCode.Fdivp            , "fdivp"            , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DEF8, 0),
  MAKE_INST(InstructionCode.Fdivr            , "fdivr"            , InstructionGroup.X87_FPU       , InstructionFlags.FPU           , 0                   , 0                   , 7, 0xD8DCF8F0, 0),
  MAKE_INST(InstructionCode.Fdivrp           , "fdivrp"           , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DEF0, 0),
  MAKE_INST(InstructionCode.Femms            , "femms"            , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x00000F0E, 0),
  MAKE_INST(InstructionCode.Ffree            , "ffree"            , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DDC0, 0),
  MAKE_INST(InstructionCode.Fiadd            , "fiadd"            , InstructionGroup.X87_MEM       , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_2_4           , 0                   , 0, 0xDEDA0000, 0),
  MAKE_INST(InstructionCode.Ficom            , "ficom"            , InstructionGroup.X87_MEM       , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_2_4           , 0                   , 2, 0xDEDA0000, 0),
  MAKE_INST(InstructionCode.Ficomp           , "ficomp"           , InstructionGroup.X87_MEM       , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_2_4           , 0                   , 3, 0xDEDA0000, 0),
  MAKE_INST(InstructionCode.Fidiv            , "fidiv"            , InstructionGroup.X87_MEM       , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_2_4           , 0                   , 6, 0xDEDA0000, 0),
  MAKE_INST(InstructionCode.Fidivr           , "fidivr"           , InstructionGroup.X87_MEM       , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_2_4           , 0                   , 7, 0xDEDA0000, 0),
  MAKE_INST(InstructionCode.Fild             , "fild"             , InstructionGroup.X87_MEM       , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_2_4_8         , 0                   , 0, 0xDFDBDF05, 0),
  MAKE_INST(InstructionCode.Fimul            , "fimul"            , InstructionGroup.X87_MEM       , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_2_4           , 0                   , 1, 0xDEDA0000, 0),
  MAKE_INST(InstructionCode.Fincstp          , "fincstp"          , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9F7, 0),
  MAKE_INST(InstructionCode.Finit            , "finit"            , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x9B00DBE3, 0),
  MAKE_INST(InstructionCode.Fist             , "fist"             , InstructionGroup.X87_MEM       , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_2_4           , 0                   , 2, 0xDFDB0000, 0),
  MAKE_INST(InstructionCode.Fistp            , "fistp"            , InstructionGroup.X87_MEM       , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_2_4_8         , 0                   , 3, 0xDFDBDF07, 0),
  MAKE_INST(InstructionCode.Fisttp           , "fisttp"           , InstructionGroup.X87_MEM       , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_2_4_8         , 0                   , 1, 0xDFDBDD01, 0),
  MAKE_INST(InstructionCode.Fisub            , "fisub"            , InstructionGroup.X87_MEM       , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_2_4           , 0                   , 4, 0xDEDA0000, 0),
  MAKE_INST(InstructionCode.Fisubr           , "fisubr"           , InstructionGroup.X87_MEM       , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_2_4           , 0                   , 5, 0xDEDA0000, 0),
  MAKE_INST(InstructionCode.Fld              , "fld"              , InstructionGroup.X87_MEM_STI   , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_4_8_10        , 0                   , 0, 0x00D9DD00, 0xD9C0DB05),
  MAKE_INST(InstructionCode.Fld1             , "fld1"             , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9E8, 0),
  MAKE_INST(InstructionCode.Fldcw            , "fldcw"            , InstructionGroup.M             , InstructionFlags.FPU           , NAsmJit.OperandFlags.MEM              , 0                   , 5, 0x000000D9, 0),
  MAKE_INST(InstructionCode.Fldenv           , "fldenv"           , InstructionGroup.M             , InstructionFlags.FPU           , NAsmJit.OperandFlags.MEM              , 0                   , 4, 0x000000D9, 0),
  MAKE_INST(InstructionCode.Fldl2e           , "fldl2e"           , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9EA, 0),
  MAKE_INST(InstructionCode.Fldl2t           , "fldl2t"           , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9E9, 0),
  MAKE_INST(InstructionCode.Fldlg2           , "fldlg2"           , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9EC, 0),
  MAKE_INST(InstructionCode.Fldln2           , "fldln2"           , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9ED, 0),
  MAKE_INST(InstructionCode.Fldpi            , "fldpi"            , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9EB, 0),
  MAKE_INST(InstructionCode.Fldz             , "fldz"             , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9EE, 0),
  MAKE_INST(InstructionCode.Fmul             , "fmul"             , InstructionGroup.X87_FPU       , InstructionFlags.FPU           , 0                   , 0                   , 1, 0xD8DCC8C8, 0),
  MAKE_INST(InstructionCode.Fmulp            , "fmulp"            , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DEC8, 0),
  MAKE_INST(InstructionCode.Fnclex           , "fnclex"           , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DBE2, 0),
  MAKE_INST(InstructionCode.Fninit           , "fninit"           , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DBE3, 0),
  MAKE_INST(InstructionCode.Fnop             , "fnop"             , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9D0, 0),
  MAKE_INST(InstructionCode.Fnsave           , "fnsave"           , InstructionGroup.M             , InstructionFlags.FPU           , NAsmJit.OperandFlags.MEM              , 0                   , 6, 0x000000DD, 0),
  MAKE_INST(InstructionCode.Fnstcw           , "fnstcw"           , InstructionGroup.M             , InstructionFlags.FPU           , NAsmJit.OperandFlags.MEM              , 0                   , 7, 0x000000D9, 0),
  MAKE_INST(InstructionCode.Fnstenv          , "fnstenv"          , InstructionGroup.M             , InstructionFlags.FPU           , NAsmJit.OperandFlags.MEM              , 0                   , 6, 0x000000D9, 0),
  MAKE_INST(InstructionCode.Fnstsw           , "fnstsw"           , InstructionGroup.X87_FSTSW     , InstructionFlags.FPU           , NAsmJit.OperandFlags.MEM              , 0                   , 7, 0x000000DD, 0x0000DFE0),
  MAKE_INST(InstructionCode.Fpatan           , "fpatan"           , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9F3, 0),
  MAKE_INST(InstructionCode.Fprem            , "fprem"            , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9F8, 0),
  MAKE_INST(InstructionCode.Fprem1           , "fprem1"           , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9F5, 0),
  MAKE_INST(InstructionCode.Fptan            , "fptan"            , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9F2, 0),
  MAKE_INST(InstructionCode.Frndint          , "frndint"          , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9FC, 0),
  MAKE_INST(InstructionCode.Frstor           , "frstor"           , InstructionGroup.M             , InstructionFlags.FPU           , NAsmJit.OperandFlags.MEM              , 0                   , 4, 0x000000DD, 0),
  MAKE_INST(InstructionCode.Fsave            , "fsave"            , InstructionGroup.M             , InstructionFlags.FPU           , NAsmJit.OperandFlags.MEM              , 0                   , 6, 0x9B0000DD, 0),
  MAKE_INST(InstructionCode.Fscale           , "fscale"           , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9FD, 0),
  MAKE_INST(InstructionCode.Fsin             , "fsin"             , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9FE, 0),
  MAKE_INST(InstructionCode.Fsincos          , "fsincos"          , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9FB, 0),
  MAKE_INST(InstructionCode.Fsqrt            , "fsqrt"            , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9FA, 0),
  MAKE_INST(InstructionCode.Fst              , "fst"              , InstructionGroup.X87_MEM_STI   , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_4_8           , 0                   , 2, 0x00D9DD02, 0xDDD00000),
  MAKE_INST(InstructionCode.Fstcw            , "fstcw"            , InstructionGroup.M             , InstructionFlags.FPU           , NAsmJit.OperandFlags.MEM              , 0                   , 7, 0x9B0000D9, 0),
  MAKE_INST(InstructionCode.Fstenv           , "fstenv"           , InstructionGroup.M             , InstructionFlags.FPU           , NAsmJit.OperandFlags.MEM              , 0                   , 6, 0x9B0000D9, 0),
  MAKE_INST(InstructionCode.Fstp             , "fstp"             , InstructionGroup.X87_MEM_STI   , InstructionFlags.FPU           , NAsmJit.OperandFlags.FM_4_8_10        , 0                   , 3, 0x00D9DD03, 0xDDD8DB07),
  MAKE_INST(InstructionCode.Fstsw            , "fstsw"            , InstructionGroup.X87_FSTSW     , InstructionFlags.FPU           , NAsmJit.OperandFlags.MEM              , 0                   , 7, 0x9B0000DD, 0x9B00DFE0),
  MAKE_INST(InstructionCode.Fsub             , "fsub"             , InstructionGroup.X87_FPU       , InstructionFlags.FPU           , 0                   , 0                   , 4, 0xD8DCE0E8, 0),
  MAKE_INST(InstructionCode.Fsubp            , "fsubp"            , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DEE8, 0),
  MAKE_INST(InstructionCode.Fsubr            , "fsubr"            , InstructionGroup.X87_FPU       , InstructionFlags.FPU           , 0                   , 0                   , 5, 0xD8DCE8E0, 0),
  MAKE_INST(InstructionCode.Fsubrp           , "fsubrp"           , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DEE0, 0),
  MAKE_INST(InstructionCode.Ftst             , "ftst"             , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9E4, 0),
  MAKE_INST(InstructionCode.Fucom            , "fucom"            , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DDE0, 0),
  MAKE_INST(InstructionCode.Fucomi           , "fucomi"           , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DBE8, 0),
  MAKE_INST(InstructionCode.Fucomip          , "fucomip"          , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DFE8, 0),
  MAKE_INST(InstructionCode.Fucomp           , "fucomp"           , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DDE8, 0),
  MAKE_INST(InstructionCode.Fucompp          , "fucompp"          , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000DAE9, 0),
  MAKE_INST(InstructionCode.Fwait            , "fwait"            , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x000000DB, 0),
  MAKE_INST(InstructionCode.Fxam             , "fxam"             , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9E5, 0),
  MAKE_INST(InstructionCode.Fxch             , "fxch"             , InstructionGroup.X87_STI       , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9C8, 0),
  MAKE_INST(InstructionCode.Fxrstor          , "fxrstor"          , InstructionGroup.M             , InstructionFlags.FPU           , 0                   , 0                   , 1, 0x00000FAE, 0),
  MAKE_INST(InstructionCode.Fxsave           , "fxsave"           , InstructionGroup.M             , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x00000FAE, 0),
  MAKE_INST(InstructionCode.Fxtract          , "fxtract"          , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9F4, 0),
  MAKE_INST(InstructionCode.Fyl2x            , "fyl2x"            , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9F1, 0),
  MAKE_INST(InstructionCode.Fyl2xp1          , "fyl2xp1"          , InstructionGroup.EMIT          , InstructionFlags.FPU           , 0                   , 0                   , 0, 0x0000D9F9, 0),
  MAKE_INST(InstructionCode.Haddpd           , "haddpd"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F7C, 0),
  MAKE_INST(InstructionCode.Haddps           , "haddps"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000F7C, 0),
  MAKE_INST(InstructionCode.Hsubpd           , "hsubpd"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F7D, 0),
  MAKE_INST(InstructionCode.Hsubps           , "hsubps"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000F7D, 0),
  MAKE_INST(InstructionCode.Idiv             , "idiv"             , InstructionGroup.RM            , InstructionFlags.SPECIAL       , 0                   , 0                   , 7, 0x000000F6, 0),
  MAKE_INST(InstructionCode.Imul             , "imul"             , InstructionGroup.IMUL          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0         , 0),
  MAKE_INST(InstructionCode.Inc              , "inc"              , InstructionGroup.INC_DEC       , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDWB_MEM        , 0                   , 0, 0x00000040, 0x000000FE),
  MAKE_INST(InstructionCode.Int3             , "int3"             , InstructionGroup.EMIT          , InstructionFlags.NONE          , 0                   , 0                   , 0, 0x000000CC, 0),
  MAKE_INST(InstructionCode.Int              , "int"              , InstructionGroup.EMIT          , InstructionFlags.NONE          , NAsmJit.OperandFlags.IMM              , 0                   , 0, 0x000000CD, 0),
  MAKE_INST(InstructionCode.Into             , "into"             , InstructionGroup.EMIT          , InstructionFlags.NONE          , 0                   , 0                   , 0, 0x000000CE, 0),

  MAKE_INST(InstructionCode.Ja               , "ja"               , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x7       , 0),
  MAKE_INST(InstructionCode.Jae              , "jae"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x3       , 0),
  MAKE_INST(InstructionCode.Jb               , "jb"               , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x2       , 0),
  MAKE_INST(InstructionCode.Jbe              , "jbe"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x6       , 0),
  MAKE_INST(InstructionCode.Jc               , "jc"               , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x2       , 0),
  MAKE_INST(InstructionCode.Je               , "je"               , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x4       , 0),
  MAKE_INST(InstructionCode.Jg               , "jg"               , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0xF       , 0),
  MAKE_INST(InstructionCode.Jge              , "jge"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0xD       , 0),
  MAKE_INST(InstructionCode.Jl               , "jl"               , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0xC       , 0),
  MAKE_INST(InstructionCode.Jle              , "jle"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0xE       , 0),
  MAKE_INST(InstructionCode.Jna              , "jna"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x6       , 0),
  MAKE_INST(InstructionCode.Jnae             , "jnae"             , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x2       , 0),
  MAKE_INST(InstructionCode.Jnb              , "jnb"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x3       , 0),
  MAKE_INST(InstructionCode.Jnbe             , "jnbe"             , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x7       , 0),
  MAKE_INST(InstructionCode.Jnc              , "jnc"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x3       , 0),
  MAKE_INST(InstructionCode.Jne              , "jne"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x5       , 0),
  MAKE_INST(InstructionCode.Jng              , "jng"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0xE       , 0),
  MAKE_INST(InstructionCode.Jnge             , "jnge"             , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0xC       , 0),
  MAKE_INST(InstructionCode.Jnl              , "jnl"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0xD       , 0),
  MAKE_INST(InstructionCode.Jnle             , "jnle"             , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0xF       , 0),
  MAKE_INST(InstructionCode.Jno              , "jno"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x1       , 0),
  MAKE_INST(InstructionCode.Jnp              , "jnp"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0xB       , 0),
  MAKE_INST(InstructionCode.Jns              , "jns"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x9       , 0),
  MAKE_INST(InstructionCode.Jnz              , "jnz"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x5       , 0),
  MAKE_INST(InstructionCode.Jo               , "jo"               , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x0       , 0),
  MAKE_INST(InstructionCode.Jp               , "jp"               , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0xA       , 0),
  MAKE_INST(InstructionCode.Jpe              , "jpe"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0xA       , 0),
  MAKE_INST(InstructionCode.Jpo              , "jpo"              , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0xB       , 0),
  MAKE_INST(InstructionCode.Js               , "js"               , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x8       , 0),
  MAKE_INST(InstructionCode.Jz               , "jz"               , InstructionGroup.J             , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0x4       , 0),
  MAKE_INST(InstructionCode.Jmp              , "jmp"              , InstructionGroup.JMP           , InstructionFlags.JUMP          , 0                   , 0                   , 0, 0         , 0),

  MAKE_INST(InstructionCode.Lddqu            , "lddqu"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.MEM              , 0, 0xF2000FF0, 0),
  MAKE_INST(InstructionCode.Ldmxcsr          , "ldmxcsr"          , InstructionGroup.M             , InstructionFlags.NONE          , NAsmJit.OperandFlags.MEM              , 0                   , 2, 0x00000FAE, 0),
  MAKE_INST(InstructionCode.Lahf             , "lahf"             , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x0000009F, 0),
  MAKE_INST(InstructionCode.Lea              , "lea"              , InstructionGroup.LEA           , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQD              , NAsmJit.OperandFlags.MEM              , 0, 0         , 0),
  MAKE_INST(InstructionCode.Leave            , "leave"            , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x000000C9, 0),
  MAKE_INST(InstructionCode.Lfence           , "lfence"           , InstructionGroup.EMIT          , InstructionFlags.NONE          , 0                   , 0                   , 0, 0x000FAEE8, 0),
  MAKE_INST(InstructionCode.Maskmovdqu       , "maskmovdqu"       , InstructionGroup.MMU_RMI       , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM              , 0, 0x66000F57, 0),
  MAKE_INST(InstructionCode.Maskmovq         , "maskmovq"         , InstructionGroup.MMU_RMI       , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM               , 0, 0x00000FF7, 0),
  MAKE_INST(InstructionCode.Maxpd            , "maxpd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F5F, 0),
  MAKE_INST(InstructionCode.Maxps            , "maxps"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F5F, 0),
  MAKE_INST(InstructionCode.Maxsd            , "maxsd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000F5F, 0),
  MAKE_INST(InstructionCode.Maxss            , "maxss"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F5F, 0),
  MAKE_INST(InstructionCode.Mfence           , "mfence"           , InstructionGroup.EMIT          , InstructionFlags.NONE          , 0                   , 0                   , 0, 0x000FAEF0, 0),
  MAKE_INST(InstructionCode.Minpd            , "minpd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F5D, 0),
  MAKE_INST(InstructionCode.Minps            , "minps"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F5D, 0),
  MAKE_INST(InstructionCode.Minsd            , "minsd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000F5D, 0),
  MAKE_INST(InstructionCode.Minss            , "minss"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F5D, 0),
  MAKE_INST(InstructionCode.Monitor          , "monitor"          , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x000F01C8, 0),
  MAKE_INST(InstructionCode.Mov              , "mov"              , InstructionGroup.MOV           , InstructionFlags.MOV           , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GQDWB_MEM|NAsmJit.OperandFlags.IMM , 0, 0         , 0),
  MAKE_INST(InstructionCode.Movapd           , "movapd"           , InstructionGroup.MMU_MOV       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM_MEM          , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F28, 0x66000F29),
  MAKE_INST(InstructionCode.Movaps           , "movaps"           , InstructionGroup.MMU_MOV       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM_MEM          , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F28, 0x00000F29),
  MAKE_INST(InstructionCode.Movbe            , "movbe"            , InstructionGroup.MOVBE         , InstructionFlags.MOV           , NAsmJit.OperandFlags.GQDW|NAsmJit.OperandFlags.MEM      , NAsmJit.OperandFlags.GQDW|NAsmJit.OperandFlags.MEM      , 0, 0x000F38F0, 0x000F38F1),
  MAKE_INST(InstructionCode.Movd             , "movd"             , InstructionGroup.MMU_MOVD      , InstructionFlags.MOV           , NAsmJit.OperandFlags.GD|NAsmJit.OperandFlags.MM_XMM_MEM , NAsmJit.OperandFlags.GD|NAsmJit.OperandFlags.MM_XMM_MEM , 0, 0         , 0),
  MAKE_INST(InstructionCode.Movddup          , "movddup"          , InstructionGroup.MMU_MOV       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000F12, 0),
  MAKE_INST(InstructionCode.Movdq2q          , "movdq2q"          , InstructionGroup.MMU_MOV       , InstructionFlags.MOV           , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.XMM              , 0, 0xF2000FD6, 0),
  MAKE_INST(InstructionCode.Movdqa           , "movdqa"           , InstructionGroup.MMU_MOV       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM_MEM          , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F6F, 0x66000F7F),
  MAKE_INST(InstructionCode.Movdqu           , "movdqu"           , InstructionGroup.MMU_MOV       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM_MEM          , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F6F, 0xF3000F7F),
  MAKE_INST(InstructionCode.Movhlps          , "movhlps"          , InstructionGroup.MMU_MOV       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM              , 0, 0x00000F12, 0),
  MAKE_INST(InstructionCode.Movhpd           , "movhpd"           , InstructionGroup.MMU_MOV       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM_MEM          , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F16, 0x66000F17),
  MAKE_INST(InstructionCode.Movhps           , "movhps"           , InstructionGroup.MMU_MOV       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM_MEM          , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F16, 0x00000F17),
  MAKE_INST(InstructionCode.Movlhps          , "movlhps"          , InstructionGroup.MMU_MOV       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM              , 0, 0x00000F16, 0),
  MAKE_INST(InstructionCode.Movlpd           , "movlpd"           , InstructionGroup.MMU_MOV       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM_MEM          , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F12, 0x66000F13),
  MAKE_INST(InstructionCode.Movlps           , "movlps"           , InstructionGroup.MMU_MOV       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM_MEM          , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F12, 0x00000F13),
  MAKE_INST(InstructionCode.Movmskpd         , "movmskpd"         , InstructionGroup.MMU_MOV       , InstructionFlags.MOV           , NAsmJit.OperandFlags.GQD|NAsmJit.OperandFlags.NOREX     , NAsmJit.OperandFlags.XMM              , 0, 0x66000F50, 0),
  MAKE_INST(InstructionCode.Movmskps         , "movmskps"         , InstructionGroup.MMU_MOV       , InstructionFlags.MOV           , NAsmJit.OperandFlags.GQD|NAsmJit.OperandFlags.NOREX     , NAsmJit.OperandFlags.XMM              , 0, 0x00000F50, 0),
  MAKE_INST(InstructionCode.Movntdq          , "movntdq"          , InstructionGroup.MMU_MOV       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.XMM              , 0, 0         , 0x66000FE7),
  MAKE_INST(InstructionCode.Movntdqa         , "movntdqa"         , InstructionGroup.MMU_MOV       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.MEM              , 0, 0x660F382A, 0),
  MAKE_INST(InstructionCode.Movnti           , "movnti"           , InstructionGroup.MMU_MOV       , InstructionFlags.MOV           , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.GQD              , 0, 0         , 0x00000FC3),
  MAKE_INST(InstructionCode.Movntpd          , "movntpd"          , InstructionGroup.MMU_MOV       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.XMM              , 0, 0         , 0x66000F2B),
  MAKE_INST(InstructionCode.Movntps          , "movntps"          , InstructionGroup.MMU_MOV       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.XMM              , 0, 0         , 0x00000F2B),
  MAKE_INST(InstructionCode.Movntq           , "movntq"           , InstructionGroup.MMU_MOV       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MM               , 0, 0         , 0x00000FE7),
  MAKE_INST(InstructionCode.Movq             , "movq"             , InstructionGroup.MMU_MOVQ      , InstructionFlags.MOV           , NAsmJit.OperandFlags.GQ|NAsmJit.OperandFlags.MM_XMM_MEM , NAsmJit.OperandFlags.GQ|NAsmJit.OperandFlags.MM_XMM_MEM , 0, 0         , 0),
  MAKE_INST(InstructionCode.Movq2dq          , "movq2dq"          , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.MM               , 0, 0xF3000FD6, 0),
  MAKE_INST(InstructionCode.Movsd            , "movsd"            , InstructionGroup.MMU_MOV       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM_MEM          , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000F10, 0xF2000F11),
  MAKE_INST(InstructionCode.Movshdup         , "movshdup"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F16, 0),
  MAKE_INST(InstructionCode.Movsldup         , "movsldup"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F12, 0),
  MAKE_INST(InstructionCode.Movss            , "movss"            , InstructionGroup.MMU_MOV       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM_MEM          , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F10, 0xF3000F11),
  MAKE_INST(InstructionCode.Movsx            , "movsx"            , InstructionGroup.MOVSX_MOVZX   , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GWB_MEM          , 0, 0x00000FBE, 0),
  MAKE_INST(InstructionCode.Movsxd           , "movsxd"           , InstructionGroup.MOVSXD        , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQ               , NAsmJit.OperandFlags.GD_MEM           , 0, 0         , 0),
  MAKE_INST(InstructionCode.Movupd           , "movupd"           , InstructionGroup.MMU_MOV       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM_MEM          , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F10, 0x66000F11),
  MAKE_INST(InstructionCode.Movups           , "movups"           , InstructionGroup.MMU_MOV       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM_MEM          , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F10, 0x00000F11),
  MAKE_INST(InstructionCode.Movzx            , "movzx"            , InstructionGroup.MOVSX_MOVZX   , InstructionFlags.MOV           , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GWB_MEM          , 0, 0x00000FB6, 0),
  MAKE_INST(InstructionCode.MovPtr           , "mov_ptr"          , InstructionGroup.MOV_PTR       , InstructionFlags.MOV|InstructionFlags.SPECIAL, NAsmJit.OperandFlags.GQDWB            , NAsmJit.OperandFlags.IMM              , 0, 0         , 0),
  MAKE_INST(InstructionCode.Mpsadbw          , "mpsadbw"          , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x660F3A42, 0),
  MAKE_INST(InstructionCode.Mul              , "mul"              , InstructionGroup.RM            , InstructionFlags.SPECIAL       , 0                   , 0                   , 4, 0x000000F6, 0),
  MAKE_INST(InstructionCode.Mulpd            , "mulpd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F59, 0),
  MAKE_INST(InstructionCode.Mulps            , "mulps"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F59, 0),
  MAKE_INST(InstructionCode.Mulsd            , "mulsd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000F59, 0),
  MAKE_INST(InstructionCode.Mulss            , "mulss"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F59, 0),
  MAKE_INST(InstructionCode.Mwait            , "mwait"            , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x000F01C9, 0),
  MAKE_INST(InstructionCode.Neg              , "neg"              , InstructionGroup.RM            , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDWB_MEM        , 0                   , 3, 0x000000F6, 0),
  MAKE_INST(InstructionCode.Nop              , "nop"              , InstructionGroup.EMIT          , InstructionFlags.NONE          , 0                   , 0                   , 0, 0x00000090, 0),
  MAKE_INST(InstructionCode.Not              , "not"              , InstructionGroup.RM            , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDWB_MEM        , 0                   , 2, 0x000000F6, 0),
  MAKE_INST(InstructionCode.Or               , "or"               , InstructionGroup.ALU           , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GQDWB_MEM|NAsmJit.OperandFlags.IMM , 1, 0x00000008, 0x00000080),
  MAKE_INST(InstructionCode.Orpd             , "orpd"             , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F56, 0),
  MAKE_INST(InstructionCode.Orps             , "orps"             , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F56, 0),
  MAKE_INST(InstructionCode.Pabsb            , "pabsb"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F381C, 0),
  MAKE_INST(InstructionCode.Pabsd            , "pabsd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F381E, 0),
  MAKE_INST(InstructionCode.Pabsw            , "pabsw"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F381D, 0),
  MAKE_INST(InstructionCode.Packssdw         , "packssdw"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F6B, 0),
  MAKE_INST(InstructionCode.Packsswb         , "packsswb"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F63, 0),
  MAKE_INST(InstructionCode.Packusdw         , "packusdw"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F382B, 0),
  MAKE_INST(InstructionCode.Packuswb         , "packuswb"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F67, 0),
  MAKE_INST(InstructionCode.Paddb            , "paddb"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FFC, 0),
  MAKE_INST(InstructionCode.Paddd            , "paddd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FFE, 0),
  MAKE_INST(InstructionCode.Paddq            , "paddq"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FD4, 0),
  MAKE_INST(InstructionCode.Paddsb           , "paddsb"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FEC, 0),
  MAKE_INST(InstructionCode.Paddsw           , "paddsw"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FED, 0),
  MAKE_INST(InstructionCode.Paddusb          , "paddusb"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FDC, 0),
  MAKE_INST(InstructionCode.Paddusw          , "paddusw"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FDD, 0),
  MAKE_INST(InstructionCode.Paddw            , "paddw"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FFD, 0),
  MAKE_INST(InstructionCode.Palignr          , "palignr"          , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F3A0F, 0),
  MAKE_INST(InstructionCode.Pand             , "pand"             , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FDB, 0),
  MAKE_INST(InstructionCode.Pandn            , "pandn"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FDF, 0),
  MAKE_INST(InstructionCode.Pause            , "pause"            , InstructionGroup.EMIT          , InstructionFlags.NONE          , 0                   , 0                   , 0, 0xF3000090, 0),
  MAKE_INST(InstructionCode.Pavgb            , "pavgb"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FE0, 0),
  MAKE_INST(InstructionCode.Pavgw            , "pavgw"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FE3, 0),
  MAKE_INST(InstructionCode.Pblendvb         , "pblendvb"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3810, 0),
  MAKE_INST(InstructionCode.Pblendw          , "pblendw"          , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3A0E, 0),
  MAKE_INST(InstructionCode.Pcmpeqb          , "pcmpeqb"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F74, 0),
  MAKE_INST(InstructionCode.Pcmpeqd          , "pcmpeqd"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F76, 0),
  MAKE_INST(InstructionCode.Pcmpeqq          , "pcmpeqq"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3829, 0),
  MAKE_INST(InstructionCode.Pcmpeqw          , "pcmpeqw"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F75, 0),
  MAKE_INST(InstructionCode.Pcmpestri        , "pcmpestri"        , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3A61, 0),
  MAKE_INST(InstructionCode.Pcmpestrm        , "pcmpestrm"        , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3A60, 0),
  MAKE_INST(InstructionCode.Pcmpgtb          , "pcmpgtb"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F64, 0),
  MAKE_INST(InstructionCode.Pcmpgtd          , "pcmpgtd"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F66, 0),
  MAKE_INST(InstructionCode.Pcmpgtq          , "pcmpgtq"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3837, 0),
  MAKE_INST(InstructionCode.Pcmpgtw          , "pcmpgtw"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F65, 0),
  MAKE_INST(InstructionCode.Pcmpistri        , "pcmpistri"        , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3A63, 0),
  MAKE_INST(InstructionCode.Pcmpistrm        , "pcmpistrm"        , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3A62, 0),
  MAKE_INST(InstructionCode.Pextrb           , "pextrb"           , InstructionGroup.MMU_PEXTR     , InstructionFlags.NONE          , NAsmJit.OperandFlags.GD|NAsmJit.OperandFlags.GB|NAsmJit.OperandFlags.MEM  , NAsmJit.OperandFlags.XMM              , 0, 0x000F3A14, 0),
  MAKE_INST(InstructionCode.Pextrd           , "pextrd"           , InstructionGroup.MMU_PEXTR     , InstructionFlags.NONE          , NAsmJit.OperandFlags.GD      |NAsmJit.OperandFlags.MEM  , NAsmJit.OperandFlags.XMM              , 0, 0x000F3A16, 0),
  MAKE_INST(InstructionCode.Pextrq           , "pextrq"           , InstructionGroup.MMU_PEXTR     , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQD     |NAsmJit.OperandFlags.MEM  , NAsmJit.OperandFlags.XMM              , 1, 0x000F3A16, 0),
  MAKE_INST(InstructionCode.Pextrw           , "pextrw"           , InstructionGroup.MMU_PEXTR     , InstructionFlags.NONE          , NAsmJit.OperandFlags.GD      |NAsmJit.OperandFlags.MEM  , NAsmJit.OperandFlags.XMM | NAsmJit.OperandFlags.MM      , 0, 0x000F3A15, 0),
  MAKE_INST(InstructionCode.Pf2id            , "pf2id"            , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0x1D),
  MAKE_INST(InstructionCode.Pf2iw            , "pf2iw"            , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0x1C),
  MAKE_INST(InstructionCode.Pfacc            , "pfacc"            , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0xAE),
  MAKE_INST(InstructionCode.Pfadd            , "pfadd"            , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0x9E),
  MAKE_INST(InstructionCode.Pfcmpeq          , "pfcmpeq"          , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0xB0),
  MAKE_INST(InstructionCode.Pfcmpge          , "pfcmpge"          , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0x90),
  MAKE_INST(InstructionCode.Pfcmpgt          , "pfcmpgt"          , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0xA0),
  MAKE_INST(InstructionCode.Pfmax            , "pfmax"            , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0xA4),
  MAKE_INST(InstructionCode.Pfmin            , "pfmin"            , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0x94),
  MAKE_INST(InstructionCode.Pfmul            , "pfmul"            , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0xB4),
  MAKE_INST(InstructionCode.Pfnacc           , "pfnacc"           , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0x8A),
  MAKE_INST(InstructionCode.Pfpnacc          , "pfpnacc"          , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0x8E),
  MAKE_INST(InstructionCode.Pfrcp            , "pfrcp"            , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0x96),
  MAKE_INST(InstructionCode.Pfrcpit1         , "pfrcpit1"         , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0xA6),
  MAKE_INST(InstructionCode.Pfrcpit2         , "pfrcpit2"         , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0xB6),
  MAKE_INST(InstructionCode.Pfrsqit1         , "pfrsqit1"         , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0xA7),
  MAKE_INST(InstructionCode.Pfrsqrt          , "pfrsqrt"          , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0x97),
  MAKE_INST(InstructionCode.Pfsub            , "pfsub"            , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0x9A),
  MAKE_INST(InstructionCode.Pfsubr           , "pfsubr"           , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0xAA),
  MAKE_INST(InstructionCode.Phaddd           , "phaddd"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F3802, 0),
  MAKE_INST(InstructionCode.Phaddsw          , "phaddsw"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F3803, 0),
  MAKE_INST(InstructionCode.Phaddw           , "phaddw"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F3801, 0),
  MAKE_INST(InstructionCode.Phminposuw       , "phminposuw"       , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3841, 0),
  MAKE_INST(InstructionCode.Phsubd           , "phsubd"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F3806, 0),
  MAKE_INST(InstructionCode.Phsubsw          , "phsubsw"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F3807, 0),
  MAKE_INST(InstructionCode.Phsubw           , "phsubw"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F3805, 0),
  MAKE_INST(InstructionCode.Pi2fd            , "pi2fd"            , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0x0D),
  MAKE_INST(InstructionCode.Pi2fw            , "pi2fw"            , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM           , 0, 0x00000F0F, 0x0C),
  MAKE_INST(InstructionCode.Pinsrb           , "pinsrb"           , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.GD | NAsmJit.OperandFlags.MEM      , 0, 0x660F3A20, 0),
  MAKE_INST(InstructionCode.Pinsrd           , "pinsrd"           , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.GD | NAsmJit.OperandFlags.MEM      , 0, 0x660F3A22, 0),
  MAKE_INST(InstructionCode.Pinsrq           , "pinsrq"           , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.GQ | NAsmJit.OperandFlags.MEM      , 0, 0x660F3A22, 0),
  MAKE_INST(InstructionCode.Pinsrw           , "pinsrw"           , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.GD | NAsmJit.OperandFlags.MEM      , 0, 0x00000FC4, 0),
  MAKE_INST(InstructionCode.Pmaddubsw        , "pmaddubsw"        , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F3804, 0),
  MAKE_INST(InstructionCode.Pmaddwd          , "pmaddwd"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FF5, 0),
  MAKE_INST(InstructionCode.Pmaxsb           , "pmaxsb"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F383C, 0),
  MAKE_INST(InstructionCode.Pmaxsd           , "pmaxsd"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F383D, 0),
  MAKE_INST(InstructionCode.Pmaxsw           , "pmaxsw"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FEE, 0),
  MAKE_INST(InstructionCode.Pmaxub           , "pmaxub"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FDE, 0),
  MAKE_INST(InstructionCode.Pmaxud           , "pmaxud"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F383F, 0),
  MAKE_INST(InstructionCode.Pmaxuw           , "pmaxuw"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F383E, 0),
  MAKE_INST(InstructionCode.Pminsb           , "pminsb"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3838, 0),
  MAKE_INST(InstructionCode.Pminsd           , "pminsd"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3839, 0),
  MAKE_INST(InstructionCode.Pminsw           , "pminsw"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FEA, 0),
  MAKE_INST(InstructionCode.Pminub           , "pminub"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FDA, 0),
  MAKE_INST(InstructionCode.Pminud           , "pminud"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F383B, 0),
  MAKE_INST(InstructionCode.Pminuw           , "pminuw"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F383A, 0),
  MAKE_INST(InstructionCode.Pmovmskb         , "pmovmskb"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.GQD              , NAsmJit.OperandFlags.MM_XMM          , 0, 0x00000FD7, 0),
  MAKE_INST(InstructionCode.Pmovsxbd         , "pmovsxbd"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3821, 0),
  MAKE_INST(InstructionCode.Pmovsxbq         , "pmovsxbq"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3822, 0),
  MAKE_INST(InstructionCode.Pmovsxbw         , "pmovsxbw"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3820, 0),
  MAKE_INST(InstructionCode.Pmovsxdq         , "pmovsxdq"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3825, 0),
  MAKE_INST(InstructionCode.Pmovsxwd         , "pmovsxwd"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3823, 0),
  MAKE_INST(InstructionCode.Pmovsxwq         , "pmovsxwq"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3824, 0),
  MAKE_INST(InstructionCode.Pmovzxbd         , "pmovzxbd"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3831, 0),
  MAKE_INST(InstructionCode.Pmovzxbq         , "pmovzxbq"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3832, 0),
  MAKE_INST(InstructionCode.Pmovzxbw         , "pmovzxbw"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3830, 0),
  MAKE_INST(InstructionCode.Pmovzxdq         , "pmovzxdq"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3835, 0),
  MAKE_INST(InstructionCode.Pmovzxwd         , "pmovzxwd"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3833, 0),
  MAKE_INST(InstructionCode.Pmovzxwq         , "pmovzxwq"         , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3834, 0),
  MAKE_INST(InstructionCode.Pmuldq           , "pmuldq"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3828, 0),
  MAKE_INST(InstructionCode.Pmulhrsw         , "pmulhrsw"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F380B, 0),
  MAKE_INST(InstructionCode.Pmulhuw          , "pmulhuw"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FE4, 0),
  MAKE_INST(InstructionCode.Pmulhw           , "pmulhw"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FE5, 0),
  MAKE_INST(InstructionCode.Pmulld           , "pmulld"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3840, 0),
  MAKE_INST(InstructionCode.Pmullw           , "pmullw"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FD5, 0),
  MAKE_INST(InstructionCode.Pmuludq          , "pmuludq"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FF4, 0),
  MAKE_INST(InstructionCode.Pop              , "pop"              , InstructionGroup.POP           , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x00000058, 0x0000008F),
  MAKE_INST(InstructionCode.Popad            , "popad"            , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x00000061, 0),
  MAKE_INST(InstructionCode.Popcnt           , "popcnt"           , InstructionGroup.R_RM          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDW             , NAsmJit.OperandFlags.GQDW_MEM         , 0, 0xF3000FB8, 0),
  MAKE_INST(InstructionCode.Popfd            , "popfd"            , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x0000009D, 0),
  MAKE_INST(InstructionCode.Popfq            , "popfq"            , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x0000009D, 0),
  MAKE_INST(InstructionCode.Por              , "por"              , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FEB, 0),
  MAKE_INST(InstructionCode.Prefetch         , "prefetch"         , InstructionGroup.MMU_PREFETCH  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.IMM             , 0, 0         , 0),
  MAKE_INST(InstructionCode.Psadbw           , "psadbw"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FF6, 0),
  MAKE_INST(InstructionCode.Pshufb           , "pshufb"           , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F3800, 0),
  MAKE_INST(InstructionCode.Pshufd           , "pshufd"           , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x66000F70, 0),
  MAKE_INST(InstructionCode.Pshufw           , "pshufw"           , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.MOV           , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F70, 0),
  MAKE_INST(InstructionCode.Pshufhw          , "pshufhw"          , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0xF3000F70, 0),
  MAKE_INST(InstructionCode.Pshuflw          , "pshuflw"          , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0xF2000F70, 0),
  MAKE_INST(InstructionCode.Psignb           , "psignb"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F3808, 0),
  MAKE_INST(InstructionCode.Psignd           , "psignd"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F380A, 0),
  MAKE_INST(InstructionCode.Psignw           , "psignw"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x000F3809, 0),
  MAKE_INST(InstructionCode.Pslld            , "pslld"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM|NAsmJit.OperandFlags.IMM, 6, 0x00000FF2, 0x00000F72),
  MAKE_INST(InstructionCode.Pslldq           , "pslldq"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.IMM             , 7, 0        , 0x66000F73),
  MAKE_INST(InstructionCode.Psllq            , "psllq"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM|NAsmJit.OperandFlags.IMM, 6, 0x00000FF3, 0x00000F73),
  MAKE_INST(InstructionCode.Psllw            , "psllw"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM|NAsmJit.OperandFlags.IMM, 6, 0x00000FF1, 0x00000F71),
  MAKE_INST(InstructionCode.Psrad            , "psrad"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM|NAsmJit.OperandFlags.IMM, 4, 0x00000FE2, 0x00000F72),
  MAKE_INST(InstructionCode.Psraw            , "psraw"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM|NAsmJit.OperandFlags.IMM, 4, 0x00000FE1, 0x00000F71),
  MAKE_INST(InstructionCode.Psrld            , "psrld"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM|NAsmJit.OperandFlags.IMM, 2, 0x00000FD2, 0x00000F72),
  MAKE_INST(InstructionCode.Psrldq           , "psrldq"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.IMM             , 3, 0        , 0x66000F73),
  MAKE_INST(InstructionCode.Psrlq            , "psrlq"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM|NAsmJit.OperandFlags.IMM, 2, 0x00000FD3, 0x00000F73),
  MAKE_INST(InstructionCode.Psrlw            , "psrlw"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM|NAsmJit.OperandFlags.IMM, 2, 0x00000FD1, 0x00000F71),
  MAKE_INST(InstructionCode.Psubb            , "psubb"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FF8, 0),
  MAKE_INST(InstructionCode.Psubd            , "psubd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FFA, 0),
  MAKE_INST(InstructionCode.Psubq            , "psubq"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FFB, 0),
  MAKE_INST(InstructionCode.Psubsb           , "psubsb"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FE8, 0),
  MAKE_INST(InstructionCode.Psubsw           , "psubsw"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FE9, 0),
  MAKE_INST(InstructionCode.Psubusb          , "psubusb"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FD8, 0),
  MAKE_INST(InstructionCode.Psubusw          , "psubusw"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FD9, 0),
  MAKE_INST(InstructionCode.Psubw            , "psubw"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FF9, 0),
  MAKE_INST(InstructionCode.Pswapd           , "pswapd"           , InstructionGroup.MMU_RM_3DNOW  , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM               , NAsmJit.OperandFlags.MM_MEM          , 0, 0x00000F0F, 0xBB),
  MAKE_INST(InstructionCode.Ptest            , "ptest"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x660F3817, 0),
  MAKE_INST(InstructionCode.Punpckhbw        , "punpckhbw"        , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F68, 0),
  MAKE_INST(InstructionCode.Punpckhdq        , "punpckhdq"        , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F6A, 0),
  MAKE_INST(InstructionCode.Punpckhqdq       , "punpckhqdq"       , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x66000F6D, 0),
  MAKE_INST(InstructionCode.Punpckhwd        , "punpckhwd"        , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F69, 0),
  MAKE_INST(InstructionCode.Punpcklbw        , "punpcklbw"        , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F60, 0),
  MAKE_INST(InstructionCode.Punpckldq        , "punpckldq"        , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F62, 0),
  MAKE_INST(InstructionCode.Punpcklqdq       , "punpcklqdq"       , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM         , 0, 0x66000F6C, 0),
  MAKE_INST(InstructionCode.Punpcklwd        , "punpcklwd"        , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000F61, 0),
  MAKE_INST(InstructionCode.Push             , "push"             , InstructionGroup.PUSH          , InstructionFlags.SPECIAL       , 0                   , 0                   , 6, 0x00000050, 0x000000FF),
  MAKE_INST(InstructionCode.Pushad           , "pushad"           , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x00000060, 0),
  MAKE_INST(InstructionCode.Pushfd           , "pushfd"           , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x0000009C, 0),
  MAKE_INST(InstructionCode.Pushfq           , "pushfq"           , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x0000009C, 0),
  MAKE_INST(InstructionCode.Pxor             , "pxor"             , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.MM_XMM           , NAsmJit.OperandFlags.MM_XMM_MEM       , 0, 0x00000FEF, 0),
  MAKE_INST(InstructionCode.Rcl              , "rcl"              , InstructionGroup.ROT           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GB|NAsmJit.OperandFlags.IMM        , 2, 0         , 0),
  MAKE_INST(InstructionCode.Rcpps            , "rcpps"            , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F53, 0),
  MAKE_INST(InstructionCode.Rcpss            , "rcpss"            , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F53, 0),
  MAKE_INST(InstructionCode.Rcr              , "rcr"              , InstructionGroup.ROT           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GB|NAsmJit.OperandFlags.IMM        , 3, 0         , 0),
  MAKE_INST(InstructionCode.Rdtsc            , "rdtsc"            , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x00000F31, 0),
  MAKE_INST(InstructionCode.Rdtscp           , "rdtscp"           , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x000F01F9, 0),
  MAKE_INST(InstructionCode.RepLodsb         , "rep lodsb"        , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , 0                   , 0, 0xF30000AC, 1 /* Size of mem */),
  MAKE_INST(InstructionCode.RepLodsd         , "rep lodsd"        , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , 0                   , 0, 0xF30000AC, 4 /* Size of mem */),
  MAKE_INST(InstructionCode.RepLodsq         , "rep lodsq"        , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , 0                   , 0, 0xF30000AC, 8 /* Size of mem */),
  MAKE_INST(InstructionCode.RepLodsw         , "rep lodsw"        , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , 0                   , 0, 0xF30000AC, 2 /* Size of mem */),
  MAKE_INST(InstructionCode.RepMovsb         , "rep movsb"        , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF30000A4, 1 /* Size of mem */),
  MAKE_INST(InstructionCode.RepMovsd         , "rep movsd"        , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF30000A4, 4 /* Size of mem */),
  MAKE_INST(InstructionCode.RepMovsq         , "rep movsq"        , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF30000A4, 8 /* Size of mem */),
  MAKE_INST(InstructionCode.RepMovsw         , "rep movsw"        , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF30000A4, 2 /* Size of mem */),
  MAKE_INST(InstructionCode.RepStosb         , "rep stosb"        , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , 0                   , 0, 0xF30000AA, 1 /* Size of mem */),
  MAKE_INST(InstructionCode.RepStosd         , "rep stosd"        , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , 0                   , 0, 0xF30000AA, 4 /* Size of mem */),
  MAKE_INST(InstructionCode.RepStosq         , "rep stosq"        , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , 0                   , 0, 0xF30000AA, 8 /* Size of mem */),
  MAKE_INST(InstructionCode.RepStosw         , "rep stosw"        , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , 0                   , 0, 0xF30000AA, 2 /* Size of mem */),
  MAKE_INST(InstructionCode.RepeCmpsb        , "repe cmpsb"       , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF30000A6, 1 /* Size of mem */),
  MAKE_INST(InstructionCode.RepeCmpsd        , "repe cmpsd"       , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF30000A6, 4 /* Size of mem */),
  MAKE_INST(InstructionCode.RepeCmpsq        , "repe cmpsq"       , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF30000A6, 8 /* Size of mem */),
  MAKE_INST(InstructionCode.RepeCmpsw        , "repe cmpsw"       , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF30000A6, 2 /* Size of mem */),
  MAKE_INST(InstructionCode.RepeScasb        , "repe scasb"       , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF30000AE, 1 /* Size of mem */),
  MAKE_INST(InstructionCode.RepeScasd        , "repe scasd"       , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF30000AE, 4 /* Size of mem */),
  MAKE_INST(InstructionCode.RepeScasq        , "repe scasq"       , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF30000AE, 8 /* Size of mem */),
  MAKE_INST(InstructionCode.RepeScasw        , "repe scasw"       , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF30000AE, 2 /* Size of mem */),
  MAKE_INST(InstructionCode.RepneCmpsb       , "repne cmpsb"      , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF20000A6, 1 /* Size of mem */),
  MAKE_INST(InstructionCode.RepneCmpsd       , "repne cmpsd"      , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF20000A6, 4 /* Size of mem */),
  MAKE_INST(InstructionCode.RepneCmpsq       , "repne cmpsq"      , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF20000A6, 8 /* Size of mem */),
  MAKE_INST(InstructionCode.RepneCmpsw       , "repne cmpsw"      , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF20000A6, 2 /* Size of mem */),
  MAKE_INST(InstructionCode.RepneScasb       , "repne scasb"      , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF20000AE, 1 /* Size of mem */),
  MAKE_INST(InstructionCode.RepneScasd       , "repne scasd"      , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF20000AE, 4 /* Size of mem */),
  MAKE_INST(InstructionCode.RepneScasq       , "repne scasq"      , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF20000AE, 8 /* Size of mem */),
  MAKE_INST(InstructionCode.RepneScasw       , "repne scasw"      , InstructionGroup.REP           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.MEM              , NAsmJit.OperandFlags.MEM              , 0, 0xF20000AE, 2 /* Size of mem */),
  MAKE_INST(InstructionCode.Ret              , "ret"              , InstructionGroup.RET           , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0         , 0),
  MAKE_INST(InstructionCode.Rol              , "rol"              , InstructionGroup.ROT           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GB|NAsmJit.OperandFlags.IMM        , 0, 0         , 0),
  MAKE_INST(InstructionCode.Ror              , "ror"              , InstructionGroup.ROT           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GB|NAsmJit.OperandFlags.IMM        , 1, 0         , 0),
  MAKE_INST(InstructionCode.Roundpd          , "roundpd"          , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x660F3A09, 0),
  MAKE_INST(InstructionCode.Roundps          , "roundps"          , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x660F3A08, 0),
  MAKE_INST(InstructionCode.Roundsd          , "roundsd"          , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x660F3A0B, 0),
  MAKE_INST(InstructionCode.Roundss          , "roundss"          , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x660F3A0A, 0),
  MAKE_INST(InstructionCode.Rsqrtps          , "rsqrtps"          , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F52, 0),
  MAKE_INST(InstructionCode.Rsqrtss          , "rsqrtss"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F52, 0),
  MAKE_INST(InstructionCode.Sahf             , "sahf"             , InstructionGroup.EMIT          , InstructionFlags.SPECIAL       , 0                   , 0                   , 0, 0x0000009E, 0),
  MAKE_INST(InstructionCode.Sal              , "sal"              , InstructionGroup.ROT           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GB|NAsmJit.OperandFlags.IMM        , 4, 0         , 0),
  MAKE_INST(InstructionCode.Sar              , "sar"              , InstructionGroup.ROT           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GB|NAsmJit.OperandFlags.IMM        , 7, 0         , 0),
  MAKE_INST(InstructionCode.Sbb              , "sbb"              , InstructionGroup.ALU           , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GQDWB_MEM|NAsmJit.OperandFlags.IMM , 3, 0x00000018, 0x00000080),
  MAKE_INST(InstructionCode.Seta             , "seta"             , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F97, 0),
  MAKE_INST(InstructionCode.Setae            , "setae"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F93, 0),
  MAKE_INST(InstructionCode.Setb             , "setb"             , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F92, 0),
  MAKE_INST(InstructionCode.Setbe            , "setbe"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F96, 0),
  MAKE_INST(InstructionCode.Setc             , "setc"             , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F92, 0),
  MAKE_INST(InstructionCode.Sete             , "sete"             , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F94, 0),
  MAKE_INST(InstructionCode.Setg             , "setg"             , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F9F, 0),
  MAKE_INST(InstructionCode.Setge            , "setge"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F9D, 0),
  MAKE_INST(InstructionCode.Setl             , "setl"             , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F9C, 0),
  MAKE_INST(InstructionCode.Setle            , "setle"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F9E, 0),
  MAKE_INST(InstructionCode.Setna            , "setna"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F96, 0),
  MAKE_INST(InstructionCode.Setnae           , "setnae"           , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F92, 0),
  MAKE_INST(InstructionCode.Setnb            , "setnb"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F93, 0),
  MAKE_INST(InstructionCode.Setnbe           , "setnbe"           , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F97, 0),
  MAKE_INST(InstructionCode.Setnc            , "setnc"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F93, 0),
  MAKE_INST(InstructionCode.Setne            , "setne"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F95, 0),
  MAKE_INST(InstructionCode.Setng            , "setng"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F9E, 0),
  MAKE_INST(InstructionCode.Setnge           , "setnge"           , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F9C, 0),
  MAKE_INST(InstructionCode.Setnl            , "setnl"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F9D, 0),
  MAKE_INST(InstructionCode.Setnle           , "setnle"           , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F9F, 0),
  MAKE_INST(InstructionCode.Setno            , "setno"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F91, 0),
  MAKE_INST(InstructionCode.Setnp            , "setnp"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F9B, 0),
  MAKE_INST(InstructionCode.Setns            , "setns"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F99, 0),
  MAKE_INST(InstructionCode.Setnz            , "setnz"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F95, 0),
  MAKE_INST(InstructionCode.Seto             , "seto"             , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F90, 0),
  MAKE_INST(InstructionCode.Setp             , "setp"             , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F9A, 0),
  MAKE_INST(InstructionCode.Setpe            , "setpe"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F9A, 0),
  MAKE_INST(InstructionCode.Setpo            , "setpo"            , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F9B, 0),
  MAKE_INST(InstructionCode.Sets             , "sets"             , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F98, 0),
  MAKE_INST(InstructionCode.Setz             , "setz"             , InstructionGroup.RM_B          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GB_MEM           , 0                   , 0, 0x00000F94, 0),
  MAKE_INST(InstructionCode.Sfence           , "sfence"           , InstructionGroup.EMIT          , InstructionFlags.NONE          , 0                   , 0                   , 0, 0x000FAEF8, 0),
  MAKE_INST(InstructionCode.Shl              , "shl"              , InstructionGroup.ROT           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GB|NAsmJit.OperandFlags.IMM        , 4, 0         , 0),
  MAKE_INST(InstructionCode.Shld             , "shld"             , InstructionGroup.SHLD_SHRD     , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GB               , 0, 0x00000FA4, 0),
  MAKE_INST(InstructionCode.Shr              , "shr"              , InstructionGroup.ROT           , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GB|NAsmJit.OperandFlags.IMM        , 5, 0         , 0),
  MAKE_INST(InstructionCode.Shrd             , "shrd"             , InstructionGroup.SHLD_SHRD     , InstructionFlags.SPECIAL       , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GQDWB            , 0, 0x00000FAC, 0),
  MAKE_INST(InstructionCode.Shufpd           , "shufpd"           , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000FC6, 0),
  MAKE_INST(InstructionCode.Shufps           , "shufps"           , InstructionGroup.MMU_RM_IMM8   , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000FC6, 0),
  MAKE_INST(InstructionCode.Sqrtpd           , "sqrtpd"           , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F51, 0),
  MAKE_INST(InstructionCode.Sqrtps           , "sqrtps"           , InstructionGroup.MMU_RMI       , InstructionFlags.MOV           , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F51, 0),
  MAKE_INST(InstructionCode.Sqrtsd           , "sqrtsd"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000F51, 0),
  MAKE_INST(InstructionCode.Sqrtss           , "sqrtss"           , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F51, 0),
  MAKE_INST(InstructionCode.Stc              , "stc"              , InstructionGroup.EMIT          , InstructionFlags.NONE          , 0                   , 0                   , 0, 0x000000F9, 0),
  MAKE_INST(InstructionCode.Std              , "std"              , InstructionGroup.EMIT          , InstructionFlags.NONE          , 0                   , 0                   , 0, 0x000000FD, 0),
  MAKE_INST(InstructionCode.Stmxcsr          , "stmxcsr"          , InstructionGroup.M             , InstructionFlags.NONE          , NAsmJit.OperandFlags.MEM              , 0                   , 3, 0x00000FAE, 0),
  MAKE_INST(InstructionCode.Sub              , "sub"              , InstructionGroup.ALU           , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GQDWB_MEM|NAsmJit.OperandFlags.IMM , 5, 0x00000028, 0x00000080),
  MAKE_INST(InstructionCode.Subpd            , "subpd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F5C, 0),
  MAKE_INST(InstructionCode.Subps            , "subps"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F5C, 0),
  MAKE_INST(InstructionCode.Subsd            , "subsd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF2000F5C, 0),
  MAKE_INST(InstructionCode.Subss            , "subss"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0xF3000F5C, 0),
  MAKE_INST(InstructionCode.Test             , "test"             , InstructionGroup.TEST          , InstructionFlags.NONE          , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GQDWB|NAsmJit.OperandFlags.IMM     , 0, 0         , 0),
  MAKE_INST(InstructionCode.Ucomisd          , "ucomisd"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F2E, 0),
  MAKE_INST(InstructionCode.Ucomiss          , "ucomiss"          , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F2E, 0),
  MAKE_INST(InstructionCode.Ud2              , "ud2"              , InstructionGroup.EMIT          , InstructionFlags.NONE          , 0                   , 0                   , 0, 0x00000F0B, 0),
  MAKE_INST(InstructionCode.Unpckhpd         , "unpckhpd"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F15, 0),
  MAKE_INST(InstructionCode.Unpckhps         , "unpckhps"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F15, 0),
  MAKE_INST(InstructionCode.Unpcklpd         , "unpcklpd"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F14, 0),
  MAKE_INST(InstructionCode.Unpcklps         , "unpcklps"         , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F14, 0),
  MAKE_INST(InstructionCode.Xadd             , "xadd"             , InstructionGroup.RM_R          , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GQDWB            , 0, 0x00000FC0, 0),
  MAKE_INST(InstructionCode.Xchg             , "xchg"             , InstructionGroup.XCHG          , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GQDWB            , 0, 0         , 0),
  MAKE_INST(InstructionCode.Xor              , "xor"              , InstructionGroup.ALU           , InstructionFlags.LOCKABLE          , NAsmJit.OperandFlags.GQDWB_MEM        , NAsmJit.OperandFlags.GQDWB_MEM|NAsmJit.OperandFlags.IMM , 6, 0x00000030, 0x00000080),
  MAKE_INST(InstructionCode.Xorpd            , "xorpd"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x66000F57, 0),
  MAKE_INST(InstructionCode.Xorps            , "xorps"            , InstructionGroup.MMU_RMI       , InstructionFlags.NONE          , NAsmJit.OperandFlags.XMM              , NAsmJit.OperandFlags.XMM_MEM          , 0, 0x00000F57, 0)
};
        }
        #endregion

        private InstructionDescription(InstructionCode code, string name, InstructionGroup group, InstructionFlags flags, OperandFlags[] operandFlags, int opReg, int opcode0, int opcode1)
        {
            Contract.Requires(operandFlags != null);

            _code = code;
            _name = name;
            _group = (byte)group;
            _flags = (byte)flags;
            _operandFlags = new ReadOnlyCollection<OperandFlags>((OperandFlags[])operandFlags.Clone());
            _opcodeR = (short)opReg;
            _opcode0 = opcode0;
            _opcode1 = opcode1;
        }

        public InstructionCode Code
        {
            get
            {
                return _code;
            }
        }

        public string Name
        {
            get
            {
                return _name;
            }
        }

        public InstructionGroup Group
        {
            get
            {
                return (InstructionGroup)_group;
            }
        }

        public InstructionFlags Flags
        {
            get
            {
                return (InstructionFlags)_flags;
            }
        }

        public ReadOnlyCollection<OperandFlags> OperandFlags
        {
            get
            {
                return _operandFlags;
            }
        }

        public int OpCodeR
        {
            get
            {
                return _opcodeR;
            }
        }

        public int OpCode0
        {
            get
            {
                return _opcode0;
            }
        }

        public int OpCode1
        {
            get
            {
                return _opcode1;
            }
        }

        public bool IsJump
        {
            get
            {
                return (Flags & InstructionFlags.JUMP) != 0;
            }
        }

        public bool IsMov
        {
            get
            {
                return (Flags & InstructionFlags.MOV) != 0;
            }
        }

        public bool IsFPU
        {
            get
            {
                return (Flags & InstructionFlags.FPU) != 0;
            }
        }

        public bool IsLockable
        {
            get
            {
                return (Flags & InstructionFlags.LOCKABLE) != 0;
            }
        }

        public bool IsSpecial
        {
            get
            {
                return (Flags & InstructionFlags.SPECIAL) != 0;
            }
        }

        public bool IsSpecialMem
        {
            get
            {
                return (Flags & InstructionFlags.SPECIAL_MEM) != 0;
            }
        }

        public static InstructionDescription FromInstruction(InstructionCode code)
        {
            InstructionDescription description = instructionDescriptions[(int)code];
            if (description.Code != code)
                throw new InvalidOperationException();

            return description;
        }

        private static InstructionDescription MAKE_INST(InstructionCode code, string name, InstructionGroup group, InstructionFlags flags, OperandFlags oflags0, OperandFlags oflags1, int opReg, uint opcode0, uint opcode1)
        {
            return new InstructionDescription(code, name, group, flags, new OperandFlags[] { oflags0, oflags1 }, opReg, (int)opcode0, (int)opcode1);
        }
    }
}
