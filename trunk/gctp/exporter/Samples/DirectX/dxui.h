/*********************************************************************/
/*                                                                   */
/* FILE :        dxui.h                                              */
/*                                                                   */
/* DESCRIPTION : LightWave 3D to DirectX Export                      */
/*                                                                   */
/* HISTORY :     Oct 29, 2000    written by Yoshiaki Tazaki          */
/*               Oct 02, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/

#ifndef _DXUI_H_
#define _DXUI_H_


extern "C" {
#include <lwserver.h>
#include <lwmeshes.h>
#include <lwcmdseq.h>
#include <lwgeneric.h>
#include <lwhost.h>
#include <lwdisplay.h>
#include <lwimage.h>
#include <lwpanel.h>
}

#include "dxopt.h"

///////////////////////////////////////////////  PANEL CONTROLS

#define PROGRAM_NAME "DirectX Export v1.3.1 ("__DATE__")"
#define COPYRIGHT    "(c) 2000-2001 D-STORM, Inc. by Yoshiaki Tazaki"

///////////////////////////////////////////////  PANEL CONTROLS

enum UI_MAIN_CNTL {
	UI_MAIN_FILE = 0,
	UI_MAIN_FBTN,
	UI_MAIN_TXTD,
	UI_MAIN_TBTN,
	UI_MAIN_TBMN,
	UI_MAIN_VERS,
	UI_MAIN_FORM,
	UI_MAIN_MATE,
#ifndef GCTP_USE_XFILEAPI
	UI_MAIN_TEMP,
#endif
	UI_MAIN_ANIM,
	UI_MAIN_FRAM,
	UI_MAIN_PREF,
	UI_MAIN_NORM,
	UI_MAIN_TXUV,
	UI_MAIN_REVV,
	UI_MAIN_VCOL,
	UI_MAIN_SKWF,
	UI_MAIN_MXWV,
	UI_MAIN_MXWF,
	UI_MAIN_TRPL,
	UI_MAIN_MATF,
	UI_MAIN_DECL,
	UI_MAIN_IMGO,
	UI_MAIN_IMGF,
	UI_MAIN_IMGR,
	UI_MAIN_RESX,
	UI_MAIN_RESY,
	UI_MAIN_TXNM,
	UI_MAIN_FREX,
	UI_MAIN_STEP,
	UI_MAIN_FSCL,
	UI_MAIN_MXKY,
	UI_MAIN_RMUK,
	UI_MAIN_CAMO,
	UI_MAIN_LGTO,
	UI_MAIN_BKIK,
	UI_MAIN_VIEW,
	UI_MAIN_VWNM,
	UI_MAIN_LAST
};

const int  UI_MAIN_NUMB = UI_MAIN_LAST + 1;


///////////////////////////////////////////////  PANEL CONTROL MACROS

#define UI_MAIN_CON(ui,c)			((ui)->_control_main[(c)])


///////////////////////////////////////////////  PANEL LAYOUT CONSTANTS

#define UI_CON_HIGH					25
#define UI_CON_BASE_X				5
#define UI_CON_BASE_Y				5
#define UI_CON_PARM_X				16
#define UI_CON_NEXT_X				300
#define UI_CON_OFF1					24

typedef int                    UI_TAB;
#define UI_TAB_EXPORT				0
#define UI_TAB_MESH					1
#define UI_TAB_MATERIAL				2
#define UI_TAB_TEXTURE				3
#define UI_TAB_ANIMATION			4
#define UI_TAB_PREVIEW				5


class dxUI
{
public:
	dxUI  ( dxOptions* options, GlobalFunc* global, void* local );
	~dxUI ();

	int  Open   ();
	void Close  ();
	void Update ();
											/*  EXPORT PARAMETERS  */

	dxOptions*            _options;
	int                   _obj_no;
	UI_TAB                _tab_no;

	GlobalFunc*           _global;
	void*                 _local;
	LWStateQueryFuncs*    _query;
	LWFileActivateFunc*   _filereq;
	LWMessageFuncs*       _msgfunc;
	LWImageList*          _imglist;
	LWImageUtil*          _imgutil;
	LWObjectFuncs*        _objfunc;
	LWPanelFuncs*         _panfunc;
	LWRasterFuncs*        _rasfunc;
	unsigned long         _locale;

	LWPanelID             _panelID;
	LWRasterID            _rasterID;
	LWControl*            _control_main[UI_MAIN_NUMB];


private:
	UI_TAB                _last_tab_no;
	int     Setup        ();
	void    Export       ();
	void    ExportModel  ();
	void    ExportScene  ();
	void    ConvertImages();
	void    CopyImages   ();
	bool    RunViewer    ();
	void    BakeKeyframes( SCN3_ID scn );
};


#endif /* _DXUI_H_  */

