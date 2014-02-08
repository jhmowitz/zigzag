/* cadre.h
 *
 * Definition of a rectangular cadre defined by four points of type PNT2,
 * the points being represented in coordinates X, Y of type REAL.
 *
 * The points are organized anticlockwise        3              2
 * starting at the lower left corner               ------------
 *                                                 |          |
 *                                                 |          |
 *                                                 |          |
 *                                                 ------------
 *                                               0              1
 *
 * The cadre can be initialized by four extreme values [and a proportion]
 * - MinX, MaxX, MinY and MaxY only                     --> inherent format
 * - together with a proportion of Y-range to X-range   --> requested format
 *   where the inherent format does not fit the requested format,
 *   the range that is too small will be enlarged, the other stays intact.
 *
 * The initialized cadre can be translated along the coordinates
 * and can be enlarged/diminished by a percentage of its actual size
 */

#ifndef _CADRE_H
#define _CADRE_H

class CADRE {
   PNT2 CPnt[4];          // 4 points to define the cadre
   PNT2 MidP;             // midpoint
   VEC2 Rnge;             // ranges
public:
                                           // initial to requested format Rat
   void InitCadre( REAL MinX, REAL MaxX, REAL MinY, REAL MaxY, REAL Rat ) {
      if( ( MaxX - MinX ) * Rat > MaxY - MinY ) {  // widen up Y-range
         CPnt[0].X = CPnt[3].X = MinX;
         CPnt[1].X = CPnt[2].X = MaxX;
         CPnt[0].Y = CPnt[1].Y = ( MinY + MaxY ) / 2 - ( MaxX - MinX ) * Rat / 2;
         CPnt[2].Y = CPnt[3].Y = ( MinY + MaxY ) / 2 + ( MaxX - MinX ) * Rat / 2;
      }
      else {                                       // widen up X-range
         CPnt[0].X = CPnt[3].X = ( MinX + MaxX ) / 2 - ( MaxY - MinY ) / Rat / 2;
         CPnt[1].X = CPnt[2].X = ( MinX + MaxX ) / 2 + ( MaxY - MinY ) / Rat / 2;
         CPnt[0].Y = CPnt[1].Y = MinY;
         CPnt[2].Y = CPnt[3].Y = MaxY;
      }
   }
                                           // initial/update to inherent format
   void UpdCadre( REAL lx, REAL hx, REAL ly, REAL hy ) {
      CPnt[0].X = CPnt[3].X = lx; CPnt[1].X = CPnt[2].X = hx;
      CPnt[0].Y = CPnt[1].Y = ly; CPnt[2].Y = CPnt[3].Y = hy;
   }

   void TranslX( REAL tx ) {               // translate X to right(tx>0) or left(tx<0)
      REAL Displace = tx * ( CPnt[2].X - CPnt[0].X );
      CPnt[0].X = ( CPnt[3].X += Displace );
      CPnt[1].X = ( CPnt[2].X += Displace );
   }

   void TranslY( REAL ty ) {               // translate Y up(ty>0) or down(ty<0)
      REAL Displace = ty * ( CPnt[2].Y - CPnt[0].Y );
      CPnt[0].Y = ( CPnt[1].Y += Displace );
      CPnt[2].Y = ( CPnt[3].Y += Displace );
   }

   void ZoomOutIn( REAL Zm ) {             // zoom out(Zm>0) or in(Zm<0)
      REAL mx = ( CPnt[0].X + CPnt[2].X ) / 2, my = ( CPnt[0].Y + CPnt[2].Y ) / 2,
           rx = ( CPnt[2].X - CPnt[0].X ) / 2, ry = ( CPnt[2].Y - CPnt[0].Y ) / 2;
      CPnt[0].X = CPnt[3].X = mx - Zm * rx;
      CPnt[1].X = CPnt[2].X = mx + Zm * rx;
      CPnt[0].Y = CPnt[1].Y = my - Zm * ry;
      CPnt[2].Y = CPnt[3].Y = my + Zm * ry;
   }

   // provide access to ..
   REAL Ratio() {                          // proportion of Y-range to X-range, format
      return ( CPnt[2].Y - CPnt[0].Y ) / ( CPnt[2].X - CPnt[0].X );
   }

   pPNT2 LLCor() { return &CPnt[0]; }      // lower left corner
   pPNT2 URCor() { return &CPnt[2]; }      // upper right corner

   pPNT2 CdrPts() { return CPnt; }         // cadre points
   pPNT2 MidPnt() {                        // midpoint of cadre
      MidP = CPnt[0] + ( CPnt[2] - CPnt[0] ) * 0.5;
      return &MidP;
   } 
   pVEC2 Ranges() {                         // range of X- and Y-coordinate
      Rnge = CPnt[2] - CPnt[0];
      return &Rnge;
   }

   REAL LowX() { return CPnt[0].X; }        // minimum X
   REAL HigX() { return CPnt[2].X; }        // maximum X
   REAL LowY() { return CPnt[0].Y; }        // minimum Y
   REAL HigY() { return CPnt[2].Y; }        // maximum Y
};      
      
#endif          // _CADRE_H
