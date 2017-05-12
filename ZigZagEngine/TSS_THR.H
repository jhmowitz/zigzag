// tss_thr.h -- handle regular tesselations for a throw-in start
//              tryout 20/12/95

#ifndef _TSS_THR_H
#define _TSS_THR_H

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - TSSL
                                // -- control flags to use by calling program
enum TTYP {                        // types of "regular" tesselations
   NONE_T, TRIA_T, SQRE_T, HEXA_T,   
};
enum FTYP {                        // types of frames for tesselation
   CIRC_F, TRIA_F, SQRE_F, RECT_F, HEXA_F,   
};

class TSSL {               // -- define a "regular" tesselation
public:
   TTYP Tssl;                 // current tesselation type

   INT2 Dens;                 // density within frame
   INT2 NPrt;                 // n of PERTURB1 (stands for n%)

   uINT2 rBeg;                // begin of random number generation

   REAL RotT;                 // rotation of tesselation
   INT2 nT;                   // number of active tesselation points
   INT2 nO;                   // number of points to be omitted
   INT2 nTO;                  // number of tesselation points, equals nT + nO

   void Init() {
      Tssl = TRIA_T;          // current tesselation type is triangular
      Dens = 2;               // some minimum density
      NPrt = 5;               // to begin with 5% perturbation
      RotT = 0;               // no rotation yet
      rBeg = 0u;              // begin of random number generation
      nO = 0;                 // no omittance of points
   }
   REAL UnitDist( REAL R ) {    // unit (smallest) distance between points
      return R / Dens;
   } 
   BOOL Parse( INT2 );

   void setOmittance();         // set indeces of omitted points
   BOOL tstOmittance();         // test point index for omittance 
};

class TSSTHR {                // -- to handle "regular" tesselations
public:
   BOOL On;                     // handler is on
   TSSTHR() { On = Was = FALSE; }
   BOOL Parse( INT2 );
   void ShowUp();               // draw all
private:
   BOOL Was, Subm;              // handler was on before, submit structures
   FTYP Frme;                   // current frame type
   BOOL DubF;                   // frame is divided into inner/outer part
   REAL Wide;                   // width of ring around subframe
   INT2 nF;                     // N of frame points
   PNT2 Mp, Fp[6];              // frame midpoint, frame points
   REAL RotF;                   // frame rotation
   REAL FFac;                   // proportion for frame extension
   REAL Radi;                   // half diameter of frame
   REAL RFac;                   // factor for diameter of inner circle
   REAL RctA;                   // angle to determine format of rectangle
   BOOL Circ;                   // throw circles or other ## ?? enum STYP

   INT2 IxT;                    // index to T[]
   TSSL T[2];                   // outside/inside tesselations
                                // .. and a dummy to keep counts
   TSSL *t, *l;                 // current/loop tesselation
   INT2 nTess;                  // n of tesselations in use
   INT2 nS;                     // N of points to be thrown in one strip
   MYCOLOR TessCol;               // to draw points of tesselation

   void Init();
   void ScreenInit();                 // (re)set Mp, Radi
   void SetFrame();                   // set all frames
   void FramePts( INT2, REAL, REAL ); // set nF, Fp[]
   
   void SetTessela();                 // handle "regular" tesselations
   void TesselTriSqr( REAL, MYCOLOR );  // generate triangular/square tesselation
   void TesselHexa();                 // generate hexagonal tesselation

   void ApplyStrip( pPNT2, pVEC2, INT2 );
   void ApplyStrp2( pPNT2, pPNT2, pVEC2, pVEC2 );

   void Apply_( pPNT2 );              // branch for several throw objects
   BOOL InsideFrame( pPNT2 );         // check inside for either frame
   void Perturb( pPNT2 );             // perturbation of one point

   void ApplyP( pPNT2 );              // handle circle/point draw/throw
   void DrawP( pPNT2, MYCOLOR );      // draw point

   void ApplyV( pPNT2 );                // handle the objects "double V"
   void DrawV( pPNT2, REAL, MYCOLOR );  // draw "double V"

   void DrawFrame();                  // draw frame
   void DrawC( pPNT2, REAL, INT2 );   // draw full circle
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - RDMTHR
#define  XARCS    10     // maximum number of arcs (sides in polygon)

class RDMTHR {        // -- definition of randomly emerging circles/triangles
   BZTYP What;           // != TRIANG (circles) or != CIRCLE (triangles)
   pPNT2 midP;           // keeps domain midpoint from viewgrph.cpp
   REAL  Xfac, Yfac;     // extension factors around midpoint for throwing
   REAL  Marg;           // margin for random throwing
   BOOL  Acut;           // throw smooth or folded triangles
   REAL  Arcs[XARCS];    // "raw" directions for contours in a polygon to throw
   INT2  nA;             // number of contours in a polygon

   uINT2 rBeg;                  // start value for random number generator
   BOOL  RandPars( INT2 );      // parameters to set random numbers

   void  ThrowIt( INT2 );       // create ..
   pTISP Point( REAL );         // generate a random point
   void  Angles( INT2, pREAL ); // generate a number of random directions
public:
   RDMTHR() {
      What = CTBOTH;
      Acut = TRUE;
      Marg = 0.2;
      srand ( rBeg=0u );
      nA = 3;
   }
   BOOL Parse( INT2 );      // parameters to control this class
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - FILTHR
#define  XTAG   100

class MYFILE {
   void tab_number();
   void fixTGClip();
protected:
	// MT: made filpath protected
   CHAR filpath[150];                // to carry path, name and extension of file
public:
   FILE *f;
   pCHAR n;

   MYFILE() { n = filpath; }

   BOOL Open( pCHAR );

   CHAR  TG[XTAG+1];
   INT2  nTab;
   pCHAR b, e;

   BOOL  iniTGClip();
   void  updTGClip() { b = e + 1; }
   pCHAR getTGClip();
};

class FILTHR: public MYFILE {
public:
   INT2 nC, nT;

   FILTHR(): MYFILE() {}

   BOOL Parse( INT2 );                // parse for throw of file configuration
   void put_closures();
};

#endif                 // _TSS_THR_H
