/*
 * UI_KEY.CPP -- DirectX Export
 *
 * Copyright (c) 2000-2001, D-STORM, Inc. 
 *
 * written by Yoshiaki Tazaki
 * revision  history  10/02/2001
 *
 */

#include "dxui.h"


/******  PROTOTYPES  ******************************/

#if 0
 ‘½•ª‚¢‚ç‚È‚¢
static bool HasIKGoal( LWItemInfo* iinfo, LWW::Item item );


/*
 *
 *  BakeKeyframes   - bake keyframes before conversion
 * 
 */
void dxUI::BakeKeyframes( )
{
	if (_options->status != DX_STATUS_LAYOUT ||
		_options->outputAnimations == false) return;

	LWLayoutGeneric*  local = (LWLayoutGeneric *)_local;
	LWItemInfo*       iinfo = (LWItemInfo *)(*_global)( LWITEMINFO_GLOBAL, GFUSE_TRANSIENT );

	int firstFrame = (scn->previewFirstFrame < 0) ? 0 : scn->previewFirstFrame;
	int lastFrame  = (scn->previewLastFrame  < 0) ? 0 : scn->previewLastFrame;

	LWW::Item    objSN, bonSN;
	LWW::Item     objLW, bonLW;
	double       time, pos[3], rot[3], scl[3];
	char         command[64];

	strcpy( command, "AutoConfirm on" );
	(*local->evaluate)( local->data, command );

	for (int frame = firstFrame; frame <= lastFrame;)
	{
		sprintf( command, "GoToFrame %d", frame );
		(*local->evaluate)( local->data, command );

		time = FRAME2TIME(scn,frame);

		objSN = (*scn->first)( scn, LWI_OBJECT, (LWW::Item) NULL );
		objLW = (*iinfo->first)( LWI_OBJECT, (LWW::Item) NULL );

		while (objLW)
		{
			if (HasIKGoal( iinfo, objLW ))
			{
				sprintf( command, "SelectByName %s", (*iinfo->name)( objLW ) );
				(*local->evaluate)( local->data, command );
				sprintf( command, "CreateKey %f", time );
				(*local->evaluate)( local->data, command );

				(*iinfo->param)( objLW, LWIP_POSITION, time, pos );
				(*iinfo->param)( objLW, LWIP_ROTATION, time, rot );
				(*iinfo->param)( objLW, LWIP_SCALING , time, scl );

				(*scn->addParam)( scn, objSN, LWIP_POSITION, time, pos );
				(*scn->addParam)( scn, objSN, LWIP_ROTATION, time, rot );
				(*scn->addParam)( scn, objSN, LWIP_SCALING , time, scl );

				bonSN = (*scn->first)( scn, LWI_BONE, objSN );
				bonLW = (*iinfo->first)( LWI_BONE, objLW );

				if (bonLW) {
					strcpy( command, "SelectAllBones" );
					(*local->evaluate)( local->data, command );
				}
				sprintf( command, "CreateKey %f", time );
				(*local->evaluate)( local->data, command );

				while (bonLW)
				{
					if (HasIKGoal( iinfo, objLW ))
					{
						(*iinfo->param)( bonLW, LWIP_POSITION, time, pos );
						(*iinfo->param)( bonLW, LWIP_ROTATION, time, rot );
						(*iinfo->param)( bonLW, LWIP_SCALING , time, scl );

						(*scn->addParam)( scn, bonSN, LWIP_POSITION, time, pos );
						(*scn->addParam)( scn, bonSN, LWIP_ROTATION, time, rot );
						(*scn->addParam)( scn, bonSN, LWIP_SCALING , time, scl );
					}

					bonSN = (*scn->next)( scn, bonSN );
					bonLW = (*iinfo->next)( bonLW );
				}
			}

			objSN = (*scn->next)( scn, objSN );
			objLW = (*iinfo->next)( objLW );
		}
		if (frame < lastFrame && (frame + _options->frameSteps) > lastFrame) {
			frame  = lastFrame;
		} else {
			frame += _options->frameSteps;
		}
	}

	objLW = (*iinfo->first)( LWI_OBJECT, (LWW::Item) NULL );
	if (objLW) {
		sprintf( command, "SelectByName %s", (*iinfo->name)( objLW ) );
		(*local->evaluate)( local->data, command );
	}

	strcpy( command, "AutoConfirm off" );
	(*local->evaluate)( local->data, command );
}

static bool HasIKGoal( LWItemInfo* iinfo, LWW::Item item )
{
	if ((*iinfo->goal)( item )) {
		return true;
	} else {
		LWW::Item  child = (*iinfo->firstChild)( item );
		while (child) {
			if (HasIKGoal( iinfo, child )) return true;
			child = (*iinfo->nextChild)( item, child );
		}
	}
	return false;
}
#endif
