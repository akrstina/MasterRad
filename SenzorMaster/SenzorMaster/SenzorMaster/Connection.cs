using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Http;

namespace SenzorMaster
{
    class Connection
    {
        private static Connection _instance;
        public static Connection Instance
        {
            get
            {
                if (_instance == null) _instance = new Connection();
                return _instance;
            }
        }
        public HttpClient client { get; set; }
        public string address { get; set; }
    }
}
