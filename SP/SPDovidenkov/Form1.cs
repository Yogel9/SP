using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;

namespace SPDovidenkov
{
    public partial class Form1 : Form
    {
        Process CProcess = null;
        EventWaitHandle EvStart = new EventWaitHandle(false, EventResetMode.AutoReset, "EvStart");
        EventWaitHandle EvStop = new EventWaitHandle(false, EventResetMode.AutoReset, "EvStop");
        EventWaitHandle EvExit = new EventWaitHandle(false, EventResetMode.AutoReset, "EvExit");
        EventWaitHandle EvConfirm = new EventWaitHandle(false, EventResetMode.AutoReset, "EvConfirm");
        public Form1()
        {
            InitializeComponent();
        }

        private Boolean IsConsoleOpen()
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
            if (IsConsoleOpen())
            {
                EvExit.Set();
                EvConfirm.WaitOne();
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            comboBox.SelectedIndex = 0;//при старте в комобобоксе 1
        }

        private void ButStart_Click(object sender, EventArgs e)
        {
            int NumOfThreads = comboBox.SelectedIndex + 1;  // количество потоков
            for (int i = 0; i < NumOfThreads;i++)
            {
                if (IsConsoleOpen())
                {
                    EvStart.Set();
                    EvConfirm.WaitOne();
                    listBox1.Items.Add("Поток номер - " + (listBox1.Items.Count-1).ToString());
                }
                else
                {
                    CProcess = Process.Start(@"D:\GitHub\SP-Leonov\Debug\ProjectC.exe");
                    listBox1.Items.Add("Главный поток");
                    break;
                }
            }

            
        }

        private void ButDel_Click(object sender, EventArgs e)
        {
            if (!IsConsoleOpen()) return;

            if (listBox1.Items.Count - 1 == 0)
            {
                EvExit.Set();
                EvConfirm.WaitOne();
                listBox1.Items.RemoveAt(0);
            }
            else
            {
                EvStop.Set();
                EvConfirm.WaitOne();
                listBox1.Items.RemoveAt(listBox1.Items.Count - 1);
            }
        }
    }
}
