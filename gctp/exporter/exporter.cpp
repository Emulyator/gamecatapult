/*
 * EXPORTER.CPP -- LightWave Plugins LayoutGenericClass
 *			   exporter source code for LayoutGenericClass.
 *
 *
 * このソースコードは、LayoutGenericClassのプラグインを作成するための
 * 雛形を提供するしています。exporterの接頭子で記述されている関数や構造体
 * の名称は必要に応じて名称変更して使用します。
 *
 */
#include "pch.h"
#include <fstream>

using namespace std;

/*
 *    XPanel の記述
 */

GlobalFunc *g_lwglobal = NULL;

typedef enum en_TPID {
	// some control IDs
	TPC_PARENT = 0x8001,
	TPC_SISTER,
	TPC_BROTHER,
	TPC_AUNT,
	TPC_UNCLE,
	// And some group IDs
	TPG_ENABLE,
	TPG_TAB
};

void TP_cb_notify(LWXPanelID pan, unsigned long cid, unsigned long vid, int event_type)
{
	int   ival = 0;
	float fval = (float)0.0;
	char *str_ptr = NULL;

	LWXPanelFuncs *lwxpf = NULL;

	if( !pan ) return;
	lwxpf = static_cast<LWXPanelFuncs *>( (*g_lwglobal) ( LWXPANELFUNCS_GLOBAL, GFUSE_TRANSIENT) );

	if( lwxpf ) {
		ival = FGETINT(lwxpf,pan,TPC_PARENT);
		fval = (float)FGETFLT(lwxpf,pan,TPC_SISTER);
		fval = (float)FGETFLT(lwxpf,pan,TPC_BROTHER);
		fval = (float)FGETFLT(lwxpf,pan,TPC_AUNT);
		fval = (float)FGETFLT(lwxpf,pan,TPC_UNCLE);
	}
}


/*
 *	LayoutGenericのエントリー関数
 */

XCALL_(static int)
activateLayoutGeneric( long version, GlobalFunc *global, LWLayoutGeneric *local, void *serverData )
{
	if (version != LWLAYOUTGENERIC_VERSION)
		return (AFUNC_BADVERSION);

	/*
	 *  LayoutGenericは、汎用的なクラスでさまざまな用途で使用することが可能です。
	 *  LayoutGenericのローカルであるLWLayoutGenericには、このクラスで使用可能な
	 *  以下の関数が用意されています。
	 *
	 *  saveScene		現在作業中のシーンを指定したファイルに保存します。
	 *
	 *  loadScene		指定したシーンファイルをレイアウトに読み込みます。
	 *
	 *  lookup			レイアウトで使用可能なコマンドの識別子を呼び出します。
	 *					使用可能なコマンドの一覧は、SaveCommandListコマンドを実行
	 *					してテキストファイルに書き出すことが可能です。
	 *
	 *  execute			レイアウトコマンドを実行します。
	 */

	LWXPanelID     pan   = NULL;
	LWXPanelFuncs *lwxpf = NULL;

	int ival = 0;
	float fval = 0;

	static LWXPanelControl ctrl_list[] = {
		{ TPC_PARENT,  "Parent",  "iBoolean" },
		{ TPC_SISTER,  "Sister",  "float" },
		{ TPC_BROTHER, "Brother", "distance" },
		{ TPC_AUNT,    "Aunt",    "percent" },
		{ TPC_UNCLE,   "Uncle",   "angle" },
		{0}
	};

	static LWXPanelDataDesc data_descrip[] = {
		{ TPC_PARENT,  "Parent",  "integer" },
		{ TPC_SISTER,  "Sister",  "float" },
		{ TPC_BROTHER, "Brother", "distance" },
		{ TPC_AUNT,    "Aunt",    "percent" },
		{ TPC_UNCLE,   "Uncle",   "angle" },
		{0},
	};

	static LWXPanelHint hint[] = {
		XpLABEL(0,"LWXPanel LayoutGeneric Example"),
		XpCHGNOTIFY(TP_cb_notify),
		{0}
	};

	XCALL_INIT;
	if(version < 2) return AFUNC_BADVERSION;

	g_lwglobal = global;

	lwxpf = static_cast<LWXPanelFuncs *>( (*global) ( LWXPANELFUNCS_GLOBAL, GFUSE_TRANSIENT) );
	if(!lwxpf) return AFUNC_BADGLOBAL;

	// Create panel
	pan = (*lwxpf->create)(LWXP_FORM, ctrl_list);

	if ( pan ) {

		// Describe data
		(*lwxpf->describe)( pan, data_descrip, NULL, NULL );

		// Apply some hints
		(*lwxpf->hint)(pan, 0, hint);

		// Some initial values
		FSETINT(lwxpf, pan, TPC_PARENT, 1);
		FSETFLT(lwxpf, pan, TPC_SISTER, 24);
		FSETFLT(lwxpf, pan, TPC_BROTHER, 10);
		FSETFLT(lwxpf, pan, TPC_AUNT, 0.55);
		FSETFLT(lwxpf, pan, TPC_UNCLE, 3.141592);

		// Open Panel
		if((*lwxpf->post)(pan)) {
			// Ok
			ival = FGETINT(lwxpf,pan,TPC_PARENT);
			fval = (float)FGETFLT(lwxpf,pan,TPC_SISTER);
			fval = (float)FGETFLT(lwxpf,pan,TPC_BROTHER);
			fval = (float)FGETFLT(lwxpf,pan,TPC_AUNT);
			fval = (float)FGETFLT(lwxpf,pan,TPC_UNCLE);
		}
		else {
			// Cancel
		}

		// Destroy panel
		(*lwxpf->destroy)(pan);

	} // End if panel

	return AFUNC_OK;
}


/*
 *	このプラグインサーバーのクラスと名称を宣言するために必要なグローバル変数
 */

extern "C" {
ServerTagInfo tag_info__[] = {
	{ "GameCatapult XFile Exporter", SRVTAG_USERNAME },
	{ "Export XFile for gctp", SRVTAG_BUTTONNAME|LANGID_USENGLISH },
	{ "Ｘファイルに保存", SRVTAG_BUTTONNAME|LANGID_JAPANESE },
	{ "file", SRVTAG_CMDGROUP },
	{ "GameCatapult XFile Exporter Plug-In", SRVTAG_DESCRIPTION|LANGID_USENGLISH },
	{ "GameCatapult Ｘファイル保存 プラグイン", SRVTAG_DESCRIPTION|LANGID_JAPANESE },
	{ NULL },
};
ServerRecord ServerDesc[] = {
	{ LWLAYOUTGENERIC_CLASS, "gctp_lwmod_export_xfile", ( ActivateFunc * )activateLayoutGeneric, tag_info__ },
	{ NULL }
};
}
