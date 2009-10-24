/*********************************************************************/
/*                                                                   */
/* FILE :        dxopt.cpp                                           */
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



/*
 *  dxOptions       - constructor
 */
dxOptions::dxOptions( DX_STATUS dx_status )
{
	status                    = dx_status;
	scale                     = 1.0;

	strcpy( outputFile,  "Unnamed.x" );
	strcpy( exportDirectory, "" );
	strcpy( textureDirectory, "" );

	//  Export
	version                   = DX_VERSION_DIRECTX9;
	format                    = DX_FORMAT_RTEXT;
	outputMaterials           = TRUE;
	materialType              = DX_MATTYPE_INLINE;
	outputFrames              = TRUE;
	strcpy( framePrefix, "Frame" );

	//  Mesh
	outputMeshNormals         = TRUE;
	outputTextureCoords       = TRUE;
	outputVertexColors        = FALSE;
	reverseUVCoords           = FALSE;
	useSkinWeightsPerVertex   = FALSE;
	nMaxSkinWeightsPerVertex  = 4;
	nMaxSkinWeightsPerFace    = 6;
	tripleAll                 = FALSE;

	//  Material
	enbaleDecalTexture        = TRUE;

	//  Texture
	outputTextureImages       = DX_TXTOUT_USEORGIN;
	strcpy( imageExtension, "" );
	imageSaver                = 0;
	resizeTextures            = 0;
	textureMaxResX            = 256;
	textureMaxResY            = 256;
	textureFilename           = DX_TXTNAME_BODYNAME;

	//  Animation
	outputAnimations          = TRUE;
	frameToExport             = DX_FRM2EXP_KEYANDSAMPLE;
	frameSteps                = 1;
	removeUnusableKeyframes   = FALSE;
	frameTimeScale            = 1;
	useMatrixKey              = FALSE;
	outputCameraFrames        = FALSE;
	outputLightFrames         = FALSE;
	frameTimeScale            = 1;
	bakeIKMotions             = TRUE;

	//  Preview
	enableViewer              = FALSE;
	strcpy( viewerName, "" );
}


/*
 *  ~dxOptions      - destructor
 */
dxOptions::~dxOptions()
{
}

