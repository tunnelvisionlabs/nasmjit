namespace AsmJitNet2
{
    using TextWriter = System.IO.TextWriter;

    public class FileLogger : Logger
    {
        private TextWriter _stream;

        public FileLogger(TextWriter stream)
        {
            Stream = stream;
        }

        public TextWriter Stream
        {
            get
            {
                return _stream;
            }

            set
            {
                _stream = value;
                IsUsed = IsEnabled && _stream != null;
            }
        }

        public override bool IsEnabled
        {
            get
            {
                return base.IsEnabled;
            }

            set
            {
                base.IsEnabled = value;
                IsUsed = IsEnabled && _stream != null;
            }
        }

        public override void LogString(string text)
        {
            if (!IsUsed)
                return;

            _stream.Write(text);
        }
    }
}
