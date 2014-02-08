// cir_str.h -- definition of all classes supporting process of circles(arcs)

/* -- SUPPC: a class to support looping around one component of arcs
 *           with precalculated values dependent of a control parameter NIVO
 *
 *     usage:
 *     SUPPC S( pCLOS/pARCS, NIVO, time value );
 *     do {
 *        statements with:  S.p, S.c, S.n, ... (or others depending of NIVO)
 *     } while( S.next() );
 */

// computation control for remote values for several levels of usage
enum NIVO {                   // -- possible values
   NEIGHBOUR,                    // neighbors only
   DIRECTION,                    // directions of limits of arc
   ASYMPTOTE,                    // asymptotes of intersectors
};

// -- to support handling of one closure (component of arcs)
class SUPPC {
public:                       // -- input values
   pCLOS comp;                   // current closure
   pARCS p, c, n, f;             // arcs: previous, current, next and first==last in loop
   REAL  L, H;                   // low(right) and high(left) directions for current arc
   REAL  DL, DH;                 // directions of asymtotes with previous and next arc

                              // -- remote values for reuse
   pPNT2 pp, cp, np;             // origine sites of arcs p, c and n
   REAL  pR, cR, nR, Lcp, Lcn;   // radii of arcs and distances between origines

                                     // -- to support do-while-loop:
   SUPPC( pARCS cc, NIVO N, REAL T ) {  // initial loop from arc
      comp = cc->Ident;                 // current component
      c = f = cc;                       // set current arc and first==last in loop
      setNivo( N, T );                  // set further values
   }
   SUPPC( pCLOS cp, NIVO N, REAL T ) {  // initial loop from closure
      comp = cp;                        // current component
      c = f = (pARCS)cp->Bound;         // set current arc and first==last in loop
      setNivo( N, T );                  // set further values
   }
   BOOL next() {                        // increment loop, next arc
      if( n == f ) return FALSE;        // close loop (finished)
      p = c; c = n; n = c->Neigh;       // set references to next arc in Neigh-link
      NivoVal();                        // set derived values for higher levels
      return TRUE;
   }
private:
   NIVO Nivo;                           // nivo of support
   REAL Time;                           // time of computations
   void setNivo( NIVO N, REAL T ) {  // -- serves both constructors
      Nivo = N;                         // nivo of computational support
      Time = T;                         // time of computations
      p = c->previous();                // .. previous arc and
      n = c->Neigh;                     // .. next arc (Neigh-reference)
      NivoVal();                        // set remote values for higher nivos
   }
   void NivoVal();                      // remote values for reuse (to save computing)
};

#ifdef _CIR_STR_CPP                 // ! to include below definitions

// -- compute remote values to support computations concerning ARCS
void SUPPC::NivoVal() {                       // serves setNivo() and next()
                                           // -- Nivo equals NEIGHBOUR
   cp = c->Orign;                             // midpunt of arc c
   cR = Time - c->Orign->TimB;                // radius of arc c
   if( Nivo == NEIGHBOUR ) return;
                                           // -- Nivo equals DIRECTION
   if( c != n ) {                             // midpts., radii of arcs p and n
      pp = p->Orign; pR = Time - p->Orign->TimB; Lcp = Distan( cp, pp );
      np = n->Orign; nR = Time - n->Orign->TimB; Lcn = Distan( cp, np );
   }
   DL = ( c == n )  ? 0        : SlopeX( cp, pp );           // prepatory use for
   DH = ( c == n || np == pp )                               // .. later construction
                    ? DL + TPI : OrHig( DL, SlopeX(cp,np) );
   L  = ( c == n )  ? DL       : DL + ArcCos( cR, Lcp, pR ); // low, high
   H  = ( c == n )  ? DH       : DH - ArcCos( cR, Lcn, nR ); // direction of c
   if( Nivo == DIRECTION ) return;
                                           // -- Nivo equals ASYMPTOTE
   if( c != n ) {
      DL += HPI - ArcSin( cR-pR, Lcp );       // low and high direction
      DH -= HPI - ArcSin( cR-nR, Lcn );       // .. of asymptotes
   }
}

class TIMVAL {                             // -- keep a time value
public:
   REAL  TimE;                                // time of event
   TIMVAL( REAL T ) { TimE = T; }
};

class C_WEAVE: public TIMVAL {             // -- to weave in an emerging circle
   pCLOS newC;                                // new closure (circle)
   pPNT2 cp;                                  // point of origin of new closure
   ETOP  Keep;                                // keep all valid bumps of arc of new closure

   void WeaveIn();                            // weave a new closure into the process
   BOOL Outside();                            // determine generation of new closure
   void TryBumps( pCLOS );                    // compute direct encounters of new closure
   BOOL Prevent( pPNT2, pPNT2, REAL );        // prevention of tryout bumps
public:
   C_WEAVE( pCLOS cmp ): TIMVAL( cmp->Bound->Orign->TimB ) {
      newC = cmp;                             // take over closure
      cp = cmp->Bound->Orign;                 // and site of event
      WeaveIn();                              // weave new circle into BUBBLE
   }
};

class CCONTCT: public TIMVAL {             // -- to handle a contact of two arcs
   pCLOS cmp1, cmp2;                          // involved closures
   pARCS arc1, arc2, new1, new2;              // arcs to bump(break), new arcs

   void ConnectBump( pBUMP );                 // do one bump of two arcs
   void ConnectArcs();                        // interconnect arcs
   void ConnectComp();                        // (dis)connect closures
   void Distribute( pARCS, pARCS );           // distribute bumps after split
public:
   CCONTCT( pBUMP bmp ): TIMVAL( bmp->Event->TimB ) {
      arc1 = bmp->Arc1; cmp1 = arc1->Ident;   // set arc and closures
      arc2 = bmp->Arc2; cmp2 = arc2->Ident;   // .. for both participants
      ConnectBump( bmp );                     // do the event: bump of two arcs
   }
};

class CDISINT: public TIMVAL {             // -- to handle a disintegration or convergence
   pTISP e;                                   // point of event
   void  DisintConver( pARCS );               // do disintegration or convergence
   void  Disintegrate( SUPPC * );             // event of disintegration
public:
   CDISINT( pARCS c ): TIMVAL( c->Event->TimB ) {
      e = c->Event;
      DisintConver( c );
   }
};
#endif                                      // _CIR_STR_CPP

