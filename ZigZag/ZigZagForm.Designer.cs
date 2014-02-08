namespace ZigZag
{
  partial class ZigZagForm
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
      this.components = new System.ComponentModel.Container();
      System.Windows.Forms.Label label2;
      System.Windows.Forms.Label label4;
      System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
      System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
      System.Windows.Forms.ToolStripMenuItem simulationToolStripMenuItem;
      System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
      System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
      this.StartButton = new System.Windows.Forms.Button();
      this.TestButton = new System.Windows.Forms.Button();
      this.ConsoleTextBox = new System.Windows.Forms.TextBox();
      this.tabControl1 = new System.Windows.Forms.TabControl();
      this.tabPage1 = new System.Windows.Forms.TabPage();
      this.screenContextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.zoomHereMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.zoomOutHereMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.centerHereMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.tabPage2 = new System.Windows.Forms.TabPage();
      this.checkBoxK = new System.Windows.Forms.CheckBox();
      this.checkBoxF = new System.Windows.Forms.CheckBox();
      this.checkBoxPaused = new System.Windows.Forms.CheckBox();
      this.buttonUp = new System.Windows.Forms.Button();
      this.buttonLeft = new System.Windows.Forms.Button();
      this.buttonRight = new System.Windows.Forms.Button();
      this.buttonDown = new System.Windows.Forms.Button();
      this.buttonZoomOut = new System.Windows.Forms.Button();
      this.buttonZoomIn = new System.Windows.Forms.Button();
      this.label1 = new System.Windows.Forms.Label();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.updatesLabel = new System.Windows.Forms.Label();
      this.activePageLabel = new System.Windows.Forms.Label();
      this.mainStatusStrip = new System.Windows.Forms.StatusStrip();
      this.mainStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
      this.splitContainer1 = new System.Windows.Forms.SplitContainer();
      this.controlPanel = new System.Windows.Forms.Panel();
      this.mainMenuStrip = new System.Windows.Forms.MenuStrip();
      this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.openDataDirToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.startToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.stopToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.pauseToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.controlPanelToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.statusBarToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.DisplayPictureBox = new ZigZag.BitmapPictureBox();
      label2 = new System.Windows.Forms.Label();
      label4 = new System.Windows.Forms.Label();
      fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
      simulationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
      this.tabControl1.SuspendLayout();
      this.tabPage1.SuspendLayout();
      this.screenContextMenuStrip.SuspendLayout();
      this.tabPage2.SuspendLayout();
      this.groupBox1.SuspendLayout();
      this.mainStatusStrip.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
      this.splitContainer1.Panel1.SuspendLayout();
      this.splitContainer1.SuspendLayout();
      this.controlPanel.SuspendLayout();
      this.mainMenuStrip.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.DisplayPictureBox)).BeginInit();
      this.SuspendLayout();
      // 
      // label2
      // 
      label2.AutoSize = true;
      label2.Location = new System.Drawing.Point(10, 19);
      label2.Name = "label2";
      label2.Size = new System.Drawing.Size(72, 15);
      label2.TabIndex = 15;
      label2.Text = "Active page:";
      // 
      // label4
      // 
      label4.AutoSize = true;
      label4.Location = new System.Drawing.Point(10, 49);
      label4.Name = "label4";
      label4.Size = new System.Drawing.Size(53, 15);
      label4.TabIndex = 17;
      label4.Text = "Updates:";
      // 
      // StartButton
      // 
      this.StartButton.Location = new System.Drawing.Point(3, 12);
      this.StartButton.Name = "StartButton";
      this.StartButton.Size = new System.Drawing.Size(101, 23);
      this.StartButton.TabIndex = 1;
      this.StartButton.Text = "Start";
      this.StartButton.UseVisualStyleBackColor = true;
      this.StartButton.Click += new System.EventHandler(this.StartButton_Click);
      // 
      // TestButton
      // 
      this.TestButton.Location = new System.Drawing.Point(3, 41);
      this.TestButton.Name = "TestButton";
      this.TestButton.Size = new System.Drawing.Size(101, 23);
      this.TestButton.TabIndex = 2;
      this.TestButton.Text = "Test";
      this.TestButton.UseVisualStyleBackColor = true;
      this.TestButton.Visible = false;
      this.TestButton.Click += new System.EventHandler(this.TestButton_Click);
      // 
      // ConsoleTextBox
      // 
      this.ConsoleTextBox.BackColor = System.Drawing.SystemColors.Window;
      this.ConsoleTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
      this.ConsoleTextBox.Font = new System.Drawing.Font("Lucida Console", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ConsoleTextBox.Location = new System.Drawing.Point(3, 3);
      this.ConsoleTextBox.Multiline = true;
      this.ConsoleTextBox.Name = "ConsoleTextBox";
      this.ConsoleTextBox.ReadOnly = true;
      this.ConsoleTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
      this.ConsoleTextBox.Size = new System.Drawing.Size(1390, 624);
      this.ConsoleTextBox.TabIndex = 3;
      // 
      // tabControl1
      // 
      this.tabControl1.Controls.Add(this.tabPage1);
      this.tabControl1.Controls.Add(this.tabPage2);
      this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tabControl1.Location = new System.Drawing.Point(0, 0);
      this.tabControl1.Name = "tabControl1";
      this.tabControl1.SelectedIndex = 0;
      this.tabControl1.Size = new System.Drawing.Size(1404, 656);
      this.tabControl1.TabIndex = 4;
      // 
      // tabPage1
      // 
      this.tabPage1.Controls.Add(this.DisplayPictureBox);
      this.tabPage1.Controls.Add(this.controlPanel);
      this.tabPage1.Location = new System.Drawing.Point(4, 24);
      this.tabPage1.Name = "tabPage1";
      this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
      this.tabPage1.Size = new System.Drawing.Size(1396, 628);
      this.tabPage1.TabIndex = 0;
      this.tabPage1.Text = "Screen";
      this.tabPage1.UseVisualStyleBackColor = true;
      // 
      // screenContextMenuStrip
      // 
      this.screenContextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.zoomHereMenuItem,
            this.zoomOutHereMenuItem,
            this.centerHereMenuItem});
      this.screenContextMenuStrip.Name = "screenContextMenuStrip";
      this.screenContextMenuStrip.Size = new System.Drawing.Size(154, 70);
      this.screenContextMenuStrip.Opening += new System.ComponentModel.CancelEventHandler(this.screenContextMenuStrip_Opening);
      // 
      // zoomHereMenuItem
      // 
      this.zoomHereMenuItem.Name = "zoomHereMenuItem";
      this.zoomHereMenuItem.Size = new System.Drawing.Size(153, 22);
      this.zoomHereMenuItem.Text = "Zoom in here";
      this.zoomHereMenuItem.Click += new System.EventHandler(this.zoomHereMenuItem_Click);
      // 
      // zoomOutHereMenuItem
      // 
      this.zoomOutHereMenuItem.Name = "zoomOutHereMenuItem";
      this.zoomOutHereMenuItem.Size = new System.Drawing.Size(153, 22);
      this.zoomOutHereMenuItem.Text = "Zoom out here";
      this.zoomOutHereMenuItem.Click += new System.EventHandler(this.zoomOutHereMenuItem_Click);
      // 
      // centerHereMenuItem
      // 
      this.centerHereMenuItem.Name = "centerHereMenuItem";
      this.centerHereMenuItem.Size = new System.Drawing.Size(153, 22);
      this.centerHereMenuItem.Text = "Center here";
      this.centerHereMenuItem.Click += new System.EventHandler(this.centerHereMenuItem_Click);
      // 
      // tabPage2
      // 
      this.tabPage2.Controls.Add(this.ConsoleTextBox);
      this.tabPage2.Location = new System.Drawing.Point(4, 24);
      this.tabPage2.Name = "tabPage2";
      this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
      this.tabPage2.Size = new System.Drawing.Size(1396, 628);
      this.tabPage2.TabIndex = 1;
      this.tabPage2.Text = "Console";
      this.tabPage2.UseVisualStyleBackColor = true;
      // 
      // checkBoxK
      // 
      this.checkBoxK.AutoSize = true;
      this.checkBoxK.Location = new System.Drawing.Point(3, 95);
      this.checkBoxK.Name = "checkBoxK";
      this.checkBoxK.Size = new System.Drawing.Size(33, 19);
      this.checkBoxK.TabIndex = 5;
      this.checkBoxK.Tag = "K";
      this.checkBoxK.Text = "K";
      this.checkBoxK.UseVisualStyleBackColor = true;
      this.checkBoxK.CheckedChanged += new System.EventHandler(this.checkBoxK_CheckedChanged);
      // 
      // checkBoxF
      // 
      this.checkBoxF.AutoSize = true;
      this.checkBoxF.Location = new System.Drawing.Point(3, 70);
      this.checkBoxF.Name = "checkBoxF";
      this.checkBoxF.Size = new System.Drawing.Size(32, 19);
      this.checkBoxF.TabIndex = 6;
      this.checkBoxF.Tag = "F";
      this.checkBoxF.Text = "F";
      this.checkBoxF.UseVisualStyleBackColor = true;
      this.checkBoxF.CheckedChanged += new System.EventHandler(this.checkBoxF_CheckedChanged);
      // 
      // checkBoxPaused
      // 
      this.checkBoxPaused.AutoSize = true;
      this.checkBoxPaused.Location = new System.Drawing.Point(3, 120);
      this.checkBoxPaused.Name = "checkBoxPaused";
      this.checkBoxPaused.Size = new System.Drawing.Size(78, 19);
      this.checkBoxPaused.TabIndex = 7;
      this.checkBoxPaused.Text = "Paused (!)";
      this.checkBoxPaused.UseVisualStyleBackColor = true;
      this.checkBoxPaused.CheckedChanged += new System.EventHandler(this.checkBoxPaused_CheckedChanged);
      // 
      // buttonUp
      // 
      this.buttonUp.Location = new System.Drawing.Point(27, 152);
      this.buttonUp.Name = "buttonUp";
      this.buttonUp.Size = new System.Drawing.Size(29, 23);
      this.buttonUp.TabIndex = 8;
      this.buttonUp.Text = "^";
      this.buttonUp.UseVisualStyleBackColor = true;
      this.buttonUp.Click += new System.EventHandler(this.buttonUp_Click);
      // 
      // buttonLeft
      // 
      this.buttonLeft.Location = new System.Drawing.Point(2, 181);
      this.buttonLeft.Name = "buttonLeft";
      this.buttonLeft.Size = new System.Drawing.Size(29, 23);
      this.buttonLeft.TabIndex = 9;
      this.buttonLeft.Text = "<";
      this.buttonLeft.UseVisualStyleBackColor = true;
      this.buttonLeft.Click += new System.EventHandler(this.buttonLeft_Click);
      // 
      // buttonRight
      // 
      this.buttonRight.Location = new System.Drawing.Point(56, 181);
      this.buttonRight.Name = "buttonRight";
      this.buttonRight.Size = new System.Drawing.Size(29, 23);
      this.buttonRight.TabIndex = 10;
      this.buttonRight.Text = ">";
      this.buttonRight.UseVisualStyleBackColor = true;
      this.buttonRight.Click += new System.EventHandler(this.buttonRight_Click);
      // 
      // buttonDown
      // 
      this.buttonDown.Location = new System.Drawing.Point(27, 210);
      this.buttonDown.Name = "buttonDown";
      this.buttonDown.Size = new System.Drawing.Size(29, 23);
      this.buttonDown.TabIndex = 11;
      this.buttonDown.Text = ".";
      this.buttonDown.UseVisualStyleBackColor = true;
      this.buttonDown.Click += new System.EventHandler(this.buttonDown_Click);
      // 
      // buttonZoomOut
      // 
      this.buttonZoomOut.Location = new System.Drawing.Point(4, 253);
      this.buttonZoomOut.Name = "buttonZoomOut";
      this.buttonZoomOut.Size = new System.Drawing.Size(29, 23);
      this.buttonZoomOut.TabIndex = 12;
      this.buttonZoomOut.Text = "-";
      this.buttonZoomOut.UseVisualStyleBackColor = true;
      this.buttonZoomOut.Click += new System.EventHandler(this.buttonZoomOut_Click);
      // 
      // buttonZoomIn
      // 
      this.buttonZoomIn.Location = new System.Drawing.Point(96, 253);
      this.buttonZoomIn.Name = "buttonZoomIn";
      this.buttonZoomIn.Size = new System.Drawing.Size(29, 23);
      this.buttonZoomIn.TabIndex = 13;
      this.buttonZoomIn.Text = "+";
      this.buttonZoomIn.UseVisualStyleBackColor = true;
      this.buttonZoomIn.Click += new System.EventHandler(this.buttonZoomIn_Click);
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(42, 257);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(39, 15);
      this.label1.TabIndex = 14;
      this.label1.Text = "Zoom";
      // 
      // groupBox1
      // 
      this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.groupBox1.Controls.Add(this.updatesLabel);
      this.groupBox1.Controls.Add(label4);
      this.groupBox1.Controls.Add(this.activePageLabel);
      this.groupBox1.Controls.Add(label2);
      this.groupBox1.Location = new System.Drawing.Point(3, 347);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(184, 275);
      this.groupBox1.TabIndex = 16;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "Status";
      // 
      // updatesLabel
      // 
      this.updatesLabel.AutoSize = true;
      this.updatesLabel.Location = new System.Drawing.Point(89, 49);
      this.updatesLabel.Name = "updatesLabel";
      this.updatesLabel.Size = new System.Drawing.Size(12, 15);
      this.updatesLabel.TabIndex = 18;
      this.updatesLabel.Text = "-";
      // 
      // activePageLabel
      // 
      this.activePageLabel.AutoSize = true;
      this.activePageLabel.Location = new System.Drawing.Point(89, 19);
      this.activePageLabel.Name = "activePageLabel";
      this.activePageLabel.Size = new System.Drawing.Size(12, 15);
      this.activePageLabel.TabIndex = 16;
      this.activePageLabel.Text = "-";
      // 
      // mainStatusStrip
      // 
      this.mainStatusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mainStatusLabel});
      this.mainStatusStrip.Location = new System.Drawing.Point(0, 680);
      this.mainStatusStrip.Name = "mainStatusStrip";
      this.mainStatusStrip.Size = new System.Drawing.Size(1404, 22);
      this.mainStatusStrip.TabIndex = 17;
      this.mainStatusStrip.Text = "statusStrip1";
      // 
      // mainStatusLabel
      // 
      this.mainStatusLabel.Name = "mainStatusLabel";
      this.mainStatusLabel.Size = new System.Drawing.Size(1389, 17);
      this.mainStatusLabel.Spring = true;
      this.mainStatusLabel.Text = "Ready.";
      this.mainStatusLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // splitContainer1
      // 
      this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.splitContainer1.Location = new System.Drawing.Point(0, 24);
      this.splitContainer1.Name = "splitContainer1";
      // 
      // splitContainer1.Panel1
      // 
      this.splitContainer1.Panel1.Controls.Add(this.tabControl1);
      this.splitContainer1.Panel2Collapsed = true;
      this.splitContainer1.Size = new System.Drawing.Size(1404, 656);
      this.splitContainer1.SplitterDistance = 1249;
      this.splitContainer1.TabIndex = 18;
      // 
      // controlPanel
      // 
      this.controlPanel.Controls.Add(this.TestButton);
      this.controlPanel.Controls.Add(this.label1);
      this.controlPanel.Controls.Add(this.buttonZoomIn);
      this.controlPanel.Controls.Add(this.groupBox1);
      this.controlPanel.Controls.Add(this.buttonZoomOut);
      this.controlPanel.Controls.Add(this.StartButton);
      this.controlPanel.Controls.Add(this.checkBoxK);
      this.controlPanel.Controls.Add(this.checkBoxF);
      this.controlPanel.Controls.Add(this.checkBoxPaused);
      this.controlPanel.Controls.Add(this.buttonDown);
      this.controlPanel.Controls.Add(this.buttonUp);
      this.controlPanel.Controls.Add(this.buttonRight);
      this.controlPanel.Controls.Add(this.buttonLeft);
      this.controlPanel.Dock = System.Windows.Forms.DockStyle.Right;
      this.controlPanel.Location = new System.Drawing.Point(1196, 3);
      this.controlPanel.Name = "controlPanel";
      this.controlPanel.Size = new System.Drawing.Size(197, 622);
      this.controlPanel.TabIndex = 0;
      this.controlPanel.Visible = false;
      // 
      // mainMenuStrip
      // 
      this.mainMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            fileToolStripMenuItem,
            simulationToolStripMenuItem,
            viewToolStripMenuItem});
      this.mainMenuStrip.Location = new System.Drawing.Point(0, 0);
      this.mainMenuStrip.Name = "mainMenuStrip";
      this.mainMenuStrip.Size = new System.Drawing.Size(1404, 24);
      this.mainMenuStrip.TabIndex = 19;
      this.mainMenuStrip.Text = "menuStrip1";
      // 
      // fileToolStripMenuItem
      // 
      fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openDataDirToolStripMenuItem,
            toolStripSeparator1,
            this.exitToolStripMenuItem});
      fileToolStripMenuItem.Name = "fileToolStripMenuItem";
      fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
      fileToolStripMenuItem.Text = "&File";
      // 
      // toolStripSeparator1
      // 
      toolStripSeparator1.Name = "toolStripSeparator1";
      toolStripSeparator1.Size = new System.Drawing.Size(232, 6);
      // 
      // exitToolStripMenuItem
      // 
      this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
      this.exitToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.F4)));
      this.exitToolStripMenuItem.Size = new System.Drawing.Size(235, 22);
      this.exitToolStripMenuItem.Text = "E&xit";
      this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
      // 
      // openDataDirToolStripMenuItem
      // 
      this.openDataDirToolStripMenuItem.Name = "openDataDirToolStripMenuItem";
      this.openDataDirToolStripMenuItem.Size = new System.Drawing.Size(235, 22);
      this.openDataDirToolStripMenuItem.Text = "Show &EPS-directory in Explorer";
      this.openDataDirToolStripMenuItem.Click += new System.EventHandler(this.openDataDirToolStripMenuItem_Click);
      // 
      // simulationToolStripMenuItem
      // 
      simulationToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.startToolStripMenuItem,
            this.stopToolStripMenuItem,
            toolStripSeparator2,
            this.pauseToolStripMenuItem});
      simulationToolStripMenuItem.Name = "simulationToolStripMenuItem";
      simulationToolStripMenuItem.Size = new System.Drawing.Size(76, 20);
      simulationToolStripMenuItem.Text = "&Simulation";
      // 
      // startToolStripMenuItem
      // 
      this.startToolStripMenuItem.Name = "startToolStripMenuItem";
      this.startToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.G)));
      this.startToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
      this.startToolStripMenuItem.Text = "&Start";
      this.startToolStripMenuItem.Click += new System.EventHandler(this.startToolStripMenuItem_Click);
      // 
      // stopToolStripMenuItem
      // 
      this.stopToolStripMenuItem.Enabled = false;
      this.stopToolStripMenuItem.Name = "stopToolStripMenuItem";
      this.stopToolStripMenuItem.ShortcutKeyDisplayString = "ESC";
      this.stopToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
      this.stopToolStripMenuItem.Text = "&Stop";
      this.stopToolStripMenuItem.Click += new System.EventHandler(this.stopToolStripMenuItem_Click);
      // 
      // pauseToolStripMenuItem
      // 
      this.pauseToolStripMenuItem.Enabled = false;
      this.pauseToolStripMenuItem.Name = "pauseToolStripMenuItem";
      this.pauseToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
      this.pauseToolStripMenuItem.Text = "&Pause";
      this.pauseToolStripMenuItem.Click += new System.EventHandler(this.pauseToolStripMenuItem_Click);
      // 
      // viewToolStripMenuItem
      // 
      viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.controlPanelToolStripMenuItem,
            this.statusBarToolStripMenuItem});
      viewToolStripMenuItem.Name = "viewToolStripMenuItem";
      viewToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
      viewToolStripMenuItem.Text = "&View";
      // 
      // controlPanelToolStripMenuItem
      // 
      this.controlPanelToolStripMenuItem.CheckOnClick = true;
      this.controlPanelToolStripMenuItem.Name = "controlPanelToolStripMenuItem";
      this.controlPanelToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
      this.controlPanelToolStripMenuItem.Text = "Control panel";
      this.controlPanelToolStripMenuItem.Click += new System.EventHandler(this.controlPanelToolStripMenuItem_Click);
      // 
      // statusBarToolStripMenuItem
      // 
      this.statusBarToolStripMenuItem.Checked = true;
      this.statusBarToolStripMenuItem.CheckOnClick = true;
      this.statusBarToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
      this.statusBarToolStripMenuItem.Name = "statusBarToolStripMenuItem";
      this.statusBarToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
      this.statusBarToolStripMenuItem.Text = "Status bar";
      this.statusBarToolStripMenuItem.Click += new System.EventHandler(this.statusBarToolStripMenuItem_Click);
      // 
      // DisplayPictureBox
      // 
      this.DisplayPictureBox.BackColor = System.Drawing.Color.Black;
      this.DisplayPictureBox.ContextMenuStrip = this.screenContextMenuStrip;
      this.DisplayPictureBox.Dock = System.Windows.Forms.DockStyle.Fill;
      this.DisplayPictureBox.Location = new System.Drawing.Point(3, 3);
      this.DisplayPictureBox.Name = "DisplayPictureBox";
      this.DisplayPictureBox.Size = new System.Drawing.Size(1193, 622);
      this.DisplayPictureBox.TabIndex = 0;
      this.DisplayPictureBox.TabStop = false;
      this.DisplayPictureBox.SizeChanged += new System.EventHandler(this.DisplayPictureBox_SizeChanged);
      this.DisplayPictureBox.Paint += new System.Windows.Forms.PaintEventHandler(this.DisplayPictureBox_Paint);
      this.DisplayPictureBox.MouseDown += new System.Windows.Forms.MouseEventHandler(this.DisplayPictureBox_MouseDown);
      this.DisplayPictureBox.MouseMove += new System.Windows.Forms.MouseEventHandler(this.DisplayPictureBox_MouseMove);
      this.DisplayPictureBox.MouseUp += new System.Windows.Forms.MouseEventHandler(this.DisplayPictureBox_MouseUp);
      // 
      // toolStripSeparator2
      // 
      toolStripSeparator2.Name = "toolStripSeparator2";
      toolStripSeparator2.Size = new System.Drawing.Size(149, 6);
      // 
      // ZigZagForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(1404, 702);
      this.Controls.Add(this.splitContainer1);
      this.Controls.Add(this.mainStatusStrip);
      this.Controls.Add(this.mainMenuStrip);
      this.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.KeyPreview = true;
      this.Name = "ZigZagForm";
      this.Text = "ZigZag.NET";
      this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
      this.tabControl1.ResumeLayout(false);
      this.tabPage1.ResumeLayout(false);
      this.screenContextMenuStrip.ResumeLayout(false);
      this.tabPage2.ResumeLayout(false);
      this.tabPage2.PerformLayout();
      this.groupBox1.ResumeLayout(false);
      this.groupBox1.PerformLayout();
      this.mainStatusStrip.ResumeLayout(false);
      this.mainStatusStrip.PerformLayout();
      this.splitContainer1.Panel1.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
      this.splitContainer1.ResumeLayout(false);
      this.controlPanel.ResumeLayout(false);
      this.controlPanel.PerformLayout();
      this.mainMenuStrip.ResumeLayout(false);
      this.mainMenuStrip.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this.DisplayPictureBox)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private BitmapPictureBox DisplayPictureBox;
    private System.Windows.Forms.Button StartButton;
    private System.Windows.Forms.Button TestButton;
    private System.Windows.Forms.TextBox ConsoleTextBox;
    private System.Windows.Forms.TabControl tabControl1;
    private System.Windows.Forms.TabPage tabPage1;
    private System.Windows.Forms.TabPage tabPage2;
    private System.Windows.Forms.CheckBox checkBoxK;
    private System.Windows.Forms.CheckBox checkBoxF;
    private System.Windows.Forms.CheckBox checkBoxPaused;
    private System.Windows.Forms.Button buttonUp;
    private System.Windows.Forms.Button buttonLeft;
    private System.Windows.Forms.Button buttonRight;
    private System.Windows.Forms.Button buttonDown;
    private System.Windows.Forms.Button buttonZoomOut;
    private System.Windows.Forms.Button buttonZoomIn;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.Label updatesLabel;
    private System.Windows.Forms.Label activePageLabel;
    private System.Windows.Forms.StatusStrip mainStatusStrip;
    private System.Windows.Forms.SplitContainer splitContainer1;
    private System.Windows.Forms.Panel controlPanel;
    private System.Windows.Forms.ToolStripStatusLabel mainStatusLabel;
    private System.Windows.Forms.ContextMenuStrip screenContextMenuStrip;
    private System.Windows.Forms.ToolStripMenuItem zoomHereMenuItem;
    private System.Windows.Forms.ToolStripMenuItem zoomOutHereMenuItem;
    private System.Windows.Forms.ToolStripMenuItem centerHereMenuItem;
    private System.Windows.Forms.MenuStrip mainMenuStrip;
    private System.Windows.Forms.ToolStripMenuItem openDataDirToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem startToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem stopToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem pauseToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem controlPanelToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem statusBarToolStripMenuItem;
  }
}

