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
#include <gctp/vector.hpp>
#include <gctp/wcstr.hpp>

#include "dxexp.h"


const char *dxExporter::CHNAME_XPOS = "Position.X";
const char *dxExporter::CHNAME_YPOS = "Position.Y";
const char *dxExporter::CHNAME_ZPOS = "Position.Z";
const char *dxExporter::CHNAME_HEADING = "Rotation.H";
const char *dxExporter::CHNAME_PITCH = "Rotation.P";
const char *dxExporter::CHNAME_BANK = "Rotation.B";
const char *dxExporter::CHNAME_XSCALE = "Scale.X";
const char *dxExporter::CHNAME_YSCALE = "Scale.Y";
const char *dxExporter::CHNAME_ZSCALE = "Scale.Z";

/*
 *
 *  dxExporter   - constructor
 *
 */

dxExporter::dxExporter( dxOptions* opts, GlobalFunc* global ) : globals_(global)
{
	_errCode      = DX_ERR_NOERROR;
	_errFile[0]   = 0;
	_sceneRoot    = NULL;
	_animationSet = NULL;
	_options      = opts;
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

bool dxExporter::ExportScene()
{
	MakeScene();

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

bool dxExporter::ExportObject( int obj )
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

void dxExporter::MakeScene()
{
	_ticksPerSec = static_cast<DX_DWORD>(globals_.getSceneInfo()->framesPerSecond);
	_sceneRoot   = MakeRootFrame();

	if (_animationSet == NULL) {
		_animationSet = MakeAnimationSet();
	}

							/*  Create Frames for Objects and Bones  */

	LWW::Item item = globals_.firstItem( LWI_OBJECT );

	while (item)
	{
		MakeItemFrame( item );

		{
			LWW::Item bone = item.firstBone();
			while (bone)
			{
				MakeItemFrame( bone );
				bone = bone.next();
			}
		}

		item = item.next();
	}

							/*  Create Frames for Lights  */

	if (_options->outputLightFrames == TRUE)
	{
		item = globals_.firstItem( LWI_OBJECT );
	
		while (item)
		{
			MakeItemFrame( item );
			item = item.next();
		}
	}

							/*  Create Frames for Cameras  */

	if (_options->outputCameraFrames == TRUE)
	{
		item = globals_.firstItem( LWI_CAMERA );

		while (item)
		{
			MakeItemFrame( item );
			item = item.next();
		}
	}

							/*  Set Parent, Sibling and Children  */

	LinkFrames();

							/*  Create Skined Mesh  */

	DX_Frame*  dx_frm;

	dx_frm = _sceneRoot;

	while (dx_frm) {
		if (dx_frm->mesh) {
			MakeSkinWeights( dx_frm );
			MakeXSkinMesh  ( dx_frm );
		}
		dx_frm = dx_frm->next;
	}

							/*  Create Animations  */

	dx_frm = _sceneRoot;

	while (dx_frm) {
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

void dxExporter::MakeObject( int obj )
{
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
	if(_xfile.isOpen()) _xfile.close();
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


