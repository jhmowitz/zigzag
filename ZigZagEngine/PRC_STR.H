// prc_str.h : types and classes for process, prototypes for prc_str.cpp

#ifndef _PRC_STR_H
#define _PRC_STR_H

#include "direct.h"      // handle specially modelled discrete directions SMDD and ENWS

#define _CLASS_COUNT     // class count for program testing is done

// - - - - - - - - - - - type definitions: of special actions and supporting COLORS
enum CTYP {
                                        // -- identity types
   AVANE, ASPLT, AONCE, AFULL,             // (A) arcs
   BVANE, BFOLD, BGROW,                    // (B) angular bisectors
   CVANE, CVFLD, CONST, CGFLD, CGROW,      // (C) contours
                                        // -- event types: circles\triangles
   EDISI, ECTCT, ECONV, EMERG,             // (E) types (colors) of events
                                        // -- action types: triangles
   PEEK0, PEEK1, PEEK2, nPEEK,             // messages about diverse peeks
   GLUED, GLUE_,                           // glueing of contours
                                        // -- diverse types
   BACKG, ATTND, mTEST                     // background, attention, testing
};

typedef class TISP *    pTISP;             // to a time/space
typedef class CLOS *    pCLOS;             // to a closure: components/polygons
typedef class ETOP *    pETOP;             // to BASIC class ETOP
typedef class EVNT *    pEVNT;             // to BASIC class EVNT
typedef class BUMP *    pBUMP;             // to a point bisector
typedef class IDNT *    pIDNT;             // to BASIC class IDNT
typedef class ARCS *    pARCS;             // to an arc or a circle
typedef class CONT *    pCONT;             // to a contour
typedef class BISC *    pBISC;             // to an angular bisector

// -- definition of a time/space
class TISP: public PNT2, public LINK {
public:
   REAL TimB;                              // time of birth (or death)
   INT2 nRef;                              // number of references to this point as origin

   void incRef();                          // increment nRef to this
   void decRef();                          // decrement nRef, if 0 give back to storage
};

// -- definition of a component (of arcs) or polygon (of contours)
class CLOS: public LINK {
public:
   pCLOS Prev;                      // previous in working order (opposite to Next())
//   pCLOS AEgo;                      // the other triangle ! triangles only  // 25-11-2017 #####

   pIDNT Bound;                        // entry to boundary: ARCS (component), BISC (polygon)

   void DeSequence() {                 // cut component out of working sequence
      ((pCLOS)Next())->Prev = Prev;    // cut along Prev
      Prev->Cut();                     // cut along Next()
   }
   void SequenceIn( pCLOS newC ) {     // put given new closure behind this (the top)
      newC->Prev = this;               // newC has this as a previous
      ((pCLOS)Next())->Prev = newC;    // Next() takes newC as a previous
      Push( newC );                    // link newC behind this along Next()
   }
};

// -- definition of a BASIC class to link events of class EVNT in order of time
class ETOP: public LINK {
public:
   void ordEvnt( pEVNT );                                 // link actor into time order (TO)
   void getEvnt( pLINK, pCHAR = _T("no string into getEvnt()") ); // delink actor from TO
   void ridEvnt( pEVNT, pCHAR = _T("no string into getEvnt()") ); // delink from TO, rid Event
};

// -- definition of a BASIC class with event
class EVNT: public LINK {
public:
   pTISP Event;                           // time/space of event to come

   void  ClearEvent() { Event = NULL; }   // clear link to event
};

// -- definition of a point bisector ***** process event
class BUMP: public EVNT {
public:
   pARCS Arc1, Arc2;                      // reference to two adjacent arcs

   pARCS other( pARCS );                  // other arc to point bisector
   void  value( pARCS, pARCS );           // revalue arc of point bisector
};

// -- definition of a PART
class PART: public EVNT {
public:
   pTISP Orign;                           // time/space for the origin of the PART
   CTYP  Type;                            // type: growing, shrinking etc.
};

// -- definition of an identification to a closure
class IDNT: public PART {                 // used by ARCS (for component)
public:                                   // .. and by BISC (for polygon)
   pCLOS Ident;                           // closure (component/polygon) this belongs to
};

// -- definition of a circle or arc ***** process event
class ARCS: public IDNT {
public:
   INT2  BCnt;                            // count of future bumps
   pARCS Neigh;                           // reference to adjacent arc

   pARCS previous();                      // get previous arc in component
};

// -- definition of a contour with specially modelled direction ***** process event
class CONT: public PART, public IDIR {
public:
   pBISC Bis1, Bis2;                      // reference to two adjacent bisectors
   INT2  PCnt;                            // count of future peeks

   pBISC other( pBISC );                  // get other bisector to contour
   void  value( pBISC, pBISC );           // revalue bisector of contour
   void  valot( pBISC, pBISC );           // revalue other bisector of contour
   void  blend( REAL );                   // classify a contour at time
   void  CntLn( pLIN2, REAL );            // standardized contour (distance=1/length=2)
};

// -- definition of an angular bisector with specially modelled direction ***** process event
class BISC: public IDNT, public VELO {
public:
   pCONT Cnt1, Cnt2, Peek;                // two adjacent contours, contour to be hit

   pCONT other( pCONT );                  // get other contour to bisector
   void  value( pCONT, pCONT );           // revalue contour of bisector
   void  blend( REAL );                   // classify a bisector at time
   void  BisPt( pPNT2, REAL );            // compute bisector point
   void  BisLn( pLIN2, REAL, REAL );      // compute (piece of) bisector line
};

/*class OPPS {  ## to catch unwanted PeekPeek-situations
public: pBISC bs1, bs2; pCONT pk1, pk2; pTISP at1, at2; };*/

                                                 // -- general use
void  InitClasses();                                // initiate structures for arcs
                                                 // -- get from .. and give back to storage
pTISP Get_Tisp( REAL );                             // get a new time/space
void  Rid_Tisp( pTISP );                            // give time/space back
pBUMP Get_Bump( pARCS, pARCS, REAL );               // get a new point bisector
void  Rid_Bump( pBUMP );                            // give point bisector back
pARCS Get_Arcs( pCLOS, pTISP );                     // get a new arc
void  Rid_Arcs( pARCS );                            // give arc back
pCONT Get_Cont( pTISP, ENWS );                      // get a new contour
void  Rid_Cont( pCONT );                            // give contour back
pBISC Get_Bisc( pTISP, pCONT, pCONT );              // get a bisector
void  Rid_Bisc( pBISC );                            // give bisector back
                                                 // -- closures
pCLOS Get_Clos();                                   // get a new closure
void  Rid_Clos( pCLOS );                            // give closure back
                                                 // .. arcs or circles
void  MakeComp( pTISP );                            // implement new component
void  RevalComp( pCLOS, pARCS );                    // revalue component
INT2  VolComp( pCLOS );                             // N of arcs in one component
                                                 // .. polygonial
void  MakePoly( INT2, pREAL, pTISP );               // init a new polygon
void  MakeDoubleV( pPNT2, REAL, REAL, BOOL, BOOL ); // init a "double V" polygon
void  RevalPoly( pCLOS, pBISC );                    // revalue polygon
void  BlendPoly( pCLOS, REAL );                     // determine types of contors/bisectors
INT2  VolPoly( pCLOS );                             // N contours/bisectors in polygon
#endif             // _PRC_STR_H
// End of File
