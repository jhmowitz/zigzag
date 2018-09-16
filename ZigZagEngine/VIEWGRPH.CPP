// viewgrph.cpp -- all screen facitities

#if WIN32
#include "doscompat.h"
#else
#include <bios.h>               // biostime()
#include <dos.h>                // delay()
#endif

#include "graphcmd.h"           // graphics library and definition of graphic dependencies
#include <string.h>             // strcpy() - in make_ps.h

#include "mow_defs.h"           // incriptions for includes (use long double)
#include "alg_geo.h"            // structures for points, lines and circles
#include  MOW_LINK_H            // linkage: include class definition
#include "prc_str.h"            // definitions of production classes

#include "prc_def.h"            // definitions of general process classes

#define  _MYCOLORS_CPP
#include "mycolors.h"           // transfer of own type COLOR to system type COLORS
#undef   _MYCOLORS_CPP

#define  _MOW_CLIP_CPP          // includes sources for clipping
#include  LIN_CLIP_H            // definition and sources for clipping against a cadre
#undef   _MOW_CLIP_CPP

#include "cir_str.h"            // definition of component support class SUPPC
#include "tri_str.h"            // definition of polygon support class SUPPT

#include  PS_MASK_H             // "c:\mow_util\ps_file\psf_mask.h" - masking pattern
#define  _VIEWGRPH_CPP          // to include key definitions and PostScript facilities
#ifdef   __MAKE_PSF__           // if program is enabled to make PS-file
#include  PRC_MAKE_PSF_H        // "c:\mow_util\make_psf\p_mk_psf.h" 
#endif                          // __MAKE_PSF__
#include "key_def.h"            // include key definitions
#undef   _VIEWGRPH_CPP

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - TSSTHR
extern void TssThrowProlog();       // initial tesselation throw, if requested
extern void TssThrowEpilog();       // finish tesselation throw, if requested
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - IMAGO/VIEW

// ## probeer IMAGO samen met VIEW te modelleren !
class IMAGO {
   PNT2 Lup, Rdw, Rup, Ldw;  // points of cadre: left/right, up/down
public:
   // ##### 2018-04-08 en
   // ##### 2018-04-20
   // BZTYP What;               // indication drawing circles, triangles or both
   ScrDrawCirc WhatCir;      // draw (screen) circles
   ScrDrawTria WhatTri;      // draw (screen) triangles 
   BOOL  Cadr, DrwC;         // draw cadres, draw on moment(contours) or a time interval
   BOOL  reP, ReP;           // to repaint (one) screen once or always
   uINT2 msDl;               // milliseconds for delay of displays
   BOOL  SeeD;               // make delay visible

   void InitImago( BOOL );   // initial IMAGO
   BOOL Parse( INT2 );       // (ks) parse for keystrokes
   void Cadre();             // draw cadre or not
};

IMAGO IM;

#define   N_VIEW     8       // number of views to handle
class VIEW {
public:
   BOOL  On;
   REAL  LX, HX, LY, HY;    // min/max of X and Y of clip cadre
   // ##### 2018-04-08 
   // BZTYP BZ;                // indication drawing circles, triangles or both
   ScrDrawCirc D_Cir;          // indication drawing circles
   ScrDrawTria D_Tri;          // indication drawing triangles
   VIEW() { On = FALSE; }
   // ##### 2018-04-08 
   //void init(BZTYP W, REAL Lx, REAL Hx, REAL Ly, REAL Hy) {
      void init( ScrDrawCirc Cir, ScrDrawTria Tri, REAL Lx, REAL Hx, REAL Ly, REAL Hy ) {
      On = TRUE;
      // ##### 2018-04-08 
      // BZ = W;
      D_Cir = Cir;
      D_Tri = Tri;
      LX = Lx; HX = Hx;
      LY = Ly; HY = Hy;
   }
   void finit() { On = FALSE; }
};

VIEW View[N_VIEW];

// - - - - - - - - - - - - - - - - - - - - - - - - - VDOM: domain for screen drawing
#define N_PIXEL       (REAL)+0.5      // try normation from LowX/Y to HigX/Y
#define RUI10PC       (REAL)-0.1      // 10 percent:translate right or up or zoom in
#define LDO10PC       (REAL)+0.1      // 10 percent:translate left or down or zoom out
#define LDO18PM       (REAL)+0.018    // 1.8 percent:translate left or down or zoom out

#define FULLCLIP       TRUE           // full clipping in polygonial or curve drawing
#define CURVCLIP       FALSE          // do (not) clip a single piece of line of an arc

#define NofARCLINES   (REAL)16        // minimum number of lines to draw full circle
#define NofLINPIXELS  (REAL)5         // number of pixel for a piece of line

class VDOM: public LIN_CLIP {      // -- rectangular x,y-region around M
   REAL RatYX;                           // the factual screen ratio: y to x

public:
   VEC2 PixRge;                          // X- and Y-range in pixels
   INT2 X, Y;                            // current pixel
//   BOOL ShowRP;                          // show resolving power
//   REAL CurDist();                       // for resolving power

   void GraphInit( BOOL );               // set suitable(starting) screen parameter
   void CadreInit( BOOL );               // initial viewdomain
   BOOL Parse( INT2 );                   // parse keybord for VDOM and IMAGO
                                      // -- clip/draw upon line of two points
   void ClipLeave( pPNT2, pPNT2 );       // line leaving domain
   void ClipEnter( pPNT2, pPNT2 );       // line entering domain
   void ClipShear( pPNT2, pPNT2 );       // line of two points outside

   void ClipALine( pPNT2, pPNT2 );       // points of unknown position
   void DrawClipArcs( BOOL );            // draw arcs "roughly" inside the screen
   void DrawFurther( BOOL, pPNT2, BOOL, pPNT2, BOOL );
   void DrawPolygon( pCLOS p );          // draw a polygon to screen

   void FlatMap( pPNT2 p ) {
      REAL R = ( p->X - LowX() ) * PixRge.X; X = (INT2)R;
           R = ( HigY() - p->Y ) * PixRge.Y; Y = (INT2)R;
   }
   void SetPix( pPNT2 p, INT2 &x, INT2 &y ) { 
      REAL R = ( p->X - LowX() ) * PixRge.X; x = (INT2)R;
           R = ( HigY() - p->Y ) * PixRge.Y; y = (INT2)R;
   }
   void PutPix( pPNT2 p, MYCOLOR Col ) {       // put one pixel, if inside
      if( Inner(p) ) { FlatMap( p ); PutPixel( X, Y, Col ); }
   }
   void MovTo( pPNT2 p ) { FlatMap( p ); MOVETO( X, Y ); }
   void LinTo( pPNT2 p ) { FlatMap( p ); LINETO( X, Y ); }

	 void Zooming( REAL fac ) {
      PixRge.X /= fac;
      PixRge.Y /= fac;
      LIN_CLIP::ZoomOutIn( fac );
   }
};

VDOM VDom;                   // class to represent view/domain

// -- hide VDom for external usage - - - - - - - - - - - - - - - - - - - - - - - - -
// domain initial and domain parsing for prcmain.cpp
void DomainGraphInit( BOOL isVGAMED ) {                     // set screen parameter
   VDom.GraphInit( isVGAMED );
}
void DomainCadreInit( BOOL reset ) { VDom.CadreInit( reset ); }                // initial VIEW and IMAGO
INT2 DomainHandleKeys( INT2 c ) { return VDom.Parse( c ); } // parse for VIEW and IMAGO

// get midpoint and ranges of view domain and manipulate the screen for tss_thr.cpp
pPNT2 CadreMidPnt() { return VDom.MidPnt(); }
REAL  HalfRangeX() { return VDom.Ranges()->X / 2; }
REAL  HalfRangeY() { return VDom.Ranges()->Y / 2; }
REAL  DatRatioYX() { return VDom.Ranges()->Y / VDom.Ranges()->X; }

void MoveTo( pPNT2 p ) { VDom.MovTo( p ); }
void LineTo( pPNT2 p ) { VDom.LinTo( p ); }
void SetPixel( pPNT2 p, INT2 &x, INT2 &y ) { VDom.SetPix( p, x, y ); }

// Communication with host
void ZoomTo(INT2 p, INT2 q, REAL zoomfactor)
{
	INT2 xp = getmaxx() + 1;
	INT2 xq = getmaxy() + 1;
	REAL tx = (REAL)( p - xp / 2 ) / xp;
	REAL ty = (REAL)( xq / 2 - q ) / xq;

	VDom.TranslX(tx);
	VDom.TranslY(ty);
	VDom.Zooming(zoomfactor);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL VDOM::Parse( INT2 c ) {                 // parse keybord for VDOM and IMAGO
   switch( c ) {                                          // -- screen appearence
      case INIT_VIEW: CadreInit( TRUE ); return TRUE;        // '0': reset domain
                                                          // -- zooming:
      case FAST_ZOOM_IN:  Zooming( RUI10PC+1 ); return TRUE; // 'S':   zoom in
      case FAST_ZOOM_OUT: Zooming( LDO10PC+1 ); return TRUE; // 's':   zoom out
      case SLOW_ZOOM_OUT: Zooming( LDO18PM+1 ); return TRUE; // alt_s: zoom out a bit (-31)
                                                          // -- translation: left, right
      case BIG_STEP_RIGHT: TranslX( RUI10PC ); return TRUE;  // 'X':   to right
      case BIG_STEP_LEFT:  TranslX( LDO10PC ); return TRUE;  // 'x':   to left
      case SML_STEP_LEFT:  TranslX( LDO18PM ); return TRUE;  // alt_x: a bit to left (-45)
                                                          // -- translation: up, down
      case BIG_STEP_UP:    TranslY( RUI10PC ); return TRUE;  // 'Y':   up
      case BIG_STEP_DOWN:  TranslY( LDO10PC ); return TRUE;  // 'y':   down
      case SML_STEP_DOWN:  TranslY( LDO18PM ); return TRUE;  // alt_y: a bit down (-21)

      case COLOR_GRAY:     NoColor = !NoColor; return TRUE;  // 'k': gray or color
      case COLOR_EXCITE:   ExColor = !ExColor; return TRUE;  // 'K': normal or excited
   }

   if( c <= INITAL_VIEW && c+N_VIEW > INITAL_VIEW ) {  // -104 (alt_F1): ! c is negative
      INT2 iV = INITAL_VIEW - c;
      View[iV].init( IM.WhatCir, IM.WhatTri , LowX(), HigX(), LowY(), HigY() );
      Message( L"View %d is initialled !", iV+1 );
      return TRUE;
   }
   if( c <= DELETE_VIEW && c+N_VIEW > DELETE_VIEW ) {  // -84 (Shift_F1): ! c is negative
      INT2 iV = DELETE_VIEW - c;
      if( View[iV].On ) {
         View[iV].finit();
         Message( L"View %d is deleted !", iV+1 );
      }
      return TRUE;
   }
   if( c <= ACTIVE_VIEW && c+N_VIEW > ACTIVE_VIEW ) {   // -59 (F1): ! c is negative
      INT2 iV = ACTIVE_VIEW - c;
      if( !View[iV].On )
         Message( L"View %d is not active !", iV+1 );
      else {
         VIEW *v = &View[iV];
         // ##### 2018-04-12 (of eerder?)
         IM.WhatCir = v->D_Cir;
         IM.WhatTri = v->D_Tri;

         UpdCadre( v->LX, v->HX, v->LY, v->HY );           // update CADRE
         PixRge.X = ( N_PIXEL + GETMAXX() ) / Ranges()->X; // pixels per unit of x-data
         PixRge.Y = ( N_PIXEL + GETMAXY() ) / Ranges()->Y; // pixels per unit of y-data
         Message( L"View %d is activated !", iV+1 );
      }
      return TRUE;
   }

   if( IM.Parse(c) ) return TRUE;             // parse keybord for IMAGO

   return FALSE;                              // found nothing
}

void VDOM::GraphInit( BOOL isVGAMED ) { // -- set suitable screen parameter
   settextstyle( DEFAULT_FONT, HORIZ_DIR, 0 );
   setusercharsize( 1, 3, 1, 3 );
   setviewport( 0, 0, GETMAXX(), GETMAXY(), 1 );

	 // GetAspectRatio();

   //// ## to get a good result under Windows-NT together with VGAMED the pixel-xwidth
   ////    obtains an (empirical) average value (7750[VGAMED]+10000[VGAHI])/2
   //if( isVGAMED ) PxWd = 8875;
}

void VDOM::CadreInit( BOOL reset ) {                          // -- set suitable screen parameter
   INT2 PxWd, PxHg;
   getaspectratio( &PxWd, &PxHg );

   // ## to get a good result under Windows-NT together with VGAMED the pixel-xwidth
   //    obtains an (empirical) average value (7750[VGAMED]+10000[VGAHI])/2
   // PxWd = 8875;

   RatYX = (REAL)PxHg * ( GETMAXY() + 1 ) / PxWd / ( GETMAXX() + 1 );

   if (reset) {
		 InitCadre( 0, 1, 0, 0.7, RatYX );                 // initialize a data domain
	 }
	 else {
		 InitCadre( LowX(), HigX(), LowY(), HigY(), RatYX );                 // initialize a data domain
	 }
   PixRge.X = ( N_PIXEL + GETMAXX() ) / Ranges()->X; // x-range in pixels
   PixRge.Y = ( N_PIXEL + GETMAXY() ) / Ranges()->Y; // y-range in pixels
   IM.InitImago( reset );                            // initialize Imago
}

//----------------------------------------------------------------- clipping of lines
/* Clipping against the screen cadre:
 * - ClipALine:   clips one single line
 * - DrawFurther: clips a line of a polygon or curve
 * - InnerLine:   the line is inside the frame, no clipping is necessary
 *
 * - ClipShear:   both points are outside
 * - ClipEnter:   the first point is outside and the second inside
 *
 *   ClipEnter and ClipShear could be used for clipping of polygons as well
 * - ClipLeave: for polygons only, as moveto() to the inside point is assumed
 */

void VDOM::ClipALine( pPNT2 p, pPNT2 q ) {    // -- clip a single line
   if( Inner(p) )                                // p is inside
      if( Inner(q) ) {                           // p and q are inside
         MovTo( p );
         LinTo( q );
      }
      else           ClipEnter( q, p );          // only p is inside
   else
      if( Inner(q) ) ClipEnter( p, q );          // only q is inside
      else           ClipShear( p, q );          // p and q are outside
}

void VDOM::DrawFurther( BOOL prvIn, pPNT2 prvPt,
                        BOOL curIn, pPNT2 curPt, BOOL FClip ) {
   if( curIn ) {
      if( prvIn ) LinTo( curPt );               // continue line to curPt
      else        ClipEnter( prvPt, curPt );    // enter cadre, line to curPt
   }
   else if( prvIn ) ClipLeave( prvPt, curPt );  // leave cadre
   else if( FClip ) ClipShear( prvPt, curPt );  // clip twice or do nothing
}

void VDOM::ClipShear( pPNT2 p, pPNT2 q ) {
   REAL LowPar, HigPar;                     // parameters for clipping
   if( LIN_CLIP::Shear(p,q,LowPar,HigPar) ) {
      MovTo( &( *p + (*q-*p)*LowPar ) );
      LinTo( &( *p + (*q-*p)*HigPar ) );
   }
}

void VDOM::ClipEnter( pPNT2 o, pPNT2 i ) {    // o is outside, i is inside
   MovTo( &( *o + (*i-*o)*LIN_CLIP::Enter(o,i) ) );
   LinTo( i );
}

// ! used in polygonial sequence: MovTo(i) has already happened !
void VDOM::ClipLeave( pPNT2 i, pPNT2 o ) {    // i is inside, o is outside
   LinTo( &( *o + (*i-*o)*LIN_CLIP::Enter(o,i) ) );
}

//----------------------------------------------------------------- draw arcs/circles
class ARCLIN { // to determine the number of lines to draw an arc
public:
   INT2 nLn;   // number of steps within divided arc
   REAL Stp;   // step arc is divided into
               // PixX: number of pixels per data
               // PixL: number of pixels per line piece
               // MinL: minimum number of lines to use for a (small) full circle
   ARCLIN( REAL Arc, REAL Rad, REAL PxXL, REAL MinL ) {
      REAL I = MinL * Arc / TPI;
      REAL X = Arc * Rad * PxXL + 1;
      nLn = ( I > X ) ? (INT2)I : (INT2)X;
      Stp = ( nLn > 0 ) ? Arc / nLn : 0;   
   }
};
                                     // -- facility to draw circle/arc on screen
void VDOM::DrawClipArcs( BOOL XClip ) { // draw arcs "roughly" inside the screen
   PNT2 P[2];                           // along two switching points
   INT2 W;                              // which point to use: 0 or 1
   BOOL prvIn, curIn;                   // previous/current point was/is inside

   for( INT2 i = 0; i < nArcs(); i++ ) {
      ARCLIN AL( HigBound(i)-LowBound(i), Rad(), PixRge.X/NofLINPIXELS, NofARCLINES );
      if( AL.nLn < 0 ) continue;
	  if (AL.nLn == 0) {
		  //if( Inner(mdP()) ) PutPixel( (INT2)(mdP()->X), (INT2)(QQmdP()->Y), LGy);
		  continue;
	  }

      P[W=0] = *mdP() + UnitVec2( LowBound(i) ) * Rad(); // first point
      curIn = Inner( &P[W] );
      if( curIn ) MovTo( &P[W] );

      for( INT2 k = 1; k <= AL.nLn; k++ ) {              // line pieces to draw arc
		                                                 // trailing point(s)
		 P[W = 1 - W] = *mdP() + UnitVec2(LowBound(i) + k*AL.Stp) * Rad();
         prvIn = curIn;
         curIn = Inner( &P[W] );
         DrawFurther( prvIn, &P[1-W], curIn, &P[W], XClip );
      }
   }
}

void VDOM::DrawPolygon( pCLOS p ) {        // -- draw a polygon to screen
   INT2 W;                                    // index of current point (0 or 1)
   PNT2 P[2];                                 // current point and previous point

   SUPPT S( p );
   S.c->other(S.b)->BisPt( &P[W=0], Prcs.D ); // W == 0 for first point
   BOOL Prv, Ins = Inner( &P[W] );
   if( Ins ) MovTo( &P[W] );

   do {
      if( !NoColor ) 
         if( ExColor && S.c->PCnt > 0 ) SetColEx( S.c->Type );
         else                           SetColor( S.c->Type );

      S.b->BisPt( &P[W=1-W], Prcs.D );        // switch W for trailing point(s)
      Prv = Ins;
      Ins = Inner( &P[W] );
      DrawFurther( Prv, &P[1-W], Ins, &P[W], FULLCLIP );
   } while( S.next() );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - IMAGO
void IMAGO::InitImago( BOOL reset ) {      // -- initialize Imago ( after VDom.Init() )
	 if (reset) {
    // ##### 2018-04-08
	 // What = CTBOTH;                 // draw both planes
       WhatCir = CIRC_ON;            // not drawing circles
       WhatTri = TRIA_ON0;             // not drawing triagles
		 DrwC = TRUE;                   // draw mode: contours (not bisectors)
		 Cadr = FALSE;                  // draw cadre not
		 ReP = TRUE;                    // repaint always
		 reP = FALSE;                   // not necessary to repaint on incident
		 SeeD = FALSE;                  // delay is not visible
		 msDl = 5u;                     // ## delay for message
	 }
   Lup.X = Ldw.X = 0.75 * VDom.LowX() + 0.25 * VDom.HigX();
   Rdw.X = Rup.X = 0.25 * VDom.LowX() + 0.75 * VDom.HigX();
   Rdw.Y = Ldw.Y = 0.75 * VDom.LowY() + 0.25 * VDom.HigY();
   Lup.Y = Rup.Y = 0.25 * VDom.LowY() + 0.75 * VDom.HigY();
}

BOOL IMAGO::Parse( INT2 c ) {
   switch( c ) {
      case DRAW_MODE:                      // 'alt_m': change draw mode (-50)
         DrwC = !DrwC; return TRUE;

      case NEXT_GENERA:                    // 'g': draw next generation
         Bubb.incrDG(); return TRUE;
      case PREV_GENERA:                    // 'G': draw previous generation
         Bubb.decrDG(); return TRUE;
      case DRAW_GENERA:                    // alt_g: toggle: draw all (-34)
         Bubb.toggDG(); return TRUE;

      case DRAW_CADRE:                     // alt_/: draw fixed cadre (-53)
         Cadr = !Cadr; return TRUE;
     // ##### 2018-04-08
     // case DRAW_WHICH:                     // 'W': which plane to draw
     //    What = (What == CTBOTH) ? CIRCLE : (BZTYP)(What + 1); return TRUE;
      case DRAW_CIR:                     // 'W': plane for circles to draw
         WhatCir = ( WhatCir == CIRC_OFF ) ? CIRC_ON : CIRC_OFF; return TRUE;
      case DRAW_TRI:                     // 'w': plane for triangles to draw -- Triangle number are bound to their sequence !
         WhatTri = ( WhatTri == TRIA_ON2 ) ? TRIA_OFF : (ScrDrawTria)(WhatTri + 1); return TRUE;

      case ONCE_REPAINT:                   // 'r': repaint screen once
         reP = TWICE; return TRUE;
      case SCRN_REPAINT:                   // 'R': toggle: screen repaint
         ReP = !ReP; return TRUE;

      case SHOW_DELAY:                     // '~': make delay visible
         SeeD = !SeeD; return TRUE;
      case INCR_DELAY:                     // 'D': milliseconds of delay
         msDl++; return TRUE;
      case DECR_DELAY:                     // 'd': decrease delay
         if( msDl > 1u ) msDl--; return TRUE;
   }
   return FALSE;                           // found nothing
}

void IMAGO::Cadre() {
   if( !Cadr ) return;
        if( NoColor ) SetColor( DGy );
     // ##### 2018-04-08                               // ! de waarden van TRIA_ON1 en TRIA_ON2 zijn willekeurig
     //   else if( What == CIRCLE ) SetColor( LMa );
     //   else if( What == TRIANG ) SetColor( LCn );
   else if( WhatCir == CIRC_ON ) SetColor( LMa );
   else if( WhatTri == TRIA_ON0 ) SetColor( LCn );      // is deze kleur juist ?
   else if (WhatTri == TRIA_ON1 ) SetColor( LCn );      // of deze ?
   else if (WhatTri == TRIA_ON2 ) SetColor( LCn );               // en wat is hiermee ?
   else                          SetColor( LWe );
   VDom.ClipALine( &Lup, &Rup );  VDom.ClipALine( &Rup, &Rdw );
   VDom.ClipALine( &Rdw, &Ldw );  VDom.ClipALine( &Ldw, &Lup );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - screen drawing
                                             // -- prototypes of modules for screen drawing
void ScrDrawCirContours();                      // circle contours of all generations
void ScrDrawTriContours();                      // all triangular contours
void ScrDrawTriBisectors();                     // all triangular bisectors

void ExecuteScreenDraw(REAL T) {           // -- make order of display and execute
   Prcs.D = T;                                  // set time for drawing
   TssThrowProlog();                            // initial tesselation throw, if requested

#ifdef   __MAKE_PSF__                        // -- if program is enabled to make PS-file
   MakePSDisplayProlog(IM.WhatCir, IM.WhatTri);              // initial PostScript print
   MakePSShowOptions(GETMAXX(), GETMAXY());   // show PostScript options on screen
#endif                                       // __MAKE_PSF__

   IM.Cadre();
   if (NoColor) SetColor(LGy);               // all in the same color

   // ##### 2018-04-08
   //if (IM.What == CIRCLE || IM.What == CTBOTH) // display circles
   //   ScrDrawCirContours();
   //if (IM.What == TRIANG || IM.What == CTBOTH) // display contours
   //   if( IM.What == CIRCLE || IM.What == CTBOTH ) // display circles
   //   ScrDrawCirContours();
   //if( IM.What == TRIANG || IM.What == CTBOTH ) // display contours
   //   if( IM.DrwC ) ScrDrawTriContours();
   //   else          ScrDrawTriBisectors();

   // ##### 2018-04-11
   if (IM.WhatCir == CIRC_ON) // display circles
      ScrDrawCirContours();

   // ##### 2018-04-13 en 2018-04-20
   if (IM.WhatTri == TRIA_ON0 || IM.WhatTri == TRIA_ON2) // display contours
   {
      zgzg = &ZgZg0;
      if (IM.DrwC) ScrDrawTriContours();
      else         ScrDrawTriBisectors();
   }
   if (IM.WhatTri == TRIA_ON1 || IM.WhatTri == TRIA_ON2) // display contours
   {
      zgzg = &ZgZg1;
      if (IM.DrwC) ScrDrawTriContours();
      else         ScrDrawTriBisectors();
   }
   
      
      TssThrowEpilog();                            // finish tesselation throw, if requested

#ifdef   __MAKE_PSF__                        // -- if program is enabled to make PS-file
   MakePSDisplayEpilog();                       // finish PostScript print
#endif                                       // __MAKE_PSF__
}

#define    MY_CLK_TCK    20.0       // ## wat moet hier staan ? 50.0 beperkt te zeer
void DelayScreenDraw() {            // delay the image relatively to its duration
   static INT4 Now = biostime( 0, 0L );
   INT4 Prv = Now; Now = biostime( 0, 0L );
	 REAL perSec = (REAL)IM.msDl * MY_CLK_TCK / ( Now - Prv + 1 );
	 delay( (uINT2)perSec );
   if( IM.SeeD ) {
      INT4 per10 = (INT4)(10L * MY_CLK_TCK / ( Now - Prv + 1 ));
      QuickMs( _T("%d"), (INT2)per10 );
   }
}
#undef MY_CLK_TCK

// -- general screen drawing modules for one page: VGAHI
//                                   or two pages: VGALO or VGAMED

void Draw1Page( REAL Tim ) {              // VGAHI: draw on one page (! no PostScript)
   if( IM.reP || IM.ReP ) { IM.reP = FALSE; CLEARDEVICE(); }
   ExecuteScreenDraw( Tim );              // draw at Tim
   DelayScreenDraw();                     // delay the image
}

void Draw2Pages( REAL Tim ) {             // VGALO or VGAMED: draw on two pages
   static INT2 Page = 0;                  // to start with page 0 being visible and active
   SETACTIVEPAGE( Page=1-Page );          // active page is not the visual one
        if( IM.ReP ) CLEARDEVICE();
   else if( IM.reP ) { IM.reP--; CLEARDEVICE(); }

#ifdef   __MAKE_PSF__                     // -- if program is enabled to make PS-file
   // several time constellations into one image (one page of paper only!)
   if( MakePSTestNext_FS1(Tim) || MakePSTestNext_FSM(Tim) ) {
      ExecuteScreenDraw( MakePS.FS_LastTime );
      SETVISUALPAGE( Page );              // active page is visual now
      SETACTIVEPAGE( Page=1-Page );       // active page is not the visual one
   }
   // one time constellation using multiple pages of paper
   else {
      INT2 iP = 0;                        // page index
      while( MakePSHandleMultPages() ) {  // redraw while pages of paper are requested
         ExecuteScreenDraw( Tim );        // draw at Tim
         SETVISUALPAGE( Page );           // active page is visual now
         Message( L"Draw page nr %d", ++iP );
         DelayScreenDraw();               // delay the image
         SETACTIVEPAGE( Page=1-Page );    // active page is not the visual one
         CLEARDEVICE();
      }
   }
#endif                                    // __MAKE_PSF__

   ExecuteScreenDraw( Tim );              // draw at Tim
   SETVISUALPAGE( Page );                 // active page is visual now
   DelayScreenDraw();                     // delay the image
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - modules containing MakePS
void DrawPnt( pPNT2 p, MYCOLOR C, MskP mP ) {
   VDom.PutPix( p, C );
#ifdef   __MAKE_PSF__                     // -- if program is enabled to make PS-file
   if( MakePS.DoDraw ) MakePS.Point( p, mP );
#endif                                    // __MAKE_PSF__
}

void DrawArc( pPNT2 p, REAL R ) {                  // draw full circle
   if( VDom.ClipArc(p,R) ) VDom.DrawClipArcs( CURVCLIP );
}
void DrawArc( pPNT2 p, REAL R, REAL L, REAL H ) {  // draw arc (part of circle)
   if( VDom.ClipArc(p,R,L,H) ) VDom.DrawClipArcs( CURVCLIP );
}

/*
void DrawCirc( pPNT2 p, REAL R, MskL mL ) {                      // ## unused
   if( VDom.ClipArc(p,R) ) VDom.DrawClipArcs( CURVCLIP );
#ifdef   __MAKE_PSF__                     // -- if program is enabled to make PS-file
   if( MakePS.DoDraw ) MakePS.Circ( p, R, mL );
#endif                                    // __MAKE_PSF__
}
void DrawArc( pPNT2 p, REAL R, REAL L, REAL H, MskL mL ) {       // ## unused
   if( VDom.ClipArc(p,R,L,H) ) VDom.DrawClipArcs( CURVCLIP );
#ifdef   __MAKE_PSF__                     // -- if program is enabled to make PS-file
   if( MakePS.DoDraw ) MakePS.Arc( p, R, L, H, mL );
#endif                                    // __MAKE_PSF__
}
*/

void DrawLin( pPNT2 p, pPNT2 q, MskL mL ) {
   VDom.ClipALine( p, q );
#ifdef   __MAKE_PSF__                     // -- if program is enabled to make PS-file
   if( MakePS.DoDraw ) MakePS.Line( p, q, mL );
#endif                                    // __MAKE_PSF__
}
void DrawPol( pCLOS p, MskL mL ) {
   VDom.DrawPolygon( p );
#ifdef   __MAKE_PSF__                     // -- if program is enabled to make PS-file
   if( MakePS.DoDraw ) MakePS.Poly( p, mL ); // draw PostScript polygon
#endif                                    // __MAKE_PSF__
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - screen draw
void SDrw_PntBisectors() {                                            // ! no prototype
   SetColor( DBn );
   pBUMP v = (pBUMP)Bubb.Cct;
   while( (v=(pBUMP)v->Next()) != NULL ) {
      pTISP e = v->Event, o1 = v->Arc1->Orign, o2 = v->Arc2->Orign;
      DrawLin( ParamP2(o1,e,(Prcs.D-o1->TimB)/(e->TimB-o1->TimB)),
               ParamP2(o2,e,(Prcs.D-o2->TimB)/(e->TimB-o2->TimB)), L_TINY );
      DrawPnt( e, LYw, T_BLCK );
   }
   if( Prcs.Flw > 0 ) {
      SetColor( DGy );
      pARCS c = (pARCS)Bubb.Dis;
      while( (c=(pARCS)c->Next()) != NULL ) {
         pTISP e = c->Event, o = c->Orign;
         DrawLin( e, ParamP2(o,e,(Prcs.D-o->TimB)/(e->TimB-o->TimB)), L_NULL );
         DrawPnt( e, LWe, T_BLCK );
      }
   }
}
void DrawCompArcs( pCLOS cmp ) {                                     // ! no prototype
   SUPPC S( cmp, DIRECTION, Prcs.D );
   do {
      if( !NoColor ) SetColor( S.c->Type );       // colour of current arc
      if( S.c->Neigh != S.c ) DrawArc( S.cp, S.cR, S.L, S.H );
      else                    DrawArc( S.cp, S.cR );
   } while( S.next() );
#ifdef   __MAKE_PSF__                     // -- if program is enabled to make PS-file
   if( MakePS.DoDraw ) MakePS.Comp( cmp, L_MEDI ); // draw PostScript circle component
#endif                                    // __MAKE_PSF__
}

void SDrw_OneCirContour() {                       // draw circles/arcs of one generation
   if( !NoColor && ExColor ) SDrw_PntBisectors(); // draw bumps of one generation
   pCLOS cmp = Bubb.Act;
   while( (cmp=cmp->Prev) != Bubb.Act ) DrawCompArcs( cmp );
}
void ScrDrawCirContours() {                       // draw circles/arcs
   if( Bubb.Drw1G ) {                             // draw one generation only
      Bubb.initDG();
      SDrw_OneCirContour();
   }
   else for( Bubb.init(); Bubb.valid(); Bubb.incr() ) // draw all generations
           SDrw_OneCirContour();
}
                                                                        // ! no prototype
void DrawBis( pCLOS p ) {  // draw bisector line last 20% up to Prcs.D (presence)
   SUPPT S( p );
   do {                    // draw current bisector
      // ## change for Arno's drawings, needing full bisector
      PNT2 P; S.b->BisPt( &P, S.b->Orign->TimB*0.2+Prcs.D*0.8 );
//      PNT2 P; S.b->BisPt( &P, S.b->Orign->TimB );
      PNT2 Q; S.b->BisPt( &Q, Prcs.D );
      if( !NoColor )
         if( ExColor && S.b->Event != NULL ) SetColEx( S.b->Type );
         else                                SetColor( S.b->Type );
      DrawLin( &P, &Q, L_MEDI );
   } while( S.prev() );
}
                                                                        // ! no prototype
void SDrw_PeekBisc() {           // draw peeking bisectors from (current) drawing time
   // ##### 2018-04-13
   // pBISC b = (pBISC)ZgZg1.Cct;    // .. up to some future time T
   pBISC b = (pBISC)(zgzg->Cct);    // .. up to some future time T     ?? waarom de haakjes ?? priority ?
   while( (b=(pBISC)b->Next()) != NULL ) {         // piece of bisector line
      REAL T = ( Prcs.S > Prcs.X ) ? ( Prcs.D + 5 * Prcs.S )
                                   : ( Prcs.D + 3 * Prcs.X );
      if( T > b->Event->TimB ) T = b->Event->TimB; // limitation of T to time of event
      PNT2 P; b->BisPt( &P, Prcs.D );              // point at drawing time
      PNT2 Q; b->BisPt( &Q, T );                   // point at T
      SetColor( LRd );
      DrawLin( &P, &Q, L_SMLL );

      if( Prcs.Flw > 0 ) {                         // line from bisector to event
         SetColor( DGy );
         DrawLin( &Q, b->Event, L_NULL );
         DrawPnt( b->Event, LWe, T_BLCK );
         if( Prcs.Flw > 1 ) {                      // line from Site to midpoint of contour
            pCONT c = b->Peek;                     // .. to be peeked
            c->Bis1->BisPt( &P, Prcs.D );
            c->Bis2->BisPt( &Q, Prcs.D );
            SetColor( DRd );
            VDom.ClipALine( ParamP2(&P,&Q,0.5), b->Event );
         }
      }
   }
}

void ScrDrawTriContours() {               // -- draw all polygons
   if( !NoColor && ExColor )
      SDrw_PeekBisc();                       // if requested, draw all peeks
   // ##### 2018-04-22
   pCLOS p = zgzg->Act;
   while( (p=p->Prev) != zgzg->Act )
      DrawPol( p, L_MEDI );                  // clip/draw the whole polygon
}
void ScrDrawTriBisectors() {
   // ##### 2018-04-22
   pCLOS p = zgzg->Act;
   while( (p=p->Prev) != zgzg->Act )
      DrawBis( p );                          // draw current bisector
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - TEST
//* ##### 2018-04-13
void ShowEvent( ScrDrawCirc FromC, ScrDrawTria FromT, pPNT2 p, CTYP C, pCHAR T ) {
   // the BZTYP that is shown is not the one requesting to show an event

   // ##### 2018-04-20
   return;
   /* ? very voorlopig !
   if( IM.WhatCir != FromC && IM.WhatTri != FromT ) return;     <- has been translated only, must be checked
   // the circle event is not from the generation shown currently
   if( From == CIRCLE && Bubb.Drw1G && Bubb.DrawG != Bubb.curGen() ) return;
   */
   SetColor( C );
   OUTTEXTXY( 5, 15, T );
   if( VDom.Inner(p) ) {
      VDom.FlatMap( p );
      OUTTEXTXY( VDom.X+15, VDom.Y+10, T );
      circle( VDom.X, VDom.Y, 10 );
      if( C != ECONV ) PutPixel( VDom.X, VDom.Y, ATTND );
   }
   delay( IM.msDl * 150 );
   // ## a possibility to stop here under test condition
   // ## a means to set the process step to 0
}

void ShowCircle( pPNT2 p, REAL L, REAL H, REAL R, CTYP C, uINT2 D ) {
   if( IM.WhatCir != CIRC_ON ) return;                              // nothing to draw
   if( Bubb.Drw1G && Bubb.DrawG != Bubb.curGen() ) return;      // wrong generation
   SetColor( C );                                               // colour of test arc
   if( VDom.ClipArc(p,R,L,H) ) VDom.DrawClipArcs( CURVCLIP );   // draw circle
   delay( D * 1000 );
}
 // End of File
