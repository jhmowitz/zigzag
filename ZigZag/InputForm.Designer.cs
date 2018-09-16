namespace ZigZag
{
  partial class InputForm
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
      this.promptLabel = new System.Windows.Forms.Label();
      this.inputTextBox = new System.Windows.Forms.TextBox();
      this.okButton = new System.Windows.Forms.Button();
      this.button1 = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // promptLabel
      // 
      this.promptLabel.AutoSize = true;
      this.promptLabel.Location = new System.Drawing.Point(20, 19);
      this.promptLabel.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
      this.promptLabel.Name = "promptLabel";
      this.promptLabel.Size = new System.Drawing.Size(69, 20);
      this.promptLabel.TabIndex = 0;
      this.promptLabel.Text = "(prompt)";
      // 
      // inputTextBox
      // 
      this.inputTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.inputTextBox.Location = new System.Drawing.Point(25, 44);
      this.inputTextBox.Margin = new System.Windows.Forms.Padding(5);
      this.inputTextBox.Name = "inputTextBox";
      this.inputTextBox.Size = new System.Drawing.Size(761, 26);
      this.inputTextBox.TabIndex = 1;
      this.inputTextBox.TextChanged += new System.EventHandler(this.inputTextBox_TextChanged);
      // 
      // okButton
      // 
      this.okButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.okButton.Enabled = false;
      this.okButton.Location = new System.Drawing.Point(536, 100);
      this.okButton.Margin = new System.Windows.Forms.Padding(5);
      this.okButton.Name = "okButton";
      this.okButton.Size = new System.Drawing.Size(120, 34);
      this.okButton.TabIndex = 2;
      this.okButton.Text = "OK";
      this.okButton.UseVisualStyleBackColor = true;
      this.okButton.Click += new System.EventHandler(this.okButton_Click);
      // 
      // button1
      // 
      this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button1.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.button1.Location = new System.Drawing.Point(666, 100);
      this.button1.Margin = new System.Windows.Forms.Padding(5);
      this.button1.Name = "button1";
      this.button1.Size = new System.Drawing.Size(120, 34);
      this.button1.TabIndex = 3;
      this.button1.Text = "Cancel";
      this.button1.UseVisualStyleBackColor = true;
      // 
      // InputForm
      // 
      this.AcceptButton = this.okButton;
      this.AutoScaleDimensions = new System.Drawing.SizeF(144F, 144F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
      this.CancelButton = this.button1;
      this.ClientSize = new System.Drawing.Size(808, 148);
      this.ControlBox = false;
      this.Controls.Add(this.button1);
      this.Controls.Add(this.okButton);
      this.Controls.Add(this.inputTextBox);
      this.Controls.Add(this.promptLabel);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.Margin = new System.Windows.Forms.Padding(5);
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "InputForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Enter input";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Label promptLabel;
    private System.Windows.Forms.TextBox inputTextBox;
    private System.Windows.Forms.Button okButton;
    private System.Windows.Forms.Button button1;
  }
}