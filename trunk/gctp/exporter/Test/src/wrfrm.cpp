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
#include <gctp/vector.hpp>

#include "dxexp.h"

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
