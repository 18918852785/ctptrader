using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace trader_monitor
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main()
        {
            bool ret = true;

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new TraderMonitorForm());


        quit:
            indicator_model.job.command_line.exit(ret ? 0 : 1);
        }
    }
}
