// psf_cpp.h -- to include c++ code into an application
//              to make PostScript code files of Adobe 1.0

//-----------------------------------------------------------------------
// ! handling of PostScript-coordinates:
//   when the page is divided into several pictures all is done in data
//   when the picture is divided into several pages all is done in data
//-----------------------------------------------------------------------

#include <dos.h>             // getdate()
#include <math.h>            // ABS(), SQRT()
#include <stdlib.h>          // system(), to spool a file
#include <string.h>          // strncpy()

#define  _HAL_CLIP_CPP       // sources for modules and ..
#include  HAL_CLIP_H         // definitions for clipping against a cadre
#undef   _HAL_CLIP_CPP

#include  PS_FILE_H          // definition of ps_file.h

// -- used by: PSMASK::Thickness()
const REAL ThickVal[] = { 0.6, 0.8, 1.1, 1.5, 2.0, 2.6, 3.3 };
const REAL Mask = 2.5;       // pragmatical mask factor

// -- used by: PSMASK::defMaskCore(), drawCurve(),
//             PSPAGE::datemarkPage(), drawFrame(), frawTitle()
//             PSMULT::pageFrame()                     
#define  PSF_WHITE  (REAL)1
#define  PSF_BLACK  (REAL)0

// -- used by: PS_DOM::formatPage()
//             PSMULT::InitMPage(), DrawMPage()
//    number of pixels refering to drawable space of A4-page:
const REAL n_PIX = 555, N_PIX = 810,   // used lengths of short edge / long edge
           t_PIX = 595,                // total length of short side
           o_PIX =  20, O_PIX =  15;   // pixel-coordinates of left lower corner
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef __LONGREAL__            // formats: combined of (long) double and string
  pCHAR gs_F = _T("%Lg %s"),   ggs_F = _T("%Lg %Lg %s"),
        gg_F = _T("%Lg %Lg "), ggg_F = _T("%Lg %Lg %Lg ");
#else
  pCHAR gs_F = _T("%lg %s"),   ggs_F = _T("%lg %lg %s"),
        gg_F = _T("%lg %lg "), ggg_F = _T("%lg %lg %lg ");
#endif                      // __LONGREAL__

const pCHAR Month[] = {        // indication for month in use of date
  _T("jan"), _T("feb"), _T("mar"), _T("apr"), _T("mei"), _T("jun"),
  _T("jul"), _T("aug"), _T("sep"), _T("oct"), _T("nov"), _T("dec"),
};

// ! ----------------------------------- for scale command for paper sizes other than A4
pCHAR sclComment   =   _T("           A0-A6, prescale for standard paper sizes");

struct date {
   INT2        da_day;
   INT2        da_mon;
   INT2        da_year;
};

class PSPRNT {
   struct date Dat;            // keep current date
   pFILE       outF;           // file pointer to file of postscript code 
	 // MT: removed spooling
   // BOOL        Spool;          // spool PSF-file to printer or not
   CHAR        Sbuff[50];      // string to execute a DOS-system command
public:

   PSPRNT() {
     // TODO MT getdate( &Dat ); 
     Dat.da_day = 1;
     Dat.da_mon = 1;
     Dat.da_year = 2012;
   }

   void openFile( pCHAR fName, INT2 lpt, REAL scl ) {
      outF = FOPEN( fName, _T("w") );
      FPRINTF( outF, _T("%s!PS-Adobe-1.0\n"), _T("%") );                  // print file header
      FPRINTF( outF, _T("%sCreator: (%s)\n"), _T("%%"), fName );
      FPRINTF( outF, _T("%sTitle: Picture 1\n"), _T("%%") );
      FPRINTF( outF, _T("%sCreationDate: %2.2d-%2.2d-%2.2d\n"),
                     _T("%%"), Dat.da_day, Dat.da_mon, Dat.da_year );
      FPRINTF( outF, _T("%sDocumentFonts: Helvetica\n\n"), _T("%%") );
      FPRINTF( outF, _T("statusdict begin\n") );                // sequence of pages
      FPRINTF( outF, _T("0 setjobtimeout\nend\n\n") );

      // prescale for standard paper sizes A0 to A6
      comment( sclComment );
      scaleXY( scl );

      // Spool = (BOOL)( lpt == 1 || lpt == 2 || lpt == 3 );   // spool file
      // MT: removed SPRINTF( Sbuff, _T("copy %s lpt%d"), fName, lpt ); because we don't spool to LPTx anymore
			// Keep file name we are printing to, though
			SPRINTF( Sbuff, _T("%s"), fName );
   }
   void closeFile( BOOL Regular ) {
      fclose( outF );
			// MT: removed spooling
      if( Regular /*&& Spool*/ ) {
				// if Spool then system( Sbuff );
				NotifyEvent(_T("eps"), Sbuff);
			}
   }

   void cmdDate() {
      FPRINTF( outF, _T("(MOW %d%s%d) show\n"), Dat.da_day, Month[Dat.da_mon-1], Dat.da_year );
   }
                                            // translate origine
   void translateO( REAL tx, REAL ty ) { FPRINTF( outF, ggs_F, tx, ty, _T("translate\n") ); }
   void rotatePL() {                        // rotate from portrait to landscape
      FPRINTF( outF, gs_F, t_PIX, _T("0 translate ") );  // origine to right below
      FPRINTF( outF, _T("90 rotate\n") );                // rotate axes counter-cw
   }
                                            // set scale for both coordinates
   void scaleXY( REAL s ) { FPRINTF( outF, ggs_F, s, s, _T("scale\n") ); }
   void lineFormat( REAL c, REAL j ) {      // set line cap and line join
      FPRINTF( outF, gs_F, c, _T("setlinecap\n") );
      FPRINTF( outF, gs_F, j, _T("setlinejoin\n") );
   }
   void pageEnd() { FPRINTF( outF, _T("showpage\n\n") ); }
   void pathOpen( REAL G, REAL W ) {        // print Mask(width) or Core(width)
      FPRINTF( outF, gs_F, G, _T("setgray ") );
      FPRINTF( outF, gs_F, W, _T("setlinewidth\nnewpath ") );
   }

   void prtLine( pPNT2 p, pPNT2 q, REAL G, REAL W ) {
      pathOpen( G, W );
      FPRINTF( outF, ggs_F, p->X, p->Y, _T("moveto ") );
      FPRINTF( outF, ggs_F, q->X, q->Y, _T("lineto stroke\n") );
   }
   void prtArc( pPNT2 m, REAL R, REAL L, REAL H, REAL G, REAL W ) {
      pathOpen( G, W );
      FPRINTF( outF, ggg_F, m->X, m->Y, R );
      FPRINTF( outF, ggs_F, degF(L), degF(H), _T("arc stroke\n") ); // ! arc in degrees
   }
   void prtDisc( pPNT2 m, REAL R, REAL G ) { prtDisc( m->X, m->Y, R, G ); }
   void prtDisc( REAL x, REAL y, REAL R, REAL G ) {
      FPRINTF( outF, gs_F, G, _T("setgray\nnewpath ") );
      FPRINTF( outF, gg_F, x, y );
      FPRINTF( outF, gs_F, R, _T("0 360 arc fill\n") );             // ! arc in degrees
   }

   void command( pCHAR t ) { FPRINTF( outF, t ); }
   void comment( pCHAR t ) { FPRINTF( outF, _T("%s%s%c"), _T("%"), t, '\n'  ); }
                                              // comment with address ! zigzag only
   void comment( pCHAR t, INT4 a ) { FPRINTF( outF, _T("%s%s %ld%c"), _T("%"), t, a, '\n'  ); }

   void cmdReal( REAL r, pCHAR t ) { FPRINTF( outF, gs_F, r, t ); }
   void cmdText( pCHAR t ) { FPRINTF( outF, _T("(%s) show\n"), t ); }

   void movetoP( REAL x, REAL y ) { FPRINTF( outF, ggs_F, x, y, _T("moveto ") ); }
   void linetoP( REAL x, REAL y ) { FPRINTF( outF, ggs_F, x, y, _T("lineto ") ); }

   void horiLin( REAL x1, REAL x2, REAL y ) {
      FPRINTF( outF, ggs_F, x1, y, _T("moveto ") );
      FPRINTF( outF, ggs_F, x2, y, _T("lineto ") );
   }
   void vertLin( REAL x, REAL y1, REAL y2 ) {
      FPRINTF( outF, ggs_F, x, y1, _T("moveto ") );
      FPRINTF( outF, ggs_F, x, y2, _T("lineto ") );
   }
};

class PSMASK: public LINHCLIP, public PSPRNT {  // handles widths and masks(gray)
public:
   // -- fixed width and mask for point or line/arc
   PW_IDX PWid;
   LW_IDX LWid;
   MKPATT PMsk, LMsk;

   void SetP( PW_IDX W, MKPATT M ) { PWid = W; PMsk = M; }
   void SetL( LW_IDX W, MKPATT M ) { LWid = W; LMsk = M; }
   REAL LinWid( LW_IDX );           // compute requested line width
   REAL PntWid( PW_IDX );           // compute requested point width

   void baseWidths( TH_IDX, REAL ); // set basic width factors 
   REAL baseWid;                    // basic (smallest) local width
   REAL maskLin,                    // factor to enlarge linewidth with expanding mask
        maskPnt;                    // factor to enlarge pointwidth with expanding mask

   REAL curW,                       // real width for current point or line
        maxW;                       // maximum width, used for clipping halo

   void drawArc( pPNT2, REAL, REAL, REAL, LW_IDX, MKPATT ); // draw arc
   void drawCirc( pPNT2, REAL, LW_IDX, MKPATT );            // draw circle
   void insArcs( pPNT2, REAL, LW_IDX, MKPATT );             // draw arc(s) from clipping

   // ! ----------------------------------------------------- for zigzag only !
   void drawArc( pPNT2, REAL, REAL, REAL  );                // draw arc
   void drawCirc( pPNT2, REAL  );                           // draw circle
   void insArcs( pPNT2, REAL );                             // draw arc(s) from clipping

   void drawCLine( pPNT2, pPNT2, LW_IDX, MKPATT );          // draw (clipped) line
   void insLine( pPNT2, pPNT2, MKPATT, INT2  );             // draw the inside line

   BOOL drawPoint( pPNT2, PW_IDX, MKPATT );                 // draw point
                                 // -- curve drawing
   BOOL Curve, Valid;               // curve is drawn, has a valid previous point PrevP
   PNT2 PrevP;                      // last point to be handled
   void openCurve( pPNT2 );         // print comment, set first point
   void drawCurve( pPNT2 );         // draw curve piecewise
   void endCurve();                 // close ("stroke") curve
   void shortLine();                // to keep the lines on PS-file short

   // ! ------------------------- component, polygon drawing, for zigzag only !
   INT4 cAdr;                       // address of closure to appear in comment
   void openCompo( INT4 );          // print comment, address
   void openPolyg( pPNT2, INT4 );   // print comment, address, set first point

   PSMASK(): LINHCLIP(), PSPRNT() { Curve = FALSE; }
private:
   REAL coreGray,                   // current gray for drawing the core
        maskGray;                   // current gray for drawing the mask
   BOOL defMaskCore( MKPATT );      // initial masking
};

class PSPAGE: public PSMASK {    // handle one or more pictures for one page 
public:                          // .. (PSMULT) indicates uses by PSMULT
   REAL YXrat;        // (PSMULT) y-range divided by x-range
   REAL Scale;        // (PSMULT) divisor to interprete coordinates as pixels
   BOOL Rotate;       // (PSMULT) rotate page to landscape(T) or keep portrait(F)

   PSPAGE(): PSMASK() {}

   // -- initialize page divided into several pictures
   void divValues( INT2 C, INT2 R, REAL YXr ) {     // n of pictures and format
      nCol = C;                       // divide page into C(olum)*R(ow) pictures
      nRow = R;
      YXrat = YXr;                    // picture format
      Scale = 1;                      // neutral scale
      OffX = OffY = 0;                // no offset for picture yet
      PrvX = PrvY = 0;                // no offset of previous picture yet
      FtSiz = 10 - ( C + R + 1 ) / 2; // font size
   }
   void formatPage( REAL, DRAWMK, DRAWDT, MARGIN ); // initialize page
   void datemarkPage( DRAWMK, DRAWDT, REAL ); // (PSMULT) date page and/or mark corner(s)

   // -- handle current picture of divided page
   void selectPos( INT2, pPNT2, REAL );       // -- init #th picture
   void clipFrame();                          // (PSMULT) set frame for clipping
   void drawFrame( DRAWFR, REAL );            // draw frame of picture

   void drawTitle( pCHAR );       // draw title of picture
private:
   INT2 nCol, nRow;   // N of pictures on page (colums*rows)
   INT2 FtSiz;        // font size for title
                   // -- to keep in REAL values
   PNT2 PxPage;       // number of pixels of the page
   PNT2 PxPict;       // number of pixels per picture
   PNT2 PxFrme;       // number of pixels for frame width between pictures

   REAL PrvX, PrvY;   // previous lower left corner under "divided page"
   REAL OffX, OffY;   // last offsets to position a frame
                                             // ! used by formatPage() only ..
   BOOL rotatePage();                          // compute and perform page rotation
   void usagePage( REAL, BOOL, MARGIN );       // page use, pixel of picture and margin
};

class PSMULT: public PSPAGE {   // handle a picture of multiple pages
public:
   INT2 Nc, Nr;        // number of pages used along colums (x) and rows (y)
                    // -- values of the complete picture
   REAL MidX, MidY,    // coordinates of midpoint
        HRgeX;         // half the range of x

   REAL XPage, YPage,  // length of a single page in data after scaling
        SSide;         // length of short side of paper after scaling, Rotate?YPage:XPage
   REAL ThFac;         // factor for base thickness: line, arc and point

   DRAWMK drwM;        // draw mark in corner
   DRAWDT drwD;        // draw date
   DRAWFR drwF;        // draw frame
   DRAWTT drwT;        // draw title
   CUTLIN drwL;        // draw cutlines

   TH_IDX Thickn;      // thickness
   REAL backGr;        // background grayness
   REAL UsePge;        // ## fixed value ## no usage

   PSMULT(): PSPAGE() { UsePge = 0.95; }

   void InitMPage( pPNT2, REAL,           // midpoint, range
                   REAL, REAL,            // ratio, magnifying factor
                   DRAWMK, DRAWDT,        // mark corner, draw date
                   DRAWFR, CUTLIN,        // draw frame, draw cut lines
                   TH_IDX, REAL,          // thickness, backgray
                   INT2 &, INT2 & );      // n of pages: colums (x) and rows (y)
   void DrawMPage( INT2, INT2 );          // draw page of colum c# and row r#
   void pageFrame( INT2, INT2 );          // draw frame of picture
};

static PSMULT PSLoc;                      // local variable to control the domain of data

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - PSMASK
pCHAR pntComment   =   _T("           ..... point");
pCHAR arcComment   =   _T("           ooooo circle or arc");
pCHAR crvComment   =   _T("           {{ }} open curve");
pCHAR linComment[] = { _T("           ----- inside line"),
                       _T("           |---- clipped (single) line"),
                       _T("           |---| clipped (double) line"),
                     };

// ! ----------------------------------- for zigzag only !
pCHAR cmpComment   =   _T("           (( )) component");
pCHAR polComment   =   _T("           << >> polygon");

REAL PSMASK::LinWid( LW_IDX W ) {         // compute requested line width
   return ( 0.2 + 0.3 * W ) * baseWid;
}
REAL PSMASK::PntWid( PW_IDX W ) {         // compute requested point width
   return ( 1.5 + 0.6 * W ) * baseWid;
}
void PSMASK::baseWidths( TH_IDX IxT, REAL Fac ) {
   REAL T = ThickVal[IxT];                // general thickness used for picture
   baseWid = T * Fac * 2;                 // smallest local width ## waarom * 2 ?
   maskLin = Mask / T + T / Mask / 2;     // width factors for line
   maskPnt = SQRT( maskLin );             // .. and for point masking
   // compute an additional cadre from the original and some maximum maxW
   // for the clipping of all circles in the current picture
   LINHCLIP::setCClipP( LinWid(LW_VAST)*maskPnt );
}

BOOL PSMASK::defMaskCore( MKPATT M ) {                // initial masking
   coreGray = ( M != B_FULL && M != W_BB_W ) ? PSF_WHITE : PSF_BLACK;
   if( M == W_FULL || M == B_FULL ) return FALSE;
   maskGray = 1 - coreGray;
   return TRUE;
}

void PSMASK::drawArc( pPNT2 m, REAL R, REAL L, REAL H, LW_IDX W, MKPATT M  ) {
   if( ClipArc(m,R,L,H) ) insArcs( m, R, W, M );
}
void PSMASK::drawCirc( pPNT2 m, REAL R, LW_IDX W, MKPATT M  ) {
   if( ClipArc(m,R) ) insArcs( m, R, W, M );
}
void PSMASK::insArcs( pPNT2 m, REAL R, LW_IDX W, MKPATT M ) { // the inside arcs
   curW = LinWid( W );                      // real width of current line
   maxW = curW * maskLin;                   // possible maximum width
   comment( arcComment );                   // comment on PS-file
   for( INT2 i = 0; i < nArcs(); i++ ) {    // draw n of arcs resulting from clipping
      if( defMaskCore(M) )                                      // draw (thicker) mask
         prtArc( m, R, LowBound(i), HigBound(i), maskGray, maxW );
      prtArc( m, R, LowBound(i), HigBound(i), coreGray, curW ); // draw (thinner) core
   }
}

// ! ----------------------------------------------------- for zigzag only !
void PSMASK::drawArc( pPNT2 m, REAL R, REAL L, REAL H  ) {
   if( ClipArc(m,R,L,H) ) insArcs( m, R );
}
void PSMASK::drawCirc( pPNT2 m, REAL R  ) {
   if( ClipArc(m,R) ) insArcs( m, R );
}
void PSMASK::insArcs( pPNT2 m, REAL R ) {     // ! without comment
   curW = LinWid( LWid );                     // ! use preset width of current line
   maxW = curW * maskLin;
   if( cAdr != 0 ) {
      comment( cmpComment, cAdr );            // component-comment on PS-file
      cAdr = 0L;
   }
   for( INT2 i = 0; i < nArcs(); i++ ) {
      if( defMaskCore(LMsk) )                 // ! use preset mask
         prtArc( m, R, LowBound(i), HigBound(i), maskGray, maxW );
      prtArc( m, R, LowBound(i), HigBound(i), coreGray, curW );
   }
}
// ! ----------------------------------------------------- for zigzag only !

void PSMASK::drawCLine( pPNT2 p, pPNT2 q, LW_IDX W, MKPATT M  ) {
   curW = LinWid( W );                      // real width of current line
   maxW = curW * maskLin;                   // possible maximum width
   BOOL Inp = LINHCLIP::Inner( p, maxW ),
        Inq = LINHCLIP::Inner( q, maxW );
        if( Inp && Inq ) insLine( p, q, M, 0 );
   else if( Inp )        insLine( &(*q+(*p-*q)*LINHCLIP::Enter(q,p,maxW)), p, M, 1 );
   else if( Inq )        insLine( &(*p+(*q-*p)*LINHCLIP::Enter(p,q,maxW)), q, M, 1 );
   else {
      REAL Low, Hig;
      if( LINHCLIP::Shear(p,q,maxW,Low,Hig ) )
         insLine( &(*p+(*q-*p)*Low), &(*p+(*q-*p)*Hig), M, 2 );
   }
}
void PSMASK::insLine( pPNT2 p, pPNT2 q, MKPATT M, INT2 LC  ) {
   comment( linComment[LC] );                            // comment on PS-file
   if( defMaskCore(M) ) prtLine( p, q, maskGray, maxW ); // draw line (thicker) mask
   prtLine( p, q, coreGray, curW );                      // draw line (thinner) core
}

BOOL PSMASK::drawPoint( pPNT2 p, PW_IDX W, MKPATT M ) {
   curW = PntWid( W );                                // width of current point
   maxW = curW * maskPnt;
   if( !LINHCLIP::Inner(p,maxW) ) return TRUE;
   comment( pntComment );                             // comment on PS-file
   if( defMaskCore(M) ) prtDisc( p, maxW, maskGray ); // draw point (thicker) mask
   prtDisc( p, curW, coreGray );                      // draw point (smaller) core
   if( M == B_CRSS ) {
      REAL Dis = curW / sqrt( (REAL)2 );
      pathOpen( maskGray, LinWid(LW_TINY) );          // print Mask(width) or Core(width)
      movetoP( p->X-Dis, p->Y-Dis );
      linetoP( p->X+Dis, p->Y+Dis );
      movetoP( p->X-Dis, p->Y+Dis );
      linetoP( p->X+Dis, p->Y-Dis );
      command( _T("stroke\n") );
   }
   if( M == B_CIRC )
      prtDisc( p, curW/maskPnt, maskGray );           // draw point (smallest) extra core
   return FALSE;
}

void PSMASK::openCompo( INT4 a ) {            // ! zigzag only
   cAdr = a;                                  // keep address of closure (component)
}
void PSMASK::openPolyg( pPNT2 p, INT4 a ) {   // ! zigzag only
   cAdr = a;                                  // keep address of closure (polygon)
   drawCurve( p );                            // draw curve first point
}

void PSMASK::openCurve( pPNT2 p ) {           // print comment, set first point
   cAdr = -1L;                                // no address for curve, differ from polygon
   drawCurve( p );                            // draw curve first point
}
void PSMASK::drawCurve( pPNT2 p ) {             // draw curve piecewise
   curW = LinWid( LWid );                       // ## curvedikte
   if( !LINHCLIP::Inner(p,2*curW) ) {           // ## waarom 2* ?
      if( Curve ) {                             // exit cadre, close curve
         linetoP( p->X, p->Y );
         command( _T("stroke\n") );
         Curve = FALSE;
      }
      PrevP = *p;                     // keep last point while outside
      Valid = TRUE;
   }
   else if( Curve ) {                 // stay inside cadre, continue drawing
      linetoP( p->X, p->Y );
      shortLine();                    // to keep the lines on PS-file short
   }
   else {                             // enter cadre
      Curve = TRUE;
      if( cAdr != 0L ) {
         if( cAdr > 0L ) comment( polComment, cAdr ); // polygon-comment on PS-file
         else            comment( crvComment );       // curve-comment on PS-file
         cAdr = 0L;
      }
                                   // ## linewidth (of curve) possibly variable
      pathOpen( PSF_BLACK, curW );    // print Mask(width) or Core(width)
      if( Valid ) {                   // .. do 1st interval
         movetoP( PrevP.X, PrevP.Y ); // .. for PrevP
         linetoP( p->X, p->Y );       // .. and p
         shortLine();                 // to keep the lines on PS-file short
      }
      else movetoP( p->X, p->Y );     // .. set current point
   }
}
void PSMASK::endCurve() {
   if( Curve ) command( _T("stroke\n") );
   Curve = Valid = FALSE;
}

void PSMASK::shortLine() {      // to keep the lines on PS-file short
   static INT2 nLin = 0;        // maximally 5 (moveto/lineto)-points in one file line
   if( ++nLin == 5 ) { nLin = 0; command( _T("\n") );  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - PSPAGE
void PSPAGE::formatPage( REAL UPg, DRAWMK mkC, DRAWDT DDt, MARGIN Mrg ) {
   BOOL WsmallerH = rotatePage();        // compute and perform page rotation
   datemarkPage( mkC, DDt, PxPage.X );   // date page and/or mark corner(s)
   usagePage( UPg, WsmallerH, Mrg );     // page use, pixel of picture and margin
   lineFormat( 1, 1 );                   // set line cap(round) and line join(round)
}
// -- date page and/or mark corner(s)
void PSPAGE::datemarkPage( DRAWMK mkC, DRAWDT DDt, REAL XL ) {
   // ! before scaling and before positioning the first picture
   // mark left below corner with black circle around white core
   if( mkC == MARK_LEFT || mkC == MARK_BOTH ) {
      prtDisc( 0, 0, 1.2, PSF_BLACK );
      prtDisc( 0, 0, 0.9, PSF_WHITE );
   }
   // draw current date into right below corner (!overrides MARK_RIGHT)
   if( DDt == DATE_YES ) {
      cmdReal( PSF_BLACK, _T("setgray\n") );
      command( _T("/Helvetica findfont\n5 scalefont setfont\n") );
      movetoP( XL-38, -2 );
      cmdDate();
   }
   // mark right (if no date) below corner with full black circle
   else if( mkC == MARK_RIGHT || mkC == MARK_BOTH )
      prtDisc( XL-2.4, 0, 1.2, PSF_BLACK );
}
BOOL PSPAGE::rotatePage() {
   // -- leave page in portrait or rotate to landscape
   //    assign the pixel values of the page in type REAL
   REAL WMin = n_PIX / nCol, HMin = n_PIX / YXrat / nRow,
        WMax = N_PIX / nCol, HMax = N_PIX / YXrat / nRow;
   //     TRUE, if          "portrait"    <     "landscape"  -- take larger of minima
   Rotate = (BOOL)( (WMin<HMax?WMin:HMax) < (WMax<HMin?WMax:HMin) );

   if( Rotate ) {                             // -- page rotates to landscape
      rotatePL();
      PxPage.Assign( N_PIX, n_PIX );
      translateO( O_PIX, o_PIX );                // translate page margins
      return (BOOL)WMax < HMin;
   }
   PxPage.Assign( n_PIX, N_PIX );             // -- page stays in portrait
   translateO( o_PIX, O_PIX );                   // translate page margins
   return (BOOL)WMin < HMax;
}
// -- page use, pixel of picture and margin
void PSPAGE::usagePage( REAL UPg, BOOL WsmlH, MARGIN Mrg ) {
   if( WsmlH ) {                                 // width smaller than height
      REAL Px = UPg * PxPage.X / nCol;           // n of pixels along the side of X
      PxPict.Assign( Px, Px*YXrat );             // use width to make height
      PxFrme.X = PxFrme.Y = (1-UPg) * PxPage.X / nCol / 2;
   }
   else {
      REAL Px = UPg * PxPage.Y / nRow;           // n of pixels along the side of Y
      PxPict.Assign( Px/YXrat, Px );             // use height to make width
      PxFrme.X = PxFrme.Y = (1-UPg) * PxPage.Y / nRow / 2;
   }
                                                 // translate page margin
   REAL PMargX = ( PxPage.X - ( PxPict.X + 2 * PxFrme.X ) * nCol ) / 2,
        PMargY = ( PxPage.Y - ( PxPict.Y + 2 * PxFrme.Y ) * nRow ) / 2;

   if( Mrg == MG_NORMAL ) {
      PxFrme.X += PMargX / nCol / 3; PMargX /= 1.5;
      PxFrme.Y += PMargY / nRow / 3; PMargY /= 1.5;
   }
   else if( Mrg == MG_SPREAD ) {
      PxFrme.X += PMargX / nCol / 2; PMargX /= 2;
      PxFrme.Y += PMargY / nRow / 2; PMargY /= 2;
   }
   translateO( PMargX, PMargY );
}

void PSPAGE::selectPos( INT2 iPic, pPNT2 mP, REAL hXR ) {  // -- init #th picture
   // initial cadre with format YXrat
   LINHCLIP::UpdCadre( mP->X-hXR, mP->X+hXR, mP->Y-hXR*YXrat, mP->Y+hXR*YXrat );

   translateO( PrvX, PrvY );                     // undo previous data translation
   scaleXY( 1./Scale );                          // scale data back to pixel values

   // pixel translate to left lower frame corner of picture iPic
   INT2 C = iPic % nCol,                             // column of picture iPic
        R = nRow - iPic / nCol - 1;                  // row of picture

   REAL offX = ( PxPict.X + 2*PxFrme.X ) * C + PxFrme.X, // coordinates
        offY = ( PxPict.Y + 2*PxFrme.Y ) * R + PxFrme.Y; // .. of origine
   translateO( offX-OffX, offY-OffY );      // translate for next picture
   OffX = offX; OffY = offY;                // keep pixel origine translation

   Scale = PxPict.X / ( HigX() - LowX() );  // scale to bring pixels into data
   scaleXY(0.95 * Scale );                        // set new scale ''20210331 tijdelijk
   translateO( -LowX(), -LowY() );          // translate lower left data corner
                                            // .. into lower left frame corner
   PrvX = LowX(); PrvY = LowY();            // keep data origine translation
}

void PSPAGE::clipFrame() {               // -- set frame for clipping
   command( _T("clippath newpath ") );
   horiLin( LowX(), HigX(), LowY() );
   linetoP( HigX(), HigY() );
   linetoP( LowX(), HigY() );
   command( _T("closepath clip\n") );
}
void PSPAGE::drawFrame( DRAWFR doFrame, REAL backGray ) {
   command( _T("initclip ") );
   if( doFrame ) {                          // draw cadre around picture
      curW = LinWid( LW_TINY );
      LINHCLIP::setHalo( 2*curW );          // draw a bigger cadre than data
      pathOpen( PSF_BLACK, curW );              // print Mask(width) or Core(width)
      horiLin( HLwX(), HHgX(), HLwY() );
      linetoP( HHgX(), HHgY() );
      linetoP( HLwX(), HHgY() );
      command( _T("closepath stroke\n") );
   }
   clipFrame();
   cmdReal( backGray, _T("setgray fill\n") );
}
void PSPAGE::drawTitle( pCHAR tit ) {  // -- draw title of picture
   command( _T("initclip ") );
   REAL FS = (REAL)FtSiz / Scale;
   cmdReal( PSF_BLACK, _T("setgray\n") );
   command( _T("/Helvetica findfont\n") );
   cmdReal( FS, _T("scalefont setfont\n") );
   movetoP( LowX(), LowY()-1.2*FS );
   cmdText( tit );
   clipFrame();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - PSMULT
void PSMULT::InitMPage( pPNT2 mP, REAL hrX, REAL YXr, REAL Mag,
                        DRAWMK dM, DRAWDT dD, DRAWFR dF, CUTLIN dL,
                        TH_IDX th, REAL bG, INT2 &C, INT2 &R ) {
   MidX = mP->X; MidY = mP->Y;
   HRgeX = hrX;
   YXrat = YXr;

   REAL nssx = Mag,                       // number of short side used for x-coordinate
        nssy = Mag;                       // number of short side used for y-coordinate
   if( YXrat < 1.0 ) nssx = Mag / YXrat;  // format of picture is landscapish
   else              nssy = Mag * YXrat;  // format of picture is portraitish

   REAL x_factor = N_PIX / n_PIX;
   // ! regarding a minimum margin of ?? of a short side; thus 2*??
   //   REAL MinMrgX = 0.0;    ## use 0.1(left) and 0.1(right)
   //   REAL MinMrgY = 0.0;    ## and 0.2(under) and 0.1(up)

   // test for solution using the paper with landscape format
   REAL xlong = nssx / x_factor,
        yshrt = nssy;
   INT2 ixl = (INT2)xlong; if( (REAL)ixl < xlong ) ixl++;
   INT2 iyl = (INT2)yshrt; if( (REAL)iyl < yshrt ) iyl++;
   REAL MDL = ABS( (REAL)(ixl - xlong) * x_factor - (REAL)(iyl - yshrt) );

   // test for solution using the paper with portrait format
   REAL xshrt = nssx,
        ylong = nssy / x_factor;
   INT2 ixp = (INT2)xshrt; if( (REAL)ixp < xshrt ) ixp++;
   INT2 iyp = (INT2)ylong; if( (REAL)iyp < ylong ) iyp++;
   REAL MDP = ABS( (REAL)(ixp - xshrt) - (REAL)(iyp - ylong) * x_factor );

   if( MDL < MDP ) {    // A4-page is used with landscape format
      Rotate = TRUE;
      Nc = ixl;
      Nr = iyl;
      YPage = HRgeX * YXrat / yshrt;
      XPage = YPage * x_factor;
   }
   else {               // A4-page is used with portrait format
      Rotate = FALSE;
      Nc = ixp;
      Nr = iyp;
      XPage = HRgeX / xshrt;
      YPage = XPage * x_factor;      
   }
   SSide = Rotate ? YPage : XPage;
   Scale = SSide / n_PIX;
   ThFac = SQRT( Mag ) * Scale / 4;

   Thickn = th;
   backGr = bG;

   drwM = dM;
   drwD = dD;
   drwF = dF;
   drwL = dL;
   drwT = TITLE_NO;      // = dT; ## (voorlopig) geen title

   C = Nc;               // number of pages in colum (x)
   R = Nr;               // number of pages in row (y)
}

void PSMULT::DrawMPage( INT2 Ic, INT2 Ir ) {
   if( Rotate ) {
      rotatePL();                            // rotate from portrait to landscape
      translateO( O_PIX, o_PIX );            // page origin
   }
   else translateO( o_PIX, O_PIX );          // page origin

   // -- mark corner before scaling !
   if( Ir == 0 ) {       // the lowest row
      if( Ic == 0 ) {    // the leftmost page - mark left below corner
         if( Nc == 1 )   // the only colum
            datemarkPage( drwM, drwD, Rotate?N_PIX:n_PIX );
         else if( drwM == MARK_LEFT || drwM == MARK_BOTH )
            datemarkPage( MARK_LEFT, DATE_NO, 0 );
      }
      if( Ic == Nc-1 ) { // the rightmost page - draw date or mark right below corner
         DRAWMK dM;      // draw corner mark in rightmost page
              if( Nc == 1 )                                 dM = drwM;
         else if( drwM == MARK_RIGHT || drwM == MARK_BOTH ) dM = MARK_RIGHT;
         else                                               dM = MARK_NO;
         datemarkPage( dM, drwD, Rotate?N_PIX:n_PIX );
      }
   }

   scaleXY( 1./Scale );                              // set new scale

   REAL lX = MidX - (REAL)( Nc - 2*Ic )/2 * XPage,
        lY = MidY - (REAL)( Nr - 2*Ir )/2 * YPage;
   translateO( -lX, -lY );                          // page origine

   REAL hX = (Ic == Nc-1) ? (MidX + HRgeX / 2 )        : (lX + XPage),
        hY = (Ir == Nr-1) ? (MidY + HRgeX * YXrat / 2) : (lY + YPage);
   if( Ic == 0 ) lX = MidX - HRgeX / 2;
   if( Ir == 0 ) lY = MidY - HRgeX * YXrat / 2;
   LINHCLIP::UpdCadre( lX, hX, lY, hY );

   baseWidths( Thickn, ThFac );            // set basic thickness for line, arc and point
   pageFrame( Ic, Ir );
}

void PSMULT::pageFrame( INT2 c, INT2 r ) {
   lineFormat( 1, 1 );                     // set line cap(butt) and line join(round)
   command( _T("initclip ") );                 // invalid clip frame
   if( drwL != NOLINES ) {                 // draw cutlines for later glueing 
      curW = LinWid( LW_NULL );            // draw a bigger cadre than data
      maxW = 2 * curW;
      REAL lowX = MidX - XPage * ( Nc - 2*c )/2, higX = lowX + XPage,
           lowY = MidY - YPage * ( Nr - 2*r )/2, higY = lowY + YPage;
      REAL outW = 15 * curW;               // to reach outside the (drawable) page
                                           // print Mask(width) or Core(width)
      if( drwL == JOINING || c+r > 0 ) pathOpen( PSF_BLACK, curW );
      if( drwL == OVERLAP || drwL == JOINING ) {
         if( c > 0 ) vertLin( lowX-maxW, lowY-outW, higY+outW ); // vertical left
         if( r > 0 ) horiLin( lowX-outW, higX+outW, lowY-maxW ); // horizontal below
      }
      if( drwL == JOINING ) {
         if( c < Nc-1 ) vertLin( higX+maxW, lowY-outW, higY+outW ); // vertical right
         if( r < Nr-1 ) horiLin( lowX-outW, higX+outW, higY+maxW ); // horizontal above
      }
      command( _T("stroke\n") );
   }

   if( drwF ) {                                   // -- draw cadre around picture
      curW = LinWid( LW_MEDIUM );                    // draw a bigger cadre than data
      maxW = 5 * curW;
      LINHCLIP::setHalo( maxW );
      if( c == 0 || c == Nc-1 || r == 0 || r == Nr-1 )
         pathOpen( PSF_BLACK, curW );                    // print Mask(width) or Core(width)
      if( c == 0 && r > 0 ) {
         vertLin( HLwX(), HLwY(), HHgY() );          // vertival left to LU-corner
         if( r == Nr-1 ) {                           // horizontal above
            linetoP( HHgX(), HHgY() );
            if( Nc == 1 ) linetoP( HHgX(), HLwY() ); // ! Nc == 1 -- do vertical right
         }
      }
      if( r == Nr-1 && c > 0 ) {
         horiLin( HLwX(), HHgX(), HHgY() );          // horizontal above to RU-corner
         if( c == Nc-1 ) {                           // vertical right
            linetoP( HHgX(), HLwY() );
            if( Nr == 1 ) linetoP( HLwX(), HLwY() ); // ! Nr == 1 -- do horizontal below
         }
      }
      if( c == Nc-1 && r < Nr-1 ) {
         vertLin( HHgX(), HHgY(), HLwY() );          // vertical right to RL-corner
         if( r == 0 ) {                              // horizontal below
            linetoP( HLwX(), HLwY() );
            if( Nc == 1 ) linetoP( HLwX(), HHgY() ); // ! Nc == 1 -- do vertical left
         }
      }
      if( r == 0 && c < Nc-1 ) {
         horiLin( HHgX(), HLwX(), HLwY() );          // horizontal below to LL-corner
         if( c == 0 ) {                              // vertical left
            linetoP( HLwX(), HHgY() );
            if( Nr == 1 ) linetoP( HHgX(), HHgY() ); // ! Nr == 1 -- do horizontal above
         }
      }
      command( _T("stroke\n") );
   }
   clipFrame();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - PSFILE
// -- used by the calling program using local variable PSLoc

void PSFILE::Open( pCHAR fName, INT2 lpt, INT2 din ) { // open PostScript file
   // keep paper size or change to standards A3 to A0
   REAL scl = 1, fac = SQRT( 2.0 );      // fac has to equal sqrt(2)
   for( INT2 i = din; i < 4; i++ ) scl *= fac;
   if( din == 4 ) PSLoc.openFile( fName, lpt, 1 );
   else           PSLoc.openFile( fName, 0, scl );  // ! larger sizes will not be spooled

   // ## do fixed width/mask earlier, i.e. in MakePS
   FixWidMsk( PW_MEDIUM, B_WW_B, LW_MEDIUM, B_FULL );  // fixed width/mask for point/line
}

#define  REGULAR  TRUE
#define  ABORTED  FALSE
void PSFILE::Close() { PSLoc.closeFile( REGULAR ); } // close (finished) file
void PSFILE::Quit()  { PSLoc.closeFile( ABORTED ); } // close file, no spooling

void PSFILE::InitMultPage( pPNT2 mP, REAL hrX, REAL YXr, REAL Mag,
                           DRAWMK dM, DRAWDT dD, DRAWFR dF, CUTLIN dL,
                           TH_IDX th, REAL bG, INT2 &C, INT2 &R ) {
   PSLoc.InitMPage( mP, hrX, YXr, Mag, dM, dD, dF, dL, th, bG, C, R );
}
void PSFILE::DrawMultPage( INT2 Ic, INT2 Ir ) { PSLoc.DrawMPage( Ic, Ir ); }

void PSFILE::PicturePage( pPNT2 mdP, REAL XHRng, REAL YXr, REAL UsePage,
                          DRAWMK mkC, DRAWDT DDt,
                          TH_IDX ThIdx, DRAWFR Frm, REAL bGr ) {
   DividePage( 1, 1, YXr, UsePage, mkC, DDt, MG_NARROW );  // one picture only
   InitPicture( mdP, XHRng, 0, ThIdx, Frm, bGr );          // initial that one (0)
}

void PSFILE::DividePage( INT2 C, INT2 R, REAL YXr, REAL UPg,
                         DRAWMK mkC, DRAWDT DDt, MARGIN Mrg ) {
   PSLoc.divValues( C, R, YXr );                  // n of pictures and format
   PSLoc.formatPage( UPg, mkC, DDt, Mrg );        // format page acc. to request
}
void PSFILE::InitPicture( pPNT2 mP, REAL Xr, INT2 iPic,
                          TH_IDX IxT, DRAWFR doF, REAL bGr ) {
   PSLoc.selectPos( iPic, mP, Xr );               // init #th picture
   PSLoc.baseWidths( IxT, 0.2/PSLoc.Scale );      // set basic widths ## waarom 0.2 ?
   PSLoc.drawFrame( doF, bGr );                   // draw frame and clip area
}

void PSFILE::SetTitle( pCHAR T ) { PSLoc.drawTitle( T ); }
void PSFILE::PageEnd() { PSLoc.pageEnd(); }

// -- fixed width and mask for points and/or lines
void PSFILE::PntWidMsk( PW_IDX W, MKPATT M ) { PSLoc.SetP( W, M ); }
void PSFILE::LinWidMsk( LW_IDX W, MKPATT M ) { PSLoc.SetL( W, M ); }
void PSFILE::FixWidMsk( PW_IDX PW, MKPATT PM, LW_IDX LW, MKPATT LM ) {
   PSLoc.SetP( PW, PM );
   PSLoc.SetL( LW, LM );
}

// - - - - - - - - - - - - - draw-functions: fixed or variable width and mask
void PSFILE::DrawArc( pPNT2 m, REAL R, REAL L, REAL H ) {
   PSLoc.drawArc( m, R, L, H, PSLoc.LWid, PSLoc.LMsk );
}
void PSFILE::DrawArc( pPNT2 m, REAL R, REAL L, REAL H, LW_IDX W, MKPATT M ) {
   PSLoc.drawArc( m, R, L, H, W, M );
}
void PSFILE::DrawCirc( pPNT2 m, REAL R ) {
   PSLoc.drawCirc( m, R, PSLoc.LWid, PSLoc.LMsk );
}
void PSFILE::DrawCirc( pPNT2 m, REAL R, LW_IDX W, MKPATT M ) {
   PSLoc.drawCirc( m, R, W, M );
}
void PSFILE::DrawLine( pPNT2 p, pPNT2 q ) {
   PSLoc.drawCLine( p, q, PSLoc.LWid, PSLoc.LMsk );
}
void PSFILE::DrawLine( pPNT2 p, pPNT2 q, LW_IDX W, MKPATT M ) {
   PSLoc.drawCLine( p, q, W, M );
}
BOOL PSFILE::DrawPoint( pPNT2 p ) {
   return PSLoc.drawPoint( p, PSLoc.PWid, PSLoc.PMsk );
}
BOOL PSFILE::DrawPoint( pPNT2 p, PW_IDX W, MKPATT M ) {
   return PSLoc.drawPoint( p, W, M );
}
void PSFILE::OpenCurve( pPNT2 p ) { PSLoc.openCurve( p ); }
void PSFILE::DrawCurve( pPNT2 p ) { PSLoc.drawCurve( p ); }
void PSFILE::EndCurve() { PSLoc.endCurve(); }

// ! ------------------------------------------- for zigzag only !
void PSFILE::OpenCompo( INT4 a ) { PSLoc.openCompo( a ); }
void PSFILE::OpenPolyg( pPNT2 p, INT4 a ) { PSLoc.openPolyg( p, a ); }
void PSFILE::DrawCrcZZ( pPNT2 m, REAL R ) { PSLoc.drawCirc( m, R ); }
void PSFILE::DrawArcZZ( pPNT2 m, REAL R, REAL L, REAL H ) { PSLoc.drawArc( m, R, L, H ); }
