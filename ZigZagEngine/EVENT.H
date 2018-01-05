// event.h -- is included into prcmain.cpp only !
#ifndef _EVENT_H
#define _EVENT_H

// ##### 2018-01-05
enum EVTYP {                           // type of disintegration, bump or emergence
   CirDIS, CirCTC, CirEMG,             // for circlres
   TriDIS0, TriCTC0, TriEMG0,          // for triangles continuing
   TriDIS1, TriCTC1, TriEMG1,          // for triangles reflecting
   noEVNT = -1                         // or no event is to occur
};

class EVENT: public GTIM {             // -- definition of an event to take place
   EVTYP EvTyp;                           // type of event
   INT2  Gener;                           // generation (circles only)
   void askEvent( pLINK top, EVTYP ET, INT2 G = UNDEF ) {
      pEVNT a = (pEVNT)top->Next();       // look at first on queue
      if( a != NULL )
         if( valT(a->Event->TimB) ) iniEvent( a->Event, ET, G );
   }
   void askEmerg( pLINK top, EVTYP ET ) {
      pCLOS c = (pCLOS)top->Next();       // emergence of full circle or triangle
      if( c != NULL )
         if( valT(c->Bound->Orign->TimB) ) iniEvent( c->Bound->Orign, ET );
   }
public:
   void iniEvent( pTISP ts = NULL, EVTYP E = noEVNT, INT2 G = UNDEF ) {
      EvTyp = E;                          // type of event
      Gener = G;                          // generation (if CIRCLE)
      if( ts == NULL ) iniT();            // initialize time keeping
      else             updT( ts->TimB );  // update of time keeping
   }
   void ExcEvent();                       // process next event
   void SetEvent();                       // look for new Event
};

// -- PROTOTYPES: for events to be executed
                               // -- from cir_str.cpp
extern void DoCirEmerge();        // emergence of a circle
extern void DoCirContct();        // contact of two arcs/circles
extern void DoCirDisint();        // disintegration of an arc
                               // -- from tri_str.cpp
extern void DoTriEmerge();        // emerging of a triangle
extern void DoTriContct();        // contact of a bisector with a contour
extern void DoTriDisint();        // disintegration of a contour

// ##### 2018-01-05
void EVENT::ExcEvent() {          // execute event and look for next one
   if( Act ) {                    // do event
      switch( EvTyp ) {
                                              // handle circles
         case CirDIS: Bubb.setGen( Gener );   // set generation
                      DoCirDisint(); break;   // handle desintegration
         case CirCTC: Bubb.setGen( Gener );   // set generation 
                      DoCirContct(); break;   // handle peek
         case CirEMG: Bubb.setGen( Gener );   // set generation
                      DoCirEmerge(); break;   // handle emergence

                                               // handle triangles continuing
         case TriDIS0: DoTriDisint(); break;      // handle desintegration
         case TriCTC0: DoTriContct(); break;      // handle contact/peek
         case TriEMG0: DoTriEmerge(); break;      // handle emergence
                                           
                                               // handle triangles reflecting
//         case TriDIS1: DoTriDisint(); break;      // handle desintegration
//         case TriCTC1: DoTriContct(); break;      // handle contact/peek
//         case TriEMG1: DoTriEmerge(); break;      // handle emergence
      }
   }
   SetEvent();                     // next event from any queue
}

void EVENT::SetEvent() {
   iniEvent();                                      // initially no event

   askEmerg( Bubb.Emg, CirEMG );                    // emergence of full circle
   for( Bubb.init(); Bubb.valid(); Bubb.incr() ) {  // arc events over generations
      askEvent( Bubb.Dis, CirDIS, Bubb.curGen() );  // .. disintegrations
      askEvent( Bubb.Cct, CirCTC, Bubb.curGen() );  // .. contacts
   }

   askEmerg( ZgZg1.Emg, TriEMG0 );                    // emergence of a triangle
   askEvent( ZgZg1.Dis, TriDIS0 );                    // disintegration of a contour
   askEvent( ZgZg1.Cct, TriCTC0 );                    // contact of a bisector
}

EVENT Event;                       // current event
#endif                             // _EVENT_H
