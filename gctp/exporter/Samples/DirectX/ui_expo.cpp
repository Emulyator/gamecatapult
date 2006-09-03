/*
 * UI_EXPO.CPP -- DirectX Export
 *
 * Copyright (c) 2000-2001, D-STORM, Inc. 
 *
 * written by Yoshiaki Tazaki
 * revision  history  10/02/2001
 */

#include "dxui.h"
#ifdef GCTP_USE_XFILEAPI
#include <gctp/vector.hpp>
#endif
#include "dxexp.h"


/*****  DECLARATIONS FOR STATIC FUNCTIONS   *****/

static OBJ2_ID UI_GetSource  ( SCN3_ID scn, char *filename );
static int     UI_FindObject ( GlobalFunc *global, char *filename );


/*
 *  Export               EXPORT LW TO THE X FILE
 */
void dxUI::Export()
{
	if (_options->status == DX_STATUS_MODELER) {
		ExportModel();
	}

	if (_options->status == DX_STATUS_LAYOUT) {
		ExportScene();
	}

	switch (_options->outputTextureImages) {
	case DX_TXTOUT_CONVERT:
		ConvertImages();
		break;
	case DX_TXTOUT_FILECOPY:
		CopyImages();
		break;
	}

	if (_options->enableViewer)
	{
		RunViewer();
	}
}


/*
 *  ExportModel         EXPORT MESH TO THE X FILE
 */
void dxUI::ExportModel()
{
	OBJ2_ID  obj = OBJ2_new( OBJ_FLAG_NONE );
	Mesh_Load( obj, _global, _obj_no, TRUE );

	if (_options->outputTextureCoords) {
		(*obj->buildUVmap) ( obj, MAT_CHAN_COLR|MAT_CHAN_DIFF, 1, OPT_UV_NONE );
	}
	if (_options->outputMeshNormals) {
		(*obj->makeNormal) ( obj, OPT_UV_NONE );
	}
	if (_options->outputVertexColors) {
		(*obj->makeColor)  ( obj, OPT_COL_NONE );
	}
	if (_options->tripleAll) {
		(*obj->tripleAll)( obj, OPT_TRI_NONE );
	} else {
		(*obj->tripleAll)( obj, OPT_TRI_CONCAVE );
	}
	(*obj->sortPols) ( obj );

	dxExporter dxs( _options, _global );

	bool    result;

	if (dxs.IsNewScene()) {
		result = true;
	} else {
		DX_STRING   info;
		sprintf( info, "The file %s", _options->outputFile );
		if ((*_msgfunc->yesNo)("Warning", info, "already exist. Override ?")) {
			result = true;
		} else {
			result = false;
		}
	}

	if (result) {
		if (dxs.ExportObject( obj ) == false) {
			(*_msgfunc->error)( "Can't open the X File to write.", _options->outputFile );
		}
	}

	OBJ2_close( &obj );
}



/*
 *  ExportScene         EXPORT SCENE TO THE X FILE
 */
void dxUI::ExportScene()
{
	LWDirInfoFunc*    dirInfo;
	LWLayoutGeneric*  local;
	OBJ2_ID           obj;
	SCN3_ID           scn;
	SCNItemID         item;
	SCNObjectID       obj_id;
	LayerData*        layer;
	DX_FNAM           tmpFile, filename;
	DX_STRING         command;
	int               obj_no;
	PathConvParam     param;

	dirInfo = (LWDirInfoFunc  *) (*_global)(LWDIRINFOFUNC_GLOBAL, GFUSE_TRANSIENT);

	tmpnam( filename );
#ifdef _WIN32
	if (filename[0] == '\\') {
		strcpy( filename, filename+1 );
	}
#endif
	param.directory = (char *) (*dirInfo)("Install");
	param.name      = filename;
	param.path_out  = tmpFile;
	PathConvert( PATHCONV_BUILDPATH, &param );

	local = (LWLayoutGeneric *) _local;
	sprintf( command, "SaveSceneCopy %s", tmpFile );
	(*local->evaluate)( local->data, (const char *) command );

	scn  = SCN3_open( tmpFile, SCN_FLAG_MOT2CHAN, (OBJ2_option *) NULL );
	if (scn == NULL) return;
	remove( tmpFile );

	item = (*scn->first)( scn, SCN_OBJECT, (SCNItemID) NULL );

	while (item)
	{
		obj_id = (*scn->getObject)( scn, item );

		if (obj_id->type == SCN_OBJ_FILE)
		{
			if ((obj = UI_GetSource( scn, obj_id->filename )))
			{
				obj   = (*obj->instObj)( obj );
				layer = 0;
				while ((layer = (*obj->nextLayer)( obj, layer ))) 
				{
					if ((layer->layer_no + 1) == obj_id->layer) {
						(*obj->setLayer)( obj, layer->idx );
						obj_id->obj2 = obj;
					}
				}
			}
			else
			{
				obj    = OBJ2_new ( OBJ_FLAG_NONE );
				obj_no = UI_FindObject( _global, obj_id->filename );
				if (obj_no < 0) {
					(*_msgfunc->error)( "Can't open the object file.", obj_id->filename );
					return;
				}
				
				Mesh_Load( obj, _global, obj_no, FALSE );
				obj_id->obj2 = obj;
			}

			if (_options->outputTextureCoords) {
				(*obj->buildUVmap)( obj, MAT_CHAN_COLR|MAT_CHAN_DIFF, 1, OPT_UV_NONE );
			}
			if (_options->outputMeshNormals) {
				(*obj->makeNormal)( obj, OPT_UV_NONE );
			}
			if (_options->outputVertexColors) {
				(*obj->makeColor) ( obj, OPT_COL_NONE );
			}
			if (_options->tripleAll) {
				(*obj->tripleAll) ( obj, OPT_TRI_NONE );
			} else {
				(*obj->tripleAll) ( obj, OPT_TRI_CONCAVE );
			}
			(*obj->sortPols) ( obj );
		}

		item = (*scn->next)( scn, item );
	}

	dxExporter dxs( _options, _global );

	bool    result;

	if (dxs.IsNewScene()) {
		result = true;
	} else {
		DX_STRING   info;
		sprintf( info, "The file %s", _options->outputFile );
		if ((*_msgfunc->yesNo)("Warning", info, "already exist. Override ?")) {
			result = true;
		} else {
			result = false;
		}
	}

	if (result) 
	{
		if (_options->bakeIKMotions) {
			BakeKeyframes( scn );
		}
		if (dxs.ExportScene( scn ) == false) {
			(*_msgfunc->error)( "Can't open the X File to write.", _options->outputFile );
		}
	}

	SCN3_close( &scn );
}



/*
 *  RunViewer         RUN VIEWER APPLICATION
 */
bool dxUI::RunViewer()
{
#ifdef _WIN32
	static STARTUPINFO         siStartInfo;
	static PROCESS_INFORMATION piProcInfo;
	DX_STRING                  commandLine;
	DX_FNAM                    path, drive, dir, filename, ext;

	_splitpath( _options->outputFile, drive, dir, filename, ext );
	_makepath ( path, drive, dir, 0, 0 );

    GetStartupInfo( &siStartInfo );

    siStartInfo.dwFlags     = STARTF_USESHOWWINDOW;
    siStartInfo.wShowWindow = SW_SHOWDEFAULT;

    wsprintf( commandLine, "%s %s", _options->viewerName, _options->outputFile ) ;

    if(!CreateProcess(0, commandLine, 0, 0, FALSE, 0, 0, path, &siStartInfo, &piProcInfo)) {
		return false;
    }
    else {
		CloseHandle( piProcInfo.hThread );
		CloseHandle( piProcInfo.hProcess );
		return true;
    }
#endif
    return false;
}



/*
 *  UI_GetSource         GET THE SOURCE OBJECT
 */
static OBJ2_ID UI_GetSource( SCN3_ID scn, char *filename )
{
	SCNItemID    item;
	SCNObjectID  obj;

	item = (*scn->first)( scn, SCN_OBJECT, (SCNItemID) NULL );
	while (item )
	{
		obj  = (*scn->getObject)( scn, item );
		if (strcmp(obj->filename, filename) == 0 && obj->obj2) {
			return obj->obj2;
		}
		item = (*scn->next)( scn, item );
	}
	return (OBJ2_ID) NULL;
}



/*
 *  UI_FindObject         FIND THE OBJECT FILENAME AND RETURN ID
 */
static int UI_FindObject( GlobalFunc *global, char *filename )
{
	LWDirInfoFunc*    dirInfo;
	PathConvParam     param;
	DX_FNAM           full;
	int               obj_no;

	obj_no = Mesh_Find( global, filename );
	if (obj_no < 0) {
		dirInfo = (LWDirInfoFunc  *) (*global)(LWDIRINFOFUNC_GLOBAL, GFUSE_TRANSIENT);
		param.path_in   = filename;
		param.directory = (char *) (*dirInfo)("Content");
		param.path_out  = full;
		PathConvert( PATHCONV_REL2FULL, &param );
		obj_no = Mesh_Find( global, full );
	}
	return obj_no;
}

