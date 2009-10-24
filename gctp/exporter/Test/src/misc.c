/*********************************************************************/
/*                                                                   */
/* FILE :        misc.c                                              */
/*                                                                   */
/* DESCRIPTION : MISC UTILITY FUNCTIONS.                             */
/*                                                                   */
/* HISTORY :     Jan 01, 2000    written by Yoshiaki Tazaki          */
/*               Nov 13, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/

#include "objdef.h"
#include "misc.h"


/***********  PROTOTYPES  **********/

static PathConvResult pc_path_conv    ( char *path_in, char *path_out );
static PathConvResult pc_split_path   ( char *path_in, char *directory, char *name );
static PathConvResult pc_build_path   ( char *directory, char *name, char *path_out );
static PathConvResult pc_replace_ext  ( char *path_in, char *ext, char *path_out );
static PathConvResult pc_get_body     ( char *path_in, char *name );
static PathConvResult pc_get_directory( char *path_in, char *directory );
static PathConvResult pc_get_ext      ( char *path_in, char *ext );
static PathConvResult pc_rel2full     ( char *path_in, char *directory, char *path_out );
static PathConvResult pc_full2rel     ( char *path_in, char *directory, char *path_out );
static PathConvResult pc_up_directory ( char *path_in, char *directory );
static PathConvResult pc_file2neut    ( char *path_in, char *path_out );
static PathConvResult pc_neut2file    ( char *path_in, char *path_out );
static PathConvResult pc_file_exist   ( char *path_in );


/*
 *  Swap byte order for LITTLE_ENDIAN
 */
U2 _SwapTwoBytes (U2 w)
{
	U2 tmp;
	tmp =  (w & 0x00ff);
	tmp = ((w & 0xff00) >> 0x08) | (tmp << 0x08);
	return tmp;
}

U4 _SwapFourBytes (U4 w)
{
	U4 tmp;
	tmp =  (w & 0x000000ff);
	tmp = ((w & 0x0000ff00) >> 0x08) | (tmp << 0x08);
	tmp = ((w & 0x00ff0000) >> 0x10) | (tmp << 0x08);
	tmp = ((w & 0xff000000) >> 0x18) | (tmp << 0x08);
	return tmp;
}


/*
 *  File Pathname Converter
 */

int PathConvert( PathConvOperator ope, PathConvParam *param )
{
	param->result = PATHCONV_NO_ERROR;

	switch (ope) {
	case PATHCONV_PATHCONV:
	  if (!param->path_in || !param->path_out) goto PARAM_ERROR;
	  param->result = pc_path_conv( param->path_in, param->path_out );
	  break;
	case PATHCONV_SPLITPATH:
	  if (!param->path_in || !param->directory || !param->name) goto PARAM_ERROR;
	  param->result = pc_split_path( param->path_in, param->directory, param->name );
	  break;
	case PATHCONV_BUILDPATH:
	  if (!param->path_out || !param->directory || !param->name) goto PARAM_ERROR;
	  param->result = pc_build_path( param->directory, param->name, param->path_out );
	  break;
	case PATHCONV_REPLACEXT:
	  if (!param->path_in || !param->path_out || !param->ext) goto PARAM_ERROR;
	  param->result = pc_replace_ext( param->path_in, param->ext, param->path_out );
	  break;
	case PATHCONV_GETBODY:
	  if (!param->path_in || !param->name) goto PARAM_ERROR;
	  param->result = pc_get_body( param->path_in, param->name );
	  break;
	case PATHCONV_GETDIRECTORY:
	  if (!param->path_in || !param->directory) goto PARAM_ERROR;
	  param->result = pc_get_directory( param->path_in, param->directory );
	  break;
	case PATHCONV_GETEXT:
	  if (!param->path_in || !param->ext) goto PARAM_ERROR;
	  param->result = pc_get_ext( param->path_in, param->ext );
	  break;
	case PATHCONV_REL2FULL:
	  if (!param->path_in || !param->directory || !param->path_out) goto PARAM_ERROR;
	  param->result = pc_rel2full( param->path_in, param->directory, param->path_out );
	  break;
	case PATHCONV_FULL2REL:
	  if (!param->path_in || !param->directory || !param->path_out) goto PARAM_ERROR;
	  param->result = pc_full2rel( param->path_in, param->directory, param->path_out );
	  break;
	case PATHCONV_UPDIRECT:
	  if (!param->path_in || !param->directory) goto PARAM_ERROR;
	  param->result = pc_up_directory( param->path_in, param->directory );
	  break;
	case PATHCONV_FILE2NEUT:
	  if (!param->path_in || !param->path_out) goto PARAM_ERROR;
	  param->result = pc_file2neut( param->path_in, param->path_out );
	  break;
	case PATHCONV_NEUT2FILE:
	  if (!param->path_in || !param->path_out) goto PARAM_ERROR;
	  param->result = pc_neut2file( param->path_in, param->path_out );
	  break;
	case PATHCONV_FILEEXIST:
	  if (!param->path_in) goto PARAM_ERROR;
	  param->result = pc_file_exist( param->path_in );
	  break;
	}

	return (param->result == PATHCONV_NO_ERROR) ? TRUE : FALSE;

PARAM_ERROR:
	param->result = PATHCONV_PARAM_ERROR;
	return FALSE;
}


/***** CONVER FILENAME PATH *****/

static PathConvResult pc_path_conv( char *path_in, char *path_out )
{
	char  *u, *d, *m, *p, *q;

	u = strchr( path_in, '/' );
	m = strchr( path_in, ':' );
	d = strchr( path_in, '\\' );
	p = path_in;
	q = path_out;

#ifdef _XGL
	if (m && !d) {
		p = (path_in[0] == ':') ? (m+1) : m;
		while (*p) { 
			if (*p == ':') *q = '/';
			else           *q = *p;
			p++; q++;
		}
		*q = '\0';
	}
	else if (d) {
		if (path_in[1] == ':' && path_in[0] >= 0x41 && path_in[0] <= 0x7a) p = &(path_in[2]);
		while (*p) { 
			if (*p == '\\') *q = '/';
			else            *q = *p;
			p++; q++;
		}
		*q = '\0';
	}
	else {
		strcpy( path_out, path_in );
	}
#endif

#ifdef _MACOS
	if (u) {
		if (path_in[0] != '/') *q++ = ':';
		while (*p) { 
			if (*p == '/') *q = ':';
			else           *q = *p;
			p++; q++;
		}
		*q = '\0';
	}
	else if (d) {
		if (path_in[1] == ':' && path_in[0] >= 0x41 && path_in[0] <= 0x7a) p = &(path_in[2]);
		if (*p != '\\') *q++ = ':';
		while (*p) { 
			if (*p == '\\') *q = ':';
			else            *q = *p;
			p++; q++;
		}
		*q = '\0';
	}
	else {
		strcpy( path_out, path_in );
	}
#endif

#ifdef _WIN32
	if (u) {
		while (*p) { 
			if (*p == '/') *q = '\\';
			else           *q = *p;
			p++; q++;
		}
		*q = '\0';
	}
	else if (m && !d) {
		p = (path_in[0] == ':') ? (m+1) : m;
		while (*p) { 
			if (*p == ':') *q = '\\';
			else           *q = *p;
			p++; q++;
		}
		*q = '\0';
	}
	else {
		strcpy( path_out, path_in );
	}
#endif
	return PATHCONV_NO_ERROR;
}


/***** SPLIT FILENAME *****/

static PathConvResult pc_split_path( char *path_in, char *directory, char *name )
{
	pc_get_directory( path_in, directory );
	pc_get_body( path_in, name );
	return PATHCONV_NO_ERROR;
}


/***** BUILD FILENAME *****/

static PathConvResult pc_build_path( char *directory, char *name, char *path_out )
{
	char   *p;

	strcpy( path_out, directory );
	p = path_out + strlen(path_out) - 1;
	if (p[0] != SEPA && name[0] != SEPA) {
		p[1] = SEPA; p[2] = '\0';
	}
	if (name[0] == SEPA) name++;
	strcat( path_out, name );

	return PATHCONV_NO_ERROR;
}


/***** REPLACE EXTENSION OF FILENAME *****/

static PathConvResult pc_replace_ext( char *path_in, char *ext, char *path_out )
{
	char  *p;

	strcpy( path_out, path_in );
	p = strrchr( path_out, '.' );
	if (p) strcpy( p, ext );
	else   strcat( path_out, ext );

	return PATHCONV_NO_ERROR;
}


/***** GET BODY NAME OF THE FILENAME *****/

static PathConvResult pc_get_body( char *path_in, char *name )
{
	char  *p;

	p = strrchr( path_in, SEPA );
	strcpy( name, p ? (p+1) : path_in );

	return PATHCONV_NO_ERROR;
}


/***** GET DIRECTORY NAME FROM FILENAME *****/

static PathConvResult pc_get_directory( char *path_in, char *directory )
{
	char  *p;

	strcpy( directory, path_in );

	if (p = strrchr( directory, SEPA )) {
		*p = '\0';
	} else {
	    directory[0] = '\0';
	}

	return PATHCONV_NO_ERROR;
}


/***** GET EXTENSION OF FILENAME *****/

static PathConvResult pc_get_ext( char *path_in, char *ext )
{
	char  *p;

	p = strrchr( path_in, '.' );
	strcpy( ext, p ? p : "" );

	return PATHCONV_NO_ERROR;
}


/***** RELATIVE PATH TO FULL PATH *****/

static PathConvResult pc_rel2full( char *path_in, char *directory, char *path_out )
{
	pc_build_path( directory, path_in, path_out );

	return PATHCONV_NO_ERROR;
}


/***** RELATIVE FULL TO RELATIVE PATH *****/

static PathConvResult pc_full2rel( char *path_in, char *directory, char *path_out )
{
	char   *p;

	if (STR_ncmp( path_in, directory, strlen(directory) ) == 0) {
		p = path_in + strlen(directory);
		if (*p == SEPA) p++;
		strcpy( path_out, p );
	} else {
		strcpy( path_out, path_in );
	}

	return PATHCONV_NO_ERROR;
}


/***** UP DIRECTORY *****/

static PathConvResult pc_up_directory( char *path_in, char *directory )
{
	char   *p;

	strcpy( directory, path_in );
	p = strrchr( directory, SEPA );
	if (p == (directory + strlen(directory) -1)) {
		*p = '\0';
		p = strrchr( directory, SEPA );
		if (p == NULL) {
			strcpy( directory, path_in );
		}
	}
	if (p) *p = '\0';

	return PATHCONV_NO_ERROR;
}


/*******  MAKE NEUTORAL FORMAT FILENAME  ********/

static PathConvResult pc_file2neut( char *path_in, char *path_out )
{
	char	*fi = path_in;
	char	*ne = path_out;

#ifdef _WIN32
	if (ISALPHABET(fi[0]) && fi[1] == ':') {
		*ne++ = *fi++;
		*ne++ = *fi++;
		if (*fi == '\\') fi++;
	}
	while (*fi) {
		*ne++ =  (*fi == '\\') ? '/' : *fi;
		fi++;
	}
	*ne = '\0';
#endif
#ifdef _XGL
	if (*fi == '/') {
		fi++;
		while (*fi != '/') {
			*ne++ = *fi++;
		}
		*ne++ = ':';
		fi++;
	}
	while (*fi) {
		*ne++ = *fi++;
	}
	*ne = '\0';
#endif
#ifdef _MACOS
	if (*fi == ':') {
		fi++;
		while (*fi) {
			if (*fi == ':') {
				*ne++ = '/';
			} else {
				*ne++ = *fi;
			}
			fi++;
		}
	} else {
		while (*fi != ':') {
			*ne++ = *fi++;
		}
		*ne ++ = *fi++;
		while (*fi) {
			if (*fi == ':') {
				*ne++ = '/';
			} else {
				*ne++ = *fi;
			}
			fi++;
		}
	}
	*ne = '\0';
#endif

	return PATHCONV_NO_ERROR;
}


/*******  MAKE PLATFORM FORMAT FILENAME  ********/

static PathConvResult pc_neut2file( char *path_in, char *path_out )
{
	char	*fi = path_out;
	char	*ne = path_in;

#ifdef _WIN32
	while (*ne) 
	{
		if (*ne == ':') {
			*fi++ = ':';
			*fi++ = '\\';
		}
		else if (*ne == '/') {
			*fi++ = '\\';
		}
		else {
			*fi++ = *ne;
		}
		ne++;
	}
	*fi = '\0';
#endif
#ifdef _XGL
	if (strchr(ne, ':') != NULL) *fi++ = '/';
	while (*ne)
	{
		if (*ne == ':') {
			*fi++ = '/';
		} else {
			*fi++ = *ne;
		}
		ne++;
	}
	*fi = '\0';
#endif
#ifdef _MACOS
	if (strchr(ne, ':') == NULL) *fi++ = ':';
	while (*ne)
	{
		if (*ne == '/') {
			*fi++ = ':';
		} else {
			*fi++ = *ne;
		}
		ne++;
	}
	*fi = '\0';
#endif

	return PATHCONV_NO_ERROR;
}


/*******  CHECK FILE EXISTING  ********/

#ifdef _MACOS
static unsigned char *CtoP ( char *c, unsigned char *buf )
{
	unsigned char  *p = buf;

	unsigned int ct = strlen(c);
	*p++ = (unsigned char) ct;
	while (ct--) *p++ = (unsigned char) *c++;
	return buf;
}

static int AccessFile( char *filename )
{
	Str255	name;
	FSSpec	spec;
	
	CtoP( filename, name );
	if (FSMakeFSSpec( 0, 0, name, &spec ) == fnfErr) {
		return (-1);
	}
	return 0;
}
#endif

static PathConvResult pc_file_exist( char *path_in )
{
#ifdef _WIN32
if (_access((path_in),0) == (-1)) return PATHCONV_FILENOTFOUND;
#endif
#ifdef _XGL
if (access((path_in), F_OK) != 0) return PATHCONV_FILENOTFOUND;
#endif
#ifdef _MACOS	
if (AccessFile((path_in)) == (-1)) return PATHCONV_FILENOTFOUND;
#endif

	return PATHCONV_NO_ERROR;
}


/***** COMPARE STRINGS WITH CAPS *****/

int STR_cmp( char *a, char *b )
{
	while(1)
	{
		if (*a == '\0' && *b == '\0') {
			return 0;
		}
		else if (*a == '\0' || *b == '\0') {
			return 1;
		}
		else {
			if (toupper(*a) == toupper(*b)) {
				a++; b++;
			} else {
				return 1;
			}
		}
	}
}


/***** COMPARE STRINGS WITH CAPS *****/

int STR_ncmp( char *a, char *b, int n )
{
	while(n--)
	{
		if (*a == '\0' && *b == '\0') {
			return 0;
		}
		else if (*a == '\0' || *b == '\0') {
			return 1;
		}
		else {
			if (toupper(*a) == toupper(*b)) {
				a++; b++;
			} else {
				return 1;
			}
		}
	}
	return 0;
}


