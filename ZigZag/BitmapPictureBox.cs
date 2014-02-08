using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ZigZag
{
  class BitmapPictureBox : PictureBox
  {
    public BitmapPictureBox()
    {
      // Prevent this picture box from painting its background
      // this.SetStyle(ControlStyles.Opaque, true);
    }
  }
}
