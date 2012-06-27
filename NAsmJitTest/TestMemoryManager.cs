namespace NAsmJitTest
{
    using System;
    using AsmJitNet;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Marshal = System.Runtime.InteropServices.Marshal;
    using TextWriter = System.IO.TextWriter;

    [TestClass]
    public class TestMemoryManager
    {
        private readonly Random rand = new Random(100);

        public TestContext TestContext
        {
            get;
            set;
        }

        [TestMethod]
        public void TestMemoryManager1()
        {
            MemoryManager memmgr = MemoryManager.Global;

            int count = 20000;

            Console.Error.WriteLine("[Memory alloc/free test - {0} allocations]", count);
            Console.Error.WriteLine();

            IntPtr[] a = new IntPtr[count];
            IntPtr[] b = new IntPtr[count];

            Console.Error.Write("Allocating virtual memory... ");

            for (int i = 0; i < count; i++)
            {
                int r = (rand.Next() % 1000) + 4;
                a[i] = memmgr.Alloc(r);
                Assert.AreNotEqual(IntPtr.Zero, a[i]);
                Memset(a[i], 0, r);
            }

            Console.Error.WriteLine(" done");
            Stats(Console.Error);

            Console.Error.WriteLine();
            Console.Error.Write("Freeing virtual memory...");
            for (int i = 0; i < count; i++)
            {
                Assert.IsTrue(memmgr.Free(a[i]));
            }

            Console.Error.WriteLine(" done");
            Stats(Console.Error);

            Console.Error.WriteLine();
            Console.Error.WriteLine("[Verified alloc/free test - {0} allocations]", count);
        }

        [TestMethod]
        public void TestVerifyAndFree()
        {
            MemoryManager memmgr = MemoryManager.Global;

            int count = 20000;

            Console.Error.WriteLine("[Memory alloc/free test - {0} allocations]", count);
            Console.Error.WriteLine();

            IntPtr[] a = new IntPtr[count];
            IntPtr[] b = new IntPtr[count];

            Console.Error.Write("Alloc... ");
            for (int i = 0; i < count / 2; i++)
            {
                int r = rand.Next(1000) + 4;
                a[i] = memmgr.Alloc(r);
                b[i] = Marshal.AllocHGlobal(r);
                Assert.AreNotEqual(IntPtr.Zero, a[i]);
                Assert.AreNotEqual(IntPtr.Zero, b[i]);

                Gen(a[i], b[i], r);
            }
            Console.Error.WriteLine("done.");
            Stats(Console.Error);

            Console.Error.Write("Verify and free... ");
            for (int i = 0; i < count / 2; i++)
            {
                Verify(a[i], b[i]);
                Assert.IsTrue(memmgr.Free(a[i]));
                Marshal.FreeHGlobal(b[i]);
            }
            Console.Error.WriteLine("done.");
            Stats(Console.Error);
        }

        [TestMethod]
        public void TestShuffle()
        {
            MemoryManager memmgr = MemoryManager.Global;

            int count = 20000;

            Console.Error.WriteLine("[Memory alloc/free test - {0} allocations]", count);
            Console.Error.WriteLine();

            IntPtr[] a = new IntPtr[count];
            IntPtr[] b = new IntPtr[count];

            Console.Error.Write("Alloc... ");
            for (int i = 0; i < count; i++)
            {
                int r = rand.Next(1000) + 4;
                a[i] = memmgr.Alloc(r);
                b[i] = Marshal.AllocHGlobal(r);
                Assert.AreNotEqual(IntPtr.Zero, a[i]);
                Assert.AreNotEqual(IntPtr.Zero, b[i]);

                Gen(a[i], b[i], r);
            }
            Console.Error.WriteLine("done.");
            Stats(Console.Error);

            Console.Error.WriteLine();
            Console.Error.Write("Shuffling... ");
            Shuffle(a, b);
            Console.Error.WriteLine("done.");

            Console.Error.WriteLine();
            Console.Error.Write("Verify and free...");
            for (int i = 0; i < count / 2; i++)
            {
                Verify(a[i], b[i]);
                Assert.IsTrue(memmgr.Free(a[i]));
                Marshal.FreeHGlobal(b[i]);
            }
            Console.Error.WriteLine("done.");
            Stats(Console.Error);
        }

        private static void Stats(TextWriter writer)
        {
            MemoryManager memmgr = MemoryManager.Global;
            writer.WriteLine("-- Used: {0}", memmgr.UsedBytes);
            writer.WriteLine("-- Allocated: {0}", memmgr.AllocatedBytes);

            VirtualMemoryManager vmm = memmgr as VirtualMemoryManager;
            if (vmm != null)
                vmm.Dump(writer);
        }

        private void Shuffle(IntPtr[] a, IntPtr[] b)
        {
            if (a == null)
                throw new ArgumentNullException("a");
            if (b == null)
                throw new ArgumentNullException("b");
            if (a.Length != b.Length)
                throw new ArgumentException();

            for (int i = 0; i < a.Length; i++)
            {
                int si = rand.Next(a.Length);

                IntPtr ta = a[i];
                IntPtr tb = b[i];

                a[i] = a[si];
                b[i] = b[si];

                a[si] = ta;
                b[si] = tb;
            }
        }

        private void Verify(IntPtr a, IntPtr b)
        {
            if (a == IntPtr.Zero)
                throw new ArgumentException();
            if (b == IntPtr.Zero)
                throw new ArgumentException();

            int ai = Marshal.ReadInt32(a);
            int bi = Marshal.ReadInt32(b);
            Assert.AreEqual(ai, bi);
            Assert.AreEqual(0, Memcmp(a, b, ai));
        }

        private void Gen(IntPtr a, IntPtr b, int i)
        {
            if (a == IntPtr.Zero)
                throw new ArgumentException();
            if (b == IntPtr.Zero)
                throw new ArgumentException();
            if (i < 0)
                throw new ArgumentOutOfRangeException("i");

            byte pattern = (byte)rand.Next(256);
            Marshal.WriteInt32(a, i);
            Marshal.WriteInt32(b, i);
            Memset((IntPtr)(a.ToInt64() + sizeof(int)), pattern, i - sizeof(int));
            Memset((IntPtr)(b.ToInt64() + sizeof(int)), pattern, i - sizeof(int));
        }

        private void Memset(IntPtr memory, int value, int count)
        {
            if (memory == IntPtr.Zero)
                throw new ArgumentException();
            if (count < 0)
                throw new ArgumentOutOfRangeException("count");

            byte b = (byte)value;
            for (int i = 0; i < count; i++)
                Marshal.WriteByte((IntPtr)(memory.ToInt64() + i), b);
        }

        private int Memcmp(IntPtr a, IntPtr b, int ai)
        {
            if (a == IntPtr.Zero)
                throw new ArgumentException();
            if (b == IntPtr.Zero)
                throw new ArgumentException();
            if (ai < 0)
                throw new ArgumentOutOfRangeException("ai");

            if (a == b)
                return 0;

            for (int i = 0; i < ai; i++)
            {
                int result = Marshal.ReadByte(a, i) - Marshal.ReadByte(b, i);
                if (result != 0)
                    return result;
            }

            return 0;
        }
    }
}
