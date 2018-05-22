using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using indicator_model.listener;
using indicator_model.job;

namespace trader_monitor
{
    public partial class TraderMonitorForm : Form
    {

        process_launcher m_process;
        bool print_enabled_ = true;
        bool loading_ = true;
        string last_msg_ = "";

        public TraderMonitorForm()
        {
            InitializeComponent();
        }

        void load_text(string key, TextBox tb)
        {
            if (!price_monitor_form_data.g_instance.m_dict.ContainsKey(key))
                return;
            var s = price_monitor_form_data.g_instance.m_dict[key];
            if (!string.IsNullOrEmpty(s))
                tb.Text = s;

        }
        private void load_parameter()
        {
            load_text("textBoxExe", textBoxExe);
            load_text("textBoxParameter", textBoxParameter);
            load_text("textBoxTime", textBoxTime);

            checkBox_simnow.Checked = textBoxParameter.Text.Contains("--simnow=true");
        }

        private void TraderMonitorForm_Load(object sender, EventArgs e)
        {
            create_process_launcher();

            //////////////////////////////////////////////////////////////////////////
            tabControl1.Dock = DockStyle.Fill;

            //////////////////////////////////////////////////////////////////////////
            price_monitor_form_data.load_font(this, null);

            //////////////////////////////////////////////////////////////////////////
            load_parameter();

            //////////////////////////////////////////////////////////////////////////
            tabControl1.Focus();
            tabPage1.Focus();
            button_start.Focus();
            button_stop.Enabled = false;

            //////////////////////////////////////////////////////////////////////////
            loading_ = false;
        }


        public void print(string msg)
        {
            if (!print_enabled_)
                return;
            //if (last_msg_ == msg) return;
            last_msg_ = msg;
            msg = DateTime.Now.ToLongDateString() + " " + DateTime.Now.ToLongTimeString()+
                " " + msg;
            Action<string> a = (x) =>
            {
                if (textBoxMessage.Lines.Length > 10 * 1000)
                    textBoxMessage.Clear();
                if (!msg.EndsWith("\n"))
                    msg += "\n";
                textBoxMessage.SelectionStart = textBoxMessage.Text.Length;
                textBoxMessage.AppendText(msg);
            };
            if (textBoxMessage.InvokeRequired)
            {
                textBoxMessage.BeginInvoke(a, msg);
            }
            else
            {
                a.Invoke(msg);
            }
        }
        private void create_process_launcher()
        {
            Action<string> on_print =

                (s) =>
                {
                    this.print(s);
                };
            Func<string> on_get_paramter = () =>
            {
                return this.textBoxParameter.Text;
            };
            var param = new process_launcher.Parameters();
            param.on_print = on_print;
            param.on_get_paramter = on_get_paramter;
            param.on_get_use_job = () => { return false;};// this.checkBoxUseJob.Checked; };
            param.on_get_use_debug = () => { return false;};// this.checkBox_use_debug.Checked; };
            m_process = new process_launcher(param);
        }

        private void TraderMonitorForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_process.close_processes();
            price_monitor_form_data.save_font(null, this);
        }

        private void button_start_Click(object sender, EventArgs e)
        {
            button_stop_Click(null, null);

            m_process.start(textBoxExe.Text, use_debug: false, hide_ui: false);

            button_start.Enabled = false;
            button_stop.Enabled = true;
            button_stop.Focus();
        }

        private void button_stop_Click(object sender, EventArgs e)
        {
            m_process.close_processes();

            button_start.Enabled = true;
            button_stop.Enabled = false;
            button_start.Focus();
        }

        private void textBoxExe_TextChanged(object sender, EventArgs e)
        {
            if (loading_) return;

            //////////////////////////////////////////////////////////////////////////
            price_monitor_form_data.g_instance.m_dict["textBoxExe"] = textBoxExe.Text;
            price_monitor_form_data.g_instance.m_dict["textBoxParameter"] = textBoxParameter.Text;
            price_monitor_form_data.g_instance.m_dict["textBoxTime"] = textBoxTime.Text;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            try
            {
                //解析时间
                List<int> times = new List<int>();
                if (!parse_time(textBoxTime.Text, times))
                {
                    print("时间解析出错:" + textBoxTime.Text);
                    return;
                }
                var curr_time = get_current_time();

                if (times.IndexOf(curr_time) != -1)
                {
                    //找到了,要重启程序
                    restart_exe(curr_time);
                }
            }
            catch (Exception e1)
            {
                print(e1.ToString());
            }
        }

        void restart_exe(int curr_time){
            //目前时间
            var key = "last_run_" + curr_time.ToString();
            if (price_monitor_form_data.g_instance.m_dict.ContainsKey(key))
            {
                var s_last_time = price_monitor_form_data.g_instance.m_dict[key];

                int i_diff_seconds = 0;
                if (diff(s_last_time, out i_diff_seconds) &&
                    i_diff_seconds < 10 * 60)//10分钟
                    return;
            }
            //启动
            price_monitor_form_data.g_instance.m_dict[key]=DateTime.Now.ToLongDateString()+" "+
            DateTime.Now.ToLongTimeString();

            button_stop_Click(null, null);
            button_start_Click(null, null);
        }
        static bool diff(string s,out int seconds)
        {
            seconds = 0;
            DateTime last;
            if( ! DateTime.TryParse(s,out last))
                return false;
            var diff = DateTime.Now - last;
            seconds = (int)diff.TotalSeconds;
            return true;
        }
        static int get_current_time()
        {
            var n = DateTime.Now;
            return n.Hour * 100 + n.Minute;
        }

        static bool parse_time(string s, List<int> ret)
        {
            var arr = s.Trim().Split(new char[] { ',' });
            for (int i = 0; i < arr.Length; i++)
            {
                var ts = arr[i].Trim();
                if (string.IsNullOrEmpty(ts))
                    continue;
                if (ts.Length != 5)
                    return false;
                var f = ts.Split(new char[] { ':' });
                if (f.Length != 2)
                    return false;
                int t = 0;
                t = int.Parse(f[0])*100;
                if (f.Length >= 2)
                    t += int.Parse(f[1]);
                ret.Add(t);
            }
            return true;
        }

        private void button_quit_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void checkBox_simnow_CheckedChanged(object sender, EventArgs e)
        {
            if (loading_) return;
            var t = textBoxParameter.Text;
            if (checkBox_simnow.Checked)
                t = t.Replace("--simnow=false", "--simnow=true");
            else
                t = t.Replace("--simnow=true", "--simnow=false");
            textBoxParameter.Text = t;
        }
    }
}
