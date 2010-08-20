﻿namespace AsmJitNet2
{
    using System;
    using BitArray = System.Collections.BitArray;
    using Debug = System.Diagnostics.Debug;
    using System.Runtime.InteropServices;

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

            // [Allocation]

            public M_Node CreateNode(long size, long density)
            {
                int vsize;
                IntPtr vmem = AllocVirtualMemory((int)size, out vsize);

                // Out of memory.
                if (vmem == IntPtr.Zero)
                    return null;

                long blocks = (vsize / density);
                long basize = (((blocks + 7) >> 3) + sizeof(int) - 1) & ~(uint)(sizeof(int) - 1);
                //long memSize = sizeof(M_Node) + (basize * 2);

                M_Node node;
                try
                {
                    node = new M_Node();
                }
                catch (OutOfMemoryException)
                {
                    return null;
                }

                //memset(node, 0, memSize);

                node.NlColor = NodeColor.Red;
                node.Memory = vmem;

                node.Size = vsize;
                node.Blocks = blocks;
                node.Density = density;
                node.LargestBlock = vsize;
                node.BaUsed = new int[basize / sizeof(int)];
                node.BaCont = new int[basize / sizeof(int)];

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
                        if ((node.Remain < vsize) ||
                            (node.LargestBlock < vsize && node.LargestBlock != 0))
                        {
                            M_Node next = node.Next;
                            if (node.Remain < minVSize && node == _optimal && next != null)
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
                    Debug.Assert(result.ToInt64() >= node.Memory.ToInt64() && result.ToInt64() < node.Memory.ToInt64() + node.Size);
                    return result;
                }
            }

            private void SetBits(int[] buf, int offset, int count)
            {
                if (count == 0)
                    return;

                int i = offset / BITS_PER_ENTITY; // sysuint_t[]
                int j = offset % BITS_PER_ENTITY; // sysuint_t[][] bit index

                // How many bytes process in first group.
                int c = BITS_PER_ENTITY - j;

                // Offset.
                int bufIndex = i;

                if (c > count)
                {
                    buf[bufIndex] |= (int)(((~0U) >> (BITS_PER_ENTITY - count)) << j);
                    return;
                }
                else
                {
                    buf[bufIndex++] |= (int)(((~0U) >> (BITS_PER_ENTITY - c)) << j);
                    count -= c;
                }

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

            public bool Free(IntPtr address)
            {
                throw new NotImplementedException();
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

            public static bool NlIsRed(M_Node n)
            {
                throw new NotImplementedException();
            }
            public static M_Node NlRotateLeft(M_Node n)
            {
                throw new NotImplementedException();
            }
            public static M_Node NlRotateRight(M_Node n)
            {
                throw new NotImplementedException();
            }
            public static void NlFlipColor(M_Node n)
            {
                throw new NotImplementedException();
            }
            public static M_Node NlMoveRedLeft(M_Node h)
            {
                throw new NotImplementedException();
            }
            public static M_Node NlMoveRedRight(M_Node h)
            {
                throw new NotImplementedException();
            }
            public static M_Node NlFixUp(M_Node h)
            {
                throw new NotImplementedException();
            }

            public void NlInsertNode(M_Node n)
            {
                _root = NlInsertNode_(_root, n);
            }
            public M_Node NlInsertNode_(M_Node h, M_Node n)
            {
                if (h == null)
                    return n;

                if (NlIsRed(h.NlLeft) && NlIsRed(h.NlRight))
                    NlFlipColor(h);

                if (n.Memory.ToInt64() < h.Memory.ToInt64())
                    h.NlLeft = NlInsertNode_(h.NlLeft, n);
                else
                    h.NlRight = NlInsertNode_(h.NlRight, n);

                if (NlIsRed(h.NlRight) && !NlIsRed(h.NlLeft))
                    h = NlRotateLeft(h);
                if (NlIsRed(h.NlLeft) && NlIsRed(h.NlLeft.NlLeft))
                    h = NlRotateRight(h);

                return h;
            }

            public void NlRemoveNode(M_Node n)
            {
                throw new NotImplementedException();
            }
            public M_Node NlRemoveNode_(M_Node h, M_Node n)
            {
                throw new NotImplementedException();
            }
            public M_Node NlRemoveMin(M_Node h)
            {
                throw new NotImplementedException();
            }

            public M_Node NlFindPtr(byte mem)
            {
                throw new NotImplementedException();
            }

            public sealed class M_Node
            {
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

                public M_Node NlLeft
                {
                    get;
                    set;
                }

                public M_Node NlRight
                {
                    get;
                    set;
                }

                public NodeColor NlColor
                {
                    get;
                    set;
                }

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

                public long Blocks
                {
                    get;
                    set;
                }

                public long Density
                {
                    get;
                    set;
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

                public long Remain
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
        }

        private static class UnsafeNativeMethods
        {
            [DllImport("kernel32.dll")]
            public static extern IntPtr GetCurrentProcess();
        }
    }
}