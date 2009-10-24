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
#ifdef GCTP_USE_XFILEAPI
#include <gctp/vector.hpp>
#endif

#include "dxexp.h"


/******  PROTOTYPES  ******************************/


#ifdef GCTP_USE_XFILEAPI
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

#else
/*
 *
 *  WriteMaterial   - write the material
 *
 */

void dxExporter::WriteMaterial( DX_Material* dx_mat, int tabs )
{
	if (_options->outputMaterials == FALSE) return;

	switch (_options->format) {
	case DX_FORMAT_BINARY:
		BIN_WriteMaterial( dx_mat );
		break;
	case DX_FORMAT_TEXT:
		TXT_WriteMaterial( dx_mat, tabs );
		break;
	}
}

/*
 *
 *  TXT_WriteMaterial   - write the material
 *
 */

void dxExporter::TXT_WriteMaterial( DX_Material* dx_mat, int tabs )
{
	if (_options->outputMaterials == FALSE) return;

	ffp( 0, "\n" );

	switch (_options->materialType) {
	case DX_MATTYPE_INLINE:
		ffp( tabs, "Material {\n" );
		break;
	case DX_MATTYPE_EXTERN:
		ffp( tabs, "Material %s {\n", dx_mat->nodeName );
		break;
	case DX_MATTYPE_SHARED:
		ffp( tabs, "Material %s {\n", dx_mat->surfName );
		break;
	}

	ffp( tabs+1, "%f;%f;%f;%f;;\n", dx_mat->faceColor.red,
									dx_mat->faceColor.green,
									dx_mat->faceColor.blue,
									dx_mat->faceColor.alpha );

	ffp( tabs+1, "%f;\n", dx_mat->power );

	ffp( tabs+1, "%f;%f;%f;;\n",    dx_mat->specularColor.red,
									dx_mat->specularColor.green,
									dx_mat->specularColor.blue );

	ffp( tabs+1, "%f;%f;%f;;\n",    dx_mat->emissiveColor.red,
									dx_mat->emissiveColor.green,
									dx_mat->emissiveColor.blue );

	if (strlen(dx_mat->textureFilename.filename) > 0) {
		ffp( tabs+1, "TextureFilename {\n" );
		ffp( tabs+2, "\"%s\";\n", dx_mat->textureFilename.filename );
		ffp( tabs+1, "}\n" );
	}

	ffp( tabs, "}\n" );
}




/*
 *
 *  BIN_WriteMaterial   - write the material
 *
 */

void dxExporter::BIN_WriteMaterial( DX_Material *dx_mat )
{
	if (_options->outputMaterials == FALSE) return;

	switch (_options->materialType) {
	case DX_MATTYPE_INLINE:
		ffp_NAME  ( "Material" );
		ffp_TOKEN ( TOKEN_OBRACE );
		break;
	case DX_MATTYPE_EXTERN:
		ffp_NAME  ( "Material" );
		ffp_NAME  ( dx_mat->nodeName );
		ffp_TOKEN ( TOKEN_OBRACE );
		break;
	case DX_MATTYPE_SHARED:
		ffp_NAME  ( "Material" );
		ffp_NAME  ( dx_mat->surfName );
		ffp_TOKEN ( TOKEN_OBRACE );
		break;
	}

	ffp_TOKEN ( TOKEN_FLOAT_LIST );
	ffp_dword ( 11 );

	ffp_float ( dx_mat->faceColor.red   );
	ffp_float ( dx_mat->faceColor.green );
	ffp_float ( dx_mat->faceColor.blue  );
	ffp_float ( dx_mat->faceColor.alpha );

	ffp_float ( dx_mat->power );

	ffp_float ( dx_mat->specularColor.red   );
	ffp_float ( dx_mat->specularColor.green );
	ffp_float ( dx_mat->specularColor.blue  );

	ffp_float ( dx_mat->emissiveColor.red   );
	ffp_float ( dx_mat->emissiveColor.green );
	ffp_float ( dx_mat->emissiveColor.blue  );

	if (strlen(dx_mat->textureFilename.filename) > 0) {
		ffp_NAME  ( "TextureFilename" );
		ffp_TOKEN ( TOKEN_OBRACE );
		ffp_STRING( dx_mat->textureFilename.filename, TOKEN_SEMICOLON );
		ffp_TOKEN ( TOKEN_CBRACE );
	}

	ffp_TOKEN ( TOKEN_CBRACE );
}
#endif
