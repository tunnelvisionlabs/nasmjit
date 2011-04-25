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

        public override bool Shrink(IntPtr address, IntPtr used)
        {
            return _data.Shrink(address, used);
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
            private MemNode _first;
            private MemNode _last;
            private MemNode _optimal;

            /// <summary>
            /// Memory nodes tree
            /// </summary>
            private MemNode _root;

            /// <summary>
            /// Permanent memory
            /// </summary>
            private PermanentNode _permanent;

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

            public MemNode CreateNode(long size, int density)
            {
                int vsize;
                IntPtr vmem = AllocVirtualMemory((int)size, out vsize);
                if (vmem == IntPtr.Zero)
                    throw new OutOfMemoryException();

                MemNode node = new MemNode(vmem, vsize, density);
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

                    PermanentNode node = _permanent;

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
                            node = new PermanentNode();
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
                    MemNode node = _optimal;

                    minVSize = _newChunkSize;

                    // Try to find memory block in existing nodes.
                    while (node != null)
                    {
                        // Skip this node?
                        if ((node.Available < vsize) ||
                            (node.LargestBlock < vsize && node.LargestBlock != 0))
                        {
                            MemNode next = node.Next;
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

                        // Update binary tree.
                        InsertNode(node);
                        Debug.Assert(CheckTree());

                        // Alloc first node at start.
                        i = 0;
                        need = (vsize + node.Density - 1) / node.Density;

                        // Update statistics.
                        _allocated += node.Size;
                    }

                found:
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
                    if (result.ToInt64() < node.Memory.ToInt64() || result.ToInt64() > node.Memory.ToInt64() + node.Size - vsize)
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

                    MemNode node = FindPtr(address);
                    if (node == null)
                        return false;

                    IntPtr offset = (IntPtr)(address.ToInt64() - node.Memory.ToInt64());
                    int bitpos = (int)(offset.ToInt64() / node.Density);
                    int i = (bitpos / BITS_PER_ENTITY);

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
                                uint bit = 1U << (bitpos % BITS_PER_ENTITY);

                                bool stop;

                                for (; ; )
                                {
                                    stop = (cbits & bit) == 0;
                                    ubits &= ~bit;
                                    cbits &= ~bit;

                                    bit <<= 1;
                                    cont++;

                                    if (stop || bit == 0)
                                    {
                                        *up = ubits;
                                        *cp = cbits;

                                        if (stop)
                                            break;

                                        ubits = *++up;
                                        cbits = *++cp;
                                        bit = 1;
                                    }
                                }
                            }
                        }
                    }

                    // If the freed block is fully allocated node then it's needed to
                    // update 'optimal' pointer in memory manager.
                    if (node.Used == node.Size)
                    {
                        MemNode cur = _optimal;

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

                    // Statistics.
                    cont *= node.Density;
                    if (node.LargestBlock < cont)
                        node.LargestBlock = cont;
                    node.Used -= cont;
                    _used -= cont;

                    // If page is empty, we can free it.
                    if (node.Used == 0)
                    {
                        // Free memory associated with node (this memory is not accessed
                        // anymore so it's safe).
                        FreeVirtualMemory(node.Memory, (int)node.Size);

                        node.BaUsed = null;
                        node.BaCont = null;

                        // Statistics.
                        _allocated -= node.Size;

                        // Remove node. This function can return different node than
                        // passed into, but data is copied into previous node if needed.
                        Debug.Assert(CheckTree());

                        _allocated -= node.Size;
                        RemoveNode(node);
                        FreeVirtualMemory(node.Memory, (int)node.Size);
                    }
                }

                return true;
            }

            public bool Shrink(IntPtr address, IntPtr used)
            {
                if (address == IntPtr.Zero)
                    return false;

                if (used == IntPtr.Zero)
                    return Free(address);

                lock (_lock)
                {
                    MemNode node = FindPtr(address);
                    if (node == null)
                        return false;

                    long offset = address.ToInt64() - node.Memory.ToInt64();
                    long bitpos = offset / node.Density;
                    long i = (bitpos / BITS_PER_ENTITY);

                    long upIndex = /*node.BaUsed +*/ i;  // Current ubits address.
                    long cpIndex = /*node.BaCont +*/ i;  // Current cbits address.
                    int ubits = node.BaUsed[upIndex];             // Current ubits[0] value.
                    int cbits = node.BaCont[cpIndex];             // Current cbits[0] value.
                    int bit = 1 << (int)(bitpos % BITS_PER_ENTITY);

                    long cont = 0;
                    long usedBlocks = (used.ToInt64() + node.Density - 1) / node.Density;

                    bool stop;

                    // Find the first block we can mark as free.
                    for (; ; )
                    {
                        stop = (cbits & bit) == 0;
                        if (stop)
                            return true;

                        if (++cont == usedBlocks)
                            break;

                        bit <<= 1;
                        if (bit == 0)
                        {
                            upIndex++;
                            ubits = node.BaUsed[upIndex];
                            cpIndex++;
                            cbits = node.BaCont[cpIndex];
                            bit = 1;
                        }
                    }

                    // Free the tail blocks.
                    cont = -1;

                    for (bool firstPass = true; ; firstPass = false)
                    {
                        if (!firstPass)
                        {
                            stop = (cbits & bit) == 0;
                            ubits &= ~bit;
                        }

                        cbits &= ~bit;

                        bit <<= 1;
                        cont++;

                        if (stop || bit == 0)
                        {
                            node.BaUsed[upIndex] = ubits;
                            node.BaCont[cpIndex] = cbits;
                            if (stop)
                                break;

                            upIndex++;
                            ubits = node.BaUsed[upIndex];
                            cpIndex++;
                            cbits = node.BaCont[cpIndex];
                            bit = 1;
                        }
                    }

                    // Statistics.
                    cont *= node.Density;
                    if (node.LargestBlock < cont)
                        node.LargestBlock = cont;
                    node.Used -= cont;
                    _used -= cont;

                    return true;
                }
            }

            public void FreeAll(bool keepVirtualMemory)
            {
                MemNode node = _first;

                while (node != null)
                {
                    MemNode next = node.Next;

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

            public void InsertNode(MemNode node)
            {
                if (_root == null)
                {
                    // Empty tree case.
                    _root = node;
                }
                else
                {
                    // False tree root.
                    MemNode head = new MemNode(IntPtr.Zero, 0, 1);

                    // Grandparent & parent.
                    MemNode g = null;
                    MemNode t = head;

                    // Iterator & parent.
                    MemNode p = null;
                    MemNode q = t.Right = _root;

                    bool dir = false;
                    bool last = false;

                    // Search down the tree.
                    for (; ; )
                    {
                        if (q == null)
                        {
                            // Insert new node at the bottom.
                            q = node;
                            if (dir)
                                p.Right = node;
                            else
                                p.Left = node;
                        }
                        else if (MemNode.IsRed(q.Left) && MemNode.IsRed(q.Right))
                        {
                            // Color flip.
                            q.Red = true;
                            q.Left.Red = false;
                            q.Right.Red = false;
                        }

                        // Fix red violation.
                        if (MemNode.IsRed(q) && MemNode.IsRed(p))
                        {
                            bool dir2 = t.Right == g;
                            var result = (q == (last ? p.Right : p.Left)) ? MemNode.RotateSingle(g, !last) : MemNode.RotateDouble(g, !last);
                            if (dir2)
                                t.Right = result;
                            else
                                t.Left = result;
                        }

                        // Stop if found.
                        if (q == node)
                            break;

                        last = dir;
                        dir = q.Memory.ToInt64() < node.Memory.ToInt64();

                        // Update helpers.
                        if (g != null)
                            t = g;
                        g = p;
                        p = q;
                        q = dir ? q.Right : q.Left;
                    }

                    // Update root.
                    _root = head.Right;
                }

                // Make root black.
                _root.Red = false;

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
            }

            public MemNode RemoveNode(MemNode node)
            {
                // False tree root.
                MemNode head = new MemNode(IntPtr.Zero, 0, 1);

                // Helpers.
                MemNode q = head;
                MemNode p = null;
                MemNode g = null;

                // Found item.
                MemNode f = null;
                bool dir = true;

                // Set up.
                q.Right = _root;

                // Search and push a red down.
                while ((dir ? q.Right : q.Left) != null)
                {
                    bool last = dir;

                    // Update helpers.
                    g = p;
                    p = q;
                    q = dir ? q.Right : q.Left;
                    dir = q.Memory.ToInt64() < node.Memory.ToInt64();

                    // Save found node.
                    if (q == node)
                        f = q;

                    // Push the red node down.
                    if (!MemNode.IsRed(q) && !MemNode.IsRed(dir ? q.Right : q.Left))
                    {
                        if (MemNode.IsRed(dir ? q.Left : q.Right))
                        {
                            var t = MemNode.RotateSingle(q, dir);
                            if (last)
                                p.Right = t;
                            else
                                p.Left = t;

                            p = t;
                        }
                        else if (!MemNode.IsRed(dir ? q.Left : q.Right))
                        {
                            MemNode s = last ? p.Left : p.Right;

                            if (s != null)
                            {
                                if (!MemNode.IsRed(last ? s.Left : s.Right) && !MemNode.IsRed(last ? s.Right : s.Left))
                                {
                                    // Color flip.
                                    p.Red = false;
                                    s.Red = true;
                                    q.Red = true;
                                }
                                else
                                {
                                    bool dir2 = g.Right == p;

                                    MemNode t = null;
                                    if (MemNode.IsRed(last ? s.Right : s.Left))
                                        t = MemNode.RotateDouble(p, last);
                                    else if (MemNode.IsRed(last ? s.Left : s.Right))
                                        t = MemNode.RotateSingle(p, last);

                                    if (dir2)
                                        g.Right = t;
                                    else
                                        g.Left = t;

                                    // Ensure correct coloring.
                                    q.Red = (dir2 ? g.Right : g.Left).Red = true;
                                    (dir2 ? g.Right : g.Left).Left.Red = false;
                                    (dir2 ? g.Right : g.Left).Right.Red = false;
                                }
                            }
                        }
                    }
                }

                // Replace and remove.
                Debug.Assert(f != null);
                Debug.Assert(f != head);
                Debug.Assert(q != head);

                if (f != q)
                    f.FillData(q);

                MemNode c = q.Left ?? q.Right;
                if (p.Right == q)
                    p.Right = c;
                else
                    p.Left = c;

                // Update root and make it black.
                if ((_root = head.Right) != null)
                    _root.Red = false;

                // Unlink.
                MemNode next = q.Next;
                MemNode prev = q.Previous;

                if (prev != null)
                {
                    prev.Next = next;
                }
                else
                {
                    _first = next;
                }

                if (next != null)
                {
                    next.Previous = prev;
                }
                else
                {
                    _last = prev;
                }

                if (_optimal == q)
                {
                    _optimal = prev ?? next;
                }

                return q;
            }

            public MemNode FindPtr(IntPtr memory)
            {
                MemNode cur = _root;
                while (cur != null)
                {
                    IntPtr curMem = cur.Memory;
                    if (memory.ToInt64() < curMem.ToInt64())
                    {
                        // Go left.
                        cur = cur.Left;
                        continue;
                    }
                    else
                    {
                        long curEnd = curMem.ToInt64() + cur.Size;
                        if (memory.ToInt64() >= curEnd)
                        {
                            // Go right.
                            cur = cur.Right;
                            continue;
                        }
                        else
                        {
                            // Match.
                            break;
                        }
                    }
                }

                return cur;
            }

            public bool CheckTree()
            {
                return MemNode.CheckTree(_root);
            }

            public abstract class RbNode<T>
                where T : RbNode<T>
            {
                private T _left;
                private T _right;

                private bool _red;

                // virtual memory address
                private IntPtr _memory;

                protected RbNode(IntPtr memory)
                {
                    _red = true;
                    _memory = memory;
                }

                public T Left
                {
                    get
                    {
                        return _left;
                    }

                    set
                    {
                        _left = value;
                    }
                }

                public T Right
                {
                    get
                    {
                        return _right;
                    }

                    set
                    {
                        _right = value;
                    }
                }

                public bool Red
                {
                    get
                    {
                        return _red;
                    }

                    set
                    {
                        _red = value;
                    }
                }

                public IntPtr Memory
                {
                    get
                    {
                        return _memory;
                    }

                    protected set
                    {
                        _memory = value;
                    }
                }

                public static int RbAssert(T root)
                {
                    if (root == null)
                        return 1;

                    T ln = root.Left;
                    T rn = root.Right;

                    // Red violation.
                    Debug.Assert(!(IsRed(root) && (IsRed(ln) || IsRed(rn))));

                    int lh = RbAssert(ln);
                    int rh = RbAssert(rn);

                    // Invalid btree.
                    Debug.Assert(ln == null || ln.Memory.ToInt64() < root.Memory.ToInt64());
                    Debug.Assert(rn == null || rn.Memory.ToInt64() > root.Memory.ToInt64());

                    // Black violation.
                    Debug.Assert(!(lh != 0 && rh != 0 && lh != rh));

                    // Only count black links.
                    if (lh != 0 && rh != 0)
                        return IsRed(root) ? lh : lh + 1;
                    else
                        return 0;
                }

                public static bool CheckTree(T root)
                {
                    return RbAssert(root) > 0;
                }

                public static T RotateSingle(T root, bool rightDirection)
                {
                    T save = rightDirection ? root._left : root._right;

                    if (rightDirection)
                    {
                        root._left = save._right;
                        save._right = root;
                    }
                    else
                    {
                        root._right = save._left;
                        save._left = root;
                    }

                    root._red = true;
                    save._red = false;

                    return save;
                }

                public static T RotateDouble(T root, bool rightDirection)
                {
                    if (rightDirection)
                        root._left = RotateSingle(root._left, !rightDirection);
                    else
                        root._right = RotateSingle(root._right, !rightDirection);

                    return RotateSingle(root, rightDirection);
                }

                public static bool IsRed(RbNode<T> node)
                {
                    return node != null && node._red;
                }

                public override string ToString()
                {
                    Func<RbNode<T>, string> shortFormat =
                        node =>
                        {
                            return node == null ? "{null}" : string.Format("{0}/{1}", node.Memory, node.Red ? "R" : "B");
                        };

                    return string.Format("{0} : {1}, {2}", shortFormat(this), shortFormat(Left), shortFormat(Right));
                }
            }

            public sealed class MemNode : RbNode<MemNode>
            {
                // --------------------------------------------------------------------------
                // [Node double-linked list]
                // --------------------------------------------------------------------------

                private MemNode _prev;           // Prev node in list.
                private MemNode _next;           // Next node in list.

                private long _size;
                private int _density;

                public MemNode(IntPtr memory, long size, int density)
                    : base(memory)
                {
                    _size = size;
                    _density = density;

                    long bsize = (((Blocks + 7) >> 3) + sizeof(int) - 1) & ~(uint)(sizeof(int) - 1);
                    LargestBlock = size;
                    BaUsed = new int[bsize / sizeof(int)];
                    BaCont = new int[bsize / sizeof(int)];
                }

                public MemNode Previous
                {
                    get
                    {
                        return _prev;
                    }

                    set
                    {
                        _prev = value;
                    }
                }

                public MemNode Next
                {
                    get
                    {
                        return _next;
                    }

                    set
                    {
                        _next = value;
                    }
                }

                public long Size
                {
                    get
                    {
                        return _size;
                    }

                    private set
                    {
                        _size = value;
                    }
                }

                public int Density
                {
                    get
                    {
                        return _density;
                    }

                    private set
                    {
                        _density = value;
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

                public void FillData(MemNode other)
                {
                    Memory = other.Memory;

                    Size = other.Size;
                    Density = other.Density;
                    Used = other.Used;
                    LargestBlock = other.LargestBlock;
                    BaUsed = other.BaUsed;
                    BaCont = other.BaCont;
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

            private sealed class PermanentNode
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

                public PermanentNode Previous
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

                public void DumpNode(MemNode node)
                {
                    Writer.WriteLine("  NODE_{0:X} [shape=record, style=filled, color={1}, label=\"<L>|<C>Mem: {2:X}, Used: {3}/{4}|<R>\"];",
                        RuntimeHelpers.GetHashCode(node),
                        node.Red ? "red" : "gray",
                        node.Memory, node.Used, node.Size);

                    if (node.Left != null)
                        Connect(node, node.Left, "L");
                    if (node.Right != null)
                        Connect(node, node.Right, "R");
                }

                public void Connect(MemNode node, MemNode other, string destination)
                {
                    DumpNode(other);

                    Writer.Write("  NODE_{0:X}:{1} -> NODE_{2:X}:C", RuntimeHelpers.GetHashCode(node), destination, RuntimeHelpers.GetHashCode(other));
                    if (other.Red)
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
