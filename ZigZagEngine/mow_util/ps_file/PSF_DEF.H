// psf_def.h -- create a PostScript image representation on file, march '95

// -- control flags to use by calling program

// -------------------------------------- page control for multipages print
enum CUTLIN {
   NOLINES, OVERLAP, JOINING,
};

// -------------------------------------- page control
enum DRAWMK {         // marking of corner(s)
   MARK_NO, MARK_LEFT, MARK_RIGHT, MARK_BOTH,
};

enum DRAWDT {         // drawing date into right below corner
   DATE_NO, DATE_YES,
};

enum MARGIN {         // spread margin between pictures
   MG_NARROW, MG_NORMAL, MG_SPREAD,
};

// -------------------------------------- page and picture control
enum TH_IDX {         // control page/picture thicknesses
    // 0.6       0.8        1.1       1.5      2.0      2.6          3.3
   TK_TINY, TK_SMALL, TK_MEDIUM, TK_LARGE, TK_HUGE, TK_VAST, TK_GIGANTIC,
};

enum DRAWFR {         // drawing of frame
   FRAME_NO, FRAME_YES,
};

enum DRAWTT {         // drawing of title
   TITLE_NO, TITLE_YES,
};

enum LW_IDX {         // (individual) control widths of line/arc
   LW_NULL,           // 0.2 * baseWid
   LW_TINY,           // 0.5 * baseWid
   LW_SMALL,          // 0.8 * baseWid
   LW_MEDIUM,         // 1.1 * baseWid
   LW_LARGE,          // 1.4 * baseWid
   LW_HUGE,           // 1.7 * baseWid
   LW_VAST,           // 2.0 * baseWid
};

enum PW_IDX {         // (individual) control widths of point
   PW_TINY,           // 1.5 * baseWid
   PW_SMALL,          // 2.1 * baseWid
   PW_MEDIUM,         // 2.7 * baseWid
   PW_LARGE,          // 3.3 * baseWid
   PW_HUGE,           // 3.9 * baseWid
};

enum MKPATT {         // (individual) control masking of point, line, arc
   B_FULL,            // fully black
   W_BB_W,            // white around black core
   W_FULL,            // fully white
   B_WW_B,            // black around white core
   B_CRSS,            // circle with cross
   B_CIRC,            // black around white around black core
};

// -- control class for PostScript image representation
class PSFILE {
public:
   void Open( pCHAR, INT2, INT2 );                    // open PostScript file
   void Close();                                      // close PostScipt file
   void Quit();                                       // close file, no printing

   void InitMultPage( pPNT2, REAL, REAL, REAL,        
                      DRAWMK, DRAWDT, DRAWFR, CUTLIN,
                      TH_IDX, REAL, INT2 &, INT2 & ); // sets n of pages: colums by rows
   void DrawMultPage( INT2, INT2 );                   // draw page of colum c and row r 

   void PicturePage( pPNT2, REAL, REAL, REAL,         // one picture per page
                     DRAWMK, DRAWDT,
                     TH_IDX, DRAWFR, REAL );
   void DividePage( INT2, INT2, REAL, REAL,           // several pictures pp
                    DRAWMK, DRAWDT, MARGIN );
   void InitPicture( pPNT2, REAL, INT2,               // init #th picture
                     TH_IDX, DRAWFR, REAL );

   void SetTitle( pCHAR );                            // set title for DatDom
   void PageEnd( void );                              // print end of page

   void PntWidMsk( PW_IDX, MKPATT );                  // point fixed width/masking
   void LinWidMsk( LW_IDX, MKPATT );                  // line fixed width/masking
   void FixWidMsk( PW_IDX, MKPATT, LW_IDX, MKPATT );  // fixed width/masking

                    // -- draw-functions: fixed or variable width and mask
   void DrawArc( pPNT2, REAL, REAL, REAL );                 // arc (fix)
   void DrawArc( pPNT2, REAL, REAL, REAL, LW_IDX, MKPATT ); // arc (var)

   void DrawCirc( pPNT2, REAL );                            // circle (fix)
   void DrawCirc( pPNT2, REAL, LW_IDX, MKPATT );            // circle (var)

   void DrawLine( pPNT2, pPNT2 );                           // line (fix)
   void DrawLine( pPNT2, pPNT2, LW_IDX, MKPATT );           // line (var)

   BOOL DrawPoint( pPNT2 );                    // point, if inside (fix)
   BOOL DrawPoint( pPNT2, PW_IDX, MKPATT );    // point, if inside (var)

   void OpenCurve( pPNT2 );                    // print comment, set first point
   void DrawCurve( pPNT2 );                    // draw curve piecewise
   void EndCurve();                            // close ("stroke") curve

   // ! ------------------------------------------- for zigzag only !
   void OpenCompo( INT4 );                     // print comment, address
   void OpenPolyg( pPNT2, INT4 );              // print comment, address, set first point
   void DrawCrcZZ( pPNT2, REAL );              // without comment
   void DrawArcZZ( pPNT2, REAL, REAL, REAL );  // without comment
};
