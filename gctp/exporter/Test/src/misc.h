/*********************************************************************/
/*                                                                   */
/* FILE :        misc.h                                              */
/*                                                                   */
/* DESCRIPTION : LightWave 3D Object Library 2                       */
/*                                                                   */
/* HISTORY :     Jan 01, 2000    written by Yoshiaki Tazaki          */
/*               Nov 13, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/

#ifndef _MISC_H
#define _MISC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <io.h>
#endif
#ifdef _XGL
#include <unistd.h>
#endif
#include "objdef.h"


/*
 *  File Pathname Conversion
 */
typedef int      PathConvOperator;
#define PATHCONV_PATHCONV        1
#define PATHCONV_SPLITPATH       2
#define PATHCONV_BUILDPATH       3
#define PATHCONV_REPLACEXT       4
#define PATHCONV_GETBODY         5
#define PATHCONV_GETDIRECTORY    6
#define PATHCONV_GETEXT          7
#define PATHCONV_REL2FULL        8
#define PATHCONV_FULL2REL        9
#define PATHCONV_UPDIRECT       10
#define PATHCONV_FILE2NEUT      11
#define PATHCONV_NEUT2FILE      12
#define PATHCONV_FILEEXIST      13

typedef int        PathConvResult;
#define PATHCONV_NO_ERROR        0
#define PATHCONV_PARAM_ERROR     1
#define PATHCONV_FILENOTFOUND    2

typedef struct st_PathConvParam {
	char                *directory;
	char                *name;
	char                *ext;
	char                *path_in;
	char                *path_out;
	PathConvResult       result;
} PathConvParam;

#ifdef _LITTLE_ENDIAN
#define MSB2			_SwapTwoBytes
#define MSB4			_SwapFourBytes 
#define LSB2(w)			(w)
#define LSB4(w)			(w)
#endif
#ifdef  _BIG_ENDIAN
#define MSB2(w)			(w)
#define MSB4(w)			(w)
#define LSB2			_SwapTwoBytes
#define LSB4			_SwapFourBytes
#endif

#ifdef _WIN32
#define SEPA			'\\'
#endif
#ifdef  _MACOS
#define SEPA			':'
#endif
#ifdef  _XGL
#define SEPA			'/'
#endif

#define ISALPHABET(c)    ( ((c)>=0x41&&(c)<=0x5A) || ((c)>=0x61&&(c)<=0x7A) )
#define ISDIGIT(c)       ((c)>=0x30&&(c)<=0x39)

extern U2      _SwapTwoBytes( U2 w );
extern U4      _SwapFourBytes( U4 w );
extern int     PathConvert( PathConvOperator ope, PathConvParam *param );
extern int     STR_cmp( char *a, char *b );
extern int     STR_ncmp( char *a, char *b, int n );

#ifdef __cplusplus
}
#endif

#endif /* _MISC_H  */

