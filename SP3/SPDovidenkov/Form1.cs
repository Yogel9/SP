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
        [DllImport("MFCLibrary.dll")] static extern void dllSendMessage(string message, int threadNumber);
        [DllImport("MFCLibrary.dll")] static extern int dllSendCommand(int commandId);
        [DllImport("MFCLibrary.dll")] static extern int dllAcceptConfirm();
        [DllImport("MFCLibrary.dll")] public static extern void dllStartProcess();
        [DllImport("MFCLibrary.dll")] public static extern bool isProcessActive();


        public Form1()
        {
            InitializeComponent();
        }

        private Boolean IsTheConsoleOpen()
        {
            if (!isProcessActive())    //проверяем есть ли наш процесс
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
                dllSendCommand(2);

                dllAcceptConfirm();
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            comboBox.SelectedIndex = 0;//при старте в комобобоксе 1
        }

        private void ButStart_Click(object sender, EventArgs e)
        {
            int NumOfThreads = comboBox.SelectedIndex + 1;  // количество потоков

            if (IsTheConsoleOpen())
            {
                for (int i = 0; i < NumOfThreads; i++)
                {
                    dllSendCommand(0);
                    // Блокирует текущий поток до получения сигнала объектом WaitHandle.
                    dllAcceptConfirm();
                    listBox1.Items.Add("Поток №" + (listBox1.Items.Count - 2).ToString());
                }
            }
            else
            {
                dllStartProcess();

                // Блокирует текущий поток до получения сигнала объектом WaitHandle.
                int a = dllAcceptConfirm();

                listBox1.Items.Add("Все потоки");
                listBox1.Items.Add("Главный поток");
                listBox1.SetSelected(1, true);
            };
        }


        private void ButDel_Click(object sender, EventArgs e)
        {
            if (!IsTheConsoleOpen()) return;

            dllSendCommand(1);

            dllAcceptConfirm();

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

            string message = textBox1.Text;
            int recipient = listBox1.SelectedIndex - 1;

            dllSendCommand(3);

            dllSendMessage(message, recipient);

            dllAcceptConfirm();
            textBox1.Text = "";
        }

    }
}
