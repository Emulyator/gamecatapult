/*********************************************************************/
/*                                                                   */
/* FILE :        dxtyp.h                                             */
/*                                                                   */
/* DESCRIPTION : LightWave 3D to DirectX Export                      */
/*                                                                   */
/* HISTORY :     Oct 29, 2000    written by Yoshiaki Tazaki          */
/*               Oct 02, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000, D-STORM, Inc.                                 */
/*                                                                   */
/*********************************************************************/

#ifndef _DXTYP_H_
#define _DXTYP_H_


typedef unsigned int              DX_DWORD;
typedef unsigned short            DX_WORD;
typedef unsigned char             DX_BYTE;
typedef int                       DX_INT;
typedef float                     DX_FLOAT;
typedef double                    DX_DOUBLE;
typedef char                      DX_STRING[255];
typedef char                      DX_FNAM[255];
typedef int                       DX_BOOL;

#define TOKEN_NAME 				1
#define TOKEN_STRING 			2
#define TOKEN_INTEGER 			3
#define TOKEN_GUID 				5
#define TOKEN_INTEGER_LIST 		6
#define TOKEN_FLOAT_LIST 		7

#define TOKEN_OBRACE 			10
#define TOKEN_CBRACE 			11
#define TOKEN_OPAREN 			12
#define TOKEN_CPAREN 			13
#define TOKEN_OBRACKET 			14
#define TOKEN_CBRACKET 			15
#define TOKEN_OANGLE 			16
#define TOKEN_CANGLE 			17
#define TOKEN_DOT 				18
#define TOKEN_COMMA 			19
#define TOKEN_SEMICOLON 		20
#define TOKEN_TEMPLATE 			31
#define TOKEN_WORD 				40
#define TOKEN_DWORD 			41
#define TOKEN_FLOAT 			42
#define TOKEN_DOUBLE 			43
#define TOKEN_CHAR 				44
#define TOKEN_UCHAR 			45
#define TOKEN_SWORD 			46
#define TOKEN_SDWORD 			47
#define TOKEN_VOID 				48
#define TOKEN_LPSTR 			49
#define TOKEN_UNICODE 			50
#define TOKEN_CSTRING 			51
#define TOKEN_ARRAY 			52



typedef struct st_DX_Vector {

	DX_FLOAT                      x;
	DX_FLOAT                      y;
	DX_FLOAT                      z;

} DX_Vector;

typedef struct st_DX_Coord2d {

	DX_FLOAT                      u;
	DX_FLOAT                      v;

} DX_Coord2d;

typedef struct st_DX_Quaternion {

	DX_FLOAT                      s;
	DX_Vector                     v;

} DX_Quaternion;

typedef struct st_DX_Matrix4x4 {

	DX_FLOAT                      matrix[16];

} DX_Matrix4x4;

typedef struct st_DX_ColorRGBA {

	DX_FLOAT                      red;
	DX_FLOAT                      green;
	DX_FLOAT                      blue;
	DX_FLOAT                      alpha;

} DX_ColorRGBA;

typedef struct st_DX_ColorRGB {

	DX_FLOAT                      red;
	DX_FLOAT                      green;
	DX_FLOAT                      blue;

} DX_ColorRGB;

typedef struct st_DX_IndexColor {

	DX_DWORD                      index;
	DX_ColorRGBA                  indexColor;

} DX_IndexColor;

typedef struct st_DX_Boolean {

	DX_DWORD                      truefalse;

} DX_Boolean;

typedef struct st_DX_Boolean2d {

	DX_Boolean                    u;
	DX_Boolean                    v;

} DX_Boolean2d;



#endif /* _DXTYP_H_  */

