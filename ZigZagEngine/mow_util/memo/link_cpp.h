/* link_cpp.h
 *
 *    source(s) for link class LINK
 *
 *    and bulk memory control for link classes derived from LINK
 *    ! with return to storage within application range
 *    ! with deletion of all allocated memory
 */

#include <new.h>           // new, delete memory

#include MOW_LINK_H        // "c:\mow_util\memo\link_def.h" -- LINK class definition

// ************************************ has to be provided by calling program
extern void Exit( pCHAR, ... );

// ------------------------------------------------------------- modules for class LINK
INT2 LINK::NN() {                    // count length of linkage behind nx
   INT2 N = 0;
   for( pLINK a = nx; a != NULL; a = a->nx ) N++;
   return N;
}

pLINK LINK::Pop() {                  // delink and return from behind this
   if( nx == NULL ) Exit( L"LINK::Pop() -- nx is NULL" );

   pLINK lk = nx;
   nx = lk->nx;                      // remove lk from linkage
   lk->nx = NULL;                    // ## not really necessary
   return lk;
}

// --------------------- template and modules of memory classes derived from class LINK

// maxS -- maximally available number of addresses to allocate memory strips
// nByt -- length of one strip of memory
template < class MEMO, INT2 maxS, INT2 nByt > class GMEMO {
   MEMO *Memo[maxS];       // array of pointers to MEMO, to allocate strips
   INT2 numS;              // number of allocated memory strips
   INT2 numC;              // number of classes in one memory strip
   LINK Keep;              // top class for storage of unused classes
public:
   GMEMO() {               // initialize:
      numS = 0;            // no memory strips yet
      numC = nByt / (INT2)sizeof( MEMO );
      Keep.Clear();        // condition to allocate next memory strip
   }
   MEMO *Get();            // get one class, automatic allocation
   void Put( pLINK );      // put previously used class back on store
   void Reset();           // delete all allocated memory
   INT2 N_() { return numS * numC; }             // number of allocated classes
   INT2 NS() { return Keep.NN(); }               // number of classes in store
   INT2 KB() { return numS * ( nByt / 1024 ); }  // allocated kbyte
};

// get one class, automatic allocation
template < class MEMO, INT2 maxS, INT2 nByt >
MEMO *GMEMO< MEMO, maxS, nByt >::Get() {
   if( Keep.Empty() ) {                      // storage list is empty
      if( numS == maxS )
         Exit( L"GMEMO::Get() -- strip of addresses is full" );
      Memo[numS++] = new MEMO[numC];         // allocate new classes
      if( Memo[numS-1] == NULL )             // no memory left
         Exit( L"GMEMO::Get() -- no memory left" );
      for( INT2 i = 0; i < numC; i++ )       // put newly allocated classes on storage
         Keep.Push( &Memo[numS-1][i] );
   }
   return (MEMO *)Keep.Pop();                // return next class (first from storage)
}

// put previously used class back on storage
template < class MEMO, INT2 maxS, INT2 nByt >
void GMEMO< MEMO, maxS, nByt >::Put( pLINK pl ) {
   Keep.Push( pl );
}

// delete all allocated memory
template < class MEMO, INT2 maxS, INT2 nByt >
void GMEMO< MEMO, maxS, nByt >::Reset() {
   while( numS > 0 ) delete Memo[--numS];    // delete allocated memory
   Keep.Clear();                             // clear top of storage
}
