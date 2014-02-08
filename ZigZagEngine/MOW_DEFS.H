/*
 * mow_defs.h  -- must be copied to specific appplication directory
 */

#ifdef WIN32
#define far /* not used */
#define farmalloc malloc

#define FOPEN     DoFopen
#define FGETS     _fgetts
#define STRCHR    _tcschr
#define ATOF      _tstof
#define STRNCMP   _tcsnccmp
// #define GETS      _getts --> see doscompat.h
#define STRLEN    _tcsclen
#define VSPRINTF  _vstprintf
#define FPRINTF   _ftprintf
#define SPRINTF   _stprintf
#define STRCPY	  _tcscpy
#define STRNCPY		_tcsncpy

// #define PRINTF    _tprintf --> Zie doscompat.h

#define _atold(x) _tcstod(x, 0)

#ifndef __POW10__
#define __POW10__

#define M_LN10      2.30258509299404568402

// extern double exp(double);
#include <math.h>
inline double pow10(double x)
{
  return exp(x*M_LN10);
}
#endif

#include <tchar.h>
#endif

// -- general definitions of types and constants
// #define __LONGREAL__                    // program uses --> long double (instead of double)

// #include "mow_util\\gen_def.h"        // ! early version
#include "mow_util\mow_type.h"       // ! later version

// -- flags to include/exclude sources present in the application into/from the compiler
#define __MAKE_PSF__                // program is enabled to make PS-file

// -------------------------------------------------> PostScript-image representation
#define  PS_MASK_H       "mow_util\ps_file\psf_mask.h"  // definitions of masking pattern
#define  PS_FILE_H       "mow_util\ps_file\psf_def.h"   // definitions only
#define  PS_FILE_CPP     "mow_util\ps_file\psf_cpp.h"   // source (includes definitions)

// make_psf.h for zigzagPS and zgzgPS_D
#define  PRC_MAKE_PSF_H  "mow_util\make_psf\make_psf.h" // "p_mk_psf.h" 

// -------------------------------------------------> general geometry
/* definition of point, vector, circle, ellips, line and Simpson line
 * sources on alg_geo.cpp are added by compiler insert !
 */
#define  ALG_GEO_H       "mow_util\alg_geo\alg_geo.h"   // definitions only

// -------------------------------------------------> cadre and clipping
/*
 * -- handle a general cadre: class definition and inline functions only!
 *    ! included by: --> #define CIR_CLIP_H
 *                   --> #define LIN_CLIP_H
 *                   --> #define HAL_CLIP_H
 */
#define  MOW_CADRE_H     "mow_util\clip\cadre.h"

/*
 * -- clip a circle or arc against a cadre (with or without a halo)
 *    ! included by: --> LIN_CLIP_H         (!includes sources)
 *                   --> HAL_CLIP_H         (!definitions only)
 *    include sources by --> #define _MOW_CLIP_CPP
 */
#define  CIR_CLIP_H      "mow_util\clip\cir_clip.h"

/*
 * -- clip a point or line against a cadre
 *    include sources by --> #define _MOW_CLIP_CPP
 */
#define  LIN_CLIP_H      "mow_util\clip\lin_clip.h"

/*
 * -- clip a point or line against a cadre enlarged by a halo
 *    include sources by --> #define _HAL_CLIP_CPP
 */
#define  HAL_CLIP_H      "mow_util\clip\hal_clip.h"

// -------------------------------------------------> linkage and memory
/*
 * -- memo handling for bulk memory without linkage
 *    ! active inclusion of <new.h>
 */
#define  MOW_BULK_H      "mow_util\memo\bulk_def.h"

/*
 * -- linkage for structures that link singly into a queue with NULL-terminating
 *    ! included by: --> MOW_LINK_CPP
 */
#define  MOW_LINK_H      "mow_util\memo\link_def.h" 

/*
 * -- memory handling for linkage structures
 *    ! include only once into applicartion
 *    ! active inclusion of <new.h>
 */
#define  MOW_LINK_CPP    "mow_util\memo\link_cpp.h" 
