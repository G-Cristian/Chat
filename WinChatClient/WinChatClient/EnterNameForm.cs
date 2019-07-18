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
	public partial class EnterNameForm : Form
	{
		public string GetName
		{
			get { return tbName.Text.Trim(); }
		}

		public EnterNameForm()
		{
			InitializeComponent();
		}

		private void EnterNameForm_Validating(object sender, CancelEventArgs e)
		{
			if (string.IsNullOrEmpty(tbName.Text.Trim()))
			{
				MessageBox.Show("Name cannot be empty.");
				e.Cancel = true;
			}
		}

		private void btnAccept_Click(object sender, EventArgs e)
		{
			DialogResult = DialogResult.OK;
		}

		private void EnterNameForm_Load(object sender, EventArgs e)
		{
		}
	}
}
