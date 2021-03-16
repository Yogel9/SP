using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SPDovidenkov
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            comboBox.SelectedIndex = 0;//при старте в комобобоксе 1
        }

        private void ButStart_Click(object sender, EventArgs e)
        {

        }

        private void ButDel_Click(object sender, EventArgs e)
        {

        }
    }
}
