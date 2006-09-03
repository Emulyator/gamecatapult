/*********************************************************************/
/*                                                                   */
/* FILE :        wrfrm.cpp                                           */
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

#ifdef GCTP_USE_XFILEAPI
#include <rmxfguid.h>

/*
 *
 *  WriteFrames   - write the frame data
 *
 */

void dxExporter::WriteFrames( DX_Frame *dx_frms, gctp::XSaver &cur )
{
	if (_options->outputFrames == FALSE) return;

	gctp::XSaveData _frame = cur.newData(TID_D3DRMFrame, 0, NULL, dx_frms->nodeName);
	_frame.newData(TID_D3DRMFrameTransformMatrix, sizeof(DX_FrameTransformMatrix), &dx_frms->frameTransformMatrix->frameMatrix);
	
	if (dx_frms->mesh) {
		WriteMesh( dx_frms->mesh, _frame );
	}

	DX_Frame *child = dx_frms->child;
	while (child) {
		WriteFrames( child, _frame );
		child = (child->sibling == dx_frms->child) ? NULL : child->sibling;
	}
}

#else
/*
 *
 *  WriteFrames   - write the frame data
 *
 */

void dxExporter::WriteFrames( DX_Frame *dx_frms, int tabs )
{
	if (_options->outputFrames == FALSE) return;

	switch (_options->format) {
	case DX_FORMAT_BINARY:
		BIN_WriteFrames( dx_frms );
		break;
	case DX_FORMAT_TEXT:
		TXT_WriteFrames( dx_frms, tabs );
		break;
	}
}

/*
 *
 *  TXT_WriteFrames   - write the frame data
 *
 */

void dxExporter::TXT_WriteFrames( DX_Frame *dx_frms, int tabs )
{
	DX_Frame     *child;

	if (_options->outputFrames == FALSE) return;

	ffp( 0, "\n" );
	ffp( tabs, "Frame %s {\n", dx_frms->nodeName );

	ffp( 0, "\n" );
	ffp( tabs+1, "FrameTransformMatrix {\n" );
	ffp( tabs+2, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f;;\n",
			dx_frms->frameTransformMatrix->frameMatrix.matrix[ 0],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[ 1],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[ 2],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[ 3],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[ 4],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[ 5],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[ 6],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[ 7],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[ 8],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[ 9],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[10],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[11],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[12],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[13],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[14],
			dx_frms->frameTransformMatrix->frameMatrix.matrix[15] );
	ffp( tabs+1, "}\n" );

	if (dx_frms->mesh) {
		TXT_WriteMesh( dx_frms->mesh, tabs+1 );
	}

	child = dx_frms->child;
	while (child) {
		TXT_WriteFrames( child, tabs+1 );
		child = (child->sibling == dx_frms->child) ? NULL : child->sibling;
	}

	ffp( tabs, "}\n" );
}




/*
 *
 *  BIN_WriteFrames   - write the frame data
 *
 */

void dxExporter::BIN_WriteFrames( DX_Frame *dx_frms )
{
	DX_Frame     *child;

	if (_options->outputFrames == FALSE) return;

	ffp_NAME  ( "Frame" );
	ffp_NAME  ( dx_frms->nodeName );
	ffp_TOKEN ( TOKEN_OBRACE );

	ffp_NAME  ( "FrameTransformMatrix" );
	ffp_TOKEN ( TOKEN_OBRACE );
	ffp_TOKEN ( TOKEN_FLOAT_LIST );
	ffp_dword ( 4 );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[ 0] );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[ 1] );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[ 2] );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[ 3] );
	ffp_TOKEN ( TOKEN_FLOAT_LIST );
	ffp_dword ( 4 );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[ 4] );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[ 5] );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[ 6] );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[ 7] );
	ffp_TOKEN ( TOKEN_FLOAT_LIST );
	ffp_dword ( 4 );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[ 8] );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[ 9] );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[10] );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[11] );
	ffp_TOKEN ( TOKEN_FLOAT_LIST );
	ffp_dword ( 4 );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[12] );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[13] );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[14] );
	ffp_float ( dx_frms->frameTransformMatrix->frameMatrix.matrix[15] );
	ffp_TOKEN ( TOKEN_CBRACE );

	if (dx_frms->mesh) {
		BIN_WriteMesh( dx_frms->mesh );
	}

	child = dx_frms->child;
	while (child) {
		BIN_WriteFrames( child );
		child = (child->sibling == dx_frms->child) ? NULL : child->sibling;
	}

	ffp_TOKEN ( TOKEN_CBRACE );
}



#endif
