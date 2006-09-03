/*********************************************************************/
/*                                                                   */
/* FILE :        dxcfg.cpp                                           */
/*                                                                   */
/* DESCRIPTION : LightWave 3D to DirectX Export                      */
/*                                                                   */
/* HISTORY :     Oct 29, 2000    written by Yoshiaki Tazaki          */
/*               Oct 02, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/

#include "dxopt.h"

#define DX3_CONFIG   "gctp_lw2dx.cfg"

enum ES_TOKENS {
	TOK_EXPD = 0,
	TOK_TXTD,
	TOK_VERS,
	TOK_FORM,
	TOK_MATE,
#ifndef GCTP_USE_XFILEAPI
	TOK_TEMP,
#endif
	TOK_ANIM,
	TOK_FRAM,
	TOK_PREF,
	TOK_NORM,
	TOK_TXUV,
	TOK_REVV,
	TOK_VCOL,
	TOK_SKWF,
	TOK_MXWV,
	TOK_MXWF,
	TOK_TRPL,
	TOK_MATF,
	TOK_DECL,
	TOK_IMGO,
	TOK_IMGF,
	TOK_IMGR,
	TOK_RESX,
	TOK_RESY,
	TOK_TXNM,
	TOK_FREX,
	TOK_STEP,
	TOK_FSCL,
	TOK_MXKY,
	TOK_RMUK,
	TOK_CAMO,
	TOK_LGTO,
	TOK_BKIK,
	TOK_VIEW,
	TOK_VWNM,
	TOK_LAST
};

#define DX3_EXPD      LWID_( 'E', 'X', 'P', 'D' )
#define DX3_TXTD      LWID_( 'T', 'X', 'T', 'D' )
#define DX3_VERS      LWID_( 'V', 'E', 'R', 'S' )
#define DX3_FORM      LWID_( 'F', 'O', 'R', 'M' )
#define DX3_MATE      LWID_( 'M', 'A', 'T', 'E' )
#ifndef GCTP_USE_XFILEAPI
#define DX3_TEMP      LWID_( 'T', 'E', 'M', 'P' )
#endif
#define DX3_ANIM      LWID_( 'A', 'N', 'I', 'M' )
#define DX3_FRAM      LWID_( 'F', 'R', 'A', 'M' )
#define DX3_PREF      LWID_( 'P', 'R', 'E', 'F' )
#define DX3_NORM      LWID_( 'N', 'O', 'R', 'M' )
#define DX3_TXUV      LWID_( 'T', 'X', 'U', 'V' )
#define DX3_REVV      LWID_( 'R', 'E', 'V', 'V' )
#define DX3_VCOL      LWID_( 'V', 'C', 'O', 'L' )
#define DX3_SKWF      LWID_( 'S', 'K', 'W', 'F' )
#define DX3_MXWV      LWID_( 'M', 'X', 'W', 'V' )
#define DX3_MXWF      LWID_( 'M', 'X', 'W', 'F' )
#define DX3_TRPL      LWID_( 'T', 'R', 'P', 'L' )
#define DX3_MATF      LWID_( 'M', 'A', 'T', 'F' )
#define DX3_DECL      LWID_( 'D', 'E', 'C', 'L' )
#define DX3_IMGO      LWID_( 'I', 'M', 'G', 'O' )
#define DX3_IMGF      LWID_( 'I', 'M', 'G', 'F' )
#define DX3_IMGR      LWID_( 'I', 'M', 'G', 'R' )
#define DX3_RESX      LWID_( 'R', 'E', 'S', 'X' )
#define DX3_RESY      LWID_( 'R', 'E', 'S', 'Y' )
#define DX3_TXNM      LWID_( 'T', 'X', 'N', 'M' )
#define DX3_FREX      LWID_( 'F', 'R', 'E', 'X' )
#define DX3_STEP      LWID_( 'S', 'T', 'E', 'P' )
#define DX3_FSCL      LWID_( 'F', 'S', 'C', 'L' )
#define DX3_MXKY      LWID_( 'M', 'X', 'K', 'Y' )
#define DX3_RMUK      LWID_( 'R', 'M', 'U', 'K' )
#define DX3_CAMO      LWID_( 'C', 'A', 'M', 'O' )
#define DX3_LGTO      LWID_( 'L', 'G', 'T', 'O' )
#define DX3_BKIK      LWID_( 'B', 'K', 'I', 'K' )
#define DX3_VIEW      LWID_( 'V', 'I', 'E', 'W' )
#define DX3_VWNM      LWID_( 'V', 'W', 'N', 'M' )


/******  PROTOTYPES  ******************************/

static void BlockIndents    ( LWBlockIdent* blockID );
static void SettingDirectory( GlobalFunc* global, char* dirname );

/*
 *
 *  LoadConfig   - read the config file
 *
 */
void dxOptions::LoadConfig( GlobalFunc *global )
{
	LWBlockIdent         blockID[TOK_LAST+1];
	PathConvParam        param;
	DX_FNAM              fname, dirname, name;
	LWID                 lwid ;
	const char*          content;

	LWStateQueryFuncs* query = (LWStateQueryFuncs *) (*global)( LWSTATEQUERYFUNCS_GLOBAL, GFUSE_TRANSIENT );
	LWSceneInfo*       scene = (LWSceneInfo       *) (*global)( LWSCENEINFO_GLOBAL,       GFUSE_TRANSIENT );
	LWFileIOFuncs*     fio   = (LWFileIOFuncs     *) (*global)( LWFILEIOFUNCS_GLOBAL,     GFUSE_TRANSIENT );
	LWMessageFuncs*    im    = (LWMessageFuncs    *) (*global)( LWMESSAGEFUNCS_GLOBAL,    GFUSE_TRANSIENT );
	LWDirInfoFunc*     dire  = (LWDirInfoFunc     *) (*global)( LWDIRINFOFUNC_GLOBAL,     GFUSE_TRANSIENT );

	content = (*dire)("Content");
	if (content) {
		strcpy( exportDirectory,  content );
		strcpy( textureDirectory, content );
	}

	if (query && (*query->object)()) {
		param.path_in  = (char *) (*query->object)();
		param.name     = name;
		PathConvert( PATHCONV_GETBODY, &param );
		param.path_in  = name;
		param.ext      = ".x";
		param.path_out = outputFile;
		PathConvert( PATHCONV_REPLACEXT, &param );
	}

	if (scene && scene->filename) {
		param.path_in  = (char *) scene->filename;
		param.name     = name;
		PathConvert( PATHCONV_GETBODY, &param );
		param.path_in  = name;
		param.ext      = ".x";
		param.path_out = outputFile;
		PathConvert( PATHCONV_REPLACEXT, &param );
	}

	SettingDirectory( global, dirname );

	param.directory = dirname;
	param.name      = DX3_CONFIG;
	param.path_out  = fname;
	PathConvert( PATHCONV_BUILDPATH, &param );

	BlockIndents( blockID );

	LWLoadState* lstate = (*fio->openLoad)( fname, LWIO_ASCII );
	if (lstate == NULL) return;

	while ((lwid = LWLOAD_FIND( lstate, blockID )))
	{
		switch (lwid) {
		case DX3_EXPD:
		  LWLOAD_STR( lstate, exportDirectory, sizeof(DX_FNAM) );
		  break;
		case DX3_TXTD:
		  LWLOAD_STR( lstate, textureDirectory, sizeof(DX_FNAM) );
		  break;
		case DX3_VERS:
		  LWLOAD_I4 ( lstate, (long *) &version, 1 );
		  break;
		case DX3_FORM:
		  LWLOAD_I4 ( lstate, (long *) &format, 1 );
		  break;
		case DX3_MATE:
		  LWLOAD_I4 ( lstate, (long *) &outputMaterials, 1 );
		  break;
#ifndef GCTP_USE_XFILEAPI
		case DX3_TEMP:
		  LWLOAD_I4 ( lstate, (long *) &outputTemplates, 1 );
		  break;
#endif
		case DX3_ANIM:
		  LWLOAD_I4 ( lstate, (long *) &outputAnimations, 1 );
		  break;
		case DX3_FRAM:
		  LWLOAD_I4 ( lstate, (long *) &outputFrames, 1 );
		  break;
		case DX3_PREF:
		  LWLOAD_STR( lstate, framePrefix, sizeof(DX_STRING) );
		  break;
		case DX3_NORM:
		  LWLOAD_I4 ( lstate, (long *) &outputMeshNormals, 1 );
		  break;
		case DX3_TXUV:
		  LWLOAD_I4 ( lstate, (long *) &outputTextureCoords, 1 );
		  break;
		case DX3_REVV:
		  LWLOAD_I4 ( lstate, (long *) &reverseUVCoords, 1 );
		  break;
		case DX3_VCOL:
		  LWLOAD_I4 ( lstate, (long *) &outputVertexColors, 1 );
		  break;
		case DX3_SKWF:
		  LWLOAD_I4 ( lstate, (long *) &useSkinWeightsPerVertex, 1 );
		  break;
		case DX3_MXWV:
		  LWLOAD_I4 ( lstate, (long *) &nMaxSkinWeightsPerVertex, 1 );
		  break;
		case DX3_MXWF:
		  LWLOAD_I4 ( lstate, (long *) &nMaxSkinWeightsPerFace, 1 );
		  break;
		case DX3_TRPL:
		  LWLOAD_I4 ( lstate, (long *) &tripleAll, 1 );
		  break;
		case DX3_MATF:
		  LWLOAD_I4 ( lstate, (long *) &materialType, 1 );
		  break;
		case DX3_DECL:
		  LWLOAD_I4 ( lstate, (long *) &enbaleDecalTexture, 1 );
		  break;
		case DX3_IMGO:
		  LWLOAD_I4 ( lstate, (long *) &outputTextureImages, 1 );
		  break;
		case DX3_IMGF:
		  LWLOAD_I4 ( lstate, (long *) &imageSaver, 1 );
		  break;
		case DX3_IMGR:
		  LWLOAD_I4 ( lstate, (long *) &resizeTextures, 1 );
		  break;
		case DX3_RESX:
		  LWLOAD_I4 ( lstate, (long *) &textureMaxResX, 1 );
		  break;
		case DX3_RESY:
		  LWLOAD_I4 ( lstate, (long *) &textureMaxResY, 1 );
		  break;
#ifdef _WIN32
		case DX3_TXNM:
		  LWLOAD_I4 ( lstate, (long *) &textureFilename, 1 );
		  break;
#endif
		case DX3_FREX:
		  LWLOAD_I4 ( lstate, (long *) &frameToExport, 1 );
		  break;
		case DX3_STEP:
		  LWLOAD_I4 ( lstate, (long *) &frameSteps, 1 );
		  break;
		case DX3_FSCL:
		  LWLOAD_I4 ( lstate, (long *) &frameTimeScale, 1 );
		  break;
		case DX3_MXKY:
		  LWLOAD_I4 ( lstate, (long *) &useMatrixKey, 1 );
		  break;
		case DX3_RMUK:
		  LWLOAD_I4 ( lstate, (long *) &removeUnusableKeyframes, 1 );
		  break;
		case DX3_CAMO:
		  LWLOAD_I4 ( lstate, (long *) &outputCameraFrames, 1 );
		  break;
		case DX3_LGTO:
		  LWLOAD_I4 ( lstate, (long *) &outputLightFrames, 1 );
		  break;
		case DX3_BKIK:
		  LWLOAD_I4 ( lstate, (long *) &bakeIKMotions, 1 );
		  break;
		case DX3_VIEW:
		  LWLOAD_I4 ( lstate, (long *) &enableViewer, 1 );
		  break;
		case DX3_VWNM:
		  LWLOAD_STR( lstate, viewerName, sizeof(DX_STRING) );
		  break;
		}
		LWLOAD_END( lstate );
	}

	(*fio->closeLoad)( lstate );

	if (strlen(exportDirectory) > 0) {
		strcpy( name, outputFile );
		param.directory = exportDirectory;
		param.name      = name;
		param.path_out  = outputFile;
		PathConvert( PATHCONV_BUILDPATH, &param );
	}
}


/*
 *
 *  SaveConfig   - save the config file
 *
 */
void dxOptions::SaveConfig( GlobalFunc *global )
{
	LWBlockIdent         blockID[TOK_LAST+1];
	PathConvParam        param;
	DX_FNAM              fname, dirname;

	LWFileIOFuncs*  fio  = (LWFileIOFuncs *) (*global)( LWFILEIOFUNCS_GLOBAL, GFUSE_TRANSIENT );
	if (fio == NULL) return;

	SettingDirectory( global, dirname );

	param.directory = dirname;
	param.name      = DX3_CONFIG;
	param.path_out  = fname;
	PathConvert( PATHCONV_BUILDPATH, &param );

	BlockIndents( blockID );

	LWSaveState* sstate = (*fio->openSave)( fname, LWIO_ASCII );
	if (sstate == NULL) return;

	LWSAVE_BEGIN( sstate, &blockID[TOK_EXPD], 0 );
	LWSAVE_STR  ( sstate, exportDirectory );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_TXTD], 0 );
	LWSAVE_STR  ( sstate, textureDirectory );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_VERS], 0 );
	LWSAVE_I4   ( sstate, (const long *) &version, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_FORM], 0 );
	LWSAVE_I4   ( sstate, (const long *) &format, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_MATE], 0 );
	LWSAVE_I4   ( sstate, (const long *) &outputMaterials, 1 );
	LWSAVE_END  ( sstate );

#ifndef GCTP_USE_XFILEAPI
	LWSAVE_BEGIN( sstate, &blockID[TOK_TEMP], 0 );
	LWSAVE_I4   ( sstate, (const long *) &outputTemplates, 1 );
	LWSAVE_END  ( sstate );
#endif

	LWSAVE_BEGIN( sstate, &blockID[TOK_ANIM], 0 );
	LWSAVE_I4   ( sstate, (const long *) &outputAnimations, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_FRAM], 0 );
	LWSAVE_I4   ( sstate, (const long *) &outputFrames, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_PREF], 0 );
	LWSAVE_STR  ( sstate, framePrefix );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_NORM], 0 );
	LWSAVE_I4   ( sstate, (const long *) &outputMeshNormals, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_TXUV], 0 );
	LWSAVE_I4   ( sstate, (const long *) &outputTextureCoords, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_REVV], 0 );
	LWSAVE_I4   ( sstate, (const long *) &reverseUVCoords, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_VCOL], 0 );
	LWSAVE_I4   ( sstate, (const long *) &outputVertexColors, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_SKWF], 0 );
	LWSAVE_I4   ( sstate, (const long *) &useSkinWeightsPerVertex, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_MXWV], 0 );
	LWSAVE_I4   ( sstate, (const long *) &nMaxSkinWeightsPerVertex, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_MXWF], 0 );
	LWSAVE_I4   ( sstate, (const long *) &nMaxSkinWeightsPerFace, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_TRPL], 0 );
	LWSAVE_I4   ( sstate, (const long *) &tripleAll, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_MATF], 0 );
	LWSAVE_I4   ( sstate, (const long *) &materialType, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_DECL], 0 );
	LWSAVE_I4   ( sstate, (const long *) &enbaleDecalTexture, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_IMGO], 0 );
	LWSAVE_I4   ( sstate, (const long *) &outputTextureImages, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_IMGF], 0 );
	LWSAVE_I4   ( sstate, (const long *) &imageSaver, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_IMGR], 0 );
	LWSAVE_I4   ( sstate, (const long *) &resizeTextures, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_RESX], 0 );
	LWSAVE_I4   ( sstate, (const long *) &textureMaxResX, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_RESY], 0 );
	LWSAVE_I4   ( sstate, (const long *) &textureMaxResY, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_TXNM], 0 );
	LWSAVE_I4   ( sstate, (const long *) &textureFilename, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_FREX], 0 );
	LWSAVE_I4   ( sstate, (const long *) &frameToExport, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_STEP], 0 );
	LWSAVE_I4   ( sstate, (const long *) &frameSteps, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_FSCL], 0 );
	LWSAVE_I4   ( sstate, (const long *) &frameTimeScale, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_RMUK], 0 );
	LWSAVE_I4   ( sstate, (const long *) &removeUnusableKeyframes, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_MXKY], 0 );
	LWSAVE_I4   ( sstate, (const long *) &useMatrixKey, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_CAMO], 0 );
	LWSAVE_I4   ( sstate, (const long *) &outputCameraFrames, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_LGTO], 0 );
	LWSAVE_I4   ( sstate, (const long *) &outputLightFrames, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_BKIK], 0 );
	LWSAVE_I4   ( sstate, (const long *) &bakeIKMotions, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_VIEW], 0 );
	LWSAVE_I4   ( sstate, (const long *) &enableViewer, 1 );
	LWSAVE_END  ( sstate );

	LWSAVE_BEGIN( sstate, &blockID[TOK_VWNM], 0 );
	LWSAVE_STR  ( sstate, viewerName );
	LWSAVE_END  ( sstate );


	(*fio->closeSave)( sstate );
}



/*
 *
 *  BlockIndents   - setting block IDs
 *
 */
static void BlockIndents( LWBlockIdent* blockID )
{
	blockID[TOK_EXPD].id    = DX3_EXPD;
	blockID[TOK_EXPD].token = "ExportDirectory";

	blockID[TOK_TXTD].id    = DX3_TXTD;
	blockID[TOK_TXTD].token = "TextureDirectory";
	
	blockID[TOK_VERS].id    = DX3_VERS;
	blockID[TOK_VERS].token = "Version";
	
	blockID[TOK_FORM].id    = DX3_FORM;
	blockID[TOK_FORM].token = "Format";
	
	blockID[TOK_MATE].id    = DX3_MATE;
	blockID[TOK_MATE].token = "OutputMaterial";

#ifndef GCTP_USE_XFILEAPI
	blockID[TOK_TEMP].id    = DX3_TEMP;
	blockID[TOK_TEMP].token = "OutputTemplates";
#endif
	
	blockID[TOK_ANIM].id    = DX3_ANIM;
	blockID[TOK_ANIM].token = "OutputAnimations";
	
	blockID[TOK_FRAM].id    = DX3_FRAM;
	blockID[TOK_FRAM].token = "OutputFrames";
	
	blockID[TOK_PREF].id    = DX3_PREF;
	blockID[TOK_PREF].token = "FramePrefix";
	
	blockID[TOK_NORM].id    = DX3_NORM;
	blockID[TOK_NORM].token = "OutputMeshNormals";
	
	blockID[TOK_TXUV].id    = DX3_TXUV;
	blockID[TOK_TXUV].token = "OutputTextureCoords";
	
	blockID[TOK_REVV].id    = DX3_REVV;
	blockID[TOK_REVV].token = "ReverseUVCoords";
	
	blockID[TOK_VCOL].id    = DX3_VCOL;
	blockID[TOK_VCOL].token = "OutputVertexColors";
	
	blockID[TOK_SKWF].id    = DX3_SKWF;
	blockID[TOK_SKWF].token = "SkinWeightsPerVertexFlag";
	
	blockID[TOK_MXWV].id    = DX3_MXWV;
	blockID[TOK_MXWV].token = "MaxSkinWeightsPerVertex";
	
	blockID[TOK_MXWF].id    = DX3_MXWF;
	blockID[TOK_MXWF].token = "MaxSkinWeightsPerFace";
	
	blockID[TOK_TRPL].id    = DX3_TRPL;
	blockID[TOK_TRPL].token = "TripleAll";
	
	blockID[TOK_MATF].id    = DX3_MATF;
	blockID[TOK_MATF].token = "MaterialFormat";
	
	blockID[TOK_DECL].id    = DX3_DECL;
	blockID[TOK_DECL].token = "EnableDecalTexture";
	
	blockID[TOK_IMGO].id    = DX3_IMGO;
	blockID[TOK_IMGO].token = "OutputTextureImages";
	
	blockID[TOK_IMGF].id    = DX3_IMGF;
	blockID[TOK_IMGF].token = "ImageSaver";
	
	blockID[TOK_IMGR].id    = DX3_IMGR;
	blockID[TOK_IMGR].token = "ResizeTextures";
	
	blockID[TOK_RESX].id    = DX3_RESX;
	blockID[TOK_RESX].token = "TextureMaxResX";
	
	blockID[TOK_RESY].id    = DX3_RESY;
	blockID[TOK_RESY].token = "TextureMaxResY";
	
	blockID[TOK_TXNM].id    = DX3_TXNM;
	blockID[TOK_TXNM].token = "TextureFilename";
	
	blockID[TOK_FREX].id    = DX3_FREX;
	blockID[TOK_FREX].token = "FrameToExport";
	
	blockID[TOK_STEP].id    = DX3_STEP;
	blockID[TOK_STEP].token = "FrameSteps";
	
	blockID[TOK_FSCL].id    = DX3_FSCL;
	blockID[TOK_FSCL].token = "FrameTimeScale";
	
	blockID[TOK_MXKY].id    = DX3_MXKY;
	blockID[TOK_MXKY].token = "UseMatrixKey";
	
	blockID[TOK_RMUK].id    = DX3_RMUK;
	blockID[TOK_RMUK].token = "RemoveUnusedKeyframes";
	
	blockID[TOK_CAMO].id    = DX3_CAMO;
	blockID[TOK_CAMO].token = "OutputCameraFrames";
	
	blockID[TOK_LGTO].id    = DX3_LGTO;
	blockID[TOK_LGTO].token = "OutputLightFrames";
	
	blockID[TOK_BKIK].id    = DX3_BKIK;
	blockID[TOK_BKIK].token = "BakeIKMotions";
	
	blockID[TOK_VIEW].id    = DX3_VIEW;
	blockID[TOK_VIEW].token = "EnableViewer";
	
	blockID[TOK_VWNM].id    = DX3_VWNM;
	blockID[TOK_VWNM].token = "ViewerName";
	
	blockID[TOK_LAST].token = NULL;
	blockID[TOK_LAST].id    = 0;
}



/*
 *
 *  SettingDirectory   - setting block IDs
 *
 */
static void SettingDirectory( GlobalFunc* global, char* dirname )
{
#ifdef _WIN32
	char                *profile;

	dirname[0] = 0x00;
	profile    = getenv("USERPROFILE");
	if (profile == NULL) {
		profile = getenv("windir");
	}
	if (profile) {
		strcpy( dirname, profile );
	}
#else
	LWDirInfoFunc       *dire;
	const char          *profile;

	dirname[0] = 0x00;
	dire    = (LWDirInfoFunc *) (*global)( LWDIRINFOFUNC_GLOBAL, GFUSE_TRANSIENT );
	profile = (*dire)("Settings");
	if (profile) {
		strcpy( dirname, profile );
	}
#endif
}


