/** @file Defs.h
 *  @brief General preprocessor directives
 *
 *  @details This contains general preprocessor directives used for
 *  better readability of code and for keeping a standard of terms.
 */

#ifndef DEFS_HPP
#define DEFS_HPP

//defines
#define no_argument 0
#define required_argument 1 
#define optional_argument 2

#define TRUE 1
#define FALSE 0

//#define YES 1
//#define NO 0

//Corner defines
#define TOP_LEFT 0
#define TOP_RIGHT 1
#define BOTTOM_LEFT 2
#define BOTTOM_RIGHT 3

//Sync modes
#define TIMECODE_MODE 1
#define MANUAL_MODE 2
#define GRAPHIC_MODE 3

#define DEBUG_PANO false

//Stitch modes
#define STITCH_FULL 1
#define STITCH_SURF1 2
#define STITCH_SURF2 3
#define STITCH_MJ 4
#define STITCH_GEORS 5
#define STITCH_GEO 6
#define STITCH_GEOP 7

//Point selection mode
#define NO_VIDEO_PMODE 0
#define SINGLE_VIDEO_PMODE 1
#define FULL_VIDEO_PMODE 2

#define J_PI 3.141592653589793238462643383279502884L /* pi */

//Development debug macros
#ifdef DEBUG
#  define D(x) x
#else
#  define D(x)
#endif
#ifdef VDEBUG
#  define VD(x) x
#else
#  define VD(x)
#endif

#define PMIN(a, b) ((a < b) ? a : b)
#define PMAX(a, b) ((a > b) ? a : b)

#endif
