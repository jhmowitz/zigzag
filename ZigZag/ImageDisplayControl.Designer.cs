namespace ZigZag
{
  partial class ImageDisplayControl
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

    #region Component Designer generated code

    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ImageDisplayControl));
      this.imageToolstrip = new System.Windows.Forms.ToolStrip();
      this.panel1 = new System.Windows.Forms.Panel();
      this.imagePictureBox = new System.Windows.Forms.PictureBox();
      this.closeButton = new System.Windows.Forms.ToolStripButton();
      this.imageToolstrip.SuspendLayout();
      this.panel1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.imagePictureBox)).BeginInit();
      this.SuspendLayout();
      // 
      // imageToolstrip
      // 
      this.imageToolstrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.closeButton});
      this.imageToolstrip.Location = new System.Drawing.Point(0, 0);
      this.imageToolstrip.Name = "imageToolstrip";
      this.imageToolstrip.Size = new System.Drawing.Size(582, 25);
      this.imageToolstrip.TabIndex = 0;
      this.imageToolstrip.Text = "toolStrip1";
      // 
      // panel1
      // 
      this.panel1.AutoScroll = true;
      this.panel1.Controls.Add(this.imagePictureBox);
      this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel1.Location = new System.Drawing.Point(0, 25);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(582, 506);
      this.panel1.TabIndex = 1;
      // 
      // imagePictureBox
      // 
      this.imagePictureBox.Location = new System.Drawing.Point(0, 0);
      this.imagePictureBox.Name = "imagePictureBox";
      this.imagePictureBox.Size = new System.Drawing.Size(100, 50);
      this.imagePictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
      this.imagePictureBox.TabIndex = 0;
      this.imagePictureBox.TabStop = false;
      // 
      // closeButton
      // 
      this.closeButton.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
      this.closeButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
      this.closeButton.Image = ((System.Drawing.Image)(resources.GetObject("closeButton.Image")));
      this.closeButton.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.closeButton.Name = "closeButton";
      this.closeButton.Size = new System.Drawing.Size(40, 22);
      this.closeButton.Text = "Close";
      this.closeButton.Click += new System.EventHandler(this.closeButton_Click);
      // 
      // ImageDisplayControl
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add(this.panel1);
      this.Controls.Add(this.imageToolstrip);
      this.Name = "ImageDisplayControl";
      this.Size = new System.Drawing.Size(582, 531);
      this.imageToolstrip.ResumeLayout(false);
      this.imageToolstrip.PerformLayout();
      this.panel1.ResumeLayout(false);
      this.panel1.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this.imagePictureBox)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.ToolStrip imageToolstrip;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.PictureBox imagePictureBox;
    private System.Windows.Forms.ToolStripButton closeButton;
  }
}
