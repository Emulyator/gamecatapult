/*********************************************************************/
/*                                                                   */
/* FILE :        wrmat.cpp                                           */
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


/******  PROTOTYPES  ******************************/


#include <rmxfguid.h>
/*
 *
 *  WriteMaterial   - write the material
 *
 */

void dxExporter::WriteMaterial( DX_Material* dx_mat, gctp::XSaver &cur )
{
	if (_options->outputMaterials == FALSE) return;

	gctp::XSaveData _mtrl;
	switch (_options->materialType) {
	case DX_MATTYPE_INLINE:
		_mtrl = cur.newData(TID_D3DRMMaterial, sizeof(DX_MaterialData), &dx_mat->mtrl, dx_mat->nodeName);
		break;
	case DX_MATTYPE_EXTERN:
		_mtrl = cur.newData(TID_D3DRMMaterial, sizeof(DX_MaterialData), &dx_mat->mtrl, dx_mat->nodeName);
		break;
	case DX_MATTYPE_SHARED:
		_mtrl = cur.newData(TID_D3DRMMaterial, sizeof(DX_MaterialData), &dx_mat->mtrl, dx_mat->surfName);
		break;
	default:
		return;
	}

	if(_mtrl && strlen(dx_mat->textureFilename.filename) > 0) {
		_mtrl.newData(TID_D3DRMTextureFilename, strlen(dx_mat->textureFilename.filename)+1, dx_mat->textureFilename.filename);
	}
}
