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
#ifdef GCTP_USE_XFILEAPI
#include <gctp/vector.hpp>
#endif

#include "dxexp.h"
#include "dxvec.h"
#include "mathlib.h"

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

int dxExporter::GetRotateValue( SCNItemID item, DX_DWORD frame, DX_FLOAT* vals )
{
	EulerAngles             euler;
	Quat                    quat, quat1, quat2;
	SCNItemID               target;
	double                  rot[3], pvt[3];

	if ((target = (*_scn->target)( _scn, item ))) {
		quat  = GetTargetRotation( item, target, FRAME2TIME(_scn,frame) );
	}
	else if (item->hController == SCN_CTRL_ALIGN ||
			 item->pController == SCN_CTRL_ALIGN) {
		quat1 = PathAlignLookAhead( item, FRAME2TIME(_scn,frame) );
	}
	else {
		(*_scn->param)( _scn, item, SCNP_ROTATION, FRAME2TIME(_scn,frame), rot );
		euler = mat4_euler   ( -rot[0], -rot[1], -rot[2], EulOrdYXZs );
		quat1 = mat4_eul2quat( euler );
	}
	(*_scn->param)( _scn, item, SCNP_PIVOT_ROT, 0.0, pvt );
	euler = mat4_euler   ( -pvt[0], -pvt[1], -pvt[2], EulOrdYXZs );
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
	SCNItemID               item   = dx_ani->frame->item;

	DX_DWORD firstFrame = (_scn->previewFirstFrame < 0) ? 0 : _scn->previewFirstFrame;
	DX_DWORD lastFrame  = (_scn->previewLastFrame  < 0) ? 0 : _scn->previewLastFrame;

	SCNChannelNo      ch[3];
	ch[0] = SCN_CHAN_HEADING;
	ch[1] = SCN_CHAN_PITCH;
	ch[2] = SCN_CHAN_BANK;

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

int dxExporter::GetScaleValue( SCNItemID item, DX_DWORD frame, DX_FLOAT* vals )
{
	double      scl[3];

	(*_scn->param)( _scn, item, SCNP_SCALING, FRAME2TIME(_scn,frame), scl );
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
	SCNItemID               item   = dx_ani->frame->item;

	DX_DWORD firstFrame = (_scn->previewFirstFrame < 0) ? 0 : _scn->previewFirstFrame;
	DX_DWORD lastFrame  = (_scn->previewLastFrame  < 0) ? 0 : _scn->previewLastFrame;

	SCNChannelNo      ch[3];
	ch[0] = SCN_CHAN_XSCALE;
	ch[1] = SCN_CHAN_YSCALE;
	ch[2] = SCN_CHAN_ZSCALE;

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

int dxExporter::GetPositionValue( SCNItemID item, DX_DWORD frame, DX_FLOAT* vals )
{
	double    pos[3];

	(*_scn->param)( _scn, item, SCNP_POSITION, FRAME2TIME(_scn,frame), pos );
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
	SCNItemID               item   = dx_ani->frame->item;

	DX_DWORD firstFrame = (_scn->previewFirstFrame < 0) ? 0 : _scn->previewFirstFrame;
	DX_DWORD lastFrame  = (_scn->previewLastFrame  < 0) ? 0 : _scn->previewLastFrame;

	SCNChannelNo      ch[3];
	ch[0] = SCN_CHAN_XPOS;
	ch[1] = SCN_CHAN_YPOS;
	ch[2] = SCN_CHAN_ZPOS;

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

int dxExporter::GetMatrixValue( SCNItemID item, DX_DWORD frame, DX_FLOAT* vals )
{
	DX_FLOAT       data[4], rotmat[16], quat[4];
	double         pvt[3];

	DX_MatrixIdentity( vals );

	if (item->type == SCN_OBJECT || item->type == SCN_BONE)
	{
		(*_scn->param)( _scn, item, SCNP_PIVOT, 0.0, pvt );
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
	SCNItemID               item   = dx_ani->frame->item;

	DX_DWORD firstFrame = (_scn->previewFirstFrame < 0) ? 0 : _scn->previewFirstFrame;
	DX_DWORD lastFrame  = (_scn->previewLastFrame  < 0) ? 0 : _scn->previewLastFrame;

	SCNChannelNo      ch[9];
	ch[0] = SCN_CHAN_HEADING;
	ch[1] = SCN_CHAN_PITCH;
	ch[2] = SCN_CHAN_BANK;
	ch[3] = SCN_CHAN_XSCALE;
	ch[4] = SCN_CHAN_YSCALE;
	ch[5] = SCN_CHAN_ZSCALE;
	ch[6] = SCN_CHAN_XPOS;
	ch[7] = SCN_CHAN_YPOS;
	ch[8] = SCN_CHAN_ZPOS;

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

DX_DWORD dxExporter::CountKeyframe( SCNItemID item, SCNChannelNo* ch, int nch )
{
	DX_DWORD firstFrame = (_scn->previewFirstFrame < 0) ? 0 : _scn->previewFirstFrame;
	DX_DWORD lastFrame  = (_scn->previewLastFrame  < 0) ? 0 : _scn->previewLastFrame;

	int    key = 0;

	for (DX_DWORD frame = firstFrame; frame <= lastFrame; frame++)
	{
		if (TestKeyframe( item, frame, ch, nch ) == TRUE) {
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

bool dxExporter::TestChannel( SCNChannelID channel, SCNChannelNo* ch, int nch )
{
	while (nch--) {
		if (channel->no == (*ch++)) return true;
	}
	return false;
}


/*
 *
 *  TestKeyframe   - test the keyframe data
 *
 */

bool dxExporter::TestKeyframe( SCNItemID item, int frame, SCNChannelNo* ch, int nch )
{
	DX_DWORD firstFrame = (_scn->previewFirstFrame < 0) ? 0 : _scn->previewFirstFrame;
	DX_DWORD lastFrame  = (_scn->previewLastFrame  < 0) ? 0 : _scn->previewLastFrame;

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
		SCNChannelID   channel = item->channel;
		SCNKey*        key;

		while (channel) {
			if (TestChannel( channel, ch, nch ))
			{
				key = channel->keys;
				while (key) {
					if (frame == TIME2FRAME(_scn,key->time)) return true;
					key = key->next;
				}
			}
			channel = channel->next;
		}

		SCNItemID  target = (*_scn->target)( _scn, item );

		if (target) {
			channel = target->channel;
			while (channel) {
				if (TestChannel( channel, ch, nch ))
				{
					key = channel->keys;
					while (key) {
						if (frame == TIME2FRAME(_scn,key->time)) return true;
						key = key->next;
					}
				}
				channel = channel->next;
			}
		}
	}

	return false;
}


/*
 *
 *  GetTargetRotation   - return quatanion for camera target
 *
 */

Quat dxExporter::GetTargetRotation( SCNItemID item, SCNItemID target, SCNTime ti )
{
	Quat               quat;
	EulerAngles        euler1, euler2;
	double             vo[3], vt[3], vi[3], rot[3], theta;

	quat.x = 0.0;
	quat.y = 0.0;
	quat.z = 0.0;
	quat.w = 1.0;
	DVEC_SET( vi, 0.0, 0.0, 1.0 );

	(*_scn->param)( _scn, item,   SCNP_POSITION, ti, vo  );
	(*_scn->param)( _scn, item,   SCNP_ROTATION, ti, rot );
	(*_scn->param)( _scn, target, SCNP_POSITION, ti, vt  );

	DVEC_SUB ( vt, vt, vo );
	if (DVEC_UNIT(vt,vt) < EPSILON_D) return quat;
	DVEC_OUTP( vo, vt, vi );
	theta  = DVEC_ANG( vt, vi );
	if (DVEC_UNIT(vo,vo) < EPSILON_D) {
		quat.x = 0.0;
		quat.y = sin(theta/2);
		quat.z = 0.0;
		quat.w = cos(theta/2);
	} else {
		quat.x = sin(theta/2) * vo[0];
		quat.y = sin(theta/2) * vo[1];
		quat.z = sin(theta/2) * vo[2];
		quat.w = cos(theta/2);
	}

	euler1   = mat4_quat2eul( quat, EulOrdYXZs );
	euler2   = mat4_euler   ( -rot[0], -rot[1], -rot[2], EulOrdYXZs );
	if (item->hController == SCN_CTRL_TARGET) euler2.x = euler1.x;
	if (item->pController == SCN_CTRL_TARGET) euler2.y = euler1.y;
	quat     = mat4_eul2quat( euler2 );

	return quat;
}


/*
 *
 *  PathAlignLookAhead   - return quatanion for PathAlignLookAhead
 *
 */

Quat dxExporter::PathAlignLookAhead( SCNItemID item, SCNTime ti )
{
	Quat                    quat;
	EulerAngles             euler1, euler2;
	double                  vo[3], vt[3], vi[3], rot[3], theta;

	quat.x = 0.0;
	quat.y = 0.0;
	quat.z = 0.0;
	quat.w = 1.0;
	DVEC_SET( vi, 0.0, 0.0, 1.0 );

	(*_scn->param)( _scn, item, SCNP_POSITION, ti, vo  );
	(*_scn->param)( _scn, item, SCNP_ROTATION, ti, rot );
	(*_scn->param)( _scn, item, SCNP_POSITION, ti + item->pathAlignLookAhead, vt );

	DVEC_SUB ( vt, vt, vo );
	if (DVEC_UNIT(vt,vt) < EPSILON_D) return quat;
	DVEC_OUTP( vo, vt, vi );
	theta  = DVEC_ANG( vt, vi );
	if (DVEC_UNIT(vo,vo) < EPSILON_D) {
		quat.x = 0.0;
		quat.y = sin(theta/2);
		quat.z = 0.0;
		quat.w = cos(theta/2);
	} else {
		quat.x = sin(theta/2) * vo[0];
		quat.y = sin(theta/2) * vo[1];
		quat.z = sin(theta/2) * vo[2];
		quat.w = cos(theta/2);
	}

	euler1   = mat4_quat2eul( quat, EulOrdYXZs );
	euler2   = mat4_euler   ( -rot[0], -rot[1], -rot[2], EulOrdYXZs );
	if (item->hController == SCN_CTRL_ALIGN) euler2.x = euler1.x;
	if (item->pController == SCN_CTRL_ALIGN) euler2.y = euler1.y;
	quat     = mat4_eul2quat( euler2 );

	return quat;
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



