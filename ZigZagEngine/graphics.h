#include "MOW_DEFS.H"

typedef System::Drawing::Color COLORS;

#define BROWN             System::Drawing::Color::Brown
#define YELLOW            System::Drawing::Color::Yellow
#define GREEN             System::Drawing::Color::Green
#define LIGHTGREEN        System::Drawing::Color::LightGreen
#define RED               System::Drawing::Color::DarkRed       // Red
#define LIGHTRED          System::Drawing::Color::Red           // LightRed?
#define CYAN              System::Drawing::Color::Cyan
#define LIGHTCYAN         System::Drawing::Color::LightCyan
#define MAGENTA           System::Drawing::Color::DarkMagenta   // Magenta
#define LIGHTMAGENTA      System::Drawing::Color::Magenta       // LightMagenta
#define BLUE              System::Drawing::Color::Blue
#define LIGHTBLUE         System::Drawing::Color::LightBlue
#define DARKGRAY          System::Drawing::Color::DarkGray
#define LIGHTGRAY         System::Drawing::Color::LightGray
#define BLACK             System::Drawing::Color::Black
#define WHITE             System::Drawing::Color::White

#define VGALO	1
#define VGAMED 2
#define VGAHI	3

#define VGA 1

//COPY_PUT   	0	Copy
//XOR_PUT	1	Exclusive or
//OR_PUT	2	Inclusive or
//AND_PUT	3	And
//NOT_PUT	4	Copy the inverse of the source

#define COPY_PUT 0
#define DEFAULT_FONT 1
#define HORIZ_DIR 1

extern void moveto(INT2, INT2);
extern void lineto(INT2, INT2);
extern INT2 getmaxx(void);
extern INT2 getmaxy(void);
extern void outtextxy(INT2, INT2, pCHAR);
extern void cleardevice();
extern void putpixel(INT2, INT2, COLORS);
extern void setcolor(COLORS);
extern void setactivepage(INT2);
extern void setvisualpage(INT2);
extern INT2 textwidth(pCHAR t);
extern INT2 textheight(pCHAR t);

extern void initgraph(INT2 *, INT2 *, pCHAR);
extern void closegraph(void);

extern void getimage(INT2 u, INT2 d, INT2 l, INT2 r, void *buffer);
extern void putimage(INT2 u, INT2 d, void *buffer, INT2 mode);
extern size_t imagesize(INT2 u, INT2 d, INT2 l, INT2 r);

extern void settextstyle(INT2, INT2, INT2);
extern void setusercharsize(INT2, INT2, INT2, INT2);
extern void setviewport(INT2, INT2, INT2, INT2, INT2);
extern void getaspectratio(INT2 *, INT2 *);

extern void circle(REAL, REAL, REAL);
extern void line(INT2, INT2, INT2, INT2);