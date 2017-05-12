/* lin_clip.h
 *            -- clipping of points/lines against a (REAL) cadre 
 *               providing some facilities
 * - Inner: test point inside the cadre
 * - Shear: test two outside points to have a piece inside the cadre (or not)
 *          and provide the two cutting parameters with the cadre
 * - Enter: entering from one point outside to one point inside
 *          and providing the cutting parameter with the cadre
 */

#ifndef _LIN_CLIP_H
#define _LIN_CLIP_H

#include CIR_CLIP_H     // definition for clipping circles/arcs against a cadre

// -- class to provide functions for clipping points, lines and circles/arcs
class LIN_CLIP: public CIR_CLIP {
public:
   LIN_CLIP(): CIR_CLIP() {}   // initial circle clipping with underlying cadre points

   BOOL Inner( pPNT2 );                        // test inside cadre
   BOOL Shear( pPNT2, pPNT2, REAL &, REAL & ); // both points outside the cadre
   REAL Enter( pPNT2, pPNT2 );                 // enter cadre
};

#ifdef _MOW_CLIP_CPP                // ! include sources maar niet heus

BOOL LIN_CLIP::Inner( pPNT2 p ) {   // test inside cadre
   return (BOOL)( LowX() < p->X && p->X < HigX() &&
                  LowY() < p->Y && p->Y < HigY() );
}

#define   OUT_OF_CADRE   FALSE
#define   INSIDE_CADRE   TRUE
BOOL LIN_CLIP::Shear( pPNT2 p, pPNT2 q, REAL &Low, REAL &Hig ) { // p and q outside
   // simple cases when the cadre is not touched
   if( p->X < LowX() && q->X < LowX() ) return OUT_OF_CADRE;
   if( p->X > HigX() && q->X > HigX() ) return OUT_OF_CADRE;
   if( p->Y < LowY() && q->Y < LowY() ) return OUT_OF_CADRE;
   if( p->Y > HigY() && q->Y > HigY() ) return OUT_OF_CADRE;

   Low = 0;          // initial clip parameter for low clipping on x-coordiante
   Hig = 1;          // initial clip parameter for high clipping on x-coordinate
   if( p->X < LowX() || HigX() < p->X || q->X < LowX() || HigX() < q->X ) {
      REAL pl = ( LowX() - p->X ) / ( q->X - p->X ),
           ph = ( HigX() - p->X ) / ( q->X - p->X );
      if( pl < ph ) {
         if( pl > 0 ) Low = pl;
         if( ph < 1 ) Hig = ph;
      }
      else {
         if( ph > 0 ) Low = ph;
         if( pl < 1 ) Hig = pl;
      }
   }

   REAL Ylw = 0,     // initial clip parameter for low clipping on y-coordinate
        Yhg = 1;     // initial clip parameter for high clipping on y-coordinate
   if( p->Y < LowY() || HigY() < p->Y || q->Y < LowY() || HigY() < q->Y ) {
      REAL pl = ( LowY() - p->Y ) / ( q->Y - p->Y ),
           ph = ( HigY() - p->Y ) / ( q->Y - p->Y );
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
   if( Low >= Hig )                 // the line does not touch the cadre
      return OUT_OF_CADRE;          // line and cadre are distinct
   return INSIDE_CADRE;             // the line cuts the cadre
}
#undef   OUT_OF_CADRE
#undef   INSIDE_CADRE

REAL LIN_CLIP::Enter( pPNT2 o, pPNT2 i ) {    // i is inside, o is outside
   REAL ParX = 0;                             // find entering parameter for X ( > 0 ) 
        if( o->X < LowX() ) ParX = ( LowX() - o->X ) / ( i->X - o->X );
   else if( o->X > HigX() ) ParX = ( HigX() - o->X ) / ( i->X - o->X );
   REAL ParY = 0;                             // find entering parameter for Y ( > 0 )
        if( o->Y < LowY() ) ParY = ( LowY() - o->Y ) / ( i->Y - o->Y );
   else if( o->Y > HigY() ) ParY = ( HigY() - o->Y ) / ( i->Y - o->Y );
   if( ParX < ParY ) return ParY;             // take larger of low parameters
   return ParX;
}

#endif          // _MOW_CLIP_CPP .. end of including sources
#endif          // _LIN_CLIP_H
