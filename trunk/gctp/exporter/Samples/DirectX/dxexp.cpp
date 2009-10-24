/*********************************************************************/
/*                                                                   */
/* FILE :        dxexp.cpp                                           */
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
#include <gctp/wcstr.hpp>
#endif

#include "dxexp.h"


static void eS_PrintStatus( char* buf, ... );


/*
 *
 *  dxExporter   - constructor
 *
 */

dxExporter::dxExporter( dxOptions* opts, GlobalFunc* global )
{
	_errCode      = DX_ERR_NOERROR;
	_errFile[0]   = 0;
#ifndef GCTP_USE_XFILEAPI
	_fp           = NULL;
#endif
	_sceneRoot    = NULL;
	_animationSet = NULL;
	_obj          = NULL;
	_scn          = NULL;
	_options      = opts;
	_global       = global;
}


/*
 *
 *  ~dxExporter   - destructor
 *
 */

dxExporter::~dxExporter()
{
}


/*
 *
 *  ExportScene   - export the scene
 *
 */

bool dxExporter::ExportScene( SCN3_ID scn )
{
	MakeScene( scn );

	if (_errCode == DX_ERR_NOERROR)
	{
		if (OpenScene() == DX_ERR_NOERROR)
		{
			WriteScene();
			CloseScene();
		}
	}

	return (_errCode == DX_ERR_NOERROR) ? true : false;
}


/*
 *
 *  ExportObject  - export the object
 *
 */

bool dxExporter::ExportObject( OBJ2_ID obj )
{
	MakeObject( obj );

	if (_errCode == DX_ERR_NOERROR)
	{
		if (OpenScene() == DX_ERR_NOERROR)
		{
			WriteScene();
			CloseScene();
		}
	}

	return (_errCode == DX_ERR_NOERROR) ? true : false;
}


/*
 *
 *  MakeScene   - make a new scene
 *
 */

void dxExporter::MakeScene( SCN3_ID scn )
{
    LWSceneInfo *sceneinfo;
    sceneinfo = reinterpret_cast<LWSceneInfo *>(_global( LWSCENEINFO_GLOBAL, GFUSE_TRANSIENT ));

	_ticksPerSec = static_cast<DX_DWORD>(sceneinfo->framesPerSecond);
	_scn         = scn;
	_sceneRoot   = MakeRootFrame();

	if (_animationSet == NULL) {
		_animationSet = MakeAnimationSet();
	}

							/*  Create Frames for Objects and Bones  */

	SCNItemID   item, bone;

	item = (*_scn->first)( _scn, SCN_OBJECT, NULL );

	while (item)
	{
		MakeItemFrame( item );

#ifndef GCTP_USE_XFILEAPI
		if (_options->version > DX_VERSION_DIRECTX7)
#endif
		{
			bone = (*_scn->first)( _scn, SCN_BONE, item );
			while (bone)
			{
				MakeItemFrame( bone );
				bone = (*_scn->next)( _scn, bone );
			}
		}

		item = (*_scn->next)( _scn, item );
	}

							/*  Create Frames for Lights  */

	if (_options->outputLightFrames == TRUE)
	{
		item = (*_scn->first)( _scn, SCN_LIGHT, NULL );

		while (item)
		{
			MakeItemFrame( item );
			item = (*_scn->next)( _scn, item );
		}
	}

							/*  Create Frames for Cameras  */

	if (_options->outputCameraFrames == TRUE)
	{
		item = (*_scn->first)( _scn, SCN_CAMERA, NULL );

		while (item)
		{
			MakeItemFrame( item );
			item = (*_scn->next)( _scn, item );
		}
	}

							/*  Set Parent, Sibling and Children  */

	LinkFrames();

							/*  Create Skined Mesh  */

	DX_Frame*  dx_frm;

	dx_frm = _sceneRoot;

	while (dx_frm)
	{
		if (dx_frm->mesh) {
			MakeSkinWeights( dx_frm );
			MakeXSkinMesh  ( dx_frm );
		}
		dx_frm = dx_frm->next;
	}

							/*  Create Animations  */

	dx_frm = _sceneRoot;

	while (dx_frm)
	{
		switch (dx_frm->frameType) {
		case DX_FRAME_NULL:
		case DX_FRAME_MESH:
		case DX_FRAME_LIGHT:
		case DX_FRAME_CAMERA:
		case DX_FRAME_BONE:
			MakeAnimation( dx_frm );
			break;
		}
		dx_frm = dx_frm->next;
	}
}


/*
 *
 *  MakeObject   - make a new object
 *
 */

void dxExporter::MakeObject( OBJ2_ID obj )
{
	_obj = obj;

	_sceneRoot = MakeRootFrame();
	MakeObjectFrame( obj );
}


/*
 *
 *  OpenScene   - open the scne file
 *
 */

DX_ErrorCode dxExporter::OpenScene()
{
	_errCode = DX_ERR_NOERROR;
#ifdef GCTP_USE_XFILEAPI
	_xenv.registerDefaultTemplate();
	_xenv.registerSkinTemplate();
	_xenv.registerExTemplate();
	if (strlen(_options->outputFile) > 0) {
		if (_options->format == DX_FORMAT_RTEXT) {
			_xfile.open(_xenv, gctp::WCStr(_options->outputFile).c_str(), gctp::XFileWriter::TEXT);
		}
		else if (_options->format == DX_FORMAT_CTEXT) {
			_xfile.open(_xenv, gctp::WCStr(_options->outputFile).c_str(), gctp::XFileWriter::COMPRESSEDTEXT);
		}
		else if (_options->format == DX_FORMAT_RBINARY) {
			_xfile.open(_xenv, gctp::WCStr(_options->outputFile).c_str(), gctp::XFileWriter::BINARY);
		}
		else if (_options->format == DX_FORMAT_CBINARY) {
			_xfile.open(_xenv, gctp::WCStr(_options->outputFile).c_str(), gctp::XFileWriter::COMPRESSEDBINARY);
		}
	}
#else
	if (strlen(_options->outputFile) > 0) {
		if (_options->format == DX_FORMAT_TEXT) {
			_fp = fopen( _options->outputFile, "wt" );
		}
		if (_options->format == DX_FORMAT_BINARY) {
			_fp = fopen( _options->outputFile, "wb" );
		}
	} else {
		_fp = stdout;
	}

	if (_fp == NULL) {
		_errCode = DX_ERR_SCNFILE;
		strcpy( _errFile, _options->outputFile );
	}
#endif
	return _errCode;
}



/*
 *
 *  CloseScene   - close the scne file
 *
 */

DX_ErrorCode dxExporter::CloseScene()
{
	_errCode = DX_ERR_NOERROR;
#ifdef GCTP_USE_XFILEAPI
	if(_xfile.isOpen()) _xfile.close();
#else
	if (_fp)
	{
		if (_fp != stdout) fclose( _fp );
		_fp = NULL;
	}
#endif
	return _errCode;
}


/*
 *
 *  IsNewScene       - check the DX File existing
 *
 */
bool dxExporter::IsNewScene()
{
	PathConvParam  param;

	param.path_in   = _options->outputFile;
	PathConvert( PATHCONV_FILEEXIST, &param );

	return (param.result == PATHCONV_FILENOTFOUND) ? true : false;
}


