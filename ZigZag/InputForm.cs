using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ZigZag
{
  public partial class InputForm : Form
  {
    public InputForm(string prompt, int maxLength = 0)
    {
      InitializeComponent();

      this.promptLabel.Text = prompt;
      if (maxLength != 0)
        this.inputTextBox.MaxLength = maxLength;
    }

    public event EventHandler ValidateInput;
    
    protected void OnValidateInput(EventArgs e)
    {
      if (this.ValidateInput != null)
      {
        this.ValidateInput(this, e);
      }
    }

    public string Input
    {
      get
      {
        return this.inputTextBox.Text;
      }
    }

    private void okButton_Click(object sender, EventArgs e)
    {
      try
      {
        OnValidateInput(EventArgs.Empty);
      }
      catch (Exception ex)
      {
        MessageBox.Show(this, ex.Message, "Invalid input", MessageBoxButtons.OK, MessageBoxIcon.Error);
        this.DialogResult = System.Windows.Forms.DialogResult.None;
        return;
      }

      this.DialogResult = System.Windows.Forms.DialogResult.OK;
    }

    private void inputTextBox_TextChanged(object sender, EventArgs e)
    {
      okButton.Enabled = (inputTextBox.Text.Length > 0);
    }
  }
}
