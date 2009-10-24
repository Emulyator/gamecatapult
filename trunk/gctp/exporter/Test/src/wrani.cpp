/*********************************************************************/
/*                                                                   */
/* FILE :        wrani.cpp                                           */
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


#include <gctp/bbstream.hpp>
#include <rmxfguid.h>

/*
 *
 *  WriteAnimTicksPerSecond - write the animation set
 *
 */

void dxExporter::WriteAnimTicksPerSecond( gctp::XSaver &cur )
{
	if (_options->outputAnimations == FALSE) return;
	cur.newData(DXFILEOBJ_AnimTicksPerSecond, sizeof(DX_DWORD), &_ticksPerSec);
}

/*
 *
 *  WriteAnimationSet   - write the animation set
 *
 */

void dxExporter::WriteAnimationSet( DX_AnimationSet* dx_set, gctp::XSaver &cur )
{
	if (dx_set == NULL) return;
	if (_options->outputAnimations == FALSE) return;
	if (dx_set->animations == NULL) return;

	gctp::XSaveData animset = cur.newData(TID_D3DRMAnimationSet, 0, NULL, dx_set->nodeName);
	
	for(DX_Animation *dx_ani = dx_set->animations; dx_ani; dx_ani = dx_ani->next)
	{
		WriteAnimation( dx_ani, animset );
	}
}

/*
 *
 *  WriteAnimation   - write the animation
 *
 */

void dxExporter::WriteAnimation( DX_Animation* dx_ani, gctp::XSaver &cur )
{
	DX_AnimationKey*    dx_key;

	if (dx_ani == NULL) return;
	if (dx_ani->animationKey == NULL) return;

	gctp::XSaveData _anim = cur.newData(TID_D3DRMAnimation, 0, NULL, dx_ani->nodeName);
	_anim.addRefData(dx_ani->frame->nodeName);

	dx_key = dx_ani->animationKey;
	while (dx_key) {
		WriteAnimationKey( dx_key, _anim );
		dx_key = dx_key->next;
	}

	WriteAnimationOptions( dx_ani->animationOptions, _anim );
}


/*
 *
 *  WriteAnimationOptions   - write the animation options
 *
 */

void dxExporter::WriteAnimationOptions( DX_AnimationOptions* dx_opt, gctp::XSaver &cur )
{
	if (dx_opt == NULL) return;

	cur.newData(TID_D3DRMAnimationOptions, sizeof(DX_AnimationOptions), dx_opt);
}


/*
 *
 *  WriteAnimationKey   - write the animation keys
 *
 */

void dxExporter::WriteAnimationKey( DX_AnimationKey* dx_key, gctp::XSaver &cur )
{
	DX_DWORD        n;

	if (dx_key == NULL) return;

	gctp::bbstream strm;
	strm << dx_key->keyType << dx_key->nKeys;
	for (n = 0; n < dx_key->nKeys; n++) 
	{
		switch (dx_key->keyType)
		{
		case DX_ANIM_ROTATEKEY:
			strm << dx_key->keys[n].time * _options->frameTimeScale
				 << dx_key->keys[n].tfkeys.nValues
				 << dx_key->keys[n].tfkeys.values[0]
				 << dx_key->keys[n].tfkeys.values[1]
				 << dx_key->keys[n].tfkeys.values[2]
				 << dx_key->keys[n].tfkeys.values[3];
			break;
		case DX_ANIM_SCALEKEY:
			strm << dx_key->keys[n].time * _options->frameTimeScale
				 << dx_key->keys[n].tfkeys.nValues
				 << dx_key->keys[n].tfkeys.values[0]
				 << dx_key->keys[n].tfkeys.values[1]
				 << dx_key->keys[n].tfkeys.values[2];
			break;
		case DX_ANIM_POSITIONKEY:
			strm << dx_key->keys[n].time * _options->frameTimeScale
				 << dx_key->keys[n].tfkeys.nValues
				 << dx_key->keys[n].tfkeys.values[0]
				 << dx_key->keys[n].tfkeys.values[1]
				 << dx_key->keys[n].tfkeys.values[2];
			break;
		case DX_ANIM_MATRIXKEY:
			strm << dx_key->keys[n].time * _options->frameTimeScale
				 << dx_key->keys[n].tfkeys.nValues
				 << dx_key->keys[n].tfkeys.values[ 0]
				 << dx_key->keys[n].tfkeys.values[ 1]
				 << dx_key->keys[n].tfkeys.values[ 2]
				 << dx_key->keys[n].tfkeys.values[ 3]
				 << dx_key->keys[n].tfkeys.values[ 4]
				 << dx_key->keys[n].tfkeys.values[ 5]
				 << dx_key->keys[n].tfkeys.values[ 6]
				 << dx_key->keys[n].tfkeys.values[ 7]
				 << dx_key->keys[n].tfkeys.values[ 8]
				 << dx_key->keys[n].tfkeys.values[ 9]
				 << dx_key->keys[n].tfkeys.values[10]
				 << dx_key->keys[n].tfkeys.values[11]
				 << dx_key->keys[n].tfkeys.values[12]
				 << dx_key->keys[n].tfkeys.values[13]
				 << dx_key->keys[n].tfkeys.values[14]
				 << dx_key->keys[n].tfkeys.values[15];
			break;
		}
	}
	
	cur.newData(TID_D3DRMAnimationKey, strm.pcount(), strm.buf());
}
