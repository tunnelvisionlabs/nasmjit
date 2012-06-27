namespace AsmJitNet
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics.Contracts;
    using System.Linq;
    using System.Text;
    using Marshal = System.Runtime.InteropServices.Marshal;

    public class Assembler
        : IIntrinsicSupport<GPReg, X87Reg, MMReg, XMMReg>
        , IX86IntrinsicSupport<GPReg>
        , IX87IntrinsicSupport<X87Reg>
        , IMmIntrinsicSupport<MMReg>
        , IXmmIntrinsicSupport<XMMReg>
    {
        private CodeGenerator _codeGenerator;

        /// <summary>
        /// Properties
        /// </summary>
        private CompilerProperties _properties;

        /// <summary>
        /// Emit flags for next instruction (cleared after emit)
        /// </summary>
        private EmitOptions _emitOptions;

        /// <summary>
        /// Binary code buffer
        /// </summary>
        private readonly Buffer _buffer = new Buffer();

        /// <summary>
        /// Size of possible trampolines
        /// </summary>
        private long _trampolineSize;

        private LabelLink _unusedLinks;

        private readonly List<AssemblerLabelData> _labelData = new List<AssemblerLabelData>();

        private readonly List<RelocationData> _relocationData = new List<RelocationData>();

        private string _comment;

        public Assembler(CodeGenerator codeGenerator = null)
        {
            _codeGenerator = codeGenerator ?? CodeGenerator.Global;
        }

        /// <summary>
        /// Gets or sets the logger
        /// </summary>
        public Logger Logger
        {
            get;
            set;
        }

        public CodeGenerator CodeGenerator
        {
            get
            {
                return _codeGenerator;
            }
        }

        public CompilerProperties Properties
        {
            get
            {
                return _properties;
            }

            set
            {
                _properties = value;
            }
        }

        public EmitOptions EmitOptions
        {
            get
            {
                return _emitOptions;
            }

            set
            {
                _emitOptions = value;
            }
        }

        /// <summary>
        /// Gets the current offset in the buffer
        /// </summary>
        public long Offset
        {
            get
            {
                return _buffer.Offset;
            }
        }

        public long CodeSize
        {
            get
            {
                return _buffer.Offset + TrampolineSize;
            }
        }

        /// <summary>
        /// Gets the size of all possible trampolines needed to successfully generate relative jumps to absolute addresses.
        /// This value is only non-zero if jmp or call instructions were used with immediate operand (this means jump or
        /// call absolute address directly).
        /// </summary>
        /// <remarks>
        /// Currently only EmitJmpOrCallReloc() method can increase trampoline size.
        /// </remarks>
        public long TrampolineSize
        {
            get
            {
                return _trampolineSize;
            }
        }

        public IList<AssemblerLabelData> LabelData
        {
            get
            {
                return _labelData;
            }
        }

        public string Comment
        {
            get
            {
                return _comment;
            }
            set
            {
                _comment = value;
            }
        }

        [ContractInvariantMethod]
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance", "CA1822:MarkMembersAsStatic", Justification = "Required for code contracts.")]
        private void ObjectInvariant()
        {
            Contract.Invariant(_labelData != null);
            Contract.Invariant(_relocationData != null);
            Contract.Invariant(_buffer != null);
        }


        public byte GetByteAt(int position)
        {
            return _buffer.GetByteAt(position);
        }

        public void SetByteAt(int position, byte value)
        {
            _buffer.SetByteAt(position, value);
        }

        public void SetInt32At(int position, int value)
        {
            _buffer.SetDWordAt(position, (uint)value);
        }

        public Label DefineLabel()
        {
            Label label = new Label(_labelData.Count | Operand.OperandIdTypeLabel);
            _labelData.Add(new AssemblerLabelData());
            return label;
        }

        public void RegisterLabels(int count)
        {
            if (count < 0)
                throw new ArgumentOutOfRangeException("count");
            Contract.EndContractBlock();

            for (int i = 0; i < count; i++)
                _labelData.Add(new AssemblerLabelData());
        }

        public void MarkLabel(Label label)
        {
            if (label == null)
                throw new ArgumentNullException("label");
            if (label.Id == Operand.InvalidValue)
                throw new ArgumentException("Only labels created by DefineLabel() can be used by Assembler.");
            if ((label.Id & Operand.OperandIdValueMask) >= _labelData.Count)
                throw new ArgumentException("The label index is out of bounds.");

            // Get label data based on label id.
            AssemblerLabelData l_data = _labelData[label.Id & Operand.OperandIdValueMask];

            // Label can be bound only once.
            if (l_data.Offset != -1)
                throw new InvalidOperationException("The label is already bound.");

            // Log.
            if (Logger != null)
                Logger.LogFormat("L.{0}:" + Environment.NewLine, label.Id & Operand.OperandIdValueMask);

            long pos = Offset;

            LabelLink link = l_data.Links;
            LabelLink prev = null;

            while (link != null)
            {
                int offset = link.Offset;

                if (link.RelocId != -1)
                {
                    // If linked label points to RelocData then instead of writing relative
                    // displacement to assembler stream, we will write it to RelocData.
                    _relocationData[link.RelocId].Destination = (IntPtr)(_relocationData[link.RelocId].Destination.ToInt64() + pos);
                }
                else
                {
                    // Not using relocId, this means that we overwriting real displacement
                    // in assembler stream.
                    int patchedValue = (int)(pos - offset + link.Displacement);
                    int size = GetByteAt(offset);

                    // Only these size specifiers are allowed.
                    if (size != 1 && size != 4)
                        throw new AssemblerException("Incorrect specifier size.");

                    if (size == 4)
                    {
                        SetInt32At(offset, patchedValue);
                    }
                    else // if (size == 1)
                    {
                        if (Util.IsInt8(patchedValue))
                        {
                            SetByteAt(offset, (byte)(sbyte)patchedValue);
                        }
                        else
                        {
                            // Fatal error.
                            throw new AssemblerException("Illegal short jump");
                        }
                    }
                }

                prev = link.Previous;
                link = prev;
            }

            // Chain unused links.
            link = l_data.Links;
            if (link != null)
            {
                if (prev == null)
                    prev = link;

                prev.Previous = _unusedLinks;
                _unusedLinks = link;
            }

            // Unlink label if it was linked.
            l_data.Offset = (int)pos;
            l_data.Links = null;
        }

        public IntPtr Make()
        {
            // Do nothing on error state or when no instruction was emitted.
            if (CodeSize == 0)
                return IntPtr.Zero;

            IntPtr p;
            _codeGenerator.Generate(out p, this);

            return p;
        }

        public IntPtr RelocCode(IntPtr destination)
        {
            return RelocCode(destination, destination);
        }

        public IntPtr RelocCode(IntPtr destination, IntPtr addressBase)
        {
            // Copy code to virtual memory (this is a given destination pointer).

            var coff = _buffer.Offset;
            var csize = CodeSize;

            // We are copying the exact size of the generated code. Extra code for trampolines
            // is generated on-the-fly by relocator (this code doesn't exist at the moment).
            Marshal.Copy(_buffer.Data, 0, destination, (int)coff);

            // Trampoline pointer
            IntPtr tramp = (IntPtr)(destination.ToInt64() + coff);

            int i;
            int len = _relocationData.Count;

            for (i = 0; i < len; i++)
            {
                RelocationData r = _relocationData[i];
                IntPtr val;

                // Whether to use trampoline, can be only used if relocation type is ABSOLUTE_TO_RELATIVE_TRAMPOLINE
                bool useTrampoline = false;

                // Be sure that reloc data structure is correct.
                if (r.Offset + r.Size > csize)
                    throw new AssemblerException("Incorrect reloc data structure.");

                switch (r.Type)
                {
                case RelocationType.AbsoluteToAbsolute:
                    val = r.Destination;
                    break;

                case RelocationType.RelativeToAbsolute:
                    val = (IntPtr)(addressBase.ToInt64() + r.Destination.ToInt64());
                    break;

                case RelocationType.AbsoluteToRelative:
                case RelocationType.AbsoluteToRelativeTrampoline:
                    val = (IntPtr)(r.Destination.ToInt64() - (addressBase.ToInt64() + r.Offset + 4));

                    if (Util.IsX64)
                    {
                        if (r.Type == RelocationType.AbsoluteToRelativeTrampoline && !Util.IsInt32(val.ToInt64()))
                        {
                            val = (IntPtr)(tramp.ToInt64() - (destination.ToInt64() + r.Offset + 4));
                            useTrampoline = true;
                        }
                    }

                    break;

                default:
                    throw new AssemblerException("Invalid relocation type.");
                }

                switch (r.Size)
                {
                case 4:
                    Marshal.WriteInt32((IntPtr)(destination.ToInt64() + r.Offset), val.ToInt32());
                    break;

                case 8:
                    Marshal.WriteInt64((IntPtr)(destination.ToInt64() + r.Offset), val.ToInt64());
                    break;

                default:
                    throw new AssemblerException("Invalid relocation size.");
                }

                if (useTrampoline)
                {
                    if (Logger != null)
                    {
                        Logger.LogFormat("; Trampoline from 0x{0:x} -> 0x{1:x}" + Environment.NewLine, addressBase.ToInt64() + r.Offset, r.Destination);
                    }

                    TrampolineWriter.WriteTrampoline(tramp, r.Destination);
                    tramp += TrampolineWriter.TRAMPOLINE_SIZE;
                }
            }

            if (Util.IsX64)
                return (IntPtr)(tramp.ToInt64() - destination.ToInt64());
            else
                return (IntPtr)coff;
        }

        public void Embed(byte[] data)
        {
            if (!CanEmit())
                return;

            int size = data.Length;

            if (Logger != null)
            {
                int i;
                int j;
                int max;
                StringBuilder buf = new StringBuilder(128);
                string dot = ".data ";

                buf.Append(dot);

                for (i = 0; i < size; i += 16)
                {
                    max = (size - i < 16) ? size - i : 16;

                    for (j = 0; j < max; j++)
                        buf.AppendFormat("{0:x}", data[i + j]);

                    buf.AppendLine();

                    Logger.LogString(buf.ToString());
                }
            }

            _buffer.EmitData(data);
        }

        public void EmbedLabel(Label label)
        {
            Contract.Requires(label != null);
            Contract.Requires(label.Id != Operand.InvalidValue);

            if (CanEmit())
                return;

            AssemblerLabelData l_data = _labelData[label.Id & Operand.OperandIdValueMask];
            RelocationData r_data = new RelocationData();

            if (Logger != null)
            {
                Logger.LogFormat(IntPtr.Size == 4 ? ".dd L.{0}\n" : ".dq L.{1}\n", label.Id & Operand.OperandIdValueMask);
            }

            r_data.Type = RelocationType.RelativeToAbsolute;
            r_data.Size = IntPtr.Size;
            r_data.Offset = Offset;
            r_data.Destination = IntPtr.Zero;

            if (l_data.Offset != -1)
            {
                // Bound label.
                r_data.Destination = (IntPtr)l_data.Offset;
            }
            else
            {
                // Non-bound label. Need to chain.
                LabelLink link = NewLabelLink();

                link.Previous = l_data.Links;
                link.Offset = (int)Offset;
                link.Displacement = 0;
                link.RelocId = _relocationData.Count;

                l_data.Links = link;
            }

            _relocationData.Add(r_data);

            // Emit dummy sysint (4 or 8 bytes that depends on address size).
            EmitSysInt(IntPtr.Zero);
        }

        // Intel and AMD.
        private static readonly byte[] _nop1 = { 0x90 };
        private static readonly byte[] _nop2 = { 0x66, 0x90 };
        private static readonly byte[] _nop3 = { 0x0F, 0x1F, 0x00 };
        private static readonly byte[] _nop4 = { 0x0F, 0x1F, 0x40, 0x00 };
        private static readonly byte[] _nop5 = { 0x0F, 0x1F, 0x44, 0x00, 0x00 };
        private static readonly byte[] _nop6 = { 0x66, 0x0F, 0x1F, 0x44, 0x00, 0x00 };
        private static readonly byte[] _nop7 = { 0x0F, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00 };
        private static readonly byte[] _nop8 = { 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00 };
        private static readonly byte[] _nop9 = { 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00 };

        // AMD.
        private static readonly byte[] _nop10 = { 0x66, 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00 };
        private static readonly byte[] _nop11 = { 0x66, 0x66, 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00 };

        public void Align(int m)
        {
            if (m < 0)
                throw new ArgumentOutOfRangeException("m");
            if (m > 64)
                throw new NotSupportedException();

            if (!CanEmit())
                return;

            if (Logger != null)
            {
                Logger.LogFormat(".align {0}", m);
            }

            if (m == 0)
                return;

            int i = m - ((int)Offset % m);
            if (i == m)
                return;

            if ((_properties & CompilerProperties.OptimizeAlign) != 0)
            {
                CpuInfo ci = CpuInfo.Instance;

                // NOPs optimized for Intel:
                //   Intel 64 and IA-32 Architectures Software Developer's Manual
                //   - Volume 2B 
                //   - Instruction Set Reference N-Z
                //     - NOP

                // NOPs optimized for AMD:
                //   Software Optimization Guide for AMD Family 10h Processors (Quad-Core)
                //   - 4.13 - Code Padding with Operand-Size Override and Multibyte NOP

                byte[] p;
                bool validIntel = ci.VendorId == CpuVendor.Intel &&
                   ((ci.Family & 0x0F) == 6 ||
                    (ci.Family & 0x0F) == 15);

                bool validAmd = ci.VendorId == CpuVendor.Amd && ci.Family >= 0x0F;

                if (validIntel || validAmd)
                {
                    do
                    {
                        switch (i)
                        {
                        case 1:
                            p = _nop1;
                            break;

                        case 2:
                            p = _nop2;
                            break;

                        case 3:
                            p = _nop3;
                            break;

                        case 4:
                            p = _nop4;
                            break;

                        case 5:
                            p = _nop5;
                            break;

                        case 6:
                            p = _nop6;
                            break;

                        case 7:
                            p = _nop7;
                            break;

                        case 8:
                            p = _nop8;
                            break;

                        case 9:
                            p = _nop9;
                            break;

                        case 10:
                            p = validAmd ? _nop10 : _nop9;
                            break;

                        default:
                            p = validAmd ? _nop11 : _nop9;
                            break;
                        }

                        i -= p.Length;
                        _buffer.EmitData(p);
                    } while (i != 0);

                    return;
                }

                if (Util.IsX86)
                {
                    // legacy NOPs, 0x90 with 0x66 prefix.
                    do
                    {
                        switch (i)
                        {
                        default:
                            EmitByte(0x66);
                            i--;
                            goto case 3;

                        case 3:
                            EmitByte(0x66);
                            i--;
                            goto case 2;

                        case 2:
                            EmitByte(0x66);
                            i--;
                            goto case 1;

                        case 1:
                            EmitByte(0x90);
                            i--;
                            break;
                        }
                    } while (i != 0);
                }
            }

            // legacy NOPs, only 0x90
            // In 64-bit mode, we can't use 0x66 prefix
            do
            {
                EmitByte(0x90);
            } while (--i != 0);
        }

        public LabelLink NewLabelLink()
        {
            LabelLink link = _unusedLinks;

            if (link != null)
            {
                _unusedLinks = link.Previous;
            }
            else
            {
                link = new LabelLink();
            }

            // clean up
            link.Previous = null;
            link.Offset = 0;
            link.Displacement = 0;
            link.RelocId = -1;

            return link;
        }

        private static readonly InstructionCode[] _jcctable =
            {
                InstructionCode.Jo,
                InstructionCode.Jno,
                InstructionCode.Jb,
                InstructionCode.Jae,
                InstructionCode.Je,
                InstructionCode.Jne,
                InstructionCode.Jbe,
                InstructionCode.Ja,
                InstructionCode.Js,
                InstructionCode.Jns,
                InstructionCode.Jpe,
                InstructionCode.Jpo,
                InstructionCode.Jl,
                InstructionCode.Jge,
                InstructionCode.Jle,
                InstructionCode.Jg
            };

        private static readonly InstructionCode[] _cmovcctable =
            {
                InstructionCode.Cmovo,
                InstructionCode.Cmovno,
                InstructionCode.Cmovb,
                InstructionCode.Cmovae,
                InstructionCode.Cmove,
                InstructionCode.Cmovne,
                InstructionCode.Cmovbe,
                InstructionCode.Cmova,
                InstructionCode.Cmovs,
                InstructionCode.Cmovns,
                InstructionCode.Cmovpe,
                InstructionCode.Cmovpo,
                InstructionCode.Cmovl,
                InstructionCode.Cmovge,
                InstructionCode.Cmovle,
                InstructionCode.Cmovg
            };

        private static readonly InstructionCode[] _setcctable =
            {
                InstructionCode.Seto,
                InstructionCode.Setno,
                InstructionCode.Setb,
                InstructionCode.Setae,
                InstructionCode.Sete,
                InstructionCode.Setne,
                InstructionCode.Setbe,
                InstructionCode.Seta,
                InstructionCode.Sets,
                InstructionCode.Setns,
                InstructionCode.Setpe,
                InstructionCode.Setpo,
                InstructionCode.Setl,
                InstructionCode.Setge,
                InstructionCode.Setle,
                InstructionCode.Setg
            };

        internal static InstructionCode ConditionToJump(Condition condition)
        {
            Contract.Requires(condition >= 0 && condition <= (Condition)0xF);

            return _jcctable[(int)condition];
        }

        internal static InstructionCode ConditionToMovCC(Condition condition)
        {
            Contract.Requires(condition >= 0 && condition <= (Condition)0xF);

            return _cmovcctable[(int)condition];
        }

        internal static InstructionCode ConditionToSetCC(Condition condition)
        {
            Contract.Requires(condition >= 0 && condition <= (Condition)0xF);

            return _setcctable[(int)condition];
        }

        #region Instructions

        public void Call(GPReg dst)
        {
            Contract.Requires(dst != null);

            if (!dst.IsRegType(Register.NativeRegisterType))
                throw new ArgumentException();

            EmitInstruction(InstructionCode.Call, dst);
        }

        public void Call(Mem dst)
        {
            Contract.Requires(dst != null);

            EmitInstruction(InstructionCode.Call, dst);
        }

        public void Call(Imm dst)
        {
            Contract.Requires(dst != null);

            EmitInstruction(InstructionCode.Call, dst);
        }

        public void Call(IntPtr dst)
        {
            EmitInstruction(InstructionCode.Call, (Imm)dst);
        }

        public void Call(Label dst)
        {
            Contract.Requires(dst != null);

            EmitInstruction(InstructionCode.Call, dst);
        }

        public void ShortJ(Condition cc, Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            _emitOptions |= EmitOptions.ShortJump;
            this.J(cc, label, hint);
        }

        public void ShortJa(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Ja, label, hint);
        }

        public void ShortJae(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jae, label, hint);
        }

        public void ShortJb(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jb, label, hint);
        }

        public void ShortJbe(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jbe, label, hint);
        }

        public void ShortJc(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jc, label, hint);
        }

        public void ShortJe(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Je, label, hint);
        }

        public void ShortJg(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jg, label, hint);
        }

        public void ShortJge(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jge, label, hint);
        }

        public void ShortJl(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jl, label, hint);
        }

        public void ShortJle(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jle, label, hint);
        }

        public void ShortJna(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jna, label, hint);
        }

        public void ShortJnae(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jnae, label, hint);
        }

        public void ShortJnb(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jnb, label, hint);
        }

        public void ShortJnbe(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jnbe, label, hint);
        }

        public void ShortJnc(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jnc, label, hint);
        }

        public void ShortJne(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jne, label, hint);
        }

        public void ShortJng(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jng, label, hint);
        }

        public void ShortJnge(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jnge, label, hint);
        }

        public void ShortJnl(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jnl, label, hint);
        }

        public void ShortJnle(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jnle, label, hint);
        }

        public void ShortJno(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jno, label, hint);
        }

        public void ShortJnp(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jnp, label, hint);
        }

        public void ShortJns(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jns, label, hint);
        }

        public void ShortJnz(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jnz, label, hint);
        }

        public void ShortJo(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jo, label, hint);
        }

        public void ShortJp(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jp, label, hint);
        }

        public void ShortJpe(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jpe, label, hint);
        }

        public void ShortJpo(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jpo, label, hint);
        }

        public void ShortJs(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Js, label, hint);
        }

        public void ShortJz(Label label, Hint hint = Hint.None)
        {
            Contract.Requires(label != null);

            EmitShortJcc(InstructionCode.Jz, label, hint);
        }

        public void ShortJmp(Label label)
        {
            Contract.Requires(label != null);

            _emitOptions |= EmitOptions.ShortJump;
            EmitInstruction(InstructionCode.Jmp, label);
        }

        public void Ret()
        {
            EmitInstruction(InstructionCode.Ret);
        }

        public void Ret(Imm imm16)
        {
            Contract.Requires(imm16 != null);

            EmitInstruction(InstructionCode.Ret, imm16);
        }

        #endregion

        public void Lock()
        {
            _emitOptions |= EmitOptions.LockPrefix;
        }

        public void Rex()
        {
            _emitOptions |= EmitOptions.RexPrefix;
        }

        public sealed class LabelLink
        {
            public LabelLink Previous
            {
                get;
                set;
            }

            public int Offset
            {
                get;
                set;
            }

            public int Displacement
            {
                get;
                set;
            }

            public int RelocId
            {
                get;
                set;
            }
        }

        public sealed class AssemblerLabelData
        {
            public AssemblerLabelData()
            {
                Offset = -1;
            }

            public int Offset
            {
                get;
                set;
            }

            public LabelLink Links
            {
                get;
                set;
            }
        }

        public void EmitInstruction(InstructionCode code)
        {
            EmitInstructionImpl(code, null, null, null);
        }

        public void EmitInstruction(InstructionCode code, Operand operand0)
        {
            if (operand0 == null)
                throw new ArgumentNullException("operand0");

            EmitInstructionImpl(code, operand0, null, null);
        }

        public void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1)
        {
            if (operand0 == null)
                throw new ArgumentNullException("operand0");
            if (operand1 == null)
                throw new ArgumentNullException("operand1");

            EmitInstructionImpl(code, operand0, operand1, null);
        }

        public void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1, Operand operand2)
        {
            if (operand0 == null)
                throw new ArgumentNullException("operand0");
            if (operand1 == null)
                throw new ArgumentNullException("operand1");
            if (operand2 == null)
                throw new ArgumentNullException("operand2");

            EmitInstructionImpl(code, operand0, operand1, operand2);
        }

        private static readonly int _instructionCount = Enum.GetValues(typeof(InstructionCode)).Cast<int>().Max() + 1;

        private static readonly RegData[] _patchedHiRegs =
            {
                new RegData(OperandType.Reg, 1, Operand.InvalidValue, (int)RegType.GPB_LO | 4),
                new RegData(OperandType.Reg, 1, Operand.InvalidValue, (int)RegType.GPB_LO | 5),
                new RegData(OperandType.Reg, 1, Operand.InvalidValue, (int)RegType.GPB_LO | 6),
                new RegData(OperandType.Reg, 1, Operand.InvalidValue, (int)RegType.GPB_LO | 7),
            };

        internal void EmitInstructionImpl(InstructionCode code, Operand o0, Operand o1, Operand o2)
        {
            Operand[] loggerOperands = new Operand[3];

            int bLoHiUsed = 0;
            bool forceRexPrefix = (_emitOptions & EmitOptions.RexPrefix) != 0;
            RegType memRegType = Register.NativeRegisterType;
            int bufferStartOffset = _buffer.Offset;

            Imm immOperand = null;
            int immSize = 0;

            // Convert operands to OPERAND_NONE if needed.
            if (o0 == null)
            {
                o0 = Operand.None;
            }
            else if (o0.IsReg)
            {
                bLoHiUsed |= (int)((BaseReg)o0).Code & (int)(RegType.GPB_LO | RegType.GPB_HI);
            }

            if (o1 == null)
            {
                o1 = Operand.None;
            }
            else if (o1.IsReg)
            {
                bLoHiUsed |= (int)((BaseReg)o1).Code & (int)(RegType.GPB_LO | RegType.GPB_HI);
            }

            if (o2 == null)
            {
                o2 = Operand.None;
            }
            else if (o2.IsReg)
            {
                bLoHiUsed |= (int)((BaseReg)o2).Code & (int)(RegType.GPB_LO | RegType.GPB_HI);
            }

            long beginOffset = Offset;
            InstructionDescription id = InstructionDescription.FromInstruction(code);

            if ((int)code >= _instructionCount)
            {
                throw new AssemblerException(string.Format("Unknown instruction '{0}'", code));
            }

            // Check if register operand is BPL, SPL, SIL, DIL and do action that depends
            // to current mode:
            //   - 64-bit: - Force rex prefix.
            //
            // Check if register operand is AH, BH, CH or DH and do action that depends
            // to current mode:
            //   - 32-bit: - Patch operand index (index += 4), because we are using
            //               different index what is used in opcode.
            //   - 64-bit: - Check whether there is REX prefix and raise error if it is.
            //             - Do the same as in 32-bit mode - patch register index.
            //
            // NOTE: This is a hit hacky, but I added this to older code-base and I have
            // no energy to rewrite it. Maybe in future all of this can be cleaned up!
            if (bLoHiUsed != 0 || forceRexPrefix)
            {
                loggerOperands[0] = o0;
                loggerOperands[1] = o1;
                loggerOperands[2] = o2;

                if (Util.IsX64)
                {
                    // Check if there is register that makes this instruction un-encodable.
                    forceRexPrefix |= o0.IsExtendedRegisterUsed;
                    forceRexPrefix |= o1.IsExtendedRegisterUsed;
                    forceRexPrefix |= o2.IsExtendedRegisterUsed;

                    if (o0.IsRegType(RegType.GPB_LO) && ((int)((BaseReg)o0).Code & (int)RegIndex.Mask) >= 4)
                        forceRexPrefix = true;
                    else if (o1.IsRegType(RegType.GPB_LO) && ((int)((BaseReg)o1).Code & (int)RegIndex.Mask) >= 4)
                        forceRexPrefix = true;
                    else if (o2.IsRegType(RegType.GPB_LO) && ((int)((BaseReg)o2).Code & (int)RegIndex.Mask) >= 4)
                        forceRexPrefix = true;

                    if ((bLoHiUsed & (int)RegType.GPB_HI) != 0 && forceRexPrefix)
                    {
                        throw NewIllegalInstructionException();
                    }
                }

                // Patch GPB.HI operand index.
                if ((bLoHiUsed & (int)RegType.GPB_HI) != 0)
                {
                    throw new NotImplementedException();
                    //if (o0.IsRegType(RegType.GPB_HI))
                    //    o0 = (Operand)(_patchedHiRegs[((BaseReg)o0).Code & (int)RegIndex.Mask]);
                    //if (o1.IsRegType(RegType.GPB_HI))
                    //    o1 = (Operand)(_patchedHiRegs[((BaseReg)o1).Code & (int)RegIndex.Mask]);
                    //if (o2.IsRegType(RegType.GPB_HI))
                    //    o2 = (Operand)(_patchedHiRegs[((BaseReg)o2).Code & (int)RegIndex.Mask]);
                }
            }

            // Check for buffer space (and grow if needed).
            if (!CanEmit())
                return;

            if ((_emitOptions & EmitOptions.LockPrefix) != 0)
            {
                if (!id.IsLockable)
                    throw NewIllegalInstructionException();

                EmitByte(0xF0);
            }

            switch (id.Group)
            {
            case InstructionGroup.EMIT:
                {
                    EmitOpCode(id.OpCode0);
                    goto end;
                }

            case InstructionGroup.ALU:
                {
                    int opCode = id.OpCode0;
                    byte opReg = (byte)id.OpCodeR;

                    // Mem <- Reg
                    if (o0.IsMem && o1.IsReg)
                    {
                        EmitX86RM(opCode + (o1.Size != 1 ? 1 : 0),
                          o1.Size == 2,
                          o1.Size == 8,
                          (byte)((GPReg)o1).Code,
                          o0,
                          0, forceRexPrefix);
                        goto end;
                    }

                    // Reg <- Reg|Mem
                    if (o0.IsReg && o1.IsRegMem)
                    {
                        EmitX86RM(opCode + 2 + (o0.Size != 1 ? 1 : 0),
                          o0.Size == 2,
                          o0.Size == 8,
                          (byte)((GPReg)o0).Code,
                          o1,
                          0, forceRexPrefix);
                        goto end;
                    }

                    // AL, AX, EAX, RAX register shortcuts
                    if (o0.IsRegIndex(0) && o1.IsImm)
                    {
                        if (o0.Size == 2)
                            EmitByte(0x66); // 16 bit
                        else if (o0.Size == 8)
                            EmitByte(0x48); // REX.W

                        EmitByte((byte)((opReg << 3) | (0x04 + (o0.Size != 1 ? 1 : 0))));
                        immOperand = (Imm)o1;
                        immSize = o0.Size <= 4 ? o0.Size : 4;
                        goto emitImmediate;
                    }

                    if (o0.IsRegMem && o1.IsImm)
                    {
                        Imm imm = (Imm)o1;
                        immSize = Util.IsInt8(imm.Value.ToInt64()) ? (byte)1 : (o0.Size <= 4 ? o0.Size : (byte)4);

                        EmitX86RM(id.OpCode1 + (o0.Size != 1 ? (immSize != 1 ? 1 : 3) : 0),
                          o0.Size == 2,
                          o0.Size == 8,
                          opReg, o0,
                          (byte)immSize, forceRexPrefix);
                        immOperand = (Imm)imm;
                        goto emitImmediate;
                    }

                    break;
                }

            case InstructionGroup.BSWAP:
                {
                    if (o0.IsReg)
                    {
                        GPReg dst = ((GPReg)o0);

                        if (Util.IsX64)
                            EmitRexR(dst.RegisterType == RegType.GPQ, 1, (byte)dst.Code, forceRexPrefix);

                        EmitByte(0x0F);
                        EmitModR(1, (byte)dst.Code);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.BT:
                {
                    if (o0.IsRegMem && o1.IsReg)
                    {
                        Operand dst = o0;
                        GPReg src = (GPReg)o1;

                        EmitX86RM(id.OpCode0,
                          src.IsRegType(RegType.GPW),
                          src.IsRegType(RegType.GPQ),
                          (byte)src.Code,
                          dst,
                          0, forceRexPrefix);
                        goto end;
                    }

                    if (o0.IsRegMem && o1.IsImm)
                    {
                        Operand dst = o0;
                        Imm src = (Imm)o1;

                        EmitX86RM(id.OpCode1,
                          dst.Size == 2,
                          dst.Size == 8,
                          (byte)id.OpCodeR,
                          dst,
                          1, forceRexPrefix);
                        immOperand = src;
                        immSize = 1;
                        goto emitImmediate;
                    }

                    break;
                }

            case InstructionGroup.CALL:
                {
                    if (o0.IsRegTypeMem(Register.NativeRegisterType))
                    {
                        Operand dst = o0;
                        EmitX86RM(0xFF,
                          false,
                          false, 2, dst,
                          0, forceRexPrefix);
                        goto end;
                    }

                    if (o0.IsImm)
                    {
                        Imm imm = (Imm)o0;
                        EmitByte(0xE8);
                        EmitJmpOrCallReloc((int)InstructionGroup.CALL, imm.Value);
                        goto end;
                    }

                    if (o0.IsLabel)
                    {
                        AssemblerLabelData l_data = _labelData[((Label)o0).Id & Operand.OperandIdValueMask];

                        if (l_data.Offset != -1)
                        {
                            // Bound label.
                            const int rel32_size = 5;
                            long offs = l_data.Offset - Offset;

                            if (offs > 0)
                                throw new AssemblerException();

                            EmitByte(0xE8);
                            EmitInt32((int)(offs - rel32_size));
                        }
                        else
                        {
                            // Non-bound label.
                            EmitByte(0xE8);
                            EmitDisplacement(l_data, -4, 4);
                        }
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.CRC32:
                {
                    if (o0.IsReg && o1.IsRegMem)
                    {
                        GPReg dst = (GPReg)o0;
                        Operand src = o1;
                        if (dst.RegisterType != RegType.GPD && dst.RegisterType != RegType.GPQ)
                            throw new ArgumentException("Destination register operand is not the correct size.");

                        EmitX86RM(id.OpCode0 + (src.Size != 1 ? 1 : 0),
                          src.Size == 2,
                          dst.RegisterType == (RegType)8, (byte)dst.Code, src,
                          0, forceRexPrefix);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.ENTER:
                {
                    if (o0.IsImm && o1.IsImm)
                    {
                        EmitByte(0xC8);
                        EmitWord((ushort)((Imm)o2).Value);
                        EmitByte((byte)((Imm)o1).Value);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.IMUL:
                {
                    // 1 operand
                    if (o0.IsRegMem && o1.IsNone && o2.IsNone)
                    {
                        Operand src = o0;
                        EmitX86RM(0xF6 + (src.Size != 1 ? 1 : 0),
                          src.Size == 2,
                          src.Size == 8, 5, src,
                          0, forceRexPrefix);
                        goto end;
                    }
                    // 2 operands
                    else if (o0.IsReg && !o1.IsNone && o2.IsNone)
                    {
                        GPReg dst = (GPReg)o0;

                        if (dst.IsRegType(RegType.GPW))
                            throw new ArgumentException("The destination register type is not supported for this instruction.");

                        if (o1.IsRegMem)
                        {
                            Operand src = o1;

                            EmitX86RM(0x0FAF,
                              dst.IsRegType(RegType.GPW),
                              dst.IsRegType(RegType.GPQ), (byte)dst.Code, src,
                              0, forceRexPrefix);
                            goto end;
                        }
                        else if (o1.IsImm)
                        {
                            Imm imm = (Imm)o1;

                            if (Util.IsInt8(imm.Value.ToInt64()))
                            {
                                EmitX86RM(0x6B,
                                  dst.IsRegType(RegType.GPW),
                                  dst.IsRegType(RegType.GPQ), (byte)dst.Code, dst,
                                  1, forceRexPrefix);
                                immOperand = (Imm)imm;
                                immSize = 1;
                                goto emitImmediate;
                            }
                            else
                            {
                                immSize = dst.IsRegType(RegType.GPW) ? 2 : 4;
                                EmitX86RM(0x69,
                                  dst.IsRegType(RegType.GPW),
                                  dst.IsRegType(RegType.GPQ), (byte)dst.Code, dst,
                                  immSize, forceRexPrefix);
                                immOperand = (Imm)imm;
                                goto emitImmediate;
                            }
                        }
                    }
                    // 3 operands
                    else if (o0.IsReg && o1.IsRegMem && o2.IsImm)
                    {
                        GPReg dst = (GPReg)o0;
                        Operand src = o1;
                        Imm imm = (Imm)o2;

                        if (Util.IsInt8(imm.Value.ToInt64()))
                        {
                            EmitX86RM(0x6B,
                              dst.IsRegType(RegType.GPW),
                              dst.IsRegType(RegType.GPQ), (byte)dst.Code, src,
                              1, forceRexPrefix);
                            immOperand = (Imm)imm;
                            immSize = 1;
                            goto emitImmediate;
                        }
                        else
                        {
                            immSize = dst.IsRegType(RegType.GPW) ? 2 : 4;
                            EmitX86RM(0x69,
                              dst.IsRegType(RegType.GPW),
                              dst.IsRegType(RegType.GPQ), (byte)dst.Code, src,
                              immSize, forceRexPrefix);
                            immOperand = (Imm)imm;
                            goto emitImmediate;
                        }
                    }

                    break;
                }

            case InstructionGroup.INC_DEC:
                {
                    if (o0.IsRegMem)
                    {
                        Operand dst = o0;

                        // INC [r16|r32] in 64 bit mode is not encodable.
                        if (Util.IsX86)
                        {
                            if ((dst.IsReg) && (dst.IsRegType(RegType.GPW) || dst.IsRegType(RegType.GPD)))
                            {
                                EmitX86Inl(id.OpCode0,
                                  dst.IsRegType(RegType.GPW),
                                  false, (byte)((BaseReg)dst).Code,
                                  false);
                                goto end;
                            }
                        }

                        EmitX86RM(id.OpCode1 + (dst.Size != 1 ? 1 : 0),
                          dst.Size == 2,
                          dst.Size == 8, (byte)id.OpCodeR, dst,
                          0, forceRexPrefix);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.J:
                {
                    if (o0.IsImm)
                    {
                        throw new NotImplementedException();
                    }

                    if (o0.IsLabel)
                    {
                        AssemblerLabelData l_data = _labelData[((Label)o0).Id & Operand.OperandIdValueMask];

                        int hint = o1.IsImm ? (int)((Imm)o1).Value : 0;
                        bool isShortJump = _emitOptions.HasFlag(EmitOptions.ShortJump);

                        // Emit jump hint if configured for that.
                        if ((hint & (int)(Hint.Taken | Hint.NotTaken)) != 0 && (_properties & CompilerProperties.JumpHints) != 0)
                        {
                            if ((hint & (int)Hint.Taken) != 0)
                                EmitByte(HintByteValue.Taken);
                            else if ((hint & (int)Hint.NotTaken) != 0)
                                EmitByte(HintByteValue.NotTaken);
                            else
                                throw new AssemblerException("Invalid jump hint.");
                        }

                        if (l_data.Offset != -1)
                        {
                            // Bound label.
                            const int rel8_size = 2;
                            const int rel32_size = 6;
                            long offs = l_data.Offset - Offset;

                            if (offs > 0)
                                throw new AssemblerException();

                            if (Util.IsInt8(offs - rel8_size))
                            {
                                EmitByte((byte)(0x70 | id.OpCode0));
                                EmitByte((byte)(sbyte)(offs - rel8_size));

                                // Change the emit options so logger can log instruction correctly.
                                _emitOptions |= EmitOptions.ShortJump;
                            }
                            else
                            {
                                if (isShortJump && Logger != null)
                                {
                                    Logger.LogString("*** ASSEMBLER WARNING: Emitting long conditional jump, but short jump instruction forced!" + Environment.NewLine);
                                    _emitOptions &= ~EmitOptions.ShortJump;
                                }

                                EmitByte(0x0F);
                                EmitByte((byte)(0x80 | id.OpCode0));
                                EmitInt32((int)(offs - rel32_size));
                            }
                        }
                        else
                        {
                            // Non-bound label.
                            if (isShortJump)
                            {
                                EmitByte((byte)(0x70 | id.OpCode0));
                                EmitDisplacement(l_data, -1, 1);
                            }
                            else
                            {
                                EmitByte(0x0F);
                                EmitByte((byte)(0x80 | id.OpCode0));
                                EmitDisplacement(l_data, -4, 4);
                            }
                        }
                        goto end;
                    }

                    throw new NotImplementedException();
                }

            case InstructionGroup.JMP:
                {
                    if (o0.IsRegMem)
                    {
                        Operand dst = o0;

                        EmitX86RM(0xFF,
                          false,
                          false, 4, dst,
                          0, forceRexPrefix);
                        goto end;
                    }

                    if (o0.IsImm)
                    {
                        Imm imm = (Imm)o0;
                        EmitByte(0xE9);
                        EmitJmpOrCallReloc((int)InstructionGroup.JMP, imm.Value);
                        goto end;
                    }

                    if (o0.IsLabel)
                    {
                        AssemblerLabelData l_data = _labelData[((Label)o0).Id & Operand.OperandIdValueMask];
                        bool isShortJump = _emitOptions.HasFlag(EmitOptions.ShortJump);

                        if (l_data.Offset != -1)
                        {
                            // Bound label.
                            const int rel8_size = 2;
                            const int rel32_size = 5;
                            long offs = l_data.Offset - Offset;

                            if (Util.IsInt8(offs - rel8_size))
                            {
                                EmitByte(0xEB);
                                EmitByte((byte)(sbyte)(offs - rel8_size));

                                // Change the emit options so logger can log instruction correctly.
                                _emitOptions |= EmitOptions.ShortJump;
                            }
                            else
                            {
                                if (isShortJump)
                                {
                                    if (Logger != null)
                                    {
                                        Logger.LogString("*** ASSEMBLER WARNING: Emitting long jump, but short jump instruction forced!" + Environment.NewLine);
                                        _emitOptions &= ~EmitOptions.ShortJump;
                                    }
                                }

                                EmitByte(0xE9);
                                EmitInt32((int)(offs - rel32_size));
                            }
                        }
                        else
                        {
                            // Non-bound label.
                            if (isShortJump)
                            {
                                EmitByte(0xEB);
                                EmitDisplacement(l_data, -1, 1);
                            }
                            else
                            {
                                EmitByte(0xE9);
                                EmitDisplacement(l_data, -4, 4);
                            }
                        }
                        goto end;
                    }

                    throw new NotImplementedException();
                }

            case InstructionGroup.LEA:
                {
                    if (o0.IsReg && o1.IsMem)
                    {
                        GPReg dst = (GPReg)o0;
                        Mem src = (Mem)o1;

                        // Size override prefix support
                        if (src.HasSizePrefix)
                        {
                            EmitByte(0x67);
                            if (Util.IsX86)
                                memRegType = RegType.GPW;
                            else
                                memRegType = RegType.GPD;
                        }

                        EmitX86RM(0x8D,
                          dst.IsRegType(RegType.GPW),
                          dst.IsRegType(RegType.GPQ), (byte)dst.Code, src,
                          0, forceRexPrefix);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.M:
                {
                    if (o0.IsMem)
                    {
                        EmitX86RM(id.OpCode0, false, (byte)id.OpCode1 != 0, (byte)id.OpCodeR, (Mem)o0, 0, forceRexPrefix);
                        goto end;
                    }
                    break;
                }

            case InstructionGroup.MOV:
                {
                    Operand dst = o0;
                    Operand src = o1;

                    switch ((int)dst.OperandType << 4 | (int)src.OperandType)
                    {
                    // Reg <- Reg/Mem
                    case ((int)OperandType.Reg << 4) | (int)OperandType.Reg:
                        {
                            // Reg <- Sreg
                            if (src.IsRegType(RegType.Segment))
                            {
                                if (!dst.IsRegType(RegType.GPW) &&
                                    !dst.IsRegType(RegType.GPD) &&
                                    !dst.IsRegType(RegType.GPQ))
                                {
                                    throw new AssemblerException();
                                }

                                EmitX86RM(0x8C,
                                    dst.Size == 2,
                                    dst.Size == 8,
                                    (byte)((SegmentReg)src).RegisterIndex,
                                    (Operand)dst,
                                    0, forceRexPrefix);
                                goto end;
                            }

                            // Sreg <- Reg/Mem
                            if (dst.IsRegType(RegType.Segment))
                            {
                                if (!src.IsRegType(RegType.GPW) &&
                                    !src.IsRegType(RegType.GPD) &&
                                    !src.IsRegType(RegType.GPQ))
                                {
                                    throw new AssemblerException();
                                }

                                EmitMovSregRM((SegmentReg)src, forceRexPrefix);
                                goto end;
                            }

                            if (!src.IsRegType(RegType.GPB_LO)
                                && !src.IsRegType(RegType.GPB_HI)
                                && !src.IsRegType(RegType.GPW)
                                && !src.IsRegType(RegType.GPD)
                                && !src.IsRegType(RegType.GPQ))
                            {
                                throw new AssemblerException();
                            }

                            // ... fall through ...
                            goto case ((int)OperandType.Reg << 4) | (int)OperandType.Mem;
                        }

                    case ((int)OperandType.Reg << 4) | (int)OperandType.Mem:
                        {
                            // Sreg <- Mem
                            if (dst.IsRegType(RegType.Segment))
                            {
                                EmitMovSregRM((SegmentReg)src, forceRexPrefix);
                                goto end;
                            }

                            if (!dst.IsRegType(RegType.GPB_LO)
                                && !dst.IsRegType(RegType.GPB_HI)
                                && !dst.IsRegType(RegType.GPW)
                                && !dst.IsRegType(RegType.GPD)
                                && !dst.IsRegType(RegType.GPQ))
                            {
                                throw new AssemblerException();
                            }

                            EmitX86RM(0x0000008A + (dst.Size != 1 ? 1 : 0),
                              dst.IsRegType(RegType.GPW),
                              dst.IsRegType(RegType.GPQ),
                              (byte)((GPReg)dst).Code,
                              src,
                              0, forceRexPrefix);
                            goto end;
                        }

                    // Reg <- Imm
                    case ((int)OperandType.Reg << 4) | (int)OperandType.Imm:
                        {
                            //GPReg dst = ((GPReg)o0);
                            //Imm src = ((Imm)o1);

                            // In 64-bit mode the immediate can be 64-bits long if the
                            // destination operand type is register (otherwise 32-bits).
                            immSize = dst.Size;

                            // Optimize instruction size by using 32 bit immediate if value can
                            // fit into it.
                            if (Util.IsX64 && immSize == 8 && Util.IsInt32(((Imm)src).Value.ToInt64()))
                            {
                                EmitX86RM(0xC7,
                                  false, // 16 bit
                                  true, // rex.w
                                  0, // O
                                  dst,
                                  0, forceRexPrefix);
                                immSize = 4;
                            }
                            else
                            {
                                EmitX86Inl((dst.Size == 1 ? 0xB0 : 0xB8),
                                  dst.IsRegType(RegType.GPW),
                                  dst.IsRegType(RegType.GPQ),
                                  (byte)((GPReg)dst).Code, forceRexPrefix);
                            }

                            immOperand = (Imm)src;
                            goto emitImmediate;
                        }

                    // Mem <- Reg/Sreg
                    case ((int)OperandType.Mem << 4) | (int)OperandType.Reg:
                        {
                            if (src.IsRegType(RegType.Segment))
                            {
                                // Mem <- Sreg
                                EmitX86RM(0x8C,
                                    dst.Size == 2,
                                    dst.Size == 8,
                                    (byte)((SegmentReg)src).RegisterIndex,
                                    (Operand)dst,
                                    0, forceRexPrefix);
                                goto end;
                            }
                            else
                            {
                                // Mem <- Reg
                                if (!src.IsRegType(RegType.GPB_LO)
                                    && !src.IsRegType(RegType.GPB_HI)
                                    && !src.IsRegType(RegType.GPW)
                                    && !src.IsRegType(RegType.GPD)
                                    && !src.IsRegType(RegType.GPQ))
                                {
                                    throw new AssemblerException();
                                }

                                EmitX86RM(0x88 + (src.Size != 1 ? 1 : 0),
                                  src.IsRegType(RegType.GPW),
                                  src.IsRegType(RegType.GPQ),
                                  (byte)((GPReg)src).Code,
                                  dst,
                                  0, forceRexPrefix);
                                goto end;
                            }
                        }

                    // Mem <- Imm
                    case ((int)OperandType.Mem << 4) | (int)OperandType.Imm:
                        {
                            immSize = dst.Size <= 4 ? dst.Size : 4;

                            EmitX86RM(0xC6 + (dst.Size != 1 ? 1 : 0),
                              dst.Size == 2,
                              dst.Size == 8,
                              0,
                              dst,
                              immSize, forceRexPrefix);
                            immOperand = (Imm)src;
                            goto emitImmediate;
                        }
                    }

                    break;
                }

            case InstructionGroup.MOV_PTR:
                {
                    if ((o0.IsReg && o1.IsImm) || (o0.IsImm && o1.IsReg))
                    {
                        bool reverse = o1.OperandType == OperandType.Reg;
                        byte opCode = !reverse ? (byte)0xA0 : (byte)0xA2;
                        GPReg reg = (GPReg)(!reverse ? o0 : o1);
                        Imm imm = (Imm)(!reverse ? o1 : o0);

                        if (reg.RegisterIndex != 0)
                            throw NewIllegalInstructionException();

                        if (reg.IsRegType(RegType.GPW))
                            EmitByte(0x66);

                        if (Util.IsX64)
                            EmitRexR(reg.Size == 8, 0, 0, forceRexPrefix);

                        EmitByte((byte)(opCode + (reg.Size != 1 ? 1 : 0)));
                        immOperand = (Imm)imm;
                        immSize = IntPtr.Size;
                        goto emitImmediate;
                    }

                    break;
                }

            case InstructionGroup.MOVSX_MOVZX:
                {
                    if (o0.IsReg && o1.IsRegMem)
                    {
                        GPReg dst = ((GPReg)o0);
                        Operand src = o1;

                        if (dst.Size == 1)
                            throw NewIllegalInstructionException();
                        if (src.Size != 1 && src.Size != 2)
                            throw NewIllegalInstructionException();
                        if (src.Size == 2 && dst.Size == 2)
                            throw NewIllegalInstructionException();

                        EmitX86RM(id.OpCode0 + (src.Size != 1 ? 1 : 0),
                          dst.IsRegType(RegType.GPW),
                          dst.IsRegType(RegType.GPQ),
                          (byte)dst.Code,
                          src,
                          0, forceRexPrefix);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.MOVSXD:
                {
                    if (!Util.IsX64)
                        throw new AssemblerException(string.Format("The '{0}' instruction group is only supported on X64", InstructionGroup.MOVSXD));

                    if (o0.IsReg && o1.IsRegMem)
                    {
                        GPReg dst = (GPReg)o0;
                        Operand src = o1;
                        EmitX86RM(0x00000063,
                          false,
                          true, (byte)dst.Code, src,
                          0, forceRexPrefix);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.PUSH:
                {
                    // This section is only for immediates, memory/register operands are handled in InstructionGroup.POP.
                    if (o0.IsImm)
                    {
                        Imm imm = (Imm)o0;

                        if (Util.IsInt8(imm.Value.ToInt64()))
                        {
                            EmitByte(0x6A);
                            immOperand = (Imm)imm;
                            immSize = 1;
                            goto emitImmediate;
                        }
                        else
                        {
                            EmitByte(0x68);
                            immOperand = (Imm)imm;
                            immSize = 4;
                            goto emitImmediate;
                        }
                    }

                    // ... goto I_POP ...
                    goto case InstructionGroup.POP;
                }

            case InstructionGroup.POP:
                {
                    if (o0.IsReg)
                    {
                        if (!o0.IsRegType(RegType.GPW) && !o0.IsRegType(Register.NativeRegisterType))
                            throw new AssemblerException();

                        EmitX86Inl(id.OpCode0, o0.IsRegType(RegType.GPW), false, (byte)((GPReg)o0).Code, forceRexPrefix);
                        goto end;
                    }

                    if (o0.IsMem)
                    {
                        EmitX86RM(id.OpCode1, o0.Size == 2, false, (byte)id.OpCodeR, o0, 0, forceRexPrefix);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.R_RM:
                {
                    if (o0.IsReg && o1.IsRegMem)
                    {
                        GPReg dst = (GPReg)o0;
                        Operand src = o1;
                        if (dst.Size == 1)
                            throw new AssemblerException();

                        EmitX86RM(id.OpCode0,
                          dst.RegisterType == RegType.GPW,
                          dst.RegisterType == RegType.GPQ, (byte)dst.Code, src,
                          0, forceRexPrefix);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.RM_B:
                {
                    if (o0.IsRegMem)
                    {
                        Operand op = o0;

                        // Only BYTE register or BYTE/TYPELESS memory location can be used.
                        Contract.Assert(op.Size <= 1);

                        EmitX86RM(id.OpCode0, false, false, 0, op, 0, forceRexPrefix);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.RM:
                {
                    if (o0.IsRegMem)
                    {
                        Operand op = o0;
                        EmitX86RM(id.OpCode0 + (op.Size != 1 ? 1 : 0),
                          op.Size == 2,
                          op.Size == 8, (byte)id.OpCodeR, op,
                          0, forceRexPrefix);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.RM_R:
                {
                    if (o0.IsRegMem && o1.IsReg)
                    {
                        Operand dst = o0;
                        GPReg src = (GPReg)o1;
                        EmitX86RM(id.OpCode0 + (src.Size != 1 ? 1 : 0),
                          src.RegisterType == RegType.GPW,
                          src.RegisterType == RegType.GPQ, (byte)src.Code, dst,
                          0, forceRexPrefix);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.REP:
                {
                    int opCode = id.OpCode0;
                    int opSize = id.OpCode1;

                    // Emit REP prefix (1 BYTE).
                    EmitByte((byte)(opCode >> 24));

                    if (opSize != 1)
                    {
                        opCode++; // D, Q and W form.
                    }
                    if (opSize == 2)
                    {
                        EmitByte(0x66); // 16-bit prefix.
                    }
                    else if (opSize == 8)
                    {
                        if (!Util.IsX64)
                            throw new AssemblerException("8 byte operands are only supported on X64.");

                        EmitByte(0x48); // REX.W prefix.
                    }

                    // Emit opcode (1 BYTE).
                    EmitByte((byte)(opCode & 0xFF));
                    goto end;
                }

            case InstructionGroup.RET:
                {
                    if (o0.IsNone)
                    {
                        EmitByte(0xC3);
                        goto end;
                    }
                    else if (o0.IsImm)
                    {
                        Imm imm = (Imm)o0;
                        if (!Util.IsUInt16(imm.Value.ToInt64()))
                            throw new AssemblerException();

                        if (imm.Value == IntPtr.Zero)
                        {
                            EmitByte(0xC3);
                        }
                        else
                        {
                            EmitByte(0xC2);
                            immOperand = (Imm)imm;
                            immSize = 2;
                            goto emitImmediate;
                        }
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.ROT:
                {
                    if (o0.IsRegMem && (o1.IsRegCode(RegCode.CL) || o1.IsImm))
                    {
                        // generate opcode. For these operations is base 0xC0 or 0xD0.
                        bool useImm8 = o1.IsImm && ((Imm)o1).Value != (IntPtr)1;
                        int opCode = useImm8 ? 0xC0 : 0xD0;

                        // size and operand type modifies the opcode
                        if (o0.Size != 1)
                            opCode |= 0x01;
                        if (o1.OperandType == OperandType.Reg)
                            opCode |= 0x02;

                        EmitX86RM(opCode,
                          o0.Size == 2,
                          o0.Size == 8,
                          (byte)id.OpCodeR, o0,
                          useImm8 ? 1 : 0, forceRexPrefix);

                        if (useImm8)
                        {
                            immOperand = (Imm)o1;
                            immSize = 1;
                            goto emitImmediate;
                        }

                        goto end;
                    }

                    break;
                }

            case InstructionGroup.SHLD_SHRD:
                {
                    if (o0.IsRegMem && o1.IsReg && (o2.IsImm || (o2.IsReg && o2.IsRegCode(RegCode.CL))))
                    {
                        Operand dst = o0;
                        GPReg src1 = (GPReg)o1;
                        Operand src2 = o2;

                        if (dst.Size != src1.Size)
                            throw new AssemblerException();

                        EmitX86RM(id.OpCode0 + (src2.IsReg ? 1 : 0),
                          src1.IsRegType(RegType.GPW),
                          src1.IsRegType(RegType.GPQ),
                          (byte)src1.Code, dst,
                          src2.IsImm ? 1 : 0, forceRexPrefix);

                        if (src2.IsImm)
                        {
                            immOperand = (Imm)src2;
                            immSize = 1;
                            goto emitImmediate;
                        }

                        goto end;
                    }

                    break;
                }

            case InstructionGroup.TEST:
                {
                    if (o0.IsRegMem && o1.IsReg)
                    {
                        if (o0.Size != o1.Size)
                            throw new AssemblerException();

                        EmitX86RM(0x84 + (o1.Size != 1 ? 1 : 0),
                          o1.Size == 2, o1.Size == 8,
                          (byte)((BaseReg)o1).Code,
                          o0,
                          0, forceRexPrefix);
                        goto end;
                    }

                    if (o0.IsRegIndex(0) && o1.IsImm)
                    {
                        immSize = o0.Size <= 4 ? o0.Size : 4;

                        if (o0.Size == 2)
                            EmitByte(0x66); // 16 bit

                        if (Util.IsX64)
                            EmitRexRM(o0.Size == 8, 0, o0, forceRexPrefix);

                        EmitByte((byte)(0xA8 + (o0.Size != 1 ? 1 : 0)));
                        immOperand = (Imm)o1;
                        goto emitImmediate;
                    }

                    if (o0.IsRegMem && o1.IsImm)
                    {
                        immSize = o0.Size <= 4 ? o0.Size : 4;

                        if (o0.Size == 2)
                            EmitByte(0x66); // 16 bit
                        EmitSegmentPrefix(o0); // segment prefix

                        if (Util.IsX64)
                            EmitRexRM(o0.Size == 8, 0, o0, forceRexPrefix);

                        EmitByte((byte)(0xF6 + (o0.Size != 1 ? 1 : 0)));
                        EmitModRM(0, o0, immSize);
                        immOperand = (Imm)o1;
                        goto emitImmediate;
                    }

                    break;
                }

            case InstructionGroup.XCHG:
                {
                    if (o0.IsRegMem && o1.IsReg)
                    {
                        Operand dst = o0;
                        GPReg src = (GPReg)o1;

                        if (src.IsRegType(RegType.GPW))
                            EmitByte(0x66); // 16 bit
                        EmitSegmentPrefix(dst); // segment prefix

                        if (Util.IsX64)
                            EmitRexRM(src.IsRegType(RegType.GPQ), (byte)src.Code, dst, forceRexPrefix);

                        // Special opcode for index 0 registers (AX, EAX, RAX vs register)
                        if ((dst.OperandType == OperandType.Reg && dst.Size > 1) &&
                            (((GPReg)dst).Code == 0 ||
                             ((GPReg)src).Code == 0))
                        {
                            byte index = (byte)(((GPReg)dst).Code | src.Code);
                            EmitByte((byte)(0x90 + index));
                            goto end;
                        }

                        EmitByte((byte)(0x86 + (src.Size != 1 ? 1 : 0)));
                        EmitModRM((byte)src.Code, dst, 0);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.MOVBE:
                {
                    if (o0.IsReg && o1.IsMem)
                    {
                        EmitX86RM(0x000F38F0,
                          o0.IsRegType(RegType.GPW),
                          o0.IsRegType(RegType.GPQ),
                          (byte)((GPReg)o0).Code,
                          (Mem)o1,
                          0, forceRexPrefix);
                        goto end;
                    }

                    if (o0.IsMem && o1.IsReg)
                    {
                        EmitX86RM(0x000F38F1,
                          o1.IsRegType(RegType.GPW),
                          o1.IsRegType(RegType.GPQ),
                          (byte)((GPReg)o1).Code,
                          ((Mem)o0),
                          0, forceRexPrefix);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.X87_FPU:
                {
                    if (o0.IsRegType(RegType.X87))
                    {
                        RegIndex i1 = ((X87Reg)o0).RegisterIndex;
                        RegIndex i2 = 0;

                        if (code != InstructionCode.Fcom && code != InstructionCode.Fcomp)
                        {
                            if (!o1.IsRegType(RegType.X87))
                                throw NewIllegalInstructionException();
                            i2 = ((X87Reg)o1).RegisterIndex;
                        }
                        else if (i1 != 0 && i2 != 0)
                        {
                            throw NewIllegalInstructionException();
                        }

                        EmitByte(i1 == 0
                          ? (byte)((id.OpCode0 & 0xFF000000) >> 24)
                          : (byte)((id.OpCode0 & 0x00FF0000) >> 16));
                        EmitByte(i1 == 0
                          ? (byte)(((id.OpCode0 & 0x0000FF00) >> 8) + i2)
                          : (byte)(((id.OpCode0 & 0x000000FF)) + i1));
                        goto end;
                    }

                    if (o0.IsMem && (o0.Size == 4 || o0.Size == 8) && o1.IsNone)
                    {
                        Mem m = ((Mem)o0);

                        // segment prefix
                        EmitSegmentPrefix(m);

                        EmitByte(o0.Size == 4
                          ? (byte)((id.OpCode0 & 0xFF000000) >> 24)
                          : (byte)((id.OpCode0 & 0x00FF0000) >> 16));
                        EmitModM((byte)id.OpCodeR, m, 0);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.X87_STI:
                {
                    if (o0.IsRegType(RegType.X87))
                    {
                        RegIndex i = ((X87Reg)o0).RegisterIndex;
                        EmitByte((byte)((id.OpCode0 & 0x0000FF00) >> 8));
                        EmitByte((byte)((id.OpCode0 & 0x000000FF) + i));
                        goto end;
                    }
                    break;
                }

            case InstructionGroup.X87_FSTSW:
                {
                    if (o0.IsReg &&
                        ((BaseReg)o0).RegisterType <= RegType.GPQ &&
                        ((BaseReg)o0).RegisterIndex == 0)
                    {
                        EmitOpCode(id.OpCode1);
                        goto end;
                    }

                    if (o0.IsMem)
                    {
                        EmitX86RM(id.OpCode0, false, false, (byte)id.OpCodeR, ((Mem)o0), 0, forceRexPrefix);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.X87_MEM_STI:
                {
                    if (o0.IsRegType(RegType.X87))
                    {
                        EmitByte((byte)((id.OpCode1 & 0xFF000000) >> 24));
                        EmitByte((byte)(((id.OpCode1 & 0x00FF0000) >> 16) +
                          (byte)((X87Reg)o0).RegisterIndex));
                        goto end;
                    }

                    // ... fall through to InstructionGroup.X87_MEM ...
                    goto case InstructionGroup.X87_MEM;
                }

            case InstructionGroup.X87_MEM:
                {
                    if (!o0.IsMem)
                        throw NewIllegalInstructionException();
                    Mem m = ((Mem)o0);

                    byte opCode = 0x00, mod = 0;

                    if (o0.Size == 2 && (id.OperandFlags[0] & OperandFlags.FM_2) != 0)
                    {
                        opCode = (byte)((id.OpCode0 & 0xFF000000) >> 24);
                        mod = (byte)id.OpCodeR;
                    }
                    if (o0.Size == 4 && (id.OperandFlags[0] & OperandFlags.FM_4) != 0)
                    {
                        opCode = (byte)((id.OpCode0 & 0x00FF0000) >> 16);
                        mod = (byte)id.OpCodeR;
                    }
                    if (o0.Size == 8 && (id.OperandFlags[0] & OperandFlags.FM_8) != 0)
                    {
                        opCode = (byte)((id.OpCode0 & 0x0000FF00) >> 8);
                        mod = (byte)((id.OpCode0 & 0x000000FF));
                    }

                    if (opCode != 0)
                    {
                        EmitSegmentPrefix(m);
                        EmitByte(opCode);
                        EmitModM(mod, m, 0);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.MMU_MOV:
                {
                    if (id.OperandFlags[0] == OperandFlags.None || id.OperandFlags[1] == OperandFlags.None)
                        throw new AssemblerException();

                    // Check parameters (X)MM|GP32_64 <- (X)MM|GP32_64|Mem|Imm
                    if ((o0.IsMem && (id.OperandFlags[0] & OperandFlags.MEM) == 0) ||
                        (o0.IsRegType(RegType.MM) && (id.OperandFlags[0] & OperandFlags.MM) == 0) ||
                        (o0.IsRegType(RegType.XMM) && (id.OperandFlags[0] & OperandFlags.XMM) == 0) ||
                        (o0.IsRegType(RegType.GPD) && (id.OperandFlags[0] & OperandFlags.GD) == 0) ||
                        (o0.IsRegType(RegType.GPQ) && (id.OperandFlags[0] & OperandFlags.GQ) == 0) ||
                        (o1.IsRegType(RegType.MM) && (id.OperandFlags[1] & OperandFlags.MM) == 0) ||
                        (o1.IsRegType(RegType.XMM) && (id.OperandFlags[1] & OperandFlags.XMM) == 0) ||
                        (o1.IsRegType(RegType.GPD) && (id.OperandFlags[1] & OperandFlags.GD) == 0) ||
                        (o1.IsRegType(RegType.GPQ) && (id.OperandFlags[1] & OperandFlags.GQ) == 0) ||
                        (o1.IsMem && (id.OperandFlags[1] & OperandFlags.MEM) == 0))
                    {
                        throw NewIllegalInstructionException();
                    }

                    // Illegal.
                    if (o0.IsMem && o1.IsMem)
                        throw NewIllegalInstructionException();

                    bool rexw = ((id.OperandFlags[0] | id.OperandFlags[1]) & OperandFlags.NOREX) != 0
                      ? false
                      : (o0.IsRegType(RegType.GPQ) || o1.IsRegType(RegType.GPQ));

                    // (X)MM|Reg <- (X)MM|Reg
                    if (o0.IsReg && o1.IsReg)
                    {
                        EmitMmu((uint)id.OpCode0, rexw,
                          (byte)((BaseReg)o0).Code,
                          ((BaseReg)o1),
                          IntPtr.Zero);
                        goto end;
                    }

                    // (X)MM|Reg <- Mem
                    if (o0.IsReg && o1.IsMem)
                    {
                        EmitMmu((uint)id.OpCode0, rexw,
                          (byte)((BaseReg)o0).Code,
                          ((Mem)o1),
                          IntPtr.Zero);
                        goto end;
                    }

                    // Mem <- (X)MM|Reg
                    if (o0.IsMem && o1.IsReg)
                    {
                        EmitMmu((uint)id.OpCode1, rexw,
                          (byte)((BaseReg)o1).Code,
                          (Mem)o0,
                          IntPtr.Zero);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.MMU_MOVD:
                {
                    if ((o0.IsRegType(RegType.MM) || o0.IsRegType(RegType.XMM)) && (o1.IsRegType(RegType.GPD) || o1.IsMem))
                    {
                        EmitMmu(o0.IsRegType(RegType.XMM) ? 0x66000F6EU : 0x00000F6EU, false,
                          (byte)((BaseReg)o0).Code,
                          o1,
                          IntPtr.Zero);
                        goto end;
                    }

                    if ((o0.IsRegType(RegType.GPD) || o0.IsMem) && (o1.IsRegType(RegType.MM) || o1.IsRegType(RegType.XMM)))
                    {
                        EmitMmu(o1.IsRegType(RegType.XMM) ? 0x66000F7EU : 0x00000F7EU, false,
                          (byte)((BaseReg)o1).Code,
                          o0,
                          IntPtr.Zero);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.MMU_MOVQ:
                {
                    if (o0.IsRegType(RegType.MM) && o1.IsRegType(RegType.MM))
                    {
                        EmitMmu(0x00000F6F, false,
                          (byte)((MMReg)o0).Code,
                          ((MMReg)o1),
                          IntPtr.Zero);
                        goto end;
                    }

                    if (o0.IsRegType(RegType.XMM) && o1.IsRegType(RegType.XMM))
                    {
                        EmitMmu(0xF3000F7E, false,
                          (byte)((XMMReg)o0).Code,
                          ((XMMReg)o1),
                          IntPtr.Zero);
                        goto end;
                    }

                    // Convenience - movdq2q
                    if (o0.IsRegType(RegType.MM) && o1.IsRegType(RegType.XMM))
                    {
                        EmitMmu(0xF2000FD6, false,
                          (byte)((MMReg)o0).Code,
                          ((XMMReg)o1),
                          IntPtr.Zero);
                        goto end;
                    }

                    // Convenience - movq2dq
                    if (o0.IsRegType(RegType.XMM) && o1.IsRegType(RegType.MM))
                    {
                        EmitMmu(0xF3000FD6, false,
                          (byte)((XMMReg)o0).Code,
                          ((MMReg)o1),
                          IntPtr.Zero);
                        goto end;
                    }

                    if (o0.IsRegType(RegType.MM) && o1.IsMem)
                    {
                        EmitMmu(0x00000F6F, false,
                          (byte)((MMReg)o0).Code,
                          (Mem)o1,
                          IntPtr.Zero);
                        goto end;
                    }

                    if (o0.IsRegType(RegType.XMM) && o1.IsMem)
                    {
                        EmitMmu(0xF3000F7E, false,
                          (byte)((XMMReg)o0).Code,
                          ((Mem)o1),
                          IntPtr.Zero);
                        goto end;
                    }

                    if (o0.IsMem && o1.IsRegType(RegType.MM))
                    {
                        EmitMmu(0x00000F7F, false,
                          (byte)((MMReg)o1).Code,
                          ((Mem)o0),
                          IntPtr.Zero);
                        goto end;
                    }

                    if (o0.IsMem && o1.IsRegType(RegType.XMM))
                    {
                        EmitMmu(0x66000FD6, false,
                          (byte)((XMMReg)o1).Code,
                          ((Mem)o0),
                          IntPtr.Zero);
                        goto end;
                    }

                    if (Util.IsX64)
                    {
                        if ((o0.IsRegType(RegType.MM) || o0.IsRegType(RegType.XMM)) && (o1.IsRegType(RegType.GPQ) || o1.IsMem))
                        {
                            EmitMmu(o0.IsRegType(RegType.XMM) ? 0x66000F6EU : 0x00000F6EU, true,
                              (byte)((BaseReg)o0).Code,
                              o1,
                              IntPtr.Zero);
                            goto end;
                        }

                        if ((o0.IsRegType(RegType.GPQ) || o0.IsMem) && (o1.IsRegType(RegType.MM) || o1.IsRegType(RegType.XMM)))
                        {
                            EmitMmu(o1.IsRegType(RegType.XMM) ? 0x66000F7EU : 0x00000F7EU, true,
                              (byte)((BaseReg)o1).Code,
                              o0,
                              IntPtr.Zero);
                            goto end;
                        }
                    }

                    break;
                }

            case InstructionGroup.MMU_PREFETCH:
                {
                    if (o0.IsMem && o1.IsImm)
                    {
                        Mem mem = (Mem)o0;
                        Imm hint = (Imm)o1;

                        EmitMmu(0x00000F18U, false, (byte)hint.Value, mem, IntPtr.Zero);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.MMU_PEXTR:
                {
                    if (!(o0.IsRegMem &&
                         (o1.IsRegType(RegType.XMM) || (code == InstructionCode.Pextrw && o1.IsRegType(RegType.MM))) &&
                          o2.IsImm))
                    {
                        throw NewIllegalInstructionException();
                    }

                    int opCode = id.OpCode0;
                    bool isGpdGpq = o0.IsRegType(RegType.GPD) | o0.IsRegType(RegType.GPQ);

                    if (code == InstructionCode.Pextrb && (o0.Size != 0 && o0.Size != 1) && !isGpdGpq)
                        throw NewIllegalInstructionException();
                    if (code == InstructionCode.Pextrw && (o0.Size != 0 && o0.Size != 2) && !isGpdGpq)
                        throw NewIllegalInstructionException();
                    if (code == InstructionCode.Pextrd && (o0.Size != 0 && o0.Size != 4) && !isGpdGpq)
                        throw NewIllegalInstructionException();
                    if (code == InstructionCode.Pextrq && (o0.Size != 0 && o0.Size != 8) && !isGpdGpq)
                        throw NewIllegalInstructionException();

                    if (o1.IsRegType(RegType.XMM))
                        opCode |= 0x66000000;

                    if (o0.IsReg)
                    {
                        EmitMmu((uint)opCode, id.OpCodeR != 0 || o0.IsRegType(RegType.GPQ),
                          (byte)((BaseReg)o1).Code,
                          ((BaseReg)o0), (IntPtr)1);
                        immOperand = (Imm)o2;
                        immSize = 1;
                        goto emitImmediate;
                    }

                    if (o0.IsMem)
                    {
                        EmitMmu((uint)opCode, id.OpCodeR != 0,
                          (byte)((BaseReg)o1).Code,
                          ((Mem)o0), (IntPtr)1);
                        immOperand = (Imm)o2;
                        immSize = 1;
                        goto emitImmediate;
                    }

                    break;
                }

            case InstructionGroup.MMU_RMI:
                {
                    if (id.OperandFlags[0] == OperandFlags.None)
                        throw new AssemblerException();
                    if (id.OperandFlags[1] == OperandFlags.None)
                        throw new AssemblerException();

                    // Check parameters (X)MM|GP32_64 <- (X)MM|GP32_64|Mem|Imm
                    if (!o0.IsReg ||
                        (o0.IsRegType(RegType.MM) && (id.OperandFlags[0] & OperandFlags.MM) == 0) ||
                        (o0.IsRegType(RegType.XMM) && (id.OperandFlags[0] & OperandFlags.XMM) == 0) ||
                        (o0.IsRegType(RegType.GPD) && (id.OperandFlags[0] & OperandFlags.GD) == 0) ||
                        (o0.IsRegType(RegType.GPQ) && (id.OperandFlags[0] & OperandFlags.GQ) == 0) ||
                        (o1.IsRegType(RegType.MM) && (id.OperandFlags[1] & OperandFlags.MM) == 0) ||
                        (o1.IsRegType(RegType.XMM) && (id.OperandFlags[1] & OperandFlags.XMM) == 0) ||
                        (o1.IsRegType(RegType.GPD) && (id.OperandFlags[1] & OperandFlags.GD) == 0) ||
                        (o1.IsRegType(RegType.GPQ) && (id.OperandFlags[1] & OperandFlags.GQ) == 0) ||
                        (o1.IsMem && (id.OperandFlags[1] & OperandFlags.MEM) == 0) ||
                        (o1.IsImm && (id.OperandFlags[1] & OperandFlags.IMM) == 0))
                    {
                        throw NewIllegalInstructionException();
                    }

                    int prefix =
                      ((id.OperandFlags[0] & OperandFlags.MM_XMM) == OperandFlags.MM_XMM && o0.IsRegType(RegType.XMM)) ||
                      ((id.OperandFlags[1] & OperandFlags.MM_XMM) == OperandFlags.MM_XMM && o1.IsRegType(RegType.XMM))
                        ? 0x66000000
                        : 0x00000000;
                    bool rexw = ((id.OperandFlags[0] | id.OperandFlags[1]) & OperandFlags.NOREX) != 0
                      ? false
                      : (o0.IsRegType(RegType.GPQ) || o1.IsRegType(RegType.GPQ));

                    // (X)MM <- (X)MM (opcode0)
                    if (o1.IsReg)
                    {
                        if ((id.OperandFlags[1] & (OperandFlags.MM_XMM | OperandFlags.GQD)) == 0)
                            throw NewIllegalInstructionException();
                        EmitMmu((uint)id.OpCode0 | (uint)prefix, rexw,
                          (byte)((BaseReg)o0).Code,
                          ((BaseReg)o1), IntPtr.Zero);
                        goto end;
                    }
                    // (X)MM <- Mem (opcode0)
                    if (o1.IsMem)
                    {
                        if ((id.OperandFlags[1] & OperandFlags.MEM) == 0)
                            throw NewIllegalInstructionException();

                        EmitMmu((uint)id.OpCode0 | (uint)prefix, rexw,
                          (byte)((BaseReg)o0).Code,
                          ((Mem)o1), IntPtr.Zero);
                        goto end;
                    }
                    // (X)MM <- Imm (opcode1+opcodeR)
                    if (o1.IsImm)
                    {
                        if ((id.OperandFlags[1] & OperandFlags.IMM) == 0)
                            throw NewIllegalInstructionException();

                        EmitMmu((uint)id.OpCode1 | (uint)prefix, rexw,
                          (byte)id.OpCodeR,
                          ((BaseReg)o0), (IntPtr)1);
                        immOperand = (Imm)o1;
                        immSize = 1;
                        goto emitImmediate;
                    }

                    break;
                }

            case InstructionGroup.MMU_RM_IMM8:
                {
                    if (id.OperandFlags[0] == OperandFlags.None)
                        throw new AssemblerException();
                    if (id.OperandFlags[1] == OperandFlags.None)
                        throw new AssemblerException();

                    // Check parameters (X)MM|GP32_64 <- (X)MM|GP32_64|Mem|Imm
                    if (!o0.IsReg ||
                        (o0.IsRegType(RegType.MM) && (id.OperandFlags[0] & OperandFlags.MM) == 0) ||
                        (o0.IsRegType(RegType.XMM) && (id.OperandFlags[0] & OperandFlags.XMM) == 0) ||
                        (o0.IsRegType(RegType.GPD) && (id.OperandFlags[0] & OperandFlags.GD) == 0) ||
                        (o0.IsRegType(RegType.GPQ) && (id.OperandFlags[0] & OperandFlags.GQ) == 0) ||
                        (o1.IsRegType(RegType.MM) && (id.OperandFlags[1] & OperandFlags.MM) == 0) ||
                        (o1.IsRegType(RegType.XMM) && (id.OperandFlags[1] & OperandFlags.XMM) == 0) ||
                        (o1.IsRegType(RegType.GPD) && (id.OperandFlags[1] & OperandFlags.GD) == 0) ||
                        (o1.IsRegType(RegType.GPQ) && (id.OperandFlags[1] & OperandFlags.GQ) == 0) ||
                        (o1.IsMem && (id.OperandFlags[1] & OperandFlags.MEM) == 0) ||
                        !o2.IsImm)
                    {
                        throw NewIllegalInstructionException();
                    }

                    int prefix =
                      ((id.OperandFlags[0] & OperandFlags.MM_XMM) == OperandFlags.MM_XMM && o0.IsRegType(RegType.XMM)) ||
                      ((id.OperandFlags[1] & OperandFlags.MM_XMM) == OperandFlags.MM_XMM && o1.IsRegType(RegType.XMM))
                        ? 0x66000000
                        : 0x00000000;
                    bool rexw = ((id.OperandFlags[0] | id.OperandFlags[1]) & OperandFlags.NOREX) != 0
                      ? false
                      : (o0.IsRegType(RegType.GPQ) || o1.IsRegType(RegType.GPQ));

                    // (X)MM <- (X)MM (opcode0)
                    if (o1.IsReg)
                    {
                        if ((id.OperandFlags[1] & (OperandFlags.MM_XMM | OperandFlags.GQD)) == 0)
                            throw NewIllegalInstructionException();
                        EmitMmu((uint)id.OpCode0 | (uint)prefix, rexw,
                          (byte)((BaseReg)o0).Code,
                          ((BaseReg)o1), (IntPtr)1);
                        immOperand = (Imm)o2;
                        immSize = 1;
                        goto emitImmediate;
                    }
                    // (X)MM <- Mem (opcode0)
                    if (o1.IsMem)
                    {
                        if ((id.OperandFlags[1] & OperandFlags.MEM) == 0)
                            throw NewIllegalInstructionException();
                        EmitMmu((uint)id.OpCode0 | (uint)prefix, rexw,
                          (byte)((BaseReg)o0).Code,
                          ((Mem)o1), (IntPtr)1);
                        immOperand = (Imm)o2;
                        immSize = 1;
                        goto emitImmediate;
                    }

                    break;
                }

            case InstructionGroup.MMU_RM_3DNOW:
                {
                    if (o0.IsRegType(RegType.MM) && (o1.IsRegType(RegType.MM) || o1.IsMem))
                    {
                        EmitMmu((uint)id.OpCode0, false,
                          (byte)((BaseReg)o0).Code,
                          ((Mem)o1), (IntPtr)1);
                        EmitByte((byte)id.OpCode1);
                        goto end;
                    }

                    break;
                }
            }

            throw new InvalidOperationException();

        emitImmediate:
            {
                IntPtr value = immOperand.Value;
                switch (immSize)
                {
                case 1:
                    EmitByte((byte)value);
                    break;

                case 2:
                    EmitWord((ushort)value);
                    break;

                case 4:
                    EmitDWord((uint)value);
                    break;

                case 8:
                    if (Util.IsX64)
                    {
                        EmitQWord((ulong)value);
                        break;
                    }
                    else
                    {
                        goto default;
                    }

                default:
                    throw new ArgumentException();
                }
            }

        end:
            if (Logger != null)
            {
                // Detect truncated operand.
                Imm immTemporary = 0;

                // Use the original operands, because BYTE some of them were replaced.
                if (bLoHiUsed != 0)
                {
                    o0 = loggerOperands[0];
                    o1 = loggerOperands[1];
                    o2 = loggerOperands[2];
                }

                if (immOperand != null)
                {
                    IntPtr value = immOperand.Value;
                    bool isUnsigned = immOperand.IsUnsigned;

                    switch (immSize)
                    {
                    case 1:
                        if (isUnsigned && !Util.IsUInt8(value.ToInt64()))
                        {
                            immTemporary = new Imm(value, true);
                            break;
                        }

                        if (!isUnsigned && !Util.IsInt8(value.ToInt64()))
                        {
                            immTemporary = new Imm(value, false);
                            break;
                        }

                        break;

                    case 2:
                        if (isUnsigned && !Util.IsUInt16(value.ToInt64()))
                        {
                            immTemporary = new Imm(value, true);
                            break;
                        }

                        if (!isUnsigned && !Util.IsInt16(value.ToInt64()))
                        {
                            immTemporary = new Imm(value, false);
                            break;
                        }

                        break;

                    case 4:
                        if (isUnsigned && !Util.IsUInt32(value.ToInt64()))
                        {
                            immTemporary = new Imm(value, true);
                            break;
                        }

                        if (!isUnsigned && !Util.IsInt32(value.ToInt64()))
                        {
                            immTemporary = new Imm(value, false);
                            break;
                        }

                        break;
                    }

                    if (immTemporary.Value != IntPtr.Zero)
                    {
                        if (o0 == immOperand)
                            o0 = immTemporary;

                        if (o1 == immOperand)
                            o1 = immTemporary;

                        if (o2 == immOperand)
                            o2 = immTemporary;
                    }
                }

                StringBuilder buf = new StringBuilder();
                int bufferStopOffset = _buffer.Offset;

                DumpInstruction(buf, new ArraySegment<byte>(_buffer.Data, bufferStartOffset, bufferStopOffset - bufferStartOffset), code, _emitOptions, o0, o1, o2, memRegType);

                if (Logger.LogBinary)
                    DumpComment(buf, new ArraySegment<byte>(GetCode(), (int)beginOffset, (int)(Offset - beginOffset)), _comment);
                else
                    DumpComment(buf, null, _comment);

                // We don't need to null terminate the resulting string.
                Logger.LogString(buf.ToString());
            }

            _comment = null;
            _emitOptions = EmitOptions.None;
        }

        private void EmitMovSregRM(SegmentReg src, bool forceRexPrefix)
        {
            Contract.Requires(src != null);

            EmitX86RM(0x8E,
                src.Size == 2,
                src.Size == 8,
                (byte)((SegmentReg)src).RegisterIndex,
                (Operand)src,
                0, forceRexPrefix);
        }

        private Exception NewIllegalInstructionException()
        {
            return new AssemblerException("Encountered an illegal instruction");
        }

        private void DumpComment(StringBuilder buf, IList<byte> binaryData, string comment)
        {
            Contract.Requires(buf != null);

            int currentLength = buf.Length;
            int commentLength = comment != null ? comment.Length : 0;
            binaryData = binaryData ?? new byte[0];

            if (binaryData.Count != 0 || commentLength != 0)
            {
                int align = 32;
                char sep = ';';

                // Truncate if comment is too long (it shouldn't be, larger than 80 seems to
                // be an exploit).
                if (commentLength > 80)
                    commentLength = 80;

                for (int i = (binaryData.Count == 0 ? 1 : 0); i < 2; i++)
                {
                    // Append align.
                    if (currentLength < align)
                    {
                        buf.Append(new string(' ', align - currentLength));
                    }

                    // Append separator.
                    if (sep != 0)
                    {
                        buf.Append(sep);
                        buf.Append(' ');
                    }

                    // Append binary data or comment.
                    if (i == 0)
                    {
                        Util.myhex(buf, binaryData);
                        if (commentLength == 0)
                            break;
                    }
                    else
                    {
                        buf.Append(comment);
                    }

                    currentLength = buf.Length;
                    align += 18;
                    sep = '|';
                }
            }

            buf.AppendLine();
        }

        private byte[] GetCode()
        {
            return _buffer.Data;
        }

        private static void DumpInstruction(StringBuilder buf, IList<byte> machineCode, InstructionCode code, EmitOptions emitOptions, Operand o0, Operand o1, Operand o2, RegType memRegType)
        {
            Contract.Requires(buf != null);
            Contract.Requires(machineCode != null);
            Contract.Requires(o0 != null);
            Contract.Requires(o1 != null);
            Contract.Requires(o2 != null);

            // Dump the machine code
            for (int i = 0; i < machineCode.Count; i++)
            {
                buf.AppendFormat("{0:X2} ", machineCode[i]);
            }

            buf.Append(new string(' ', 3 * Math.Max(0, 6 - machineCode.Count)));

            if (emitOptions.HasFlag(EmitOptions.RexPrefix))
                buf.Append("rex ");
            if (emitOptions.HasFlag(EmitOptions.LockPrefix))
                buf.Append("lock ");
            if (emitOptions.HasFlag(EmitOptions.ShortJump))
                buf.Append("short ");

            // Dump instruction.
            DumpInstructionName(buf, code);

            // Dump operands.
            if (!o0.IsNone)
            {
                buf.Append(' ');
                DumpOperand(buf, o0, memRegType);
            }

            if (!o1.IsNone)
            {
                buf.Append(", ");
                DumpOperand(buf, o1, memRegType);
            }

            if (!o2.IsNone)
            {
                buf.Append(", ");
                DumpOperand(buf, o2, memRegType);
            }
        }

        private static void DumpInstructionName(StringBuilder buf, InstructionCode code)
        {
            Contract.Requires(buf != null);

            if ((int)code >= _instructionCount)
                throw new ArgumentException();

            buf.Append(InstructionDescription.FromInstruction(code).Name);
        }

        private static readonly string[] _operandSize =
            {
                null,
                "byte ptr ",
                "word ptr ",
                null,
                "dword ptr ",
                null,
                null,
                null,
                "qword ptr ",
                null,
                "tword ptr ",
                null,
                null,
                null,
                null,
                null,
                "dqword ptr ",
            };

        private static readonly string[] _segmentPrefixName =
            {
                "es:",
                "cs:",
                "ss:",
                "ds:",
                "fs:",
                "gs:",
                "",
            };

        internal static void DumpOperand(StringBuilder buf, Operand op, RegType memRegType)
        {
            Contract.Requires(op != null);

            if (op.IsReg)
            {
                BaseReg reg = ((BaseReg)op);
                DumpRegister(buf, reg.RegisterType, reg.RegisterIndex);
            }
            else if (op.IsMem)
            {
                Mem mem = ((Mem)op);
                SegmentPrefix segmentPrefix = mem.SegmentPrefix;

                bool isAbsolute = false;

                if (op.Size <= 16)
                {
                    buf.Append(_operandSize[op.Size]);
                }

                if ((int)segmentPrefix < RegNum.Segment)
                {
                    buf.Append(_segmentPrefixName[(int)segmentPrefix]);
                }

                buf.Append('[');

                switch (mem.MemoryType)
                {
                case MemoryType.Native:
                    {
                        // [base + index*scale + displacement]
                        DumpRegister(buf, memRegType, mem.Base);
                        break;
                    }
                case MemoryType.Label:
                    {
                        // [label + index*scale + displacement]
                        buf.AppendFormat("L.{0}", (int)mem.Base & Operand.OperandIdValueMask);
                        break;
                    }
                case MemoryType.Absolute:
                    {
                        // [absolute]
                        isAbsolute = true;
                        buf.AppendFormat("{0:x}", (long)mem.Target + (long)mem.Displacement);
                        break;
                    }
                }

                if (mem.HasIndex)
                {
                    buf.Append(" + ");
                    DumpRegister(buf, memRegType, mem.Index);

                    if (mem.Shift != 0)
                    {
                        buf.Append(" * ");
                        buf.Append("1248"[mem.Shift & 3]);
                    }
                }

                if (mem.Displacement != IntPtr.Zero && !isAbsolute)
                {
                    var d = mem.Displacement.ToInt64();
                    buf.Append(' ');
                    buf.Append(d < 0 ? '-' : '+');
                    buf.Append(' ');
                    buf.Append(d < 0 ? -d : d);
                }

                buf.Append(']');
                return;
            }
            else if (op.IsImm)
            {
                Imm i = ((Imm)op);
                buf.AppendFormat(string.Format("0x{{0:X{0}}}", IntPtr.Size * 2), i.Value.ToInt64());
                return;
            }
            else if (op.IsLabel)
            {
                buf.AppendFormat("L.{0}", op.Id & Operand.OperandIdValueMask);
                return;
            }
            else
            {
                buf.Append("None");
                return;
            }
        }

        private static readonly string[] _reg8l = { "al", "cl", "dl", "bl", "spl", "bpl", "sil", "dil" };
        private static readonly string[] _reg8h = { "ah", "ch", "dh", "bh", "NE", "NE", "NE", "NE" };
        private static readonly string[] _reg16 = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };

        private static void DumpRegister(StringBuilder buf, RegType type, RegIndex index)
        {
            switch (type)
            {
            case RegType.GPB_LO:
                if ((int)index < 8)
                {
                    //return buf + sprintf(buf, "%s", _reg8l[(int)index]);
                    buf.Append(_reg8l[(int)index]);
                    return;
                }

                //return buf + sprintf(buf, "r%ub", (uint32_t)index);
                buf.Append('r');
                goto _EmitID;

            case RegType.GPB_HI:
                if ((int)index < 4)
                {
                    //return buf + sprintf(buf, "%s", _reg8h[(int)index]);
                    buf.Append(_reg8h[(int)index]);
                    return;
                }

            _EmitNE:
                //return buf + sprintf(buf, "%s", "INVALID");
                buf.Append("NE");
                return;

            case RegType.GPW:
                if ((int)index < 8)
                {
                    //return buf + sprintf(buf, "%s", _reg16[(int)index]);
                    buf.Append(_reg16[(int)index]);
                    return;
                }

                buf.AppendFormat("r{0}w", (int)index);
                return;

            case RegType.GPD:
                if ((int)index < 8)
                {
                    //return buf + sprintf(buf, "e%s", _reg16[(int)index]);
                    buf.AppendFormat("e{0}", _reg16[(int)index]);
                    return;
                }

                //return buf + sprintf(buf, "r%uw", (uint32_t)index);
                buf.AppendFormat("r{0}d", (int)index);
                return;

            case RegType.GPQ:
                buf.Append('r');

                if ((int)index < 8)
                {
                    buf.Append(_reg16[(int)index]);
                    return;
                }

            _EmitID:
                buf.Append((int)index);
                return;

            case RegType.X87:
                buf.Append("st");
                goto _EmitID;

            case RegType.MM:
                buf.Append("mm");
                goto _EmitID;

            case RegType.XMM:
                buf.Append("xmm");
                goto _EmitID;

            case RegType.Segment:
                if ((int)index < RegNum.Segment)
                {
                    buf.Append(_segmentPrefixName[(int)index]);
                    return;
                }

                goto _EmitNE;

            default:
                return;
            }
        }

        private bool CanEmit()
        {
            // The ensureSpace() method returns true on success and false on failure. We
            // are catching return value and setting error code here.
            EnsureSpace();
            return true;
        }

        private void EnsureSpace()
        {
            _buffer.EnsureSpace();
        }

        private void EmitByte(byte x)
        {
            _buffer.EmitByte(x);
        }

        private void EmitWord(ushort x)
        {
            _buffer.EmitWord(x);
        }

        private void EmitDWord(uint x)
        {
            _buffer.EmitDWord(x);
        }

        private void EmitQWord(ulong x)
        {
            _buffer.EmitQWord(x);
        }

        private void EmitInt32(int x)
        {
            _buffer.EmitDWord((uint)x);
        }

        private void EmitSysInt(IntPtr x)
        {
            _buffer.EmitSysInt(x);
        }

        private void EmitSysUInt(UIntPtr x)
        {
            _buffer.EmitSysUInt(x);
        }

        private void EmitOpCode(int opcode)
        {
            // instruction prefix
            if (((uint)opcode & 0xFF000000) != 0)
                EmitByte((byte)((opcode & 0xFF000000) >> 24));
            // instruction opcodes
            if ((opcode & 0x00FF0000) != 0)
                EmitByte((byte)((opcode & 0x00FF0000) >> 16));
            if ((opcode & 0x0000FF00) != 0)
                EmitByte((byte)((opcode & 0x0000FF00) >> 8));
            // last opcode is always emitted (can be also 0x00)
            EmitByte((byte)(opcode & 0x000000FF));
        }

        private static readonly byte[] _segmentPrefixCode =
            {
                0x2E, // ES
                0x36, // SS
                0x3E, // SS
                0x26, // DS
                0x64, // FS
                0x65  // GS
            };

        private void EmitSegmentPrefix(Operand rm)
        {
            Contract.Requires(rm != null);

            if (!rm.IsMem)
                return;

            SegmentPrefix segmentPrefix = ((Mem)rm).SegmentPrefix;
            if ((int)segmentPrefix > RegNum.Segment)
                return;

            EmitByte(_segmentPrefixCode[(int)segmentPrefix]);
        }

        private void EmitMod(byte m, byte o, byte r)
        {
            EmitByte((byte)(((m & 0x03) << 6) | ((o & 0x07) << 3) | (r & 0x07)));
        }

        private void EmitSib(byte s, byte i, byte b)
        {
            EmitByte((byte)(((s & 0x03) << 6) | ((i & 0x07) << 3) | (b & 0x07)));
        }

        private void EmitRexR(bool w, byte opReg, byte regCode, bool forceRexPrefix)
        {
            if (Util.IsX64)
            {
                bool r = (opReg & 0x8) != 0;
                bool b = (regCode & 0x8) != 0;

                // w Default operand size(0=Default, 1=64 bits).
                // r Register field (1=high bit extension of the ModR/M REG field).
                // x Index field not used in RexR
                // b Base field (1=high bit extension of the ModR/M or SIB Base field).
                if (w || r || b || forceRexPrefix)
                {
                    EmitByte((byte)(0x40 | ((w ? 1 : 0) << 3) | ((r ? 1 : 0) << 2) | (b ? 1 : 0)));
                }
            }
        }

        private void EmitRexRM(bool w, byte opReg, Operand rm, bool forceRexPrefix)
        {
            if (Util.IsX64)
            {
                bool r = (opReg & 0x8) != 0;
                bool x = false;
                bool b = false;

                if (rm.IsReg)
                {
                    b = ((int)((BaseReg)rm).Code & 0x8) != 0;
                }
                else if (rm.IsMem)
                {
                    x = (((int)((Mem)rm).Index & 0x8) != 0) && (((Mem)rm).Index != RegIndex.Invalid);
                    b = (((int)((Mem)rm).Base & 0x8) != 0) && (((Mem)rm).Base != RegIndex.Invalid);
                }

                // w Default operand size(0=Default, 1=64 bits).
                // r Register field (1=high bit extension of the ModR/M REG field).
                // x Index field (1=high bit extension of the SIB Index field).
                // b Base field (1=high bit extension of the ModR/M or SIB Base field).
                if (w || r || x || b || forceRexPrefix)
                {
                    EmitByte((byte)(0x40 | ((w ? 1 : 0) << 3) | ((r ? 1 : 0) << 2) | ((x ? 1 : 0) << 1) | (b ? 1 : 0)));
                }
            }
        }

        private void EmitModR(byte opReg, byte r)
        {
            EmitMod(3, opReg, r);
        }

        private void EmitModR(byte opReg, BaseReg r)
        {
            Contract.Requires(r != null);

            EmitMod(3, opReg, (byte)r.Code);
        }

        private void EmitModM(byte opReg, Mem mem, int immSize)
        {
            Contract.Requires(mem != null);

            byte baseReg = (byte)((int)mem.Base & 0x7);
            byte indexReg = (byte)((int)mem.Index & 0x7);
            IntPtr disp = mem.Displacement;
            int shift = mem.Shift;

            if (mem.MemoryType == MemoryType.Native)
            {
                // [base + displacemnt]
                if (!mem.HasIndex)
                {
                    // ESP/RSP/R12 == 4
                    if (baseReg == 4)
                    {
                        byte mod = 0;

                        if (disp != IntPtr.Zero)
                        {
                            mod = Util.IsInt8(disp.ToInt64()) ? (byte)1 : (byte)2;
                        }

                        EmitMod(mod, opReg, 4);
                        EmitSib(0, 4, 4);

                        if (disp != IntPtr.Zero)
                        {
                            if (Util.IsInt8(disp.ToInt64()))
                                EmitByte((byte)disp);
                            else
                                EmitInt32((int)disp);
                        }
                    }
                    // EBP/RBP/R13 == 5
                    else if (baseReg != 5 && disp == IntPtr.Zero)
                    {
                        EmitMod(0, opReg, baseReg);
                    }
                    else if (Util.IsInt8(disp.ToInt64()))
                    {
                        EmitMod(1, opReg, baseReg);
                        EmitByte((byte)disp);
                    }
                    else
                    {
                        EmitMod(2, opReg, baseReg);
                        EmitInt32((int)disp);
                    }
                }

                // [base + index * scale + displacemnt]
                else
                {
                    // ASMJIT_ASSERT(indexReg != RID_ESP);

                    // EBP/RBP/R13 == 5
                    if (baseReg != 5 && disp == IntPtr.Zero)
                    {
                        EmitMod(0, opReg, 4);
                        EmitSib((byte)shift, indexReg, baseReg);
                    }
                    else if (Util.IsInt8(disp.ToInt64()))
                    {
                        EmitMod(1, opReg, 4);
                        EmitSib((byte)shift, indexReg, baseReg);
                        EmitByte((byte)disp);
                    }
                    else
                    {
                        EmitMod(2, opReg, 4);
                        EmitSib((byte)shift, indexReg, baseReg);
                        EmitInt32((int)disp);
                    }
                }
            }

            // Address                       | 32-bit mode | 64-bit mode
            // ------------------------------+-------------+---------------
            // [displacement]                |   ABSOLUTE  | RELATIVE (RIP)
            // [index * scale + displacemnt] |   ABSOLUTE  | ABSOLUTE (ZERO EXTENDED)
            else
            {
                // - In 32 bit mode the absolute addressing model is used.
                // - In 64 bit mode the relative addressing model is used together with
                //   the absolute addressing one. Main problem is that if instruction
                //   contains SIB then relative addressing (RIP) is not possible.

                if (Util.IsX86)
                {
                    if (mem.HasIndex)
                    {
                        // ASMJIT_ASSERT(mem.getMemIndex() != 4); // ESP/RSP == 4
                        EmitMod(0, opReg, 4);
                        EmitSib((byte)shift, indexReg, 5);
                    }
                    else
                    {
                        EmitMod(0, opReg, 5);
                    }

                    // X86 uses absolute addressing model, all relative addresses will be
                    // relocated to absolute ones.
                    if (mem.MemoryType == MemoryType.Label)
                    {
                        AssemblerLabelData l_data = _labelData[(int)mem.Base & Operand.OperandIdValueMask];
                        RelocationData r_data = new RelocationData();
                        int relocId = _relocationData.Count;

                        // Relative addressing will be relocated to absolute address.
                        r_data.Type = RelocationType.RelativeToAbsolute;
                        r_data.Size = 4;
                        r_data.Offset = Offset;
                        r_data.Destination = disp;

                        if (l_data.Offset != -1)
                        {
                            // Bound label.
                            r_data.Destination += l_data.Offset;

                            // Add a dummy DWORD.
                            EmitInt32(0);
                        }
                        else
                        {
                            // Non-bound label.
                            EmitDisplacement(l_data, -4 - immSize, 4).RelocId = relocId;
                        }

                        _relocationData.Add(r_data);
                    }
                    else
                    {
                        // Absolute address
                        EmitInt32(mem.Target.ToInt32() + disp.ToInt32());
                    }
                }
                else if (Util.IsX64)
                {
                    // X64 uses relative addressing model
                    if (mem.MemoryType == MemoryType.Label)
                    {
                        AssemblerLabelData l_data = _labelData[(int)mem.Base & Operand.OperandIdValueMask];

                        if (mem.HasIndex)
                        {
                            throw new AssemblerException("Illegal addressing: indexing is not possible.");
                        }

                        // Relative address (RIP +/- displacement)
                        EmitMod(0, opReg, 5);

                        disp -= (4 + immSize);

                        if (l_data.Offset != -1)
                        {
                            // Bound label.
                            disp = (IntPtr)(disp.ToInt64() + Offset - l_data.Offset);

                            // Displacement is known.
                            EmitInt32(disp.ToInt32());
                        }
                        else
                        {
                            // Non-bound label.
                            EmitDisplacement(l_data, disp.ToInt64(), 4);
                        }
                    }
                    else
                    {
                        // Absolute address (truncated to 32 bits), this kind of address requires
                        // SIB byte (4)
                        EmitMod(0, opReg, 4);

                        if (mem.HasIndex)
                        {
                            // ASMJIT_ASSERT(mem.getMemIndex() != 4); // ESP/RSP == 4
                            EmitSib((byte)shift, indexReg, 5);
                        }
                        else
                        {
                            EmitSib(0, 4, 5);
                        }

                        // truncate to 32 bits
                        UIntPtr target = (UIntPtr)(mem.Target.ToInt64() + disp.ToInt64());

                        if (target.ToUInt64() > uint.MaxValue)
                        {
                            if (Logger != null)
                            {
                                Logger.LogString("*** ASSEMBER WARNING - Absolute address truncated to 32 bits." + Environment.NewLine);
                            }
                            target = (UIntPtr)(target.ToUInt64() & uint.MaxValue);
                        }

                        EmitInt32((int)((uint)target));
                    }
                }
                else
                {
                    throw new NotImplementedException();
                }
            }
        }

        private void EmitModRM(byte opReg, Operand op, int immSize)
        {
            Contract.Requires(op != null);

            if (op.OperandType != OperandType.Reg && op.OperandType != OperandType.Mem)
                throw new ArgumentException();

            if (op.OperandType == OperandType.Reg)
                EmitModR(opReg, (byte)((BaseReg)op).Code);
            else
                EmitModM(opReg, ((Mem)op), immSize);
        }

        private void EmitX86Inl(int opcode, bool i16bit, bool rexw, byte reg, bool forceRexPrefix)
        {
            // 16 bit prefix
            if (i16bit)
                EmitByte(0x66);

            // instruction prefix
            if ((opcode & 0xFF000000) != 0)
                EmitByte((byte)((opcode & 0xFF000000) >> 24));

            // rex prefix
            if (Util.IsX64)
                EmitRexR(rexw, 0, reg, forceRexPrefix);

            // instruction opcodes
            if ((opcode & 0x00FF0000) != 0)
                EmitByte((byte)((opcode & 0x00FF0000) >> 16));
            if ((opcode & 0x0000FF00) != 0)
                EmitByte((byte)((opcode & 0x0000FF00) >> 8));

            EmitByte((byte)((opcode & 0x000000FF) + (reg & 0x7)));
        }

        private void EmitX86RM(int opCode, bool i16bit, bool rexw, byte o, Operand op, int immSize, bool forceRexPrefix)
        {
            Contract.Requires(op != null);

            // 16 bit prefix.
            if (i16bit)
                EmitByte(0x66);

            // Segment prefix.
            EmitSegmentPrefix(op);

            // Instruction prefix.
            if (((uint)opCode & 0xFF000000) != 0)
                EmitByte((byte)((opCode & 0xFF000000) >> 24));

            // REX prefix.
            if (Util.IsX64)
                EmitRexRM(rexw, o, op, forceRexPrefix);

            // Instruction opcodes.
            if ((opCode & 0x00FF0000) != 0)
                EmitByte((byte)((opCode & 0x00FF0000) >> 16));
            if ((opCode & 0x0000FF00) != 0)
                EmitByte((byte)((opCode & 0x0000FF00) >> 8));
            EmitByte((byte)(opCode & 0x000000FF));

            // Mod R/M.
            EmitModRM(o, op, immSize);
        }

        /// <summary>Emit FPU instruction with no operands.</summary>
        private void EmitFpu(int opCode)
        {
            EmitOpCode(opCode);
        }

        /// <summary>Emit FPU instruction with one operand @a sti (index of FPU register).</summary>
        private void EmitFpuSTI(int opCode, int sti)
        {
            Contract.Requires(sti >= 0 && sti < 8);
            EmitOpCode(opCode + sti);
        }

        /// <summary>Emit FPU instruction with one operand @a opReg and memory operand @a mem.</summary>
        private void EmitFpuMEM(int opCode, byte opReg, Mem mem)
        {
            Contract.Requires(mem != null);

            // segment prefix
            EmitSegmentPrefix(mem);

            // instruction prefix
            if ((opCode & 0xFF000000) != 0)
                EmitByte((byte)((opCode & 0xFF000000) >> 24));

            // rex prefix
            if (Util.IsX64)
                EmitRexRM(false, opReg, mem, false);

            // instruction opcodes
            if ((opCode & 0x00FF0000) != 0)
                EmitByte((byte)((opCode & 0x00FF0000) >> 16));
            if ((opCode & 0x0000FF00) != 0)
                EmitByte((byte)((opCode & 0x0000FF00) >> 8));

            EmitByte((byte)((opCode & 0x000000FF)));
            EmitModM(opReg, mem, 0);
        }

        /// <summary>Emit MMX/SSE instruction.</summary>
        private void EmitMmu(uint opCode, bool rexw, byte opReg, Operand src, IntPtr immSize)
        {
            Contract.Requires(src != null);

            // Segment prefix.
            EmitSegmentPrefix(src);

            // Instruction prefix.
            if ((opCode & 0xFF000000) != 0)
                EmitByte((byte)((opCode & 0xFF000000) >> 24));

            // Rex prefix
            if (Util.IsX64)
                EmitRexRM(rexw, opReg, src, false);

            // Instruction opcodes.
            if ((opCode & 0x00FF0000) != 0)
                EmitByte((byte)((opCode & 0x00FF0000) >> 16));

            // No checking, MMX/SSE instructions have always two opcodes or more.
            EmitByte((byte)((opCode & 0x0000FF00) >> 8));
            EmitByte((byte)((opCode & 0x000000FF)));

            if (src.IsReg)
                EmitModR(opReg, (byte)((BaseReg)src).Code);
            else
                EmitModM(opReg, (Mem)src, (int)immSize);
        }

        /// <summary>Emit displacement.</summary>
        private LabelLink EmitDisplacement(AssemblerLabelData l_data, long inlinedDisplacement, int size)
        {
            Contract.Requires(l_data != null);

            if (l_data.Offset != -1)
                throw new ArgumentException();
            if (size != 1 && size != 4)
                throw new ArgumentException();

            // Chain with label.
            LabelLink link = NewLabelLink();
            link.Previous = l_data.Links;
            link.Offset = (int)Offset;
            link.Displacement = (int)inlinedDisplacement;

            l_data.Links = link;

            // Emit label size as dummy data.
            if (size == 1)
                EmitByte(0x01);
            else // if (size == 4)
                EmitDWord(0x04040404);

            return link;
        }

        /// <summary>Emit relative relocation to absolute pointer @a target. It's needed
        /// to add what instruction is emitting this, because in x64 mode the relative
        /// displacement can be impossible to calculate and in this case the trampoline
        /// is used.</summary>
        private void EmitJmpOrCallReloc(int instruction, IntPtr target)
        {
            RelocationData rd = new RelocationData();

            rd.Type = RelocationType.AbsoluteToRelativeTrampoline;

            if (Util.IsX64)
            {
                // If we are compiling in 64-bit mode, we can use trampoline if relative jump
                // is not possible.
                _trampolineSize += TrampolineWriter.TRAMPOLINE_SIZE;
            }

            rd.Size = 4;
            rd.Offset = Offset;
            rd.Destination = target;

            _relocationData.Add(rd);

            // Emit dummy 32-bit integer (will be overwritten by relocCode()).
            EmitInt32(0);
        }

        // Helpers to decrease binary code size. These four emit methods are just
        // helpers thats used by assembler. They call emitX86() adding NULLs
        // to first, second and third operand, if needed.

        /// <summary>Private method for emitting jcc.</summary>
        public void EmitJcc(InstructionCode code, Label label, Hint hint)
        {
            if (hint == Hint.None)
            {
                EmitInstruction(code, label);
            }
            else
            {
                Imm imm = (Imm)(int)hint;
                EmitInstruction(code, label, imm);
            }
        }

        private void EmitShortJcc(InstructionCode code, Label label, Hint hint)
        {
            Contract.Requires(label != null);

            _emitOptions |= EmitOptions.ShortJump;
            EmitJcc(code, label, hint);
        }
    }
}
