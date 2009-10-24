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
#ifdef GCTP_USE_XFILEAPI
#include <gctp/vector.hpp>
#endif

#include "dxexp.h"

#ifdef GCTP_USE_XFILEAPI

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

#else
/*
 *
 *  WriteAnimationSet   - write the animation set
 *
 */

void dxExporter::WriteAnimationSet( DX_AnimationSet* dx_set, int tabs )
{
	if (dx_set == NULL) return;
	if (_options->outputAnimations == FALSE) return;

	switch (_options->format) {
	case DX_FORMAT_BINARY:
		BIN_WriteAnimationSet( dx_set );
		break;
	case DX_FORMAT_TEXT:
		TXT_WriteAnimationSet( dx_set, tabs );
		break;
	}
}

/*
 *
 *  TXT_WriteAnimationSet   - write the animation set
 *
 */

void dxExporter::TXT_WriteAnimationSet( DX_AnimationSet* dx_set, int tabs )
{
	DX_Animation*    dx_ani;

	if (dx_set->animations == NULL) return;

	ffp( 0, "\n" );
	ffp( tabs, "AnimationSet %s {\n", dx_set->nodeName );

	dx_ani = dx_set->animations;
	while (dx_ani) {
		TXT_WriteAnimation( dx_ani, tabs+1 );
		dx_ani = dx_ani->next;
	}

	ffp( tabs, "}\n" );
}


/*
 *
 *  TXT_WriteAnimation   - write the animation
 *
 */

void dxExporter::TXT_WriteAnimation( DX_Animation* dx_ani, int tabs )
{
	DX_AnimationKey*    dx_key;

	if (dx_ani == NULL) return;
	if (dx_ani->animationKey == NULL) return;

	ffp( 0, "\n" );
	ffp( tabs, "Animation %s {\n", dx_ani->nodeName );
	ffp( tabs+1, "{ %s }\n", dx_ani->frame->nodeName );

	dx_key = dx_ani->animationKey;
	while (dx_key) {
		TXT_WriteAnimationKey( dx_key, tabs+1 );
		dx_key = dx_key->next;
	}

	TXT_WriteAnimationOptions( dx_ani->animationOptions, tabs+1 );

	ffp( tabs, "}\n" );
}


/*
 *
 *  TXT_WriteAnimationOptions   - write the animation options
 *
 */

void dxExporter::TXT_WriteAnimationOptions( DX_AnimationOptions* dx_opt, int tabs )
{
	if (dx_opt == NULL) return;

	ffp( tabs, "AnimationOptions {\n" );
	ffp( tabs+1, "%d;\n", dx_opt->openclosed );
	ffp( tabs+1, "%d;\n", dx_opt->positionquality );

	ffp( tabs, "}\n" );
}



/*
 *
 *  TXT_WriteAnimationKey   - write the animation keys
 *
 */

void dxExporter::TXT_WriteAnimationKey( DX_AnimationKey* dx_key, int tabs )
{
	DX_DWORD        n;

	if (dx_key == NULL) return;

	ffp( tabs, "AnimationKey {\n" );
	ffp( tabs+1, "%d;\n", dx_key->keyType );
	ffp( tabs+1, "%d;\n", dx_key->nKeys );

	for (n = 0; n < dx_key->nKeys; n++) 
	{
		switch (dx_key->keyType)
		{
		case DX_ANIM_ROTATEKEY:
			ffp( tabs+1, "%d;%d;%f,%f,%f,%f;;", 
							dx_key->keys[n].time * _options->frameTimeScale,
							dx_key->keys[n].tfkeys.nValues,
							dx_key->keys[n].tfkeys.values[0],
							dx_key->keys[n].tfkeys.values[1],
							dx_key->keys[n].tfkeys.values[2],
							dx_key->keys[n].tfkeys.values[3] );
			break;
		case DX_ANIM_SCALEKEY:
			ffp( tabs+1, "%d;%d;%f,%f,%f;;", 
							dx_key->keys[n].time * _options->frameTimeScale,
							dx_key->keys[n].tfkeys.nValues,
							dx_key->keys[n].tfkeys.values[0],
							dx_key->keys[n].tfkeys.values[1],
							dx_key->keys[n].tfkeys.values[2] );
			break;
		case DX_ANIM_POSITIONKEY:
			ffp( tabs+1, "%d;%d;%f,%f,%f;;", 
							dx_key->keys[n].time * _options->frameTimeScale,
							dx_key->keys[n].tfkeys.nValues,
							dx_key->keys[n].tfkeys.values[0],
							dx_key->keys[n].tfkeys.values[1],
							dx_key->keys[n].tfkeys.values[2] );
			break;
		case DX_ANIM_MATRIXKEY:
			ffp( tabs+1, "%d;%d;%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f;;", 
							dx_key->keys[n].time * _options->frameTimeScale,
							dx_key->keys[n].tfkeys.nValues,
							dx_key->keys[n].tfkeys.values[ 0],
							dx_key->keys[n].tfkeys.values[ 1],
							dx_key->keys[n].tfkeys.values[ 2],
							dx_key->keys[n].tfkeys.values[ 3],
							dx_key->keys[n].tfkeys.values[ 4],
							dx_key->keys[n].tfkeys.values[ 5],
							dx_key->keys[n].tfkeys.values[ 6],
							dx_key->keys[n].tfkeys.values[ 7],
							dx_key->keys[n].tfkeys.values[ 8],
							dx_key->keys[n].tfkeys.values[ 9],
							dx_key->keys[n].tfkeys.values[10],
							dx_key->keys[n].tfkeys.values[11],
							dx_key->keys[n].tfkeys.values[12],
							dx_key->keys[n].tfkeys.values[13],
							dx_key->keys[n].tfkeys.values[14],
							dx_key->keys[n].tfkeys.values[15] );
			break;
		}
		ffp( 0, "%c\n", DX_TERM(n, dx_key->nKeys) );
	}

	ffp( tabs, "}\n" );
}


/*
 *
 *  BIN_WriteAnimationSet   - write the animation set
 *
 */

void dxExporter::BIN_WriteAnimationSet( DX_AnimationSet* dx_set )
{
	DX_Animation*    dx_ani;

	if (dx_set->animations == NULL) return;

	ffp_NAME  ( "AnimationSet" );
	ffp_NAME  ( dx_set->nodeName );
	ffp_TOKEN ( TOKEN_OBRACE );

	dx_ani = dx_set->animations;
	while (dx_ani) {
		BIN_WriteAnimation( dx_ani );
		dx_ani = dx_ani->next;
	}

	ffp_TOKEN ( TOKEN_CBRACE );
}


/*
 *
 *  BIN_WriteAnimation   - write the animation
 *
 */

void dxExporter::BIN_WriteAnimation( DX_Animation* dx_ani )
{
	DX_AnimationKey*    dx_key;

	if (dx_ani == NULL) return;
	if (dx_ani->animationKey == NULL) return;

	ffp_NAME  ( "Animation" );
	ffp_NAME  ( dx_ani->nodeName );
	ffp_TOKEN ( TOKEN_OBRACE );

	ffp_TOKEN ( TOKEN_OBRACE );
	ffp_NAME  ( dx_ani->frame->nodeName );
	ffp_TOKEN ( TOKEN_CBRACE );

	dx_key = dx_ani->animationKey;
	while (dx_key) {
		BIN_WriteAnimationKey( dx_key );
		dx_key = dx_key->next;
	}

	BIN_WriteAnimationOptions( dx_ani->animationOptions );

	ffp_TOKEN ( TOKEN_CBRACE );
}


/*
 *
 *  BIN_WriteAnimationOptions   - write the animation options
 *
 */

void dxExporter::BIN_WriteAnimationOptions( DX_AnimationOptions* dx_opt )
{
	if (dx_opt == NULL) return;

	ffp_NAME  ( "AnimationOptions" );
	ffp_TOKEN ( TOKEN_OBRACE );

	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	ffp_dword ( 2 );
	ffp_dword ( dx_opt->openclosed );
	ffp_dword ( dx_opt->positionquality );

	ffp_TOKEN ( TOKEN_CBRACE );
}



/*
 *
 *  BIN_WriteAnimationKey   - write the animation keys
 *
 */

void dxExporter::BIN_WriteAnimationKey( DX_AnimationKey* dx_key )
{
	DX_DWORD        n;

	if (dx_key == NULL) return;

	ffp_NAME  ( "AnimationKey" );
	ffp_TOKEN ( TOKEN_OBRACE );

	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	ffp_dword ( 2 );
	ffp_dword ( dx_key->keyType );
	ffp_dword ( dx_key->nKeys );

	for (n = 0; n < dx_key->nKeys; n++) 
	{
		switch (dx_key->keyType)
		{
		case DX_ANIM_ROTATEKEY:
			ffp_TOKEN ( TOKEN_INTEGER_LIST );
			ffp_dword ( 2 );
			ffp_dword ( dx_key->keys[n].time * _options->frameTimeScale );
			ffp_dword ( dx_key->keys[n].tfkeys.nValues );
			ffp_TOKEN ( TOKEN_FLOAT_LIST );
			ffp_dword ( 4 );
			ffp_float ( dx_key->keys[n].tfkeys.values[0] );
			ffp_float ( dx_key->keys[n].tfkeys.values[1] );
			ffp_float ( dx_key->keys[n].tfkeys.values[2] );
			ffp_float ( dx_key->keys[n].tfkeys.values[3] );
			break;
		case DX_ANIM_SCALEKEY:
			ffp_TOKEN ( TOKEN_INTEGER_LIST );
			ffp_dword ( 2 );
			ffp_dword ( dx_key->keys[n].time * _options->frameTimeScale );
			ffp_dword ( dx_key->keys[n].tfkeys.nValues );
			ffp_TOKEN ( TOKEN_FLOAT_LIST );
			ffp_dword ( 3 );
			ffp_float ( dx_key->keys[n].tfkeys.values[0] );
			ffp_float ( dx_key->keys[n].tfkeys.values[1] );
			ffp_float ( dx_key->keys[n].tfkeys.values[2] );
			break;
		case DX_ANIM_POSITIONKEY:
			ffp_TOKEN ( TOKEN_INTEGER_LIST );
			ffp_dword ( 2 );
			ffp_dword ( dx_key->keys[n].time * _options->frameTimeScale );
			ffp_dword ( dx_key->keys[n].tfkeys.nValues );
			ffp_TOKEN ( TOKEN_FLOAT_LIST );
			ffp_dword ( 3 );
			ffp_float ( dx_key->keys[n].tfkeys.values[0] );
			ffp_float ( dx_key->keys[n].tfkeys.values[1] );
			ffp_float ( dx_key->keys[n].tfkeys.values[2] );
			break;
		case DX_ANIM_MATRIXKEY:
			ffp_TOKEN ( TOKEN_INTEGER_LIST );
			ffp_dword ( 2 );
			ffp_dword ( dx_key->keys[n].time * _options->frameTimeScale );
			ffp_dword ( dx_key->keys[n].tfkeys.nValues );
			ffp_TOKEN ( TOKEN_FLOAT_LIST );
			ffp_dword ( 4 );
			ffp_float ( dx_key->keys[n].tfkeys.values[ 0] );
			ffp_float ( dx_key->keys[n].tfkeys.values[ 1] );
			ffp_float ( dx_key->keys[n].tfkeys.values[ 2] );
			ffp_float ( dx_key->keys[n].tfkeys.values[ 3] );
			ffp_TOKEN ( TOKEN_FLOAT_LIST );
			ffp_dword ( 4 );
			ffp_float ( dx_key->keys[n].tfkeys.values[ 4] );
			ffp_float ( dx_key->keys[n].tfkeys.values[ 5] );
			ffp_float ( dx_key->keys[n].tfkeys.values[ 6] );
			ffp_float ( dx_key->keys[n].tfkeys.values[ 7] );
			ffp_TOKEN ( TOKEN_FLOAT_LIST );
			ffp_dword ( 4 );
			ffp_float ( dx_key->keys[n].tfkeys.values[ 8] );
			ffp_float ( dx_key->keys[n].tfkeys.values[ 9] );
			ffp_float ( dx_key->keys[n].tfkeys.values[10] );
			ffp_float ( dx_key->keys[n].tfkeys.values[11] );
			ffp_TOKEN ( TOKEN_FLOAT_LIST );
			ffp_dword ( 4 );
			ffp_float ( dx_key->keys[n].tfkeys.values[12] );
			ffp_float ( dx_key->keys[n].tfkeys.values[13] );
			ffp_float ( dx_key->keys[n].tfkeys.values[14] );
			ffp_float ( dx_key->keys[n].tfkeys.values[15] );
			break;
		}
	}

	ffp_TOKEN ( TOKEN_CBRACE );
}




#endif
