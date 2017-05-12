// prc_tim.h -- time definitions for PROCESS
//              guard a minimal time value being larger than a starting one

class GTIM {                        // -- guard time integrity of a check
public:
   BOOL Act;                           // is active
   REAL Lim;                           // smallest upper bound for check

   GTIM() { Act = FALSE; }             // initial/nonactive by declaration
   void iniT() { Act = FALSE; }        // initial/nonactive by function for reuse
   
   BOOL valT( REAL T ) {               // test for valid value
      if( !Act ) return TRUE;
      return (BOOL)( T < Lim );
   }
   
   void updT( REAL T ) {               // conditional update from value
      if( !Act ) {                     // initial at any value
         Act = TRUE;
         Lim = T;
      }
      else if( Lim > T ) Lim = T;      // update with lower value
   }
};

