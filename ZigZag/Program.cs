using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace ZigZag
{
  static class Program
  {
    [System.Runtime.InteropServices.DllImport("user32.dll")]
    private static extern bool SetProcessDPIAware();

    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main()
    {
      try
      {
        if (Environment.OSVersion.Version.Major >= 6)
          SetProcessDPIAware();

        Application.EnableVisualStyles();
        Application.SetCompatibleTextRenderingDefault(false);
        Application.Run(new ZigZagForm());
      }
      catch (Exception ex)
      {
        MessageBox.Show(ex.Message + "\r\n"+ ex.StackTrace, "ZigZag.NET Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
    }
  }
}
