using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ZigZag
{
  public partial class ImageDisplayControl : UserControl
  {
    public EventHandler Closing;

    public ImageDisplayControl()
    {
      InitializeComponent();
    }

    public void OnClosing(EventArgs e)
    {
      if (this.Closing != null)
      {
        this.Closing(this, e);
      }
    }

    public void LoadImageFromFile(string filename)
    {
      using (Image i = Image.FromFile(filename))
      {
        this.imagePictureBox.Image = new Bitmap(i);
      }
    }

    private void closeButton_Click(object sender, EventArgs e)
    {
      this.OnClosing(EventArgs.Empty);
    }
  }
}
