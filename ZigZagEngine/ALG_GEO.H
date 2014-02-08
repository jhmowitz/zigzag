// alg_geo.h -- structures and prototypes for alg_geo.c

#ifndef _ALG_GEO_H
#define _ALG_GEO_H
                                                // -- pointers to geometrical classes
typedef class PNT2 *    pPNT2;                     // to 2D-point
typedef class VEC2 *    pVEC2;                     // to 2D-vector
typedef class LIN2 *    pLIN2;                     // to line of two 2D-points

class VEC2 {                                    // -- definition of a 2D-vector
public:
   REAL X, Y;                                      // x, y-coordinates
};

class PNT2 {                                    // -- definition of a 2D-point
public:
   REAL X, Y;                                      // x, y-coordinates
   void Assign( REAL x, REAL y ) { X = x; Y = y; }
                                                // -- from directed points
   void IncidP( pPNT2, REAL, pPNT2, REAL );        // incidence of two lines
                                                // -- from pairs of points
   REAL RelPar( pPNT2, pPNT2 );                    // relative parameter to directed line
   REAL RelDis( pPNT2, pPNT2 );                    // relative signed distance to dir. line
                                                // -- from lines
   void IncidP( pLIN2, pLIN2 );                    // incidence of two lines
   REAL RelPar( pLIN2 );                           // relative parameter to directed line
   REAL RelDis( pLIN2 );                           // relative signed distance to dir. line

   void operator=( PNT2 &P ) { X = P.X; Y = P.Y; } // assignment by a point
};

class CIRC {                                    // -- definition of a circle geometrically
   PNT2 P;                                         // midpoint (if used)
public:
   pPNT2 p;                                        // address to midpoint
   REAL  R;                                        // radius

   CIRC() { p = &P; }                              // use internal midpoint
   CIRC( pPNT2 q, REAL Rad ) { p = q; R = Rad; }   // use external midpoint

   void outCir( pPNT2, pPNT2, pPNT2 );             // outer circle of three points
};

class LIN2 {                                    // -- def. of a directed line of 2 points
public:
   PNT2 Tail, Head;                                // tail and head of directed line
};
                                  // -- prototypes
PNT2 operator+( PNT2 &, VEC2 & );    // point  = point + vector
VEC2 operator-( PNT2 &, PNT2 & );    // vector = point - point
VEC2 operator|( PNT2 &, PNT2 & );    // vector = clockwise orthogonal of difference: 2nd-1st
VEC2 operator+( VEC2 &, VEC2 & );    // vector = vector + vector
VEC2 operator*( VEC2 &, REAL );      // vector = vector * real
REAL operator*( VEC2 &, VEC2 & );    // real   = vector * vector
VEC2 UnitVec2( REAL );               // vector = unit vector in a given direction

REAL SumSqr( pPNT2, pPNT2 );         // sum of squares of 2D-points
REAL Distan( pPNT2, pPNT2 );         // distance of 2D-points
REAL SlopeX( pPNT2, pPNT2 );         // angle of two points to pos. x-axis

REAL OrHig( REAL, REAL );            // into higher disc
REAL ArcCos( REAL, REAL, REAL );     // return angle (arc) by acos()
REAL ArcSin( REAL, REAL );           // angle in right triangle

pPNT2 ParamP2( pPNT2, pPNT2, REAL ); // keep two static points, used alternatingly
#endif                     // _ALG_GEO_H
