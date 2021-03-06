﻿namespace NAsmJit
{
    using System;
    using System.Diagnostics.Contracts;

    /// <summary>
    /// Item used to align assembler code.
    /// </summary>
    public class CompilerAlign : CompilerItem
    {
        private readonly int _size;

        /// <summary>
        /// Create a new Align instance.
        /// </summary>
        /// <param name="compiler">The compiler.</param>
        /// <param name="size">The alignment size in bytes.</param>
        public CompilerAlign(Compiler compiler, int size = 0)
            : base(compiler)
        {
            if (size < 0)
                throw new ArgumentOutOfRangeException("size");
            Contract.Requires(compiler != null);
            Contract.EndContractBlock();

            _size = size;
        }

        public sealed override ItemType ItemType
        {
            get
            {
                return ItemType.Align;
            }
        }

        public override int MaxSize
        {
            get
            {
                return (_size > 0) ? _size - 1 : 0;
            }
        }

        /// <summary>
        /// Gets the alignment size in bytes.
        /// </summary>
        public int Size
        {
            get
            {
                return _size;
            }
        }

        protected override void EmitImpl(Assembler a)
        {
            a.Align(_size);
        }
    }
}
