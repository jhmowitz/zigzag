#include "MOW_DEFS.H"

extern void delay(uINT2);
extern INT4 biostime(INT2, INT4);

extern INT2 getch();
extern BOOL kbhit();

// Windows heeft geen Console, dus we leiden printf() en gets() om
#define PRINTF DoPrintf
#define GETS DoGets

extern void DoPrintf(pCONSTCHAR, ...);
extern void DoGets(pCONSTCHAR, pCHAR, INT2);
extern void NotifyEvent(pCONSTCHAR, pCONSTCHAR);
extern void ReadString(pCHAR s, pCONSTCHAR prompt);
extern INT2 ReadInt(pCONSTCHAR prompt);
extern REAL ReadReal(pCONSTCHAR prompt);
extern pFILE DoFopen(pCONSTCHAR, pCONSTCHAR);
