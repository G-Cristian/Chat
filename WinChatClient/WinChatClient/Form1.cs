using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WinChatClient
{
	public partial class formChat : Form
	{

		private Configuration _config = new Configuration();
		private Socket _socket = null;
		private String _name = "";
		private static bool _exiting = false;
		private static Queue<string> _messages = new Queue<string>();
		private static EventWaitHandle _messagesLock = new EventWaitHandle(true, EventResetMode.AutoReset);
		private static EventWaitHandle _notEmpty = new EventWaitHandle(false, EventResetMode.AutoReset);

		public formChat()
		{
			InitializeComponent();
		}

		private void OnConfigConnectionClick(object sender, EventArgs e)
		{
			formConfigConnection configConnForm = new formConfigConnection();
			configConnForm.Config = _config;
			if(configConnForm.ShowDialog(this) == DialogResult.OK)
			{
				CloseConnection();
				Connect();
			}
		}

		private void CloseConnection()
		{
			if(_socket != null)
			{
				_socket.Shutdown(SocketShutdown.Both);
				_socket.Close();
				_socket = null;
			}
		}

		private bool Connect()
		{
			bool bOk = true;

			IPAddress[] ips = Dns.GetHostAddresses(_config.ServerIP);

			_socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			tbOutput.Text += "------------";
			tbOutput.Text += "Establishing connection to " + _config.ServerIP;

			try {
				_socket.Connect(ips[0], int.Parse(_config.Port));
				tbOutput.Text += "\n";
				tbOutput.Text += "------------";
				tbOutput.Text += "Connection Stablished";
				if (SendName())
				{
					new Thread(Read_Callback).Start();
				}
			}
			catch(Exception ex)
			{
				bOk = false;
				tbOutput.Text += "\n";
				tbOutput.Text += "------------";
				tbOutput.Text += ex.ToString();
			}

			return bOk;
		}

		public void Read_Callback()
		{
			//TODO: Complete this method, check errors and check server status;
			bool reading = true;
			while (reading)
			{
				byte[] messageReceived = new byte[300];

				// We receive the messagge using  
				// the method Receive(). This  
				// method returns number of bytes 
				// received, that we'll use to  
				// convert them to string 
				int byteRecv = _socket.Receive(messageReceived);
				//TODO: Interpret received status, command, etc.
				//TODO: check errors.
				//TODO: add messages to queue of messages to print.
				byte status = messageReceived[0];
				string msg = Encoding.ASCII.GetString(messageReceived,
												 1, byteRecv - 1);
			}
		}

		private byte ReadStatus(Socket socket)
		{
			byte status = 0;

			byte[] messageReceived = new byte[300];

			// We receive the messagge using  
			// the method Receive(). This  
			// method returns number of bytes 
			// received, that we'll use to  
			// convert them to string 
			int byteRecv = socket.Receive(messageReceived);
			if(byteRecv > 1)
			{
				//SERVER_COMMAND_STATUS           0x11
				if (messageReceived[0] == 0x11)
				{
					status = messageReceived[1];
				}
			}

			return status;
		}


		private bool SendName()
		{
			bool errorSendingName = true;
			do
			{
				if (string.IsNullOrEmpty(_name))
				{
					EnterNameForm nameForm = new EnterNameForm();
					if (nameForm.ShowDialog(this) == DialogResult.OK)
					{
						_name = nameForm.GetName;
					}

				}
				//CLIENT_CONNECTS         0x01
				SendMsg(1, _name);
				byte status = ReadStatus(_socket);

				//STATUS_OK                       0x21
				//STATUS_CLIENT_NAME_EXISTS       0x22

				if(status == 0x21)
				{
					ProduceMsg("Connected with name " + _name);
					errorSendingName = false;
				}
				else if (status == 0x22)
				{
					ProduceMsg("Name already exist (" + _name + ")");
				}
				else
				{
					ProduceMsg("Unexpected error reading status");
				}
			} while (errorSendingName);

			return !errorSendingName;
		}

		private int SendMsg(byte command, string msg)
		{
			byte[] msgToSend = Encoding.ASCII.GetBytes("_" + msg);
			msgToSend[0] = command;
			return _socket.Send(msgToSend);
		}

		private void OnSendClick(object sender, EventArgs e)
		{
			//TODO: Complete this method, checking errors and checking server status;
			//CLIENT_SENDS_MESSAGE    0x06
			byte[] msg = Encoding.ASCII.GetBytes("6" + tbInput.Text);
			msg[0] = (byte)(6);
			_socket.Send(msg);
		}

		private void ProduceMsg(string msg)
		{
			_messagesLock.WaitOne();
			_messages.Enqueue(msg);
			if(_messages.Count == 1)
			{
				_notEmpty.Set();
			}
			_messagesLock.Set();
		}

		private void ConsumeMsg()
		{
			while (!_exiting)
			{
				_notEmpty.WaitOne();
				_messagesLock.WaitOne();
				
				while(_messages.Count > 0)
				{
					string msg = _messages.Dequeue();
					PrintMessage(msg);
				}
				_messagesLock.Set();
			}
		}

		private void PrintMessage(string msg)
		{
			if (InvokeRequired)
			{
				this.Invoke(new Action<string>(PrintMessage), new object[] { msg });
				return;
			}

			tbOutput.Text += "\n";
			tbOutput.Text += msg;
		}

		private void formChat_Load(object sender, EventArgs e)
		{
			new Thread(ConsumeMsg).Start();
		}
	}
}
