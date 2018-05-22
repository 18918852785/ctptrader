using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using System.Data;
using System.Data.SQLite;
using System.Diagnostics;
using System.Threading;

namespace indicator_model.future_analyzer.db.sqllite
{
    class sqlutil
    {
        public static string DB_PATH;// { get { return get_db_path();}}

        public static string get_db_path()
        {
            var name = "future_db.log";
            //name += Process.GetCurrentProcess().Id.ToString();
            return get_log_folder() + "\\" + name;
        }

        public static string get_log_folder()
        {
            var folder = AppDomain.CurrentDomain.SetupInformation.ApplicationBase;
            if (folder[folder.Length - 1] != '\\')
                folder += "\\";
            folder += "..\\log";
            if (!Directory.Exists(folder))
                Directory.CreateDirectory(folder);
            return folder;
        }


        public static DataTable GetDataTable(string sql)
        {
            DataTable dt = new DataTable();
            try
            {
                SQLiteConnection cnn = new SQLiteConnection("Data Source=C:CheckoutWorldDominator.s3db");
                cnn.Open();
                SQLiteCommand mycommand = new SQLiteCommand(cnn);
                mycommand.CommandText = sql;
                SQLiteDataReader reader = mycommand.ExecuteReader();
                dt.Load(reader);
                reader.Close();
                cnn.Close();
            }
            catch
            {
                // Catching exceptions is for communists
            }
            return dt;
        }

        public static int ExecuteNonQuery(string sql)
        {
            SQLiteConnection cnn = new SQLiteConnection(string.Format("Data Source={0}", DB_PATH));
            cnn.Open();
            SQLiteCommand mycommand = new SQLiteCommand(cnn);
            mycommand.CommandText = sql;
            int rowsUpdated = mycommand.ExecuteNonQuery();
            cnn.Close();
            return rowsUpdated;
        }

        public static string ExecuteScalar(string sql)
        {
            SQLiteConnection cnn = new SQLiteConnection(string.Format("Data Source={0}", DB_PATH));
            cnn.Open();
            SQLiteCommand mycommand = new SQLiteCommand(cnn);
            mycommand.CommandText = sql;
            object value = mycommand.ExecuteScalar();
            cnn.Close();
            if (value != null)
            {
                return value.ToString();
            }
            return "";
        }

        public static void test()
        {
            DB_PATH = get_db_path();//初始化

            var conn_string = get_connection_string(DB_PATH);

            SQLiteConnection m_dbConnection = new SQLiteConnection( conn_string );
            m_dbConnection.Open();

            string sql = "create table highscores (name varchar(20), score int)";

            SQLiteCommand command = new SQLiteCommand(sql, m_dbConnection);
            command.ExecuteNonQuery();

            sql = "insert into highscores (name, score) values ('Me', 9001)";

            command = new SQLiteCommand(sql, m_dbConnection);
            command.ExecuteNonQuery();

            m_dbConnection.Close();

            sqlutil.ExecuteNonQuery("create table a( id int)");
        }

        public static void create_db_file(string db_path,out bool exist)
        {
            exist = false;
            if (  File.Exists(db_path))
            {
                exist = true;
                return;
            }
            {
                //File.Delete(DB_PATH);
                SQLiteConnection.CreateFile(db_path);
            }
        }

        public static string get_connection_string(string db_path)
        {
            string conn_string;
            //http://stackoverflow.com/questions/19111064/cannot-use-sqlite-wal-mode
            //Cannot use SQLite WAL mode
            SQLiteConnectionStringBuilder connectionStringBuilder = new SQLiteConnectionStringBuilder();
            connectionStringBuilder.DataSource = DB_PATH;
            connectionStringBuilder.JournalMode = SQLiteJournalModeEnum.Delete;//Wal
            conn_string = connectionStringBuilder.ToString();

            conn_string = string.Format("Data Source={0};PRAGMA journal_mode=WAL;", db_path);
            return conn_string;
        }
    }
}
