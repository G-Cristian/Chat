using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WinChatClient
{
	public class Configuration
	{
		private string _serverIP = "";
		private string _port = "";

		public string ServerIP
		{
			get { return _serverIP; }
			set { _serverIP = value; }
		}
		public string Port
		{
			get { return _port; }
			set { _port = value; }
		}
	}
}
