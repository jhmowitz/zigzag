// graphcmd.h -- 

// #define MOW_DOS
#define MOW_WIN

#ifdef MOW_DOS
   #include <graphics.h>           // graphics library
#else
   #include "graphics.h"
#endif

#if defined(MOW_DOS) || defined(MOW_WIN)

  // define graphics dependant modules
   #define  MOVETO         moveto
   #define  LINETO         lineto
   #define  GETMAXX        getmaxx
   #define  GETMAXY        getmaxy
   #define  OUTTEXTXY      outtextxy
   #define  CLEARDEVICE    cleardevice
   #define  PUTPIXEL       putpixel
   #define  SETCOLOR       setcolor
   #define  SETACTIVEPAGE  setactivepage
   #define  SETVISUALPAGE  setvisualpage
   #define  TEXTWIDTH      textwidth
   #define  TEXTHEIGHT     textheight

#endif

