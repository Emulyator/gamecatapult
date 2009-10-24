/*
 * ACTIVATE.CPP -- DirectX Export
 *
 * Copyright (c) 2000-2001, D-STORM, Inc. 
 *
 * written by Yoshiaki Tazaki
 * revision  history  10/02/2001
 */


#include "dxui.h"


/******  PROTOTYPES  ******************************/



/*
 *  ModelerActivate   - DirectX Export for Modeler
 */

XCALL_(static int)
ModelerActivate( long version, GlobalFunc* global, LWModCommand* local, void* serverData )
{
	unsigned long    prodinfo;
	LWMessageFuncs*  msgfunc;

	XCALL_INIT;

	if (version != LWMODCOMMAND_VERSION)
		return AFUNC_BADVERSION;

	msgfunc  = (LWMessageFuncs *) global( LWMESSAGEFUNCS_GLOBAL, GFUSE_TRANSIENT );
	prodinfo = (unsigned long)    global( LWPRODUCTINFO_GLOBAL,  GFUSE_TRANSIENT );

	int ver = LWINF_GETMAJOR( prodinfo );
	int minor = LWINF_GETMINOR( prodinfo );
	int build = LWINF_GETBUILD( prodinfo );
	dbgprintf("ver %d.%d(%d)\n", ver, minor, build);
	if (LWINF_GETMAJOR( prodinfo ) <= 6 &&
		LWINF_GETBUILD( prodinfo ) < 443) {
		msgfunc->error("This plugin works on LW6.5B or later.", (const char *) NULL);
		return AFUNC_OK;
	}

	dxOptions  options = dxOptions( DX_STATUS_MODELER );
	dxUI       ui      = dxUI     ( &options, global, local );

	if (ui._obj_no >= 0) {
		//local->evaluate(local->data, "CalculateAllNormals 1");
		ui.Open();
		ui.Close();
		//local->evaluate(local->data, "CalculateAllNormals 0");
	}

	return AFUNC_OK;
}




/*
 *  LayoutActivate   - DirectX Export for Layout
 */

XCALL_(static int)
LayoutActivate( long version, GlobalFunc* global, LWLayoutGeneric* local, void* serverData )
{
	unsigned long    prodinfo;
	LWMessageFuncs*  msgfunc;

	XCALL_INIT;

	if (version != LWLAYOUTGENERIC_VERSION)
		return AFUNC_BADVERSION;

	msgfunc  = (LWMessageFuncs *) global( LWMESSAGEFUNCS_GLOBAL, GFUSE_TRANSIENT );
	prodinfo = (unsigned long)    global( LWPRODUCTINFO_GLOBAL,  GFUSE_TRANSIENT );

	int ver = LWINF_GETMAJOR( prodinfo );
	int minor = LWINF_GETMINOR( prodinfo );
	int build = LWINF_GETBUILD( prodinfo );
	dbgprintf("ver %d.%d(%d)\n", ver, minor, build);
	if (LWINF_GETMAJOR( prodinfo ) <= 6 &&
		LWINF_GETBUILD( prodinfo ) < 508) {
		msgfunc->error("This plugin works on LW6.5B or later.", (const char *) NULL);
		return AFUNC_OK;
	}

	dxOptions  options = dxOptions( DX_STATUS_LAYOUT );
	dxUI       ui      = dxUI     ( &options, global, local );

	if (ui._obj_no >= 0) {
		//local->evaluate(local->data, "CalculateAllNormals 1"); // �Ӗ��Ȃ�����
		ui.Open();
		ui.Close();
		//local->evaluate(local->data, "CalculateAllNormals 0");
	}

	return AFUNC_OK;
}


extern "C" {
ServerTagInfo ModelerTagInfo[] = {
	{ "gctp Export X File",                    SRVTAG_USERNAME|LANGID_USENGLISH },
	{ "gctp Export X File",                    SRVTAG_USERNAME|LANGID_JAPANESE  },
	{ "gctp Export X File",                    SRVTAG_BUTTONNAME },
	{ "gctp X File Exporter for LightWave 3D", SRVTAG_DESCRIPTION },
	{ "file",                             SRVTAG_CMDGROUP   },
    { (const char *) NULL },
};
ServerTagInfo LayoutTagInfo[] = {
	{ "gctp Export X File (.x)",               SRVTAG_USERNAME|LANGID_USENGLISH },
	{ "gctp Export X File (.x)",               SRVTAG_USERNAME|LANGID_JAPANESE  },
	{ "gctp Export X File (.x)",               SRVTAG_BUTTONNAME },
	{ "gctp X File Exporter for LightWave 3D", SRVTAG_DESCRIPTION },
	{ "file",                             SRVTAG_CMDGROUP   },
    { (const char *) NULL },
};
ServerRecord ServerDesc[] = {
    { LWMODCOMMAND_CLASS,    "gamecataput_lightwave_mod_plugin_ExportXFile", (ActivateFunc *) ModelerActivate, ModelerTagInfo },
    { LWLAYOUTGENERIC_CLASS, "gamecataput_lightwave_lay_plugin_ExportXFile", (ActivateFunc *) LayoutActivate,  LayoutTagInfo  },
    { (const char *) NULL },
};
}

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "DxErr9.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "quartz.lib")
#pragma comment(lib, "d3dx9.lib")
