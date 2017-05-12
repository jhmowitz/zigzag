// key_def.h

#ifndef _KEY_DEF_H
#define _KEY_DEF_H

// ! keys marked (&) are handled only before processing started
// ! keys marked (*) are handled only, if tesselation throw is initiated


#ifdef _STT2MAIN_CPP      // -- parsing before processing
   // (&) toggle: circles connected to triangles and v.v.
   #define   CROSS_OVER      '_'       // underscore, circles to triangles coupled or not
   // (&) change propability for extra emergence at "loss of complexity"
   #define   EXTRA_EMERG     '$'     // toggle: extra emergence 0 to 1
   #define   INCR_EMERG      'Z'     // increase
   #define   DECR_EMERG      'z'     // decrease
   #define   BITD_EMERG      -44     // little decrease (alt_z)
#endif                           // _STT2MAIN_CPP


#ifdef _TSS_THR_CPP       // -- tesselation or random throw
   // throw a (pertubated) regular tesselation
   #define   TESS_THROW      '.'     // initiate

   #define   TESS_SUBMIT     ':'     // (*) submit
   #define   TESS_ABORT       27     // (*) escape

   // frame and subframe for tesselation(s)
   #define   FRAME_TYPE      'f'     // (*) change type of frame
   #define   FRAME_SPLT      '/'     // (*) toggle: single or split frame
   #define   FRAME_SWITCH    -33     // (*) switch current frame (alt_f) inside/outside

   // rotate frame 3 (or about 0.6) degrees
   #define   FRAME_ROT_CW    'Q'     // (*) clockwise
   #define   FRAME_ROT_ACW   'q'     // (*) anticlockwise
   #define   FRAME_ROT_BIT   -16     // (*) a bit anticlockwise (alt_q)

   // change radius of frame
   #define   INCR_FRAME      'L'     // (*) increase
   #define   DECR_FRAME      'l'     // (*) decrease
   #define   BITD_FRAME      -38     // (*) little decrease (alt_l)
   
   // change format of [rectangular] frame
   #define   FRAME_LONGER    'A'     // (*) increase
   #define   FRAME_SHRTER    'a'     // (*) decrease
   #define   FRAME_BTSHRT    -30     // (*) little decrease (alt_a)
   
   // change radius of subframe
   #define   INCR_SUBFRAME   'O'     // (*) increase
   #define   DECR_SUBFRAME   'o'     // (*) decrease
   #define   BITD_SUBFRAME   -24     // (*) little decrease (alt_o)
   
   #define   WIDEN_RING      'H'     // (*) widen halo between frames
   #define   NARROW_RING     'h'     // (*) narrow halo between frames

   #define   THROW_TYPE      ','     // (*) change type of throw objects

   // handle tesselation(s)
   #define   TESS_TYPE       't'     // (*) change tesselation type

   #define   INCR_DENSITY    'D'     // (*) increase density of points
   #define   DECR_DENSITY    'd'     // (*) decrease density of points

   #define   INCR_OMITT      'E'     // (*) increase number of omitted points
   #define   DECR_OMITT      'e'     // (*) decrease number of omitted points

   // rotate tesselation 3 (or about 0.7) degrees
   #define   TESS_ROT_CW     'R'     // (*) clockwise
   #define   TESS_ROT_ACW    'r'     // (*) anticlockwise
   #define   TESS_ROT_BIT    -19     // (*) a bit anticlockwise (alt_r)

   // factor of point pertubation within a tesselation
   #define   INCR_PERTFAC    '+'     // (*) increase pertubation
   #define   DECR_PERTFAC    '-'     // (*) decrease pertubation

   #define   INCR_RANDOM     '>'     // (*) increase start of random generator
   #define   DECR_RANDOM     '<'     // (*) decrease start of random generator

   // throw a random configuration
   #define   THR_RDM___1     '1'     // create one circle/triangle
   #define   THR_RDM___2     '2'     // create two circles/triangles
   #define   THR_RDM___5     '3'     // .. or 5
   #define   THR_RDM__10     '4'     // .. or 10
   #define   THR_RDM__20     '5'     // .. or 20
   #define   THR_RDM__50     '6'     // .. or 50
   #define   THR_RDM_100     '7'     // .. or 100
   #define   THR_RDM_200     '8'     // .. or 200
   #define   THR_RDM_500     '9'     // .. or 500

   #define   THROW_WHICH     '*'     // toggle: circles/triangles throw
   #define   OPEN_FOLDED     '^'     // toggle: tame/possibly folded triangles

   // polygons other than triangles (open type only)
   #define   DECR_CORNER     '('     // decrease number of corners
   #define   INCR_CORNER     ')'     // increase number of corners

   #define   INCR_RANDOM     '>'     // increase start of random generator
   #define   DECR_RANDOM     '<'     // decrease start of random generator
   #define   REST_RANDOM     '`'     // refresh only: start random numbers

   // throw data from existing file circles or/and triangles
   #define   THR_FILE        ';'     // throw file data
   #define   FIL_CANCEL       27     // cancel file data throw (esc)
#endif                            // _TSS_THR_CPP 


#ifdef _VIEWGRPH_CPP              // -- graphics: screen and view

#ifdef __MAKE_PSF__                    // PostScript prints
   // -- layout of multiple pages:
   #define   PS_CUTLINES     -38         // toggle3: draw cut lines  (alt_l)

   // -- layout of page:
   #define   PS_DATE         -32         // toggle: draw date  (alt_d)
   #define   PS_MARK_PAGE    -50         // mark page corner left/right (alt_m)
   #define   PS_SPREADING    -43         // vary margins between pictures (alt_\)

   // -- cadre: ten steps from portrait to landscape
   #define   PS_CADRE_LSCP   'c'        // landscape (+1.0)
   #define   PS_CADRE_PORT   'C'        // portrait  (-1.0)
   #define   PS_CADRE_P2L    -46        // from portrait to landscape (alt_c)

   // -- layout of picture:
   #define   PS_FRAME        -33        // toggle: draw frame (alt_f)
   #define   PS_TITLE        -20        // toggle: draw title (alt_t)

   #define   PS_BK_WHITE     'b'        // background is WHITE (1.0)
   #define   PS_BK_GRAY      'B'        // background is gray  (0.8)
   #define   PS_BK_DARKER    -48        // background is darkening (0.02) (alt_b)
   
   #define   PS_PAGETHICK    'T'        // 3 possible page thicknesses
   #define   PS_PICTTHICK    't'        // 4 possible picture thicknesses

   // -- printing of page or picture:
   #define   PS_PICTURE      'p'        // print next picture (even whole page)
   #define   PS_DIVIDED      'P'        // divide page into several pictures
   #define   PS_PLACE_I      'i'        // print number i to be next
   #define   PS_SEQUENCE_I   'I'        // print number i to be next in sequence
   #define   PS_FINISH       -25        // print page to be finished (alt_p)
   #define   PS_CANCEL        27        // cancel divided print (esc)

   // -- printing a series of contours with a fixed timestep:
   #define   PS_FIXSTEP_1   -133        // initial, finish the series (F11) on one file
   #define   PS_FIXSTEP_M   -134        // initial, finish the series (F12) on several files

   #define   PS_MLTPAGE      'm'        // print picture of multiple pages
   #define   PS_MLTPAGEF     'M'        // print picture of multiple pages, var. file names

   #define   PS_ALTER_LPT    '['        // alter print queue for .eps file
   #define   PS_SHOW_LPT     ']'        // show print queue for .eps file

   #define   PS_INCR_DIN     -92        // increase DIN number of paper size (Shft_F9)
   #define   PS_DECR_DIN     -67        // decrease DIN number of paper size (F9)

   #define   Q_MARK          '?'        // question mark to interrupt file name reading
#endif                               // __MAKE_PSF__

                                     // screendraw
   #define   INIT_VIEW       '0'          // init/reset view

   #define   FAST_ZOOM_IN    'S'          // zoom in
   #define   FAST_ZOOM_OUT   's'          // zoom out
   #define   SLOW_ZOOM_OUT   -31          // zoom out slowly (alt_s)

   #define   BIG_STEP_RIGHT  'X'          // step right
   #define   BIG_STEP_LEFT   'x'          // step left
   #define   SML_STEP_LEFT   -45          // step left slightly (alt_x)

   #define   BIG_STEP_UP     'Y'          // step up
   #define   BIG_STEP_DOWN   'y'          // step down
   #define   SML_STEP_DOWN   -21          // step down slightly (alt_y)

   #define   COLOR_GRAY      'k'          // toggle: color or gray
   #define   COLOR_EXCITE    'K'          // toggle: color excited or normal

   #define   DRAW_MODE       -50          // toggle: draw contours or bisectors (alt_m)

   #define   DRAW_GENERA     -34          // toggle: draw all/one generation(s) (alt_g)
   #define   NEXT_GENERA     'g'          // draw next generation
   #define   PREV_GENERA     'G'          // draw previous generation

   #define   TOG_SING_POLY   -44          // toggle: draw single polygon or not (alt_z)
   #define   NXT_SING_POLY   'Z'          // next polygon is single
   #define   PRV_SING_POLY   'z'          // previous polygon is single

   #define   DRAW_CADRE      -53          // toggle: draw cadre (alt_/)
   #define   DRAW_WHICH      'W'          // draw circles, triangles or both

   #define   SCRN_REPAINT    'R'          // toggle: screen repaint
   #define   ONCE_REPAINT    'r'          // repaint screen once

   #define   INCR_DELAY      'D'          // increase delay
   #define   DECR_DELAY      'd'          // decrease delay
   #define   SHOW_DELAY      '~'          // show number of pictures per 10 seconds

   // -- (screen)views (0, .. N_VIEW-1) to handle at key stroke
   #define   ACTIVE_VIEW     -59          // activate   (F1)
   #define   DELETE_VIEW     -84          // delete     (shift_F1)
   #define   INITAL_VIEW    -104          // initialize (alt_F1)
#endif                                 // _VIEWGRPH_CPP 


#ifdef _STT2MAIN_CPP           // -- processing, flow and test mode
   
   // -- processing
   #define   INCR_STEP       '+'          // increase process time step
   #define   DECR_STEP       '-'          // decrease process time step
   #define   PAUSE_GO        '!'          // toggle: process pause/go
   #define   PROG_QUIT        27          // quit computing (esc)

   // -- flow and test mode
   #define   TEST_MODE       '|'          // toggle: test mode
   #define   MORE_FLOW       'F'          // activate more flow
   #define   LESS_FLOW       'f'          // deactivate flow
   #define   MORE_SECS       '}'          // increase number of delay seconds 
   #define   LESS_SECS       '{'          // decrease number of delay seconds 
   #define   IND_EVENT       -18          // indicate events (alt_e)

   #define   COUNT_CLASS     '&'          // count all production classes
   #define   SHOW_TABLE      '#'          // show table of events, class counts
#endif                                // _STT2MAIN_CPP

#endif                           // _KEY_DEF_H 
