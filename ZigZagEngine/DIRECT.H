/* -- direct.h

      Handle specially modelled directions between 0 and 2*pi
      for contours and angular bisectors in the process of expanding triangles.

      A direction will be represented as a number of type  --> unsigned long integer.
      These numbers range from 0 to about 4 billion (4,294,967,295).
      For convienience they will be used from 0 to 3.6 billion - 1.
      0 and 3.6 billion then represent the directions 0 and 2*pi, that are
      identical and both identified with the direction of the positive x-axis.

      Such a number is typed --> ENWS (derived from: East, North, West, South).
      Using the following constants for computations:

      - NULLCIRCLE equals 0
      - HALFCIRCLE equals 1.8 billion
      - FULLCIRCLE equals 3.6 billion

      A bisectors can use all values of the ENWS-domain, whereas a contour can only
      use even values. By that it is assured, that two contours can always
      have a valid bisector between them except in two cases (to be avoided):
      - two contours are parallel in the same direction (they differ NULLCIRCLE)
        then they are identical or they have no incidence
      - two contours are parallel in opposite directions (they differ HALFCIRCLE)
        then the bisector has infinite velocity
*/

#ifndef _DIRECT_H
#define _DIRECT_H

typedef  uINT4 *  pENWS;               // pointer to direction type
typedef  uINT4     ENWS;               // direction:

// constants of direction type used for computations between REAL and ENWS
#define  NULLCIRCLE           0ul      // 0 degrees
#define  EVEN_MAKER           2ul      // to make even values for contour direction
#define  HALFCIRCLE  1800000000ul      // 1.8 billion for 180 degrees
#define  FULLCIRCLE  3600000000ul      // 3.6 billion for 360 degrees

// -- a unique service class to support basic computations between ENWS and REAL
class SSMDD {        // S(upport) S(pecially) M(odelled) D(iscrete) D(irections)
   REAL ArcFactor;                     // divide 2pi by FULLCIRCLE
   REAL DirFactor;                     // divide HALFCIRCLE by 2pi
public:
   SSMDD() {
      ArcFactor = (REAL)TPI / FULLCIRCLE;
      DirFactor = (REAL)HALFCIRCLE / TPI;
   }
   REAL Dir2Arc() { return ArcFactor; }
   REAL Arc2Dir() { return DirFactor; }
};

                                     // -- prototypes
REAL RealArc( ENWS );                   // change ENWS to REAL direction (arc)
ENWS ContourDir( REAL );                // change REAL direction to ENWS
ENWS OppositDir( ENWS );                // compute opposite direction
ENWS SubDir( ENWS, ENWS );              // subtraction of directions
ENWS AddDir( ENWS, ENWS );              // addition of directions

class IDIR {                         // -- class to guard a direction for CONT and BISC
public:
   ENWS iDir;                           // all even values  between 0 and 3.6 billion - 2
   REAL D() { return RealArc( iDir ); } // REAL values between 0 and 2*pi
};

class VELO: public IDIR {            // -- class to guard a velocity of a bisector
   ENWS MeanDir( ENWS, ENWS );          // compute direction of bisector (iDir)
   ENWS HalfDiff( ENWS, ENWS );         // compute velocity of bisector (VFac)
public:                                 // .. used by class BISC
   REAL VFac;                           // factor for velocity of bisector (1/cos)
   void DirVelo( ENWS, ENWS );          // ENWS for direction, parameter for velocity
};

// --------------------------------------------------------------------------------------
#ifdef _DIRECT_CPP       // to include code for specially modelled directions

extern void Message( pCHAR, ... );

SSMDD sSMDD;                           // initial support for SMDD

// from ENWS to REAL direction
REAL RealArc( ENWS eDir ) { return (REAL)eDir * sSMDD.Dir2Arc(); }

// change REAL (contour) direction to ENWS ! even numbers of ENWS only
ENWS ContourDir( REAL aDir ) {
   REAL Prop = OrHig( (REAL)0, aDir ) * sSMDD.Arc2Dir();
   return (ENWS)Prop * EVEN_MAKER;
}

ENWS OppositDir( ENWS eDir ) {         // compute opposite direction
   return ( eDir < HALFCIRCLE ) ? ( eDir + HALFCIRCLE ) : ( eDir - HALFCIRCLE );
}

ENWS SubDir( ENWS Pos, ENWS Neg ) {    // subtraction of directions
   return ( Pos >= Neg ) ? ( Pos - Neg ) : ( ( FULLCIRCLE - Neg ) + Pos );
}

ENWS AddDir( ENWS D1, ENWS D2 ) {      // addition of directions
   if( D1 < HALFCIRCLE ) {
      if( D2 < HALFCIRCLE )    return D1 + D2;
      if( FULLCIRCLE-D2 > D1 ) return D1 + D2;
      else                     return ( ( D2 - HALFCIRCLE ) + D1 ) - HALFCIRCLE;
   }
   else if( D2 < HALFCIRCLE )
      if( FULLCIRCLE-D1 > D2 ) return D1 + D2;
      else                     return ( ( D1 - HALFCIRCLE ) + D2 ) - HALFCIRCLE;
   else return ( D1 - HALFCIRCLE ) + ( D2 - HALFCIRCLE );
}

// -- from the directions of two neighboring contours
//    compute the direction and the velocity of the bisector in between
void VELO::DirVelo( ENWS D1, ENWS D2 ) {
   // mean of directions of neighboring contours to compute direction
   iDir = MeanDir( D1, D2 );
   // difference of directions of neighboring contours to compute velocity of bisector
   VFac = 1.0 / cos( RealArc( HalfDiff(D1,D2) ) );
}

// mean of directions of neighboring contours to compute direction of bisector
ENWS VELO::MeanDir( ENWS D1, ENWS D2 ) {
   if( D1 == D2 ) Exit( L"VELO::MeanDir() -- equal values" );
//   if( D1 == D2 ) ???                            // ! equal values ## error

   ENWS i = ( D1 < D2 ) ? D1 : D2,               // sample minimum (i)
        x = ( D1 < D2 ) ? D2 : D1;               // and maximum (x) of D1, D2
   if( i < HALFCIRCLE )                          // otherwise there is no opposition
      if( i+HALFCIRCLE == x ) Exit( L"VELO::MeanDir() -- opposite values" );
//      if( i+HALFCIRCLE == x ) return FULLCIRCLE; // ! opposite values ## error

   ENWS Dif = ( x - i ) / EVEN_MAKER;
        if( x-i < HALFCIRCLE ) return i + Dif;              // between x and FULLCIRCLE
   else if( i < FULLCIRCLE-x ) return i + Dif + HALFCIRCLE; // between 0 and i

   return i + Dif - HALFCIRCLE;
}

// difference of directions of neighboring contours to compute velocity of bisector
ENWS VELO::HalfDiff( ENWS D1, ENWS D2 ) {
   ENWS Dif = ( D1 > D2 ) ? ( D1 - D2 ) : ( D2 - D1 );
   if( Dif > HALFCIRCLE ) Dif = FULLCIRCLE - Dif;
   if( Dif == NULLCIRCLE ) Message( L"VELO::HalfDiff() -- Bisector has contour velocity" );
   if( Dif == HALFCIRCLE ) Exit( L"VELO::HalfDiff() -- Bisector has infinite velocity" );
   return Dif /= EVEN_MAKER;
}

#endif                     // _DIRECT_CPP
#endif                     // _DIRECT_H
