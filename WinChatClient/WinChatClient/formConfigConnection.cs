using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WinChatClient
{
	public partial class formConfigConnection : Form
	{
		private Configuration _config;

		public Configuration Config
		{
			set { _config = value; }
			get { return _config; }
		}

		public formConfigConnection()
		{
			InitializeComponent();
		}

		private void OnAcceptClick(object sender, EventArgs e)
		{
			_config.ServerIP = tbServerIP.Text;
			_config.Port = tbPort.Text;

			DialogResult = DialogResult.OK;
		}

		private void formConfigConnection_Load(object sender, EventArgs e)
		{
			tbServerIP.Text = _config.ServerIP;
			tbPort.Text = _config.Port;
		}

		private void OnCancelClick(object sender, EventArgs e)
		{
			DialogResult = DialogResult.Cancel;
		}
	}
}
