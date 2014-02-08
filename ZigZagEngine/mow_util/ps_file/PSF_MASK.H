// psf_mask.h -- pattern code for PostScript printing for points and linelike structures
//               such as lines, circles, ellipses and curves generally

#ifndef _PSF_MASK_H
#define _PSF_MASK_H

// --------------------------------> handle PostScript masking of a point
enum MskP {                // -- pattern codes
   T_BLCK,                    // black                                 ## ! in zigzag only
   S_BLCK, M_BLCK, L_BLCK,    // black:          small, medium, large
   S_WHTE, M_WHTE,            // white:          small, medium
   S_BinW,                    // black in white: small
   S_WinB,                    // white in black: small
   T_CRSS, S_CRSS, M_CRSS,    // with a cross:   tiny, small, medium
   S_CIRC,                    // ??? :           small                 ## nalopen
   NoMsk = -1,
};

class MPNT {               // -- mask pattern for a point
public: 
   MskP  mP;                  // point mask
   MPNT( MskP m ) { mP = m; }
};

// --------------------------------> handle PostScript masking of a line
enum MskL {                // -- pattern codes
   L_NULL, L_TINY, L_SMLL, L_MEDI, L_LRGE,
};

class MLIN {               // -- mask pattern (thickness) for a linelike structure
public: 
   MskL  mL;                  // line mask
   MLIN( MskL m ) { mL = m; }
   MLIN() { mL = L_TINY; }
};

#endif                             // _PSF_MASK_H 
