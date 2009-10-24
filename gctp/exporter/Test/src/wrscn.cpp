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
#include <gctp/vector.hpp>

#include "dxexp.h"


/*
 *
 *  WriteScene       - write the scene
 *
 */
void dxExporter::WriteScene()
{
	if (!_xfile.isOpen()) return;

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
			dx_mat = (dx_frm->mesh) ? dx_frm->mesh->wireMaterialList->materials : NULL;
			while (dx_mat)
			{
				WriteMaterial( dx_mat, _xfile );
				dx_mat = dx_mat->next;
			}
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
			dx_mat = (dx_frm->mesh) ? dx_frm->mesh->wireMaterialList->materials : NULL;
			while (dx_mat)
			{
				if (SharedMaterial( dx_mat )) {
					WriteMaterial( dx_mat, _xfile );
				}
				dx_mat = dx_mat->next;
			}
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
