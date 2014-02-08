/*
 * alg_geo.cpp -- basic geometric functions
 *                distance, slope, parameter point, incidence of lines,
 *                relative parameter, relative distance and others
 *                operators between points and vectors
 */

#include <math.h>                    // ABS(), ACOS(), ASIN(), COS(), SIN(), SQRT()

#include "mow_defs.h"                // incriptions for includes (use long double) etc.
#include "alg_geo.h"                 // structures for points, lines and circles

#include "prc_main.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - handle 2D-Points
// sum of squares of 2D-points
REAL SumSqr( pPNT2 p, pPNT2 q ) { return ( *p - *q ) * ( *p - *q ); }

// computes distance of 2D-points
REAL Distan( pPNT2 p, pPNT2 q ) { return SQRT( ( *p - *q ) * ( *p - *q ) ); }

// angle between positive x-axis and line -*p--*q->
REAL SlopeX( pPNT2 p, pPNT2 q ) {
   if( p->X == q->X && p->Y == q->Y ) Exit( L"SlopeX() -- equal points" );
   REAL Dis = ACOS( ( q->X - p->X ) / Distan( p, q ) );
   if ( p->Y > q->Y ) Dis = TPI - Dis;
   return Dis;
}

// compute point of parameter relative to p, q. ( Par == 0 -> p, Par == 1 -> q )
pPNT2 ParamP2( pPNT2 p, pPNT2 q, REAL Par ) {
   static INT2 W = 1;             // keep two points, used alternatively
   static PNT2 P[2];
               
   P[W = 1 - W] = *p + ( *q - *p ) * Par;

   return &P[W];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - PNT2 -- class functions
// incidence of two lines as directed points (used by CIRC::outCir() only)
void PNT2::IncidP( pPNT2 p, REAL Dir_p, pPNT2 q, REAL Dir_q ) {
   PNT2 P = *p + UnitVec2( Dir_p );
   PNT2 Q = *q + UnitVec2( Dir_q );
   REAL DisP = p->RelDis( q, &Q ),         // relative signed distance of p to (q,Q)
        DisQ = P.RelDis( q, &Q );          // relative signed distance of P to (q,Q)
   if( DisP == DisQ ) Exit( L"IncidP( (p,Dir_p) by (q,Dir_q) ) -- unreliable point" );
   *this = *p + UnitVec2( Dir_p ) * ( DisP/(DisP-DisQ) );  // parameter point on line (p,P)
}

// p is projected onto a line with a parameter: 0 (p), > 0 (q) [SumSqr(p,q)]
REAL PNT2::RelPar( pPNT2 p, pPNT2 q ) { return ( *this - *p ) * ( *q - *p ); }

// signed distance point to directed line
REAL PNT2::RelDis( pPNT2 p, pPNT2 q ) { return ( *this | *p ) * ( *q - *p ); }

void PNT2::IncidP( pLIN2 l1, pLIN2 l2 ) {  // compute incidence of two lines
   REAL T = l1->Tail.RelDis( l2 ),         // relative signed distance l1->Tail to l2
        H = l1->Head.RelDis( l2 );         // relative signed distance l1->Head to l2
   if( T == H ) Exit( L"IncidP( line by line ) -- unreliable point" );
   *this = l1->Tail + ( l1->Head - l1->Tail ) * ( T/(T-H) );  // parameter point
}

// -- compute value relative to a directed line
   // relative parameter
REAL PNT2::RelPar( pLIN2 l ) { return ( *this - l->Tail ) * ( l->Head - l->Tail ); }

   // signed distance of a point (relative to line length)
REAL PNT2::RelDis( pLIN2 l ) { return ( *this | l->Tail ) * ( l->Head - l->Tail ); }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - operators of points and vectors
PNT2 operator+( PNT2 &P, VEC2 &V ) { // point = point + vector
   static PNT2 Q;
   Q.X = P.X + V.X;
   Q.Y = P.Y + V.Y; return Q;
}
VEC2 operator-( PNT2 &P, PNT2 &Q ) { // vector = point - point
   static INT2 W = 1;
   static VEC2 D[2];
   W = 1 - W;
   D[W].X = P.X - Q.X;
   D[W].Y = P.Y - Q.Y; return D[W];
}
VEC2 operator|( PNT2 &P, PNT2 &Q ) { // vector = clockwise orthogonal of the difference Q - P
   static VEC2 O;
   O.X = P.Y - Q.Y;
   O.Y = Q.X - P.X; return O;
}
VEC2 operator+( VEC2 &V, VEC2 &W ) { // vector = vector + vector
   static VEC2 S;
   S.X = V.X + W.X;
   S.Y = V.Y + W.Y; return S;
}
VEC2 operator*( VEC2 &V, REAL r ) {  // vector = vector * real
   static VEC2 P;
   P.X = V.X * r;
   P.Y = V.Y * r; return P;
}
REAL operator*( VEC2 &V, VEC2 &W ) { // real = vector * vector
   static REAL r;
   return r = V.X * W.X + V.Y * W.Y;
}
VEC2 UnitVec2( REAL Dir ) {          // vector = unit vector in a given direction
   static VEC2 V;
   V.X = COS( Dir );
   V.Y = SIN( Dir ); return V;
}

// --------------------------------------------------------------------- circles - CIRC
void CIRC::outCir( pPNT2 p1, pPNT2 p2, pPNT2 p3 ) {     // outer circle of three points
   p->IncidP( ParamP2(p1,p2,0.5), SlopeX(p1,p2)+HPI,
              ParamP2(p1,p3,0.5), SlopeX(p1,p3)+HPI );  // .. position
   R = Distan( p, p1 );                                 // .. and radius
}

// ---------------------------------------------------------- handle angles, directions
REAL OrHig( REAL Fix, REAL Var ) {     // bring direction into disk [Fix,Fix+2*pi)
   REAL Dir = Var;
   while( Dir <  Fix       ) Dir += TPI;
   while( Dir >= Fix + TPI ) Dir -= TPI;
   return Dir;
}

REAL ArcCos( REAL Leg1, REAL Leg2, REAL Crss ) {  // angle at Leg1-Leg2
   if( Leg1*Leg2 == 0 ) {
      if( Leg1+Leg2 == 0 )
         Exit( L"ArcCos() -- three identical points" );
      Message( L"ArcCos( unreliable angle ) returns Pi/2" );
      return HPI;
   }
   REAL C = ( Leg1 * Leg1 + Leg2 * Leg2 - Crss * Crss ) / ( 2 * Leg1 * Leg2 );
   /*
   const INT2 ErrI = 17;
   if( C >= +1 ) {              // | Leg1 - Leg2 | >= Crss ! occurs regularly
      if( C-1 > POW10(-ErrI) ) Message( L"ArcCos() -- value > +1 returns 0!" );
      return 0;
   }
   else if( C <= -1 ) {         //   Leg1 + Leg2   <= Crss ! occurs never
      if( C+1 < POW10(ErrI) ) Message( L"ArcCos() -- value < -1 returns pi!" );
      return _PI;
   }
   */
        if( C >= +1 ) return 0;                   // | Leg1 - Leg2 | >= Crss
   else if( C <= -1 ) return _PI;                 //   Leg1 + Leg2   <= Crss
   return ACOS( C );
}

REAL ArcSin( REAL Cat, REAL Hyp ) {               // angle in right triangle
   if( Hyp == 0 ) {
      Message( L"ArcSin( unreliable angle ) returns 0" );
      return 0;
   }
   if( ABS(Cat) > Hyp ) {
      Message( L"ArcSin( bad input ) by", Cat/Hyp );
      return ( Cat/Hyp > 0 ) ? _PI : -_PI;
   }
   return ASIN( Cat/Hyp );
}
