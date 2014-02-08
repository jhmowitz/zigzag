// tri_str.h -- definitions:
//            - of a class SUPPT to support one polygon (global use)
//            - of classes to support triangle events (local use for tri_str.cpp only)

/* ------------------------------------------------------------------------------ SUPPT
 * -- to support handling of one polygon
 *
 *    use as:
 *
 *    pPOLY pol;                         // pointer to polygon
 *
 *    SUPPT( pol ) SP;
 *    do {
 *       .. use SP.b and SP.c
 *    } while( SP.next() );              // loop forward
 *   [} while( SP.prev() );]             // loop backward
 */

class SUPPT {
   pBISC f;                       // end of loop
public:
   pBISC b;                       // current bisector : b and c work as a pair
   pCONT c;                       // current contour  : like a matchstick ( .- or -. )

   SUPPT( pCLOS pol ) {           // initiate loop over polygon
      b = f = (pBISC)pol->Bound;  // set first (and last) bisector to be current
      c = b->Cnt1;                // set first contour to be current
   }

   BOOL prev() {                  // increment loop, by bisector first
      b = c->other( b );          // the matchstick loop backward ( .- .- etc. )
      c = b->other( c );
      return (BOOL)( b != f );    // loop finishes at entry: poly->Bound
   }
   BOOL next() {                  // increment loop, by contour first
      c = b->other( c );          // the matchstick loop forward ( -. -. etc. )
      b = c->other( b );
      return (BOOL)( b != f );    // loop finishes at entry: poly->Bound
   }
};

// ------------------------------------------------------------- triangle event support
#ifdef _TRI_STR_CPP               // ! for use within tri_str.cpp only
class ETIM: public GTIM {         // keep minimum time of an event of neighbouring structures
public:
   ETIM(): GTIM() {}
   ETIM( pTISP e ): GTIM() { ETIM::updT( e ); }

   void iniT( pBISC b ) {           // -- init time of integrity of a bisector
      GTIM::iniT();
      ETIM::updT( b->Event );          // limited by bisector's event
      ETIM::updT( b->Cnt1->Event );    // or by events of adjacent contours
      ETIM::updT( b->Cnt2->Event );
   }
   void updT( pTISP e ) {           // -- conditional update from event upon GTIM
      if( e != NULL ) GTIM::updT( e->TimB );
   }
};

class EARLY: public GTIM {          // -- earliest peek of a current bisector
   ETIM  bLim;                         // limitation of time validity
   pCONT Peek, Opp;                    // contour to be peeked
public:
   pBISC bTry;                         // bisector to try out
   PNT2  B0, B1;                       // points of bTry at Prcs.T, Prcs.T+1

   BOOL validT( REAL T ) { return (BOOL)( GTIM::valT( T ) && bLim.valT( T ) ); }
   void updatT( pCONT c, REAL T ) {
      Peek = c;                        // keep contour to peek
      GTIM::updT( T );                 // keep time of peek
   }
   void SetInteg( pBISC );             // set time of bTry-integrity (begin of search)
   void SetEvent();                    // set event of best pair  (end of search)
   BOOL IsOppositApp();                // opposite contour directions of bTry and Peek
};

class TRYOUT: public EARLY {           // -- a tryout: bisector peeks a contour
   REAL DisT;                             // distance of bTry to contour at Prcs.T
public:
   REAL TimP;                             // earliest time for bTry to peek
   BOOL ApproachTime( pCONT );            // compute time of Approach
   void ValidPosPar( pCONT );             // valid position and valid parameter
};

class T_WEAVE: public TRYOUT {         // -- defines a tryout: bisector peeks contour
   void  DirectGrab( pCONT );             // bTry peeks contour (on emergence)
public:
   T_WEAVE( pCLOS );                      // weave in a simple polygon and its dual
};

class HOOK: public TRYOUT {            // -- definition for two bisectors/contours
   INT2  nBE;                             // number of exception for bisectors
   pBISC bExcp[2];                        // bisector exceptions
   BOOL  NotIn( pBISC );                  // is bisector not in exception set

   INT2  nCE;                             // number of exception for contours
   pCONT cExcp[4];                        // contour exceptions
   BOOL  NotIn( pCONT );                  // is contour not in exception set

   BOOL validParam( pBISC );              // check valid parameter contact
   void Compare( pCONT );                 // bTry peeks with contour ?  (oncourse)
   BOOL tryPP( pCONT );                   // try for peekpeek (or Approach)
public:
   pBISC bx;                              // bisector (of ct) for PeekPeek
   pCONT same_iD( pBISC, pCONT );         // same iD for bisector and (adjacent) contour

   BISC  Left;                            // Top to link (TO) bisector for peeking
   void  UniqueLeft( pLINK );             // push a pBISC (as pLINK) uniquely to Left

   HOOK() {                               // clear exception lists
      Left.Clear();
      nBE = nCE = 0;
   }

   pBISC Classify( pCONT, pBISC,               // handle one HOOK
                   pCONT, pBISC, pTISP );
   pBISC Unify( pCONT, pBISC, pCONT, pBISC );  // even out one HOOK

   void DiesOut( pCONT );                 // check contour to disintegrate
   void reActiv( pCONT, pBISC );          // (re)activate adjacing bisectors
   void Repare();                         // check all contacts
   void Few_b_Peek_Many_c();              // b in Left peek c not in cExcp[]
   void Many_b_Peek_Few_c();              // all b not in bExcp[] peek c in cExcp[]
   void ClearPeeks( pCONT );              // clear all peeks of contour
};

class TCONTCT: public HOOK {
   pCLOS pl_b, pl_c;                      // polygons: of b, of c
   pBISC b, b1, b2;                       // b peeks: b1, b2 are created
   pCONT grab, splt;                      // is peeked: is split off grab
   pTISP o;                               // current event: becomes Orign of b1 and b2

   void clearBis();                       // clear all contour events for bisector
   BOOL PeekPeek();                       // handle peeks between two bisectors
   void PeekCont();                       // handle peeks of bisector into contour
   void connectPoly();                    // (dis)connect polygons

   void  RidEgoPeeks();                   // dismantle aternative triangles: one or two
   pCLOS ClearEgoPeeks( pCLOS );          // clear peeks of bisectors of other triangle
   void  ClearEgoGrabs( pCLOS );          // clear/retry peeks to contours of other triangle
public:
   TCONTCT( pBISC );                      // handle one peek
};

class TDISINT: public HOOK {   // -- to control one disintegration
   pCLOS pol;                     // initial: polygon of disintegration
   pTISP ori;                     // initial: time/space of event for origin
   pCONT die;                     // initial: disintegrating contour
   pCONT c1, c2;                  // constructing: new neighbours after disintegration
   pBISC b1, b2;                  // constructing: melt into bisector between c1 and c2

   BOOL  Converge();              // check around for convergence
   void  Reach();                 // check for overdefinition
   void  Clr_Half( INT2 );
   pBISC ClassifD();              // handle one HOOK
public:
   TDISINT( pCONT );
};

#endif                            // _TRI_STR_CPP
