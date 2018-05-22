namespace trader_monitor
{
    partial class TraderMonitorForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.checkBox_simnow = new System.Windows.Forms.CheckBox();
            this.button_quit = new System.Windows.Forms.Button();
            this.textBoxTime = new System.Windows.Forms.TextBox();
            this.textBoxMessage = new System.Windows.Forms.TextBox();
            this.button_stop = new System.Windows.Forms.Button();
            this.textBoxParameter = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.button_start = new System.Windows.Forms.Button();
            this.textBoxExe = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Location = new System.Drawing.Point(12, 12);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(684, 292);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.checkBox_simnow);
            this.tabPage1.Controls.Add(this.button_quit);
            this.tabPage1.Controls.Add(this.textBoxTime);
            this.tabPage1.Controls.Add(this.textBoxMessage);
            this.tabPage1.Controls.Add(this.button_stop);
            this.tabPage1.Controls.Add(this.textBoxParameter);
            this.tabPage1.Controls.Add(this.label2);
            this.tabPage1.Controls.Add(this.button_start);
            this.tabPage1.Controls.Add(this.textBoxExe);
            this.tabPage1.Controls.Add(this.label1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(676, 266);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "tabPage1";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // checkBox_simnow
            // 
            this.checkBox_simnow.AutoSize = true;
            this.checkBox_simnow.Checked = true;
            this.checkBox_simnow.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBox_simnow.Location = new System.Drawing.Point(529, 49);
            this.checkBox_simnow.Name = "checkBox_simnow";
            this.checkBox_simnow.Size = new System.Drawing.Size(60, 16);
            this.checkBox_simnow.TabIndex = 29;
            this.checkBox_simnow.Text = "Simnow";
            this.checkBox_simnow.UseVisualStyleBackColor = true;
            this.checkBox_simnow.CheckedChanged += new System.EventHandler(this.checkBox_simnow_CheckedChanged);
            // 
            // button_quit
            // 
            this.button_quit.Location = new System.Drawing.Point(582, 76);
            this.button_quit.Name = "button_quit";
            this.button_quit.Size = new System.Drawing.Size(75, 23);
            this.button_quit.TabIndex = 28;
            this.button_quit.Text = "&Quit";
            this.button_quit.UseVisualStyleBackColor = true;
            this.button_quit.Click += new System.EventHandler(this.button_quit_Click);
            // 
            // textBoxTime
            // 
            this.textBoxTime.Location = new System.Drawing.Point(314, 78);
            this.textBoxTime.Name = "textBoxTime";
            this.textBoxTime.Size = new System.Drawing.Size(194, 21);
            this.textBoxTime.TabIndex = 27;
            this.textBoxTime.Text = "08:55,20:55";
            this.textBoxTime.TextChanged += new System.EventHandler(this.textBoxExe_TextChanged);
            // 
            // textBoxMessage
            // 
            this.textBoxMessage.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxMessage.Location = new System.Drawing.Point(21, 107);
            this.textBoxMessage.Multiline = true;
            this.textBoxMessage.Name = "textBoxMessage";
            this.textBoxMessage.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.textBoxMessage.Size = new System.Drawing.Size(649, 153);
            this.textBoxMessage.TabIndex = 26;
            this.textBoxMessage.WordWrap = false;
            // 
            // button_stop
            // 
            this.button_stop.Enabled = false;
            this.button_stop.Location = new System.Drawing.Point(164, 78);
            this.button_stop.Name = "button_stop";
            this.button_stop.Size = new System.Drawing.Size(75, 23);
            this.button_stop.TabIndex = 3;
            this.button_stop.Text = "S&top";
            this.button_stop.UseVisualStyleBackColor = true;
            this.button_stop.Click += new System.EventHandler(this.button_stop_Click);
            // 
            // textBoxParameter
            // 
            this.textBoxParameter.Location = new System.Drawing.Point(70, 51);
            this.textBoxParameter.Name = "textBoxParameter";
            this.textBoxParameter.Size = new System.Drawing.Size(438, 21);
            this.textBoxParameter.TabIndex = 1;
            this.textBoxParameter.Text = "--trade=spread --simnow=true --range=\"-25,-36,3,5;-10,-31,3,5;-1,-41,3,5\" --ctp_m" +
    "arket_code=\"m1809,m1901\" --ctp_trade_code=\"SP m1809&m1901\"";
            this.textBoxParameter.TextChanged += new System.EventHandler(this.textBoxExe_TextChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(19, 53);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 12);
            this.label2.TabIndex = 3;
            this.label2.Text = "参数:";
            // 
            // button_start
            // 
            this.button_start.Location = new System.Drawing.Point(70, 78);
            this.button_start.Name = "button_start";
            this.button_start.Size = new System.Drawing.Size(75, 23);
            this.button_start.TabIndex = 2;
            this.button_start.Text = "&Start";
            this.button_start.UseVisualStyleBackColor = true;
            this.button_start.Click += new System.EventHandler(this.button_start_Click);
            // 
            // textBoxExe
            // 
            this.textBoxExe.Location = new System.Drawing.Point(70, 24);
            this.textBoxExe.Name = "textBoxExe";
            this.textBoxExe.Size = new System.Drawing.Size(438, 21);
            this.textBoxExe.TabIndex = 0;
            this.textBoxExe.Text = "WindowsTraderApi.exe";
            this.textBoxExe.TextChanged += new System.EventHandler(this.textBoxExe_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 26);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(47, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "进程名:";
            // 
            // tabPage2
            // 
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(676, 266);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "tabPage2";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 1000;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // TraderMonitorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(703, 314);
            this.Controls.Add(this.tabControl1);
            this.Name = "TraderMonitorForm";
            this.Text = "TraderMonitorForm";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.TraderMonitorForm_FormClosing);
            this.Load += new System.EventHandler(this.TraderMonitorForm_Load);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Button button_stop;
        private System.Windows.Forms.TextBox textBoxParameter;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button button_start;
        private System.Windows.Forms.TextBox textBoxExe;
        private System.Windows.Forms.Label label1;
        public System.Windows.Forms.TextBox textBoxMessage;
        private System.Windows.Forms.TextBox textBoxTime;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Button button_quit;
        private System.Windows.Forms.CheckBox checkBox_simnow;
    }
}

