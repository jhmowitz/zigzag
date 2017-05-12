// mycolors.h -- use own (enum) type MYCOLOR before system type COLORS

#ifndef _MYCOLORS_H
#define _MYCOLORS_H

enum MYCOLOR {           // -- colors for graphics: D (dark), L (light)
   DBn, LYw,              // brown, yellow
   DGn, LGn,              // green
   DRd, LRd,              // red
   DCn, LCn,              // cyan
   DMa, LMa,              // magenta
   DBe, LBe,              // blue
   DGy, LGy,              // gray
   DBk, LWe               // black, white
};

#ifdef _MYCOLORS_CPP
#define EXTERN
#else
#define EXTERN	extern
#endif                             // _MYCOLORS_CPP 

EXTERN void InitColor();       // initial main color flags
EXTERN void SetTempGray();     // to set TmpColor
EXTERN void SetColorOn();      // to reset after using TmpColor

                               // transfer own (enum) type MYCOLOR to system type COLORS
EXTERN void SetColor( MYCOLOR );
EXTERN void PutPixel( INT2, INT2, MYCOLOR );
                               // transfer own (enum) type CTYP to system type COLORS
EXTERN void SetColor( CTYP );
EXTERN void SetColEx( CTYP );
EXTERN void PutPixel( INT2, INT2, CTYP );

EXTERN BOOL NoColor;                  // show all in DARKGRAY or not
EXTERN BOOL ExColor;                  // show all in extensified colors or not

#undef EXTERN

// - - - - - - - - - system type COLORS appear on viewgrph.cpp only ! - - - -
#ifdef _MYCOLORS_CPP

BOOL TmpColor;                                   // to keep color temporarily

void InitColor() { NoColor = ExColor = FALSE; }  // initial main color flags
void SetTempGray() {                             // to set TmpColor
   TmpColor = NoColor;
   NoColor = TRUE;
}
void SetColorOn() { NoColor = TmpColor; }        // to reset colors after TmpColor

// transfer MYCOLOR to COLORS -- ! no prototype
COLORS TransferColor( MYCOLOR C ) {
   switch( (INT2)C ) {
      case DBn: return BROWN;
      case LYw: return YELLOW;
      case DGn: return GREEN;
      case LGn: return LIGHTGREEN;
      case DRd: return RED;
      case LRd: return LIGHTRED;
      case DCn: return CYAN;
      case LCn: return LIGHTCYAN;
      case DMa: return MAGENTA;
      case LMa: return LIGHTMAGENTA;
      case DBe: return BLUE;
      case LBe: return LIGHTBLUE;
      case DGy: return DARKGRAY;
      case LGy: return LIGHTGRAY;
      case DBk: return BLACK;
      case LWe: return WHITE;
   }
   return BLACK;
}

void SetColor( MYCOLOR C ) { SETCOLOR( TransferColor(C) ); }

void PutPixel( INT2 X, INT2 Y, MYCOLOR C ) { PUTPIXEL( X, Y, TransferColor(C) ); }

// transfer CTYP to COLORS -- ! no prototype
COLORS Type2Color( CTYP Tp ) {
   switch( Tp ) {
      case AVANE: return LIGHTGREEN;    // arc shrinking
      case ASPLT: return LIGHTRED;      // arc delimited and growing
      case AONCE: return MAGENTA;       // arc of one domain
      case AFULL: return LIGHTGRAY;     // arc of no domain yet (full)

      case BVANE: return YELLOW;        // bisector directed inside, shrinking
      case BGROW: return LIGHTBLUE;     // bisector directed outside, growing
      case BFOLD: return GREEN;         // bisector folded over

      case CONST: return BROWN;         // constant contour
      case CVANE: return YELLOW;        // contour shrinking both sides
      case CGROW: return LIGHTGRAY;     // contour growing both sides
      case CGFLD: return LIGHTBLUE;     // contour folded over, growing
      case CVFLD: return RED;           // contour folded over, vaning

      case EDISI: return LIGHTRED;      // disintegration, loss of complexity
      case ECTCT: return LIGHTCYAN;     // fractioning, gain of complexity
      case ECONV: return YELLOW;        // convergence, transcription of complexity
      case EMERG: return WHITE;         // emergence

      case GLUED: return LIGHTGREEN;    // glued contour
      case GLUE_: return BROWN;         // glued contour (without site)
      case PEEK0: return LIGHTRED;      // good peek: (PeekPeek)
      case PEEK1: return BLUE;          // bad peek: first location
      case PEEK2: return LIGHTBLUE;     // bad peek: second location
      case nPEEK: return LIGHTCYAN;     // no peek: (Peek<>Peek)
      case ATTND: return WHITE;         // attention for something wrong
      case mTEST: return DARKGRAY;      // some test color
      case BACKG: return BLACK;         // background
   }
   return LIGHTGRAY;         // ?? for compiler only
}

// transfer CTYP to COLORS -- ! no prototype
COLORS Type2ColEx( CTYP Tp ) {
   switch( Tp ) {
      case BVANE: return YELLOW;        // bisector directed inside, shrinking
      case BGROW: return LIGHTCYAN;     // bisector directed outside, growing
      case BFOLD: return LIGHTRED;      // bisector folded over

      case CONST: return LIGHTRED;      // constant contour
      case CVANE: return GREEN;         // contour shrinking both sides
      case CGROW: return LIGHTCYAN;     // contour growing both sides
      case CGFLD: return CYAN;          // contour folded over, growing
      case CVFLD: return LIGHTMAGENTA;  // contour folded over, vaning
   }
   return LIGHTGRAY;         // ?? for compiler only
}

void SetColor( CTYP Tp ) { SETCOLOR( Type2Color(Tp) ); }
void SetColEx( CTYP Tp ) { SETCOLOR( Type2ColEx(Tp) ); }
void PutPixel( INT2 X, INT2 Y, CTYP Tp ) { PUTPIXEL( X, Y, Type2Color(Tp) ); }

#endif                             // _MYCOLORS_CPP 
#endif                             // _MYCOLORS_H 
