/*********************************************************************/
/*                                                                   */
/* FILE :        dxffp.cpp                                           */
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

#define SWAP(a,b)	{ a^=b; b^=a; a^=b; }
#ifdef _BIG_ENDIAN
#define	BSWAP_W(w)	SWAP( (((U1 *)&w)[0]), (((U1 *)&w)[1]) )
#define	BSWAP_L(l)	{ BSWAP_W( (((U2 *)&l)[0])); BSWAP_W( (((U2 *)&l)[1])); SWAP( (((U2 *)&l)[0]), (((U2 *)&l)[1]) ) }
#endif
#ifdef _LITTLE_ENDIAN
#define	BSWAP_W(w)	
#define	BSWAP_L(l)
#endif


#ifndef GCTP_USE_XFILEAPI
/*
 *
 *  ffp   - print the strings with indents
 *
 */
void dxExporter::ffp( int tabs, char *buf, ... )
{
	static char  buffer[4096];
	va_list      arglist;


	if (_fp)
	{
		while(tabs--){ fputc(' ',_fp); }
		va_start (arglist, buf);
		vsprintf (buffer, buf, arglist);
		va_end (arglist);
		fprintf (_fp, buffer);
	}
}



int dxExporter::ffp_bytes( DX_BYTE *vals, int num )
{
	if (_fp)
	{
		if (fwrite( vals, sizeof(DX_BYTE), num, _fp ) != (size_t)num) {
			return 0;
		}
	}

	return num;
}

int dxExporter::ffp_words( DX_WORD *vals, int num )
{
	DX_WORD     data[1];
	int         i;

	for (i = 0; i < num; i++) 
	{
		data[0] = vals[i]; BSWAP_W(data[0]);
		if (fwrite( data, sizeof(DX_WORD), 1, _fp ) != 1) {
			return 0;
		}
	}

	return num;
}

int dxExporter::ffp_dwords( DX_DWORD *vals, int num )
{
	DX_DWORD    data[1];
	int         i;

	for (i = 0; i < num; i++) 
	{
		data[0] = vals[i]; BSWAP_L(data[0]);
		if (fwrite( data, sizeof(DX_DWORD), 1, _fp ) != 1) {
			return 0;
		}
	}

	return num;
}

int dxExporter::ffp_floats( DX_FLOAT *vals, int num )
{
	return ffp_dwords( (DX_DWORD *) vals, num );
}

int dxExporter::ffp_word( DX_WORD vals )
{
	ffp_words( &vals, 1 );

	return 1;
}

int dxExporter::ffp_dword( DX_DWORD vals )
{
	ffp_dwords( &vals, 1 );

	return 1;
}

int dxExporter::ffp_float( DX_FLOAT vals )
{
	ffp_floats( &vals, 1 );

	return 1;
}


void dxExporter::ffp_TOKEN( int token )
{
	DX_WORD    vals[1];

	vals[0] = (DX_WORD) token;
	ffp_words( vals, 1 );
}


void dxExporter::ffp_NAME( char *name )
{
	DX_DWORD    vals[1];

	ffp_TOKEN ( TOKEN_NAME );
	vals[0] = (DX_DWORD) strlen(name);
	ffp_dwords( vals, 1 );
	ffp_bytes ( (unsigned char *) name, strlen(name) );
}


void dxExporter::ffp_STRING( char *string, int token )
{
	DX_DWORD    vals[1];

	ffp_TOKEN ( TOKEN_STRING );
	vals[0] = (DX_DWORD) strlen(string);
	ffp_dwords( vals, 1 );
	ffp_bytes ( (unsigned char *) string, strlen(string) );
	ffp_TOKEN ( token );
}


void dxExporter::ffp_INTEGER_LIST( DX_DWORD *list, int num )
{
	DX_DWORD    vals[1];

	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	vals[0] = (DX_DWORD) num;
	ffp_dwords( vals, 1 );
	ffp_dwords( list, num );
}


void dxExporter::ffp_FLOAT_LIST( DX_FLOAT *list, int num )
{
	DX_DWORD    vals[1];

	ffp_TOKEN ( TOKEN_FLOAT_LIST );
	vals[0] = (DX_DWORD) num;
	ffp_dwords( vals, 1 );
	ffp_dwords( (DX_DWORD *)list, num );
}

#endif
