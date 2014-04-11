namespace DesuraLogRecorder
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
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
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.dgMessages = new System.Windows.Forms.DataGridView();
            this.butRecord = new System.Windows.Forms.Button();
            this.butConnect = new System.Windows.Forms.Button();
            this.butExport = new System.Windows.Forms.Button();
            this.Time = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.App = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Thread = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Function = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Module = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Message = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ClassInfo = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.dgMessages)).BeginInit();
            this.SuspendLayout();
            // 
            // dgMessages
            // 
            this.dgMessages.AllowUserToAddRows = false;
            this.dgMessages.AllowUserToDeleteRows = false;
            this.dgMessages.AllowUserToOrderColumns = true;
            this.dgMessages.AllowUserToResizeColumns = false;
            this.dgMessages.AllowUserToResizeRows = false;
            this.dgMessages.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.dgMessages.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.dgMessages.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Time,
            this.App,
            this.Thread,
            this.Function,
            this.Module,
            this.Message,
            this.ClassInfo});
            this.dgMessages.Location = new System.Drawing.Point(12, 40);
            this.dgMessages.Name = "dgMessages";
            this.dgMessages.ReadOnly = true;
            this.dgMessages.RowHeadersVisible = false;
            this.dgMessages.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.dgMessages.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.dgMessages.ShowEditingIcon = false;
            this.dgMessages.Size = new System.Drawing.Size(843, 590);
            this.dgMessages.TabIndex = 0;
            // 
            // butRecord
            // 
            this.butRecord.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.butRecord.Location = new System.Drawing.Point(780, 12);
            this.butRecord.Name = "butRecord";
            this.butRecord.Size = new System.Drawing.Size(75, 23);
            this.butRecord.TabIndex = 1;
            this.butRecord.Text = "Record";
            this.butRecord.UseVisualStyleBackColor = true;
            // 
            // butConnect
            // 
            this.butConnect.Location = new System.Drawing.Point(12, 12);
            this.butConnect.Name = "butConnect";
            this.butConnect.Size = new System.Drawing.Size(75, 23);
            this.butConnect.TabIndex = 2;
            this.butConnect.Text = "Connect";
            this.butConnect.UseVisualStyleBackColor = true;
            this.butConnect.Click += new System.EventHandler(this.butConnect_Click);
            // 
            // butExport
            // 
            this.butExport.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.butExport.Location = new System.Drawing.Point(699, 12);
            this.butExport.Name = "butExport";
            this.butExport.Size = new System.Drawing.Size(75, 23);
            this.butExport.TabIndex = 3;
            this.butExport.Text = "Export";
            this.butExport.UseVisualStyleBackColor = true;
            this.butExport.Click += new System.EventHandler(this.butExport_Click);
            // 
            // Time
            // 
            this.Time.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.DisplayedCells;
            this.Time.HeaderText = "Time";
            this.Time.Name = "Time";
            this.Time.ReadOnly = true;
            this.Time.Width = 53;
            // 
            // App
            // 
            this.App.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.DisplayedCells;
            this.App.HeaderText = "App";
            this.App.Name = "App";
            this.App.ReadOnly = true;
            this.App.Width = 49;
            // 
            // Thread
            // 
            this.Thread.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.DisplayedCells;
            this.Thread.HeaderText = "Thread";
            this.Thread.Name = "Thread";
            this.Thread.ReadOnly = true;
            this.Thread.Width = 64;
            // 
            // Function
            // 
            this.Function.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.DisplayedCells;
            this.Function.HeaderText = "Function";
            this.Function.Name = "Function";
            this.Function.ReadOnly = true;
            this.Function.Width = 71;
            // 
            // Module
            // 
            this.Module.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.DisplayedCells;
            this.Module.HeaderText = "Module";
            this.Module.Name = "Module";
            this.Module.ReadOnly = true;
            this.Module.Width = 65;
            // 
            // Message
            // 
            this.Message.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.Message.HeaderText = "Message";
            this.Message.Name = "Message";
            this.Message.ReadOnly = true;
            // 
            // ClassInfo
            // 
            this.ClassInfo.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.ClassInfo.HeaderText = "Class Info";
            this.ClassInfo.Name = "ClassInfo";
            this.ClassInfo.ReadOnly = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(867, 642);
            this.Controls.Add(this.butExport);
            this.Controls.Add(this.butConnect);
            this.Controls.Add(this.butRecord);
            this.Controls.Add(this.dgMessages);
            this.Name = "Form1";
            this.Text = "Desura Log Recorder";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.dgMessages)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataGridView dgMessages;
        private System.Windows.Forms.Button butRecord;
        private System.Windows.Forms.Button butConnect;
        private System.Windows.Forms.Button butExport;
        private System.Windows.Forms.DataGridViewTextBoxColumn Time;
        private System.Windows.Forms.DataGridViewTextBoxColumn App;
        private System.Windows.Forms.DataGridViewTextBoxColumn Thread;
        private System.Windows.Forms.DataGridViewTextBoxColumn Function;
        private System.Windows.Forms.DataGridViewTextBoxColumn Module;
        private System.Windows.Forms.DataGridViewTextBoxColumn Message;
        private System.Windows.Forms.DataGridViewTextBoxColumn ClassInfo;
    }
}

