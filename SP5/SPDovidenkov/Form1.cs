using System;
using System.Threading;
using System.Diagnostics;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace SPDovidenkov
{
    public partial class Form1 : Form
    {
        [DllImport("MFCLibrary.dll")] static extern void dllInit();

        [DllImport("MFCLibrary.dll")] static extern void dllSendInt(int command);

        [DllImport("MFCLibrary.dll")] static extern int dllGetInt();

        [DllImport("MFCLibrary.dll")] static extern void dllSendString(string message);

        [DllImport("MFCLibrary.dll")] public static extern bool dllConnectToServer();

        [DllImport("MFCLibrary.dll")] public static extern void dllDisconnectFromServer();



        public Form1()
        {
            InitializeComponent();
            dllInit();
        }


        private void updateListBox()
        {
            int threads = dllGetInt();

            listBox1.Items.Clear();

            listBox1.Items.Add("Все потоки");
            listBox1.Items.Add("Главный поток");

            listBox1.SetSelected(1, true);

            for (int t = 1; t <= threads; t++)
            {
                listBox1.Items.Add("Поток №" + t);
            }
        }

        private void startThread()
        {
            dllConnectToServer();

            dllSendInt(0);

            updateListBox();

            dllDisconnectFromServer();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            comboBox.SelectedIndex = 0;//при старте в комобобоксе 1
        }

        private void ButStart_Click(object sender, EventArgs e)
        {
            int NumOfThreads = comboBox.SelectedIndex + 1;  // количество потоков

            for (int i = 1; i <= NumOfThreads; i++)
            {
                startThread();
            }
        }


        private void ButDel_Click(object sender, EventArgs e)
        {
            dllConnectToServer();

            dllSendInt(1);

            updateListBox();

            dllDisconnectFromServer();
        }
        private void ButSendOne_Click(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex != -1)
            {
                dllConnectToServer();

                string message = textBox1.Text;
                int recipient = listBox1.SelectedIndex - 1;

                dllSendInt(2);

                dllGetInt();

                dllSendInt(recipient);

                dllGetInt();

                dllSendString(message);

                dllGetInt();

                textBox1.Text = "";

                dllDisconnectFromServer();

            }
        }

        private void Form1_Activated(object sender, EventArgs e)
        {
            dllConnectToServer();

            dllSendInt(-1);

            updateListBox();

            dllDisconnectFromServer();
        }
    }
}



