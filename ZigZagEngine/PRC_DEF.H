// prc_def.h -- definitions for PROCESS ( of ZIGZAG and BUBBLE ),
//              single classes of support/control

#ifdef _STT2MAIN_CPP
   #define EXTERN
#else
   #define EXTERN extern
#endif                     // _STT2MAIN_CPP

// definition of whitespace characters
#define  TABUL  '\t'
#define  E_STR  '\0'
#define  CR_LF  '\n'
#define  USCOR  '_'                       // underscore to pad file names (rightly)

class MESSAGE {
public:
   INT2 L, R, U, D;
   void far *ima;
   void GetImage( INT2, INT2, INT2, INT2);
   void ShowText( INT2, pCHAR );
   void PutImage();
   void Message( INT2, pCHAR );
};

EXTERN MESSAGE Mess;                      // to control support messages

#include "prc_main.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - to handle circles/arcs
class GENC {                        // -- one generation of cirle components
public:
   CLOS Comp;                          // top for active components
   ETOP Dies,                          // top for disintegration of arcs
        Bump;                          // top for mutual contact of two arcs
   GENC() {                            // initial the tops at construction
      Comp.Link( &Comp ); Comp.Prev = &Comp;
      Dies.Clear();
      Bump.Clear();
   }
};

#define    XGENS     100               // maximum number of generations
class HANDC {                       // -- to support handling generations of components
   // tops of structural elements
   GENC GComp[XGENS];                  // classes for headers of generations
   LINK E;                             // top for emerging components (non-generational)

   INT2 nActG;                         // number of active generations
   INT2 currG;                         // current generation
   void SetTops( INT2 ix ) {           // set top classes for generation number ix
      Act = &GComp[ix].Comp;
      Dis = &GComp[ix].Dies;
      Cct = &GComp[ix].Bump;
   }
public:
   // addresses for current tops of structural elements
   pCLOS Act;                             // all active components
   pETOP Dis, Cct;                        // disintegration, contact
   pLINK Emg;                             // emergence
   void ClearTops();                      // clear all tops

   HANDC() { Emg = &E; }                  // set top for emergences once
   void initGen() {                       // initiate for all generation
      nActG = DrawG = 0;                  // no generation yet
      Drw1G = FALSE;                      // draw not one generation only
   }

   INT2 curGen() { return currG; }        // report current generation
   void setGen( INT2 G ) {                // initiate requested generation
      if( 0 <= G && G < nActG ) SetTops( currG=G );
   }
   void addGen() {                        // add new generation
      if( nActG == XGENS ) Exit( L"Too many circle generations (> %d!)", XGENS );
      SetTops( currG=nActG++ );
   }
                                       // -- functions to loop over generations
   void init() { currG = 0; }             // initial loop index
   void incr() { currG++; }               // increment loop index
   BOOL valid() {                         // validity condition for loop-body
      if( currG < nActG ) SetTops( currG );
      return (BOOL)( currG < nActG );
   }
                                       // -- variables and functions for drawing only
   BOOL Drw1G;                            // draw one generation / all generations
   void toggDG() { Drw1G = !Drw1G; };     // toggle all/one generation(s)
                                       // -- handle single generation
   INT2 DrawG;                            // current drawing index on GComp[]
   void initDG() { SetTops( DrawG ); }    // initiate for one generation
   void incrDG() {                        // increment for one generation
      if( ++DrawG == nActG ) DrawG = 0;
      SetTops( DrawG );
   }
   void decrDG() {                        // decrement for one generation
      if( --DrawG < 0 ) DrawG = nActG - 1;
      SetTops( DrawG );
   }
};
#undef XGENS

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - to handle triangles
class HANDT {                         // -- to support handling polygons
   CLOS A;                               // top classes for active/existing polygons,
   ETOP D, C;                            // .. disintegration, contact
   LINK E;                               // .. emergence
public:                               // -- pointers to above classes
   pCLOS Act;                            // active polygons
   pETOP Dis, Cct;                       // disintegration, contact
   pLINK Emg;                            // emergence
                                         // set top classes at construction
   HANDT() { Act = &A; Dis = &D; Cct = &C; Emg = &E; }

   void ClearTops() {                 // -- initial all tops
      Act->Link( Act ); Act->Prev = Act; // link to itself for all polygons
      Dis->Clear();                      // clear for disintegrating contours
      Cct->Clear();                      // clear for peeking bisectors
      Emg->Clear();                      // clear for converging components
   }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - to handle the process
// -- which space is in use (computation or on screen): circles, triangles or both
enum BZTYP { CIRCLE, TRIANG, CTBOTH };

class GRPH {                     // -- to control graphics
   pCHAR dirBGI;                    // directory for the BGI file
public:
   INT2 Driver, Mode;               // driver, mode of graphic interface
   BOOL IsNT;                       // program runs under Windows-NT together with VGAMED
   BOOL IsOpen;                     // BGI initialized or not
   void (*ScrDraw)( REAL );         // draw according to Driver/Mode

   GRPH() { IsOpen = FALSE; }       // initialize graphics not opened
   void CommLine( INT2, pCHAR [] ); // scan command line for graphics
   void setBGI( pCHAR );            // to initialize dirBGI
   void setDrv( pCHAR );            // to support CL-initialization
   void Open();                     // open graphics mode
   void Close();                    // close graphics mode
};

class TEST {                     // -- diverse test facilities
public:
   BOOL TOn, Evt;                   // test on/off, show events
   INT2 Flw, Sec;                   // flow option, second to wait after message,
   TEST() {                         // initial testing
      TOn = Cnt = Evt = FALSE;      // test option on/off, class count
      Flw = XO = 0;                 // test flow option on/off
      Sec = 1;                      // number of seconds to show test messages
   }
   BOOL Parse( INT2 );              // parse keybord
                                 // -- count of production classes
   BOOL Cnt;                        // class count active (or not)
   INT2 XO;                         // largest number of reference to a TISP over all events
   void Count();                    // execute count of all production classes
   void EvTable();                  // print counts of production classes (to screen)
};

class PROCS: public GRPH, public TEST { // -- to control the process
   void Init();                            // initialize one process
   BOOL Parse( INT2 );                     // parse for process
   BOOL preParse();                        // handle keyboard hits before dynamics
   BOOL EmerParse( INT2 );                 // parse for extra emergency
   BOOL _Parse( INT2 );                    // parse for diverse actions
   BOOL Parsing();                         // parse as long as kbhit()
   BOOL Processing();                      // processing one process
public:
   BOOL CrossOver;                         // from circles to triangles and vice versa
   REAL ExtraEm_V;                         // probability treshold for extra emergency
   BOOL ExtraEmer();                       // emergence at "loss of structure"
   REAL T, S, X, D;                        // time values: current, step, at a stop, to draw
   PROCS(): GRPH(), TEST() {
      ExtraEm_V = 0;
      CrossOver = TRUE;
   }
   void Process();                         // do all the processing
                                        // -- the twofold construction of triangles
   INT2 F;                                 // to reflect (1) or continue (0)
   REAL Flip() { return (REAL)F * _PI; }   // additional direction according to F
};

EXTERN HANDC Bubb;         // to control generation handling of arcs
EXTERN HANDT ZgZg;         // to control non-generation handling of lines
EXTERN PROCS Prcs;         // to handle time of process and graphics

#undef EXTERN
