/*********************************************************************/
/*                                                                   */
/* FILE :        dxutl.cpp                                           */
/*                                                                   */
/* DESCRIPTION : LightWave 3D to DirectX Export                      */
/*                                                                   */
/* HISTORY :     Oct 29, 2000    written by Yoshiaki Tazaki          */
/*               Oct 02, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/
#ifdef GCTP_USE_XFILEAPI
#include <gctp/vector.hpp>
#endif

#include "dxexp.h"
extern "C" {
#include "misc.h"
}



/*
 *
 *  DX_MakeNodeName    - make a node name
 *
 */
void DX_MakeNodeName( char* dst, char* src )
{
	while (*src) {
		if (!(*src >= 'a' && *src <= 'z') &&
			!(*src >= 'A' && *src <= 'Z') &&
			!(*src >= '0' && *src <= '9') &&
			*src != '_') {
			*dst++ = '_';
		} else {
			*dst++ = *src;
		}
		src++;
	}

	*dst = 0x00;
}


/*
 *
 *  DX_FileBodyName    - get the bodyname of the file
 *
 */
void DX_FileBodyName( char* dst, char* src )
{
	PathConvParam   param;

	param.path_in = src;
	param.name    = dst;
	PathConvert( PATHCONV_GETBODY, &param );
}


/*
 *
 *  DX_TextureName    - get the texture filename
 *
 */
void DX_TextureName( char* dst, char* src, char* ext )
{
	PathConvParam   param;
	DX_FNAM         temp;

	if (strlen(ext) > 0) {
		param.path_in  = src;
		param.name     = temp;
		PathConvert( PATHCONV_GETBODY, &param );
		param.path_in  = temp;
		param.ext      = ext;
		param.path_out = dst;
		PathConvert( PATHCONV_REPLACEXT, &param );
	} else {
		param.path_in = src;
		param.name    = dst;
		PathConvert( PATHCONV_GETBODY, &param );
	}
}


/*
 *
 *  DX_FileUniqName    - get the bodyname without extension
 *
 */
void DX_FileUniqName( char* dst, char* src )
{
	PathConvParam   param;
	char*           ext;

	param.path_in = src;
	param.name    = dst;
	PathConvert( PATHCONV_GETBODY, &param );
	if ((ext = strrchr(dst, '.'))) {
		*ext = 0x00;
	}
}

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdarg.h>
/*
 *
 *  DX_FileUniqName    - get the bodyname without extension
 *
 */
void dbgprintf( const char* format, ... )
{
	static char buf[1024];
	va_list arg;
	va_start( arg, format );
	vsprintf(buf, format, arg);
	va_end( arg );
	OutputDebugString(buf);
}
