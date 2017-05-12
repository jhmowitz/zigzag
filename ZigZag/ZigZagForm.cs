using System;
using System.IO;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;
using System.Diagnostics;
using System.Collections.Generic;

namespace ZigZag
{
  /*private struct EngineState
  {
    bool paused;
    bool K;
  }*/

  /// <summary>
  /// The main form of the application.
  /// Implement IZigZagHost, so it gets called with methods like lineto(), moveto() etc
  /// by the ZigZagEngine it hosts.
  /// </summary>
  public partial class ZigZagForm : Form, IZigZagHost
  {
    #region "Member variables"
    /// <summary>
    /// The engine we're hosting
    /// </summary>
    protected ZigZagEngine _engine = null;

    protected int _updateCount = 0;
    protected bool _closeRequested = false;
    protected bool _startOnNextIdle = false;
    protected Point _mousePosition;
    protected Point _mouseDownPosition;
    protected bool _mouseDown = false;
    #endregion

    #region "Graphics state"
    /*
     * The state variables for the graphics system, to support IZigZagHost
     */

    /// <summary>
    /// The number of pages in the graphics system. Should be 2 to allow for double buffering.
    /// Less disables double buffering, more is unnecessary
    /// </summary>
    protected const short NumberOfPages = 2;

    /// <summary>
    /// The active page, i.e. the one graphics operations are performed on. 0+
    /// </summary>
    protected int _currentActivePage = 0;
    /// <summary>
    /// The currently displayed page. May differ from currentActivePage. 0+
    /// </summary>
    protected int _currentVisualPage = 0;

    /// <summary>
    /// The current color
    /// </summary>
    protected Color _currentColor = Color.White;

    /// <summary>
    /// Current position
    /// </summary>
    protected int _currentX = 0;
    protected int _currentY = 0;

    /// <summary>
    /// Current font
    /// </summary>
    protected Font _currentFont = new Font("Lucida Console", 10);

    /// <summary>
    /// The current screen size. Now determined at program startup, but should be some fixed size. TODO
    /// </summary>
    protected Size _screenSize;

    /// <summary>
    /// The bitmaps used to maintain the pages
    /// </summary>
    protected Bitmap[] _screenbitmaps = null;

    protected string _dataPath;
    #endregion

    #region "Keyboard state"
    /*
     * State for the keyboard buffer
     */

    /// <summary>
    /// A queue of keypresses.
    /// </summary>
    protected Queue<short> _keyboardBuffer = new Queue<short>();
    #endregion

    /// <summary>
    /// Constructor of this form
    /// </summary>
    public ZigZagForm()
    {
      this.InitializeComponent();

      this.MouseWheel += DisplayPictureBox_MouseWheel;
      this.mainStatusLabel.Text = "ZigZag.NET v" + new Version(Application.ProductVersion).ToString(3) + " ready.";
      Application.Idle += Application_Idle;

      // Set up a data path
      string dataFolder = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "ZigZag");
      Directory.CreateDirectory(dataFolder);
      Directory.CreateDirectory(Path.Combine(dataFolder, "eps"));

      this._dataPath = dataFolder;
    }
// arno: bij het debuggen met F11 kom ik telkens hier terug, zonder dat ik het inputscherm te zien krijg...
    void Application_Idle(object sender, EventArgs e)
    {
      if (this._startOnNextIdle)
      {
        this._startOnNextIdle = false;

        // Set up the ZigZag engine here, passing ourselves as the ZigZag host.
        // This means the methods in our IZigZagHost implementation (e.g. moveto, lineto)
        // get called from ZigZagEngine.

        this.StartButton.Text = "Stop";
        this.startToolStripMenuItem.Enabled = false;
        this.stopToolStripMenuItem.Enabled = true;
        this.pauseToolStripMenuItem.Enabled = true;
        this.mainStatusLabel.Text = "Running";

        // Clear the keyboard buffer
        this._keyboardBuffer.Clear();

        this._engine = new ZigZagEngine(this, Environment.GetCommandLineArgs());

        // This in fact starts a loop that calls kbhit() and/or getch()
        // until the engine is stopped using ESC
        this.LogMessage("--- Started ---");
        this._engine.Run();
        this.LogMessage("\r\n--- Stopped ---\r\n\r\n");

        // Clear the keyboard buffer again to make sure no spurious ESC characters remain
        this._keyboardBuffer.Clear();

        this.StartButton.Text = "Start";
        this.startToolStripMenuItem.Enabled = true;
        this.stopToolStripMenuItem.Enabled = false;
        this.pauseToolStripMenuItem.Enabled = false;
        this._engine = null;

        this.mainStatusLabel.Text = "Stopped.";

        if (this._closeRequested)
          this.Close();
        else
          MessageBox.Show(this, "Done.", "ZigZag.NET", MessageBoxButtons.OK, MessageBoxIcon.Information);
      }
    }

    /// <summary>
    /// First-time initialisation
    /// </summary>
    protected override void OnShown(EventArgs e)
    {
      base.OnShown(e);
      // this._screenSize = this.DisplayPictureBox.Size;
      // StartButton.PerformClick();
    }

    protected void QueueKey(short key)
    {
      this._keyboardBuffer.Enqueue(key);
    }

    /// <summary>
    /// Prevent closing when the engine is running. Instead, set the closeRequested flag and enqueue a few ESC characters
    /// </summary>
    /// <param name="e"></param>
    protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
    {
      if (this._engine != null)
      {
        if (MessageBox.Show(this, "Simulation is running. Stop it and exit?", "ZigZag.NET", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
        {
          this.mainStatusLabel.Text = "Stopping simulation...";
          this._closeRequested = true;
          for (int i = 0; i < 3; i++)
            this.QueueKey(27);
        }
        e.Cancel = true;
        return;
      }
      base.OnClosing(e);
    }

    /// <summary>
    /// Shortcut to ourselves, cast as IZigZagHost.
    /// Enables short acces to the IZigZagHost interface
    /// </summary>
    protected IZigZagHost Host
    {
      get
      {
        return (IZigZagHost)this;
      }
    }

    protected void InitBitmaps()
    {
      Bitmap[] oldBitmaps = this._screenbitmaps;

      // Declare a screen bitmap for each page. Use the existing bitmap (if any) as an original
      this._screenbitmaps = new Bitmap[NumberOfPages];
      for (int page = 0; page < NumberOfPages; page++)
      {
        if (oldBitmaps != null)
        {
          this._screenbitmaps[page] = new Bitmap(oldBitmaps[page], this._screenSize);
          oldBitmaps[page].Dispose();
        }
        else
        {
          this._screenbitmaps[page] = new Bitmap(this._screenSize.Width, this._screenSize.Height, PixelFormat.Format32bppArgb);
        }
      }
    }

    /// <summary>
    /// Invalidate the current page, i.e. mark it as changed
    /// so it will be redrawn at the next opportunity
    /// </summary>
    protected void InvalidatePage()
    {
      // If we're not on the active page, do nothing - we're drawing off-screen!
      // If we ARE the current page, invalidate our display control
      if (this._currentActivePage == this._currentVisualPage)
      {
        this.DisplayPictureBox.Invalidate();
      }
      this.UpdateStatus();
    }

    protected void UpdateStatus()
    {
      this.activePageLabel.Text = this._currentActivePage.ToString();
    }

    /// <summary>
    /// Draw a page to a Graphics (most likely the display picture box)
    /// </summary>
    protected void DrawPage(Graphics g)
    {
      g.DrawImageUnscaled(this._screenbitmaps[this._currentVisualPage], 0, 0);
    }

    protected void LogMessage(string message)
    {
      string stamp = DateTime.Now.ToString("yyyy'-'MM'-'dd HH':'mm':'ss") + " - ";
      if (message.StartsWith("\r\n"))
      {
        message = "\r\n" + stamp + message.Substring(2);
      }
      else
      {
        message = stamp + message;
      }
      this.ConsoleTextBox.Text += message;
    }

    #region "ZigZagHost implementation"

    /*
     * This class supplies the methods that the ZigZagEngine needs
     * to operate in a graphical environment by implementing the IZigZagHost interface.
     *
     * IZigZagHost is defined in ZigZagEngine.
     */

    void IZigZagHost.moveto(short x, short y)
    {
      // Debug.WriteLine(String.Format("moveto: {0} {1}", x, y));
      this._currentX = x;
      this._currentY = y;
    }

    void IZigZagHost.lineto(short x, short y)
    {
      // Debug.WriteLine(String.Format("lineto: {0} {1}", x, y));
      // Draw line from (_currentX, _currentY) to (x, y)
      using (Graphics g = Graphics.FromImage(this._screenbitmaps[this._currentActivePage]))
      {
        g.DrawLine(new Pen(this._currentColor), this._currentX, this._currentY, x, y);
      }
      this._currentX = x;
      this._currentY = y;
      this.InvalidatePage();
    }

    /// <summary>
    /// Get the maximum X coordinate of the graphics system
    /// </summary>
    short IZigZagHost.getmaxx()
    {
      return (short)(this._screenSize.Width - 1);
    }

    /// <summary>
    /// Get the maximum Y coordinate of the graphics system
    /// </summary>
    short IZigZagHost.getmaxy()
    {
      return (short)(this._screenSize.Height - 1);
    }

    /// <summary>
    /// Display text at the specified position, which is the top left of the text's bounding box
    /// </summary>
    void IZigZagHost.outtextxy(short x, short y, string text)
    {
      using (Graphics g = Graphics.FromImage(this._screenbitmaps[this._currentActivePage]))
      {
        g.DrawString(text, this._currentFont, new SolidBrush(this._currentColor), x, y);
      }
      this.InvalidatePage();
    }

    /// <summary>
    /// Clear the active page. TODO: background color? All pages?
    /// </summary>
    void IZigZagHost.cleardevice()
    {
      using (Graphics g = Graphics.FromImage(this._screenbitmaps[this._currentActivePage]))
      {
        g.Clear(Color.Black);
      }
      this.InvalidatePage();
    }

    /// <summary>
    /// Draw a single pixel to the active page
    /// </summary>
    void IZigZagHost.putpixel(short x, short y, Color color)
    {
      // Draw a single pixel in the supplied color
      if (x < this._screenSize.Width && y < this._screenSize.Height)
      {
        this._screenbitmaps[this._currentActivePage].SetPixel(x, y, color);
        this.InvalidatePage();
      }
    }

    void IZigZagHost.setcolor(Color color)
    {
      this._currentColor = color;
    }

    void IZigZagHost.setactivepage(short page)
    {
      // Debug.WriteLine(String.Format("setactivepage: {0}", page));
      this._currentActivePage = page;
      this.InvalidatePage();
    }

    void IZigZagHost.setvisualpage(short page)
    {
      // Debug.WriteLine(String.Format("setvisualpage: {0}", page));
      this._currentVisualPage = page;
      this.InvalidatePage();

      this._updateCount++;
      this.updatesLabel.Text = this._updateCount.ToString();
    }

    short IZigZagHost.textwidth(String text)
    {
      // Measure text and return width
      using (Graphics g = Graphics.FromImage(this._screenbitmaps[this._currentActivePage]))
      {
        return (short)g.MeasureString(text, this._currentFont, this._screenSize.Width).Width;
      }
    }

    short IZigZagHost.textheight(String text)
    {
      // Measure text and return height
      using (Graphics g = Graphics.FromImage(this._screenbitmaps[this._currentActivePage]))
      {
        return (short)g.MeasureString(text, this._currentFont, this._screenSize.Width).Height;
      }
    }

    void IZigZagHost.initgraph(ref short driver, ref short mode, string directory)
    {
      driver = 1; // VGA
      mode = 2;   // VGAMED
      Debug.WriteLine(String.Format("initgraph: {0} --> Driver: {1}, Mode: {2}", directory, driver, mode));

      this._currentActivePage = 0;
      this._currentVisualPage = 0;
      this._currentColor = Color.White;

      InitBitmaps();
    }

    void IZigZagHost.closegraph()
    {
      // Nothing
    }

    Image IZigZagHost.getimage(short left, short top, short right, short bottom)
    {
      // Create a bitmap with the correct size
      Bitmap b = new Bitmap(right - left + 1, bottom - top + 1, PixelFormat.Format32bppArgb);
      // Draw the desired part of the source image onto it
      using (Graphics gNew = Graphics.FromImage(b))
      {
        gNew.DrawImage(this._screenbitmaps[this._currentActivePage], 0, 0, new Rectangle(left, top, b.Width, b.Height), GraphicsUnit.Pixel);
      }
      // Return the new bitmap
      return b;
    }

    void IZigZagHost.putimage(short left, short top, Image image, short op)
    {
      using (Graphics g = Graphics.FromImage(this._screenbitmaps[this._currentActivePage]))
      {
        g.DrawImage(image, left, top);
      }
      this.InvalidatePage();
    }

    uint IZigZagHost.imagesize(short u, short d, short l, short r)
    {
      // This function is handled in the engine itself (returning sizeof(Image^)). 
      // This method should never be called!
      throw new NotImplementedException();
    }

    void IZigZagHost.settextstyle(short font, short direction, short charsize)
    {
      // TODO: set font and direction
      // font: DEFAULT_FONT
      // direction: HORIZ_DIR
      // charsize: fixed size or custom (0) controlled by setusercharsize
      Debug.WriteLine(String.Format("settextstyle: {0} {1} {2}", font, direction, charsize));
    }

    void IZigZagHost.setusercharsize(short multx, short divx, short multy, short divy)
    {
      // TODO: set font size
      // mulx/divx and muly/divx control character size
      Debug.WriteLine(String.Format("setusercharsize: {0} {1} {2} {3}", multx, divx, multy, divy));
    }

    void IZigZagHost.setviewport(short x1, short y1, short x2, short y2, short clip)
    {
      // The range between (x1, y1) and (x2, y2) is the new viewport, until reset.
      // All output is then relative to the viewport, e.g. outtextxy(0, 0, ...) will draw at 'real' position (x1, y1)
      // Current position is reset to (0, 0) in the new window
      // If clip == CLIP_ON (1), output is clipped to the viewport
      Debug.WriteLine(String.Format("setviewport: ({0}, {1})-({2}, {3}) {4}", x1, y1, x2, y2, clip));
      if (x1 != 0 || y1 != 0 || x2 != this._screenSize.Width - 1 || y2 != this._screenSize.Height - 1)
      {
        Debug.Assert(false, "Partial viewport not supported (only whole screen)");
      }
    }

    void IZigZagHost.getaspectratio(ref short x, ref short y)
    {
      // TODO: implement real aspect ratio. Assume 1/1 for now
      x = 10000;
      y = 10000;
      Debug.WriteLine(String.Format("getaspectratio --> {0} x {1}", x, y));
    }

    void IZigZagHost.circle(double x, double y, double r)
    {
      using (Graphics g = Graphics.FromImage(this._screenbitmaps[this._currentActivePage]))
      {
        g.DrawEllipse(new Pen(this._currentColor), (int)(x - r), (int)(y - r), (int)(2 * r), (int)(2 * r));
      }
      this.InvalidatePage();
    }

    void IZigZagHost.line(short x1, short y1, short x2, short y2)
    {
      using (Graphics g = Graphics.FromImage(this._screenbitmaps[this._currentActivePage]))
      {
        g.DrawLine(new Pen(this._currentColor), x1, y1, x2, y2);
      }
      this.InvalidatePage();
    }

    void IZigZagHost.delay(ushort milliseconds)
    {
      System.Threading.Thread.Sleep(milliseconds);
    }

    int IZigZagHost.biostime(short cmd, int newtime)
    {
      // cmd = 0: get time (18.2 ticks per second since midnight)
      // cmd = 1: set time to newtime (idem)
      return (int)(DateTime.Now.TimeOfDay.TotalSeconds * 18.2);
    }

    short IZigZagHost.kbhit()
    {
      // return 1 if key available, 0 otherwise

      Application.DoEvents();

      // Return 1 if the keyboard buffer contains keystrokes
      if (this._keyboardBuffer.Count > 0)
        return 1;
      else
        return 0;
    }

    short IZigZagHost.getch()
    {
      // Return the character code of the key pressed. Blocking!
      for (; ; )
      {
        if (this._keyboardBuffer.Count > 0)
        {
          short keystroke = this._keyboardBuffer.Dequeue();
          //if (e.KeyChar == '(')
          //  return -53; // Cadre
          return keystroke;
        }
        Application.DoEvents();
      }
    }

    void IZigZagHost.ShowMessage(string message)
    {
      this.LogMessage(message.Replace("\n", "\r\n").Replace("\n\n", "\n"));
    }

    void IZigZagHost.NotifyEvent(string eventName, string message)
    {
      switch (eventName)
      {
        case "eps":
          {
            string epsFilename = Host.MakeDataFileName(message);
            string pngFilename = Path.ChangeExtension(epsFilename, ".png");
            Ghostscript.Converter.Convert(epsFilename, pngFilename, Ghostscript.Converter.GhostScriptDeviceEnum.png16m, 1);
            Host.ShowMessage(string.Format("\n{0} converted to {1}", epsFilename, pngFilename));
            TabPage tabPage = new TabPage(Path.GetFileNameWithoutExtension(pngFilename));
            ImageDisplayControl p = new ImageDisplayControl();
            p.Closing += this.Image_Closing;
            p.LoadImageFromFile(pngFilename);
            p.Dock = DockStyle.Fill;
            tabPage.Controls.Add(p);
            this.tabControl1.TabPages.Add(tabPage);
            this.tabControl1.SelectedTab = tabPage;
          }
          break;

        default:
          this.ConsoleTextBox.Text += "Unsupported event: " + eventName + "\r\n";
          break;
      }
    }

    private void Image_Closing(object sender, EventArgs e)
    {
      ImageDisplayControl idc = (ImageDisplayControl)sender;
      idc.Closing -= Image_Closing;

      TabPage tp = (TabPage)idc.Parent;
      this.tabControl1.TabPages.Remove(tp);
    }

    string IZigZagHost.ReadString(string prompt, short maxlength)
    {
      using (InputForm f = new InputForm(prompt, maxlength))
      {
        f.ShowDialog(this);
        string s = f.Input;
        if (s.Length > maxlength)
          s = s.Substring(maxlength);
        return s;
      }
    }

    short IZigZagHost.ReadInt(string prompt)
    {
      using (InputForm f = new InputForm(prompt))
      {
        f.ShowDialog(this);
        return short.Parse(f.Input);
      }
    }
    double IZigZagHost.ReadReal(string prompt)
    {
      using (InputForm f = new InputForm(prompt))
      {
        f.ShowDialog(this);
        if (string.IsNullOrEmpty(f.Input))
          return 0;
        return double.Parse(f.Input);
      }
    }
    string IZigZagHost.MakeDataFileName(string name)
    {
      return Path.Combine(this._dataPath, name);
    }
        #endregion

        #region "Keyboard handling"
        //protected override void OnKeyDown(KeyEventArgs e)
        //{
        //  base.OnKeyDown(e);

        //  if (!e.Handled)
        //  {
        //    e.SuppressKeyPress = true;
        //    this.QueueKey(e);
        //  }
        //}

        protected override bool ProcessDialogKey(Keys keyData)
    {
      // Debug.WriteLine("{0}: {1}", Control.ModifierKeys.ToString(), keyData.ToString());

      // Translate keyData to DOS-compatbile keystrokes here

      // No Alt, Control or Shift
      if (!keyData.HasFlag(Keys.Alt) && !keyData.HasFlag(Keys.Control) && !keyData.HasFlag(Keys.Shift))
      {
        // F1 to F10
        if (keyData >= Keys.F1 && keyData <= Keys.F10)
        {
          this.QueueKey((short)(-59 - (short)(keyData - Keys.F1)));
          return true;
        }
        if (keyData >= Keys.F11 && keyData <= Keys.F12)
        {
          this.QueueKey((short)(-133 - (short)(keyData - Keys.F11)));
          return true;
        }
        else
        {
          // Arrow keys
          switch (keyData)
          {
            case Keys.Left:
              this.QueueKey((short)('X'));
              return true;
            case Keys.Right:
              this.QueueKey((short)('x'));
              return true;
            case Keys.Up:
              this.QueueKey((short)('y'));
              return true;
            case Keys.Down:
              this.QueueKey((short)('Y'));
              return true;
          }
        }
      }
      else if (!keyData.HasFlag(Keys.Alt) && !keyData.HasFlag(Keys.Control) && keyData.HasFlag(Keys.Shift))
      {
        // Shift only
        Keys key = ((Keys)((int)keyData ^ (int)Keys.Shift));
        if (key >= Keys.F1 && key <= Keys.F10)
        {
          this.QueueKey((short)(-84 - (short)(key - Keys.F1)));
          return true;
        }
      }
      //else if (!keyData.HasFlag(Keys.Alt) && keyData.HasFlag(Keys.Control) && !keyData.HasFlag(Keys.Shift))
      //{
      //  // Control only
      //  Keys key = ((Keys)((int)keyData ^ (int)Keys.Control));
      //  if (key >= Keys.F1 && key <= Keys.F9)
      //  {
      //    this.QueueKey((short)(-104 - (short)(key - Keys.F1)));
      //    return true;
      //  }
      //}
      else if (keyData.HasFlag(Keys.Alt) && !keyData.HasFlag(Keys.Control) && !keyData.HasFlag(Keys.Shift))
      {
        // Alt only
        Keys key = ((Keys)((int)keyData ^ (int)Keys.Alt));
        if (key >= Keys.F1 && key <= Keys.F10)
        {
          this.QueueKey((short)(-104 - (short)(key - Keys.F1)));
          return true;
        }
      }

      bool r = base.ProcessDialogKey(keyData);
      return r;
    }

    /// <summary>
    /// Process keystrokes with Alt only
    /// </summary>
    protected override bool ProcessDialogChar(char charCode)
    {
      short[] atozalt = { 30, 48, 46, 32, 18, 33, 34, 35, 23, 37, 38, 38, 50, 49, 24, 25, 16, 19, 31, 20, 22, 47, 17, 45, 21, 44 };

      if (Control.ModifierKeys.HasFlag(Keys.Alt) && !Control.ModifierKeys.HasFlag(Keys.Shift) && !Control.ModifierKeys.HasFlag(Keys.Control))
      {
        short newKey = 0;
        switch (charCode)
        {
          case '/':
            newKey = 53;
            break;
          case '\\':
            newKey = 43;
            break;
          default:
            if (charCode >= 'a' && charCode <= 'z') {
              newKey = atozalt[Convert.ToInt16(charCode) - Convert.ToInt16('a')];
            }
            break;
        }

        if (newKey != 0)
        {
          this.QueueKey((short)-newKey);
          return true;
        }
      }
      return base.ProcessDialogChar(charCode);
    }

    protected override void OnKeyPress(KeyPressEventArgs e)
    {
      base.OnKeyPress(e);
      if (!e.Handled)
      {
        e.Handled = true;
        this.QueueKey((short)e.KeyChar);
      }
    }
    #endregion

    #region "UI event handlers"

    /// <summary>
    /// Handle clicking the Test button
    /// </summary>
    private void TestButton_Click(object sender, EventArgs e)
    {
      short driver = 0, mode = 0;

      Host.initgraph(ref driver, ref mode, "TEST-DIR");

      Host.setactivepage(0);
      Host.cleardevice();

      Host.setcolor(Color.Yellow);
      Host.moveto(100, 100);
      Host.lineto(200, 300);

      Host.setcolor(Color.Green);
      Host.moveto(0, 0);
      Host.lineto(Host.getmaxx(), Host.getmaxy());

      int w = Host.getmaxx() + 1;
      int h = Host.getmaxy() + 1;

      Host.setcolor(Color.Cyan);
      Host.circle(w / 2, h / 2, w / 4);

      Host.setcolor(Color.Blue);

      Host.moveto(0, 0);
      Host.lineto((short)(w / 2 - 1), 0);
      Host.lineto((short)(w / 2 - 1), (short)(h / 2 - 1));
      Host.lineto(0, (short)(h / 2 - 1));
      Host.lineto(0, 0);

      Image i = Host.getimage(0, 0, (short)(w / 2 - 1), (short)(h / 2 - 1));
      Host.putimage((short)(w / 2), (short)(h / 2), i, 1);

      Host.setcolor(Color.Red);
      Host.outtextxy(0, 0, "TEST");
      Host.setcolor(Color.Magenta);
      short tw = Host.textwidth("TEST");
      short th = Host.textheight("TEST");

      Host.moveto(0, 0);
      Host.lineto(tw, 0);
      Host.lineto(tw, th);
      Host.lineto(0, th);
      Host.lineto(0, 0);

      Host.closegraph();
    }

    /// <summary>
    /// Handle painting the picture box
    /// </summary>
    private void DisplayPictureBox_Paint(object sender, PaintEventArgs e)
    {
      if (this._screenbitmaps != null)
      {
        DrawPage(e.Graphics);
      }
    }

    /// <summary>
    /// Handle pressing the Start button
    /// </summary>
    private void StartButton_Click(object sender, EventArgs e)
    {
      StartStopEngine();
    }
    #endregion

    #region "Control panel events"
    private void checkBoxF_CheckedChanged(object sender, EventArgs e)
    {
      this.QueueKey((short)('F'));
    }

    private void checkBoxK_CheckedChanged(object sender, EventArgs e)
    {
      this.QueueKey((short)('K'));
    }

    private void buttonUp_Click(object sender, EventArgs e)
    {
      this.QueueKey((short)('y'));
    }

    private void buttonDown_Click(object sender, EventArgs e)
    {
      this.QueueKey((short)('Y'));
    }

    private void buttonLeft_Click(object sender, EventArgs e)
    {
      this.QueueKey((short)('X'));
    }

    private void buttonRight_Click(object sender, EventArgs e)
    {
      this.QueueKey((short)('x'));
    }

    private void buttonZoomOut_Click(object sender, EventArgs e)
    {
      this.QueueKey((short)('s'));
    }

    private void buttonZoomIn_Click(object sender, EventArgs e)
    {
      this.QueueKey((short)('S'));
    }

    private void checkBoxPaused_CheckedChanged(object sender, EventArgs e)
    {
      this.QueueKey((short)('!'));
    }
    #endregion

    protected void StartStopEngine()
    {
      if (this._engine == null)
      {
        this._startOnNextIdle = true;
      }
      else
      {
        // Stop the engine by sending ESC
        this.QueueKey(27);
        this.QueueKey(27);
      }
    }

    private void screenContextMenuStrip_Opening(object sender, System.ComponentModel.CancelEventArgs e)
    {
      this._mousePosition = Control.MousePosition;
    }

    private void zoomHereMenuItem_Click(object sender, EventArgs e)
    {
      Point clientPoint = this.DisplayPictureBox.PointToClient(this._mousePosition);
      this._engine.ZoomTo((short)clientPoint.X, (short)clientPoint.Y, 0.9);
    }

    private void centerHereMenuItem_Click(object sender, EventArgs e)
    {
      Point clientPoint = this.DisplayPictureBox.PointToClient(this._mousePosition);
      this._engine.ZoomTo((short)clientPoint.X, (short)clientPoint.Y, 1.0);
    }

    private void zoomOutHereMenuItem_Click(object sender, EventArgs e)
    {
      Point clientPoint = this.DisplayPictureBox.PointToClient(this._mousePosition);
      this._engine.ZoomTo((short)clientPoint.X, (short)clientPoint.Y, 1.1);
    }

    private void DisplayPictureBox_MouseDown(object sender, MouseEventArgs e)
    {
      if (this._engine != null)
      {
        this._mouseDown = true;
        this._mouseDownPosition = e.Location;
      }
    }

    private void DisplayPictureBox_MouseUp(object sender, MouseEventArgs e)
    {
      if (this._mouseDown)
      {
        this._mouseDown = false;
        this.Cursor = Cursors.Default;
      }
    }

    private void DisplayPictureBox_MouseMove(object sender, MouseEventArgs e)
    {
      if (this._mouseDown && this._engine != null)
      {
        this.Cursor = Cursors.Hand;
        int dx = e.Location.X - this._mouseDownPosition.X;
        int dy = e.Location.Y - this._mouseDownPosition.Y;
        this._engine.ZoomTo((short)((Host.getmaxx() + 1) / 2 - dx), (short)((Host.getmaxy() + 1) / 2 - dy), 1.0);
        this._mouseDownPosition = e.Location;
      }
    }

    void DisplayPictureBox_MouseWheel(object sender, MouseEventArgs e)
    {
      if (this._engine != null)
      {
        Debug.WriteLine(e.Delta);
        this._engine.ZoomTo((short)((Host.getmaxx() + 1) / 2), (short)((Host.getmaxy() + 1) / 2), 1 - ((double)e.Delta / 240) / 10);
      }
    }

    private void DisplayPictureBox_SizeChanged(object sender, EventArgs e)
    {
      this._screenSize = this.DisplayPictureBox.ClientSize;
      this.InitBitmaps();

      if (this._engine != null)
      {
        this._engine.InitCadre();
      }
    }

    private void openDataDirToolStripMenuItem_Click(object sender, EventArgs e)
    {
      Process.Start(Path.Combine(this._dataPath, "eps"));
    }

    private void exitToolStripMenuItem_Click(object sender, EventArgs e)
    {
      this.Close();
    }

    private void stopToolStripMenuItem_Click(object sender, EventArgs e)
    {
      this.StartStopEngine();
    }

    private void startToolStripMenuItem_Click(object sender, EventArgs e)
    {
      this.StartStopEngine();
    }

    private void pauseToolStripMenuItem_Click(object sender, EventArgs e)
    {
      this.QueueKey((short)'!');
    }

    private void controlPanelToolStripMenuItem_Click(object sender, EventArgs e)
    {
      this.controlPanel.Visible = this.controlPanelToolStripMenuItem.Checked;
    }

    private void statusBarToolStripMenuItem_Click(object sender, EventArgs e)
    {
      this.mainStatusStrip.Visible = this.statusBarToolStripMenuItem.Checked;
    }
  }
}
