// make_psf.h -- To support an interface between ZIGZAG and modules
//               to write a PostScipt file for printer drawings.
//               To be included by viewgrph.cpp

#ifndef _MAKE_PSF_H
#define _MAKE_PSF_H

// MT: removed. Console input has been replaced with host methods like ReadString()
// #include <iostream.h>              // cin >> for standard input

#if WIN32
#ifndef CHAR
#include "..\..\MOW_DEFS.H"            // incriptions for includes (use long double)
#endif
#endif

// fixed step - flags to control series of pictures into one page
#define  FS_UNUSED   0             // no FS-pictures, initial state of program
#define  FS_INITIAL  1             // initial FS-pictures by user
#define  FS_STEPUP   2             // switched to by program after first picture
#define  FS_FINISH   3             // request for end of picture sequence

#define  NAMPOS       16           // total length of file/path name
#define  IPOS_1  (INT2)6           // number of positions in name of one(!) file
#define  IPOS_M  (INT2)4           // number of positions in name of many(!) files

class FS_FIL {                     // pictures of fixed step on 1 file or a series of files
   CHAR userid[IPOS_M + 1];        // to read identifyer: ## hoeveel is nodig ?
public:
   CHAR uds_nm[NAMPOS+1];          // to contain "eps\\ident_nr.eps\0"

   REAL FS_LastTime;               // time of drawing: next one or latest one
   REAL FS_TimeStep;               // fixed time step

   BOOL fid_name( pCHAR, INT2 ); // get file identifyer
   void set_Time();                // set begin time and time step
};

BOOL FS_FIL::fid_name( pCHAR fil_id, INT2 nId ) {        // get file identifyer
   // MT: Refactored
	 // Mess_LT( _T("give 1 to %d characters of file name"), nId );
	 CHAR prompt[50];
	 SPRINTF( prompt, _T("give 1 to %d characters of file name"), nId );
   GETS( prompt, userid, sizeof( userid ) - 1 );
   if( STRLEN(userid) > 0 ) {
      INT2 i = 0;
      while( i < nId && *(userid+i) != E_STR ) *(fil_id+i)   = *(userid+i++);
      while( i < nId && *(fil_id+i) != E_STR ) *(fil_id+i++) = USCOR;
      return TRUE;
   }
   return FALSE;
}

void FS_FIL::set_Time() {          // set begin time and time step
   FS_LastTime = Prcs.T;           // set last(first) time for printing
   REAL nStp = 0;                  // get fixed step measured in screen steps
   // MT: replaced Mess_LT + cin with ReadReal
   // Mess_LT( _T("give step factor ( > 0 )") );
   while (nStp <= 0.1) {// cin >> nStp;
     nStp = ReadReal(_T("give step factor ( > 0 )"));
     if (nStp == -9999.0)
       return;
   }
   FS_TimeStep = nStp * Prcs.S;
}

// -- printing a series of contours with a fixed timestep on 1 file
class FS1FIL: virtual public FS_FIL {
public:
   INT2  FS_1;                        // state of fixstep process
   INT2  nFil;                        // number of files with same starting identifyer
   CHAR  def_id[7];                   // file identifyer

   FS1FIL() {
      STRCPY(def_id, _T("zigzag"));              // IPOS_1 (6) positions for identifyer
      nFil = 0;
   }

   void  Sequence_OnOff( pCHAR & );   // printing sequence on or off
   pCHAR get_name();
};

pCHAR FS1FIL::get_name() {
   if( fid_name( def_id, IPOS_1) ) nFil = 0;               // get file identifyer
   SPRINTF( uds_nm, _T("eps\\%s%2.2d.eps"), def_id, nFil );  // print ident and sequence number
   nFil++;
   Mess_LT( _T("Print on one file -- file name is >%s<"), uds_nm );
   return uds_nm;
}

class FSMFIL: virtual public FS_FIL {
public:
   INT2  FS_M;                        // state of fixstep process
   INT2  nFil;                        // number of files with same starting identifyer
   CHAR def_id[IPOS_M + 1];                      // file identifyer

   FSMFIL() {
      STRCPY(def_id, _T("zgzg"));                // IPOS_M (4) positions for identifyer
      nFil = 0;
   }

   void  Sequence_OnOff();            // printing sequence on or off
   pCHAR nxt_name();
};

pCHAR FSMFIL::nxt_name() {
   SPRINTF( uds_nm, _T("eps\\%s%4.4d.eps"), def_id, nFil );  // print ident and sequence number
   nFil++;
   Mess_LT( _T("Print on a file series -- file name is >%s<"), uds_nm );
   return uds_nm;
}

#define  PATHLEN  4             // actual handled length of path - begin of filename
#define  CIRCPOS  (PATHLEN+6)   // position of character for circle indication
#define  TRIAPOS  (PATHLEN+7)   // position of character for triangle indication

// -- control file names for series of PostScript files
class FNAMES {
   BOOL Used;                   // indicate usage of file series names
   INT2 iFil;                   // index of file in series
public:
   pCHAR gen_name;              // to keep a basis for a file name series

   FNAMES() {                   // initial at begin of program
      Used = FALSE;
      iFil = 0;
      gen_name = _T("eps\\A1234567.eps");
               //      ||    |T<------ indicate triangles on pos 7 (underscore otherwise)
               //      ||    C<------- indicate circles on pos 6 (underscore otherwise)
               //      |.....<-------- index number iFil of print in series (5 positions)
               //      A<------------- series indicator: from A to Z (maximally 26 series)
   }

   void ReInit() {                    // -- (re)initial at start of simulation
      if( !Used ) return;
      CHAR L = gen_name[PATHLEN];        // take next letter for nieuw series
      if( L != USCOR )
         if( L == 'Z' ) L = USCOR;       // too many series: take underscore
         else           L++;             // next letter for next series
      Used = FALSE;
      iFil = 0;
   }
   /* voorlopig -- voor naam van een print
   pCHAR SetName( BZTYP Which ) {                  // -- set name at print request
      Used = TRUE;
      SPRINTF( &gen_name[PATHLEN+1], _T("%5.5d"), iFil ); // print sequence number
      if( iFil < 30000 ) iFil++;
      gen_name[CIRCPOS] = ( Which == CIRCLE || Which == CTBOTH ) ? 'C' : USCOR;
      gen_name[TRIAPOS] = ( Which == TRIANG || Which == CTBOTH ) ? 'T' : USCOR;
      return gen_name;
   }
   */
};   // ##### 2018-04-21

// -- control writing of a PostScript file - interface to PSF
class MAKEPS: public FNAMES, public FS1FIL, public FSMFIL {
	// TODO MT: LPT can be removed
   INT2 LPT;                      // number of lineprinter (lpt) to use (1, 2 or 3)
   pCHAR cur_name;                // current name
   CHAR fix_name[20];                // fixed name of file sent to the printer
   pCHAR var_name();              // variable name(s) of file sent to the printer

   INT2 nWid, nHgt,               // nWid*nHgt pictures on a page
        iPic, jPic;               // current picture, denoted picture

   REAL PortFac;                  // frame x/y-proportion (portrait to landscape)
   REAL Ratio();                  // ratio of ranges for PS-file
   REAL XhRng();                  // domain x-range for PS-file
   REAL MidX();                   // mid of x-domain for PS-file
   REAL MidY();                   // mid of x-domain for PS-file

public:
   BOOL DoDraw;                   // draw (one picture of) a PS file
   BOOL Divide;                   // divide into several pictures
   BOOL Finish;                   // close divide sequence

                               // -- to handle a drawing of several pages
   BOOL MultiP;                  // draw multiple pages
   CHAR swt_name[20];                // name for a serie of files
   REAL r_magnify;               // magnification factor on both coordinates
   INT2 pIdx;                    // page index
   INT2 pCol, pRow;              // number of pages in colum (x) and row (y) 

   // to draw series with fixed step
   BOOL FS1_TestNext( REAL );     // test for next drawing on one file
   BOOL FS1_Prolog();             // do not MakePSProlog for series
   BOOL FS1_Epilog();             // do not MakePSEpilog for series

   BOOL FSM_TestNext( REAL );     // test for next drawing on seperate files
   void FSM_Prolog();             // set next series filename
   void FSM_Epilog();             // ??

   MAKEPS(): FNAMES()             // defaults to begin program run
           , FS1FIL()
           , FSMFIL() {
      LPT = 0;                          // no files are sent to the printer
      DoDraw = Divide = Finish = FALSE;
      PortFac = 1.0;                    // default format (on screen) is landscape
      MultiP = FALSE;                   // print on single page
			STRCPY(fix_name,  _T("eps\\zigzag.eps"));    // fixed name for single page print
			STRCPY(swt_name, _T("eps\\12345678.eps\0")); // include 0-byte for SPRINTF
   }
   void FileIdent(pCONSTCHAR);   // write identification string to file name swt_name

   BOOL HandleKeys( INT2 );      // (ks) handle key strokes
   BOOL HandleMultPage();        // (ks) handle pages of a picture of multiple pages
   void PageDivide();            // (ks) divide page into pictures
   BOOL PageQuit();              // (ks) quit image without sending file
   void PictDirect( BOOL );      // (ks) denote specific image to be next

   // ##### 2018-04-21 
   //void DisplayProlog( BZTYP );  // do before graphical display
   void DisplayProlog( ScrDrawCirc, ScrDrawTria );  // do before graphical display
   void ShowPSOpt( int, int );   // show options on screen
   void DisplayEpilog();         // do after graphical display
   void SetTitle( pCHAR );       // set title of one picture

                                           // -- for drawing
   void Point( pPNT2, MskP );                 // a point
   void Line( pPNT2, pPNT2, MskL );           // draw line with own MskL
   void Line( pPNT2, pPNT2 );                 // a line (MskL is thinnest)
   void Circ( pPNT2, REAL, MskL );            // a circle
   void Arc( pPNT2, REAL, REAL, REAL, MskL ); // an arc

                       // draw component ! using special modules
   void Comp( pCLOS, MskL );
   void CompoOpen( pCLOS, MskL );

                       // ! polygon is drawn as a curve, with different opening
   void Poly( pCLOS, MskL );                  // draw PostScript polygon
   void PolygOpen( pCLOS, pPNT2, MskL );      // a polygon and its address

                       // ! drawing a curve must be uninterrupted by other draws
   void CurveOpen( pPNT2, MskL );             // a curve with specified thickness
   void CurveCont( pPNT2 );                   // continue curve with given mask
   void CurveEnd();                           // end curve (stroke)
};

// -- actually the .cpp -code starts here !

#ifdef  _VIEWGRPH_CPP           // to include sources for PostScript printing

// MT: removed. cin no longer used
// #include <iostream>           // cin
#include "key_def.h"

#include  PS_FILE_CPP           // definition and code of PostScript image representation

enum PICTH {
   NormTh, ThinTh, ShowTh, ThckTh,   // global thicknesses for one image
};

// -- support MakePS with PS-specific variables ! local scope on inclusion file only
class HELPPS {
public:
                           // -- per page
   REAL   UsePage;            // proportion of usage of full page 
   DRAWMK MarkCor;            // to mark left or right hand corner
   DRAWDT DrwDate;            // to draw current date right below
   MARGIN Margins;            // vary margins between pictures
   TH_IDX PagTh;              // thickness per page dependend of division

                           // -- per picture
   DRAWFR DrwFrme;            // draw frame or not
   DRAWTT DrwTitl;            // draw title or not
   CUTLIN DrwCutL;            // draw cut lines for multiple pages
   REAL   BckGray;            // set background grayness per picture
   PICTH  PicTh;              // thickness per picture

   HELPPS() {
      UsePage = 0.95;         // use 95% of x-length and y-length of the page
      MarkCor = MARK_NO;      // no corner mark
      DrwDate = DATE_NO;      // no date to draw
      Margins = MG_NARROW;    // vary margins between pictures
      PagTh   = TK_GIGANTIC;  // preset for print of multiple pages

      DrwFrme = FRAME_YES;    // draw frame of picture
      DrwTitl = TITLE_NO;     // draw no title
      DrwCutL = NOLINES;      // draw cut lines for multiple pages
      BckGray = PSF_WHITE;    // background is white (1.0)
      PicTh = NormTh;
   }

   void SetPageTh( INT2 );    // set overall thickness for the page
   void LWidMsk( MskL );      // use selected line width
   void PWidMsk( MskP );      // use selected point width/mask
   PW_IDX getWidP( MskP );    // get point width
};

static PSFILE PSF;            // interface to the PostScript modules
static HELPPS HPS;            // local class to support PostScript representation

MAKEPS MakePS;                // control class to make PostScript image

#else
extern MAKEPS MakePS;
#endif                        // _VIEWGRPH_CPP

//--------------------------------------------------------------------------------------

#ifdef  _VIEWGRPH_CPP            // to include sources for PostScript printing

// - - - - - - - - - - - - - - - - - prototypes to hide class DOM2 Dom2 from flatgrph.cpp
extern pPNT2 CadreMidPnt();      // address of midpoint of screen
extern REAL  HalfRangeX();       // half x-range of screen domain
extern REAL  DatRatioYX();       // data y/x-ratio of screen domain

// - - - - - - - - - - - - - - - - - - - - - to hide class MAKEPS MakePS for external use
                                          // handle keys for PS-print
BOOL MakePSHandleKeys( INT2 c ) { return MakePS.HandleKeys( c ); }
                                          // test for next drawing in Fixstep series
BOOL MakePSTestNext_FS1( REAL T ) { return MakePS.FS1_TestNext( T ); }
BOOL MakePSTestNext_FSM( REAL T ) { return MakePS.FSM_TestNext( T ); }
                                          // handle print over several pages
BOOL MakePSHandleMultPages() { return MakePS.HandleMultPage(); }
void MakePSReInit() { MakePS.ReInit(); }  // reinitial file series names after usage

                                          // initial PostScript print
// ##### 2018-04-20 void MakePSDisplayProlog(BZTYP W) { MakePS.DisplayProlog(W); }
void MakePSDisplayProlog( ScrDrawCirc CirW, ScrDrawTria TriW ) { MakePS.DisplayProlog( CirW, TriW ); }
                                          // finish PostScript print
void MakePSDisplayEpilog() { MakePS.DisplayEpilog(); }
                                          // show options on screen
void MakePSShowOptions( int X, int Y ) { MakePS.ShowPSOpt( X, Y ); }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - HELPPS
void HELPPS::SetPageTh( INT2 n ) {         // set thickness of page
        if( n > 10 ) PagTh = TK_TINY;      // according to number of pictures
   else if( n > 6  ) PagTh = TK_SMALL;
   else if( n > 3  ) PagTh = TK_MEDIUM;
   else if( n > 1  ) PagTh = TK_LARGE;
   else              PagTh = TK_HUGE;
}
void HELPPS::LWidMsk( MskL m ) {           // set line width for PSF
        if( PicTh == ThinTh ) PSF.LinWidMsk( LW_NULL, B_FULL );  // standard thin
   else if( PicTh == ThckTh ) PSF.LinWidMsk( LW_LARGE, B_FULL ); // standard thick
   else {                                                        // showy or normal
      LW_IDX W;
      switch( m ) {
         case L_NULL: W = LW_NULL; break;
         case L_TINY: W = ( PicTh == ShowTh ) ? LW_SMALL  : LW_TINY; break;
         case L_SMLL: W = ( PicTh == ShowTh ) ? LW_MEDIUM : LW_SMALL; break;
         case L_MEDI: W = ( PicTh == ShowTh ) ? LW_LARGE  : LW_MEDIUM; break;
         case L_LRGE: W = ( PicTh == ShowTh ) ? LW_HUGE   : LW_LARGE; break;
         default:     W = ( PicTh == ShowTh ) ? LW_TINY   : LW_NULL;
      }
      PSF.LinWidMsk( W, B_FULL );
   }
}
void HELPPS::PWidMsk( MskP m ) {           // set point width/mask for PSF
        if( PicTh == ThinTh ) PSF.PntWidMsk( PW_SMALL, B_FULL ); // standard thin
   else if( PicTh == ThckTh ) PSF.PntWidMsk( PW_LARGE, B_WW_B ); // standard thick
   else {                                                        // showy or normal
      PW_IDX W = getWidP( m );
      switch( m ) {
         case S_BLCK:
         case M_BLCK:
         case L_BLCK: PSF.PntWidMsk( W, B_FULL ); break;
         case S_WHTE:
         case M_WHTE: PSF.PntWidMsk( W, W_FULL ); break;
         case S_BinW: PSF.PntWidMsk( W, W_BB_W ); break;
         case S_WinB: PSF.PntWidMsk( W, B_WW_B ); break;
         case T_CRSS:
         case S_CRSS:
         case M_CRSS: PSF.PntWidMsk( W, B_CRSS ); break;
         case S_CIRC: PSF.PntWidMsk( W, B_CIRC ); break;
         default:     PSF.PntWidMsk( W, B_FULL );
      }
   }
}
PW_IDX HELPPS::getWidP( MskP m ) {
   switch( m ) {
      case T_BLCK: return                       PW_TINY;     // ## ! in zigzag only
      case T_CRSS: return ( PicTh == ShowTh ) ? PW_SMALL : PW_TINY;
      case S_BLCK:
      case S_WHTE:
      case S_BinW:
      case S_WinB:
      case S_CRSS:
      case S_CIRC: return ( PicTh == ShowTh ) ? PW_MEDIUM : PW_SMALL;
      case M_BLCK:
      case M_WHTE:
      case M_CRSS: return ( PicTh == ShowTh ) ? PW_LARGE : PW_MEDIUM;
      case L_BLCK: return ( PicTh == ShowTh ) ? PW_HUGE : PW_LARGE;
   }
   return PW_SMALL;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - HandleKeys
void FS1FIL::Sequence_OnOff( pCHAR &filnam ) { // printing sequence (one file) on/off 
   if( FS_1 == FS_UNUSED ) {
      filnam = get_name();                     // get name of file
      FS_1 = FS_INITIAL;                       // initial for sequence
      set_Time();                              // set begin time and time step
   }
   else FS_1 = FS_FINISH;                      // finish sequence
}

void FSMFIL::Sequence_OnOff() {                // printing sequence (several files) on/off 
   if( FS_M == FS_UNUSED ) {
      if( fid_name(def_id,IPOS_M) ) nFil = 0;  // get file identifyer
      FS_M = FS_INITIAL;                       // initial for sequence
      set_Time();                              // set begin time and time step
   }
   else FS_M = FS_FINISH;                      // finish sequence
}

#define  F_IDENT  4             // length for user part of filename

// to execute all keystrokes for PostScript image representation
BOOL MAKEPS::HandleKeys( INT2 c ) {
   switch( c ) {
      /* ## deze is voorlopig uit
      // -- layout of multiple pages:
      case PS_CUTLINES:             // alt_l: toggle3 to draw cut lines
         if( !Divide )
            if( HPS.DrwCutL == JOINING ) HPS.DrwCutL = NOLINES;
            else                         HPS.DrwCutL = (CUTLIN)(HPS.DrwCutL+1);
         return TRUE;
      */
      // -- layout of page:
      case PS_DATE:                 // alt_d: toggle draw date
         if( !Divide )
            HPS.DrwDate = ( HPS.DrwDate == DATE_NO ) ? DATE_YES : DATE_NO;
         return TRUE;
      case PS_MARK_PAGE:            // alt_m: mark page corner
         if( !Divide )
            if( HPS.MarkCor == MARK_BOTH ) HPS.MarkCor = MARK_NO;
            else                           HPS.MarkCor = (DRAWMK)(HPS.MarkCor+1);
         return TRUE;
      case PS_SPREADING:            // alt_\: vary margins between pictures (-43)
         if( !Divide )
            HPS.Margins = ( HPS.Margins == MG_SPREAD )
                        ? MG_NARROW : (MARGIN)( 1 + (INT2)HPS.Margins );
         return TRUE;
                          // -- cadre: ten steps from portrait to landscape
      case PS_CADRE_LSCP:           // 'c': print in landscape format
         if( !Divide ) PortFac =  1; return TRUE;
      case PS_CADRE_PORT:           // 'C': print in portrait format
         if( !Divide ) PortFac = -1; return TRUE;
      case PS_CADRE_P2L:            // alt_c: alter format
         if( !Divide ) {
            PortFac += 0.2;
            if( PortFac > 1 ) PortFac = 1;
         }
         return TRUE;

      // -- layout of picture:
      case PS_FRAME:                // alt_f: toggle, draw picture frame
         HPS.DrwFrme = ( HPS.DrwFrme == FRAME_NO ) ? FRAME_YES : FRAME_NO;
         return TRUE;
      case PS_TITLE:                // alt_t: toggle, draw picture title
         HPS.DrwTitl = ( HPS.DrwTitl == TITLE_NO ) ? TITLE_YES : TITLE_NO;
         return TRUE;
   /* ## conflicts with show of generations of arcs
      case PS_BK_WHITE:             // 'g': background is WHITE
         Message( _T("BackGray = %Lg"), HPS.BckGray=1 ); return TRUE;
      case PS_BK_GRAY:              // 'G': background is a bit dark
         Message( _T("BackGray = %Lg"), HPS.BckGray=0.8 ); return TRUE;
      case PS_BK_DARKER:            // alt_g: background is darkening
         if( HPS.BckGray > 0.8 ) HPS.BckGray -= 0.02;
         Message( _T("BackGray = %Lg"), HPS.BckGray );
         return TRUE;
   */
      case PS_PAGETHICK:            // 'T': values of page thickness
         HPS.PagTh = ( HPS.PagTh == TK_GIGANTIC )
                   ? TK_TINY : (TH_IDX)( 1 + (INT2)HPS.PagTh );
         pCHAR PagT;
              if( HPS.PagTh == TK_TINY )     PagT = _T("TINY");
         else if( HPS.PagTh == TK_SMALL )    PagT = _T("SMALL");
         else if( HPS.PagTh == TK_MEDIUM )   PagT = _T("MEDIUM");
         else if( HPS.PagTh == TK_LARGE )    PagT = _T("LARGE");
         else if( HPS.PagTh == TK_HUGE )     PagT = _T("HUGE");
         else if( HPS.PagTh == TK_VAST )     PagT = _T("VAST");
         else if( HPS.PagTh == TK_GIGANTIC ) PagT = _T("GIGANTIC");
         Message( _T("Page thickness is %s"), PagT );
         return TRUE;


				 // MT: unused
      //case PS_PICTTHICK:            // 't': values of picture thickness
      //   switch( (INT2)HPS.PicTh ) {
      //      case NormTh: Message( _T("Thinnest printing .. .") );
      //                   HPS.PicTh = ThinTh; break;
      //      case ThinTh: Message( _T("* * Showy printing * * !") );
      //                   HPS.PicTh = ShowTh; break;
      //      case ShowTh: Message( _T("Fatty printing O o .") );
      //                   HPS.PicTh = ThckTh; break;
      //      case ThckTh: Message( _T("Normal printing") );
      //                   HPS.PicTh = NormTh; break;
      //   }
      //   return TRUE;

      // -- printing of page:
      case PS_ALTER_LPT:            // '[': alter print queue for .eps file
         if( ++LPT > 3 ) LPT = 0;
      case PS_SHOW_LPT:             // ']': show print queue for .eps file
         if( LPT > 0 ) Message( _T("Current PS_printing to lpt%d"), LPT );
         else          Message( _T("No PS-printing") );
         return TRUE;

      case PS_MLTPAGEF:             // 'M': draw multiple pages, var. file names
         if( !Divide ) {                  // ## print on screen for input
            // QuickMs( _T("give file identification ( 1 to F_IDENT (4) chars )") );
            FileIdent( _T("give file identification ( 1 to F_IDENT (4) chars )") );
   }


      case PS_MLTPAGE:              // 'm': draw multiple pages
         if( !Divide ) {
            if( c == PS_MLTPAGE ) STRNCPY( swt_name+PATHLEN, _T("Page"), F_IDENT );
            r_magnify = 0;
            // QuickMs( _T("give magnification factor ( > 1 )") );
      while( r_magnify < 1 ) {
               // cin >> r_magnify;
								r_magnify = ReadReal(_T("give magnification factor ( > 1 )"));
               if( r_magnify < 0 )  // mechanism to interrupt the procedure
                  return TRUE;
            }
            MultiP = TRUE;
            cur_name = swt_name;
            Message( _T("magifying factor is %lg (file = %s)"), (double)r_magnify, swt_name );
   }
         return TRUE;

      // -- printing a series of contours with a fixed timestep:
      case PS_FIXSTEP_1:                    // F11: initial/finish the series
         if( FS_M == FS_UNUSED ) FS1FIL::Sequence_OnOff( cur_name );
         else Message( _T("FixStep printing on several files already in progress") );
         return TRUE;

      case PS_FIXSTEP_M:                    // F12: initial/finish the series
         if( FS_1 == FS_UNUSED ) FSMFIL::Sequence_OnOff();
         else Message( _T("FixStep printing on one file already in progress") );
         return TRUE;

      // ## hier 'b' en 'B' voor cur_name = nextname();
      case 'b':                     // print next picture, by using other file name
         if( !Divide ) {
            cur_name = var_name();
            Message( _T("Print and draw picture of full page to file %s"), cur_name );
            HPS.PagTh = TK_LARGE;
            DoDraw = TRUE;
         }
         return TRUE;

      case PS_PICTURE:              // 'p': print next picture (even whole page)
         if( Divide ) {
            Message( _T("Print picture %d to file"), iPic+1 );
            if( iPic+1 == nWid*nHgt )
               Message( _T("Draw divided page by completion") );
         }
         else {
            Message( _T("Print and draw picture of full page") );
      // ## unused            cur_name = fix_name;
            cur_name = fix_name;
            HPS.PagTh = TK_LARGE;
         }
         return DoDraw = TRUE;

      case PS_DIVIDED:              // 'P': divide page into several pictures
         if( !Divide ) {
            cur_name = fix_name;
            PageDivide();
         }
         else Message( _T("Page is already divided!") );
         return TRUE;
      case PS_PLACE_I:              // 'i': print number i out of sequence
      case PS_SEQUENCE_I:           // 'I': print number I to continue the sequence
         if( Divide ) PictDirect( (BOOL)(c==PS_SEQUENCE_I) );
         return TRUE;
      case PS_FINISH:               // alt_p: print page to be finished
         if( Divide ) {
            Message( _T("Draw divided page by request") );
            Finish = TRUE;
         }
         return TRUE;
      case PS_CANCEL:               // Esc: cancel divided print
         if( Divide ) return PageQuit();        // return TRUE !
   }
   return FALSE;
}

pCHAR MAKEPS::var_name() {
   static CHAR fname[] = _T("eps\\zgzgXXXX.eps"); 
   static INT2 nFile = 0;
	 // STRCPY(fname, _T("eps\\zgzgXXXX.eps"));
   SPRINTF( &fname[8], _T("%4.4d.eps"), nFile++ );
	 return fname;
}

void MAKEPS::FileIdent(pCONSTCHAR prompt) {
   CHAR fi[] = _T("__________");
   // MT: replaced cin >> fi with ReadString.
	 ReadString(fi, prompt);
   INT2 i;
   for( i = 0; *(fi+i) != '\0' && i < F_IDENT; i++ ) *(swt_name+PATHLEN+i) = *(fi+i);
   while( i < F_IDENT ) *(swt_name+PATHLEN+i++) = '_';
}

// -- modules to be executed on keystroke
BOOL MAKEPS::HandleMultPage() {     // handle pages of a picture of multiple pages
   if( !MultiP ) return FALSE;      // printing of multiple pages is not active
   if( !DoDraw ) {                  // initialization of requested printing
      pIdx = 0;
      DoDraw = TRUE; 
      PSF.InitMultPage( CadreMidPnt(), 2*XhRng(), Ratio(), r_magnify,
                        HPS.MarkCor, HPS.DrwDate, HPS.DrwFrme, HPS.DrwCutL,
                        HPS.PagTh, HPS.BckGray, pCol, pRow );
   }
   if( pIdx == pCol*pRow ) return DoDraw = MultiP = FALSE;
   Message( _T("Make page nr %d of MP-picture (%d,%d)"), pIdx+1, pIdx%pCol+1, pIdx/pCol+1 );
   // positions 8 and 9 for colum (x), positions 10 and 11 for row (y)
   SPRINTF( swt_name+8, _T("%2.2d%2.2d.eps"), pIdx%pCol+1, pIdx/pCol+1 );
   return TRUE;
}
void MAKEPS::PageDivide() {
   Divide = TRUE;
   iPic = nWid = nHgt = 0;
   // MT: replaced cin
   // QuickMs( _T("give colums and rows: ") );  // ## print on screen for input
	 while( nWid <= 0 || nHgt <= 0 ) {
		 // cin >> nWid >> nHgt;
		 nWid = ReadInt(_T("Number of columns:"));
     if (nWid == -9999)
       return;
		 nHgt = ReadInt(_T("Number of rows:"));
     if (nHgt == -9999)
       return;
   }
	 // TODO MT: 4 == A4
   PSF.Open( cur_name, LPT, 4);            // open file and prepare to draw
   PSF.DividePage( nWid, nHgt, Ratio(),  // several pictures on page
                   HPS.UsePage, HPS.MarkCor, HPS.DrwDate, HPS.Margins );
   HPS.SetPageTh( nWid*nHgt );
}
BOOL MAKEPS::PageQuit() {                // cancel imaging
   Message( _T("Divided page cancelled!") );
   Divide = FALSE;
   PSF.Quit();                           // close PSF-file, no print
   return TRUE;
}
void MAKEPS::PictDirect( BOOL seq ) {    // denote specific image to be done next
   INT2 n = 0;
   // MT: refactored
   // QuickMs( _T("between 1 and %d"), nWid*nHgt );  // ## on screen for input
	 CHAR prompt[20];
   SPRINTF(prompt, _T("Between 1 and %d"), nWid*nHgt );  // ## on screen for input
	 while( n < 1 || n > nWid * nHgt ) {
		 // cin >> n;
		 n = ReadInt(prompt);
	 }
   if( seq ) {                           // fix selection to loop index
      Message( _T("Print requested picture %d to be sequential"), n );
      iPic = n - 1;
   }
   else Message( _T("Print requested picture %d"), jPic=n );
   DoDraw = TRUE;
}

BOOL MAKEPS::FS1_TestNext( REAL T ) {     // test for next drawing in Fixstep series
   if( FS_1 == FS_UNUSED ) return FALSE;
   if( FS_1 == FS_INITIAL ) {
      if( T >= FS_LastTime ) return DoDraw = TRUE;
   }
   else if( T >= FS_LastTime + FS_TimeStep ) {
      FS_LastTime += FS_TimeStep;
      return DoDraw = TRUE;
   }
   return FALSE;
}

// executed before a screen representation, if PostScript image representation is requested !
BOOL MAKEPS::FS1_Prolog() {              // (sometimes) do not MakePSProlog for series
   if( FS_1 == FS_UNUSED  ) return TRUE; // unused - do prolog if requested
   if( FS_1 == FS_INITIAL ) {            // first step in series
      FS_1 = FS_STEPUP;                  //        - change status
      return TRUE;                       //        - do prolog
   }
   return FALSE;                         // all other states - no prolog
}

// executed after a screen representation, if PostScript image representation is requested !
BOOL MAKEPS::FS1_Epilog() {              // (sometimes) do not MakePSEpilog for series
   if( !DoDraw ) return FALSE;                  // no draw - no epilog
   if( FS_1 == FS_FINISH  ) FS_1 = FS_UNUSED;   // FS_UNUSED and FS_FINISH
   if( FS_1 == FS_UNUSED  ) return TRUE;        // .. both require DisplayEpilog()
   return DoDraw = FALSE;                       // other states of FS_1 do not
}

BOOL MAKEPS::FSM_TestNext( REAL T ) {    // test for next drawing on seperate files
   if( FS_M == FS_UNUSED ) return FALSE;
   if( FS_M == FS_INITIAL ) {
      if( T >= FS_LastTime ) {
         cur_name = nxt_name();          // next step - get name of file
         return DoDraw = TRUE;
      }
   }
   else if( T >= FS_LastTime + FS_TimeStep ) {
      FS_LastTime += FS_TimeStep;
      cur_name = nxt_name();             // next step - get name of file
      return DoDraw = TRUE;
   }
   return FALSE;
}

// executed before a screen representation, if PostScript image representation is requested !
void MAKEPS::FSM_Prolog() {              // (sometimes) do not MakePSProlog for series
   if( FS_M == FS_INITIAL ) FS_M = FS_STEPUP;   // first step - change status
}

// executed after a screen representation, if PostScript image representation is requested !
void MAKEPS::FSM_Epilog() {              // (sometimes) do not MakePSEpilog for series
   if( FS_M == FS_FINISH  ) FS_M = FS_UNUSED;   // FS_UNUSED and FS_FINISH
}

// ##### eerst nadenken 2018-04-20
// void MAKEPS::DisplayProlog(BZTYP W) {  // do before graphical display
   void MAKEPS::DisplayProlog( ScrDrawCirc CirW, ScrDrawTria TriW ) {  // do before graphical display
   if( !FS1_Prolog() ) return;           // (sometimes) do not MakePSProlog for series
   FSM_Prolog();                         // check for series on many files
   if( !DoDraw ) return;
   if( MultiP ) {
      PSF.Open( cur_name, LPT, 4 /* MT */);         // prepare to draw one single picture
      PSF.DrawMultPage( pIdx%pCol, pIdx/pCol );
   }
   else if( Divide ) {                   // prepare to draw a picture
      INT2 j = (jPic!=0) ? (jPic-1) : iPic;
      PSF.InitPicture( CadreMidPnt(), XhRng(), j, HPS.PagTh, HPS.DrwFrme, HPS.BckGray );
   }
   else {
//      PSF.Open( SetName(W), LPT );       // prepare to draw one single picture in a series
      PSF.Open( cur_name, LPT, 4 /* MT */ );         // prepare to draw one single picture
      PSF.PicturePage( CadreMidPnt(), XhRng(), Ratio(),
                       HPS.UsePage, HPS.MarkCor, HPS.DrwDate,
                       HPS.PagTh, HPS.DrwFrme, HPS.BckGray );
   }
}

void MAKEPS::DisplayEpilog() {           // do after graphical display
   if( !FS1_Epilog() ) return;           // (sometimes) do not MakePSEpilog for series
   FSM_Epilog();                         // ??
   if( !DoDraw && !Finish ) return;

   if( MultiP ) {
      pIdx++;
      PSF.PageEnd();                  // close and print
      PSF.Close();                    // full size picture
   }
   else if( Divide ) {
      DoDraw = FALSE;                 // end of drawing state
           if( jPic != 0 ) jPic = 0;  // no denoted picture in future
      else if( ++iPic == nWid*nHgt || Finish ) { // page is finished
         Divide = Finish = FALSE;
         PSF.PageEnd();               // full or forced
         PSF.Close();                 // close file and print
      }
   }
   else {
      DoDraw = FALSE;                 // end of drawing state
      PSF.PageEnd();                  // close and print
      PSF.Close();                    // full size picture
   }
}

// used by flatgrph.cpp/DrawCadre()
void MAKEPS::SetTitle( pCHAR T ) {       // draw title of one picture
   if( !MultiP ) PSF.SetTitle( T );
}

// modules to draw graphical elements: point, line, polygon, circle, arc, curve ----
void MAKEPS::Point( pPNT2 p, MskP mP ) {
   HPS.PWidMsk( mP );
   PSF.DrawPoint( p );
}

void MAKEPS::Line( pPNT2 p, pPNT2 q, MskL mL ) { // draw line with own MskL
   HPS.LWidMsk( mL );
   PSF.DrawLine( p, q );
}
void MAKEPS::Line( pPNT2 p, pPNT2 q ) {      // id. (MskL is smallest)
   HPS.LWidMsk( L_TINY );                    // ## L_NULL ?
   PSF.DrawLine( p, q );
}

void MAKEPS::Poly( pCLOS pol, MskL mL ) {  // -- draw polygon ! using curve drawing modules
   INT2 W;                                    // index of current point (0 or 1)
   PNT2 P[2];                                 // current/previous point (dependent of W)

   SUPPT S( pol );
   S.c->other(S.b)->BisPt( &P[W=0], Prcs.D ); // W == 0 for first point
   PolygOpen( pol, &P[W], mL );

   do {
      S.b->BisPt( &P[W=1-W], Prcs.D );        // switch W for trailing point(s)
      CurveCont( &P[W] );
   } while( S.next() );

   CurveEnd();
}

void MAKEPS::Circ( pPNT2 m, REAL R, MskL mL ) {
   HPS.LWidMsk( mL );
   PSF.DrawCirc( m, R );
}
void MAKEPS::Arc( pPNT2 m, REAL R, REAL L, REAL H, MskL mL ) {
   HPS.LWidMsk( mL );
   PSF.DrawArc( m, R, L, H );
}

void MAKEPS::Comp( pCLOS cmp, MskL mL ) {  // -- draw component ! using special modules
   CompoOpen( cmp, mL );
   SUPPC S( cmp, DIRECTION, Prcs.D );
   do {
      if( !NoColor ) SetColor( S.c->Type );       // colour of current arc
      if( S.c->Neigh != S.c ) PSF.DrawArcZZ( S.cp, S.cR, S.L, S.H );
      else                    PSF.DrawCrcZZ( S.cp, S.cR );
   } while( S.next() );
}

// -- drawing a curve or component or polygon ------------------------------------------
//  ! must be uninterrupted by other draws
void MAKEPS::CompoOpen( pCLOS cmp, MskL mL ) {
   HPS.LWidMsk( mL );
   PSF.OpenCompo( (INT4)cmp );
}
void MAKEPS::PolygOpen( pCLOS pol, pPNT2 p, MskL mL ) {
   HPS.LWidMsk( mL );
   PSF.OpenPolyg( p, (INT4)pol );
}
void MAKEPS::CurveOpen( pPNT2 p, MskL mL ) {
   HPS.LWidMsk( mL );
   PSF.OpenCurve( p );
}
void MAKEPS::CurveCont( pPNT2 p ) { PSF.DrawCurve( p ); }
void MAKEPS::CurveEnd() { PSF.EndCurve(); }

// ------------------------------------------------- to communicate domain values
// -- ratio of ranges for PS-file
REAL MAKEPS::Ratio() { return POW( DatRatioYX(), PortFac ); }
// -- domain half of x-range for PS-file
REAL MAKEPS::XhRng() { return POW( DatRatioYX(), 1-PortFac ) * HalfRangeX(); }

// -- show PostScript options on screen
void MAKEPS::ShowPSOpt( int X, int Y ) {
   // -- show drawing cadre or not
   BOOL Frame = (BOOL)( HPS.DrwFrme == FRAME_YES ); // draw frame or not
   REAL DForm = POW( DatRatioYX(), 1-PortFac );     // format of drawing
   SetColor( Frame ? LGy : DGy );
   INT2 E = (INT2)( (REAL)X * ( 1 - DForm ) / 2 );  // margin of deviated cadre
   line( E, 0, X-E, 0 ); line( E,   Y, X-E, Y );    // horizontal cadre lines
   line( E, 0, E,   Y ); line( X-E, 0, X-E, Y );    // vertical cadre lines

   // -- vary picture margins
   INT2 DMarg = HPS.Margins;                 // MG_NARROW, MG_NORMAL, MG_SPREAD,
   for( INT2 i = 0; i < DMarg; i++ ) PutPixel( X-2, 30+3*i, LMa );

   // -- show drawing of cutlines
   INT2 CutLn = HPS.DrwCutL;                 // draw cutlines: NOLINES, OVERLAP, JOINING,
   if( CutLn != NOLINES ) {
      SetColor( DBe );
      E = 2;
      line( E,   E, X-E, E );                // line above
      line( X-E, E, X-E, Y-E );              // line at right
      if( CutLn == JOINING ) {
         line( E, E,   E,   Y-E );           // line at left
         line( E, Y-E, X-E, Y-E );           // line below
      }
   }

   // -- mark or date at page corner
   BOOL MCorL = (BOOL)( HPS.MarkCor == MARK_BOTH || HPS.MarkCor == MARK_LEFT ); 
   BOOL MCorR = (BOOL)( HPS.MarkCor == MARK_BOTH || HPS.MarkCor == MARK_RIGHT );
   BOOL DDate = HPS.DrwDate;
   INT2 M = 1;
   if( MCorL ) PutPixel( E+M, Y-M, LGn );    // mark left corner
   if( DDate ) {                             // draw date (right corner only)
      PutPixel( X-M-3, Y-M, LRd );
      PutPixel( X-M-2, Y-M, LRd );
      PutPixel( X-M-1, Y-M, LRd );
      PutPixel( X-M,   Y-M, LRd );
   }
   else if( MCorR ) PutPixel( X-E-M, Y-M, LGn ); // mark right corner
}

#endif                     // _VIEWGRPH_CPP
#endif                     // _MAKE_PS_H
