
extern INT2 GetChr();                     // get one key
extern void Sleep( INT2 );                // delay n of seconds

extern void Message( pCHAR, ... );        // to visual screen and repaint
extern void Mess_LT( pCHAR, ... );        // to visual screen and repaint after long time
extern void QuickMs( pCHAR, ... );        // to visual screen, no repaint
extern void Exit( pCHAR, ... );           // exit from program
extern BOOL  TimMessage( pCHAR, INT2 );   // message for some seconds or until a key stroke
extern pCHAR fillScrBuffer( pCHAR, ... ); // fill buffer ScrBuff and return it
