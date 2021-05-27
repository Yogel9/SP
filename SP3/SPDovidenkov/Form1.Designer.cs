
namespace SPDovidenkov
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.ButStart = new System.Windows.Forms.Button();
            this.ButDel = new System.Windows.Forms.Button();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.comboBox = new System.Windows.Forms.ComboBox();
            this.ButSend = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // ButStart
            // 
            this.ButStart.Location = new System.Drawing.Point(11, 375);
            this.ButStart.Name = "ButStart";
            this.ButStart.Size = new System.Drawing.Size(267, 38);
            this.ButStart.TabIndex = 0;
            this.ButStart.Text = "Start";
            this.ButStart.UseVisualStyleBackColor = true;
            this.ButStart.Click += new System.EventHandler(this.ButStart_Click);
            // 
            // ButDel
            // 
            this.ButDel.Location = new System.Drawing.Point(12, 419);
            this.ButDel.Name = "ButDel";
            this.ButDel.Size = new System.Drawing.Size(266, 36);
            this.ButDel.TabIndex = 1;
            this.ButDel.Text = "Delete";
            this.ButDel.UseVisualStyleBackColor = true;
            this.ButDel.Click += new System.EventHandler(this.ButDel_Click);
            // 
            // listBox1
            // 
            this.listBox1.FormattingEnabled = true;
            this.listBox1.ItemHeight = 15;
            this.listBox1.Location = new System.Drawing.Point(11, 68);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(266, 289);
            this.listBox1.TabIndex = 2;
            // 
            // comboBox
            // 
            this.comboBox.AutoCompleteCustomSource.AddRange(new string[] {
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10",
            "11"});
            this.comboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox.FormattingEnabled = true;
            this.comboBox.Items.AddRange(new object[] {
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9"});
            this.comboBox.Location = new System.Drawing.Point(12, 28);
            this.comboBox.Name = "comboBox";
            this.comboBox.Size = new System.Drawing.Size(266, 23);
            this.comboBox.TabIndex = 3;
            // 
            // ButSend
            // 
            this.ButSend.Location = new System.Drawing.Point(306, 230);
            this.ButSend.Name = "ButSend";
            this.ButSend.Size = new System.Drawing.Size(340, 51);
            this.ButSend.TabIndex = 8;
            this.ButSend.Text = "Send";
            this.ButSend.UseVisualStyleBackColor = true;
            this.ButSend.Click += new System.EventHandler(this.ButSendOne_Click);
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(306, 68);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(340, 138);
            this.textBox1.TabIndex = 7;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(306, 36);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(137, 15);
            this.label1.TabIndex = 9;
            this.label1.Text = "Write your text text here:";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(680, 482);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.ButSend);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.comboBox);
            this.Controls.Add(this.listBox1);
            this.Controls.Add(this.ButDel);
            this.Controls.Add(this.ButStart);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.Text = "Form1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button ButStart;
        private System.Windows.Forms.Button ButDel;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.ComboBox comboBox;
        private System.Windows.Forms.Button ButSend;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label1;
    }
}

