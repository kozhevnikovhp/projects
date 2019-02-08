using System;

namespace ZZero.ZPlanner.Data.Provider
{
    public static class KeyHelper
    {
        private static string code = "5A706C616E6E6572566572696669636174696F6E436F6465466F724469656C6563747269634D6174657269616C4C696272617279";
        private static string sode = "7ABF8F43D5D508D5D0724A442DA84A224306639C5E6BC03A207DBD6F0847AE5420AC84405571F86DE4CBEEFADFCCF0EB89052247";

        public static string GetKey()
        {
            string[] dec = new string[code.Length / 2];

            int c = 0;
            int s = 0;
            int dc = 7;
            int ds = 9;

            for (int i = 0; i < dec.Length; ++i)
            {
                dec[i] = string.Format("{0:X2}", Convert.ToByte(code.Substring(c * 2, 2), 16) ^ Convert.ToByte(sode.Substring(s * 2, 2), 16));
                c += dc;
                if (c >= dec.Length) c -= dec.Length;
                s += ds;
                if (s >= dec.Length) s -= dec.Length;
            }

            return string.Join("-", dec);
        }
    }
}
