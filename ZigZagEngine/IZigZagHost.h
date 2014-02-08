#pragma once

using namespace System;

namespace ZigZag
{
  // This *managed* interface defines the services a host must supply
  // to support ZigZag in a graphical environment.
  //
  // Reminder:
  // ^ corresponds to unmanaged * (i.e. a reference to an Object)
  // % corresponds to unmanaged & (i.e. 'pass by reference' of (usually) a ValueType)
  //
  public interface class IZigZagHost
  {
    void initgraph(INT2%, INT2%, String^);
    void closegraph(void);

    void moveto(INT2, INT2);
    void lineto(INT2, INT2);

    INT2 getmaxx(void);
    INT2 getmaxy(void);

    void outtextxy(INT2, INT2, String^);

    void cleardevice(void);

    void setactivepage(INT2);
    void setvisualpage(INT2);

    INT2 textwidth(String^);
    INT2 textheight(String^);

    System::Drawing::Image^ getimage(INT2 u, INT2 d, INT2 l, INT2 r);
    void putimage(INT2 u, INT2 d, System::Drawing::Image^, INT2 mode);
    size_t imagesize(INT2 u, INT2 d, INT2 l, INT2 r);

    void setcolor(COLORS);
    void putpixel(INT2, INT2, COLORS);

    void settextstyle(INT2, INT2, INT2);
    void setusercharsize(INT2, INT2, INT2, INT2);
    void setviewport(INT2, INT2, INT2, INT2, INT2);
    void getaspectratio(INT2%, INT2%);

    void circle(REAL, REAL, REAL);
    void line(INT2, INT2, INT2, INT2);

    void delay(uINT2);
    INT4 biostime(INT2, INT4);

    BOOL kbhit();
    INT2 getch();

    void ShowMessage(String^);

		void NotifyEvent(String^, String^);
		String^ ReadString(String^ prompt, INT2 maxlength);
		INT2 ReadInt(String^ prompt);
		REAL ReadReal(String^ prompt);
		String^ MakeDataFileName(String^ name);
  };
}

