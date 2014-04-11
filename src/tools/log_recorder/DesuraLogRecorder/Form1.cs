using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace DesuraLogRecorder
{
    public partial class Form1 : Form
    {
        bool _Connected = false;
        Thread _Thread;
        MemoryMappedFile _MappedFile;

        public Form1()
        {
            InitializeComponent();
            butRecord.Enabled = false;
            butExport.Enabled = false;

            dgMessages.Sort(dgMessages.Columns[0], ListSortDirection.Descending);
        }

        delegate string GetValueDelegate();

        bool GetDymProp(dynamic json, String name, ref String val)
        {
            if (!new List<String>(json.Keys).Contains(name))
                return false;
            
            val = json[name];
            return true;
        }

        String GetDymProp(dynamic json, String name, String def)
        {
            String val = "";

            if (GetDymProp(json, name, ref val))
                return val;

            return def;
        }

        private void Connect()
        {
            if (_MappedFile != null)
            {
                _MappedFile.Dispose();
                _MappedFile = null;
            }

            while (true)
            {
                try
                {
                    _MappedFile = MemoryMappedFile.OpenExisting("DESURA_CLIENT_TRACER_OUTPUT", MemoryMappedFileRights.Read);
                    break;
                }
                catch (Exception e)
                {
                   System.Threading.Thread.Sleep(1000);
                }
            }

            Action connectCb = () =>
            {
                butConnect.Text = "Disconnect";
                butConnect.Enabled = true;
                butRecord.Enabled = true;
                butExport.Enabled = true;
                _Connected = true;
            };

            BeginInvoke(connectCb);

            using (var accessor = _MappedFile.CreateViewAccessor(0, 0, MemoryMappedFileAccess.Read))
            {
                int pid = accessor.ReadInt32(0);

                if (!TryFindingProcess(pid))
                    return;

                ProcessRows(accessor);
            }
        }

        private void NewRow(dynamic r)
        {
            String time = "";
            String thread = "";

            if (!GetDymProp(r, "time", ref time))
                return;

            if (!GetDymProp(r, "thread", ref thread))
                return;

            DataGridViewRow row = null;

            if (dgMessages.Rows.Count < 4096)
            {
                row = new DataGridViewRow();
            }
            else
            {
                row = dgMessages.Rows[dgMessages.Rows.GetLastRow(DataGridViewElementStates.None)];
                dgMessages.Rows.Remove(row);
                row.Cells.Clear();
            }

            var message = GetDymProp(r, "message", "");
            var app = GetDymProp(r, "app", "Desura");

            row.Tag = r;

            row.Cells.Add(new DataGridViewTextBoxCell() { Value = time });
            row.Cells.Add(new DataGridViewTextBoxCell() { Value = app });
            row.Cells.Add(new DataGridViewTextBoxCell() { Value = thread });
            row.Cells.Add(new DataGridViewTextBoxCell() { Value = GetDymProp(r, "function", "") });
            row.Cells.Add(new DataGridViewTextBoxCell() { Value = GetDymProp(r, "module", "") });
            row.Cells.Add(new DataGridViewTextBoxCell() { Value = message });
            row.Cells.Add(new DataGridViewTextBoxCell() { Value = GetDymProp(r, "classinfo", "") });

            if (app == "Service")
                row.DefaultCellStyle.ForeColor = Color.Blue;
            else if (app == "Tool")
                row.DefaultCellStyle.ForeColor = Color.Green;
            else if (message.StartsWith("Warning:"))
                row.DefaultCellStyle.ForeColor = Color.Red;
            else
                row.DefaultCellStyle.ForeColor = Color.Black;

            dgMessages.Rows.Insert(0, row);
        }

        private void ProcessRows(MemoryMappedViewAccessor accessor)
        {
            UInt32 lastPos = 0;

            _BlockCount = accessor.ReadUInt16(8);
            _BlockSize = accessor.ReadUInt16(10);

            while (true)
            {
                UInt32 curPos = accessor.ReadUInt32(4) % _BlockCount;

                var bag = _MissingItems;
                _MissingItems = new ConcurrentBag<int>();

                int temp = 0;
                while (bag.TryTake(out temp))
                    readLineAndProcess(accessor, temp);

                if (lastPos == curPos)
                {
                    System.Threading.Thread.Sleep(1000);
                }
                else
                {
                    if (lastPos < curPos)
                    {
                        for (UInt32 x = lastPos; x < curPos; ++x)
                            readLineAndProcess(accessor, (int)x);
                    }
                    else
                    {
                        for (UInt32 x = lastPos; x < _BlockCount; ++x)
                            readLineAndProcess(accessor, (int)x);

                        for (UInt32 x = 0; x < curPos; ++x)
                            readLineAndProcess(accessor, (int)x);
                    }

                    lastPos = curPos;
                }
            };
        }

        private bool TryFindingProcess(int pid)
        {
            try
            {
                var proc = Process.GetProcessById(pid);
                proc.EnableRaisingEvents = true;
                proc.Exited += (s, e) =>
                {
                    Action end = () =>
                    {
                        if (_Connected)
                            butConnect_Click(null, null);
                    };
                    BeginInvoke(end);
                };
            }
            catch
            {
                Action end = () =>
                {
                    if (_Connected)
                        butConnect_Click(null, null);
                };
                BeginInvoke(end);
                return false;
            }

            return true;
        }

        byte[] _Buffer = new byte[512];
        ConcurrentBag<int> _MissingItems = new ConcurrentBag<int>();

        private void readLineAndProcess(MemoryMappedViewAccessor accessor, int row)
        {
            accessor.ReadArray<byte>(12 + (row * _BlockSize), _Buffer, 0, _BlockSize);

            if (_Buffer[0] == 0)
            {
                _MissingItems.Add(row);
                return;
            }
                
            var str = System.Text.Encoding.Default.GetString(_Buffer);

            int index = str.IndexOf('\0');

            if (index >= 0)
                str = str.Remove(index);

            dynamic obj;
            if (SimpleJson.TryDeserializeObject(str, out obj))
            {
                Action action = () => NewRow(obj);
                BeginInvoke(action);
            }
            else
            {
                _MissingItems.Add(row);
            }
        }

        private void butConnect_Click(object sender, EventArgs e)
        {
            if (_Connected)
            {
                if (_Thread != null)
                {
                    _Thread.Abort();
                    _Thread = null;
                }

                _Connected = false;
                butConnect.Text = "Connect";
                
                butRecord.Enabled = false;

                DataGridViewRow row = new DataGridViewRow();

                var first = dgMessages.Rows[dgMessages.Rows.GetFirstRow(DataGridViewElementStates.None)].Cells[0];

                row.Cells.Add(new DataGridViewTextBoxCell() { Value = first.Value });
                row.Cells.Add(new DataGridViewTextBoxCell() { Value = "" });
                row.Cells.Add(new DataGridViewTextBoxCell() { Value = "" });
                row.Cells.Add(new DataGridViewTextBoxCell() { Value = "" });
                row.Cells.Add(new DataGridViewTextBoxCell() { Value = "" });
                row.Cells.Add(new DataGridViewTextBoxCell() { Value = "Disconnected from server" });
                row.Cells.Add(new DataGridViewTextBoxCell() { Value = "" });

                row.DefaultCellStyle.ForeColor = Color.Red;
                dgMessages.Rows.Insert(0, row);
            }
            else
            {
                if (_Thread == null)
                {
                    _Thread = new Thread(Connect);
                    _Thread.IsBackground = true;
                    _Thread.Start();

                }

                dgMessages.Rows.Clear();

                butConnect.Text = "Connecting...";
                butConnect.Enabled = false;
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (_Thread != null)
            {
                _Thread.Abort();
                _Thread = null;
            }
        }

        private void butExport_Click(object sender, EventArgs e)
        {
            var dlg = new SaveFileDialog();

            dlg.Title = "Desura Log Export";
            dlg.FileName = "Desura_Export.json";
            dlg.DefaultExt = ".json";
            dlg.AddExtension = true;

            if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                using (var fh = dlg.OpenFile())
                using (var s = new StreamWriter(fh))
                {
                    var first = true;

                    s.Write("[\n\t");

                    for (int x=dgMessages.Rows.Count; x>0; x--)
                    {
                        var row = dgMessages.Rows[x-1];

                        if (row == null || row.Tag == null)
                            continue;

                        if (!first)
                            s.Write(",\n\t");

                        first = false;

                        var res = SimpleJson.SerializeObject(row.Tag);
                        s.Write(res);
                    }

                    s.Write("\n]");
                }
            }
        }

        public ushort _BlockCount { get; set; }

        public ushort _BlockSize { get; set; }
    }
}
