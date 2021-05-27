using System;
using System.Threading;
using System.Diagnostics;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace SPDovidenkov
{
    public partial class Form1 : Form
    {
        Process CProcess = null;
        [DllImport("MFCLibrary.dll", CharSet = CharSet.Ansi)] static extern void DllBegin();
        [DllImport("MFCLibrary.dll", CharSet = CharSet.Ansi)] static extern void DllEnd();
        [DllImport("MFCLibrary.dll", CharSet = CharSet.Ansi)] static extern void DllSendMessage(string message, int adress);
        [DllImport("MFCLibrary.dll", CharSet = CharSet.Ansi)] static extern bool DllWaitConfirm();
        [DllImport("MFCLibrary.dll", CharSet = CharSet.Ansi)] static extern void DllStart();
        [DllImport("MFCLibrary.dll", CharSet = CharSet.Ansi)] static extern void DllStop();
        [DllImport("MFCLibrary.dll", CharSet = CharSet.Ansi)] static extern void DllQuit();

        public Form1()
        {
            InitializeComponent();
        }

        private Boolean IsTheConsoleOpen()
        {
            if (CProcess == null || CProcess.HasExited)    //проверяем есть ли наш процесс
            {
                listBox1.Items.Clear();
                return false;
            }
            return true;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (IsTheConsoleOpen())
            {
                DllQuit();
                DllWaitConfirm();
            }
            DllEnd();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            DllBegin();
            comboBox.SelectedIndex = 0;//при старте в комобобоксе 1
        }

        private void ButStart_Click(object sender, EventArgs e)
        {
            int NumOfThreads = comboBox.SelectedIndex + 1;  // количество потоков

            if (IsTheConsoleOpen())
            {
                for (int i = 0; i < NumOfThreads; i++)
                {
                    DllStart();
                    DllWaitConfirm();
                    listBox1.Items.Add("Поток №" + (listBox1.Items.Count - 2).ToString());
                }
            }
            else
            {
                CProcess = Process.Start("ProjectC.exe");
                listBox1.Items.Add("Все потоки");
                listBox1.Items.Add("Главный поток");
                listBox1.SetSelected(1,true);
            };
        }


        private void ButDel_Click(object sender, EventArgs e)
        {
            if (!IsTheConsoleOpen()) return;

            DllStop();
            DllWaitConfirm();
            if (listBox1.Items.Count > 2)
            {
                listBox1.Items.RemoveAt(listBox1.Items.Count - 1);
            }else
            {
                listBox1.Items.Clear();
            }
        }
        private void ButSendOne_Click(object sender, EventArgs e)
        {
            if (!IsTheConsoleOpen())
                return;

            int recipient = listBox1.SelectedIndex - 1;
            string message = textBox1.Text;
            DllSendMessage(message, recipient);
            textBox1.Text = "";
        }

    }
}
