/*
 * UI_IMG.CPP -- DirectX Export
 *
 * Copyright (c) 2000-2001, D-STORM, Inc. 
 *
 * written by Yoshiaki Tazaki
 * revision  history  10/02/2001
 *
 */

#include "dxui.h"


/******  PROTOTYPES  ******************************/

static int         UI_DefineSize   ( int len, int maxres, int resize );
static int         UI_Power2       ( int n );
static const char *UI_ConvName     ( const char *name, char *expd, char *ext );
static const char *UI_CopyName     ( const char *name, char *expd );
static bool        UI_ImageNotExist( const char *name );
static bool        UI_CopyFile     ( const char *dst, const char *src );


/*
 *
 *  ConvertImages   - convert image files
 *
 */
void dxUI::ConvertImages()
{
	LWPixmapID        src, dst;
	const char        *name, *filename;
	int               w, h, wo, ho, result, error;
	bool              action, force = false;
	DX_STRING         info;

	LWImageID  imgid = (*_imglist->first)();

	while (imgid)
	{
		(*_imglist->size)( imgid, &w, &h );
		wo       = UI_DefineSize( w, _options->textureMaxResX, _options->resizeTextures );
		ho       = UI_DefineSize( h, _options->textureMaxResY, _options->resizeTextures );
		filename = (*_imglist->filename)( imgid, 0 );
		if (!filename) {
			(*_msgfunc->error)( "Can't get an image filename.", (const char *) 0 );
			return;
		}
		name   = UI_ConvName( filename, _options->textureDirectory, _options->imageExtension );
		action = force ? true : UI_ImageNotExist( name );

		if (!action)
		{
			sprintf( info, "The file %s", name );
			result = (*_msgfunc->yesNoAll)("Warning", info, "already exist. Override ?");

			switch (result) {
			case 0:					//  Cancel
			  return;
			case 1:					//  No
			  action = false;
			  break;
			case 2:					//  Yes
			  action = true;
			  break;
			case 3:					//  YesAll
			  force  = true;
			  action = true;
			  break;
			}
		}
		if (action)
		{
			if ((src  = (*_imglist->evaluate)( imgid, 0 )))
			{
				if (wo == w && ho == h) {
					error = (*_imgutil->save)( src, _options->imageSaver, name );
				} else {
					dst   = (*_imgutil->resample)( src, wo, ho, LWISM_BILINEAR );
					error = (*_imgutil->save)    ( dst, _options->imageSaver, name );
					(*_imgutil->destroy)( dst );
				}
				if (error) {
					(*_msgfunc->error)( "Can't save the image.", name );
				}
			}
			else {
				(*_msgfunc->error)( "Can't evaluate the image data.", name );
			}
		}

		imgid = (*_imglist->next)( imgid );
	}
}


/*
 *
 *  CopyImages   - copy image files
 *
 */
void dxUI::CopyImages()
{
	const char        *name, *filename;
	int               result;
	bool              action, force = false;
	DX_STRING         info;

	LWImageID  imgid = (*_imglist->first)();

	while (imgid)
	{
		filename = (*_imglist->filename)( imgid, 0 );
		if (!filename) {
			(*_msgfunc->error)( "Can't get an image filename.", (const char *) 0 );
			return;
		}
		name   = UI_CopyName( filename, _options->textureDirectory );
		action = force ? true : UI_ImageNotExist( name );

		if (!action)
		{
			sprintf( info, "The file %s", name );
			result = (*_msgfunc->yesNoAll)("Warning", info, "already exist. Override ?");

			switch (result) {
			case 0:					//  Cancel
			  return;
			case 1:					//  No
			  action = false;
			  break;
			case 2:					//  Yes
			  action = true;
			  break;
			case 3:					//  YesAll
			  force  = true;
			  action = true;
			  break;
			}
		}
		if (action)
		{
			if (!UI_CopyFile( name, filename )) {
				(*_msgfunc->error)( "Can't save the image.", name );
			}
		}

		imgid = (*_imglist->next)( imgid );
	}
}


/*
 *
 *  UI_DefineSize   - define size with power of 2
 *
 */

#define IMG_RESIZE_NONE    0
#define IMG_RESIZE_EXPAND  1
#define IMG_RESIZE_SHRINK  2
#define IMG_RESIZE_NEAREST 3

static int UI_DefineSize( int len, int maxres, int resize )
{
	int      n, low, hig;
	int      count  = 30;

	if (resize == IMG_RESIZE_NONE) goto EXIT;

	n   = 1;
	low = 1;

	while (count--)
	{
		if (len == low) goto EXIT;

		hig = 1 << n; n++;

		switch (resize) {
		case IMG_RESIZE_EXPAND:
			if (len > low && len < hig) {
				len    = hig;
				goto EXIT;
			}
			break;
		case IMG_RESIZE_SHRINK:
			if (len < hig) {
				len    = low;
				goto EXIT;
			}
			break;
		case IMG_RESIZE_NEAREST:
			if (len > low && len < hig) {
				if ((hig-len) < (len-low)) {
					len = hig;
				} else {
					len = low;
				}
				goto EXIT;
			}
			break;
		}
		low = hig;
	}

EXIT:
	if (len > maxres) {
		len   = maxres;
	}
	return len;
}

#undef IMG_RESIZE_NONE
#undef IMG_RESIZE_EXPAND
#undef IMG_RESIZE_SHRINK
#undef IMG_RESIZE_NEAREST


/*
 *
 *  UI_ConvName   - get filename
 *
 */
static const char *UI_ConvName( const char *name, char *expd, char *ext )
{
	PathConvParam   param;
	DX_FNAM         tmp1, tmp2;

	static DX_FNAM  filename;

	param.path_in   = (char *) name;
	param.name      = tmp1;
	PathConvert( PATHCONV_GETBODY, &param );
	param.path_in   = tmp1;
	param.ext       = ext;
	param.path_out  = tmp2;
	PathConvert( PATHCONV_REPLACEXT, &param );
	param.directory = expd;
	param.name      = tmp2;
	param.path_out  = filename;
	PathConvert( PATHCONV_BUILDPATH, &param );

	return filename;
}


/*
 *
 *  UI_CopyName   - get filename
 *
 */
static const char *UI_CopyName( const char *name, char *expd )
{
	PathConvParam   param;
	DX_FNAM         tmp;

	static DX_FNAM  filename;

	param.path_in   = (char *) name;
	param.name      = tmp;
	PathConvert( PATHCONV_GETBODY, &param );
	param.directory = expd;
	param.name      = tmp;
	param.path_out  = filename;
	PathConvert( PATHCONV_BUILDPATH, &param );

	return filename;
}



/*
 *
 *  UI_ImageNotExist   - confirm the image file existing
 *
 */
static bool UI_ImageNotExist( const char *name )
{
	PathConvParam  param;

	param.path_in   = (char *) name;
	PathConvert( PATHCONV_FILEEXIST, &param );

	return (param.result == PATHCONV_FILENOTFOUND) ? true : false;
}



/*
 *
 *  UI_CopyFile   - copy image files
 *
 */
static bool UI_CopyFile( const char *dst, const char *src )
{
	FILE*  inp  = fopen( src, "rb" );
	FILE*  outp = fopen( dst, "wb" );
	long   size;
	char*  buff   = 0;
	bool   result = false;

	if (!inp)  goto ERROR_EXIT;
	if (!outp) goto ERROR_EXIT;

	fseek( inp, 0L, SEEK_END );
	size = ftell( inp );
	fseek( inp, 0L, SEEK_SET );

	buff = new char[size];
	if (buff == NULL) goto ERROR_EXIT;

	if (fread ( buff, 1, size, inp ) != (unsigned int) size) {
		goto ERROR_EXIT;
	}
	if (fwrite( buff, 1, size, outp ) != (unsigned int) size) {
		goto ERROR_EXIT;
	}

	result = true;

ERROR_EXIT:
	delete[] buff;
	if (inp)  fclose( inp );
	if (outp) fclose( outp );
	return result;
}

