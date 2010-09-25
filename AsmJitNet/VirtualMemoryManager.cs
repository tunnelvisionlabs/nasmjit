namespace AsmJitNet
{
    using System;
    using System.Runtime.InteropServices;
    using Debug = System.Diagnostics.Debug;
    using System.Diagnostics.Contracts;
    using System.Diagnostics;
    using TextWriter = System.IO.TextWriter;
    using RuntimeHelpers = System.Runtime.CompilerServices.RuntimeHelpers;

    public class VirtualMemoryManager : MemoryManager
    {
        private readonly MemoryManagerPrivate _data;

#if ASMJIT_WINDOWS
        public VirtualMemoryManager()
        {
            _data = new MemoryManagerPrivate(UnsafeNativeMethods.GetCurrentProcess());
        }

        public VirtualMemoryManager(IntPtr processHandle)
        {
            _data = new MemoryManagerPrivate(processHandle);
        }
#else
        public VirtualMemoryManager()
        {
            _data = new MemoryManagerPrivate();
        }
#endif

        public override long UsedBytes
        {
            get
            {
                return _data.UsedBytes;
            }
        }

        public override long AllocatedBytes
        {
            get
            {
                return _data.AllocatedBytes;
            }
        }

        public bool KeepVirtualMemory
        {
            get
            {
                return _data.KeepVirtualMemory;
            }

            set
            {
                _data.KeepVirtualMemory = value;
            }
        }

        public override IntPtr Alloc(long size, MemoryAllocType allocType)
        {
            if (allocType == MemoryAllocType.Permanent)
                return _data.AllocPermanent(size);
            else
                return _data.AllocFreeable(size);
        }

        public override bool Free(IntPtr address)
        {
            return _data.Free(address);
        }

        public override void FreeAll()
        {
            _data.FreeAll(false);
        }

        [Conditional("ASMJIT_MEMORY_MANAGER_DUMP")]
        public void Dump(TextWriter writer)
        {
            if (writer == null)
                throw new ArgumentNullException("writer");
            Contract.EndContractBlock();

            _data.Dump(writer);
        }

        private class MemoryManagerPrivate
        {
            /// <summary>
            /// Lock for thread safety
            /// </summary>
            private readonly Lock _lock = new Lock();

#if ASMJIT_WINDOWS
            private readonly IntPtr _process;
#endif

            /// <summary>
            /// Default node size
            /// </summary>
            private int _newChunkSize = 65536;
            /// <summary>
            /// Default node density
            /// </summary>
            private int _newChunkDensity = 64;
            /// <summary>
            /// How many bytes are allocated
            /// </summary>
            private long _allocated;
            /// <summary>
            /// How many bytes are used
            /// </summary>
            private long _used;

            // Memory nodes list.
            private M_Node _first;
            private M_Node _last;
            private M_Node _optimal;

            /// <summary>
            /// Memory nodes tree
            /// </summary>
            private M_Node _root;

            /// <summary>
            /// Permanent memory
            /// </summary>
            private M_PermanentNode _permanent;

            private bool _keepVirtualMemory;

#if ASMJIT_WINDOWS
            public MemoryManagerPrivate(IntPtr processHandle)
            {
                _process = processHandle;
            }
#else
            public MemoryManagerPrivate()
            {
            }
#endif

            public long UsedBytes
            {
                get
                {
                    return _used;
                }
            }

            public long AllocatedBytes
            {
                get
                {
                    return _allocated;
                }
            }

            public bool KeepVirtualMemory
            {
                get
                {
                    return _keepVirtualMemory;
                }

                set
                {
                    _keepVirtualMemory = value;
                }
            }

            public void Dump(TextWriter writer)
            {
                if (writer == null)
                    throw new ArgumentNullException("writer");
                Contract.EndContractBlock();

                GraphVizContext ctx = new GraphVizContext(writer);
                ctx.Writer.WriteLine("digraph {");
                if (_root != null)
                    ctx.DumpNode(_root);
                ctx.Writer.WriteLine("}");
            }

            // [Allocation]

            public M_Node CreateNode(long size, int density)
            {
                int vsize;
                IntPtr vmem = AllocVirtualMemory((int)size, out vsize);
                if (vmem == IntPtr.Zero)
                    throw new OutOfMemoryException();

                M_Node node = new M_Node(vmem, vsize, density);
                return node;
            }

            public IntPtr AllocPermanent(long vsize)
            {
                const int permanentAlignment = 32;
                const int permanentNodeSize = 32768;

                long over = vsize % permanentAlignment;
                if (over != 0)
                    over = permanentAlignment - over;
                long alignedSize = vsize + over;

                lock (_lock)
                {

                    M_PermanentNode node = _permanent;

                    // Try to find space in allocated chunks.
                    while (node != null && alignedSize > node.Available)
                        node = node.Previous;

                    // Or allocate new node.
                    if (node == null)
                    {
                        long nodeSize = permanentNodeSize;
                        if (vsize > nodeSize)
                            nodeSize = vsize;

                        try
                        {
                            node = new M_PermanentNode();
                            int size;
                            node.Memory = AllocVirtualMemory((int)nodeSize, out size);
                            node.Size = size;
                        }
                        catch (OutOfMemoryException)
                        {
                            return IntPtr.Zero;
                        }

                        node.Used = 0;
                        node.Previous = _permanent;
                        _permanent = node;
                    }

                    // Finally, copy function code to our space we reserved for.
                    IntPtr result = (IntPtr)(node.Memory.ToInt64() + node.Used);

                    // Update Statistics.
                    node.Used += alignedSize;
                    _used += alignedSize;

                    // Code can be null to only reserve space for code.
                    return result;
                }
            }

            private static readonly int BITS_PER_ENTITY = IntPtr.Size * 8;

            public IntPtr AllocFreeable(long vsize)
            {
                long i;               // Current index.
                long need;            // How many we need to be freed.
                long minVSize;

                // Align to 32 bytes (our default alignment).
                vsize = (vsize + 31) & ~31L;
                if (vsize == 0)
                    return IntPtr.Zero;

                lock (_lock)
                {
                    M_Node node = _optimal;

                    minVSize = _newChunkSize;

                    // Try to find memory block in existing nodes.
                    while (node != null)
                    {
                        // Skip this node?
                        if ((node.Available < vsize) ||
                            (node.LargestBlock < vsize && node.LargestBlock != 0))
                        {
                            M_Node next = node.Next;
                            if (node.Available < minVSize && node == _optimal && next != null)
                                _optimal = next;
                            node = next;
                            continue;
                        }

                        int[] up = node.BaUsed;    // Current ubits address.
                        int upIndex = 0;
                        long ubits;                 // Current ubits[0] value.
                        long bit;                   // Current bit mask.
                        long blocks = node.Blocks; // Count of blocks in node.
                        long cont = 0;              // How many bits are currently freed in find loop.
                        long maxCont = 0;           // Largest continuous block (bits count).
                        long j;

                        need = (vsize + node.Density - 1) / node.Density;
                        i = 0;

                        // Try to find node that is large enough.
                        while (i < blocks)
                        {
                            ubits = up[upIndex++];

                            // Fast skip used blocks.
                            if (ubits == (-1))
                            {
                                if (cont > maxCont)
                                    maxCont = cont;
                                cont = 0;

                                i += BITS_PER_ENTITY;
                                continue;
                            }

                            long max = BITS_PER_ENTITY;
                            if (i + max > blocks)
                                max = blocks - i;

                            for (j = 0, bit = 1; j < max; bit <<= 1)
                            {
                                j++;
                                if ((ubits & bit) == 0)
                                {
                                    if (++cont == need)
                                    {
                                        i += j;
                                        i -= cont;
                                        goto found;
                                    }
                                    continue;
                                }

                                if (cont > maxCont)
                                    maxCont = cont;
                                cont = 0;
                            }

                            i += BITS_PER_ENTITY;
                        }

                        // Because we traversed entire node, we can set largest node size that
                        // will be used to cache next traversing..
                        node.LargestBlock = maxCont * node.Density;

                        node = node.Next;
                    }

                    // If we are here, we failed to find existing memory block and we must
                    // allocate new.
                    {
                        long chunkSize = _newChunkSize;
                        if (chunkSize < vsize)
                            chunkSize = vsize;

                        node = CreateNode(chunkSize, _newChunkDensity);
                        if (node == null)
                            return IntPtr.Zero;

                        // Link with others.
                        node.Previous = _last;

                        if (_first == null)
                        {
                            _first = node;
                            _last = node;
                            _optimal = node;
                        }
                        else
                        {
                            node.Previous = _last;
                            _last.Next = node;
                            _last = node;
                        }

                        // Update binary tree.
                        NlInsertNode(node);

                        // Alloc first node at start.
                        i = 0;
                        need = (vsize + node.Density - 1) / node.Density;

                        // Update statistics.
                        _allocated += node.Size;
                    }

                found:
                    //Console.Error.WriteLine("ALLOCATED BLOCK {0} ({1})", node.Memory.ToInt32() + i * node.Density, need * node.Density);

                    // Update bits.
                    SetBits(node.BaUsed, (int)i, (int)need);
                    SetBits(node.BaCont, (int)i, (int)need - 1);

                    // Update statistics.
                    {
                        long u = need * node.Density;
                        node.Used += u;
                        node.LargestBlock = 0;
                        _used += u;
                    }

                    // And return pointer to allocated memory.
                    IntPtr result = (IntPtr)(node.Memory.ToInt64() + i * node.Density);
                    if (result.ToInt64() < node.Memory.ToInt64() || result.ToInt64() >= node.Memory.ToInt64() + node.Size)
                        throw new AssemblerException();

                    return result;
                }
            }

            private static void SetBits(int[] buf, int offset, int count)
            {
                Contract.Requires(buf != null);

                if (count == 0)
                    return;

                int i = offset / BITS_PER_ENTITY; // sysuint_t[]
                int j = offset % BITS_PER_ENTITY; // sysuint_t[][] bit index

                // How many bytes process in first group.
                int c = BITS_PER_ENTITY - j;
                if (c > count)
                    c = count;

                // Offset.
                int bufIndex = i;
                buf[bufIndex++] |= (int)(((~0U) >> (BITS_PER_ENTITY - c)) << j);
                count -= c;

                while (count >= BITS_PER_ENTITY)
                {
                    buf[bufIndex++] = -1;
                    count -= BITS_PER_ENTITY;
                }

                if (count != 0)
                {
                    buf[bufIndex] |= (int)(((~0U) >> (BITS_PER_ENTITY - count)));
                }
            }

            private static void ClearBits(int[] buf, int offset, int count)
            {
                Contract.Requires(buf != null);

                if (count == 0)
                    return;

                int i = offset / BITS_PER_ENTITY; // sysuint_t[]
                int j = offset % BITS_PER_ENTITY; // sysuint_t[][] bit index

                // How many bytes process in first group.
                int c = BITS_PER_ENTITY - j;
                if (c > count)
                    c = count;

                // Offset.
                int bufIndex = i;
                buf[bufIndex++] &= ~(int)(((~0U) >> (BITS_PER_ENTITY - c)) << j);
                count -= c;

                while (count >= BITS_PER_ENTITY)
                {
                    buf[bufIndex++] = 0;
                    count -= BITS_PER_ENTITY;
                }

                if (count != 0)
                {
                    buf[bufIndex] &= ~(int)(~0U << count);
                }
            }

            public bool Free(IntPtr address)
            {
                if (address == null)
                    return true;

                lock (_lock)
                {

                    M_Node node = NlFindPtr(address);
                    if (node == null)
                        return false;

                    IntPtr offset = (IntPtr)(address.ToInt64() - node.Memory.ToInt64());
                    int bitpos = (int)(offset.ToInt64() / node.Density);
                    int i = (bitpos / BITS_PER_ENTITY);
                    int j = (bitpos % BITS_PER_ENTITY);

                    int cont = 0;

                    unsafe
                    {
                        fixed (int* upPtr = node.BaUsed)
                        {
                            fixed (int* cpPtr = node.BaCont)
                            {
                                // current ubits address
                                uint* up = (uint*)upPtr + i;
                                // current cbits address
                                uint* cp = (uint*)cpPtr + i;
                                uint ubits = *up;             // Current ubits[0] value.
                                uint cbits = *cp;             // Current cbits[0] value.
                                uint bit = 1U << j; // Current bit mask.

                                bool stop;

                                for (; ; )
                                {
                                    stop = (cbits & bit) == 0;
                                    ubits &= ~bit;
                                    cbits &= ~bit;

                                    j++;
                                    bit <<= 1;
                                    cont++;

                                    if (stop || j == BITS_PER_ENTITY)
                                    {
                                        *up = ubits;
                                        *cp = cbits;

                                        if (stop)
                                            break;

                                        ubits = *++up;
                                        cbits = *++cp;

                                        j = 0;
                                        bit = 1;
                                    }
                                }
                            }
                        }
                    }

                    // If the freed block is fully allocated node, need to update optimal
                    // pointer in memory manager.
                    if (node.Used == node.Size)
                    {
                        M_Node cur = _optimal;

                        do
                        {
                            cur = cur.Previous;
                            if (cur == node)
                            {
                                _optimal = node;
                                break;
                            }
                        } while (cur != null);
                    }

                    //Console.Error.WriteLine("FREEING {0} ({1})", address, cont * node.Density);

                    // Statistics.
                    cont *= node.Density;
                    if (node.LargestBlock < cont)
                        node.LargestBlock = cont;
                    node.Used -= cont;
                    _used -= cont;

                    // If page is empty, we can free it.
                    if (node.Used == 0)
                    {
                        _allocated -= node.Size;
                        NlRemoveNode(node);
                        FreeVirtualMemory(node.Memory, (int)node.Size);

                        M_Node next = node.Next;
                        M_Node prev = node.Previous;

                        if (prev!=null)
                        {
                            prev.Next = next;
                        }
                        else
                        {
                            _first = next;
                        }

                        if (next!=null)
                        {
                            next.Previous = prev;
                        }
                        else
                        {
                            _last = prev;
                        }

                        if (_optimal == node)
                        {
                            _optimal = prev != null ? prev : next;
                        }
                    }
                }

                return true;
            }

            public void FreeAll(bool keepVirtualMemory)
            {
                M_Node node = _first;

                while (node != null)
                {
                    M_Node next = node.Next;

                    if (!keepVirtualMemory)
                        FreeVirtualMemory(node.Memory, (int)node.Size);

                    node = next;
                }

                _allocated = 0;
                _used = 0;

                _root = null;
                _first = null;
                _last = null;
                _optimal = null;
            }

            // Helpers to avoid ifdefs in the code.
            public IntPtr AllocVirtualMemory(int size, out int vsize)
            {
#if !ASMJIT_WINDOWS
                return VirtualMemory.Alloc(size, out vsize, true);
#else
                return VirtualMemory.AllocProcessMemory(_process, size, out vsize, true);
#endif
            }

            public void FreeVirtualMemory(IntPtr vmem, int vsize)
            {
#if !ASMJIT_WINDOWS
                VirtualMemory.Free(vmem, vsize);
#else
                VirtualMemory.FreeProcessMemory(_process, vmem, vsize);
#endif
            }

            // [NodeList LLRB-Tree]

            public static bool NlCheckTree(M_Node node)
            {
                bool result = true;
                if (node == null)
                    return result;

                // If I am red & any of the children are red then its a red violation.
                if (NlIsRed(node) && (NlIsRed(node.Left) || NlIsRed(node.Right)))
                    return false;

                if (node.Left != null && node.Memory.ToInt64() < node.Left.Memory.ToInt64())
                    return false;
                if (node.Right != null && node.Memory.ToInt64() > node.Right.Memory.ToInt64())
                    return false;

                result &= NlCheckTree(node.Left);
                result &= NlCheckTree(node.Right);
                return result;
            }

            public static bool NlIsRed(M_Node n)
            {
                Contract.Ensures(!Contract.Result<bool>() || n != null);

                return n != null && n.Color == NodeColor.Red;
            }

            public static M_Node NlRotateLeft(M_Node n)
            {
                Contract.Requires(n != null);
                Contract.Requires(n.Right != null);
                Contract.Ensures(Contract.Result<M_Node>() != null);
                Contract.Ensures(Contract.Result<M_Node>().Left != null);

                M_Node x = n.Right;

                n.Right = x.Left;
                x.Left = n;

                x.Color = n.Color;
                n.Color = NodeColor.Red;

                return x;
            }

            public static M_Node NlRotateRight(M_Node n)
            {
                Contract.Requires(n != null);
                Contract.Requires(n.Left != null);
                Contract.Ensures(Contract.Result<M_Node>() != null);
                Contract.Ensures(Contract.Result<M_Node>().Right != null);

                M_Node x = n.Left;

                n.Left = x.Right;
                x.Right = n;

                x.Color = n.Color;
                n.Color = NodeColor.Red;
                return x;
            }

            public static void NlFlipColor(M_Node n)
            {
                Contract.Requires(n != null);
                Contract.Requires(n.Left != null);
                Contract.Requires(n.Right != null);

                n.Color = (n.Color == NodeColor.Black) ? NodeColor.Red : NodeColor.Black;
                n.Left.Color = (n.Left.Color == NodeColor.Black) ? NodeColor.Red : NodeColor.Black;
                n.Right.Color = (n.Right.Color == NodeColor.Black) ? NodeColor.Red : NodeColor.Black;
            }

            public static M_Node NlMoveRedLeft(M_Node h)
            {
                Contract.Requires(h != null);
                Contract.Requires(h.Left != null);
                Contract.Requires(h.Right != null);
                Contract.Ensures(Contract.Result<M_Node>() != null);

                NlFlipColor(h);
                if (NlIsRed(h.Right.Left))
                {
                    h.Right = NlRotateRight(h.Right);
                    h = NlRotateLeft(h);
                    NlFlipColor(h);
                }
                return h;
            }

            public static M_Node NlMoveRedRight(M_Node h)
            {
                Contract.Requires(h != null);
                Contract.Requires(h.Left != null);
                Contract.Requires(h.Right != null);
                Contract.Ensures(Contract.Result<M_Node>() != null);

                NlFlipColor(h);
                if (NlIsRed(h.Left.Left))
                {
                    h = NlRotateRight(h);
                    NlFlipColor(h);
                }
                return h;
            }

            public static M_Node NlFixUp(M_Node h)
            {
                Contract.Requires(h != null);
                Contract.Ensures(Contract.Result<M_Node>() != null);

                if (NlIsRed(h.Right))
                    h = NlRotateLeft(h);
                if (NlIsRed(h.Left) && NlIsRed(h.Left.Left))
                    h = NlRotateRight(h);
                if (NlIsRed(h.Left) && NlIsRed(h.Right))
                    NlFlipColor(h);

                return h;
            }

            public void NlInsertNode(M_Node n)
            {
                Contract.Requires(n != null);

                _root = NlInsertNode_(_root, n);
                _root.Color = NodeColor.Black;
                Debug.Assert(NlCheckTree(_root));
            }

            public static M_Node NlInsertNode_(M_Node h, M_Node n)
            {
                Contract.Requires(n != null);
                Contract.Ensures(Contract.Result<M_Node>() != null);

                if (h == null)
                    return n;

                if (n.Memory.ToInt64() < h.Memory.ToInt64())
                    h.Left = NlInsertNode_(h.Left, n);
                else
                    h.Right = NlInsertNode_(h.Right, n);

                if (NlIsRed(h.Right) && !NlIsRed(h.Left))
                    h = NlRotateLeft(h);
                if (NlIsRed(h.Left) && NlIsRed(h.Left.Left))
                    h = NlRotateRight(h);

                if (NlIsRed(h.Left) && NlIsRed(h.Right))
                    NlFlipColor(h);

                return h;
            }

            public void NlRemoveNode(M_Node n)
            {
                Contract.Requires(n != null);

                _root = NlRemoveNode_(_root, n);
                if (_root != null)
                    _root.Color = NodeColor.Black;

                if (NlFindPtr(n.Memory) != null)
                    throw new AssemblerException();
                Debug.Assert(NlCheckTree(_root));
            }

            public M_Node NlRemoveNode_(M_Node h, M_Node n)
            {
                Contract.Requires(h != null);
                Contract.Requires(n != null);

                if (n.Memory.ToInt64() < h.Memory.ToInt64())
                {
                    if (!NlIsRed(h.Left) && !NlIsRed(h.Left.Left))
                        h = NlMoveRedLeft(h);
                    h.Left = NlRemoveNode_(h.Left, n);
                }
                else
                {
                    if (NlIsRed(h.Left))
                        h = NlRotateRight(h);

                    if (h == n && (h.Right == null))
                        return null;

                    if (!NlIsRed(h.Right) && !NlIsRed(h.Right.Left))
                        h = NlMoveRedRight(h);

                    if (h == n)
                    {
                        // Get minimum node.
                        h = n.Right;
                        while (h.Left != null)
                            h = h.Left;

                        M_Node _l = n.Left;
                        M_Node _r = NlRemoveMin(n.Right);

                        h.Left = _l;
                        h.Right = _r;
                        h.Color = n.Color;
                    }
                    else
                    {
                        h.Right = NlRemoveNode_(h.Right, n);
                    }
                }

                return NlFixUp(h);
            }

            public M_Node NlRemoveMin(M_Node h)
            {
                Contract.Requires(h != null);

                if (h.Left == null)
                    return null;
                if (!NlIsRed(h.Left) && !NlIsRed(h.Left.Left))
                    h = NlMoveRedLeft(h);
                h.Left = NlRemoveMin(h.Left);
                return NlFixUp(h);
            }

            public M_Node NlFindPtr(IntPtr mem)
            {
                M_Node cur = _root;

                while (cur != null)
                {
                    IntPtr curMem = cur.Memory;
                    if (mem.ToInt64() < curMem.ToInt64())
                    {
                        cur = cur.Left;
                        continue;
                    }
                    else
                    {
                        long curEnd = curMem.ToInt64() + cur.Size;
                        if (mem.ToInt64() >= curEnd)
                        {
                            cur = cur.Right;
                            continue;
                        }
                        break;
                    }
                }

                return cur;
            }

            public sealed class M_Node
            {
                private readonly IntPtr _memory;
                private readonly long _size;
                private readonly int _density;

                // Implementation is based on:
                //   Left-leaning Red-Black Trees by Robert Sedgewick.

                public M_Node(IntPtr memory, long size, int density)
                {
                    _memory = memory;
                    _size = size;
                    _density = density;

                    long basize = (((Blocks + 7) >> 3) + sizeof(int) - 1) & ~(uint)(sizeof(int) - 1);
                    Color = NodeColor.Red;
                    LargestBlock = size;
                    BaUsed = new int[basize / sizeof(int)];
                    BaCont = new int[basize / sizeof(int)];
                }

                public M_Node Previous
                {
                    get;
                    set;
                }

                public M_Node Next
                {
                    get;
                    set;
                }

                public M_Node Left
                {
                    get;
                    set;
                }

                public M_Node Right
                {
                    get;
                    set;
                }

                public NodeColor Color
                {
                    get;
                    set;
                }

                public IntPtr Memory
                {
                    get
                    {
                        return _memory;
                    }
                }

                public long Size
                {
                    get
                    {
                        return _size;
                    }
                }

                public int Density
                {
                    get
                    {
                        return _density;
                    }
                }

                public long Blocks
                {
                    get
                    {
                        return Size / Density;
                    }
                }

                public long Used
                {
                    get;
                    set;
                }

                public long LargestBlock
                {
                    get;
                    set;
                }

                public int[] BaUsed
                {
                    get;
                    set;
                }

                public int[] BaCont
                {
                    get;
                    set;
                }

                public long Available
                {
                    get
                    {
                        return Size - Used;
                    }
                }
            }

            public enum NodeColor
            {
                Black,
                Red
            }

            private sealed class Lock
            {
            }

            private sealed class M_PermanentNode
            {
                public IntPtr Memory
                {
                    get;
                    set;
                }

                public long Size
                {
                    get;
                    set;
                }

                public long Used
                {
                    get;
                    set;
                }

                public M_PermanentNode Previous
                {
                    get;
                    set;
                }

                public long Available
                {
                    get
                    {
                        return Size - Used;
                    }
                }
            }

            private class GraphVizContext
            {
                private readonly System.IO.TextWriter _writer;

                public GraphVizContext(TextWriter writer)
                {
                    if (writer == null)
                        throw new ArgumentNullException("writer");
                    Contract.EndContractBlock();

                    _writer = writer;
                }

                public TextWriter Writer
                {
                    get
                    {
                        return _writer;
                    }
                }

                public void DumpNode(M_Node node)
                {
                    Writer.WriteLine("  NODE_{0:X} [shape=record, style=filled, color={1}, label=\"<L>|<C>Mem: {2:X}, Used: {3}/{4}|<R>\"];",
                        RuntimeHelpers.GetHashCode(node),
                        node.Color == NodeColor.Red ? "red" : "gray",
                        node.Memory, node.Used, node.Size);

                    if (node.Left != null)
                        Connect(node, node.Left, "L");
                    if (node.Right != null)
                        Connect(node, node.Right, "R");
                }

                public void Connect(M_Node node, M_Node other, string destination)
                {
                    DumpNode(other);

                    Writer.Write("  NODE_{0:X}:{1} -> NODE_{2:X}:C", RuntimeHelpers.GetHashCode(node), destination, RuntimeHelpers.GetHashCode(other));
                    if (other.Color == NodeColor.Red)
                        Writer.Write(" [style=bold, color=red]");
                    Writer.WriteLine(";");
                }
            }
        }

        private static class UnsafeNativeMethods
        {
            [DllImport("kernel32.dll")]
            public static extern IntPtr GetCurrentProcess();
        }
    }
}
