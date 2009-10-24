/*
 * UI_EXPO.CPP -- DirectX Export
 *
 * Copyright (c) 2000-2001, D-STORM, Inc. 
 *
 * written by Yoshiaki Tazaki
 * revision  history  10/02/2001
 */

#include "dxui.h"
#include <gctp/vector.hpp>
#include "dxexp.h"


/*****  DECLARATIONS FOR STATIC FUNCTIONS   *****/

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
		const char *curfname = _query->object();
		int obj_no;
		for(obj_no = 0; obj_no < dxs.globals().getObjectFuncs()->numObjects(); obj_no++) {
			if(strcmp(dxs.globals().getObjectFuncs()->filename(obj_no), curfname) == 0) break;
		}
		if (dxs.ExportObject( obj_no ) == false) {
			(*_msgfunc->error)( "Can't open the X File to write.", _options->outputFile );
		}
	}
}



/*
 *  ExportScene         EXPORT SCENE TO THE X FILE
 */
void dxUI::ExportScene()
{
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
			//BakeKeyframes( scn );
		}
		if (dxs.ExportScene() == false) {
			(*_msgfunc->error)( "Can't open the X File to write.", _options->outputFile );
		}
	}
}



/*
 *  RunViewer         RUN VIEWER APPLICATION
 */
bool dxUI::RunViewer()
{
#ifdef _WIN32
	static STARTUPINFOA        siStartInfo;
	static PROCESS_INFORMATION piProcInfo;
	DX_STRING                  commandLine;
	DX_FNAM                    path, drive, dir, filename, ext;

	_splitpath( _options->outputFile, drive, dir, filename, ext );
	_makepath ( path, drive, dir, 0, 0 );

    GetStartupInfoA( &siStartInfo );

    siStartInfo.dwFlags     = STARTF_USESHOWWINDOW;
    siStartInfo.wShowWindow = SW_SHOWDEFAULT;

    wsprintfA( commandLine, "%s \"%s\"", _options->viewerName, _options->outputFile ) ;

    if(!CreateProcessA(0, commandLine, 0, 0, FALSE, 0, 0, path, &siStartInfo, &piProcInfo)) {
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
