/* cir_clip.h -- sources for clipping circles/arcs
 *               against a cadre of initializing corner points
 *               to indicate a system clipping facility such as:
 *                - the screen together with viewport clipping
 *                - a PostScript-cadre with the clipping of a printer
 *
 *        the incoming circle/arc is clipped roughly, but closely around
 *        the cadre to prevent the clipping facility of the system from overflow.
 *        The system facility then is used for precise clipping!
 */

#ifndef _CIR_CLIP_H
#define _CIR_CLIP_H

#include MOW_CADRE_H    // definition of a general cadre

class CIR_CLIP: public CADRE {
public:
   CIR_CLIP() { C = CADRE::CdrPts(); }          // initial with underlying points of CADRE
   CIR_CLIP( pPNT2 cdp ) { C = cdp; }           // initial with outside points (halo clipping)

   BOOL ClipArc( pPNT2 m, REAL R ) {
      Part = FALSE;                             // handling a full circle
      ValidArcs( m, R );                        // compute (roughly) arc(s) to be drawn
      return (BOOL)( nArc > 0 );                // there is something to draw (or not)
   }

   BOOL ClipArc( pPNT2 m, REAL R, REAL L, REAL H ) {
      Part = TRUE;                              // handling an arc
      LReq = L;                                 // low and ..
      HReq = H;                                 // high extension of requested arc
      ValidArcs( m, R );                        // compute (roughly) arc(s) to be drawn
      return (BOOL)( nArc > 0 );                // there is something to draw (or not)
   }

   pPNT2 mdP() { return cirM; }                 // midpoint of requested circle
   REAL  Rad() { return cirR; }                 // radius dito

   INT2 nArcs() { return nArc; }                // number of arcs to draw: 0, 1 or 2
   REAL LowBound( INT2 i ) { return LowB[i]; }  // low boundary for arc(s) to draw
   REAL HigBound( INT2 i ) { return HigB[i]; }  // high boundary for arc(s) to draw

private:
   pPNT2 C;                                     // address to active cadre pts.
                                             // -- addresses to relative indexed cadre pts.
   pPNT2 curr( INT2 i ) { return &C[i]; }       // nearest
   pPNT2 rnxt( INT2 i ) { return &C[(i+1)%4]; } // next at right hand
   pPNT2 opps( INT2 i ) { return &C[(i+2)%4]; } // opposite (farest)
   pPNT2 lnxt( INT2 i ) { return &C[(i+3)%4]; } // next at left hand
                                             // -- requested circle/arc
   pPNT2 cirM;                                  // midpoint of the circle
   REAL  cirR;                                  // its radius
   BOOL  Part;                                  // handling a full circle or an arc
   REAL  LReq, HReq;                            // extensions of arc requested to draw
                                             // -- output indication
   INT2 nArc;                                   // number of arcs to draw: none, one or two
   REAL LowB[2], HigB[2];                       // low, high boundary for arc(s) to draw

   // compute (roughly) from full circle one arc to draw ..
   // or (roughly) from one arc one or two pieces of arc to draw
   void ValidArcs( pPNT2, REAL );               // compute (roughly) arc(s) to draw
   void doInside();                             // midpoint of circle is in cadre
   void doNeighb( INT2 );                       // midpoint is outside in a side area
   void doCorner( INT2 );                       // midpoint is outside in a corner area
   void setArc1( pPNT2, pPNT2 );                // set first (rough) arc
   void splitarc();                             // handling an arc
};

#ifdef _MOW_CLIP_CPP

/*
 * Validate a full circle or an arc, that is to be drawn.
 *   If its midpoint is outside the cadre, its situation is
 *   formally described by the index of a cadre point:
 *   - in a corner    - by the index of the nearest cadre point.
 *   - in a side area - by the index of the cadre point nearest to the left hand.
 */

// -- compute clipping of arc(s) (roughly) against the cadre
void CIR_CLIP::ValidArcs( pPNT2 m, REAL R ) {

   // cadre point indices indicating the position of a midpoint outside the cadre
   INT2 LeftUpper = 3, MediUpper = 2, RightUpper = 2,
        LeftMedi  = 3,                RightMedi  = 1,
        LeftDown  = 0, MediDown  = 0, RightDown  = 1;

   cirM = m;                 // keep arc's midpoint
   cirR = R;                 // .. and radius
   nArc = 0;                 // number of arcs to draw

   if( cirM->X <= C[0].X-cirR || cirM->X >= C[2].X+cirR || // circle is too far out
       cirM->Y <= C[0].Y-cirR || cirM->Y >= C[2].Y+cirR ) return;

   if( cirM->X < C[0].X ) {
           if( cirM->Y < C[0].Y ) doCorner( LeftDown );    // cirM is in a corner
      else if( cirM->Y > C[2].Y ) doCorner( LeftUpper );   // cirM is in a corner
      else                        doNeighb( LeftMedi );    // cirM is lateral
   }
   else if( cirM->X > C[2].X ) {
           if( cirM->Y < C[0].Y ) doCorner( RightDown );   // cirM is in a corner
      else if( cirM->Y > C[2].Y ) doCorner( RightUpper );  // cirM is in a corner
      else                        doNeighb( RightMedi );   // cirM is lateral
   }
   else if( cirM->Y < C[0].Y )    doNeighb( MediDown );    // cirM is lateral
   else if( cirM->Y > C[2].Y )    doNeighb( MediUpper );   // cirM is lateral
   else                           doInside();              // cirM is in cadre
}

void CIR_CLIP::doInside() {              // -- midpoint is in cadre
   for( INT2 i = 0; i < 4; i++ )            // ## voorlopig geen verdere evaluatie
      if( Distan(cirM,&C[i]) > cirR ) {     // draw full circle or arc, if at least one
         nArc = 1;                          // cadre point is outside the circle
         LowB[0] = Part ? LReq : 0;
         HigB[0] = Part ? HReq : TPI;
         return;
      }
}
                                         // -- midpoint m is in a neighboring region
void CIR_CLIP::doNeighb( INT2 Lnear ) {
   pPNT2 ln = curr( Lnear ),                // point at left hand nearest to cirM
         rn = rnxt( Lnear ),                // dito right/near to cirM
         rf = opps( Lnear ),                // dito right/far to cirM
         lf = lnxt( Lnear );                // dito left/far to cirM

   if( Distan(cirM,lf) <= cirR )
      if( Distan(cirM,rf) <= cirR )   return;            // circle is too big
      else                            setArc1( rn, lf ); // arc (rn,lf) limit at left hand
   else if( Distan(cirM,rf) <= cirR ) setArc1( rf, ln ); // arc (rf,ln) limit at right hand
   else                               setArc1( rn, ln ); // maximal arc (rn,ln)
}

void CIR_CLIP::doCorner( INT2 N ) {          // -- cirM is in the corner of index N(ear)
   if( Distan(cirM,curr(N)) >= cirR ) return;   // circle is too small
   if( Distan(cirM,opps(N)) <= cirR ) return;   // circle is too big
   setArc1( rnxt(N), lnxt(N) );                 // set first (rough) arc
}

void CIR_CLIP::setArc1( pPNT2 r, pPNT2 l ) { // -- set first (rough) arc
   nArc = 1;                                    // one arc to draw (roughly)
   LowB[0] = SlopeX( cirM, r );                 // from direction of point at right hand r
   HigB[0] = OrHig( LowB[0], SlopeX(cirM,l) );  // to direction of point at left hand l
   if( Part ) splitarc();                       // look for arc-splitting
}

void CIR_CLIP::splitarc() {                  // -- look for arc-splitting
   LowB[0] = OrHig( LReq, LowB[0] );            // bring rougly drawable domain into the
   HigB[0] = OrHig( LowB[0], HigB[0] );         // same angular disk as the requested domain
   // -- denote interval of angles by: (low,high)
   if( HigB[0] <= HReq ) return;                // COMPRISING: (LowB,HigB)
   else if( LowB[0] < HReq ) {
      if( HigB[0] <= LReq+TPI )                 // HIGHERPART: (LowB,HReq)
         HigB[0] = HReq;
      else {                                    // SPLITSING:  (LowB,HReq), (LReq,HigB)
         nArc = 2;
         HigB[1] = HigB[0];
         LowB[1] = LReq + TPI;
         HigB[0] = HReq;
      }
   }
   else {                                    // -- LowB[0] >= HReq
      if( HigB[0] <= LReq+TPI )                 // DISJUNCT:  no visible arc
         nArc = 0;
      else if( HigB[0] <= HReq+TPI )            // LOWERPART: (LReq+TPI,HigB)
         LowB[0] = LReq + TPI;
                                             // -- HigB[0] > HReq+TPI
      else {                                    // INSIDE:    (LReq,HReq)
         LowB[0] = LReq;
         HigB[0] = HReq;
      }
   }
}

#endif                             // _MOW_CLIP_CPP
#endif                             // _CIR_CLIP_H

/*
 * ## to be worked out
                                  //           |            |
                                  //  CORNISH  |  BELOWISH  |  CORNISH
                                  //           |            |
#define   CORNISH    -1           // ----------|------------|-----------
#define   INSIDISH    0           //           |            |
#define   LEFTISH     1           //  LEFTISH  |  INSIDISH  |  RIGHTISH
#define   BELOWISH    2           //           |            |
#define   RIGHTISH    3           // ----------|------------|-----------
#define   UPPERISH    4           //           |            |
                                  //  CORNISH  |  UPPERISH  |  CORNISH
                                  //           |            |

INT2 VDOM::WhereIsPoint( pPNT2 p ) {
   INT2 W[2], n = 0;
        if( p->X < LowX ) W[n++] = LEFTISH;
   else if( p->X > HigX ) W[n++] = RIGHTISH;
        if( p->Y < LowY ) W[n++] = BELOWISH;
   else if( p->Y > HigY ) W[n++] = UPPERISH;
   if( n == 0 ) return INSIDISH;
   if( n >  1 ) return CORNISH;
   return W[0];
}
*/
