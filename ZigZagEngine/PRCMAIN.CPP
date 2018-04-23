// prcmain.cpp - PROCESS main program - first test version through 1994

#ifdef WIN32
#include "doscompat.h"
#include <stdio.h>
#else
#include <alloc.h>         // farmalloc()
#include <conio.h>         // getch(), kbhit()
#endif

#include <ctype.h>         // toupper()
#include <dos.h>           // sleep(), delay()
#include "graphcmd.h"      // graphics library and definition of graphic dependencies
#include <stdarg.h>        // ... (argument list), vsprintf()
#include <stdlib.h>
#include <string.h>        // strncmp()

#include "mow_defs.h"      // incriptions for includes (use long double)

#include "alg_geo.h"       // structures for points, lines and circles
#include  MOW_LINK_H       // linkage: include class definition
#include "prc_str.h"       // serving structures

#define  _STT2MAIN_CPP     // ! in main program declare not externally
#include "key_def.h"
#include "prc_def.h"       // all general process classes
#undef   _STT2MAIN_CPP

#include "mycolors.h"      // transfer of type or COLOR to COLORS (for tessalation throw)

#include "prc_tim.h"       // structure for time handling (used for EVENT !)
#include "event.h"         // event handling -- included only here !

extern void DomainGraphInit( BOOL );    // set screen parameters: ratio
extern void DomainCadreInit( BOOL );          // initial VIEW and IMAGO
extern INT2 DomainHandleKeys( INT2 );   // parse for VIEW and IMAGO

extern BOOL TssThrowHandleKeys( INT2 ); // throw "regular" tesselations
extern BOOL TssThrowIsOn();             // throwing tesselation is active

#ifdef   __MAKE_PSF__                // -- if program is enabled to make PS-file
extern BOOL MakePSHandleKeys( INT2 );   // make PostScript files
extern void MakePSReInit();             // reinit file series names, if it has been used
#endif                               // __MAKE_PSF__

void main( INT2 argc, pCHAR argv[] ) {
   Prcs.GRPH::CommLine( argc, argv );                  // command line for graphics
   Prcs.GRPH::Open();                                  // open GRAPHICS

   DomainGraphInit( Prcs.GRPH::IsNT );                 // set screen parameter

   Prcs.Process();                                     // do processes
   Exit( L"" );                                         // finished
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - PROCS
void PROCS::Init() {     // the domain at start is (0,1) by (0,0.7)
   S = .0007;            // time step for animation (S = VDom.Diag / 1000 ?)
   T = X = 0;            // current time, keep time step
   F = 1;                // flag for reflected construction of triangles
}
void PROCS::Process() {               // -- run one process
   for( ; ; ) {
      DomainCadreInit( TRUE );           // set view to domain: Cadre
      InitClasses();                     // initialize all structures
      InitColor();                       // initial main color flags
      Init();                            // initialize one process
      Event.iniEvent();                  // clear event
#ifdef   __MAKE_PSF__                 // -- if program is enabled to make PS-file
      MakePSReInit();                 //    reinit file series names, if it has been used
#endif                                // __MAKE_PSF__
      (*ScrDraw)( 0 );                   // draw once before loop at time == 0

      // -- keys before processing
      while( !Event.Act ) {              // no events, yet: await a start
         if( !preParse() ) return;       // (Esc) stops the programm run
         (*ScrDraw)( 0 );                // .. no time extension
         Event.SetEvent();               // look for thrown events (regular or random)
      }
      // -- loop of keys and processing
      while( Processing() )              // processing one process
         Event.SetEvent();               // look for random (and new) events
   }
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - before Processing()
BOOL PROCS::preParse() {
   INT2 c = GetChr();                   // wait for a key
   switch( c ) {
      case CROSS_OVER:                  // '_': (no) connection between parts
         CrossOver = !CrossOver;
         if( CrossOver ) Message( L"Circles and triangles are connected" );
         else            Message( L"Circles and triangles are loose" );
         return TRUE;
   }
   if( EmerParse(c) ) {
      if( ExtraEm_V == 0 ) Message( L"Normal 'loss of structure'" );
      else Message( L"Chance %lg for extra emergence", (double)ExtraEm_V );
      return TRUE;
   }
   return _Parse( c );
}
BOOL PROCS::EmerParse( INT2 c ) {
   switch( c ) {
      case EXTRA_EMERG:            // '$': extra emergence at "loss of structure"
         ExtraEm_V = ( ExtraEm_V > 0 ) ? 0 : 1;
         return TRUE;
      case INCR_EMERG:             // 'Z': increase emergence chance by 0.05
         if( ExtraEm_V <= 0.95 ) ExtraEm_V += 0.05;
         return TRUE;
      case DECR_EMERG:             // 'z': decrease emergence chance by 0.05
         if( ExtraEm_V >= 0.05 ) ExtraEm_V -= 0.05;
         return TRUE;
      case BITD_EMERG:             // alt_z: decrease emergence chance by 0.01 (-44)
         if( ExtraEm_V >= 0.01 ) ExtraEm_V -= 0.01;
         return TRUE;
   }
   return FALSE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - during Processing()
BOOL PROCS::Processing() {               // -- processing one process
   if( Event.Act && T > Event.Lim )         // future event is in the past
      Exit( L"PROCS::Processing() -- corrupted time order" );

   while( Event.Act && T+S >= Event.Lim ) {
      if( kbhit() ) return Parsing();
      (*ScrDraw)( T=Event.Lim );            // draw at event time
      Event.ExcEvent();                     // do current event (next in time)
      Count();                              // ## test: count of production classes
   }
   while( Event.valT(T+S) ) {               // tweening till next event
      if( kbhit() ) return Parsing();
      (*ScrDraw)( T+=S );                   // draw with (fixed) step S
   }
   return TRUE;
}

BOOL PROCS::Parsing() {                   // -- parse as long as kbhit()
   do {                                      // .. or wait under tesselation throw
      INT2 c = GetChr();
      if( !_Parse(c) ) return FALSE;         // end of current process
      if( TssThrowIsOn() ) (*ScrDraw)( T );  // draw without time expansion
   } while( kbhit() || TssThrowIsOn() );
   return TRUE;
}

BOOL PROCS::_Parse( INT2 c ) {            // -- parsing for diverse actions
   if( TssThrowHandleKeys(c) ) return TRUE;  // "regular" and "random" tesselations
   if( DomainHandleKeys(c) ) return TRUE;    // VIEW and IMAGO
   if( TEST::Parse(c) )   return TRUE;       // testing
   if( PROCS::Parse(c) )  return TRUE;       // process
#ifdef   __MAKE_PSF__           // if program is enabled to make PS-file
   if( MakePSHandleKeys(c) ) return TRUE;    // make PostScript files
#endif                          // __MAKE_PSF__
   return (BOOL)( c != PROG_QUIT );          // not Escape
}

BOOL PROCS::Parse( INT2 c ) {          // -- parse for process
   switch( c ) {
      case INCR_STEP:                     // '+': increase process time step
         if( X == 0 ) S *= 1.1;
         else         X *= 1.1;
         return TRUE;
      case DECR_STEP:                     // '-': decrease process time step
         if( X == 0 ) S *=  .9;
         else         X *=  .9;
         return TRUE;
      case PAUSE_GO:                      // '!': toggle: process pause/go
         if( X == 0 ) { X = S; S = 0; }
         else         { S = X; X = 0; }
         return TRUE;
                                 // -- giving messages only !
      case CROSS_OVER:              // '_': (no) connection between parts
         Message( L"Circles and triangles are %s", CrossOver?"connected":"loose" );
         return TRUE;
      case EXTRA_EMERG:             // '$': extra emergence at "loss of structure"
      case INCR_EMERG:              // 'Z': increase emergence chance by 0.05
      case DECR_EMERG:              // 'z': decrease emergence chance by 0.05
      case BITD_EMERG:              // alt_z: decrease emergence chance by 0.01 (-44)
         if( ExtraEm_V == 0 ) Message( L"Normal 'loss of structure'" );
         else Message( L"Chance %lg for extra emergence", (double)ExtraEm_V );
         return TRUE;
   }
   return FALSE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - TEST
BOOL TEST::Parse( INT2 c ) {
   switch( c ) {
      // -- flow and test mode
      case TEST_MODE: TOn = !TOn; return TRUE;          // '|': toggle: testing
      case MORE_SECS: Sec++; return TRUE;               // '}': more test seconds
      case LESS_SECS: if( Sec ) Sec--; return TRUE;     // '{': less test seconds
      case MORE_FLOW: Flw++; return TRUE;               // 'F': more test flow
      case LESS_FLOW: if( Flw > 0 ) Flw--; return TRUE; // 'f': less test flow
      case IND_EVENT: Evt = !Evt; return TRUE;          // alt_e: indicate events (-18)

      case COUNT_CLASS:                                 // '&': toggle testing class count
         Cnt = !Cnt;
         if( Cnt ) Message( L"Class count on !" );
         else      Message( L"No class count _" );
         return TRUE;
#ifdef _CLASS_COUNT            // class count for program testing is done
      case SHOW_TABLE: EvTable(); return TRUE;          // '#': show event table
#endif                         //  _CLASS_COUNT
   }
   return FALSE;                                  // found nothing
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - GRPH
                                              // -- prototypes from viewgrph.h used by GRPH
extern void Draw1Page( REAL );                   // draw screen by one page
extern void Draw2Pages( REAL );                  // draw screen by switch of two pages

void GRPH::CommLine( INT2 n, pCHAR v[] ) {    // -- scan command lines
   setBGI( _T("C:\\BORLANDC\\BGI") );                // default for own use
   setDrv( _T("VGAMED") );                           // default for own use
   IsNT = FALSE;                                 // default: not Windows-NT

   for( INT2 i = 1; i < n; i++ )              // -- scan arguments
      if( v[i][0] == '-' )                       // beginning with '-'
         switch( toupper(v[i][1]) ) {            // switch options
            case 'B': setBGI( &v[i][2] ); break; // graphics: directory for BGI file
            case 'G': setDrv( &v[i][2] ); break; // graphics: driver, mode
            case 'N': IsNT = TRUE; break;        // Windows-NT detection
         }

   if( IsNT && Mode != VGAMED ) IsNT = FALSE;    // Windows-NT only relevant for VGAMED
}
void GRPH::setBGI( pCHAR W ) { dirBGI = W; }
void GRPH::setDrv( pCHAR W ) {
   Driver = VGA;                                           // set driver
        if( *W == '\0' )                    Mode = VGAMED; // set mode
   else if( STRNCMP(W,_T("VGA"),3) == 0 )
           if( STRNCMP(&W[3],_T("LO"),2) == 0 ) Mode = VGALO;
      else if( STRNCMP(&W[3],_T("HI"),2) == 0 ) Mode = VGAHI;
      else                                  Mode = VGAMED;
   else Exit( L"The program works on VGA only !" );
   ScrDraw = ( Mode == VGAHI ) ? Draw1Page : Draw2Pages;   // set drawing module
}

void GRPH::Open() {
   initgraph( &Driver, &Mode, dirBGI );
   IsOpen = TRUE;
}
void GRPH::Close() {
   closegraph();
   IsOpen = FALSE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - message
INT2 GetChr() {
   INT2 c = getch();
   return ( c == 0 ) ? -getch() : c;
}
void Sleep( INT2 Sec ) {
   INT2 tSec = 10 * Sec;          // delay per 1 / 10 of a second
   for( INT2 i = 0; i < tSec; i++ )
      if( kbhit() ) break;
      else delay( (uINT2)100 );
}

CHAR ScrBuff[500];     // buffer for Message(), QuickMs() and Exit()

void MESSAGE::GetImage( INT2 l, INT2 r, INT2 u, INT2 d ) {
   L = l; R = r; U = u; D = d;
   ima = farmalloc( imagesize(L,U,R,D) );
   getimage( L, U, R, D, ima );
}
void MESSAGE::ShowText( INT2 Sec, pCHAR Txt ) {
   SetColor( LGy );
   OUTTEXTXY( L, U, Txt );
   Sleep( Sec );
}
void MESSAGE::PutImage() {
   putimage( L, U, ima, COPY_PUT );
   free( (void*)ima );
}
void MESSAGE::Message( INT2 Sec, pCHAR Txt ) {
   if( Prcs.IsOpen ) {
      GetImage( 5, GETMAXX(), GETMAXY()-15, GETMAXY()-5 );
      ShowText( Sec, Txt );
      PutImage();
			PRINTF( L"\n%s", Txt ); // MT: added,so we show the message on the console log
   }
   else PRINTF( L"\n%s", Txt );
}

// ! writes to the visible page, repaints after a certain delay
void Message( pCHAR fmt, ... ) {
   va_list ap;
   va_start( ap, fmt ); VSPRINTF( ScrBuff, fmt, ap );
   va_end( ap );
   Mess.Message( Prcs.Sec, ScrBuff );
}
// ! writes to the visible page (longtime), repaints after a certain delay
void Mess_LT( pCHAR fmt, ... ) {
   va_list ap;
   va_start( ap, fmt ); VSPRINTF( ScrBuff, fmt, ap );
   va_end( ap );
   Mess.Message( 3*Prcs.Sec, ScrBuff );
}

// ! writes to the visible page, no repaint
void QuickMs( pCHAR fmt, ... ) {
   va_list ap;
   va_start( ap, fmt ); VSPRINTF( ScrBuff, fmt, ap );
   va_end( ap );
   if( Prcs.IsOpen ) {
      INT2 W = (INT2)TEXTWIDTH( ScrBuff ) + 1,
           H = (INT2)TEXTHEIGHT( ScrBuff ) + 1;
      SetColor( LWe ); OUTTEXTXY( GETMAXX()-W, GETMAXY()-H, ScrBuff );
   }
   else PRINTF( L"\n%s", ScrBuff );
}
void Exit( pCHAR fmt, ... ) {
   va_list ap;
   va_start( ap, fmt ); VSPRINTF( ScrBuff, fmt, ap );
   va_end( ap );
   if( Prcs.IsOpen ) Prcs.Close();
   PRINTF( L"\n ** exit Bubble & Zigzag .. %s\n", ScrBuff );
   exit( -1 );
}

BOOL TimMessage( pCHAR ms, INT2 Sec ) {
   Mess.Message( Sec, ms );
   if( kbhit() )
      if( GetChr() == 77 ) return TRUE;             // 'M' ## voorlopig
   return FALSE;
}

pCHAR fillScrBuffer( pCHAR fmt, ... ) {
   va_list ap;
   va_start( ap, fmt ); VSPRINTF( ScrBuff, fmt, ap );
   va_end( ap );
   return ScrBuff;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - test write
void PrintTest( pCHAR fmt, ... ) {
   static BOOL TestActive = FALSE;
   static BOOL FirstWrite = TRUE;

   if( !TestActive ) return;

   static FILE *tlst;
   if( FirstWrite ) {
      FirstWrite = FALSE;
      tlst = FOPEN( _T("test.lst"), _T("w") );
   }
   else tlst = FOPEN( _T("test.lst"), _T("a") );

   CHAR buff[120];
   va_list ap;
   va_start( ap, fmt ); VSPRINTF( buff, fmt, ap );
   va_end( ap );
   FPRINTF( tlst, buff );

   fclose( tlst );
}

