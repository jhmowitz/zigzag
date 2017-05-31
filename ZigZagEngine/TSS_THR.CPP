// tss_thr.cpp - handle regular tesselations for a throw-in start
//               tryout 20/12/95

#include <stdio.h>
#include <stdlib.h>             // rand(), srand(), RAND_MAX
#include <math.h>               // COS(), SIN()

// #include "mow_defs.h"           // incriptions for includes (use long double)
#include "doscompat.h"
#include "alg_geo.h"            // structures for points, lines and circles
#include  MOW_LINK_H            // linkage: include class definition
#include "prc_str.h"            // serving structures

#include "prc_def.h"            // all general process classes

#include "mycolors.h"           // transfer of type or COLOR to COLORS
#define  _TSS_THR_CPP
#include "key_def.h"
#undef   _TSS_THR_CPP
#include "tss_thr.h"            // throwing "regular" tesselations or at random

// external from viewgrph.cpp to handle screen domain
extern pPNT2 CadreMidPnt();
extern REAL  HalfRangeX();
extern REAL  HalfRangeY();
extern void  MoveTo( pPNT2 );
extern void  LineTo( pPNT2 );
extern void  SetPixel( pPNT2, INT2 &, INT2 & );

BOOL PROCS::ExtraEmer() {            // emergence at "loss of structure"
        if( ExtraEm_V == 0 ) return FALSE;
   else if( ExtraEm_V == 1 ) return TRUE;
   return (BOOL)( (REAL)rand() / RAND_MAX <= ExtraEm_V );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - to hide TSSTHR TssThrw
TSSTHR TssThrw;                     // throwing circles into "regular" tessalations
RDMTHR RdmThrw;                     // to control "random" throwing of circles/triangles
FILTHR FilThrw;                     // to control throwing from file data

                                 // -- hide for prcmain.cpp
BOOL TssThrowHandleKeys( INT2 c ) { // throw "regular" and "random" tesselations
   return TssThrw.Parse( c );
}
BOOL TssThrowIsOn() { return TssThrw.On; }
                                 // -- hide for viewgrph.cpp
void TssThrowProlog() { if( TssThrw.On ) SetTempGray(); }
void TssThrowEpilog() {
   if( TssThrw.On ) {
      SetColorOn();
      TssThrw.ShowUp();
   }
}

// -- a random number from (Mrg,1-Mrg), 0 <= Mrg <= 1
REAL MargVal( REAL Mrg ) { return Mrg + ( 1 - 2 * Mrg ) * rand() / RAND_MAX; }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - TSSL
#define   DISPERSE   (REAL)0.01      // dispersion in time: to appear not at current time
#define   MIN_DISP   (REAL)0.00001   // minimal dispersion in time
                                     // or not at the same time
#define   MIN_DENS         2         // minimum density
#define   MAX_DENS        30         // maximum density
#define   MIN_PERT         1         // minimum perturbation (1%)
#define   MAX_PERT       100         // maximum perturbation (100%)
#define   MAX_OMIT        20         // maximum of omittance

INT2 iOmt[MAX_OMIT];                 // indeces of points of omittance

BOOL TSSL::Parse( INT2 c ) {
   switch( c ) {
      case TESS_TYPE:                         // 't': change type of tesselation
         Tssl = ( Tssl != HEXA_T ) ? (TTYP)( 1 + (INT2)Tssl ) : NONE_T;
         return TRUE;

      // -- number of points in tesselation
      case DECR_DENSITY:                      // 'd': decrease density
         if( Dens > MIN_DENS ) Dens--; return TRUE;
      case INCR_DENSITY:                      // 'D': increase density
         if( Dens < MAX_DENS ) Dens++; return TRUE;

      // -- omittance of points
      case DECR_OMITT:                        // 'e': decrease number
         if( nO > 0 ) nO--; return TRUE;
      case INCR_OMITT:                        // 'E': increase number
         if( nO < MAX_OMIT ) nO++; return TRUE;
   
      // -- rotate tesselation 3 (or about 0.7) degrees
      case TESS_ROT_CW:                       // 'R': clockwise
         RotT -= _PI / 60; return TRUE;
      case TESS_ROT_ACW:                      // 'r': anticlockwise
         RotT += _PI / 60; return TRUE;
      case TESS_ROT_BIT:                      // alt_r: a bit anticlockwise
         RotT += _PI / 260; return TRUE;

      // -- factor of point perturbation within a tesselation
      case INCR_PERTFAC:                      // '+': increase factor
         if( NPrt < MAX_PERT ) NPrt++; return TRUE;
      case DECR_PERTFAC:                      // '-': decrease factor
         if( NPrt > MIN_PERT ) NPrt--; return TRUE;

      // -- start of random generator
      case INCR_RANDOM:                       // '>': increase start
         rBeg++; return TRUE;
      case DECR_RANDOM:                       // '<': decrease start
         if( rBeg > 0u ) rBeg--; return TRUE;
   }
   return FALSE;
}

void TSSL::setOmittance() {          // -- set indeces of omitted points
   INT2 i;
   for( i = 0; i < nO; i++ ) {
      REAL tmp = (REAL)nTO * rand() / RAND_MAX;
      iOmt[i] = (INT2)tmp;
   }
   while( i++ < MAX_OMIT ) rand();      // keep random generator in tune
}
BOOL TSSL::tstOmittance() {          // -- test point index for omittance 
   for( INT2 i = 0; i < nO; i++ )
      if( iOmt[i] == nT ) {             // point is omitted
         iOmt[i] = -1;                  // point is only omitted once
         return TRUE;
      }
   return FALSE;                        // point is left in tesselation
}
#undef MIN_DENS
#undef MAX_DENS
#undef MIN_PERT
#undef MAX_PERT
#undef MAX_OMIT

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - TSSTHR
MYCOLOR AttCol = LRd, NegCol = LGn,   // color of attention or of neglect
        TriCol = LMa,                 // for tesselation points: triangles,
        SqrCol = LCn, HexCol = LBe;   // squares or hexagons

void TSSTHR::ShowUp() {          // -- draw tesselation throw to visual page
   ScreenInit();                    // reset Mp, Radi
   DrawFrame();
   SetTessela();
   if( Subm ) On = FALSE;
}

BOOL TSSTHR::Parse( INT2 c ) {      // -- parse for "regular"and "random" tesselations
   if( !On )                                      // -- is not active
      if( c == TESS_THROW ) { Init(); return TRUE; } // '.': activate tesselation throw
      else if( RdmThrw.Parse(c) ) return TRUE;       // throw "random" tesselation
      else if( FilThrw.Parse(c) ) return TRUE;       // throw tesselation from file
      else return FALSE;                             // arbitrary key is rejected

   // ! tesselation throw is active
   if( t != NULL )                   // parsing for current tesselation
      if( t->Parse(c) ) return TRUE;

   switch( c ) {
      case TESS_SUBMIT:              // ':': submit tesselation
         Message( L"Submit tesselations" );
         return Subm = Was = TRUE;
      case TESS_THROW:               // reinitial the tesselation throw
         Was = FALSE; Init(); return TRUE;
      case TESS_ABORT:               // Esc: no submit !
         On = FALSE;                 // keep existing parameters of the throw
         return TRUE;
                                  // -- handle frame (and subframe)
      case FRAME_TYPE:               // 'f': change frame type, set nF, Fp[]
         Frme = ( Frme != HEXA_F ) ? (FTYP)( 1 + (INT2)Frme ) : CIRC_F;
         SetFrame();
         return TRUE;
      case FRAME_SPLT:               // '/': toggle single/double frame
         DubF = !DubF;
         nTess = ( DubF ) ? 2 : 1;
         if( !DubF ) {
            t = &T[IxT=0];
            if( t->Tssl == NONE_T ) t->Tssl = TRIA_T;
         }
         return TRUE;
      case FRAME_SWITCH:             // alt_f: switch current frame
         if( DubF ) {
            IxT = ++IxT%3;
            t = (IxT < 2) ? &T[IxT] : NULL;
         }
         return TRUE;

      // -- change radius of frame
      case INCR_FRAME:                         // 'L': increase
         if( FFac < 0.95 ) FFac += 0.031; return TRUE;
      case DECR_FRAME:                         // 'l': decrease
         if( FFac > 0.05 ) FFac -= 0.031; return TRUE;
      case BITD_FRAME:                         // alt_l: little decrease
         if( FFac > 0.05 ) FFac -= 0.005; return TRUE;
   
      // -- change format of [rectangular] frame
      case FRAME_LONGER:                   // 'A': increase
         if( RctA < HPI-HPI/30 ) RctA += HPI / 60; return TRUE;
      case FRAME_SHRTER:                   // 'a': decrease
         if( RctA > HPI/2+HPI/30 ) RctA -= HPI / 60; return TRUE;
      case FRAME_BTSHRT:                   // alt_a: little decrease (-30)
         if( RctA > HPI/2+HPI/50 ) RctA -= HPI / 280; return TRUE;
   
      // -- rotate frame 3 (or about 0.6) degrees
      case FRAME_ROT_CW:                      // 'Q': clockwise
         RotF -= _PI / 60; return TRUE;
      case FRAME_ROT_ACW:                     // 'q': anticlockwise
         RotF += _PI / 60; return TRUE;
      case FRAME_ROT_BIT:                     // alt_q: a bit anticlockwise
         RotF += _PI / 280; return TRUE;

      // -- change radius of subframe
      case INCR_SUBFRAME:            // 'O': increase radius
         if( DubF )
            if( RFac < 0.93 ) RFac += 0.031;
         return TRUE;
      case DECR_SUBFRAME:            // 'o': decrease radius
         if( DubF )
            if( RFac > 0.07 ) RFac -= 0.031;
         return TRUE;
      case BITD_SUBFRAME:            // alt_o: decrease radius a bit
         if( DubF )
            if( RFac > 0.07 ) RFac -= 0.005;
         return TRUE;

      case NARROW_RING:              // 'h': narrow ring between frames
         if( DubF )
            if( Wide > 0 ) Wide -= 0.01;
         return TRUE;
      case WIDEN_RING:               // 'H': widen ring between frames
         if( DubF )
            if( Wide < 0.07 ) Wide += 0.01;
         return TRUE;

      case THROW_TYPE:               // ',': change type of throw objects
         Circ = !Circ;
         return TRUE;
   }
   return FALSE;
}

void TSSTHR::Init() {
   if( !Was ) {
      Frme = CIRC_F;             // current frame type is circle
      DubF = FALSE;              // draw single frame
      RotF = 0;                  // no rotation
      FFac = 0.95;               // frame is nearly full screen
      RFac = 0.5;                // subframe radius is half of frame radius
      Wide = 0;                  // no halo between frame and subframe
      RctA = _PI * 0.31;         // format of rectangle resembles screen format at start
      Circ = TRUE;               // throw circles

      nTess = 1;
      T[0].Init();
      T[1].Init();
      t = &T[IxT=0];
   }
//   ScreenInit();                 // set Mp, Radi ## deze is niet nodig !!
   Subm = FALSE;
   On = TRUE;
}

void TSSTHR::ScreenInit() {         // -- set Mp, Radi
   Mp = *CadreMidPnt();                // set the midpoint
   Radi = FFac * HalfRangeY();         // vertical diameter of frame
   SetFrame();                         // set nF, Fp[]
}

void TSSTHR::SetFrame() {              // set nF, Fp[] .. needs Frme, Radi
   switch( Frme ) {
      case CIRC_F:
         nF = 0;
         break;
      case TRIA_F:
         Mp.Y -= 0.3 * Radi;           // adjust Mp to triangle's gravity point
         FramePts( 3, HPI, 1.2*Radi );
         break;
      case SQRE_F:
         FramePts( 4, HPI/2, Radi );
         break;
      case RECT_F:
         nF = 4;
         Fp[0] = Mp + UnitVec2( HPI  -RctA+RotF ) * ( 1.5 * Radi );
         Fp[1] = Mp + UnitVec2( HPI  +RctA+RotF ) * ( 1.5 * Radi );
         Fp[2] = Mp + UnitVec2( HPI*3-RctA+RotF ) * ( 1.5 * Radi );
         Fp[3] = Mp + UnitVec2( HPI*3+RctA+RotF ) * ( 1.5 * Radi );
         break;
      case HEXA_F:
         FramePts( 6, 0, Radi );
   }
}

void TSSTHR::FramePts( INT2 n, REAL Off, REAL Rad ) {      // set nF, Fp[]
   nF = n;
   for( INT2 i = 0; i < n; i++ )
      Fp[i] = Mp + UnitVec2( Off+RotF+TPI*i/n ) * Rad;
}

void TSSTHR::SetTessela() {
   for( INT2 i = 0; i < nTess; i++ ) {
      l = &T[i];
      srand( l->rBeg );            // initiate random number generation
      l->setOmittance();           // set indeces for points to be omitted
      nS = 3 * l->Dens;            // ## waarom 3 * l->Dens ?

      l->nT = 0;                   // count number of inside and not omitted points
      switch( l->Tssl ) {
         case TRIA_T: TesselTriSqr( _PI/3, TriCol ); break;
         case SQRE_T: TesselTriSqr( HPI, SqrCol ); break;
         case HEXA_T: TesselHexa();
      }
      l->nTO = l->nT + l->nO;
   }
   pCHAR obj = Circ ? _T("points") : _T("doubleVs");
   if( IxT == 2 ) QuickMs( _T("%d %s"), T[0].nT+T[1].nT, obj );
   else           QuickMs( _T("Density %d (%d %s), Perturbation %d%%, [%u]"),
                                    t->Dens, t->nT, obj, t->NPrt, t->rBeg );
}

void TSSTHR::TesselTriSqr( REAL Off, MYCOLOR Col ) {
   TessCol = Col;
   VEC2 Vx = UnitVec2( l->RotT )     * l->UnitDist( 0.95*Radi );
   VEC2 Vy = UnitVec2( l->RotT+Off ) * l->UnitDist( 0.95*Radi );
   INT2 i;
   for( i = 1; i < nS; i++ ) ApplyStrip( &(Mp + Vx* i), &Vy,      1 );
   for( i = 1; i < nS; i++ ) ApplyStrip( &(Mp + Vx* i), &(Vy*-1), 0 );
   for( i = 0; i < nS; i++ ) ApplyStrip( &(Mp + Vx*-i), &Vy,      1 );
   for( i = 0; i < nS; i++ ) ApplyStrip( &(Mp + Vx*-i), &(Vy*-1), 0 );
}
void TSSTHR::ApplyStrip( pPNT2 p, pVEC2 v, INT2 b ) {
   for( INT2 i = b; i < nS; i++ ) Apply_( &(*p + *v * i) );
}

void TSSTHR::TesselHexa() {
   TessCol = HexCol;
   VEC2 UP = UnitVec2( l->RotT+HPI )         * l->UnitDist( 0.95*Radi ); // upward
   VEC2 DL = UnitVec2( l->RotT+HPI+TPI/3 )   * l->UnitDist( 0.95*Radi ); // down left
   VEC2 DR = UnitVec2( l->RotT+HPI+TPI*2/3 ) * l->UnitDist( 0.95*Radi ); // down right
   INT2 i;
   PNT2 P_, Pu, Pe;
   P_ = Mp + UP * -1;                       // make points upwards
   for( i = 1; i < nS; i++ ) {
      Pu = P_ + UP * (3*i-1); Apply_( &Pu );
      Pe = Pu + UP;           Apply_( &Pe );
      ApplyStrp2( &Pu, &Pe, &DR, &(DL * -1 ) );
      ApplyStrp2( &Pu, &Pe, &DL, &(DR * -1) );
   }
   P_ = Mp + UP;                             // make points downwards
   for( i = 1; i < nS; i++ ) {
      Pu = P_ + UP * (1-3*i); Apply_( &Pu );
      Pe = Pu + UP * -1;      Apply_( &Pe );
      ApplyStrp2( &Pu, &Pe, &(DL * -1), &DR );
      ApplyStrp2( &Pu, &Pe, &(DR * -1), &DL );
   }
}
void TSSTHR::ApplyStrp2( pPNT2 p, pPNT2 q, pVEC2 v1, pVEC2 v2 ) {
   for( INT2 j = 1; j < nS; j++ ) {
      Apply_( &(*p + *v1 * j + *v2 * (j-1)) );
      Apply_( &(*p + *v1 * j + *v2 * j    ) );
      Apply_( &(*q + *v2 * j + *v1 * (j-1)) );
      Apply_( &(*q + *v2 * j + *v1 * j    ) );
   }
}

void TSSTHR::Apply_( pPNT2 p ) {
   if( !InsideFrame(p) ) return;
   PNT2 P = *p;                          // copy p for perturbation
   Perturb( &P );                        // add perturbation of point
   if( Circ ) ApplyP( &P );
   else       ApplyV( &P );
}

#define  INSIDE   TRUE
#define  OUTSIDE  FALSE
BOOL TSSTHR::InsideFrame( pPNT2 p ) {
   INT2 i;
   switch( Frme ) {
      case CIRC_F:
         if( DubF ) {                    // -- double frame
            if( l == &T[0] ) {              // test outer ring
               if( Distan(p,&Mp) > Radi ) return OUTSIDE;
               if( Distan(p,&Mp) < (1+Wide)*RFac*Radi ) return OUTSIDE;
            }
            else if( Distan(p,&Mp) > (1-Wide)*RFac*Radi ) return OUTSIDE;
         }
         else if( Distan(p,&Mp) > Radi ) return OUTSIDE;
         break;
      default:
         if( DubF ) {                    // -- double frame
            if( l == &T[0] ) {
               for( i = 0; i < nF; i++ )    // outside outer frame
                  if( p->RelDis(&Fp[i],&Fp[(i+1)%nF]) < 0 ) return OUTSIDE;
               for( i = 0; i < nF; i++ )
                  if( p->RelDis(ParamP2(&Mp,&Fp[i],       (1+Wide)*RFac),
                                ParamP2(&Mp,&Fp[(i+1)%nF],(1+Wide)*RFac) ) < 0 )
                     return INSIDE;         // outside inner frame is inside
               return OUTSIDE;              // inside inner frame is outside
            }
            else for( i = 0; i < nF; i++ )
                    if( p->RelDis(ParamP2(&Mp,&Fp[i],       (1-Wide)*RFac),
                                  ParamP2(&Mp,&Fp[(i+1)%nF],(1-Wide)*RFac) ) < 0 )
                       return OUTSIDE;
         }
         else for( i = 0; i < nF; i++ )
                 if( p->RelDis(&Fp[i],&Fp[(i+1)%nF]) < 0 ) return OUTSIDE;
   }
   return INSIDE;
}
#undef  INSIDE
#undef  OUTSIDE

#define   PERTURB1    0.01         // 1% of perturbation
void TSSTHR::Perturb( pPNT2 p ) {     // perturbation of one point
   REAL Add = l->UnitDist( Radi ) * PERTURB1 * l->NPrt;
   p->X += Add * ( (REAL)rand() / RAND_MAX - 0.5 );
   p->Y += Add * ( (REAL)rand() / RAND_MAX - 0.5 );
}
#undef PERTURB1

void TSSTHR::ApplyP( pPNT2 p ) {
   if( l->tstOmittance() )     // test point for omittance
      DrawP( p, LWe );         // point is omitted
   else {
      l->nT++;
      DrawP( p, TessCol );
      if( Subm ) {
         pTISP ts = Get_Tisp( Prcs.T+DISPERSE*(Prcs.S+Prcs.X) );
         ts->Assign( p->X, p->Y );
         MakeComp( ts );
      }
   }
}
void TSSTHR::ApplyV( pPNT2 p ) {
   REAL Dir = MargVal( 0 ) * TPI;      // random direction
   REAL Dev = 0.1;                     // opening of V
   l->nT++;
   DrawV( p, Dir, TessCol );
   if( Subm ) MakeDoubleV( p, Dir, Dev, FALSE, FALSE );
}

void TSSTHR::DrawP( pPNT2 p, MYCOLOR Col ) {         // draw point
   INT2 x, y; SetPixel( p, x, y );
   if( l == t ) {                    // loop tess. equals attention tess.
      PutPixel( x+1, y,   Col );
      PutPixel( x,   y+1, Col );
      PutPixel( x-1, y,   Col );
      PutPixel( x,   y-1, Col );
   }
   PutPixel( x, y, Col );
}
void TSSTHR::DrawV( pPNT2 p, REAL D, MYCOLOR Col ) {         // draw point
   INT2 x, y; SetPixel( p, x, y );
   if( l == t ) {                    // loop tess. equals attention tess.
           if( D < _PI   /8 ) PutPixel( x+1, y,   LWe );
      else if( D < _PI* 3/8 ) PutPixel( x+1, y+1, LWe );
      else if( D < _PI* 5/8 ) PutPixel( x,   y+1, LWe );
      else if( D < _PI* 7/8 ) PutPixel( x-1, y+1, LWe );
      else if( D < _PI* 9/8 ) PutPixel( x-1, y,   LWe );
      else if( D < _PI*11/8 ) PutPixel( x-1, y-1, LWe );
      else if( D < _PI*13/8 ) PutPixel( x,   y-1, LWe );
      else if( D < _PI*15/8 ) PutPixel( x+1, y-1, LWe );
      else                    PutPixel( x+1, y,   LWe );
   }
   PutPixel( x, y, Col );
}

void TSSTHR::DrawFrame() {
   INT2 i;
   if( DubF ) {
      MYCOLOR C0 = ( IxT == 0 ) ? AttCol : NegCol;
      MYCOLOR C1 = ( IxT == 1 ) ? AttCol : NegCol;
      if( Frme == CIRC_F ) {                           // draw circle
         SetColor( C0 ); DrawC( &Mp, Radi, 120 );      // big
         SetColor( C1 ); DrawC( &Mp, RFac*Radi, 70 );  // small
      }
      else {                                           // draw other frames
         SetColor( C0 );                               // outer one
         MoveTo( &Fp[nF-1] );
         for( i = 0; i < nF; i++ ) LineTo( &Fp[i] );
         SetColor( C1 );                               // inner one
         MoveTo( ParamP2(&Mp,&Fp[nF-1],RFac) );
         for( i = 0; i < nF; i++ ) LineTo( ParamP2(&Mp,&Fp[i],RFac) );
      }
   }
   else {
      SetColor( AttCol );
      if( Frme == CIRC_F ) DrawC( &Mp, Radi, 120 );    // draw circle
      else {                                           // draw other frames
         MoveTo( &Fp[nF-1] );
         for( i = 0; i < nF; i++ ) LineTo( &Fp[i] );
      }
   }
}

void TSSTHR::DrawC( pPNT2 p, REAL R, INT2 n ) {    // draw circle
   MoveTo( &(*p + UnitVec2(0) * R) );
   for( INT2 i = 1; i <= n; i++ ) LineTo( &(*p + UnitVec2(TPI*i/n) * R) );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - RDMTHR
BOOL RDMTHR::Parse( INT2 c ) {           // -- throw a random configuration
   switch( c ) {
      case THR_RDM___1: ThrowIt( 1 );   return TRUE;  // '1': one circle/triangle
      case THR_RDM___2: ThrowIt( 2 );   return TRUE;  // '2': .. or 2
      case THR_RDM___5: ThrowIt( 5 );   return TRUE;  // '3': .. or 5
      case THR_RDM__10: ThrowIt( 10 );  return TRUE;  // '4': .. or 10
      case THR_RDM__20: ThrowIt( 20 );  return TRUE;  // '5': .. or 20
      case THR_RDM__50: ThrowIt( 50 );  return TRUE;  // '6': .. or 50
      case THR_RDM_100: ThrowIt( 100 ); return TRUE;  // '7': .. or 100
      case THR_RDM_200: ThrowIt( 200 ); return TRUE;  // '8': .. or 200
      case THR_RDM_500: ThrowIt( 500 ); return TRUE;  // '9': .. or 500

      case THROW_WHICH:         // '*': toggle: circles/triangles throw
         What = ( What == CTBOTH ) ? CIRCLE : (BZTYP)(What+1);
              if( What == CTBOTH ) Message( L"Throw both!" );
         else if( What == CIRCLE ) Message( L"Throw circles only . . o O !" );
         else if( What == TRIANG ) Message( L"Throw triangles only <| <| !" );
         return TRUE;
      case OPEN_FOLDED:         // '^': toggle: open or folded triangles
         Acut = !Acut;
         if( !Acut && nA > 3 ) nA = 3;
         if( Acut ) Message( L"Triangles are plain!" );
         else       Message( L"Triangles are folded! (triangles only)" );
         return TRUE;
      case DECR_CORNER:         // '(': polygons other than triangles (tame type only)
         if( nA > 3 ) nA--;
         Message( L"N of angles is %d !", nA );
         return TRUE;
      case INCR_CORNER:         // ')':
         if( Acut )
            if( nA < XARCS ) nA++;
         Message( L"N of angles is %d !", nA );
         return TRUE;
   }
   if( RandPars(c) ) {
      srand( rBeg );
      Message( L"Start random number generator is %u", rBeg );
      return TRUE;
   }
   return FALSE;                // no response
}

BOOL RDMTHR::RandPars( INT2 c ) {                         // -- random start numbers
   switch( c ) {
      case INCR_RANDOM: rBeg++; return TRUE;                 // '>': increase
      case DECR_RANDOM: if( rBeg > 0u ) rBeg--; return TRUE; // '<': decrease
      case REST_RANDOM: return TRUE;                         // '`': refresh only
   }
   return FALSE;                // no response
}

void RDMTHR::ThrowIt( INT2 N ) {
   midP = CadreMidPnt();                // get position of domain midpoint
   Xfac = HalfRangeX() / 2;             // get dispersion factors
   Yfac = HalfRangeY() / 2;
   if ( What == CIRCLE || What == CTBOTH ) {        // throw circles
      REAL T = Prcs.T + ( Prcs.S + Prcs.X ) * DISPERSE * ( N + 1 );
      for( INT2 I = 0; I < N; I++, T -= DISPERSE * ( Prcs.S + Prcs.X ) ) {
         MargVal( 0 );                  // ## test only
         MakeComp( Point(T) );
      }
   }
   if ( What == TRIANG || What == CTBOTH ) {        // throw triangles
      REAL T = Prcs.T + ( Prcs.S + Prcs.X ) * DISPERSE * ( N + 1 );
      for( INT2 I = 0; I < N; I++, T -= DISPERSE * ( Prcs.S + Prcs.X ) ) {
         Angles( nA, Arcs );
         MakePoly( nA, Arcs, Point(T) );
      }
   }
}

pTISP RDMTHR::Point( REAL T ) {
   pTISP ts = Get_Tisp( T );                        // new time-space
   ts->X = midP->X + Xfac * ( 2 * MargVal(0) - 1 ); // MargVal() from [-1,+1]
   ts->Y = midP->Y + Yfac * ( 2 * MargVal(0) - 1 );
   return ts;
}

//void RDMTHR::Angles( INT2 n, pENWS a ) {  // generate random directions
void RDMTHR::Angles( INT2 n, pREAL a ) {  // generate random directions
   INT2 i;
   REAL ab, ae;
   ab = MargVal( 0 ) * TPI;               // first direction arbitrary
   a[0] = ab;
   if( Acut ) {                           // ! no folded angles
      ae = ab + MargVal( Marg ) * _PI;    // second direction within PI-arc
      a[1] = ae;
      REAL Max_a, Min_a;
      for( i = 2; i < n-1; i++ ) {
         Max_a = ( ae+_PI < ab+TPI ) ? (ae+_PI) : (ab+TPI);
         ae = ae + MargVal( Marg ) * ( Max_a - ae );
         a[i] = ae;
      }
      Min_a = ( ae > ab+_PI ) ? ae : (ab+_PI);
      Max_a = ( ae+_PI < ab+TPI ) ? (ae+_PI) : (ab+TPI);
      a[i] = Min_a + MargVal( Marg ) * ( Max_a - Min_a );
   }
   else for( i = 1; i < n; i++ )          // free turn around
           a[i] = ab + MargVal( Marg ) * TPI;
}

// ------------------------------------------------------------------------- MYFILE
#include <string.h>                  // strchr()

BOOL MYFILE::Open( pCHAR m ) {
   f = FOPEN( n, m );
   if( *m == 'r' )
      if( f == NULL ) {
         Message( L"file to read %s does not exist", n );
         return FALSE;
      }
   return TRUE;
}

BOOL MYFILE::iniTGClip() {
   FGETS( TG, XTAG+1, f );
   BOOL ir = !feof( f );
   if( ir ) tab_number();
   return ir;
}

void MYFILE::tab_number() {
   INT2 k = 0;                    // number of tabulators in string
   for( pCHAR s = TG; ; s++ ) {
      s = STRCHR( s, TABUL );
      if( s == NULL ) break;
      k++;
   }
   nTab = k;
   b = TG;
}

pCHAR MYFILE::getTGClip() {
   if( (e=STRCHR(b,TABUL)) == NULL ) e = STRCHR( b, CR_LF );
   *e = E_STR;
   return b;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - FILTHR
BOOL FILTHR::Parse( INT2 c ) {           // -- throw a configuration from file
   switch( c ) {
      // -- throw file data
      case THR_FILE:                        // ';': 
         for( ; ; ) {
            // MT: refactored QuickMs( _T("give file name") );
            GETS( _T("give file name"), n, sizeof(filpath) - 1 );
            if( STRLEN(n) == 0 ) break;

            if( Open(_T("r")) ) {
               Message( L"picked up file name %s", n );
               put_closures();
               break;
            }
         }
                                            // cancel file data throw (esc)
         return TRUE;
   }
   return FALSE;                            // no response
}

#define NCIRTAB  2         // number of tabs in circle-records
#define NTRITAB  5         // number of tabs in triangle-records

void FILTHR::put_closures() {
   for( nC = 0, nT = 0; iniTGClip(); ) {
      if( nTab == NCIRTAB ) {                      // circles without time

         REAL x = (REAL)ATOF( getTGClip() ); updTGClip();
         REAL y = (REAL)ATOF( getTGClip() ); updTGClip();

         REAL t = (REAL)ATOF( getTGClip() ); updTGClip();
         REAL T = Prcs.T + ( Prcs.S + Prcs.X ) * MIN_DISP;

         pTISP ts = Get_Tisp( T+t );                        // new time-space
         ts->Assign( x, y );

         MakeComp( ts );
         nC++;
      }
      else if( nTab == NTRITAB ) {                // triangles without or with time

         REAL x = (REAL)ATOF( getTGClip() ); updTGClip();
         REAL y = (REAL)ATOF( getTGClip() ); updTGClip();

         REAL Dir[3];
         Dir[0] = arcF( _atold( getTGClip() ) ); updTGClip();
         Dir[1] = arcF( _atold( getTGClip() ) ); updTGClip();
         Dir[2] = arcF( _atold( getTGClip() ) ); updTGClip();

         REAL t = (REAL)ATOF( getTGClip() ); updTGClip();
         REAL T = Prcs.T + ( Prcs.S + Prcs.X ) * MIN_DISP;

         pTISP ts = Get_Tisp( T+t );                        // new time-space
         ts->Assign( x, y );

         MakePoly( 3, Dir, ts );
         nT++;
      }
      else Exit( L"bad record in throw file" );
   }
   Message( L"Retrieved %d cirkels, %d triangles", nC, nT );
}

#undef DISPERSE
/*
void FILTHR::set_minmax() {
   pCHAR b1, b2, e1;

   for( nC = 0, nT = 0; ; ) {
      fgets( TG, XTAG+1, f );
      if( feof(f) ) break;

      INT2 nTab = tab_number( TG );
      if( nTab == 1 ) {                       // circles without time
         b1 = TG;
         e1 = strchr( b1, TABUL );
         b2 = e1 + 1;
         e1 = E_STR;
         REAL mx = (REAL)ATOF( b1 );
         REAL my = (REAL)ATOF( b2 );
         upd_minmax( mx, my );
         nC++;
      }
      else if( nTab == 4 ) {                  // triangles without or with time
         b1 = TG;
         e1 = strchr( TG, TABUL );
         b2 = e1 + 1;
         e1 = E_STR;
         REAL mx = (REAL)ATOF( b1 );
         REAL my = (REAL)ATOF( b2 );
         upd_minmax( mx, my );
         nT++;
      }
      else Exit( L"bad record in throw file" );
   }

   // bring ranges fully (factor 1.05) into a y/x-ratio of screen (~ 0.7)
   REAL s = 0.7;                        // approximite screen ratio
   REAL z = 1.05;                       // zoom factor
   REAL r = ( yx - yi ) / ( xx - xi );  // actual y/x-ratio
   REAL md, rg;
   if( r > s ) {
      md = ( xx + xi ) / 2;             // midposition of range
      rg = ( xx - xi ) / 2;             // half length of range
      xi = md - rg * z * r / s; 
      xx = md + rg * z * r / s; 
   }
   else {
      md = ( yx + yi ) / 2;             // midposition of range
      rg = ( yx - yi ) / 2;             // half length of range
      yi = md - rg * z * r / s; 
      yx = md + rg * z * r / s; 
   }
}

void FILTHR::upd_minmax( REAL x, REAL y ) {
   if( nC+nT == 0 ) {
      xi = xx = x;
      yi = yx = y;
   }
   else {
           if( xi > x ) xi = x;
      else if( xx < x ) xx = x;
           if( yi > y ) yi = y;
      else if( yx < y ) yx = y;
   }
}
*/
