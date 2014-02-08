// link_def.h
// definition class for single linkage with NULL-ending

typedef class LINK *      pLINK;        // pointer to basic class of linkage

class LINK {                         // -- class definition
   pLINK nx;                            // next class in nx-linkage

public:
                                     // -- modules to handle linkage
   BOOL Empty() {                       // report link to NULL (or not)
      return (BOOL)( nx == NULL );
   }

   void Clear() { nx = NULL; }          // clear link

   void Link( pLINK lk ) { nx = lk; }   // link to nx

   void Cut() { nx = nx->nx; }          // delink from behind this

   pLINK Next() { return nx; }          // return link

   void Push( pLINK lk ) {              // link lk behind this
      lk->nx = nx;
      nx = lk;
   }

   pLINK Pop();                         // delink and return from behind this

   INT2 NN();                           // count length of linkage behind nx
};
