/*********************************************************************/
/*                                                                   */
/* FILE :        dxopt.h                                             */
/*                                                                   */
/* DESCRIPTION : LightWave 3D to DirectX Export                      */
/*                                                                   */
/* HISTORY :     Oct 29, 2000    written by Yoshiaki Tazaki          */
/*               Oct 02, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/

#ifndef _DXOPT_H_
#define _DXOPT_H_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "scn3.h"
#include "meshio.h"
#include "dxtyp.h"
#include "dxutl.h"

extern "C" {
#include <lwio.h>
#include <lwhost.h>
#include <lwserver.h>
#include <lwimage.h>
#include <lwmodeler.h>
#include <lwrender.h>
#include <lwmeshedt.h>
}

#ifdef _MACOS
typedef int bool;
#endif


typedef enum {
	DX_STATUS_MODELER = 0,
	DX_STATUS_LAYOUT  = 1,
	DX_STATUS_OBJ2DX  = 2,
	DX_STATUS_SCN2DX  = 3
} DX_STATUS;

typedef int                       DX_VERSION;
#ifdef GCTP_USE_XFILEAPI
#define DX_VERSION_DIRECTX9       0
#else
#define DX_VERSION_DIRECTX7       0
#define DX_VERSION_DIRECTX8       1
#endif

typedef int                       DX_FORMAT;
#ifdef GCTP_USE_XFILEAPI
#define DX_FORMAT_RTEXT           0
#define DX_FORMAT_CTEXT           1
#define DX_FORMAT_RBINARY         2
#define DX_FORMAT_CBINARY         3
#else
#define DX_FORMAT_TEXT            0
#define DX_FORMAT_BINARY          1
#endif

typedef int                       DX_MATTYPE;
#define DX_MATTYPE_INLINE         0
#define DX_MATTYPE_EXTERN         1
#define DX_MATTYPE_SHARED         2

typedef int                       DX_MODE;
#define DX_MODE_OBJECT            0
#define DX_MODE_SCENE             1

typedef int                       DX_FRM2EXP;
#define DX_FRM2EXP_KEYFRAMEONLY   0
#define DX_FRM2EXP_ALWAYSSAMPLE   1
#define DX_FRM2EXP_KEYANDSAMPLE   2

typedef int                       DX_TXTNAME;
#define DX_TXTNAME_BODYNAME       0
#define DX_TXTNAME_ABSOLUTE       1
#define DX_TXTNAME_RELATIVE       2

typedef int                       DX_TXTOUT;
#define DX_TXTOUT_USEORGIN        0
#define DX_TXTOUT_CONVERT         1
#define DX_TXTOUT_FILECOPY        2


									//  Scene Graph

class dxOptions
{
public:
	dxOptions ( DX_STATUS dx_status );
	~dxOptions();

	//  Load and Save Settings
	void                 LoadConfig ( GlobalFunc *global );
	void                 SaveConfig ( GlobalFunc *global );

	DX_STATUS            status;
	DX_DOUBLE            scale;

	DX_FNAM              outputFile;
	DX_FNAM              exportDirectory;
	DX_FNAM              textureDirectory;

	//  Export
	DX_VERSION           version;
	DX_FORMAT            format;
	DX_BOOL              outputMaterials;
	DX_MATTYPE           materialType;
	DX_BOOL              outputFrames;
	DX_STRING            framePrefix;
#ifndef GCTP_USE_XFILEAPI
	DX_BOOL              outputTemplates;
#endif

	//  Mesh
	DX_BOOL              outputMeshNormals;
	DX_BOOL              outputTextureCoords;
	DX_BOOL              outputVertexColors;
	DX_BOOL              reverseUVCoords;
	DX_INT               useSkinWeightsPerVertex;
	DX_INT               nMaxSkinWeightsPerVertex;
	DX_INT               nMaxSkinWeightsPerFace;
	DX_BOOL              tripleAll;

	//  Material
	DX_BOOL              enbaleDecalTexture;

	//  Texture
	DX_TXTOUT            outputTextureImages;
	DX_STRING            imageExtension;
	DX_INT               imageSaver;			// only for plugins
	DX_INT               resizeTextures;		// only for plugins
	DX_INT               textureMaxResX;		// only for plugins
	DX_INT               textureMaxResY;		// only for plugins
	DX_TXTNAME           textureFilename;

	//  Animation
	DX_BOOL              outputAnimations;
	DX_FRM2EXP           frameToExport;
	DX_INT               frameSteps;
	DX_INT               removeUnusableKeyframes;
	DX_INT               frameTimeScale;
	DX_INT               useMatrixKey;
	DX_BOOL              outputCameraFrames;
	DX_BOOL              outputLightFrames;
	DX_BOOL              bakeIKMotions;

	//  Preview
	DX_BOOL              enableViewer;
	DX_STRING            viewerName;

private:

};

#endif /* _DXOPT_H_  */

