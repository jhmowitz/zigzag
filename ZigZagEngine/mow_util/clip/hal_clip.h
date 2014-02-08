/* hal_clip.h
 *            -- clipping of points/lines/circles against a (REAL) cadre with a halo
 *               providing the following facilities
 * - Inner: test a point inside the cadre
 * - Shear: test two outside points to have a piece inside the cadre (or not)
 *          and provide the two cutting parameters with the cadre
 * - Enter: entering from the point outside to the point inside
 *          and providing the cutting parameters with the cadre
 */

#ifndef _HAL_CLIP_H
#define _HAL_CLIP_H

#include CIR_CLIP_H     // definition for clipping circles/arcs against a cadre

// -- class to provide functions for clipping with a cadre enlarged by a halo
class LINHCLIP: public CIR_CLIP {
   REAL Halo;      // given halo to enlarge cadre from CADRE (for points/lines)
   PNT2 CClipP[4]; // points derived from CADRE and enlarged by a halo to clip circles
public:
   LINHCLIP(): CIR_CLIP( CClipP ) {}  // give enlarged cadre to circle clip facility

   void setHalo( REAL h ) { Halo = h; }              // set Halo
   void setCClipP( REAL h ) {
      CClipP[0].X = CClipP[3].X = LLCor()->X - h;    // minimum X
      CClipP[1].X = CClipP[2].X = URCor()->X + h;    // maximum X
      CClipP[0].Y = CClipP[1].Y = LLCor()->Y - h;    // minimum Y
      CClipP[2].Y = CClipP[3].Y = URCor()->Y + h;    // maximum Y
   }
   // handle clipping
   BOOL Inner( pPNT2, REAL );                        // test inside cadre
   BOOL Shear( pPNT2, pPNT2, REAL, REAL &, REAL & ); // both points outside the cadre
   REAL Enter( pPNT2, pPNT2, REAL );                 // enter cadre
   // provide access to ..
   REAL HLwX() { return LowX() - Halo; }    // minimum X
   REAL HHgX() { return HigX() + Halo; }    // maximum X
   REAL HLwY() { return LowY() - Halo; }    // minimum Y
   REAL HHgY() { return HigY() + Halo; }    // maximum Y
};

#ifdef _HAL_CLIP_CPP            // ! include sources

BOOL LINHCLIP::Inner( pPNT2 p, REAL h ) {
   Halo = h;
   return (BOOL)( HLwX() < p->X && p->X < HHgX() &&
                  HLwY() < p->Y && p->Y < HHgY() );
}

#define   OUT_OF_CADRE   FALSE
#define   INSIDE_CADRE   TRUE
BOOL LINHCLIP::Shear( pPNT2 p, pPNT2 q, REAL h, REAL &Low, REAL &Hig ) { // p, q outside
   Halo = h;
   // simple cases when the cadre is not touched
   if( p->X < HLwX() && q->X < HLwX() ) return OUT_OF_CADRE;
   if( p->X > HHgX() && q->X > HHgX() ) return OUT_OF_CADRE;
   if( p->Y < HLwY() && q->Y < HLwY() ) return OUT_OF_CADRE;
   if( p->Y > HHgY() && q->Y > HHgY() ) return OUT_OF_CADRE;

   Low = 0;          // initial clip parameter for low clipping on x-coordiante
   Hig = 1;          // initial clip parameter for high clipping on x-coordinate
   if( p->X < HLwX() || HHgX() < p->X || q->X < HLwX() || HHgX() < q->X ) {
      REAL pl = ( HLwX() - p->X ) / ( q->X - p->X ),
           ph = ( HHgX() - p->X ) / ( q->X - p->X );
      if( pl < ph ) {
         if( pl > 0 ) Low = pl;
         if( ph < 1 ) Hig = ph;
      }
      else {
         if( ph > 0 ) Low = ph;
         if( pl < 1 ) Hig = pl;
      }
   }

   REAL Ylw = 0,      // initial clip parameter for low clipping on y-coordinate
        Yhg = 1;      // initial clip parameter for high clipping on y-coordinate
   if( p->Y < HLwY() || HHgY() < p->Y || q->Y < HLwY() || HHgY() < q->Y ) {
      REAL pl = ( HLwY() - p->Y ) / ( q->Y - p->Y ),
           ph = ( HHgY() - p->Y ) / ( q->Y - p->Y );
      if( pl < ph ) {
         if( pl > 0 ) Ylw = pl;
         if( ph < 1 ) Yhg = ph;
      }
      else {
         if( ph > 0 ) Ylw = ph;
         if( pl < 1 ) Yhg = pl;
      }
   }

   if( Low < Ylw ) Low = Ylw;       // take highest of low parameters
   if( Hig > Yhg ) Hig = Yhg;       // take lowest of high parameters
   if( Low >= Hig )                 // the line does not scrape the cadre
      return OUT_OF_CADRE;          // line and cadre are distinct
   return INSIDE_CADRE;             // the line cuts the cadre
}
#undef   OUT_OF_CADRE
#undef   INSIDE_CADRE

REAL LINHCLIP::Enter( pPNT2 o, pPNT2 i, REAL h ) {  // i is inside, o is outside
   Halo = h;
   REAL ParX = 0;                             // find entering parameter for X ( > 0 ) 
        if( o->X < HLwX() ) ParX = ( HLwX() - o->X ) / ( i->X - o->X );
   else if( o->X > HHgX() ) ParX = ( HHgX() - o->X ) / ( i->X - o->X );
   REAL ParY = 0;                             // find entering parameter for Y ( > 0 )
        if( o->Y < HLwY() ) ParY = ( HLwY() - o->Y ) / ( i->Y - o->Y );
   else if( o->Y > HHgY() ) ParY = ( HHgY() - o->Y ) / ( i->Y - o->Y );
   if( ParX < ParY ) return ParY;             // take larger of low parameters
   return ParX;
}

#endif          // _HAL_CLIP_CPP .. end of including sources
#endif          // _HAL_CLIP_H
