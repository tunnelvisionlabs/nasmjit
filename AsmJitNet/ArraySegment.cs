namespace AsmJitNet2
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using IList = System.Collections.IList;
    using ICollection = System.Collections.ICollection;
    using IEnumerable = System.Collections.IEnumerable;
    using IEnumerator = System.Collections.IEnumerator;

    public sealed class ArraySegment<T> : IList<T>, ICollection<T>, IEnumerable<T>, IList, ICollection, IEnumerable
    {
        private readonly T[] _array;
        private readonly int _offset;
        private readonly int _count;

        public ArraySegment(T[] array, int offset, int count)
        {
            if (array == null)
                throw new ArgumentNullException("array");
            if (offset < 0)
                throw new ArgumentOutOfRangeException("offset");
            if (count < 0)
                throw new ArgumentOutOfRangeException("count");
            if (offset > array.Length)
                throw new ArgumentException();
            if (checked(offset + count) > array.Length)
                throw new ArgumentException();

            _array = array;
            _offset = offset;
            _count = count;
        }

        public int Count
        {
            get
            {
                return _count;
            }
        }

        public bool IsReadOnly
        {
            get
            {
                return false;
            }
        }

        public bool IsFixedSize
        {
            get
            {
                return true;
            }
        }

        bool ICollection.IsSynchronized
        {
            get
            {
                return false;
            }
        }

        object ICollection.SyncRoot
        {
            get
            {
                return this;
            }
        }

        public T this[int index]
        {
            get
            {
                if (index < 0)
                    throw new ArgumentOutOfRangeException("index");
                if (index >= _count)
                    throw new ArgumentException();

                return _array[_offset + index];
            }
            set
            {
                if (index < 0)
                    throw new ArgumentOutOfRangeException("index");
                if (index >= _count)
                    throw new ArgumentException();

                _array[_offset + index] = value;
            }
        }

        object IList.this[int index]
        {
            get
            {
                return this[index];
            }

            set
            {
                this[index] = (T)value;
            }
        }

        public int IndexOf(T item)
        {
            return Array.IndexOf(_array, item, _offset, _count) - _offset;
        }

        void IList<T>.Insert(int index, T item)
        {
            throw new NotSupportedException();
        }

        void IList<T>.RemoveAt(int index)
        {
            throw new NotSupportedException();
        }

        public bool Contains(T item)
        {
            return IndexOf(item) >= 0;
        }

        public void CopyTo(T[] array, int arrayIndex)
        {
            Array.Copy(_array, _offset, array, arrayIndex, _count);
        }

        public IEnumerator<T> GetEnumerator()
        {
            for (int i = 0; i < _count; i++)
                yield return _array[_offset + i];
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        bool IList.Contains(object value)
        {
            return Array.IndexOf(_array, value, _offset, _count) >= 0;
        }

        int IList.IndexOf(object value)
        {
            return Array.IndexOf(_array, value, _offset, _count) - _offset;
        }

        void ICollection.CopyTo(Array array, int index)
        {
            Array.Copy(_array, _offset, array, index, _count);
        }

        void ICollection<T>.Add(T item)
        {
            throw new NotSupportedException();
        }

        void ICollection<T>.Clear()
        {
            throw new NotSupportedException();
        }

        bool ICollection<T>.Remove(T item)
        {
            throw new NotSupportedException();
        }

        int IList.Add(object value)
        {
            throw new NotSupportedException();
        }

        void IList.Clear()
        {
            throw new NotSupportedException();
        }

        void IList.Insert(int index, object value)
        {
            throw new NotSupportedException();
        }

        void IList.Remove(object value)
        {
            throw new NotSupportedException();
        }

        void IList.RemoveAt(int index)
        {
            throw new NotSupportedException();
        }
    }
}
