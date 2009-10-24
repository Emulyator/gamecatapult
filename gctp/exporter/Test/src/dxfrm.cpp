/*********************************************************************/
/*                                                                   */
/* FILE :        dxfrm.cpp                                           */
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
#include "dxvec.h"

#define USE_PIVOT(p)	((p)[0]!=0.0 || (p)[1]!=0.0 || (p)[2]!=0.0)


/*
 *
 *  MakeRootFrame   - make the root frame
 *
 */

DX_Frame* dxExporter::MakeRootFrame()
{
	DX_STRING     tempName;

	DX_Frame*  dx_frm = new DX_Frame;
	dx_frm->idx       = 0;
	dx_frm->frameType = DX_FRAME_ROOT;
	sprintf( tempName, "%s_SCENE_ROOT", _options->framePrefix );
	DX_MakeNodeName( dx_frm->nodeName, tempName );
	dx_frm->frameTransformMatrix = MakeFrameMatrix( NULL );

	dx_frm->mesh      = NULL;
	dx_frm->parent    = NULL;
	dx_frm->child     = NULL;
	dx_frm->sibling   = NULL;
	dx_frm->next      = NULL;

	return dx_frm;
}


/*
 *
 *  MakeObjectFrame   - make frames from the object file.
 *
 */

DX_Frame* dxExporter::MakeObjectFrame( int obj )
{
	DX_STRING            nodeName, tempName;

	DX_Frame *dx_root = _sceneRoot;
	DX_Frame *dx_lst  = _sceneRoot;
	DX_Frame *dx_frm;

	int  num_layer = globals_.getObjectFuncs()->maxLayers( obj );

	for (int n = 0; n < num_layer; n++)
	{
		if(globals_.getObjectFuncs()->layerExists( obj, n )) {
			LWW::MeshInfo mesh = globals_.layerMesh( obj, n );
			dx_frm = new DX_Frame;
			dx_frm->idx                  = dx_lst ? dx_lst->idx+1 : 0;
			dx_frm->frameType            = DX_FRAME_MESH;
			dx_frm->frameTransformMatrix = MakeFrameMatrix( NULL );
			dx_frm->mesh                 = MakeMesh( mesh );
			dx_frm->parent               = NULL;
			dx_frm->child                = NULL;
			dx_frm->sibling              = NULL;
			dx_frm->next                 = NULL;

			DX_FileUniqName( tempName, mesh.filename() );
			if(mesh.name()) _snprintf( nodeName, sizeof(nodeName), "%s%d_%s_%s", _options->framePrefix, dx_frm->idx, tempName, mesh.name() );
			else _snprintf( nodeName, sizeof(nodeName), "%s%d_%s_Layer%d", _options->framePrefix, dx_frm->idx, tempName, n+1 );
			DX_MakeNodeName( dx_frm->nodeName, nodeName );

			if (dx_lst) {
				dx_lst->next = dx_frm;
				if (dx_lst != dx_root) {
					dx_lst->sibling = dx_frm;
				}
			}
			if (dx_root->child == NULL) {
				dx_root->child = dx_frm;
				dx_frm->parent = dx_root;
			}
			dx_lst = dx_frm;
		}
	}

	return dx_frm;
}



/*
 *
 *  MakeItemFrame   - make a frame with the scene item
 *
 */

DX_Frame* dxExporter::MakeItemFrame( LWW::Item item )
{
	DX_STRING     nodeName, tempName;

	DX_Frame*  dx_pvt = NULL;
	DX_Frame*  dx_lst = _sceneRoot;
	while (dx_lst->next) { dx_lst = dx_lst->next; }

	DX_Frame*  dx_frm = new DX_Frame;
	dx_frm->idx      = dx_lst ? dx_lst->idx+1 : 0;
	dx_frm->item     = item;
	dx_frm->mesh     = NULL;
	dx_frm->parent   = NULL;
	dx_frm->child    = NULL;
	dx_frm->sibling  = NULL;
	dx_frm->next     = NULL;

	switch (item.type()) {
	case LWI_OBJECT:
		if (item.toObject().numPoints() > 0)
			dx_frm->frameType = DX_FRAME_MESH;
		else
			dx_frm->frameType = DX_FRAME_NULL;
		break;
	case LWI_LIGHT:
		dx_frm->frameType = DX_FRAME_LIGHT;
		break;
	case LWI_CAMERA:
		dx_frm->frameType = DX_FRAME_CAMERA;
		break;
	case LWI_BONE:
		dx_frm->frameType = DX_FRAME_BONE;
		break;
	}

	dx_frm->frameTransformMatrix = MakeFrameMatrix( dx_frm );
	CalcFrameMatrix( dx_frm );

	DX_FileBodyName( tempName, (char *)item.name() );
	sprintf( nodeName, "%s%d_%s", _options->framePrefix, dx_frm->idx, tempName );
	DX_MakeNodeName( dx_frm->nodeName, nodeName );

	if (dx_lst) dx_lst->next = dx_frm;

										/*   Extra Frame for PIVOT  */

	LWDVector pivotPosition;
	item.param(LWIP_PIVOT, 0, pivotPosition);
	if (!_options->useMatrixKey && USE_PIVOT(pivotPosition))
	{
		dx_pvt = new DX_Frame;
		dx_pvt->idx                  = dx_frm->idx + 1;
		dx_pvt->item                 = item;
		dx_pvt->frameType            = DX_FRAME_PIVOT;
		dx_pvt->frameTransformMatrix = MakeFrameMatrix( dx_pvt );
		dx_pvt->mesh     = NULL;
		dx_pvt->parent   = dx_frm;
		dx_pvt->child    = NULL;
		dx_pvt->sibling  = NULL;
		dx_pvt->next     = NULL;
		CalcFrameMatrix( dx_pvt );
		DX_FileBodyName( tempName, (char *)item.name() );
		sprintf( nodeName, "%s%d_%s_Pivot", _options->framePrefix, dx_pvt->idx, tempName );
		DX_MakeNodeName( dx_pvt->nodeName, nodeName );
		dx_frm->mesh    = NULL;
		dx_frm->sibling = NULL;
		dx_frm->next    = dx_pvt;
		dx_frm->child   = dx_pvt;
	}

	if (dx_frm->frameType == DX_FRAME_MESH)
	{
		if (dx_pvt) {
			dx_pvt->mesh = MakeMesh( item.toObject().meshInfo( TRUE ) );
		} else {
			dx_frm->mesh = MakeMesh( item.toObject().meshInfo( TRUE ) );
		}
	}

	return dx_frm;
}


/*
 *
 *  LinkFrames    - link parents, siblings and children
 *
 */

void dxExporter::LinkFrames()
{
	DX_Frame*     dx_next;
	DX_Frame*     dx_prnt;
	DX_Frame*     dx_root = _sceneRoot;
	DX_Frame*     dx_frm  = _sceneRoot;

	while (dx_frm)
	{
		if (dx_frm->parent == NULL && dx_frm->item)
		{
			dx_prnt = ItemHiererchy( dx_root, dx_frm->item.parent() );
			dx_frm->parent = dx_prnt;
		}
		if (dx_frm->parent && dx_frm->parent->child == NULL) {
			dx_frm->parent->child = dx_frm;
		}
		dx_frm = dx_frm->next;
	}

	dx_frm  = _sceneRoot;

	while (dx_frm)
	{
		if (dx_frm->sibling == NULL && dx_frm->next)
		{
			dx_next = dx_frm->next;
			dx_frm->sibling = SiblingHiererchy( dx_next, dx_frm->parent );
		}
		dx_frm = dx_frm->next;
	}
}


/*
 *
 *  DisposeFrames   - dispose frams
 *
 */

void dxExporter::DisposeFrames( DX_Frame **dx_frms )
{
	DX_Frame*  dx_frm = *dx_frms;
	DX_Frame*  temp;

	while (dx_frm)
	{
		temp = dx_frm->next;
		delete dx_frm->frameTransformMatrix;
		DisposeMeshes( &dx_frm->mesh );
		delete dx_frm;
		dx_frm = temp;
	}

	*dx_frms = NULL;
}


/*
 *
 *  CalcFrameMatrix   - calc the frameTransformMatrix
 *
 */

void dxExporter::CalcFrameMatrix( DX_Frame *dx_frm )
{
	DX_FrameTransformMatrix*   ftm = dx_frm->frameTransformMatrix;
	double                     pos[3], rot[3], scl[3], pvt[3];

	DX_MatrixIdentity ( ftm->frameMatrix.matrix );
	if (dx_frm->item == NULL) return;

	switch (dx_frm->frameType) {
	case DX_FRAME_PIVOT:
		dx_frm->item.param(LWIP_PIVOT, 0.0, pvt );
		DX_MatrixTranslation( ftm->frameMatrix.matrix, (DX_FLOAT) -pvt[0],
													   (DX_FLOAT) -pvt[1],
													   (DX_FLOAT) -pvt[2] );
		break;
	case DX_FRAME_NULL:
	case DX_FRAME_MESH:
		if (_options->useMatrixKey) {
			dx_frm->item.param( LWIP_PIVOT, 0.0, pvt );
			DX_MatrixTranslation( ftm->frameMatrix.matrix, (DX_FLOAT) -pvt[0],
														   (DX_FLOAT) -pvt[1],
														   (DX_FLOAT) -pvt[2] );
		}
		dx_frm->item.param( LWIP_SCALING,   0.0, scl );
		dx_frm->item.param( LWIP_ROTATION,  0.0, rot );
		dx_frm->item.param( LWIP_PIVOT_ROT, 0.0, pvt );
		dx_frm->item.param( LWIP_POSITION,  0.0, pos );
		DX_MatrixScaling    ( ftm->frameMatrix.matrix, (DX_FLOAT) scl[0],
													   (DX_FLOAT) scl[1],
													   (DX_FLOAT) scl[2] );
		DX_MatrixRoationZ   ( ftm->frameMatrix.matrix, (DX_FLOAT) rot[2] );
		DX_MatrixRoationX   ( ftm->frameMatrix.matrix, (DX_FLOAT) rot[1] );
		DX_MatrixRoationY   ( ftm->frameMatrix.matrix, (DX_FLOAT) rot[0] );
		DX_MatrixRoationZ   ( ftm->frameMatrix.matrix, (DX_FLOAT) pvt[2] );
		DX_MatrixRoationX   ( ftm->frameMatrix.matrix, (DX_FLOAT) pvt[1] );
		DX_MatrixRoationY   ( ftm->frameMatrix.matrix, (DX_FLOAT) pvt[0] );
		DX_MatrixTranslation( ftm->frameMatrix.matrix, (DX_FLOAT) pos[0],
													   (DX_FLOAT) pos[1],
													   (DX_FLOAT) pos[2] );
		break;
	case DX_FRAME_BONE:
		{
			double pivotPosition[3], pivotRotation[3];
			double restPosition[3], restDirection[3];

			dx_frm->item.param( LWIP_PIVOT    , 0.0, pivotPosition );
			dx_frm->item.param( LWIP_PIVOT_ROT, 0.0, pivotRotation );
			dx_frm->item.toBone().restParam( LWIP_POSITION, restPosition );
			dx_frm->item.toBone().restParam( LWIP_ROTATION, restDirection );
			DX_MatrixIdentity   ( ftm->frameMatrix.matrix );
			DX_MatrixTranslation( ftm->frameMatrix.matrix,	(DX_FLOAT)-pivotPosition[0],
															(DX_FLOAT)-pivotPosition[1],
															(DX_FLOAT)-pivotPosition[2] );
			DX_MatrixRoationZ   ( ftm->frameMatrix.matrix,  (DX_FLOAT)restDirection[2] );
			DX_MatrixRoationX   ( ftm->frameMatrix.matrix,  (DX_FLOAT)restDirection[1] );
			DX_MatrixRoationY   ( ftm->frameMatrix.matrix,  (DX_FLOAT)restDirection[0] );
			DX_MatrixRoationZ   ( ftm->frameMatrix.matrix,  (DX_FLOAT)pivotRotation[2] );
			DX_MatrixRoationX   ( ftm->frameMatrix.matrix,  (DX_FLOAT)pivotRotation[1] );
			DX_MatrixRoationY   ( ftm->frameMatrix.matrix,  (DX_FLOAT)pivotRotation[0] );
			DX_MatrixTranslation( ftm->frameMatrix.matrix,  (DX_FLOAT)restPosition[0], 
					   										(DX_FLOAT)restPosition[1], 
					   										(DX_FLOAT)restPosition[2] );
		}
		break;
	case DX_FRAME_LIGHT:
	case DX_FRAME_CAMERA:
		dx_frm->item.param( LWIP_POSITION, 0.0, pos );
		dx_frm->item.param( LWIP_ROTATION, 0.0, rot );
		DX_MatrixRoationZ   ( ftm->frameMatrix.matrix, (DX_FLOAT) rot[2] );
		DX_MatrixRoationX   ( ftm->frameMatrix.matrix, (DX_FLOAT) rot[1] );
		DX_MatrixRoationY   ( ftm->frameMatrix.matrix, (DX_FLOAT) rot[0] );
		DX_MatrixTranslation( ftm->frameMatrix.matrix, (DX_FLOAT) pos[0],
													   (DX_FLOAT) pos[1],
													   (DX_FLOAT) pos[2] );
		break;
	}
}


/*
 *
 *  ItemHiererchy   - return DX_Frame which has the same item
 *
 */

DX_Frame* dxExporter::ItemHiererchy( DX_Frame* dx_frm, LWW::Item item )
{
	if(!item) return dx_frm;

	while (dx_frm)
	{
		if (dx_frm->item == item) {
			if (dx_frm->next && dx_frm->next->item == item) {
				return dx_frm->next;
			} else {
				return dx_frm;
			}
		}
		dx_frm  = dx_frm->next;
	}

	return NULL;
}


/*
 *
 *  SiblingHiererchy   - return DX_Frame which has the same parent
 *
 */

DX_Frame* dxExporter::SiblingHiererchy( DX_Frame* dx_frm, DX_Frame* parent )
{
	while (dx_frm)
	{
		if (dx_frm->parent == parent) return dx_frm;
		dx_frm = dx_frm->next;
	}

	return NULL;
}


/*
 *
 *  MakeFrameMatrix   - make the frameTransformMatrix
 *
 */

DX_FrameTransformMatrix* dxExporter::MakeFrameMatrix( DX_Frame* dx_frm )
{
	DX_FrameTransformMatrix*  ftm = new DX_FrameTransformMatrix;
	DX_MatrixIdentity( ftm->frameMatrix.matrix );

	return ftm;
}

