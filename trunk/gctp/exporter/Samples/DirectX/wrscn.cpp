/*********************************************************************/
/*                                                                   */
/* FILE :        wrscn.cpp                                           */
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


/*
 *
 *  WriteScene       - write the scene
 *
 */
void dxExporter::WriteScene()
{
	if (!_xfile.isOpen()) return;

#ifndef GCTP_USE_XFILEAPI
	WriteFileHeader();
	WriteHeader    ();
	WriteTemplates ();
#endif

	DX_Frame*       dx_frm;
	DX_Material*    dx_mat;

	if (_options->materialType == DX_MATTYPE_EXTERN)
	{
		dx_frm = _sceneRoot;
		while (dx_frm)
		{
			dx_mat = (dx_frm->mesh) ? dx_frm->mesh->meshMaterialList->materials : NULL;
			while (dx_mat)
			{
				WriteMaterial( dx_mat, _xfile );
				dx_mat = dx_mat->next;
			}
#ifdef GCTP_USE_XFILEAPI
			dx_mat = (dx_frm->mesh) ? dx_frm->mesh->wireMaterialList->materials : NULL;
			while (dx_mat)
			{
				WriteMaterial( dx_mat, _xfile );
				dx_mat = dx_mat->next;
			}
#endif
			dx_frm = dx_frm->next;
		}
	}

	if (_options->materialType == DX_MATTYPE_SHARED)
	{
		dx_frm = _sceneRoot;
		while (dx_frm)
		{
			dx_mat = (dx_frm->mesh) ? dx_frm->mesh->meshMaterialList->materials : NULL;
			while (dx_mat)
			{
				if (SharedMaterial( dx_mat )) {
					WriteMaterial( dx_mat, _xfile );
				}
				dx_mat = dx_mat->next;
			}
#ifdef GCTP_USE_XFILEAPI
			dx_mat = (dx_frm->mesh) ? dx_frm->mesh->wireMaterialList->materials : NULL;
			while (dx_mat)
			{
				if (SharedMaterial( dx_mat )) {
					WriteMaterial( dx_mat, _xfile );
				}
				dx_mat = dx_mat->next;
			}
#endif
			dx_frm = dx_frm->next;
		}
	}

	switch (_options->status) {
	case DX_STATUS_LAYOUT:
	case DX_STATUS_SCN2DX:
		WriteFrames      ( _sceneRoot,    _xfile );
		WriteAnimTicksPerSecond( _xfile );
		WriteAnimationSet( _animationSet, _xfile );
		break;
	case DX_STATUS_MODELER:
	case DX_STATUS_OBJ2DX:
		if (_options->outputFrames == TRUE) {
			WriteFrames( _sceneRoot, _xfile );
		}
		else {
			dx_frm = _sceneRoot;
			while (dx_frm)
			{
				if (dx_frm->mesh) {
					WriteMesh( dx_frm->mesh, _xfile );
				}
				dx_frm = dx_frm->next;
			}
		}
		break;
	}
}


/*
 *
 *  DX_SharedMaterial     - check if pr_mat is the shared primary one
 *
 */
DX_Material* dxExporter::SharedMaterial( DX_Material* pr_mat )
{
	DX_Frame*       dx_frm;
	DX_Material*    dx_mat;

	dx_frm = _sceneRoot;
	while (dx_frm)
	{
		dx_mat = (dx_frm->mesh) ? dx_frm->mesh->meshMaterialList->materials : NULL;
		while (dx_mat)
		{
			if (dx_mat == pr_mat) return pr_mat;
			if (strcmp(dx_mat->surfName, pr_mat->surfName) == 0) return NULL;
			dx_mat = dx_mat->next;
		}
		dx_frm = dx_frm->next;
	}

	return pr_mat;
}


#ifndef GCTP_USE_XFILEAPI
/*
 *
 *  WriteFileHeader    - write file header
 *
 */
void dxExporter::WriteFileHeader()
{
	switch (_options->format) {
	case DX_FORMAT_BINARY:
		BIN_WriteFileHeader();
		break;
	case DX_FORMAT_TEXT:
		TXT_WriteFileHeader();
		break;
	}
}


/*
 *
 *  TXT_WriteFileHeader    - write file header as TEXT
 *
 */
void dxExporter::TXT_WriteFileHeader()
{
	if (_options->version == DX_VERSION_DIRECTX8) {
		ffp( 0, "xof 0303txt 0032\n" );
	}
	if (_options->version == DX_VERSION_DIRECTX7) {
		ffp( 0, "xof 0302txt 0032\n" );
	}
}


/*
 *
 *  BIN_WriteFileHeader    - write file header as BINARY
 *
 */
void dxExporter::BIN_WriteFileHeader()
{
	if (_options->version == DX_VERSION_DIRECTX8) {
		ffp_bytes( (DX_BYTE *) "xof 0303bin 0032", 16 );
	}
	if (_options->version == DX_VERSION_DIRECTX7) {
		ffp_bytes( (DX_BYTE *) "xof 0302bin 0032", 16 );
	}
}


/*
 *
 *  WriteHeader       - write a header
 *
 */
void dxExporter::WriteHeader()
{
	switch (_options->format) {
	case DX_FORMAT_BINARY:
		BIN_WriteHeader();
		break;
	case DX_FORMAT_TEXT:
		TXT_WriteHeader();
		break;
	}
}


/*
 *
 *  TXT_WriteHeader       - write a header as TXT
 *
 */
void dxExporter::TXT_WriteHeader()
{
	if (_options->version == DX_VERSION_DIRECTX7) {
		ffp( 0, "\n" );
		ffp( 0, "Header {\n" );
		ffp( 1, "1;\n" );
		ffp( 1, "0;\n" );
		ffp( 1, "1;\n" );
		ffp( 0, "}\n" );
	}
}


/*
 *
 *  BIN_WriteHeader       - write a header as BIN
 *
 */
void dxExporter::BIN_WriteHeader()
{
	static DX_DWORD    list[] = { 1, 0, 0 };

	if (_options->version == DX_VERSION_DIRECTX7) {
		ffp_NAME        ( "Header" );
		ffp_TOKEN       ( TOKEN_OBRACE );
		ffp_INTEGER_LIST( list, 3 );
		ffp_TOKEN       ( TOKEN_CBRACE );
	}
}
#endif
