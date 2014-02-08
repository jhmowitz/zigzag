/*
 * mow_type.h        -- to serve as basic definition file (july '99)
 *                   -- replacement of gen_def.h (tryout: march '95)
 */

#include <stdio.h>

#ifdef WIN32

typedef FILE *            pFILE;    // definition of (pointers to) structures
typedef void *            pVOID;    // of general usage
typedef wchar_t            CHAR;    // ! new usage
typedef wchar_t *         pCHAR;
typedef const wchar_t *	  pCONSTCHAR;		// Pointer to const char
typedef unsigned char     uBYTE;
typedef short *           pINT2;
typedef short              INT2;
typedef unsigned short    uINT2;
typedef short              BOOL;
typedef int                INT4;
typedef unsigned int      uINT4;
typedef float             sREAL;

#else

typedef FILE *            pFILE;    // definition of (pointers to) structures
typedef void *            pVOID;    // of general usage
typedef char               CHAR;    // ! new usage
typedef char *            pCHAR;
typedef unsigned char     uBYTE;
typedef int *             pINT2;
typedef int                INT2;
typedef unsigned int      uINT2;
typedef int                BOOL;
typedef long               INT4;
typedef unsigned long     uINT4;
typedef float             sREAL;

typedef char               TEXT;    // ! old usage
typedef char *            pTEXT;

#endif

#ifdef __LONGREAL__                 // use long double -- 10 bytes floating point
   typedef long double        REAL;
   typedef long double *     pREAL;
   typedef long double * *  ppREAL;

   #define     ABS           fabsl
   #define     SQRT          sqrtl
   #define     SIN           sinl
   #define     COS           cosl
   #define     TAN           tanl
   #define     ASIN          asinl
   #define     ACOS          acosl
   #define     ATAN          atanl
   #define     POW           powl
   #define     LOG10         log10l
   #define     POW10         pow10l
#else                               // use double -- 8 bytes floating point
   typedef double             REAL;
   typedef double *          pREAL;
   typedef double * *       ppREAL;
   
   #define     ABS           fabs
   #define     SQRT          sqrt
   #define     SIN           sin
   #define     COS           cos
   #define     TAN           tan
   #define     ASIN          asin
   #define     ACOS          acos
   #define     ATAN          atan
   #define     POW           pow
   #define     LOG10         log10
   #define     POW10         pow10
#endif

#define   UNDEF         ((BOOL)-1)                       // booleans
#define   FALSE         ((BOOL)0)
#define   TRUE          ((BOOL)1)
#define   TWICE         ((BOOL)2)

#define   HPI          (REAL)1.570796326794896619231322  // math. constants
#define   _PI          (REAL)3.141592653589793238462644
#define   TPI          (REAL)6.283185307179586476925287

#define   arcF( d )    ((REAL)( d ) * TPI / 360 )        // expressions arcs
#define   degF( a )    ((REAL)( a ) * 360 / TPI )        // .. versus degrees
