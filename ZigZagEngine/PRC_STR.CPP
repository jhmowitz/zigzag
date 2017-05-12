// prc_str.cpp

#include <stdio.h>
#include <math.h>          // ABS(), COS(), SIN()

#include "mow_defs.h"      // incriptions for includes (use long double)
#include "alg_geo.h"       // structures for points, lines and circles

#include  MOW_LINK_CPP     // linkage: class definition and source
                           // .. serve memory storage of link classes

#define  _DIRECT_CPP       // to include code for specially modelled directions
#include "prc_str.h"       // serving structures
#undef   _DIRECT_CPP

#include "prc_def.h"       // general process classes
#include "cir_str.h"       // definition of arc support class SUPPC
#include "tri_str.h"       // definition of contour support class SUPPT

#include "doscompat.h"

// -- declaration of common memory control: bulk allocation of classes with storage
                              // -- guard memory classes for production classes
GMEMO< TISP, 600, 5000 > MTisp;   // time/space

GMEMO< CLOS, 300, 2000 > MClos;   // closures: components or polygons

GMEMO< ARCS, 400, 5000 > MArcs;   // arcs of circles
GMEMO< BUMP, 500, 3000 > MBump;   // pairs of arcs, to bump into each other

GMEMO< CONT, 400, 5000 > MCont;   // contour, one piece of line
GMEMO< BISC, 600, 5000 > MBisc;   // angular bisector

//GMEMO< OPPS, 10, 1000 > MOpps;   // 2 peeks from opposite appearence

void InitClasses() {
   MTisp.Reset();                // free memory space for production classes
   MClos.Reset();
   MArcs.Reset(); MBump.Reset();
   MCont.Reset(); MBisc.Reset(); // MOpps.Reset();

   Bubb.ClearTops();             // clear tops of BUBBLE
   ZgZg.ClearTops();             // clear tops of ZIGZAG
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - HANDC
void HANDC::ClearTops() {
   Emg->Clear();                       // clear top of emerging components
   for( init(); valid(); incr() ) {    // clear (generation) top classes
      Act->Link( Act ); Act->Prev = Act;
      Dis->Clear();
      Cct->Clear();
   }
   initGen();                          // reset generation variables
}

// ---------------------------------------------------------------------- handle time/space
pTISP Get_Tisp( REAL T ) {          // -- make new time/space
   pTISP ts = MTisp.Get();             // get TISP from store
   ts->X = ts->Y = 0;                  // no coordinates yet
   ts->nRef = 0;                       // ! all references to TISP are zero
   ts->TimB = T;                       // ! set time of birth
   return ts;
}
void Rid_Tisp( pTISP ts ) {         // -- give time/space back
   MTisp.Put( ts );                    // give ts back to storage (MTisp.Keep)
}
void TISP::incRef() {               // -- increment references to this
   nRef++;                             // !! increment
#ifdef _CLASS_COUNT                    // do class count for program testing
   if( Prcs.XO < nRef )                // update maximum number of references to an Orign
      Prcs.XO = nRef;                  // .. used for class count (test)
#endif                                 //  _CLASS_COUNT
}
void TISP::decRef() {                  // decrement number of references
   if( --nRef == 0 ) Rid_Tisp( this ); // decrement [and give TISP back to storage]
}

// ---------------------------------------------------------------------- handle closures
pCLOS Get_Clos() {                  // -- get a new closure
   pCLOS cls = MClos.Get();            // get closure from store ! no pointer clearance
   cls->AEgo = NULL;                   // no other closure yet
   cls->Bound = NULL;                  // no arc yet
   return cls;
}
void Rid_Clos( pCLOS cls ) {        // -- give closure back
   cls->DeSequence();                  // cut closure from its top
   MClos.Put( cls );                   // link cls to MClos.Keep
}

void ordClosure( pCLOS cls, pLINK top ) { // find place in time sequence ! no prototype
   pLINK c = top;                         // .. equal time value is placed behind !
   pCLOS n;
   while( (n=(pCLOS)c->Next()) != NULL )
      if( cls->Bound->Orign->TimB < n->Bound->Orign->TimB ) break;
      else c = n;
   c->Push( cls );                        // sequence cls behind c
}

// ---------------------------------------------------------------------- handle components
void MakeComp( pTISP o ) {             // -- implement new component
   pCLOS cmp = Get_Clos();                // get closure from store
   cmp->Bound = Get_Arcs( cmp, o );       // get initial boundary arc from store
   ordClosure( cmp, Bubb.Emg );           // link component into emergence queue
}
void RevalComp( pCLOS cmp, pARCS a ) { // -- revalue component
   cmp->Bound = a;                        // set component's reference to a
   pARCS c = a;
   do c->Ident = cmp;                     // set all arcs' references to cmp
   while( (c=c->Neigh) != a );
}
INT2 VolComp( pCLOS cmp ) {            // -- number of arcs in one component
   INT2  N = 0;
   pARCS c = (pARCS)cmp->Bound;
   do N++; while( (c=c->Neigh) != (pARCS)cmp->Bound );
   return N;
}

// ---------------------------------------------------------------------- handle Polygons
pCLOS Make1Poly( INT2 N, pREAL arcs, REAL Flip, pTISP o ) { // init part of a new polygon
   pCLOS pol = Get_Clos();
   pBISC b;
   pCONT c[2], c1;                       // c1 keeps first contour for last bisector

   c[0] = c1 = Get_Cont( o, ContourDir(arcs[0]+Flip) );
   for( INT2 i = 0, w = 0; i < N; i++, w = 1 - w ) {
      INT2 j = ( i + 1 ) % N;            // next of i
                                         // use next contour or first one again
      c[1-w] = ( j != 0 ) ? Get_Cont( o, ContourDir(arcs[j]+Flip) ) : c1;
      b = Get_Bisc( o, c[w], c[1-w] );   // get bisector
      c[w]->Bis1 = c[1-w]->Bis2 = b;     // adjacent contours refer to b
   }

   RevalPoly( pol, b );                  // revalue polygon
   BlendPoly( pol, o->TimB );            // determine types of contors/bisectors
   return pol;
}

void MakePoly( INT2 N, pREAL arcs, pTISP o ) {  // -- init a new component
   pCLOS p_con = Make1Poly( N, arcs, 0.0, o );     // init continous part
   pCLOS p_ref = Make1Poly( N, arcs, _PI, o );     // init reflective part
   p_con->AEgo = p_ref;                            // triangles refer to each other
   p_ref->AEgo = p_con;
   ordClosure( p_con, ZgZg.Emg );                  // link polygon into emergence queue

/*
   FILE *tlst = fopen( "test.lst", "a" );
   SUPPT S( pol );
   do FPRINTF( tlst, "ENWS: contour %lu - bisector %lu\n", S.c->iDir, S.b->iDir );
   while( S.prev() );
   fclose( tlst );
*/
}

void MakeDoubleV( pPNT2 p, REAL Dir, REAL Dev, BOOL Conv1c, BOOL Conv2c ) {
   // ## conv1c, conv2c werkt niet !?

   // small deviation from deviation Dev to converge in one circle
   const REAL Prop1c = 0.0000011;
   // small deviation from deviation Dev to converge in two circles
   const REAL Prop2c = 0.0000012;
   // proportion of time bonus Bon for second V
   const REAL Prop = 0.6;

//   REAL Fut = 0.01 * ( Prcs.S + Prcs.X ); // future from current time
//   S = .0007;                   // time step for animation
   REAL Fut = 0.00001;                    // future from current time
   REAL Tim = Prcs.T + Fut;               // tijdstip van opkomst
   REAL Bon = 0.01 * Fut;                 // time bonus

   REAL D1c = Prop1c * Dev,
        D2c = Prop2c * Dev;

   ENWS dir = ContourDir( Dir ),
        dev = ContourDir( HPI - Dev ),
        d1c = Conv1c ? ContourDir( HPI - Dev - D1c ) : dev,
        d2c = Conv2c ? ContourDir( HPI - Dev - D2c ) : dev;

   // need p and 4 more points for bisectors

   pCLOS pol = Get_Clos();

   // initilize at Tim
   // when emerged:   o50->T -= Bon;
   //                 o14->T += Bon;
   //                 o23->T -= Bon * Prop;

   pTISP o50 = Get_Tisp( Tim-Bon );      o50->Assign( p->X, p->Y );
   pTISP o14 = Get_Tisp( Tim+Bon );      o14->Assign( p->X, p->Y );
   pTISP o23 = Get_Tisp( Tim-Bon*Prop ); o23->Assign( p->X, p->Y );

   pCONT c0 = Get_Cont( o50, SubDir(dir,dev) ),
         c1 = Get_Cont( o14, AddDir(dir,d1c) ),
         c2 = Get_Cont( o23, SubDir(dir,d2c) ),
         c3 = Get_Cont( o23, AddDir(dir,d2c) ),
         c4 = Get_Cont( o14, SubDir(dir,d1c) ),
         c5 = Get_Cont( o50, AddDir(dir,dev) );

   LIN2 C0; c0->CntLn( &C0, Tim );
   LIN2 C1; c1->CntLn( &C1, Tim );
   LIN2 C2; c2->CntLn( &C2, Tim );
   LIN2 C3; c3->CntLn( &C3, Tim );
   LIN2 C4; c4->CntLn( &C4, Tim );
   LIN2 C5; c5->CntLn( &C5, Tim );

   // get and compute points o01, o12, o34, o45
   pTISP o01 = Get_Tisp( Tim ); o01->IncidP( &C0, &C1 );
   pTISP o12 = Get_Tisp( Tim ); o12->IncidP( &C1, &C2 );
   pTISP o34 = Get_Tisp( Tim ); o34->IncidP( &C3, &C4 );
   pTISP o45 = Get_Tisp( Tim ); o45->IncidP( &C4, &C5 );

   pBISC b;
   b = Get_Bisc( o50, c5, c0 ); c5->Bis1 = c0->Bis2 = b;
   b = Get_Bisc( o01, c0, c1 ); c0->Bis1 = c1->Bis2 = b;
   b = Get_Bisc( o12, c1, c2 ); c1->Bis1 = c2->Bis2 = b;
   b = Get_Bisc( o23, c2, c3 ); c2->Bis1 = c3->Bis2 = b;
   b = Get_Bisc( o34, c3, c4 ); c3->Bis1 = c4->Bis2 = b;
   // ! make sure bisector for pol is of time Tim
   b = Get_Bisc( o45, c4, c5 ); c4->Bis1 = c5->Bis2 = b;

   RevalPoly( pol, b );                  // revalue polygon
   BlendPoly( pol, Tim );                // determine types of contors/bisectors
   ordClosure( pol, ZgZg.Emg );          // link polygon into emergence queue
}

void RevalPoly( pCLOS p, pBISC bNew ) {  // revalue polygon
   p->Bound = bNew;
   SUPPT S( p );
   do S.b->Ident = p; while( S.prev() );
}
void BlendPoly( pCLOS p, REAL Tim ) {    // determine types of contors/bisectors
   SUPPT S( p );                         // classify .. in new polygon
   do { S.c->blend( Tim+1 );
        S.b->blend( Tim+1 ); } while( S.prev() );
}
INT2 VolPoly( pCLOS p ) {                // N contours/bisectors in polygon
   INT2  N = 0;
   SUPPT S( p ); do N++; while( S.prev() );
   return N;
}

// ----------------------------------------------------------------------------------- ETOP
void ETOP::ordEvnt( pEVNT evt ) {             // link actor in time of Event
   pLINK cur = this;                          // equal time value is placed behind !
   pEVNT nxt;
   while( (nxt=(pEVNT)cur->Next()) != NULL )
      if( evt->Event->TimB < nxt->Event->TimB ) break;
      else cur = nxt;
   cur->Push( evt );                          // sequence evt behind cur and before nxt
}
void ETOP::getEvnt( pLINK evt, pCHAR defS ) { // delink actor from time order
   pLINK cur = this, nxt;
   while( (nxt=cur->Next()) != NULL )
      if( nxt == evt ) {                      // found class
         cur->Pop();
         return;
      }
      else cur = nxt;

   Exit( L"getEvnt(%s) ?? event not found", defS );
}
void ETOP::ridEvnt( pEVNT evt, pCHAR defS ) { // unlink event from time order
   getEvnt( evt, defS );
   Rid_Tisp( evt->Event );                    // rid event time/space
   evt->ClearEvent();                         // destroy reference
}

// ---------------------------------------------------------------------------- handle arc
pARCS Get_Arcs( pCLOS cp, pTISP ts ) {      // implement a new arc
   pARCS a = MArcs.Get();                   // get arc from store
   a->ClearEvent();                         // no event yet
   a->Ident = cp;                           // ! access to component
   a->Neigh = a;                            // ! the arc starts with self-reference
   a->Type = AFULL;                         // ! arc of no domain yet (full)
   a->BCnt = 0;                             // no bump yet
   a->Orign = ts;                           // ! origin for future parts of the arc
   ts->incRef();                            // !! increment number of references to ts
   return a;
}
void Rid_Arcs( pARCS a ) {                  // give arc back
   a->Orign->decRef();                      // decrement number of references
   MArcs.Put( a );                          // link a to MArcs.Keep
}
pARCS ARCS::previous() {                    // get arc previous in linkage for component
   pARCS a = this;
   while( a->Neigh != this ) a = a->Neigh;
   return a;
}

// ----------------------------------------------------------------- handle point bisector
pBUMP Get_Bump( pARCS a1, pARCS a2, REAL T ) { // get a new point bisector
   pBUMP b = MBump.Get();                      // get bump from store
   b->Arc1 = a1; a1->BCnt++;                   // update bump count
   b->Arc2 = a2; a2->BCnt++;                   // for both arcs
   b->Event = Get_Tisp( T );                   // ! event is obligatory for a bump
   return b;
}
void Rid_Bump( pBUMP b ) {                     // give point bisector back
   Rid_Tisp( b->Event );                       // rid event (only for bumps)
   b->Arc1->BCnt--;                            // diminish bump counts
   b->Arc2->BCnt--;                            // .. for both arcs
   MBump.Put( b );                             // link b to MBump.Keep
}

pARCS BUMP::other( pARCS c ) {                 // other arc to point bisector
   if( Arc1 == c ) return Arc2;
   if( Arc2 != c ) Exit( L"BUMP::other() -- does not find arc" );
   return Arc1;
}
void BUMP::value( pARCS aO, pARCS aN ) {       // revalue arc of point bisector
        if( Arc1 == aO ) Arc1 = aN;
   else if( Arc2 == aO ) Arc2 = aN;
   else Exit( L"BUMP::value() -- does not find arc" );
   aN->BCnt++;                                 // keep bump count up-to-date
   aO->BCnt--;
}

// ------------------------------------------------------------------------ handle contour
pCONT Get_Cont( pTISP ts, ENWS Dir ) {   // get a new contour
   pCONT c = MCont.Get();                // get contour from store
   c->ClearEvent();                      // no event yet
   c->Bis1 = c->Bis2 = NULL;             // no linked bisectors yet
   c->PCnt = 0;                          // no peek yet
   c->iDir = Dir;                        // ! direction
   c->Orign = ts;                        // ! time/space of birth/origine
   ts->incRef();                         // !! increment number of references to ts
   return c;
}
void Rid_Cont( pCONT c ) {               // give contour back
   if( c->PCnt != 0 ) Message( L"Rid_Cont() .. non-zero peek count %d", c->PCnt );
   c->Orign->decRef();                   // decrement number of references
   MCont.Put( c );                       // link c to MCont.Keep
}

pBISC CONT::other( pBISC b ) {           // find the bisector different from b
   if( Bis1 == b ) return Bis2;
   if( Bis2 != b ) Exit( L"CONT::other() -- does not find bisector" );
   return Bis1;
}
void CONT::value( pBISC bO, pBISC bN ) { // revalue bisector of contour ## unused
        if( Bis1 == bO ) Bis1 = bN;
   else if( Bis2 == bO ) Bis2 = bN;
   else Exit( L"CONT::value() -- does not find bisector" );
}
void CONT::valot( pBISC bO, pBISC bN ) { // revalue other bisector of contour
        if( Bis1 == bO ) Bis2 = bN;
   else if( Bis2 == bO ) Bis1 = bN;
   else Exit( L"CONT::valot() -- does not find bisector" );
}

void CONT::blend( REAL T ) {             // classify a contour
   if( Bis1->other(this)->iDir == Bis2->other(this)->iDir ) {
      if( Prcs.TOn ) Message( L"CONT::blend() -- contour of constant length" );
      Type = CONST;
      return;
   }
   LIN2 B1; Bis1->BisLn( &B1, T, T+1 );
   LIN2 B2; Bis2->BisLn( &B2, T, T+1 );
   REAL P1 = B1.Tail.RelPar( &B2 ),
        P2 = B2.Tail.RelPar( &B1 );
        if( P1 > 0 && P2 > 0 )             Type = CVANE;
   else if( P1 < 0 && P2 < 0 )             Type = CGROW;
   else {
      REAL D0 = B2.Tail.RelDis( &B1 ),
           D1 = B2.Head.RelDis( &B1 );
      if( D0*D1 > 0 && ABS(D0) < ABS(D1) ) Type = CGFLD;
      else                                 Type = CVFLD;
   }
}
                                                  // -- for construction
void CONT::CntLn( pLIN2 l, REAL T ) {                // contour line of constant length (2)
   REAL c = COS( D() ),                              // construct two points on the
        s = SIN( D() ),                              // .. perpendicular on (p,D)
        d = T - Orign->TimB;                         // .. at distance d from origin
   l->Tail.Assign( Orign->X+d*c-s, Orign->Y+d*s+c ); // point left of (p,D)
   l->Head.Assign( Orign->X+d*c+s, Orign->Y+d*s-c ); // point right of (p,D)
}

// ----------------------------------------------------------------- handle angular bisector
pBISC Get_Bisc( pTISP ts, pCONT c1, pCONT c2 ) {
   pBISC b = MBisc.Get();            // get a new bisector
   b->ClearEvent();                  // no event yet
   b->Ident = NULL;                  // no polygon yet
   b->Peek = NULL;                   // no contour to peek yet
   b->Cnt1 = c1;                     // an adjacent contour
   b->Cnt2 = c2;                     // the other adjacent contour
   b->DirVelo( c1->iDir, c2->iDir ); // ! ENWS for direction, parameter to compute velocity
   b->Orign = ts;                    // ! time/space of birth/origine
   ts->incRef();                     // !! increment number of references to ts
   return b;
}
void Rid_Bisc( pBISC b ) {           // give bisector back
   b->Orign->decRef();               // decrement number of references
   MBisc.Put( b );                   // give b back to storage (MBisc.Keep)
}

pCONT BISC::other( pCONT c )  {            // find the contour different from c
   if( Cnt1 == c ) return Cnt2;
   if( Cnt2 != c ) Exit( L"BISC::other() -- does not find contour" );
   return Cnt1;
}
void BISC::value( pCONT cO, pCONT cN ) {   // revalue contour of bisector
        if( Cnt1 == cO ) Cnt1 = cN;
   else if( Cnt2 == cO ) Cnt2 = cN;
   else Exit( L"BISC::value() -- does not find contour" );
}

void BISC::blend( REAL T ) {               // classify a bisector
   LIN2 B; BisLn( &B, T, T+1 );
   PNT2 P1; Cnt1->other(this)->BisPt( &P1, T );
   PNT2 P2; Cnt2->other(this)->BisPt( &P2, T );
   REAL D1 = P1.RelPar( &B ),
        D2 = P2.RelPar( &B );
   if( D1 == 0 || D2 == 0 ) Message( L"BISC::blend() .. bisector of 180 degrees" );
        if( D1 > 0 && D2 > 0 ) Type = BVANE;
   else if( D1 < 0 && D2 < 0 ) Type = BGROW;
   else                        Type = BFOLD;
}
                                        // -- for construction
void BISC::BisPt( pPNT2 p, REAL T ) {      // top point of bisector at T
   *p = *Orign + UnitVec2( D() ) * ( VFac * ( T - Orign->TimB ) );
}
void BISC::BisLn( pLIN2 l, REAL T0, REAL T1 ) {
   BisPt( &l->Tail, T0 );                  // make piece of bisector in time
   BisPt( &l->Head, T1 );                  // from time T0 to time T1
}

// -------------------------------------------------------------------- TEST (count classes)
#ifdef _CLASS_COUNT            // class count for program testing is done
BOOL CheckNClass();            // check for lost classes
INT2 NRefEvent();              // count all (Event) time/spaces
INT2 NTopEvent();              // count all (Events from tops) time/spaces
INT2 NOrign();                 // count all (Orign) time/spaces
void PrintNClass( CTYP );      // print for lost classes
void PrintNTisp();             // print (lost) time/spaces
INT2 N_CD();                   // N of disintegrations in all generations
INT2 N_CC();                   // N of contacts in all generations
INT2 NComp();                  // N of components in all generations
INT2 MassComp();               // N of arcs in all generations
INT2 NPoly();                  // N of polygons in sequence
INT2 MassPoly();               // M contours/bisectors in all polygons
INT2 MassZEmg();               // M contours/bisectors in all polygons of emergence
#endif       //  _CLASS_COUNT

BOOL  ToFile = TRUE;
pFILE ShwC;

void TEST::Count() {
#ifdef _CLASS_COUNT            // class count for program testing is done
   if( !Cnt ) return;
   if( !CheckNClass() ) return;
   TOn = TRUE;
   EvTable();
   Exit( L"TEST::Count() -- bad counts" );
#endif       //  _CLASS_COUNT
}

#ifdef _CLASS_COUNT            // class count for program testing is done
void TEST::EvTable() {
   if( ToFile ) ShwC = FOPEN( _T("ScrClass.lst"), _T("w") );

   BOOL Q = Prcs.IsOpen;
   if( Q ) Prcs.Close();
   PRINTF( L"             Cir   Tri" );
   if( TOn ) PRINTF( L"    BUBBLE                  ZIGZAG" );
   if( ToFile ) {
      FPRINTF( ShwC, L"             Cir   Tri" );
      if( TOn ) FPRINTF( ShwC, L"    BUBBLE                  ZIGZAG" );
   }

   PRINTF( L"\n N of      %5d %5d", NComp(), NPoly() );
   if( TOn ) PRINTF( L"    memo  idle actv lost    memo  idle actv lost" );
   if( ToFile ) {
      FPRINTF( ShwC, L"\n N of      %5d %5d", NComp(), NPoly() );
      if( TOn ) FPRINTF( ShwC, L"    memo  idle actv lost    memo  idle actv lost" );
   }

   PRINTF( L"\n\nEmergence  %5d %5d", Bubb.Emg->NN(), ZgZg.Emg->NN() );
   if( ToFile ) 
      FPRINTF( ShwC, L"\n\nEmergence  %5d %5d", Bubb.Emg->NN(), ZgZg.Emg->NN() );
   if( TOn ) PrintNClass( ECONV );     // check for lost components/polygons

   PRINTF( L"\n\nDisintegr. %5d %5d", N_CD(), ZgZg.Dis->NN() );
   if( ToFile ) 
      FPRINTF( ShwC, L"\n\nDisintegr. %5d %5d", N_CD(), ZgZg.Dis->NN() );
   if( TOn ) PrintNClass( EDISI );     // check for lost arcs/contours

   PRINTF( L"\n\nContact    %5d %5d", N_CC(), ZgZg.Cct->NN() );
   if( ToFile ) 
      FPRINTF( ShwC, L"\n\nContact    %5d %5d", N_CC(), ZgZg.Cct->NN() );
   if( TOn ) PrintNClass( ECTCT );     // check for lost (point) bisectors
   if( TOn ) PrintNTisp();             // print lost time/spaces

   INT2 kB = MTisp.KB() + MClos.KB() + MArcs.KB() + MBump.KB()
                                     + MCont.KB() + MBisc.KB();
   PRINTF( L"\n\nTime = %Lg  -- %d kb", Prcs.T, kB );  // time, allocated kbyte
   PRINTF( L"\nStep = %Lg", Prcs.S );                  // and time step
   if( ToFile ) {
      FPRINTF( ShwC, L"\n\nTime = %Lg  -- %d kb", Prcs.T, kB );  // time, allocated kbyte
      FPRINTF( ShwC, L"\nStep = %Lg", Prcs.S );                  // and time step
   }

   Sleep( TOn ? 60 : 20 );
   if( Q ) Prcs.Open();

   if( ToFile ) {
      fclose( ShwC );
      ToFile = FALSE;
   }
}

// -------------------------------------------------------------------------- count numbers
BOOL CheckNClass() {                         // check for lost classes
   INT2 Lc = Bubb.Emg->NN(),
        Lt = ZgZg.Emg->NN(),
        ME = MassZEmg();

   if( MClos.N_() != MClos.NS()+NComp()+Lc
                               +NPoly()+Lt )    return TRUE;  // closures

   if( MArcs.N_() != MArcs.NS()+MassComp()+Lc ) return TRUE;  // arcs
   if( MBump.N_() != MBump.NS()+N_CC() )        return TRUE;  // point bisectors
   if( MCont.N_() != MCont.NS()+MassPoly()+ME ) return TRUE;  // contours
   if( MBisc.N_() != MBisc.NS()+MassPoly()+ME ) return TRUE;  // angular bisectors

   INT2 A = MTisp.N_(),                      // N of allocated
        I = MTisp.NS(),                      // N of storage time/space
        R = NRefEvent(),                     // N of Events from reference
        T = NTopEvent(),                     // N of Events from tops
        O = NOrign();                        // N of Orign
   if( R != T || A != I+R+O ) return TRUE;   // bad count

   return FALSE;                             // ! good count
}
INT2 NRefEvent() {                           // count all (Event) time/spaces
   INT2 N = N_CC();                                // .. bumps of two arcs
   for( Bubb.init(); Bubb.valid(); Bubb.incr() ) { // .. arc disintegrations
      pCLOS cp = Bubb.Act;
      while( (cp=cp->Prev) != Bubb.Act ) {
         SUPPC S( cp, NEIGHBOUR, Prcs.T );
         do if( S.c->Event != NULL ) N++; while( S.next() );
      }
   }
   pCLOS p = ZgZg.Act;
   while( (p=p->Prev) != ZgZg.Act ) {
      SUPPT T( p );
      do { if( T.c->Event != NULL ) N++;         // .. contour disintegrations
           if( T.b->Event != NULL ) N++; } while( T.prev() ); // .. peeks
   }
   return N;
}
INT2 NTopEvent() {            // count all (Events from tops) time/spaces
   return N_CD() + N_CC() +                // .. or from tops: arcs
          ZgZg.Dis->NN() + ZgZg.Cct->NN(); // .. triangles
}
INT2 NOrign() {                      // count all (Orign) time/spaces
   INT2  N = Bubb.Emg->NN() + ZgZg.Emg->NN();
   pINT2 Cf = new INT2[Prcs.XO];
   INT2 i, A;
   for( i = 0; i < Prcs.XO; i++ ) Cf[i] = 0;
   for( Bubb.init(); Bubb.valid(); Bubb.incr() ) {
      pCLOS cp = Bubb.Act;
      while( (cp=cp->Prev) != Bubb.Act ) {
         SUPPC S( cp, NEIGHBOUR, Prcs.T );
         do {
            if( (A=S.c->Orign->nRef) < 1 ) Exit( L"NOrign(Cir) .. nRef == 0" );
            if( (++Cf[A-1])%A == 0 ) N++;
         } while( S.next() );
      }
      for( i = 0; i < Prcs.XO; i++ )
         if( Cf[i]%(i+1) != 0 ) Exit( L"NOrign(Cir) .. incomplete" );
   }
   pCLOS p = ZgZg.Act;
   while( (p=p->Prev) != ZgZg.Act ) {
      SUPPT T( p );
      do {
         if( (A=T.b->Orign->nRef) < 1 ) Exit( L"NOrign(Tri) .. nRef == 0" );
         if( (++Cf[A-1])%A == 0 ) N++;
         if( (A=T.c->Orign->nRef) < 1 ) Exit( L"NOrign(Tri) .. nRef == 0" );
         if( (++Cf[A-1])%A == 0 ) N++;
      } while( T.prev() );
   }
   for( i = 0; i < Prcs.XO; i++ )
      if( Cf[i]%(i+1) != 0 ) Exit( L"NOrign(Tri) .. incomplete" );
   delete Cf;
   return N;
}
INT2 N_CD() {         // count circle disintegrations from generation queues
   INT2 N = 0;
   for( Bubb.init(); Bubb.valid(); Bubb.incr() ) N += Bubb.Dis->NN();
   return N;
}
INT2 N_CC() {         // count circle bumps from generation queues
   INT2 N = 0;
   for( Bubb.init(); Bubb.valid(); Bubb.incr() ) N += Bubb.Cct->NN();
   return N;
}
INT2 NComp() {                   // N of components in all generations
   INT2 N = 0;
   for( Bubb.init(); Bubb.valid(); Bubb.incr() ) {
      pCLOS cmp = Bubb.Act;
      while( (cmp=cmp->Prev) != Bubb.Act ) N++;
   }
   return N;
}
INT2 MassComp() {                // N of arcs in all generations
   INT2 N = 0;
   for( Bubb.init(); Bubb.valid(); Bubb.incr() ) {
      pCLOS cmp = Bubb.Act;
      while( (cmp=cmp->Prev) != Bubb.Act ) N += VolComp( cmp );
   }
   return N;
}
INT2 NPoly() {                   // N of polygons in sequence
   INT2 N = 0;
   pCLOS p = ZgZg.Act;
   while( (p=p->Prev) != ZgZg.Act ) N++;
   return N;
}
INT2 MassPoly() {                // N contours/bisectors in all polygons
   INT2  N = 0;
   pCLOS p = ZgZg.Act;
   while( (p=p->Prev) != ZgZg.Act ) N += VolPoly( p );
   return N;
}
INT2 MassZEmg() {               // M contours/bisectors in all polygons of emergence
   INT2  N = 0;
   pCLOS p = (pCLOS)ZgZg.Emg;
   while( (p=(pCLOS)p->Next()) != NULL ) N += VolPoly( p );
   return N;
}

// -------------------------------------------------------------------------- print numbers
void PrintNClass( CTYP Which ) { // check for lost classes
   INT2 Ac, Ic, Nc, Sc,          // A(lloc), I(dle), N(in use) c(CIRCLE)
        At, It, Nt, St;          // S(memory strips)           t(TRIANG)
   INT2 &Dc = Nc, &Dt = Nt;      // dummies of Nc, Nt (components/polygons are closures)
   switch( Which ) {
      case ECONV:                                           // closures
         Sc = MClos.KB(); Ic = MClos.NS();                     // CIRCLE
         Ac = MClos.N_(); Nc = NComp() + Bubb.Emg->NN();
         St = MClos.KB(); It = MClos.NS();                     // TRIANG
         At = MClos.N_(); Nt = NPoly() + ZgZg.Emg->NN();
         Dc = Dt = Nc + Nt;
         break;
      case EDISI:                                           // disintegrations
         Sc = MArcs.KB(); Ic = MArcs.NS();                     // CIRCLE
         Ac = MArcs.N_(); Nc = MassComp() + Bubb.Emg->NN();
         St = MCont.KB(); It = MCont.NS();                     // TRIANG
         At = MCont.N_(); Nt = MassPoly() + MassZEmg();
         break;
      case ECTCT:                                           // contacts
         Sc = MBump.KB(); Ic = MBump.NS();                     // CIRCLE
         Ac = MBump.N_(); Nc = N_CC();
         St = MBisc.KB(); It = MBisc.NS();                     // TRIANG
         At = MBisc.N_(); Nt = MassPoly() + MassZEmg();
         break;
   }
   PRINTF( L"    (%2d) %5d %4d %4d", Sc, Ic, Nc, Ac-Ic-Dc ); // CIRCLE
   PRINTF( L"    (%2d) %5d %4d %4d", St, It, Nt, At-It-Dt ); // TRIANG
   if( ToFile ) {
      FPRINTF( ShwC, L"    (%2d) %5d %4d %4d", Sc, Ic, Nc, Ac-Ic-Dc ); // CIRCLE
      FPRINTF( ShwC, L"    (%2d) %5d %4d %4d", St, It, Nt, At-It-Dt ); // TRIANG
   }
}

void PrintNTisp() {                       // -- print lost time/spaces
   INT2 A = MTisp.N_(), I = MTisp.NS(),      // N of allocated, N of idle
        R = NRefEvent(), T = NTopEvent(),    // N of events from reference or tops
        O = NOrign();                        // N of Orign

   PRINTF( L"\n\n\nreport of time/spaces" );
   PRINTF( L"\nidle     %5d", I );
   PRINTF( L"\nreferred %5d   (top count %d)", R, T );
   PRINTF( L"\norigine  %5d", O );
   PRINTF( L"\n-------------" );
   PRINTF( L"\nfound    %5d, alloc %5d (%d)", I+R+O, A, MTisp.KB() );

   if( ToFile ) {
      FPRINTF( ShwC, L"\n\n\nreport of time/spaces" );
      FPRINTF( ShwC, L"\nidle     %5d", I );
      FPRINTF( ShwC, L"\nreferred %5d   (top count %d)", R, T );
      FPRINTF( ShwC, L"\norigine  %5d", O );
      FPRINTF( ShwC, L"\n-------------" );
      FPRINTF( ShwC, L"\nfound    %5d, alloc %5d (%d)", I+R+O, A, MTisp.KB() );
   }
}
#endif       //  _CLASS_COUNT
