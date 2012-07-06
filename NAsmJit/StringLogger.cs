namespace NAsmJit
{
    using System.Text;

    public class StringLogger : Logger
    {
        private readonly StringBuilder _stringBuilder;

        public StringLogger()
        {
            _stringBuilder = new StringBuilder();
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
                IsUsed = value;
            }
        }

        public override void LogString(string text)
        {
            if (!IsUsed)
                return;

            _stringBuilder.Append(text);
        }

        public override string ToString()
        {
            return _stringBuilder.ToString();
        }
    }
}
