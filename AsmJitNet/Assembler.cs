namespace AsmJitNet
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics.Contracts;
    using System.Linq;
    using System.Text;
    using Debug = System.Diagnostics.Debug;
    using Marshal = System.Runtime.InteropServices.Marshal;

    public class Assembler
    {
        private CodeGenerator _codeGenerator;

        /// <summary>
        /// Last error code
        /// </summary>
        private int _error;

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

        /// <summary>
        /// Gets or sets the last error code
        /// </summary>
        public int Error
        {
            get
            {
                return _error;
            }

            set
            {
                _error = value;

                if (_error != 0 && Logger != null && Logger.IsUsed)
                {
                    Logger.LogFormat("*** ASSEMBLER ERROR: {0} ({1})." + Environment.NewLine, Errors.GetErrorCodeAsString(value), value);
                }
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

        public Label NewLabel()
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

        public void Bind(Label label)
        {
            // Only labels created by newLabel() can be used by Assembler.
            Debug.Assert(label.Id != Operand.InvalidValue);
            // Never go out of bounds.
            Debug.Assert((label.Id & Operand.OperandIdValueMask) < _labelData.Count);

            // Get label data based on label id.
            AssemblerLabelData l_data = _labelData[label.Id & Operand.OperandIdValueMask];

            // Label can be bound only once.
            Debug.Assert(l_data.Offset == -1);

            // Log.
            if (Logger != null && Logger.IsUsed)
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
                    Debug.Assert(size == 1 || size == 4);

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
                            Error = Errors.IllegalShortJump;
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
            if (_error != 0 || CodeSize == 0)
                return IntPtr.Zero;

            IntPtr p;
            _error = _codeGenerator.Generate(out p, this);

            return p;
            //IntPtr addressPtr = IntPtr.Zero;
            //IntPtr addressBase = IntPtr.Zero;
            //long codeSize = CodeSize;

            //_error = _codeGenerator.Alloc(out addressPtr, out addressBase, codeSize);

            //// Return on error.
            //if (_error != 0)
            //    return IntPtr.Zero;

            //// This is last step. Relocate code and return generated code.
            //RelocCode(addressPtr, addressBase);
            //return addressPtr;
        }

        public void RelocCode(IntPtr destination)
        {
            RelocCode(destination, destination);
        }

        public void RelocCode(IntPtr destination, IntPtr addressBase)
        {
            // Copy code to virtual memory (this is a given destination pointer).

            var coff = _buffer.Offset;
            var csize = CodeSize;

            // We are copying exactly size of generated code. Extra code for trampolines
            // is generated on-the-fly by relocator (this code not exists at now).
            Marshal.Copy(_buffer.Data, 0, destination, (int)coff);

#if ASMJIT_X64
            // Trampoline pointer
            IntPtr tramp = (IntPtr)(destination.ToInt64() + coff);
#endif // ASMJIT_X64

            int i;
            int len = _relocationData.Count;

            for (i = 0; i < len; i++)
            {
                RelocationData r = _relocationData[i];
                IntPtr val;

#if ASMJIT_X64
                // Whether to use trampoline, can be only used if relocation type is ABSOLUTE_TO_RELATIVE_TRAMPOLINE
                bool useTrampoline = false;
#endif // ASMJIT_X64

                // Be sure that reloc data structure is correct.
                Debug.Assert(r.Offset + r.Size <= csize);

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

#if ASMJIT_X64
                    if (r.Type == RelocationType.AbsoluteToRelativeTrampoline && !Util.IsInt32(val.ToInt64()))
                    {
                        val = (IntPtr)(tramp.ToInt64() - (addressBase.ToInt64() + r.Offset + 4));
                        useTrampoline = true;
                    }
#endif // ASMJIT_X64
                    break;

                default:
                    Debug.Fail("");
                    continue;
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
                    Debug.Fail("");
                    continue;
                }

#if ASMJIT_X64
                if (useTrampoline)
                {
                    if (Logger != null && Logger.IsUsed)
                    {
                        Logger.LogFormat("; Trampoline from 0x{0:x} -> 0x{1:x}" + Environment.NewLine, addressBase.ToInt64() + r.Offset, r.Destination);
                    }

                    TrampolineWriter.WriteTrampoline(tramp, r.Destination);
                    tramp += TrampolineWriter.TRAMPOLINE_SIZE;
                }
#endif // ASMJIT_X64
            }
        }

        public void Embed(byte[] data)
        {
            if (!CanEmit())
                return;

            int size = data.Length;

            if (Logger != null && Logger.IsUsed)
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
                        //p += sprintf(p, "%0.2X", (byte*)(data)[i + j]);
                        buf.AppendFormat("{0:x}", data[i + j]);

                    buf.AppendLine();

                    Logger.LogString(buf.ToString());
                }
            }

            _buffer.EmitData(data);
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

            if (Logger != null && Logger.IsUsed)
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

                if (ci.VendorId == CpuVendor.Intel &&
                   ((ci.Family & 0x0F) == 6 ||
                    (ci.Family & 0x0F) == 15)
                   )
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

                        default:
                            p = _nop9;
                            break;
                        }

                        i -= p.Length;

                        for (int j = 0; j < p.Length; j++)
                        {
                            EmitByte(p[j]);
                        }

                    } while (i != 0);

                    return;
                }

                if (ci.VendorId == CpuVendor.Amd && ci.Family >= 0x0F)
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
                            p = _nop10;
                            break;

                        default:
                            p = _nop11;
                            break;
                        }

                        i -= p.Length;

                        for (int j = 0; j < p.Length; j++)
                        {
                            EmitByte(p[j]);
                        }

                    } while (i != 0);

                    return;
                }
#if ASMJIT_X86
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
#endif
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

        private static InstructionCode ConditionToInstruction(Condition condition)
        {
            throw new NotImplementedException();
        }

        #region Instructions

        public void Adc(GPReg dst, GPReg src)
        {
            EmitInstruction(InstructionCode.Adc, dst, src);
        }

        public void Adc(GPReg dst, Mem src)
        {
            EmitInstruction(InstructionCode.Adc, dst, src);
        }

        public void Adc(GPReg dst, Imm src)
        {
            EmitInstruction(InstructionCode.Adc, dst, src);
        }

        public void Adc(Mem dst, GPReg src)
        {
            EmitInstruction(InstructionCode.Adc, dst, src);
        }

        public void Adc(Mem dst, Imm src)
        {
            EmitInstruction(InstructionCode.Adc, dst, src);
        }

        public void Add(GPReg dst, GPReg src)
        {
            EmitInstruction(InstructionCode.Add, dst, src);
        }

        public void Add(GPReg dst, Mem src)
        {
            EmitInstruction(InstructionCode.Add, dst, src);
        }

        public void Add(GPReg dst, Imm src)
        {
            EmitInstruction(InstructionCode.Add, dst, src);
        }

        public void Add(Mem dst, GPReg src)
        {
            EmitInstruction(InstructionCode.Add, dst, src);
        }

        public void Add(Mem dst, Imm src)
        {
            EmitInstruction(InstructionCode.Add, dst, src);
        }

        public void And(GPReg dst, GPReg src)
        {
            EmitInstruction(InstructionCode.And, dst, src);
        }

        public void And(GPReg dst, Mem src)
        {
            EmitInstruction(InstructionCode.And, dst, src);
        }

        public void And(GPReg dst, Imm src)
        {
            EmitInstruction(InstructionCode.And, dst, src);
        }

        public void And(Mem dst, GPReg src)
        {
            EmitInstruction(InstructionCode.And, dst, src);
        }

        public void And(Mem dst, Imm src)
        {
            EmitInstruction(InstructionCode.And, dst, src);
        }

        public void Call(GPReg dst)
        {
            if (!dst.IsRegType(RegType.GPN))
                throw new ArgumentException();

            EmitInstruction(InstructionCode.Call, dst);
        }

        public void Call(Mem dst)
        {
            EmitInstruction(InstructionCode.Call, dst);
        }

        public void Call(Imm dst)
        {
            EmitInstruction(InstructionCode.Call, dst);
        }

        public void Call(IntPtr dst)
        {
            EmitInstruction(InstructionCode.Call, (Imm)dst);
        }

        public void Call(Label dst)
        {
            EmitInstruction(InstructionCode.Call, dst);
        }

        public void Jmp(GPReg dst)
        {
            EmitInstruction(InstructionCode.Jmp, dst);
        }

        public void Jmp(Mem dst)
        {
            EmitInstruction(InstructionCode.Jmp, dst);
        }

        public void Jmp(Imm dst)
        {
            EmitInstruction(InstructionCode.Jmp, dst);
        }

        public void Jmp(IntPtr dst)
        {
            EmitInstruction(InstructionCode.Jmp, (Imm)dst);
        }

        public void Jmp(Label label)
        {
            EmitInstruction(InstructionCode.Jmp, label);
        }

        public void JShort(Condition cc, Label label, Hint hint = Hint.None)
        {
            EmitJcc(ConditionToInstruction(cc) + InstructionDescription.JumpShortOffset, label, hint);
        }

        public void JaShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JaShort, label, hint);
        }

        public void JaeShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JaeShort, label, hint);
        }

        public void JbShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JbShort, label, hint);
        }

        public void JbeShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JbeShort, label, hint);
        }

        public void JcShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JcShort, label, hint);
        }

        public void JeShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JeShort, label, hint);
        }

        public void JgShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JgShort, label, hint);
        }

        public void JgeShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JgeShort, label, hint);
        }

        public void JlShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JlShort, label, hint);
        }

        public void JleShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JleShort, label, hint);
        }

        public void JnaShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JnaShort, label, hint);
        }

        public void JnaeShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JnaeShort, label, hint);
        }

        public void JnbShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JnbShort, label, hint);
        }

        public void JnbeShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JnbeShort, label, hint);
        }

        public void JncShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JncShort, label, hint);
        }

        public void JneShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JneShort, label, hint);
        }

        public void JngShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JngShort, label, hint);
        }

        public void JngeShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JngeShort, label, hint);
        }

        public void JnlShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JnlShort, label, hint);
        }

        public void JnleShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JnleShort, label, hint);
        }

        public void JnoShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JnoShort, label, hint);
        }

        public void JnpShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JnpShort, label, hint);
        }

        public void JnsShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JnsShort, label, hint);
        }

        public void JnzShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JnzShort, label, hint);
        }

        public void JoShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JoShort, label, hint);
        }

        public void JpShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JpShort, label, hint);
        }

        public void JpeShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JpeShort, label, hint);
        }

        public void JpoShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JpoShort, label, hint);
        }

        public void JsShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JsShort, label, hint);
        }

        public void JzShort(Label label, Hint hint = Hint.None)
        {
            EmitJcc(InstructionCode.JzShort, label, hint);
        }

        public void Mov(GPReg dst, GPReg src)
        {
            EmitInstruction(InstructionCode.Mov, dst, src);
        }

        public void Mov(GPReg dst, Mem src)
        {
            EmitInstruction(InstructionCode.Mov, dst, src);
        }

        public void Mov(GPReg dst, Imm src)
        {
            EmitInstruction(InstructionCode.Mov, dst, src);
        }

        public void Mov(Mem dst, GPReg src)
        {
            EmitInstruction(InstructionCode.Mov, dst, src);
        }

        public void Mov(Mem dst, Imm src)
        {
            EmitInstruction(InstructionCode.Mov, dst, src);
        }

        public void Pop(GPReg dst)
        {
            if (!dst.IsRegType(RegType.GPW) && !dst.IsRegType(RegType.GPN))
                throw new ArgumentException();

            EmitInstruction(InstructionCode.Pop, dst);
        }

        public void Pop(Mem dst)
        {
            if (dst.Size != 2 && dst.Size != IntPtr.Size)
                throw new ArgumentException();

            EmitInstruction(InstructionCode.Pop, dst);
        }

        public void Push(GPReg src)
        {
            if (!src.IsRegType(RegType.GPW) && !src.IsRegType(RegType.GPN))
                throw new ArgumentException();

            EmitInstruction(InstructionCode.Push, src);
        }

        public void Push(Mem src)
        {
            if (src.Size != 2 && src.Size != IntPtr.Size)
                throw new ArgumentException();

            EmitInstruction(InstructionCode.Push, src);
        }

        public void Push(Imm src)
        {
            EmitInstruction(InstructionCode.Push, src);
        }

        public void Ret()
        {
            EmitInstruction(InstructionCode.Ret);
        }

        public void Ret(Imm imm16)
        {
            EmitInstruction(InstructionCode.Ret, imm16);
        }

        public void Sub(GPReg dst, GPReg src)
        {
            EmitInstruction(InstructionCode.Sub, dst, src);
        }

        public void Sub(GPReg dst, Mem src)
        {
            EmitInstruction(InstructionCode.Sub, dst, src);
        }

        public void Sub(GPReg dst, Imm src)
        {
            EmitInstruction(InstructionCode.Sub, dst, src);
        }

        public void Sub(Mem dst, GPReg src)
        {
            EmitInstruction(InstructionCode.Sub, dst, src);
        }

        public void Sub(Mem dst, Imm src)
        {
            EmitInstruction(InstructionCode.Sub, dst, src);
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

        internal void EmitInstruction(InstructionCode code)
        {
            EmitInstructionImpl(code, null, null, null);
        }

        internal void EmitInstruction(InstructionCode code, Operand operand0)
        {
            Contract.Requires(operand0 != null);

            EmitInstructionImpl(code, operand0, null, null);
        }

        internal void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1)
        {
            Contract.Requires(operand0 != null);
            Contract.Requires(operand1 != null);

            EmitInstructionImpl(code, operand0, operand1, null);
        }

        internal void EmitInstruction(InstructionCode code, Operand operand0, Operand operand1, Operand operand2)
        {
            Contract.Requires(operand0 != null);
            Contract.Requires(operand1 != null);
            Contract.Requires(operand2 != null);

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
            int bLoHiUsed = 0;
            bool forceRexPrefix = (_emitOptions & EmitOptions.RexPrefix) != 0;
            int bufferStartOffset = _buffer.Offset;

#if ASMJIT_DEBUG
  bool assertIllegal = false;
#endif // ASMJIT_DEBUG

            // Convert operands to OPERAND_NONE if needed.
            if (o0 == null)
            {
                o0 = Operand.None;
            }
            else if (o0.IsReg)
            {
                bLoHiUsed |= ((BaseReg)o0).Code & (int)(RegType.GPB_LO | RegType.GPB_LO);
            }
            if (o1 == null)
            {
                o1 = Operand.None;
            }
            else if (o1.IsReg)
            {
                bLoHiUsed |= ((BaseReg)o1).Code & (int)(RegType.GPB_LO | RegType.GPB_LO);
            }
            if (o2 == null)
            {
                o2 = Operand.None;
            }
            else if (o2.IsReg)
            {
                bLoHiUsed |= ((BaseReg)o2).Code & (int)(RegType.GPB_LO | RegType.GPB_LO);
            }

            long beginOffset = Offset;
            InstructionDescription id = InstructionDescription.FromInstruction(code);

            if ((int)code >= _instructionCount)
            {
                Error = Errors.UnknownInstruction;
                goto cleanup;
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
#if ASMJIT_X64
                // Check if there is register that makes this instruction un-encodable.

                forceRexPrefix |= o0.IsExtendedRegisterUsed;
                forceRexPrefix |= o1.IsExtendedRegisterUsed;
                forceRexPrefix |= o2.IsExtendedRegisterUsed;

                if (o0.IsRegType(RegType.GPB_LO) && (((BaseReg)o0).Code & (int)RegIndex.Mask) >= 4)
                    forceRexPrefix = true;
                else if (o1.IsRegType(RegType.GPB_LO) && (((BaseReg)o1).Code & (int)RegIndex.Mask) >= 4)
                    forceRexPrefix = true;
                else if (o2.IsRegType(RegType.GPB_LO) && (((BaseReg)o2).Code & (int)RegIndex.Mask) >= 4)
                    forceRexPrefix = true;

                if ((bLoHiUsed & (int)RegType.GPB_HI) != 0 && forceRexPrefix)
                {
                    goto illegalInstruction;
                }
#endif // ASMJIT_X64

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
                goto cleanup;

            if ((_emitOptions & EmitOptions.LockPrefix) != 0)
            {
                if (!id.IsLockable)
                    goto illegalInstruction;
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
                        EmitImmediate(
                          ((Imm)o1), o0.Size <= 4 ? o0.Size : 4);
                        goto end;
                    }

                    if (o0.IsRegMem && o1.IsImm)
                    {
                        Imm imm = ((Imm)o1);
                        byte immSize = Util.IsInt8(imm.Value.ToInt64()) ? (byte)1 : (o0.Size <= 4 ? o0.Size : (byte)4);

                        EmitX86RM(id.OpCode1 + (o0.Size != 1 ? (immSize != 1 ? 1 : 3) : 0),
                          o0.Size == 2,
                          o0.Size == 8,
                          opReg, o0,
                          immSize, forceRexPrefix);
                        EmitImmediate(
                          ((Imm)o1),
                          immSize);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.BSWAP:
                {
                    if (o0.IsReg)
                    {
                        GPReg dst = ((GPReg)o0);

#if ASMJIT_X64
                        EmitRexR(dst.RegisterType == RegType.GPQ, 1, (byte)dst.Code, forceRexPrefix);
#endif // ASMJIT_X64
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
                          src.Size == 2,
                          src.Size == 8,
                          (byte)id.OpCodeR,
                          dst,
                          1, forceRexPrefix);
                        EmitImmediate(src, 1);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.CALL:
                {
                    if (o0.IsRegTypeMem(RegType.GPN))
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

                            Debug.Assert(offs <= 0);

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
                        Debug.Assert(dst.RegisterType == RegType.GPD || dst.RegisterType == RegType.GPQ);

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
                        EmitImmediate((Imm)o0, 2);
                        EmitImmediate((Imm)o1, 1);
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
                        Debug.Assert(!dst.IsRegType(RegType.GPW));

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
                                EmitImmediate(imm, 1);
                            }
                            else
                            {
                                int immSize = dst.IsRegType(RegType.GPW) ? 2 : 4;
                                EmitX86RM(0x69,
                                  dst.IsRegType(RegType.GPW),
                                  dst.IsRegType(RegType.GPQ), (byte)dst.Code, dst,
                                  immSize, forceRexPrefix);
                                EmitImmediate(imm, (int)immSize);
                            }
                            goto end;
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
                            EmitImmediate(imm, 1);
                        }
                        else
                        {
                            int immSize = dst.IsRegType(RegType.GPW) ? 2 : 4;
                            EmitX86RM(0x69,
                              dst.IsRegType(RegType.GPW),
                              dst.IsRegType(RegType.GPQ), (byte)dst.Code, src,
                              immSize, forceRexPrefix);
                            EmitImmediate(imm, (int)immSize);
                        }
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.INC_DEC:
                {
                    if (o0.IsRegMem)
                    {
                        Operand dst = o0;

                        // INC [r16|r32] in 64 bit mode is not encodable.
#if ASMJIT_X86
                        if ((dst.IsReg) && (dst.IsRegType(RegType.GPW) || dst.IsRegType(RegType.GPD)))
                        {
                            EmitX86Inl(id.OpCode0,
                              dst.IsRegType(RegType.GPW),
                              false, (byte)((BaseReg)dst).Code,
                              false);
                            goto end;
                        }
#endif // ASMJIT_X86

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
                    if (o0.IsLabel)
                    {
                        AssemblerLabelData l_data = _labelData[((Label)o0).Id & Operand.OperandIdValueMask];

                        int hint = o1.IsImm ? (int)((Imm)o1).Value : 0;
                        bool isShortJump = (code >= InstructionDescription.JumpShortBegin && code <= InstructionDescription.JumpShortEnd);

                        // Emit jump hint if configured for that.
                        if ((hint & (int)(Hint.Taken | Hint.NotTaken)) != 0 && (_properties & CompilerProperties.JumpHints) != 0)
                        {
                            if ((hint & (int)Hint.Taken) != 0)
                                EmitByte(HintByteValue.Taken);
                            else if ((hint & (int)Hint.NotTaken) != 0)
                                EmitByte(HintByteValue.NotTaken);
                        }

                        if (l_data.Offset != -1)
                        {
                            // Bound label.
                            const int rel8_size = 2;
                            const int rel32_size = 6;
                            long offs = l_data.Offset - Offset;

                            Debug.Assert(offs <= 0);

                            if (Util.IsInt8(offs - rel8_size))
                            {
                                EmitByte((byte)(0x70 | id.OpCode0));
                                EmitByte((byte)(sbyte)(offs - rel8_size));

                                // Change the instruction code so logger can log instruction correctly.
                                code += InstructionDescription.JumpShortOffset;
                            }
                            else
                            {
                                if (isShortJump && Logger != null)
                                {
                                    Logger.LogString("*** ASSEMBLER WARNING: Emitting long conditional jump, but short jump instruction forced!" + Environment.NewLine);
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

                    break;
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
                        bool isShortJump = (code >= InstructionDescription.JumpShortBegin && code <= InstructionDescription.JumpShortEnd);

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

                                // Change the instruction code so logger can log instruction correctly.
                                code += InstructionDescription.JumpShortOffset;
                            }
                            else
                            {
                                if (isShortJump)
                                {
                                    if (Logger != null && Logger.IsUsed)
                                    {
                                        Logger.LogString("*** ASSEMBLER WARNING: Emitting long jump, but short jump instruction forced!" + Environment.NewLine);
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

                    break;
                }

            case InstructionGroup.LEA:
                {
                    if (o0.IsReg && o1.IsMem)
                    {
                        GPReg dst = (GPReg)o0;
                        Mem src = (Mem)o1;
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
                            Debug.Assert(src.IsRegType(RegType.GPB_LO) ||
                                          src.IsRegType(RegType.GPB_HI) ||
                                          src.IsRegType(RegType.GPW) ||
                                          src.IsRegType(RegType.GPD) ||
                                          src.IsRegType(RegType.GPQ));

                            // ... fall through ...
                            goto case ((int)OperandType.Reg << 4) | (int)OperandType.Mem;
                        }

                    case ((int)OperandType.Reg << 4) | (int)OperandType.Mem:
                        {
                            Debug.Assert(dst.IsRegType(RegType.GPB_LO) ||
                                          dst.IsRegType(RegType.GPB_HI) ||
                                          dst.IsRegType(RegType.GPW) ||
                                          dst.IsRegType(RegType.GPD) ||
                                          dst.IsRegType(RegType.GPQ));

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

                            // In 64 bit mode immediate can be 64-bits long if destination operand
                            // is register (otherwise 32-bits).
                            int immSize = dst.Size;

#if ASMJIT_X64
                            // Optimize instruction size by using 32 bit immediate if value can
                            // fit to it.
                            if (immSize == 8 && Util.IsInt32(((Imm)src).Value.ToInt64()))
                            {
                                EmitX86RM(0xC7,
                                  dst.IsRegType(RegType.GPW),
                                  dst.IsRegType(RegType.GPQ),
                                  0,
                                  dst,
                                  0, forceRexPrefix);
                                immSize = 4;
                            }
                            else
                            {
#endif // ASMJIT_X64
                                EmitX86Inl((dst.Size == 1 ? 0xB0 : 0xB8),
                                  dst.IsRegType(RegType.GPW),
                                  dst.IsRegType(RegType.GPQ),
                                  (byte)((GPReg)dst).Code, forceRexPrefix);
#if ASMJIT_X64
                            }
#endif // ASMJIT_X64

                            EmitImmediate((Imm)src, (int)immSize);
                            goto end;
                        }

                    // Mem <- Reg
                    case ((int)OperandType.Mem << 4) | (int)OperandType.Reg:
                        {
                            Debug.Assert(src.IsRegType(RegType.GPB_LO) ||
                                          src.IsRegType(RegType.GPB_HI) ||
                                          src.IsRegType(RegType.GPW) ||
                                          src.IsRegType(RegType.GPD) ||
                                          src.IsRegType(RegType.GPQ));

                            EmitX86RM(0x88 + (src.Size != 1 ? 1 : 0),
                              src.IsRegType(RegType.GPW),
                              src.IsRegType(RegType.GPQ),
                              (byte)((GPReg)src).Code,
                              dst,
                              0, forceRexPrefix);
                            goto end;
                        }

                    // Mem <- Imm
                    case ((int)OperandType.Mem << 4) | (int)OperandType.Imm:
                        {
                            int immSize = dst.Size <= 4 ? dst.Size : 4;

                            EmitX86RM(0xC6 + (dst.Size != 1 ? 1 : 0),
                              dst.Size == 2,
                              dst.Size == 8,
                              0,
                              dst,
                              immSize, forceRexPrefix);
                            EmitImmediate((Imm)src,
                              (int)immSize);
                            goto end;
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
                            goto illegalInstruction;

                        if (reg.IsRegType(RegType.GPW))
                            EmitByte(0x66);
#if ASMJIT_X64
                        EmitRexR(reg.Size == 8, 0, 0, forceRexPrefix);
#endif // ASMJIT_X64
                        EmitByte((byte)(opCode + (reg.Size != 1 ? 1 : 0)));
                        EmitImmediate(imm, IntPtr.Size);
                        goto end;
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
                            goto illegalInstruction;
                        if (src.Size != 1 && src.Size != 2)
                            goto illegalInstruction;
                        if (src.Size == 2 && dst.Size == 2)
                            goto illegalInstruction;

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

#if ASMJIT_X64
            case InstructionGroup.MOVSXD:
                {
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
#endif // ASMJIT_X64

            case InstructionGroup.PUSH:
                {
                    // This section is only for immediates, memory/register operands are handled in I_POP.
                    if (o0.IsImm)
                    {
                        Imm imm = (Imm)o0;

                        if (Util.IsInt8(imm.Value.ToInt64()))
                        {
                            EmitByte(0x6A);
                            EmitImmediate(imm, 1);
                        }
                        else
                        {
                            EmitByte(0x68);
                            EmitImmediate(imm, 4);
                        }
                        goto end;
                    }

                    // ... goto I_POP ...
                    goto case InstructionGroup.POP;
                }

            case InstructionGroup.POP:
                {
                    if (o0.IsReg)
                    {
                        Debug.Assert(o0.IsRegType(RegType.GPW) || o0.IsRegType(RegType.GPN));
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
                        Debug.Assert(dst.Size != 1);

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
                        opCode++; // D, Q and W form.
                    if (opSize == 2)
                        EmitByte(0x66); // 16-bit prefix.
#if ASMJIT_X64
                    else if (opSize == 8)
                        EmitByte(0x48); // REX.W prefix.
#endif // ASMJIT_X64

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
                        Debug.Assert(Util.IsUInt16(imm.Value.ToInt64()));

                        if (imm.Value == IntPtr.Zero)
                        {
                            EmitByte(0xC3);
                        }
                        else
                        {
                            EmitByte(0xC2);
                            EmitImmediate(imm, 2);
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
                            EmitImmediate(((Imm)o1), 1);
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

                        Debug.Assert(dst.Size == src1.Size);

                        EmitX86RM(id.OpCode0 + (src2.IsReg ? 1 : 0),
                          src1.IsRegType(RegType.GPW),
                          src1.IsRegType(RegType.GPQ),
                          (byte)src1.Code, dst,
                          src2.IsImm ? 1 : 0, forceRexPrefix);
                        if (src2.IsImm)
                            EmitImmediate((Imm)src2, 1);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.TEST:
                {
                    if (o0.IsRegMem && o1.IsReg)
                    {
                        Debug.Assert(o0.Size == o1.Size);
                        EmitX86RM(0x84 + (o1.Size != 1 ? 1 : 0),
                          o1.Size == 2, o1.Size == 8,
                          (byte)((BaseReg)o1).Code,
                          o0,
                          0, forceRexPrefix);
                        goto end;
                    }

                    if (o0.IsRegIndex(0) && o1.IsImm)
                    {
                        int immSize = o0.Size <= 4 ? o0.Size : 4;

                        if (o0.Size == 2)
                            EmitByte(0x66); // 16 bit
#if ASMJIT_X64
                        EmitRexRM(o0.Size == 8, 0, o0, forceRexPrefix);
#endif // ASMJIT_X64
                        EmitByte((byte)(0xA8 + (o0.Size != 1 ? 1 : 0)));
                        EmitImmediate(((Imm)o1), (int)immSize);
                        goto end;
                    }

                    if (o0.IsRegMem && o1.IsImm)
                    {
                        int immSize = o0.Size <= 4 ? o0.Size : 4;

                        if (o0.Size == 2)
                            EmitByte(0x66); // 16 bit
                        EmitSegmentPrefix(o0); // segment prefix
#if ASMJIT_X64
                        EmitRexRM(o0.Size == 8, 0, o0, forceRexPrefix);
#endif // ASMJIT_X64
                        EmitByte((byte)(0xF6 + (o0.Size != 1 ? 1 : 0)));
                        EmitModRM(0, o0, immSize);
                        EmitImmediate((Imm)o1, (int)immSize);
                        goto end;
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
#if ASMJIT_X64
                        EmitRexRM(src.IsRegType(RegType.GPQ), (byte)src.Code, dst, forceRexPrefix);
#endif // ASMJIT_X64

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
                                goto illegalInstruction;
                            i2 = ((X87Reg)o1).RegisterIndex;
                        }
                        else if (i1 != 0 && i2 != 0)
                        {
                            goto illegalInstruction;
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

                    // ... fall through to I_X87_MEM ...
                    goto case InstructionGroup.X87_MEM;
                }

            case InstructionGroup.X87_MEM:
                {
                    if (!o0.IsMem)
                        goto illegalInstruction;
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
                    Debug.Assert(id.OperandFlags[0] != 0);
                    Debug.Assert(id.OperandFlags[1] != 0);

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
                        goto illegalInstruction;
                    }

                    // Illegal.
                    if (o0.IsMem && o1.IsMem)
                        goto illegalInstruction;

                    bool rexw = ((id.OperandFlags[0] | id.OperandFlags[1]) & OperandFlags.NOREX) != 0
                      ? false
                      : (o0.IsRegType(RegType.GPQ) || o0.IsRegType(RegType.GPQ));

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

#if ASMJIT_X64
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
#endif // ASMJIT_X64

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
                        goto illegalInstruction;
                    }

                    int opCode = id.OpCode0;
                    bool isGpdGpq = o0.IsRegType(RegType.GPD) | o0.IsRegType(RegType.GPQ);

                    if (code == InstructionCode.Pextrb && (o0.Size != 0 && o0.Size != 1) && !isGpdGpq)
                        goto illegalInstruction;
                    if (code == InstructionCode.Pextrw && (o0.Size != 0 && o0.Size != 2) && !isGpdGpq)
                        goto illegalInstruction;
                    if (code == InstructionCode.Pextrd && (o0.Size != 0 && o0.Size != 4) && !isGpdGpq)
                        goto illegalInstruction;
                    if (code == InstructionCode.Pextrq && (o0.Size != 0 && o0.Size != 8) && !isGpdGpq)
                        goto illegalInstruction;

                    if (o1.IsRegType(RegType.XMM))
                        opCode |= 0x66000000;

                    if (o0.IsReg)
                    {
                        EmitMmu((uint)opCode, id.OpCodeR != 0 || o0.IsRegType(RegType.GPQ),
                          (byte)((BaseReg)o1).Code,
                          ((BaseReg)o0), (IntPtr)1);
                        EmitImmediate(
                          ((Imm)o2), 1);
                        goto end;
                    }

                    if (o0.IsMem)
                    {
                        EmitMmu((uint)opCode, id.OpCodeR != 0,
                          (byte)((BaseReg)o1).Code,
                          ((Mem)o0), (IntPtr)1);
                        EmitImmediate(
                          ((Imm)o2), 1);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.MMU_RMI:
                {
                    Debug.Assert(id.OperandFlags[0] != 0);
                    Debug.Assert(id.OperandFlags[1] != 0);

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
                        goto illegalInstruction;
                    }

                    int prefix =
                      ((id.OperandFlags[0] & OperandFlags.MM_XMM) == OperandFlags.MM_XMM && o0.IsRegType(RegType.XMM)) ||
                      ((id.OperandFlags[1] & OperandFlags.MM_XMM) == OperandFlags.MM_XMM && o1.IsRegType(RegType.XMM))
                        ? 0x66000000
                        : 0x00000000;
                    bool rexw = ((id.OperandFlags[0] | id.OperandFlags[1]) & OperandFlags.NOREX) != 0
                      ? false
                      : (o0.IsRegType(RegType.GPQ) || o0.IsRegType(RegType.GPQ));

                    // (X)MM <- (X)MM (opcode0)
                    if (o1.IsReg)
                    {
                        if ((id.OperandFlags[1] & (OperandFlags.MM_XMM | OperandFlags.GQD)) == 0)
                            goto illegalInstruction;
                        EmitMmu((uint)id.OpCode0 | (uint)prefix, rexw,
                          (byte)((BaseReg)o0).Code,
                          ((BaseReg)o1), IntPtr.Zero);
                        goto end;
                    }
                    // (X)MM <- Mem (opcode0)
                    if (o1.IsMem)
                    {
                        if ((id.OperandFlags[1] & OperandFlags.MEM) == 0)
                            goto illegalInstruction;
                        EmitMmu((uint)id.OpCode0 | (uint)prefix, rexw,
                          (byte)((BaseReg)o0).Code,
                          ((Mem)o1), IntPtr.Zero);
                        goto end;
                    }
                    // (X)MM <- Imm (opcode1+opcodeR)
                    if (o1.IsImm)
                    {
                        if ((id.OperandFlags[1] & OperandFlags.IMM) == 0)
                            goto illegalInstruction;
                        EmitMmu((uint)id.OpCode1 | (uint)prefix, rexw,
                          (byte)id.OpCodeR,
                          ((BaseReg)o0), (IntPtr)1);
                        EmitImmediate(
                          ((Imm)o1), 1);
                        goto end;
                    }

                    break;
                }

            case InstructionGroup.MMU_RM_IMM8:
                {
                    Debug.Assert(id.OperandFlags[0] != 0);
                    Debug.Assert(id.OperandFlags[1] != 0);

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
                        goto illegalInstruction;
                    }

                    int prefix =
                      ((id.OperandFlags[0] & OperandFlags.MM_XMM) == OperandFlags.MM_XMM && o0.IsRegType(RegType.XMM)) ||
                      ((id.OperandFlags[1] & OperandFlags.MM_XMM) == OperandFlags.MM_XMM && o1.IsRegType(RegType.XMM))
                        ? 0x66000000
                        : 0x00000000;
                    bool rexw = ((id.OperandFlags[0] | id.OperandFlags[1]) & OperandFlags.NOREX) != 0
                      ? false
                      : (o0.IsRegType(RegType.GPQ) || o0.IsRegType(RegType.GPQ));

                    // (X)MM <- (X)MM (opcode0)
                    if (o1.IsReg)
                    {
                        if ((id.OperandFlags[1] & (OperandFlags.MM_XMM | OperandFlags.GQD)) == 0)
                            goto illegalInstruction;
                        EmitMmu((uint)id.OpCode0 | (uint)prefix, rexw,
                          (byte)((BaseReg)o0).Code,
                          ((BaseReg)o1), (IntPtr)1);
                        EmitImmediate(((Imm)o2), 1);
                        goto end;
                    }
                    // (X)MM <- Mem (opcode0)
                    if (o1.IsMem)
                    {
                        if ((id.OperandFlags[1] & OperandFlags.MEM) == 0)
                            goto illegalInstruction;
                        EmitMmu((uint)id.OpCode0 | (uint)prefix, rexw,
                          (byte)((BaseReg)o0).Code,
                          ((Mem)o1), (IntPtr)1);
                        EmitImmediate(((Imm)o2), 1);
                        goto end;
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

        illegalInstruction:
            // Set an error. If we run in release mode assertion will be not used, so we
            // must inform about invalid state.
            Error = Errors.IllegalInstruction;

#if ASMJIT_DEBUG
  assertIllegal = true;
#endif // ASMJIT_DEBUG

        end:
            if ((Logger != null && Logger.IsUsed)
#if ASMJIT_DEBUG
      || assertIllegal
#endif // ASMJIT_DEBUG
)
            {
                StringBuilder buf = new StringBuilder();
                int bufferStopOffset = _buffer.Offset;

                DumpInstruction(buf, new ArraySegment<byte>(_buffer.Data, bufferStartOffset, bufferStopOffset - bufferStartOffset), code, o0, o1, o2);

                if (Logger.LogBinary)
                    DumpComment(buf, new ArraySegment<byte>(GetCode(), (int)beginOffset, (int)(Offset - beginOffset)), _comment);
                else
                    DumpComment(buf, null, _comment);

                // We don't need to null terminate the resulting string.
#if ASMJIT_DEBUG
    if (Logger)
#endif // ASMJIT_DEBUG
                Logger.LogString(buf.ToString());

#if ASMJIT_DEBUG
    if (assertIllegal)
    {
      // Here we need to null terminate.
      buf[0] = '\0';

      // We raise an assertion failure, because in debugging this just shouldn't
      // happen.
      assertionFailure(__FILE__, __LINE__, bufStorage);
    }
#endif // ASMJIT_DEBUG
            }

        cleanup:
            _comment = null;
            _emitOptions = EmitOptions.None;
        }

        private void DumpComment(StringBuilder buf, IList<byte> binaryData, string comment)
        {
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

        private static void DumpInstruction(StringBuilder buf, IList<byte> machineCode, InstructionCode code, Operand o0, Operand o1, Operand o2)
        {
            Contract.Requires(buf != null);
            Contract.Requires(o0 != null);
            Contract.Requires(o1 != null);
            Contract.Requires(o2 != null);

            // Dump the machine code
            for (int i = 0; i < machineCode.Count; i++)
            {
                buf.AppendFormat("{0:X2} ", machineCode[i]);
            }

            // Dump instruction.
            DumpInstructionName(buf, code);

            // Dump operands.
            if (!o0.IsNone)
            {
                buf.Append(' ');
                DumpOperand(buf, o0);
            }

            if (!o1.IsNone)
            {
                buf.Append(", ");
                DumpOperand(buf, o1);
            }

            if (!o2.IsNone)
            {
                buf.Append(", ");
                DumpOperand(buf, o2);
            }
        }

        private static void DumpInstructionName(StringBuilder buf, InstructionCode code)
        {
            Debug.Assert((int)code < _instructionCount);
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

        private static readonly string[] _segmentName =
            {
                "",
                "cs:",
                "ss:",
                "ds:",
                "es:",
                "fs:",
                "gs:",
            };

        internal static void DumpOperand(StringBuilder buf, Operand op)
        {
            if (op.IsReg)
            {
                BaseReg reg = ((BaseReg)op);
                DumpRegister(buf, reg.RegisterType, reg.RegisterIndex);
            }
            else if (op.IsMem)
            {
                bool isAbsolute = false;
                Mem mem = ((Mem)op);

                if (op.Size <= 16)
                {
                    buf.Append(_operandSize[op.Size]);
                }

                buf.Append(_segmentName[(int)mem.SegmentPrefix]);

                buf.Append('[');

                switch (mem.MemoryType)
                {
                case MemoryType.Native:
                    {
                        // [base + index*scale + displacement]
                        DumpRegister(buf, RegType.GPN, mem.Base);
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
                        buf.AppendFormat("{0:x}", mem.Target);
                        break;
                    }
                }

                if (mem.HasIndex)
                {
                    buf.Append(" + ");
                    DumpRegister(buf, RegType.GPN, mem.Index);

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
                buf.Append(i.Value);
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
                    //return buf + sprintf(buf, "%s", _reg8l[(int)index]);
                    buf.Append(_reg8l[(int)index]);
                else
                    //return buf + sprintf(buf, "r%ub", (uint32_t)index);
                    buf.AppendFormat("r{0}b", (int)index);

                return;
            case RegType.GPB_HI:
                if ((int)index < 8)
                    //return buf + sprintf(buf, "%s", _reg8h[(int)index]);
                    buf.Append(_reg8h[(int)index]);
                else
                    //return buf + sprintf(buf, "r%ub", (uint32_t)index);
                    buf.AppendFormat("r{0}b", (int)index);

                return;

            case RegType.GPW:
                if ((int)index < 8)
                    //return buf + sprintf(buf, "%s", _reg16[(int)index]);
                    buf.Append(_reg16[(int)index]);
                else
                    //return buf + sprintf(buf, "r%uw", (uint32_t)index);
                    buf.AppendFormat("r{0}w", (int)index);

                return;

            case RegType.GPD:
                if ((int)index < 8)
                    //return buf + sprintf(buf, "e%s", _reg16[(int)index]);
                    buf.AppendFormat("e{0}", _reg16[(int)index]);
                else
                    //return buf + sprintf(buf, "r%uw", (uint32_t)index);
                    buf.AppendFormat("r{0}d", (int)index);

                return;

            case RegType.GPQ:
                if ((int)index < 8)
                    //return buf + sprintf(buf, "e%s", _reg16[(int)index]);
                    buf.AppendFormat("r{0}", _reg16[(int)index]);
                else
                    //return buf + sprintf(buf, "r%uw", (uint32_t)index);
                    buf.AppendFormat("r{0}", (int)index);

                return;

            case RegType.X87:
                buf.AppendFormat("st{0}", (int)index);
                return;

            case RegType.MM:
                buf.AppendFormat("mm{0}", (int)index);
                return;

            case RegType.XMM:
                buf.AppendFormat("xmm{0}", (int)index);
                return;

            default:
                return;
            }
        }

        private bool CanEmit()
        {
            // If there is an error, we can't emit another instruction until last error
            // is cleared by calling @c setError(ERROR_NONE). If something caused an error
            // while generating code it's probably fatal in all cases. You can't use 
            // generated code, because you are not sure about its status.
            if (_error != 0)
                return false;

            // The ensureSpace() method returns true on success and false on failure. We
            // are catching return value and setting error code here.
            if (EnsureSpace())
                return true;

            // If we are here, there is memory allocation error. Note that this is HEAP
            // allocation error, virtual allocation error can be caused only by
            // AsmJit::VirtualMemory class!
            Error = Errors.NoHeapMemory;
            return false;
        }

        private bool EnsureSpace()
        {
            return _buffer.EnsureSpace();
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

        private void EmitImmediate(Imm imm, int size)
        {
            bool isUnsigned = imm.IsUnsigned;
            IntPtr i = imm.Value;

            if (size == 1 && !isUnsigned)
                EmitByte((byte)(sbyte)i);
            else if (size == 1 && isUnsigned)
                EmitByte((byte)i);
            else if (size == 2 && !isUnsigned)
                EmitWord((ushort)(short)i);
            else if (size == 2 && isUnsigned)
                EmitWord((ushort)i);
            else if (size == 4 && !isUnsigned)
                EmitDWord((uint)(int)i);
            else if (size == 4 && isUnsigned)
                EmitDWord((uint)i);
#if ASMJIT_X64
            else if (size == 8 && !isUnsigned)
                EmitQWord((ulong)(long)i);
            else if (size == 8 && isUnsigned)
                EmitQWord((ulong)i);
#endif // ASMJIT_X64
            else
                Debug.Assert(false);
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

        private static readonly byte[] _prefixes = { 0x00, 0x2E, 0x36, 0x3E, 0x26, 0x64, 0x65 };

        private void EmitSegmentPrefix(Operand rm)
        {
            if (rm.IsMem)
            {
                SegmentPrefix segmentPrefix = ((Mem)rm).SegmentPrefix;
                if (segmentPrefix != 0)
                    EmitByte(_prefixes[(int)segmentPrefix]);
            }
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
#if ASMJIT_X64
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
#endif
        }

        private void EmitRexRM(bool w, byte opReg, Operand rm, bool forceRexPrefix)
        {
#if ASMJIT_X64
            bool r = (opReg & 0x8) != 0;
            bool x = false;
            bool b = false;

            if (rm.IsReg)
            {
                b = (((BaseReg)rm).Code & 0x8) != 0;
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
#endif
        }

        private void EmitModR(byte opReg, byte r)
        {
            EmitMod(3, opReg, r);
        }

        private void EmitModR(byte opReg, BaseReg r)
        {
            throw new NotImplementedException();
        }

        private void EmitModM(byte opReg, Mem mem, int immSize)
        {
            Debug.Assert(mem.OperandType == OperandType.Mem);

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
                // - In 32 bit mode is used absolute addressing model.
                // - In 64 bit mode is used relative addressing model together with
                //   absolute addressing one. Main problem is that if instruction
                //   contains SIB then relative addressing (RIP) is not possible.

#if ASMJIT_X86

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

#else

                // X64 uses relative addressing model
                if (mem.MemoryType == MemoryType.Label)
                {
                    AssemblerLabelData l_data = _labelData[(int)mem.Base & Operand.OperandIdValueMask];

                    if (mem.HasIndex)
                    {
                        // Indexing is not possible
                        Error = Errors.IllegalAddressing;
                        return;
                    }

                    // Relative address (RIP +/- displacement)
                    EmitMod(0, opReg, 5);

                    disp -= (4 + immSize);

                    if (l_data.Offset != -1)
                    {
                        // Bound label.
                        disp = (IntPtr)(disp.ToInt64() + Offset - l_data.Offset);

                        // Add a dummy DWORD.
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
                        if (Logger != null && Logger.IsUsed)
                        {
                            Logger.LogString("*** ASSEMBER WARNING - Absolute address truncated to 32 bits" + Environment.NewLine);
                        }
                        target = (UIntPtr)(target.ToUInt64() & uint.MaxValue);
                    }

                    EmitInt32((int)((uint)target));
                }

#endif // ASMJIT_X64

            }
        }

        private void EmitModRM(byte opReg, Operand op, int immSize)
        {
            Debug.Assert(op.OperandType == OperandType.Reg || op.OperandType == OperandType.Mem);

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
#if ASMJIT_X64
            EmitRexR(rexw, 0, reg, forceRexPrefix);
#endif // ASMJIT_X64

            // instruction opcodes
            if ((opcode & 0x00FF0000) != 0)
                EmitByte((byte)((opcode & 0x00FF0000) >> 16));
            if ((opcode & 0x0000FF00) != 0)
                EmitByte((byte)((opcode & 0x0000FF00) >> 8));

            EmitByte((byte)((opcode & 0x000000FF) + (reg & 0x7)));
        }

        private void EmitX86RM(int opCode, bool i16bit, bool rexw, byte o, Operand op, int immSize, bool forceRexPrefix)
        {
            // 16 bit prefix.
            if (i16bit)
                EmitByte(0x66);

            // Segment prefix.
            EmitSegmentPrefix(op);

            // Instruction prefix.
            if (((uint)opCode & 0xFF000000) != 0)
                EmitByte((byte)((opCode & 0xFF000000) >> 24));

            // REX prefix.
#if ASMJIT_X64
            EmitRexRM(rexw, o, op, forceRexPrefix);
#endif // ASMJIT_X64

            // Instruction opcodes.
            if ((opCode & 0x00FF0000) != 0)
                EmitByte((byte)((opCode & 0x00FF0000) >> 16));
            if ((opCode & 0x0000FF00) != 0)
                EmitByte((byte)((opCode & 0x0000FF00) >> 8));
            EmitByte((byte)(opCode & 0x000000FF));

            // Mod R/M.
            EmitModRM(o, op, immSize);
        }

        //! @brief Emit FPU instruction with no operands.
        private void EmitFpu(int opCode)
        {
            throw new NotImplementedException();
        }

        //! @brief Emit FPU instruction with one operand @a sti (index of FPU register).
        private void EmitFpuSTI(int opCode, int sti)
        {
            throw new NotImplementedException();
        }

        //! @brief Emit FPU instruction with one operand @a opReg and memory operand @a mem.
        private void EmitFpuMEM(int opCode, byte opReg, Mem mem)
        {
            throw new NotImplementedException();
        }

        //! @brief Emit MMX/SSE instruction.
        private void EmitMmu(uint opCode, bool rexw, byte opReg, Operand src, IntPtr immSize)
        {
            throw new NotImplementedException();
        }

        //! @brief Emit displacement.
        private LabelLink EmitDisplacement(AssemblerLabelData l_data, long inlinedDisplacement, int size)
        {
            Debug.Assert(l_data.Offset == -1);
            Debug.Assert(size == 1 || size == 4);

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

        //! @brief Emit relative relocation to absolute pointer @a target. It's needed
        //! to add what instruction is emitting this, because in x64 mode the relative
        //! displacement can be impossible to calculate and in this case the trampoline
        //! is used.
        private void EmitJmpOrCallReloc(int instruction, IntPtr target)
        {
            RelocationData rd = new RelocationData();

            rd.Type = RelocationType.AbsoluteToRelativeTrampoline;

#if ASMJIT_X64
            // If we are compiling in 64-bit mode, we can use trampoline if relative jump
            // is not possible.
            _trampolineSize += TrampolineWriter.TRAMPOLINE_SIZE;
#endif // ARCHITECTURE_SPECIFIC

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

        //! @brief Private method for emitting jcc.
        private void EmitJcc(InstructionCode code, Label label, Hint hint)
        {
            throw new NotImplementedException();
        }
    }
}
