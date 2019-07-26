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
		private static EventWaitHandle _endReading = new EventWaitHandle(false, EventResetMode.AutoReset);
		private static EventWaitHandle _endConsuming = new EventWaitHandle(false, EventResetMode.AutoReset);

		public formChat()
		{
			InitializeComponent();
		}

		private void OnConfigConnectionClick(object sender, EventArgs e)
		{
			formConfigConnection configConnForm = new formConfigConnection();
			configConnForm.Config = _config;
			if (configConnForm.ShowDialog(this) == DialogResult.OK)
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
				tbOutput.Text += System.Environment.NewLine;
				tbOutput.Text += "------------";
				tbOutput.Text += "Connection Stablished";
				connectionToolStripMenuItem.Enabled = false;
				if (SendName())
				{
					new Thread(Read_Callback).Start();
				}
			}
			catch(Exception ex)
			{
				bOk = false;
				tbOutput.Text += System.Environment.NewLine;
				tbOutput.Text += "------------";
				tbOutput.Text += ex.ToString();
			}

			return bOk;
		}

		public void Read_Callback()
		{
			bool leaveChat = false;
			int byteRecv = 0;
			do
			{
				byte[] messageReceived = new byte[300];

				// We receive the messagge using  
				// the method Receive(). This  
				// method returns number of bytes 
				// received, that we'll use to  
				// convert them to string 
				byteRecv = _socket.Receive(messageReceived);
				if (byteRecv > 1)
				{
					if (messageReceived[0] == 0x11)
					{
						//SERVER_COMMAND_STATUS           0x11
						byte status = messageReceived[1];
						if (status == 0x27)
						{
							//STATUS_MESSAGE_SENT_WITH_FAILS  0x27
							ProduceMsg("MESSAGE SENT WITH FAILS");
						}
						else if (status == 0x29)
						{
							//STATUS_LEFT_CHAT_OK             0x29
							leaveChat = true;

						}
						else if (status != 0x21)
						{
							//STATUS_OK                       0x21
							ProduceMsg("Unexpected status received.");
						}
					}
					else if (messageReceived[0] == 0x15) {
						//SERVER_CLIENT_SENT_MESSAGE      0x15
						string msg = Encoding.ASCII.GetString(messageReceived,
														 1, byteRecv - 1);
						ProduceMsg(msg);
					}
					else if (messageReceived[0] == 0x17)
					{
						//SERVER_CLIENT_LEFT_CHAT         0x17
						string clientName = Encoding.ASCII.GetString(messageReceived,
														 1, byteRecv - 1);
						ProduceMsg(string.Concat(clientName.Substring(0, clientName.IndexOf('\0'))," left chat."));
					}
				}
			} while (!leaveChat && byteRecv > 0);

			if(byteRecv == 0)
			{
				ProduceMsg("Connection closed by server.");
			}else if(byteRecv < 0)
			{
				ProduceMsg("Error receiving");
			}
			_endReading.Set();
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
				EnterNameForm nameForm = new EnterNameForm();
				if (nameForm.ShowDialog(this) == DialogResult.OK)
				{
					_name = nameForm.GetName;
				}

				//CLIENT_CONNECTS         0x01
				SendMsg(1, _name+'\0');
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
			ProduceMsg(tbInput.Text);
			//CLIENT_SENDS_MESSAGE    0x06
			SendMsg(6, tbInput.Text+'\0');
			tbInput.Text = String.Empty;
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
			_endConsuming.Set();
		}

		private void PrintMessage(string msg)
		{
			if (InvokeRequired)
			{
				this.Invoke(new Action<string>(PrintMessage), new object[] { msg });
				return;
			}

			tbOutput.Text += System.Environment.NewLine;
			tbOutput.Text += msg;
		}

		private void formChat_Load(object sender, EventArgs e)
		{
			new Thread(ConsumeMsg).Start();
		}

		private void formChat_FormClosing(object sender, FormClosingEventArgs e)
		{
			if (_socket != null)
			{
				//CLIENT_LEAVES_CHAT      0x07
				SendMsg(7, "");
			}
			_exiting = true;
			_notEmpty.Set();
			_endReading.WaitOne(10000);
			_endConsuming.WaitOne(10000);
			_notEmpty.Close();
			_endConsuming.Close();
			_endReading.Close();
			_messagesLock.Close();
			CloseConnection();
			MessageBox.Show("Good bye");
		}
	}
}
