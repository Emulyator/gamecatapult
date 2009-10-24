/*********************************************************************/
/*                                                                   */
/* FILE :        dxani.cpp                                           */
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
#include "mathlib.hpp"

#include <gctp/dbgout.hpp>

#define FP_EQUAL(a,b)	(fabs((a)-(b)) < EPSILON)

/*
 *
 *  MakeAnimationSet   - make an animation set
 *
 */

DX_AnimationSet* dxExporter::MakeAnimationSet()
{
	DX_STRING            nodeName, tempName;

	DX_AnimationSet*  dx_set = new DX_AnimationSet;
	dx_set->animations = NULL;
	DX_FileUniqName( tempName, _options->outputFile );
	sprintf( nodeName, "AnimationSet_%s", tempName );
	DX_MakeNodeName( dx_set->nodeName, nodeName );
	dx_set->animations = NULL;

	return dx_set;
}


/*
 *
 *  MakeAnimation   - make an animation for the frame
 *
 */

DX_Animation* dxExporter::MakeAnimation( DX_Frame* frame )
{
	DX_AnimationSet*     dx_set = _animationSet;
	DX_Animation*        dx_lst = NULL;

	if (dx_set->animations) {
		dx_lst = dx_set->animations;
		while (dx_lst->next) { dx_lst = dx_lst->next; }
	}

	DX_Animation*  dx_ani = new DX_Animation;
	dx_ani->idx               = dx_lst ? dx_lst->idx+1 : 0;
	dx_ani->frame             = frame;
	dx_ani->animationKey      = NULL;
	dx_ani->animationOptions  = NULL;
	dx_ani->next              = NULL;

	MakeAnimationKey( dx_ani );

	sprintf( dx_ani->nodeName, "Animation%d", dx_ani->idx );

	if (dx_lst) dx_lst->next       = dx_ani;
	else        dx_set->animations = dx_ani;

	return dx_ani;
}


/*
 *
 *  MakeAnimationOptions   - make an animation options
 *
 */

DX_AnimationOptions* dxExporter::MakeAnimationOptions()
{
	DX_AnimationOptions*  dx_opt = new DX_AnimationOptions;
	dx_opt->openclosed      = 0;
	dx_opt->positionquality = 0;

	return dx_opt;
}


/*
 *
 *  MakeAnimationKey   - make an animation key
 *
 */

DX_AnimationKey* dxExporter::MakeAnimationKey( DX_Animation* dx_ani )
{
	DX_Frame*  frame  = dx_ani->frame;

	if (_options->useMatrixKey)
	{
		MakeMatrixKey( dx_ani );
	}
	else
	{
		switch (frame->frameType) {
		case DX_FRAME_LIGHT:
		case DX_FRAME_CAMERA:
			MakeRotateKey  ( dx_ani );
			MakePositionKey( dx_ani );
			break;
		default:
			MakeRotateKey  ( dx_ani );
			MakeScaleKey   ( dx_ani );
			MakePositionKey( dx_ani );
			break;
		}
	}

	return dx_ani->animationKey;
}

/*
 *
 *  GetRotateValue     - get the rotate value
 *
 */

int dxExporter::GetRotateValue( LWW::Item item, DX_DWORD frame, DX_FLOAT* vals )
{
	EulerAngles             euler;
	Quat                    quat, quat1, quat2;
	double                  rot[3], pvt[3];

	item.param( LWIP_ROTATION, FRAME2TIME(frame), rot );
	euler = mat4_euler   ( (float)-rot[0], (float)-rot[1], (float)-rot[2], EulOrdYXZs );
	quat1 = mat4_eul2quat( euler );
	item.param( LWIP_PIVOT_ROT, 0.0f, pvt );
	euler = mat4_euler   ( (float)-pvt[0], (float)-pvt[1], (float)-pvt[2], EulOrdYXZs );
	quat2 = mat4_eul2quat( euler );
	quat  = mat4_multquat( quat1, quat2 );
	vals[0] = (DX_FLOAT) quat.w;
	vals[1] = (DX_FLOAT) quat.x;
	vals[2] = (DX_FLOAT) quat.y;
	vals[3] = (DX_FLOAT) quat.z;

	PRNN("rot key " << vals[0] << ", " << vals[1] << ", " << vals[2] << ", " << vals[3]);
	return 4;
}


/*
 *
 *  MakeRotateKey   - make rotation key
 *
 */

DX_AnimationKey* dxExporter::MakeRotateKey( DX_Animation* dx_ani )
{
	DX_AnimationSet*        dx_set = _animationSet;
	LWW::Item               item   = dx_ani->frame->item;

	DX_DWORD firstFrame = (globals_.getInterfaceInfo()->previewStart < 0) ? 0 : globals_.getInterfaceInfo()->previewStart;
	DX_DWORD lastFrame  = (globals_.getInterfaceInfo()->previewEnd  < 0) ? 0 : globals_.getInterfaceInfo()->previewEnd;

	const char *ch[3];
	ch[0] = CHNAME_HEADING;
	ch[1] = CHNAME_PITCH;
	ch[2] = CHNAME_BANK;

	DX_AnimationKey*  dx_key = new DX_AnimationKey;
	dx_key->keyType = DX_ANIM_ROTATEKEY;
	dx_key->nKeys   = 0;
	dx_key->keys    = NULL;
	dx_key->next    = NULL;

	DX_DWORD nKeys  = CountKeyframe( item, ch, 3 );
	DX_DWORD frame;

	if (nKeys == 1)
	{
		dx_key->nKeys = 2;
		DX_TimedFloatKeys*  keys = new DX_TimedFloatKeys[dx_key->nKeys];
		dx_key->keys  = keys;

		for (frame = firstFrame; frame <= lastFrame; frame += lastFrame - firstFrame) 
		{
			keys->time           = frame;
			keys->tfkeys.nValues = GetRotateValue( item, frame, keys->tfkeys.values );
			keys++;
		}
	}
	else
	{
		dx_key->nKeys = 0;
		nKeys = lastFrame - firstFrame + 1;
		DX_TimedFloatKeys*  keys  = new DX_TimedFloatKeys[nKeys];
		dx_key->keys  = keys;

		for (frame = firstFrame; frame <= lastFrame; frame++)
		{
			if (TestKeyframe( item, frame, ch, 3 ))
			{
				keys->time           = frame;
				keys->tfkeys.nValues = GetRotateValue( item, frame, keys->tfkeys.values );
				keys++;
				dx_key->nKeys++;
			}
		}

	}

	if (_options->removeUnusableKeyframes)
	{
		if (UsableAnimationKey( dx_key )) {
			delete[] dx_key->keys;
			delete   dx_key;
			return NULL;
		}
	}

	if (dx_ani->animationKey == NULL) {
		dx_ani->animationKey = dx_key;
	} else {
		DX_AnimationKey*  dx_lst = dx_ani->animationKey;
		while (dx_lst->next) { dx_lst = dx_lst->next; }
		dx_lst->next = dx_key;
	}

	return dx_key;
}


/*
 *
 *  GetScaleValue     - get the scale value
 *
 */

int dxExporter::GetScaleValue( LWW::Item item, DX_DWORD frame, DX_FLOAT* vals )
{
	double      scl[3];

	item.param( LWIP_SCALING, FRAME2TIME(frame), scl );
	vals[0] = (DX_FLOAT) scl[0];
	vals[1] = (DX_FLOAT) scl[1];
	vals[2] = (DX_FLOAT) scl[2];

	return 3;
}


/*
 *
 *  MakeScaleKey   - make scaling key
 *
 */

DX_AnimationKey* dxExporter::MakeScaleKey( DX_Animation* dx_ani )
{
	DX_AnimationSet*        dx_set = _animationSet;
	LWW::Item               item   = dx_ani->frame->item;

	DX_DWORD firstFrame = (globals_.getInterfaceInfo()->previewStart < 0) ? 0 : globals_.getInterfaceInfo()->previewStart;
	DX_DWORD lastFrame  = (globals_.getInterfaceInfo()->previewEnd  < 0) ? 0 : globals_.getInterfaceInfo()->previewEnd;

	const char *ch[3];
	ch[0] = CHNAME_XSCALE;
	ch[1] = CHNAME_YSCALE;
	ch[2] = CHNAME_ZSCALE;

	DX_AnimationKey*  dx_key = new DX_AnimationKey;
	dx_key->keyType = DX_ANIM_SCALEKEY;
	dx_key->nKeys   = 0;
	dx_key->keys    = NULL;
	dx_key->next    = NULL;

	DX_DWORD nKeys  = CountKeyframe( item, ch, 3 );
	DX_DWORD frame;

	if (nKeys == 1)
	{
		dx_key->nKeys = 2;
		DX_TimedFloatKeys*  keys = new DX_TimedFloatKeys[dx_key->nKeys];
		dx_key->keys  = keys;

		for (frame = firstFrame; frame <= lastFrame; frame += lastFrame - firstFrame) 
		{
			keys->time           = frame;
			keys->tfkeys.nValues = GetScaleValue( item, frame, keys->tfkeys.values );
			keys++;
		}
	}
	else
	{
		nKeys = lastFrame - firstFrame + 1;
		DX_TimedFloatKeys*  keys  = new DX_TimedFloatKeys[nKeys];
		dx_key->nKeys = 0;
		dx_key->keys  = keys;

		for (frame = firstFrame; frame <= lastFrame; frame++)
		{
			if (TestKeyframe( item, frame, ch, 3 ))
			{
				keys->time           = frame;
				keys->tfkeys.nValues = GetScaleValue( item, frame, keys->tfkeys.values );
				keys++;
				dx_key->nKeys++;
			}
		}

	}

	if (_options->removeUnusableKeyframes)
	{
		if (UsableAnimationKey( dx_key )) {
			delete[] dx_key->keys;
			delete   dx_key;
			return NULL;
		}
	}

	if (dx_ani->animationKey == NULL) {
		dx_ani->animationKey = dx_key;
	} else {
		DX_AnimationKey*  dx_lst = dx_ani->animationKey;
		while (dx_lst->next) { dx_lst = dx_lst->next; }
		dx_lst->next = dx_key;
	}

	return dx_key;
}


/*
 *
 *  GetPositionValue     - get the position value
 *
 */

int dxExporter::GetPositionValue( LWW::Item item, DX_DWORD frame, DX_FLOAT* vals )
{
	double    pos[3];

	item.param( LWIP_POSITION, FRAME2TIME(frame), pos );
	vals[0] = (DX_FLOAT) pos[0];
	vals[1] = (DX_FLOAT) pos[1];
	vals[2] = (DX_FLOAT) pos[2];

	return 3;
}


/*
 *
 *  MakePositionKey   - make translation key
 *
 */

DX_AnimationKey* dxExporter::MakePositionKey( DX_Animation* dx_ani )
{
	DX_AnimationSet*        dx_set = _animationSet;
	LWW::Item               item   = dx_ani->frame->item;

	DX_DWORD firstFrame = (globals_.getInterfaceInfo()->previewStart < 0) ? 0 : globals_.getInterfaceInfo()->previewStart;
	DX_DWORD lastFrame  = (globals_.getInterfaceInfo()->previewEnd  < 0) ? 0 : globals_.getInterfaceInfo()->previewEnd;

	const char *ch[3];
	ch[0] = CHNAME_XPOS;
	ch[1] = CHNAME_YPOS;
	ch[2] = CHNAME_ZPOS;

	DX_AnimationKey*  dx_key = new DX_AnimationKey;
	dx_key->keyType = DX_ANIM_POSITIONKEY;
	dx_key->nKeys   = 0;
	dx_key->keys    = NULL;
	dx_key->next    = NULL;

	DX_DWORD nKeys  = CountKeyframe( item, ch, 3 );
	DX_DWORD frame;

	if (nKeys == 1)
	{
		dx_key->nKeys = 2;
		DX_TimedFloatKeys*  keys = new DX_TimedFloatKeys[dx_key->nKeys];
		dx_key->keys = keys;

		for (frame = firstFrame; frame <= lastFrame; frame += lastFrame - firstFrame) 
		{
			keys->time           = frame;
			keys->tfkeys.nValues = GetPositionValue( item, frame, keys->tfkeys.values );
			keys++;
		}
	}
	else
	{
		nKeys = lastFrame - firstFrame + 1;
		DX_TimedFloatKeys*  keys  = new DX_TimedFloatKeys[nKeys];
		dx_key->nKeys = 0;
		dx_key->keys  = keys;

		for (frame = firstFrame; frame <= lastFrame; frame++)
		{
			if (TestKeyframe( item, frame, ch, 3 ))
			{
				keys->time           = frame;
				keys->tfkeys.nValues = GetPositionValue( item, frame, keys->tfkeys.values );
				keys++;
				dx_key->nKeys++;
			}
		}

	}

	if (_options->removeUnusableKeyframes)
	{
		if (UsableAnimationKey( dx_key )) {
			delete[] dx_key->keys;
			delete   dx_key;
			return NULL;
		}
	}

	if (dx_ani->animationKey == NULL) {
		dx_ani->animationKey = dx_key;
	} else {
		DX_AnimationKey*  dx_lst = dx_ani->animationKey;
		while (dx_lst->next) { dx_lst = dx_lst->next; }
		dx_lst->next = dx_key;
	}

	return dx_key;
}


/*
 *
 *  GetMatrixValue     - get the matrix value
 *
 */

int dxExporter::GetMatrixValue( LWW::Item item, DX_DWORD frame, DX_FLOAT* vals )
{
	DX_FLOAT       data[4], rotmat[16], quat[4];
	double         pvt[3];

	DX_MatrixIdentity( vals );

	if (item.type() == LWI_OBJECT || item.type() == LWI_BONE) {
		item.param( LWIP_PIVOT, 0.0, pvt );
		DX_MatrixTranslation ( vals, (DX_FLOAT) -pvt[0], (DX_FLOAT) -pvt[1], (DX_FLOAT) -pvt[2] );

		GetScaleValue     ( item, frame, data );
		DX_MatrixScaling  ( vals, data[0], data[1], data[2] );
	}

	GetRotateValue       ( item, frame, quat );
	DX_QuaternionToMatrix( quat, rotmat );
	DX_MatrixMultiply    ( vals, vals, rotmat );

	GetPositionValue     ( item, frame, data );
	DX_MatrixTranslation ( vals, data[0], data[1], data[2] );

	return 16;
}


/*
 *
 *  MakeMatrixKey   - make homogenious matrix key
 *
 */

DX_AnimationKey* dxExporter::MakeMatrixKey( DX_Animation *dx_ani )
{
	DX_AnimationSet*        dx_set = _animationSet;
	LWW::Item               item   = dx_ani->frame->item;

	DX_DWORD firstFrame = (globals_.getInterfaceInfo()->previewStart < 0) ? 0 : globals_.getInterfaceInfo()->previewStart;
	DX_DWORD lastFrame  = (globals_.getInterfaceInfo()->previewEnd  < 0) ? 0 : globals_.getInterfaceInfo()->previewEnd;

	const char *ch[9];
	ch[0] = CHNAME_HEADING;
	ch[1] = CHNAME_PITCH;
	ch[2] = CHNAME_BANK;
	ch[3] = CHNAME_XSCALE;
	ch[4] = CHNAME_YSCALE;
	ch[5] = CHNAME_ZSCALE;
	ch[6] = CHNAME_XPOS;
	ch[7] = CHNAME_YPOS;
	ch[8] = CHNAME_ZPOS;

	DX_AnimationKey*  dx_key = new DX_AnimationKey;
	dx_key->keyType = DX_ANIM_MATRIXKEY;
	dx_key->nKeys   = 0;
	dx_key->keys    = NULL;
	dx_key->next    = NULL;

	DX_DWORD nKeys  = CountKeyframe( item, ch, 9 );
	DX_DWORD frame;

	if (nKeys == 1)
	{
		dx_key->nKeys = 2;
		DX_TimedFloatKeys*  keys = new DX_TimedFloatKeys[dx_key->nKeys];
		dx_key->keys = keys;

		for (frame = firstFrame; frame <= lastFrame; frame += lastFrame - firstFrame) 
		{
			keys->time           = frame;
			keys->tfkeys.nValues = GetMatrixValue( item, frame, keys->tfkeys.values );
			keys++;
		}
	}
	else
	{
		nKeys = lastFrame - firstFrame + 1;
		DX_TimedFloatKeys*  keys  = new DX_TimedFloatKeys[nKeys];
		dx_key->nKeys = 0;
		dx_key->keys  = keys;

		for (frame = firstFrame; frame <= lastFrame; frame++)
		{
			if (TestKeyframe( item, frame, ch, 9 ))
			{
				keys->time           = frame;
				keys->tfkeys.nValues = GetMatrixValue( item, frame, keys->tfkeys.values );
				keys++;
				dx_key->nKeys++;
			}
		}

	}

	if (_options->removeUnusableKeyframes)
	{
		if (UsableAnimationKey( dx_key )) {
			delete[] dx_key->keys;
			delete   dx_key;
			return NULL;
		}
	}

	if (dx_ani->animationKey == NULL) {
		dx_ani->animationKey = dx_key;
	} else {
		DX_AnimationKey*  dx_lst = dx_ani->animationKey;
		while (dx_lst->next) { dx_lst = dx_lst->next; }
		dx_lst->next = dx_key;
	}

	return dx_key;
}


/*
 *
 *  CountKeyframe   - return number of keyframes
 *
 */

DX_DWORD dxExporter::CountKeyframe( LWW::Item item, const char **chnames, int nch )
{
	DX_DWORD firstFrame = (globals_.getInterfaceInfo()->previewStart < 0) ? 0 : globals_.getInterfaceInfo()->previewStart;
	DX_DWORD lastFrame  = (globals_.getInterfaceInfo()->previewEnd  < 0) ? 0 : globals_.getInterfaceInfo()->previewEnd;

	int    key = 0;

	for (DX_DWORD frame = firstFrame; frame <= lastFrame; frame++)
	{
		if (TestKeyframe( item, frame, chnames, nch ) == TRUE) {
			key++;
		}
	}

	return key;
}


/*
 *
 *  TestChannel   - test the channel existing
 *
 */

bool dxExporter::TestChannel( LWW::Channel channel, const char **chnames, int nch )
{
	while (nch--) {
		if(strcmp(channel.name(), *chnames++)) return true;
	}
	return false;
}


/*
 *
 *  TestKeyframe   - test the keyframe data
 *
 */

bool dxExporter::TestKeyframe( LWW::Item item, LWFrame frame, const char **chnames, int nch )
{
	DX_DWORD firstFrame = (globals_.getInterfaceInfo()->previewStart < 0) ? 0 : globals_.getInterfaceInfo()->previewStart;
	DX_DWORD lastFrame  = (globals_.getInterfaceInfo()->previewEnd  < 0) ? 0 : globals_.getInterfaceInfo()->previewEnd;

	if ((DX_DWORD) frame == firstFrame) return true;

	if (_options->frameToExport == DX_FRM2EXP_ALWAYSSAMPLE ||
		_options->frameToExport == DX_FRM2EXP_KEYANDSAMPLE)
	{
		if ((frame % _options->frameSteps) == 0) return true;
		if ((DX_DWORD) frame == lastFrame) return true;
	}


	if (_options->frameToExport == DX_FRM2EXP_KEYFRAMEONLY ||
		_options->frameToExport == DX_FRM2EXP_KEYANDSAMPLE)
	{
		LWW::ChannelGroup chgroup = item.channelGroup();
		LWW::Channel channel = chgroup.firstChannel();
		while (channel) {
			if (TestChannel( channel, chnames, nch )) {
				LWW::Envelope envelope = channel.envelope();
				if(envelope.findKey(FRAME2TIME(frame))) return true;
			}
			channel = channel.next();
		}

		LWW::Item  target = item.target();

		if (target) {
			chgroup = target.channelGroup();
			channel = chgroup.firstChannel();
			while (channel) {
				if (TestChannel( channel, chnames, nch )) {
					LWW::Envelope envelope = channel.envelope();
					if(envelope.findKey(FRAME2TIME(frame))) return true;
				}
				channel = channel.next();
			}
		}
	}

	return false;
}



/*
 *
 *  UsableAnimationKey   - test if the keyframe is usable or not.
 *
 */

bool dxExporter::UsableAnimationKey( DX_AnimationKey* dx_key )
{
	DX_TimedFloatKeys*  base = dx_key->keys;
	DX_DWORD            n, m;

	for (n = 1; n < dx_key->nKeys; n++) 
	{
		for (m = 0; m < base->tfkeys.nValues; m++) {
			if (!FP_EQUAL(base->tfkeys.values[m], base[n].tfkeys.values[m])) return false;
		}
	}

	return true;
}



