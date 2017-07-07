// tri_str.cpp

#include <math.h>               // ABS(), POW10()

#include "mow_defs.h"           // incriptions for includes (use long double)
#include "alg_geo.h"            // structures for points, lines and circles

#include  MOW_LINK_H            // linkage: include class definition
#include "prc_str.h"            // serving structures

#include "prc_tim.h"            // structure for time handling
#include "prc_def.h"            // all general process classes

#define  _TRI_STR_CPP           // to include definitions for this file only
#include "tri_str.h"            // definition of contour support class SUPPT
#undef   _TRI_STR_CPP

extern void ShowEvent( BZTYP, pPNT2, CTYP, pCHAR );
extern void PrintTest( pCHAR, ... );

// -- if an event of triangles is to be executed,
//    pop it from its queue of events and
//    initialize a class declaration that executes itself

void DoTriEmerge() { T_WEAVE W( (pCLOS)ZgZg_O.Emg->Pop() ); } // an emergence
void DoTriContct() { TCONTCT C( (pBISC)ZgZg_O.Cct->Pop() ); } // a contact
void DoTriDisint() { TDISINT D( (pCONT)ZgZg_O.Dis->Pop() ); } // a disintegration

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - general conditional
INT2 ErrSig = -7;            // number of significant positions for testing ?? waarom 7

REAL DomErr( INT2 E ) {   // -- set all relative grain sizes (no prototype)
                             // 1.23 is the diagonal from (0,1) by (0,0.7)
   return ( 1.23 + 2. * Prcs.T ) * POW10( E );
}

BOOL diffEvent( pTISP e1, pTISP e2 ) {  // check different time/site for TISPs
   REAL DE7 = DomErr( ErrSig );
   return (BOOL)( fabs(e1->TimB-e2->TimB) > DE7 ||
                  fabs(e1->X-e2->X) > DE7 ||
                  fabs(e1->Y-e2->Y) > DE7 );
}
BOOL diffETime( pTISP e1, pTISP e2 ) {  // check different time for TISPs
   return (BOOL)( fabs(e1->TimB-e2->TimB) > DomErr(ErrSig) );
}
BOOL diffESite( pTISP e1, pTISP e2 ) {  // check different site for TISPs
   REAL DE7 = DomErr( ErrSig );
   return (BOOL)( fabs(e1->X-e2->X) > DE7 || fabs(e1->Y-e2->Y) > DE7 );
}

BOOL Neighbors( pBISC b, pCONT c ) {    // check b with c: valid combination
   if( b->Ident == c->Bis1->Ident )     // only if from the same closure/polygon
      if( b == c->Bis1 || b == c->Bis1->other(c)->other(c->Bis1) ||
          b == c->Bis2 || b == c->Bis2->other(c)->other(c->Bis2) ) return TRUE;
   return FALSE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - EARLY
void EARLY::SetInteg( pBISC b ) {
   bTry = b;                             // bTry is bisector to check
   bTry->BisPt( &B0, Prcs.T );           // compute physical points of bTry
   bTry->BisPt( &B1, Prcs.T+1 );         // .. at Prcs.T and Prcs.T+1
   bLim.iniT( bTry );                    // init time of integrity of bTry
   iniT();                               // init peeking result of earliest peek
}

void EARLY::SetEvent() {                 // init for best peek of bTry
   if( !Act ) return;                    // no peek for bTry
   if( bTry->Event != NULL ) {           // if present .. rid old event
      ZgZg_O.Cct->ridEvnt( bTry, _T("EARLY::SetEvent()") );
      bTry->Peek->PCnt--;                // .. diminish peek count
   }

   // check for appearence of opposite contour directions
   if( IsOppositApp() )
      Mess_LT( _T("EARLY::IsOppositApp() -- Opposite appearence of contours!") );

   bTry->Event = Get_Tisp( Lim );        // get new time/space
   bTry->BisPt( bTry->Event, Lim );      // .. at this location
   bTry->Peek = Peek;                    // .. peeks this contour
   bTry->Peek->PCnt++;                   // .. keep up peek count of contour
   ZgZg_O.Cct->ordEvnt( bTry );            // link bTry into ZgZg.Cct sequence
}

// -- control appearence of opposite contour directions (dir with dir+pi or dir-pi)
BOOL EARLY::IsOppositApp() {                          // Opp is contour opposite to Peek
        if( bTry->Cnt1->iDir == OppositDir(Peek->iDir) ) Opp = bTry->Cnt1;
   else if( bTry->Cnt2->iDir == OppositDir(Peek->iDir) ) Opp = bTry->Cnt2;
   else return FALSE;
   return TRUE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - TRYOUT
// -- check for an approach of bisector bTry and contour ct
// -- that is in time for bTry and in time for ct
BOOL TRYOUT::ApproachTime( pCONT ct ) {
   LIN2 C0; ct->CntLn( &C0, Prcs.T );        // unit line of ct at Prcs.T
   LIN2 C1; ct->CntLn( &C1, Prcs.T+1 );      // unit line of ct at Prcs.T+1

   // test distances of bTry to ct
        DisT = B0.RelDis( &C0 );             // at Prcs.T
   REAL Dis1 = B1.RelDis( &C1 );             // at Prcs.T+1
   if( DisT*Dis1 >= 0 )                      // with equal sign (late occurence)
      if( ABS(DisT) <= ABS(Dis1) )           // .. and increasing value: no approach
         return FALSE;                       // .. "equals" denotes parallelity of contours

   // compute time of possible contact
   TimP = Prcs.T + DisT / ( DisT - Dis1 );

   // test wether time is valid for bTry
   if( !EARLY::validT(TimP) ) return FALSE;  // too late for bTry

   // test wether time is valid for ct
   ETIM cLim( ct->Event );
   if( !cLim.valT(TimP) ) return FALSE;      // too late for ct

   return TRUE;                              // valid time for bTry with ct
}

// -- check bTry with ct for valid position (1) and valid parameter (2)
// 1) the distance of each adjacent bisector point to the contour at peeking time
//    must have the same sign as the bisector point bTry itself at trying time
// 2) the relative parameter of bTry with respect to the contour must be twice >= 0
void TRYOUT::ValidPosPar( pCONT ct ) {
/* ## wat is hier mis mee ??
   LIN2 C0; ct->CntLn( &C0, Prcs.T );        // unit line of ct at Prcs.T
   PNT2 O1; bTry->Cnt1->other(bTry)->BisPt( &O1, Prcs.T );
   PNT2 O2; bTry->Cnt2->other(bTry)->BisPt( &O2, Prcs.T );
   REAL DO1 = O1.RelDis( &C0 );
   REAL DO2 = O2.RelDis( &C0 );
   if( DisT*DO1 <= 0 || DisT * DO2 <= 0 ) return;
   if( ABS(DisT) > ABS(DO1) || ABS(DisT) > ABS(DO2) ) return;
*/
   // test adjacent bisector points in conjubction with bTry with respect to the contour
   LIN2 CT; ct->CntLn( &CT, TimP );                // line of ct at TimP
   PNT2 Adj;
   bTry->Cnt1->other(bTry)->BisPt( &Adj, TimP );
   if( DisT*Adj.RelDis(&CT) < 0 ) return;          // (different signs) invalid position
   bTry->Cnt2->other(bTry)->BisPt( &Adj, TimP );
   if( DisT*Adj.RelDis(&CT) < 0 ) return;          // (different signs) invalid position

   // test peek of bisector point to be within contour range
   PNT2 B; bTry->BisPt( &B, TimP );                // point of bTry at TimP
   PNT2 P; ct->Bis1->BisPt( &P, TimP );            // points of ct
   PNT2 Q; ct->Bis2->BisPt( &Q, TimP );
   if( B.RelPar(&P,&Q) < 0 ||
       B.RelPar(&Q,&P) < 0 ) return;               // invalid parameter

   EARLY::updatT( ct, TimP );                      // found something
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - T_WEAVE
// -- handle emergence of one triangle
T_WEAVE::T_WEAVE( pCLOS pN ) {
   pCLOS newC = pN,                               // emerging polygons: continuous(C)
         newR = pN->AEgo;                         //                    reflecting(R)

   if( Prcs.Evt )                                 // show event on screen
      ShowEvent( TRIANG, newC->Bound->Orign, EMERG, _T("Emergence") );

   // try bisectors of new polygon with avtive contours
   SUPPT C( newC );
   do {
      if( C.b->Type == BVANE ) continue;          // only for "DoubleV" (or other designs)

      EARLY::SetInteg( C.b );                     // initialize for peeking

      pCLOS p = ZgZg_O.Act;                         // loop over all polygons
      while( (p=(pCLOS)p->Next()) != ZgZg_O.Act ) {
         SUPPT A( p );
         do DirectGrab( A.c ); while( A.prev() );
      }

                                                  // ## only for "DoubleV"
//      SUPPT C_( newC );                         // try bisectors of newC with itself
//      do if( !Neighbors(C.b,C_.c) ) DirectGrab( C_.c ); while( C_.prev() );

      EARLY::SetEvent();                          // set event of best pair
   } while( C.prev() );

   SUPPT R( newR );
   do {
      if( R.b->Type == BVANE ) continue;          // only for "DoubleV" (or other designs)

      EARLY::SetInteg( R.b );                     // initialize for peeking

      pCLOS p = ZgZg_O.Act;                         // loop over all polygons
      while( (p=(pCLOS)p->Next()) != ZgZg_O.Act ) {
         SUPPT A( p );
         do DirectGrab( A.c ); while( A.prev() );
      }

                                                  // ## only for "DoubleV"
//      SUPPT R_( newR );                         // try bisectors of newR with itself
//      do if( !Neighbors(R.b,R_.c) ) DirectGrab( R_.c ); while( R_.prev() );

      EARLY::SetEvent();                          // set event of best pair
   } while( R.prev() );

   // try active bisectors with contours of new polygon
   pCLOS p = ZgZg_O.Act;
   while( (p=(pCLOS)p->Next()) != ZgZg_O.Act ) {
      SUPPT A( p );
      do {
         if( A.b->Type == BVANE ) continue;
         EARLY::SetInteg( A.b );                  // initialize for peeking

         SUPPT C( newC );                         // grab for newC
         do DirectGrab( C.c ); while( C.prev() );

         SUPPT R( newR );                         // grab for newR
         do DirectGrab( R.c ); while( R.prev() );

         EARLY::SetEvent();                       // set event of best pair
      } while( A.prev() );
   }

   // put the two parts of the polygon into the sequence of all polygons
   ZgZg_O.Act->SequenceIn( newC );
   ZgZg_O.Act->SequenceIn( newR );
}

// -- check bTry to peek contour c
void T_WEAVE::DirectGrab( pCONT c ) {
   if( bTry->Cnt1->iDir == c->iDir ||    // parallel contours in same direction
       bTry->Cnt2->iDir == c->iDir ) return;

   // if bTry approaches c and time is valid and positions are valid, then keep c
   if( TRYOUT::ApproachTime(c) ) TRYOUT::ValidPosPar( c );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - HOOK
// -- construct one HOOK after disintegration
pBISC HOOK::Classify( pCONT c1, pBISC b1,
                      pCONT c2, pBISC b2, pTISP o ) {
   pBISC b = Get_Bisc( o, c1, c2 );       // get new bisector
   c1->valot( b1, b );                    // let c1 refer to b (at reference other than b1)
   c2->valot( b2, b );                    // let c2 refer to b (at reference other than b2)

   b->blend( Prcs.T );                    // ## classify bisector
   if( b->Type != BVANE ) {               // put bisector on workarrays
      UniqueLeft( b );                    // push bisector b uniquely at Left
      bExcp[nBE++] = b;                   // .. as exception
   }

   DiesOut( c1 );                         // look for disintegration c1
   reActiv( c1, b1 );                     // (re)activate b1 ( of c1 )
   DiesOut( c2 );                         // look for disintegration c2
   reActiv( c2, b2 );                     // (re)activate b2 ( of c2 )
   return b;                              // use b as new boundary entry
}

pBISC HOOK::Unify( pCONT c1, pBISC b1,             // even out one HOOK
                   pCONT c2, pBISC b2 ) {
                                     // -- a relinkage
   b2->value( c2, c1 );                 // c2 becomes c1 for bisector b2
   c1->valot( b1, b2 );                 // other than b1 becomes b2 for c1
                                     // -- take over valid information
   if( c2->PCnt > 0 ) {                 // if present, reset peeks to c2 to c1
      pBISC x = (pBISC)ZgZg_O.Cct;
      while( c2->PCnt > 0 && (x=(pBISC)x->Next()) != NULL )
         if( x->Peek == c2 ) {
            x->Peek = c1;
            c1->PCnt++;                    // increase peek count
            c2->PCnt--;                    // diminish peek count
         }
   }
   Rid_Cont( c2 );                   // -- give c2 back to store

   DiesOut( c1 );                       // look for disintegration
   reActiv( c1, b1 );                   // (re)activate b1 ( of c1 )
   reActiv( c1, b2 );                   // (re)activate b2 ( of c1 )
   return b1;                           // use b1 as new boundary entry
}

void HOOK::DiesOut( pCONT c ) {         // check contour to disintegrate
   cExcp[nCE++] = c;                    // put contour on workarray
   c->blend( Prcs.T );
                                        // ! contour is not disintegrating
   if( c->Type != CVANE && c->Type != CVFLD ) return;
                                                  // -- c will disintegrate
   LIN2 B1; c->Bis1->BisLn( &B1, Prcs.T, Prcs.T+1 ); // set one bisector of c
   LIN2 B2; c->Bis2->BisLn( &B2, Prcs.T, Prcs.T+1 ); // set other bisector of c
   REAL DT = B2.Tail.RelDis( &B1 ),                  // distance of B2 to B1 (Tail)
        DH = B2.Head.RelDis( &B1 );                  // distance of B2 to B1 (Head)
   c->Event = Get_Tisp( Prcs.T+DT/(DT-DH) );         // new time/space, set time
   c->Event->IncidP( &B1, &B2 );                     // set space
   ZgZg_O.Dis->ordEvnt( c );                           // link c into disintegration queue
}

void HOOK::reActiv( pCONT c, pBISC b ) {        // (re)activate adjacing bisectors
        if( b->Type == BVANE ) return;
   else if( b->Event == NULL ) UniqueLeft( b ); // push bisector b uniquely at Left
   else if( c->Event != NULL )
      if( b->Event->TimB >= c->Event->TimB ) {
         ZgZg_O.Cct->ridEvnt( b, _T("HOOK::reActiv()") );
         b->Peek->PCnt--;                       // diminish peek count
      }
}

void HOOK::Repare() {
   pBISC a = (pBISC)ZgZg_O.Cct, n;
   while( (n=(pBISC)a->Next()) != NULL ) {
      BOOL IsNeigh = FALSE,
           IsInval = FALSE;
           if( Neighbors(n,n->Peek) )              IsNeigh = TRUE;
      else if( !NotIn(n->Peek) && !validParam(n) ) IsInval = TRUE;
      if( IsNeigh || IsInval ) {
//         if( IsInval )           // ## waarom komt deze voor ?
//            Message( L"HOOK::Repare() -- Invalid parameter occurs!" );
         Rid_Tisp( n->Event );
         n->ClearEvent();
         n->Peek->PCnt--;
         UniqueLeft( a->Pop() );   // push bisector n uniquely at Left
      }
      else a = n;
   }

   Few_b_Peek_Many_c();              // b in Left try peeking c not in cExcp[]
   Many_b_Peek_Few_c();              // all b not in bExcp[] try peeking c in cExcp[]
}

BOOL HOOK::NotIn( pBISC b ) {
   if( b->Type == BVANE ) return FALSE;   // vaning bisectors are automatically excluded
   if( nBE == 0 ) return TRUE; if( b == bExcp[0] ) return FALSE;
   if( nBE == 1 ) return TRUE; if( b == bExcp[1] ) return FALSE;
   return TRUE;
}

// -- check b with b->Peek to have valid parameter of extension
BOOL HOOK::validParam( pBISC b ) {
   PNT2 PeekB1; b->Peek->Bis1->BisPt( &PeekB1, b->Event->TimB );
   PNT2 PeekB2; b->Peek->Bis2->BisPt( &PeekB2, b->Event->TimB );
   REAL Param1 = b->Event->RelPar( &PeekB1, &PeekB2 );
   REAL Param2 = b->Event->RelPar( &PeekB2, &PeekB1 );
   return (BOOL)( Param1 >= 0 && Param2 >= 0 );
}

void HOOK::Few_b_Peek_Many_c() {             // b in Left peek c not in cExcp[]
   while( !Left.Empty() ) {
      EARLY::SetInteg( (pBISC)Left.Pop() );  // initialize for peeking
      pCLOS p = ZgZg_O.Act;
      while( (p=(pCLOS)p->Next()) != ZgZg_O.Act ) {
         SUPPT S( p );
         do if( NotIn(S.c) ) Compare( S.c );
         while( S.prev() );
      }
      EARLY::SetEvent();                     // set event of best pair
   }
}

BOOL HOOK::NotIn( pCONT c ) {
   if( nCE == 0 ) return TRUE; if( c == cExcp[0] ) return FALSE;
   if( nCE == 1 ) return TRUE; if( c == cExcp[1] ) return FALSE;
   if( nCE == 2 ) return TRUE; if( c == cExcp[2] ) return FALSE;
   if( nCE == 3 ) return TRUE; if( c == cExcp[3] ) return FALSE;
   return TRUE;
}

void HOOK::Many_b_Peek_Few_c() {          // all b not in bExcp[] peeks c in cExcp[]
   pCLOS p = ZgZg_O.Act;
   while( (p=(pCLOS)p->Next()) != ZgZg_O.Act ) {
      SUPPT S( p );
      do if( NotIn(S.b) && S.b->Type != BVANE ) {
            EARLY::SetInteg( S.b );       // initialize for peeking
            for( INT2 i = 0; i < nCE; i++ )
               Compare( cExcp[i] );
            EARLY::SetEvent();            // set event of best pair
         }
      while( S.prev() );
   }
}

void HOOK::Compare( pCONT ct ) {             // check bTry peeks contour 
   if( Neighbors(bTry,ct) ) return;          // -- no comparision
   if( bTry->Cnt1->iDir == ct->iDir ||       //    bTry and ct
       bTry->Cnt2->iDir == ct->iDir ||       //    are of the same kind
       bTry->Orign == ct->Orign ) return;    // -- approach is impossible

   if( !TRYOUT::ApproachTime(ct) ) return;   // time of approach not valid
   if( !tryPP(ct) )                          // no try for PeekPeek
      TRYOUT::ValidPosPar( ct );             // position and parameter
}

BOOL HOOK::tryPP( pCONT ct ) {   // determine to try PeekPeek (or Approach)
   pCONT same = same_iD( bTry, ct );
   if( same == NULL ) return FALSE;                         // try normal peek
   if( bx->other(ct)->Orign != same->Orign ) return FALSE;  // try normal peek

   // ## bx->Event != NULL && bx->Peek == bTry->other(same)
   // ## && bTry->Event and bx->Event at equal site and at equal time

   ETIM cLim( bx->other(ct)->Event );
   if( !cLim.valT(TimP) ) return FALSE;    // -- too late for bx->other->(ct)

   EARLY::updatT( ct, TimP );              // .. found something
   return TRUE;                            // ! try peekpeek
}

// a contour of b having same orientation as adjacent contour of c
// the bisector between c and the adjacent contour will be set to bx
pCONT HOOK::same_iD( pBISC b, pCONT c ) {
   if( c->Bis1->other(c)->iDir == b->Cnt1->iDir ) { bx = c->Bis1; return b->Cnt1; }
   if( c->Bis1->other(c)->iDir == b->Cnt2->iDir ) { bx = c->Bis1; return b->Cnt2; }
   if( c->Bis2->other(c)->iDir == b->Cnt1->iDir ) { bx = c->Bis2; return b->Cnt1; }
   if( c->Bis2->other(c)->iDir == b->Cnt2->iDir ) { bx = c->Bis2; return b->Cnt2; }
   return NULL;                                 // try normal peek
}

void HOOK::ClearPeeks( pCONT c ) {      // clear all peeks of contour
   if( c->PCnt == 0 ) return;
   pBISC a = (pBISC)ZgZg_O.Cct, n;
   while( c->PCnt > 0 && (n=(pBISC)a->Next()) != NULL )
      if( n->Peek == c ) {
         Rid_Tisp( n->Event );
         n->ClearEvent();
         c->PCnt--;
         UniqueLeft( a->Pop() );        // push bisector n uniquely at Left
      }
      else a = n;

   if( c->PCnt > 0 ) Mess_LT( _T("PCnt werkt niet ?????????? blijft positief") );
}

void HOOK::UniqueLeft( pLINK b ) { // push a pBISC (as pLINK) uniquely after Left
   pLINK a = &Left, n;
   while( (n=a->Next()) != NULL )
      if( b == n ) return;
      else a = n;
   a->Push( b );                   // push at the end of the queue
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - TCONTCT
TCONTCT::TCONTCT( pBISC i ): HOOK() { // -- handle one peek
   b = i;
   grab = b->Peek;
   grab->PCnt--;                         // ## very voorlopig

   pl_b = b->Ident;                      // closure of peeking bisector
   pl_c = grab->Bis1->Ident;             // closure of contour to peek

   o = b->Event; b->ClearEvent();        // detach event and clear holder

   RidEgoPeeks();                        // dismantle dual triangles of pl_b or pl_c
   clearBis();                           // clear all contour events for b
   if( !PeekPeek() ) PeekCont();
   connectPoly();                        // (dis)connect polygons
   HOOK::Repare();                       // check all contacts and dieGrab
}

// dismantle aternative triangles: one or two
void TCONTCT::RidEgoPeeks() {
   pCLOS b_oth = ClearEgoPeeks( pl_b );
   pCLOS c_oth = ClearEgoPeeks( pl_c );
   if( b_oth != NULL ) ClearEgoGrabs( b_oth );
   if( c_oth != NULL ) ClearEgoGrabs( c_oth );
}

// clear all peeks of bisectors of alternative triangle
pCLOS TCONTCT::ClearEgoPeeks( pCLOS pol ) {
   if( pol->AEgo == NULL ) return NULL;    // no alternative triangle

   pCLOS oth = pol->AEgo;  // dual triangle
   pol->AEgo = NULL;       // separate dual triangle
   oth->AEgo = NULL;   

   INT2 nPeek = 0;                         // count future peeks of bisectors of oth
   SUPPT Cnt( oth );
   do if( Cnt.b->Event != NULL ) nPeek++; while( Cnt.prev() );

   pLINK cur = ZgZg_O.Cct;                   // search the queue for peeks of oth
   pBISC nxt;
   while( nPeek > 0 && (nxt=(pBISC)cur->Next()) != NULL )
      if( nxt->Ident == oth ) {
         cur->Pop();
         Rid_Tisp( nxt->Event );           // rid event time/space
         nxt->ClearEvent();                // destroy reference
         nxt->Peek->PCnt--;                // diminish peek count of contour to peek
         nPeek--;                          // one bisector is cleared
      }
      else cur = nxt;

   if (nPeek > 0 ) Exit( L"TCONTCT::ClearEgoPeeks() ?? %d peek events not found", nPeek );
   return oth;
}
// clear/retry all peeks of bisectors that peek contours of the alternative triangle
void TCONTCT::ClearEgoGrabs( pCLOS oth ) {
   INT2 nGrab = 0;                         // count future peeks to contours of oth
   SUPPT Cnt( oth );
   do nGrab += Cnt.c->PCnt; while( Cnt.prev() );

   pLINK cur = ZgZg_O.Cct;
   pBISC nxt;
   while( nGrab > 0 && (nxt=(pBISC)cur->Next()) != NULL )
      if( nxt->Peek->Bis1->Ident == oth ) {
         Rid_Tisp( nxt->Event );           // rid event time/space
         nxt->ClearEvent();                // destroy reference
         nxt->Peek->PCnt--;
         nGrab--;
         UniqueLeft( cur->Pop() );         // push bisector nxt uniquely at Left
      }
      else cur = nxt;

   if (nGrab > 0 ) Exit( L"TCONTCT::ClearEgoGrabs() ?? %d grab events not found", nGrab );

   SUPPT Dis( oth );                       // dismantle oth
   do {
      Rid_Cont( Dis.c );                   // give contour back
      Rid_Bisc( Dis.b );                   // give bisector back
   } while( Dis.prev() );
   Rid_Clos( oth );                        // give closure back
}

// -- clear all contour events (disintegrations) for bisector
void TCONTCT::clearBis() {
   if( Prcs.Evt ) ShowEvent( TRIANG, o, ECTCT, _T("Contact") );

   if( b->Peek->Event != NULL ) ZgZg_O.Dis->ridEvnt( b->Peek, _T("TCONTCT::clearBis( b->Peek )") );
   if( b->Cnt1->Event != NULL ) ZgZg_O.Dis->ridEvnt( b->Cnt1, _T("TCONTCT::clearBis( b->Cnt1 )") );
   if( b->Cnt2->Event != NULL ) ZgZg_O.Dis->ridEvnt( b->Cnt2, _T("TCONTCT::clearBis( b->Cnt2 )") );
}

BOOL TCONTCT::PeekPeek() {                        // handle peeks between two bisectors
   pCONT bsam;                                    // one part of a restored contour
   bsam = HOOK::same_iD( b, grab );
   if( bsam == NULL ) return FALSE;               // try normal peek

   pCONT csam = bx->other( grab );                // the other part of a restored contour
   if( csam->Orign != bsam->Orign ) return FALSE; // try normal peek

   if( bx->Event == NULL )
      ShowEvent( TRIANG, bx->Event, nPEEK, _T("PeekPeek of one event") );
   else {
      // ## bx->Peek must equal b->other(bsam) !!
      if( bx->Peek != b->other(bsam) )
         Message( L"TCONTCT::PeekPeek() ?? bx peeks into wrong contour" );
      if( diffETime(o,bx->Event) ) {
         if( Prcs.TOn ) { 
            ShowEvent( TRIANG, o, PEEK1, _T("Peek ..") );
            ShowEvent( TRIANG, bx->Event, PEEK2, _T(".. and not Peek") );
         }
      }
      if( diffESite(o,bx->Event) )
                          ShowEvent( TRIANG, bx->Event, nPEEK, _T("Peek<>Peek") );
      else if( Prcs.TOn ) ShowEvent( TRIANG, bx->Event, PEEK0, _T("PeekPeek") );

      // ## deze omgewerkt tot weak extra convergence ??
      ZgZg_O.Cct->ridEvnt( bx, _T("TCONTCT::PeekPeek( bx )") );
      bx->Peek->PCnt--;
   }
                                         // clear contour (last one)
   if( csam->Event != NULL ) ZgZg_O.Dis->ridEvnt( csam, _T("TCONTCT::PeekPeek( csam )") );
                                         // reconnect and classify HOOKS
   pCONT both = b->other( bsam );
   b1 = HOOK::Classify( both, both->other(b), grab, grab->other(bx), o );
   b2 = HOOK::Unify( bsam, bsam->other(b), csam, csam->other(bx) );
   Rid_Bisc( b );
   Rid_Bisc( bx );
   return TRUE;
}

void TCONTCT::PeekCont() {                  // -- handle peeks of bisector into contour
   PNT2 P1; b->Cnt1->other(b)->BisPt( &P1, Prcs.T ); // of Cnt1
   PNT2 P2; b->Cnt2->other(b)->BisPt( &P2, Prcs.T ); // of Cnt2
                                               // line of discrimination (*s,H)
   PNT2 H; H = P1 + ( P2 - P1 ) * 0.5;         // midpoint of P1 and P2
                                               // .. P1/P2 on the same side as N
   PNT2 S; grab->Bis1->BisPt( &S, Prcs.T );    // point for samesidedness
   REAL X = S.RelDis( o, &H ) * P1.RelDis( o, &H );

   pCONT cN, cF;                               // contours of b: near, far to grab->Bis1
   if( X != 0 ) cN = ( X > 0 ) ? b->Cnt1 : b->Cnt2;
   else {
      grab->Bis2->BisPt( &S, Prcs.T );         // point for othersidedness
      cN = ( S.RelDis(o,&H) * P2.RelDis(o,&H) > 0 ) ? b->Cnt1 : b->Cnt2;
   }
   cF = b->other( cN );                        // cF farest (nearest to c->Bis2)

   splt = Get_Cont( grab->Orign, grab->iDir ); // copy of grab
   splt->Bis1 = grab->Bis2;
   grab->Bis2->value( grab, splt );            // connect bisector c->Bis2 to splt

   // classify both hooks by means of the class HOOK
   b1 = HOOK::Classify( grab, grab->Bis1, cN, cN->other(b), o );
   b2 = HOOK::Classify( splt, splt->Bis1, cF, cF->other(b), o );

   HOOK::ClearPeeks( grab );                   // erase all peeks to grab from event queue
   HOOK::ClearPeeks( b->Cnt1 );                // clear all peeks of contours adjacent to b
   HOOK::ClearPeeks( b->Cnt2 );
   Rid_Bisc( b );
}

void TCONTCT::connectPoly() {                // (dis)connect polygons
   RevalPoly( pl_b, b1 );                    // revalue polygon with b1
   if( pl_b != pl_c ) Rid_Clos( pl_c );      // give polygon back to store
   else {                                    // split polygon into two
      pl_c = Get_Clos();                     // get second polygon
      RevalPoly( pl_c, b2 );                 // revalue polygon with b2
      ZgZg_O.Act->SequenceIn( pl_c );          // put pl_c into working sequence
   }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - TDISINT
TDISINT::TDISINT( pCONT c ): HOOK() {     // -- handle one disintegration
   pol = c->Bis1->Ident;                     // closure of disintegrating contour
   die = c;
   ori = c->Event; c->Event = NULL;

   if( Converge() ) return;                  // look for convergence

   Reach();                                  // possible overdefinition

   pol->Bound = ClassifD();                  // classify HOOK
   pol->Bound->Ident = pol;                  // reconnect with (new) bisector

   HOOK::Repare();                           // check all contacts and rePeek
}

BOOL TDISINT::Converge() {
   // -- test for convergence:
   //    all conartours disintegrate and at the same time and at the same place
   INT2 NofC = VolPoly( pol ),
        Neq = 1,                        // N of events equal to die's event
        Nev = 1;                        // N of events in closure
   BOOL CCv = TRUE;                     // test for complete convergence

   pBISC b = die->Bis1;
   pCONT c = b->other( die );
   while( c != die ) {
      if( c->Event == NULL ) CCv = FALSE;           // c is not deintegrating
      else {
         Nev++;
         if( diffEvent(ori,c->Event) ) CCv = FALSE; // time or place is different
         else Neq++;
      }
      b = c->other( b );
      c = b->other( c );
   }
   // ! at least three contours do not disintegrate at ori --> no convergence
   if( Neq+2 < NofC ) return FALSE;

   // handle incomplete convergence
        if( Nev+1 <  NofC ) ShowEvent( TRIANG, ori, ATTND, _T("several events missing") );
   else if( Nev+1 == NofC ) ShowEvent( TRIANG, ori, ATTND, _T("an event missing") );
   else if( !CCv )          ShowEvent( TRIANG, ori, ATTND, _T("forced convergence") );
   else if( Prcs.Evt )      ShowEvent( TRIANG, ori, ECONV, _T("Convergence") );

   // prepare for convergence: rid all events of polygon, clear all peeks to polygon
   SUPPT Prep( pol );
   do {
      if( Prep.c->Event != NULL ) ZgZg_O.Dis->ridEvnt( Prep.c, _T("TDISINT::Converge( Prep.c )") );
      HOOK::ClearPeeks( Prep.c );        // clear all peeks of contour
   }
   while( Prep.prev() );

   // dismantle all parts of the converging polygon
   b = (pBISC)pol->Bound;
   c = b->Cnt1;
   do {
      Rid_Bisc( b ); b = c->other( b );  // get rid of bisector
      Rid_Cont( c ); c = b->other( c );  // get rid of contour
   } while( b != (pBISC)pol->Bound );
   Rid_Clos( pol );                      // get rid of closure

   if( !Left.Empty() ) {                 // ## deze wordt aktief via ClearPeeks() !!
      Message( L"Converge() -- grab from Left" );
      HOOK::Few_b_Peek_Many_c();         // b in Left try peeking c not in cExcp[]
   }

   // emergence of a circle component (CrossOver)
   if( Prcs.CrossOver ) MakeComp( ori );
   else                 Rid_Tisp( ori );

   return TRUE;
}

void TDISINT::Reach() {
   if( Prcs.Evt ) ShowEvent( TRIANG, ori, EDISI, _T("Disintegration") );
   Clr_Half( 1 );
   Clr_Half( 2 );

   HOOK::ClearPeeks( die );                  // clear all peeks of contour

   Rid_Cont( die );                          // rid contour
   Rid_Bisc( b1 ); b1 = c1->other( b1 );     // rid of b1
   Rid_Bisc( b2 ); b2 = c2->other( b2 );     // rid of b2
}

void TDISINT::Clr_Half( INT2 W ) {
   pBISC b = ( W == 1 ) ? die->Bis1 : die->Bis2,
         b_ = b;
   pCONT c = b->other( die ),
         c_ = c;
                                          // -- get rid of attached events
   while( c != die ) {
      if( c->Event == NULL || diffEvent(ori,c->Event) ) break;
      ZgZg_O.Dis->ridEvnt( c, _T("TDISINT::Clr_Half( c in loop )") );
      if( b->Event != NULL ) {
         Message( L"Clr_Half() -- erase bisector event" );
         ZgZg_O.Cct->ridEvnt( b, _T("TDISINT::Clr_Half( b in loop )") );
         b->Peek->PCnt--;                    // diminish peek count
      }
      b = c->other( b );
      c = b->other( c );
   }
   if( b->Event != NULL ) {
      ZgZg_O.Cct->ridEvnt( b, _T("TDISINT::Clr_Half( b afterwards )") );
      b->Peek->PCnt--;                       // diminish peek count
   }
   if( c->Event != NULL )
      ZgZg_O.Dis->ridEvnt( c, _T("TDISINT::Clr_Half( c afterwards )") );
                                          // -- get rid of structures
   if( W == 1 ) { c1 = c; b1 = b; }
   else         { c2 = c; b2 = b; }

   while( c_ != c ) {
      HOOK::ClearPeeks( c_ );                // clear all peeks of contour
      Rid_Bisc( b_ ); b_ = c_->other( b_ );
      Rid_Cont( c_ ); c_ = b_->other( c_ );
   }
}

// -- construct one HOOK or even it out into one contour
pBISC TDISINT::ClassifD() {
   if( c1->iDir != c2->iDir || c1->Orign != c2->Orign ) {
      if( Prcs.ExtraEmer() ) {
         pTISP emg = Get_Tisp( ori->TimB );   // copy ori for extra emergence
         emg->Assign( ori->X, ori->Y );
         MakeComp( emg );
      }
      return HOOK::Classify( c1, b1, c2, b2, ori );
   }

   // c1 and c2 have same direction and same origin ! they are from the same contour
   if( ori != NULL ) {
      if( Prcs.TOn ) ShowEvent( TRIANG, ori, GLUED, _T("Contour restored") );
      if( Prcs.ExtraEmer() ) MakeComp( ori );
      else                   Rid_Tisp( ori );
   }
   return HOOK::Unify( c1, b1, c2, b2 );
}
