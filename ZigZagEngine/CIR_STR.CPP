// cir_str.cpp  -- BUBBLE: process to handle all circle events

#include "mow_defs.h"           // incriptions for includes (use long double)
#include "alg_geo.h"            // structures for points, lines and circles
#include  MOW_LINK_H            // linkage: include class definition
#include "prc_str.h"            // serving process specific structures

#include "prc_def.h"            // all general process classes

#define  _CIR_STR_CPP           // to include some definitions only here
#include "cir_str.h"            // definition of arc support class SUPPC
#undef   _CIR_STR_CPP

// ##### 2018-04-13
extern void ShowEvent( ScrDrawCirc, ScrDrawTria, pPNT2, CTYP, pCHAR );
extern void ShowCircle( pPNT2, REAL, REAL, REAL, CTYP, uINT2 );

// -- pop the first event (of circles) from the event queue and execute it
void DoCirEmerge() { C_WEAVE W( (pCLOS)Bubb.Emg->Pop() ); } // an emergence
void DoCirContct() { CCONTCT C( (pBUMP)Bubb.Cct->Pop() ); } // a contact/bump
void DoCirDisint() { CDISINT D( (pARCS)Bubb.Dis->Pop() ); } // a disintegration

// ------------------------------------------------------------------------------ C_WEAVE
void C_WEAVE::WeaveIn() {
   // ##### 2018-04-13
   // if( Prcs.Evt ) ShowEvent( CIRCLE, cp, EMERG, _T("Emergence") );
                                // compute generation for component newC
   for( Bubb.init(); Bubb.valid(); Bubb.incr() )
      if( Outside() ) break;

   if( !Bubb.valid() ) {        // add newC to existing (deepest) generation
      Bubb.addGen();            
      if( Prcs.TOn ) Message( L"Open circle generation %d", Bubb.curGen()+1 );
   }

   pBUMP b = (pBUMP)Bubb.Cct, n;                       // purify Bubb.Cct by newC
   while( (n=(pBUMP)b->Next()) != NULL )
      if( Distan(cp,n->Event) > n->Event->TimB-TimE ) b = n;
      else Rid_Bump( (pBUMP)b->Pop() );                // newC(cp) disturbes n->Event

   Keep.Clear();                                    // -- to collect bumps of newC
   pCLOS cmp = Bubb.Act;                               // try bumping with all arcs
   while( (cmp=(pCLOS)cmp->Next()) != Bubb.Act )       // get all bumps of cmp
      TryBumps( cmp );                                 // .. with newC
   Bubb.Act->SequenceIn( newC );                       // sequence newC behind Bubb.Act
   while( !Keep.Empty() )                              // put all bumps for arc
      Bubb.Cct->ordEvnt( (pEVNT)Keep.Pop() );          // .. of newC to Bubb.Cct
}

BOOL C_WEAVE::Outside() {                       // to find generation of component
   pCLOS cmp = Bubb.Act;
   while( (cmp=(pCLOS)cmp->Next()) != Bubb.Act ) {
      pARCS c = (pARCS)cmp->Bound;
      do if( Distan(c->Orign,cp) < TimE-c->Orign->TimB )
            return FALSE;                       // new arc is inside c
      while( (c=c->Neigh) != (pARCS)cmp->Bound );
   }
                       // check for holes in representation
   INT2 Ncrs = 0;      // N of crosses over demi-line parallel to pos. y-axis
   while( (cmp=(pCLOS)cmp->Next()) != Bubb.Act ) {
      if( VolComp(cmp) < 3 ) continue;
      SUPPC Out( cmp, DIRECTION, TimE );
      BOOL  Left = (BOOL)( Out.pp->X < cp->X );  // set starting position
      do if( (  Left && Out.cp->X > cp->X ) ||
             ( !Left && Out.cp->X < cp->X ) ) {
            Left = !Left;
            if( (Out.cp->Y-Out.pp->Y) * (cp->X-Out.pp->X) /
                (Out.cp->X-Out.pp->X) > (cp->Y-Out.pp->Y) )
               Ncrs = 1 - Ncrs;
         }
      while( Out.next() );
   }
   return (BOOL)Ncrs == 0;                       // new arc is inside generation
}

void C_WEAVE::TryBumps( pCLOS cmp ) {         // -- new arc not in sight of Try.c
   SUPPC Try( cmp, DIRECTION, TimE );
   do {
      if( OrHig(Try.L,SlopeX(Try.cp,cp)) > Try.H ) continue;
      REAL D = Distan( Try.cp, cp );             // distance Try to newC
      REAL R = ( D - Try.cR ) / 2;               // radius of midcircle
      REAL T = TimE + R;                         // time of "possible" event
      if( Try.c->Event != NULL )
         if( Try.c->Event->TimB <= T ) continue; // too late for bump

      PNT2 S; S = *cp + ( *Try.cp - *cp ) * ( R/D );
      if( Prevent(Try.cp,&S,R) ) continue;       // Prevent bump of Try

      if( Try.c->Event != NULL ) {               // too late for disintegration
         Bubb.Dis->ridEvnt( Try.c );
         Try.c->Type = ASPLT;                    // arc folded again
      }

      // -- set latest bump for new arc
      pBUMP v = Get_Bump( Try.c, (pARCS)newC->Bound, T );
      *(pPNT2)(v->Event) = S;
      // (PNT2)*v->Event = S;                       // S is site of future event
      Keep.ordEvnt( v );                         // put into preliminary queue
   } while( Try.next() );
}

BOOL C_WEAVE::Prevent( pPNT2 p, pPNT2 h, REAL R ) {   // Prevent bump of Try
   pCLOS cmp = Bubb.Act;
   while( (cmp=(pCLOS)cmp->Next()) != Bubb.Act ) {
      SUPPC PrvB( cmp, NEIGHBOUR, TimE );             // prevents bump if inside
      do if( PrvB.cp != p )                           // .. midcircle (h,R)
            if( Distan(PrvB.cp,h) - PrvB.cR <= R ) return TRUE;
      while( PrvB.next() );
   }
   return FALSE;
}

// ---------------------------------------------- construct tangent circle to 3 circles
void Appolonius( pARCS c, REAL Tim ) {
   SUPPC A( c, ASYMPTOTE, Tim );          // init arc to test for disintegration
   if( A.DL <= A.DH ) return;             // arc is not disintegrating

   CIRC Fix( A.pp, A.pR-A.cR ),           // fix under inversion (R possibly negative)
        Cen( A.cp, A.cR ),                // centre of inversion (R considered 0)
        ToI( A.np, A.nR-A.cR );           // arc to be inverted (R possibly negative)

   // square of inversion radius, Fix is inverted to itself (Cen.p is inversion midpoint)
   REAL InvQ = SumSqr( Cen.p, Fix.p ) - Fix.R * Fix.R;

   REAL Dist = Distan( Cen.p, ToI.p ),           // distances to midpoint of ToI
        Dfar = InvQ / ( Dist - ToI.R ),          // near point on ToI, far on Inv
        Dcls = InvQ / ( Dist + ToI.R ),          // far point on ToI, near on Inv
        Par = ( Dfar + Dcls ) / 2 / Dist;        // midpoint parameter of Inv

   CIRC Inv;                                     // inversion of ToI
   *Inv.p = *Cen.p + ( *ToI.p - *Cen.p ) * Par;  // midpoint of Inv
   Inv.R = ( Dfar - Dcls ) / 2;                  // radius of Inv (poss. negative)

   // construct tangent line to circles Fix and Inv according to their radii
   // then inversion of that line yields the requested tangent cirle
   REAL O = SlopeX( Fix.p, Inv.p ) - HPI
          + ArcSin( Fix.R-Inv.R, Distan(Fix.p,Inv.p) );
   PNT2 F; F = *Fix.p + UnitVec2( O ) * Fix.R;   // tangent point to Fix
   PNT2 I; I = *Inv.p + UnitVec2( O ) * Inv.R;   // tangent point to Inv
   if( Cen.p->RelDis( &F, &I ) <= 0 ) return;    // bad position, negative time

   F = *Cen.p + ( F - *Cen.p ) * ( InvQ/SumSqr(Cen.p,&F) ); // (re)invert F
   I = *Cen.p + ( I - *Cen.p ) * ( InvQ/SumSqr(Cen.p,&I) ); // (re)invert I

   // "Voronoi" point is midpoint of the circle through points Cen.p, F, I
   if( A.c->Event != NULL ) {                    // use old event
      Bubb.Dis->getEvnt( A.c );
      A.c->Event->TimB = Tim;
   }
   else A.c->Event = Get_Tisp( Tim );            // .. or new disintegration
   CIRC Vor( A.c->Event, 0 );                    // event at midpoint of arc
   Vor.outCir( Cen.p, &F, &I );                  // compute: point, radius

   A.c->Event->TimB += Vor.R - Cen.R;            // add radius to current time
   A.c->Type = AVANE;                            // arc shrinking
   Bubb.Dis->ordEvnt( A.c );                     // link into event queue

   if( Prcs.TOn && Prcs.Flw > 1 )                // test draw of future event
      ShowCircle( Vor.p, 0, TPI, Vor.R-Cen.R, mTEST, 1 );
}

// ------------------------------------------------------------------------------ C_CONTCT
void CCONTCT::ConnectBump( pBUMP v ) {
   // ##### 2018-04-13
   // if( Prcs.Evt ) ShowEvent( CIRCLE, v->Event, ECTCT, _T("Contact") );

   Rid_Bump( v );                             // get rid of Bump and Event
   ConnectArcs();                             // break and cross-connect arcs
   ConnectComp();                             // (dis)connect components cmp1, cmp2
   Distribute( arc1, new1 );                  // distribute bumps of arc1(arc2) between
   Distribute( arc2, new2 );                  // arc1(arc2) and new1(new2)
}
void CCONTCT::ConnectArcs() {                 // break arc1/2 into arc1/2 and new1/2
   new1 = ( arc1 == arc1->Neigh ) ? arc1 : Get_Arcs( cmp1, arc1->Orign );
   new2 = ( arc2 == arc2->Neigh ) ? arc2 : Get_Arcs( cmp2, arc2->Orign );
   new1->Neigh = arc1->Neigh; arc1->Neigh = new2;  // connect arc1 to new2
   new2->Neigh = arc2->Neigh; arc2->Neigh = new1;  // connect arc2 to new1
}
void CCONTCT::ConnectComp() {                      // (dis)connect components
   RevalComp( cmp1, arc1 );                        // revalue component with arc1
   // if cmp were different from cmp2, they are now connected
   if( cmp1 != cmp2 ) Rid_Clos( cmp2 );            // thus, give cmp2 back to store
   // if cmp1 were equal to cmp2, it has split now into two components
   else {                                          // split component into two
      cmp2 = Get_Clos();                           // get second component
      RevalComp( cmp2, arc2 );                     // revalue component with arc2
      Bubb.Act->SequenceIn( cmp2 );                // put cmp2 into working sequence
   }
}

void CCONTCT::Distribute( pARCS aO, pARCS aN ) {
   if( aO == aN ) {                           // arc did not split
      aO->Type = AONCE;                       // there is one arc derived from circle
      return;                                 // .. no distribution
   }
   if( aO->BCnt > 0 ) {                       // split Bumps of aO between aO en aN
      SUPPC SB( aO, DIRECTION, TimE );
      pBUMP b = (pBUMP)Bubb.Cct;
      INT2  B = aO->BCnt;                     // try to speed up bump splitting
      while( B > 0 && (b=(pBUMP)b->Next()) != NULL ) {
         pPNT2 s;
              if( b->Arc1 == aO ) s = b->Arc2->Orign;
         else if( b->Arc2 == aO ) s = b->Arc1->Orign;
         else continue;
         if( OrHig(SB.L,SlopeX(SB.cp,s)) > SB.H ) // bump goes to aN
            b->value( aO, aN );
         --B;                                     // bump has been dealt with
      }
   }
   aO->Type = ASPLT; if( aO->BCnt == 0 ) Appolonius( aO, TimE );  // if no bumps, check
   aN->Type = ASPLT; if( aN->BCnt == 0 ) Appolonius( aN, TimE );  // .. for disintegration
}

// ------------------------------------------------------------------------------ C_DISINT
pREAL ComputeDir( SUPPC *s, pTISP e ) {    // compute directions ! no prototype
   static REAL Dir[3];
//   Dir[0] = SlopeX( s->cp, e ) + Prcs.Flip();
//   Dir[1] = SlopeX( s->n->Orign, e ) + Prcs.Flip();
//   Dir[2] = SlopeX( s->p->Orign, e ) + Prcs.Flip();
   Dir[0] = SlopeX( s->cp, e );
   Dir[1] = SlopeX( s->n->Orign, e );
   Dir[2] = SlopeX( s->p->Orign, e );
   return Dir;
}

void CDISINT::DisintConver( pARCS c ) {    // event handling of a disintegration
   SUPPC Die( c, NEIGHBOUR, TimE );
   INT2  N = VolComp( Die.comp );
   if( N == 3 && ( Die.p->Event != NULL || Die.n->Event != NULL ) ) {
      // ##### 2018-04-13
      // if( Prcs.Evt ) ShowEvent( CIRCLE, e, ECONV, _T("Convergence") );

      if( Die.n->Event != NULL )
         Bubb.Dis->ridEvnt( Die.n );
      else Message( L"Convergence of 3 missing an event" );
      Rid_Arcs( Die.n );

      if( Die.p->Event != NULL )
         Bubb.Dis->ridEvnt( Die.p );
      else Message( L"Convergence of 3 missing an event" );
      Rid_Arcs( Die.p );

      Rid_Arcs( Die.c );
      Rid_Clos( Die.comp );
                        // create a polygon onto emergence queue
      if( Prcs.CrossOver ) MakePoly( N, ComputeDir(&Die,e), e );
      else Rid_Tisp( e );
   }
   else if( N == 3 ) Disintegrate( &Die );  // event of disintegration
/*
   else if( multiple disintegration I <= N ) {
      N - I > 1  : disintegration, multiple of I contours
      N - I == 1 : convergence, incomplete
      N - I == 0 : convergence, complete
   }
*/
   else Disintegrate( &Die );          // event of disintegration
}
void CDISINT::Disintegrate( SUPPC *die ) {     // event of disintegration
   // ##### 2018-04-13
   // if( Prcs.Evt ) ShowEvent( CIRCLE, e, EDISI, _T("Disintegration") );
                       // ## make extra polygon if requested
   if( Prcs.ExtraEmer() ) MakePoly( 3, ComputeDir(die,e), e );
   else Rid_Tisp( e );
   // revalue reference of component and remove c from linkage (Neigh)
   die->comp->Bound = die->p->Neigh = die->n;
   Rid_Arcs( die->c );
   if( die->p->BCnt == 0 ) Appolonius( die->p, TimE );
   if( die->n->BCnt == 0 ) Appolonius( die->n, TimE );
}
// End of File
