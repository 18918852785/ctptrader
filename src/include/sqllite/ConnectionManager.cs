/*
 SQLite transactions in a multi-threaded application
 http://stackoverflow.com/questions/27551763/sqlite-transactions-in-a-multi-threaded-application
 */
/*
 * 多进程并发读写: WAL
 * http://www.sqlite.org/wal.html
 PRAGMA journal_mode=WAL
An SQLite database connection defaults to journal_mode=DELETE. To convert to WAL mode, use the following pragma:

PRAGMA journal_mode=WAL;
The journal_mode pragma returns a string which is the new journal mode. On success, the pragma will return the string "wal". If the conversion to WAL could not be completed (for example, if the VFS does not support the necessary shared-memory primitives) then the journaling mode will be unchanged and the string returned from the primitive will be the prior journaling mode (for example "delete").
 * */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Diagnostics;
using System.Data.SQLite;

namespace indicator_model.future_analyzer.db.sqllite
{
    public class ConnectionManager
    {

        public int BusyTimeout { get; set; }

        public static ConnectionManager Instance
        {
            get
            {
                if (iInstance == null)
                {
                    lock (instanceLock)
                    {
                        if (iInstance == null)
                            iInstance = new ConnectionManager();
                    }
                }
                return iInstance;
            }
        }
        private static ConnectionManager iInstance = null;

        private static object instanceLock;

        private ConnectionManager()
        {
            BusyTimeout = Convert.ToInt32(TimeSpan.FromMinutes(2).TotalMilliseconds);
        }

        static ConnectionManager()
        {
            instanceLock = new object();
        }

        public SQLiteConnection CreateConnection(string connectionString)
        {
            SQLiteConnection connection = new SQLiteConnection(connectionString);
            connection.Open();

            using (SQLiteCommand command = connection.CreateCommand())
            {
                command.CommandText = "PRAGMA journal_mode=WAL";
                using (var r = command.ExecuteReader())
                {
                    while (r.Read())
                    {
                        var result = r[0];
                        Debug.Assert(result.Equals("wal"));
                    }
                }

                command.CommandText = string.Format("PRAGMA busy_timeout={0}", BusyTimeout);
                command.ExecuteNonQuery();

            }
            return connection;
        }

        public static void test(string conn_string)
        {
            var connectionString  = conn_string;
            var _connectionManager = ConnectionManager.Instance;
            //To obtain and use a database connection, code like this is used in each thread:
            using (SQLiteConnection connection = _connectionManager.CreateConnection(connectionString)) 
            {
                // Thread's database operation code here
                string sql = "select * from highscores";

                SQLiteCommand command = new SQLiteCommand(sql, connection);
                var r = command.ExecuteReader();
                var reader = r as SQLiteDataReader;
                while(reader.Read())
                //foreach (var row in reader)
                {
                    //foreach (var v in row)
                    {
                        //Debug.WriteLine(row);
                    }
                    for (int i = 0; i < reader.FieldCount; i++)
                    {
                        var v = reader[i];
                        System.Console.Write(v);
                        System.Console.Write(",");
                        //Debug.WriteLine(v);
                    }
                    System.Console.Write("");
                }
            }        
        }
    }
}
