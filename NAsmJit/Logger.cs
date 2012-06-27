namespace NAsmJit
{
    using System.Diagnostics.Contracts;

    public abstract class Logger
    {
        private bool _enabled;

        private bool _used;

        private bool _logBinary;

        public Logger()
        {
            _enabled = true;
            _used = true;
        }

        public virtual bool IsEnabled
        {
            get
            {
                return _enabled;
            }

            set
            {
                _enabled = value;
                _used = value;
            }
        }

        public bool IsUsed
        {
            get
            {
                return _used;
            }

            protected set
            {
                _used = value;
            }
        }

        public bool LogBinary
        {
            get
            {
                return _logBinary;
            }

            set
            {
                _logBinary = value;
            }
        }

        public virtual void LogFormat(string format, params object[] args)
        {
            Contract.Requires(format != null);
            Contract.Requires(args != null);

            LogString(string.Format(format, args));
        }

        public abstract void LogString(string text);
    }
}
