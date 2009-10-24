/*
 * UI_MAIN.CPP -- DirectX Export
 *
 * Copyright (c) 2000-2001, D-STORM, Inc. 
 *
 * written by Yoshiaki Tazaki
 * revision  history  10/02/2001
 */

#include "dxui.h"
#include "ui_util.h"

/*****  STRUCTURE AND STATIC DATA DECLARATION  *****/

static char *menu[] = {
	"Export File (.x)",						// UI_MAIN_FILE
	"...",									// UI_MAIN_FBTN
	"Texture Directory",					// UI_MAIN_TXTD
	"...",									// UI_MAIN_TBTN
	"",										// UI_MAIN_TBMN
	"Version",								// UI_MAIN_VERS
	"Format",								// UI_MAIN_FORM
	"Export Materials",						// UI_MAIN_MATE
	"Export Animations",					// UI_MAIN_ANIM
	"Export Frames",						// UI_MAIN_FRAM
	"Frame Prefix",							// UI_MAIN_PREF
	"Export Mesh Normals",					// UI_MAIN_NORM
	"Export Texture Coordinates",			// UI_MAIN_TXUV
	"Reverse Texture V Coordinates",		// UI_MAIN_REVV
	"Export Vertex Colors",					// UI_MAIN_VCOL
	"Specify Max of SkinWeights/Vertex",	// UI_MAIN_SKWF
	"Max of SkinWeights/Vertex",			// UI_MAIN_MXWV
	"Max of SkinWeights/Face",				// UI_MAIN_MXWF
	"Triangulate All of Polygons",			// UI_MAIN_TRPL
	"Material Format",						// UI_MAIN_MATF
	"Replace Surface Color with Texture",	// UI_MAIN_DECL
	"Texture Images",						// UI_MAIN_IMGO
	"Image Format",							// UI_MAIN_IMGF
	"Resize to Power of 2",					// UI_MAIN_IMGR
	"Maximum Resolution X",					// UI_MAIN_RESX
	"Maximum Resolution Y",					// UI_MAIN_RESY
	"Texture Filename",						// UI_MAIN_TXNM
	"Frame to Export",						// UI_MAIN_FREX
	"Frame Steps",							// UI_MAIN_STEP
	"Time Scale Factor",					// UI_MAIN_FSCL
	"Use Matrix Key",						// UI_MAIN_MXKY
	"Remove Unusable Keyframes",			// UI_MAIN_RMUK
	"Export Camera Frames and Animations",	// UI_MAIN_CAMO
	"Export Light Frames and Animations",	// UI_MAIN_LGTO
	"Bake Fulltime IK Motions",				// UI_MAIN_BKIK
	"Run Viewer After Exporting",			// UI_MAIN_VIEW
	"Select Viewer",						// UI_MAIN_VWNM
};

static char *menuj[] = {
	"出力ファイル名 (.x)",					// UI_MAIN_FILE
	"...",									// UI_MAIN_FBTN
	"テクスチャフォルダ",						// UI_MAIN_TXTD
	"...",									// UI_MAIN_TBTN
	"",										// UI_MAIN_TBMN
	"バージョン",							// UI_MAIN_VERS
	"フォーマット",							// UI_MAIN_FORM
	"マテリアルを出力",						// UI_MAIN_MATE
	"モーションを出力",						// UI_MAIN_ANIM
	"階層を出力",							// UI_MAIN_FRAM
	"階層名の接頭文字列",						// UI_MAIN_PREF
	"メッシュ法線を出力",						// UI_MAIN_NORM
	"テクスチャ座標を出力",					// UI_MAIN_TXUV
	"テクスチャ座標Vの符号を反転",			// UI_MAIN_REVV
	"頂点カラーを出力",						// UI_MAIN_VCOL
	"1頂点あたりのスキンウェイト最大数を制限",	// UI_MAIN_SKWF
	"1頂点あたりのスキンウェイト最大数",		// UI_MAIN_MXWV
	"1ポリゴンあたりのスキンウェイト最大数",	// UI_MAIN_MXWF
	"ポリゴンをすべて三角形化",				// UI_MAIN_TRPL
	"マテリアル形式",						// UI_MAIN_MATF
	"サーフェス色をテクスチャで置き換え",		// UI_MAIN_DECL
	"テクスチャ画像",						// UI_MAIN_IMGO
	"画像フォーマット",						// UI_MAIN_IMGF
	"2の累乗にリサイズ",						// UI_MAIN_IMGR
	"横解像度の最大値",						// UI_MAIN_RESX
	"縦解像度の最大値",						// UI_MAIN_RESY
	"テクスチャファイル名",					// UI_MAIN_TXNM
	"出力するフレーム",						// UI_MAIN_FREX
	"フレームステップ数",						// UI_MAIN_STEP
	"時間の倍率",							// UI_MAIN_FSCL
	"モーションを行列で出力",					// UI_MAIN_MXKY
	"使用されていないキーを除去",				// UI_MAIN_RMUK
	"カメラの階層とモーションを出力",			// UI_MAIN_CAMO
	"ライトの階層とモーションを出力",			// UI_MAIN_LGTO
	"フルタイムIKのモーションを焼きこむ",		// UI_MAIN_BKIK
	"エキスポート後、ビューワを起動",			// UI_MAIN_VIEW
	"ビューワを指定",						// UI_MAIN_VWNM
};

const static char *menuTBMN[] = {			// UI_MAIN_TBMN
	"Export",
	"Mesh",
	"Material",
	"Texture",
	"Animation",
#ifdef _WIN32
	"Preview",
#endif
	(const char* ) NULL,
};

const static char *menuTBMNj[] = {			// UI_MAIN_TBMN
	"出力",
	"メッシュ",
	"マテリアル",
	"テクスチャ",
	"アニメーション",
#ifdef _WIN32
	"プレビュー",
#endif
	(const char* ) NULL,
};

const static char *menuVERS[] = {			// UI_MAIN_VERS
	"DirectX 9",
	(const char* ) NULL,
};

const static char *menuFORM[] = {			// UI_MAIN_FORM
	"Text",
	"Compressed Text",
	"Binary",
	"Compressed Binary",
	(const char* ) NULL,
};

const static char *menuFORMj[] = {			// UI_MAIN_FORM
	"テキスト",
	"圧縮テキスト",
	"バイナリ",
	"圧縮バイナリ",
	(const char* ) NULL,
};

const static char *menuMATF[] = {			// UI_MAIN_MATF
	"Inline",
	"External",
	"Shared External",
	(const char* ) NULL,
};

const static char *menuMATFj[] = {			// UI_MAIN_MATF
	"インライン",
	"参照",
	"共有して参照",
	(const char* ) NULL,
};

const static char *menuIMGR[] = {			// UI_MAIN_IMGR
	"Never",
	"Expand",
	"Shrink",
	"Nearest",
	(const char* ) NULL,
};

const static char *menuIMGRj[] = {			// UI_MAIN_IMGR
	"何もしない",
	"広げる",
	"縮める",
	"最近傍補間",
	(const char* ) NULL,
};

const static char *menuFREX[] = {			// UI_MAIN_FRAM
	"Keyframes Only",
	"Always Sampling",
	"Keyframes & Sampling",
	(const char* ) NULL,
};

const static char *menuFREXj[] = {			// UI_MAIN_FRAM
	"キーフレームのみ",
	"常にサンプリング",
	"キーフレーム＆サンプリング",
	(const char* ) NULL,
};

const static char *menuIMGO[] = {			// UI_MAIN_IMGO
	"Use Original Files",
	"Convert Image Format",
	"Copy to Texture Directory",
	(const char* ) NULL,
};

const static char *menuIMGOj[] = {			// UI_MAIN_IMGO
	"元のファイルを使用",
	"フォーマット変換",
	"テクスチャフォルダにコピー",
	(const char* ) NULL,
};

const static char *menuTXNM[] = {			// UI_MAIN_TXNM
	"File Bodyname",
#ifdef _WIN32
	"Absolute Path",
	"Relative Path",
#endif
	(const char* ) NULL,
};

const static char *menuTXNMj[] = {			// UI_MAIN_TXNM
	"ファイル名のみ",
#ifdef _WIN32
	"絶対パス",
	"相対パス",
#endif
	(const char* ) NULL,
};


/*****  DECLARATIONS FOR STATIC FUNCTIONS   *****/

typedef char* (*LWConNameFunc)(void *, int);
typedef int   (*LWConCountFunc)(void *);

static void  UI_MAIN_Draw ( LWPanelID panelID,  dxUI* ui, int draw );
static void  UI_FILE_Event( LWControl* control, dxUI* ui );
static void  UI_FBTN_Event( LWControl* control, dxUI* ui );
static void  UI_TXTD_Event( LWControl* control, dxUI* ui );
static void  UI_TBTN_Event( LWControl* control, dxUI* ui );
static void  UI_TBMN_Event( LWControl* control, dxUI* ui );
static void  UI_VERS_Event( LWControl* control, dxUI* ui );
static void  UI_FORM_Event( LWControl* control, dxUI* ui );
static void  UI_MATE_Event( LWControl* control, dxUI* ui );
static void  UI_ANIM_Event( LWControl* control, dxUI* ui );
static void  UI_FRAM_Event( LWControl* control, dxUI* ui );
static void  UI_PREF_Event( LWControl* control, dxUI* ui );
static void  UI_NORM_Event( LWControl* control, dxUI* ui );
static void  UI_TXUV_Event( LWControl* control, dxUI* ui );
static void  UI_REVV_Event( LWControl* control, dxUI* ui );
static void  UI_VCOL_Event( LWControl* control, dxUI* ui );
static void  UI_SKWF_Event( LWControl* control, dxUI* ui );
static void  UI_MXWV_Event( LWControl* control, dxUI* ui );
static void  UI_MXWF_Event( LWControl* control, dxUI* ui );
static void  UI_TRPL_Event( LWControl* control, dxUI* ui );
static void  UI_MATF_Event( LWControl* control, dxUI* ui );
static void  UI_DECL_Event( LWControl* control, dxUI* ui );
static void  UI_IMGO_Event( LWControl* control, dxUI* ui );
static void  UI_IMGF_Event( LWControl* control, dxUI* ui );
static int   UI_IMGF_Count( dxUI* ui );
static char *UI_IMGF_Name ( dxUI* ui, int n );
static void  UI_IMGR_Event( LWControl* control, dxUI* ui );
static void  UI_RESX_Event( LWControl* control, dxUI* ui );
static void  UI_RESY_Event( LWControl* control, dxUI* ui );
static void  UI_TXNM_Event( LWControl* control, dxUI* ui );
static void  UI_FREX_Event( LWControl* control, dxUI* ui );
static void  UI_STEP_Event( LWControl* control, dxUI* ui );
static void  UI_FSCL_Event( LWControl* control, dxUI* ui );
static void  UI_RMUK_Event( LWControl* control, dxUI* ui );
static void  UI_MXKY_Event( LWControl* control, dxUI* ui );
static void  UI_CAMO_Event( LWControl* control, dxUI* ui );
static void  UI_LGTO_Event( LWControl* control, dxUI* ui );
static void  UI_BKIK_Event( LWControl* control, dxUI* ui );
static void  UI_VIEW_Event( LWControl* control, dxUI* ui );
static void  UI_VWNM_Event( LWControl* control, dxUI* ui );

static bool  UI_TextureResolution( int *res );


/*
 *  dxUI       CONSTRUCTOR
 */
dxUI::dxUI( dxOptions* options, GlobalFunc* global, void* local )
{
	_tab_no    = UI_TAB_EXPORT;
	_last_tab_no = -1;
	_options   = options;
	_obj_no    = 0;
	_global    = global;
	_local     = local;
	_query     = (LWStateQueryFuncs  *)(*global)( LWSTATEQUERYFUNCS_GLOBAL , GFUSE_TRANSIENT );
	_filereq   = (LWFileActivateFunc *)(*global)( LWFILEACTIVATEFUNC_GLOBAL, GFUSE_TRANSIENT );
	_msgfunc   = (LWMessageFuncs     *)(*global)( LWMESSAGEFUNCS_GLOBAL    , GFUSE_TRANSIENT );
    _imglist   = (LWImageList        *)(*global)( LWIMAGELIST_GLOBAL       , GFUSE_TRANSIENT );
    _imgutil   = (LWImageUtil        *)(*global)( LWIMAGEUTIL_GLOBAL       , GFUSE_TRANSIENT );
    _objfunc   = (LWObjectFuncs      *)(*global)( LWOBJECTFUNCS_GLOBAL     , GFUSE_TRANSIENT );
	_rasfunc   = (LWRasterFuncs      *)(*global)( LWRASTERFUNCS_GLOBAL     , GFUSE_TRANSIENT );
	_panfunc   = (LWPanelFuncs       *)(*global)( LWPANELFUNCS_GLOBAL      , GFUSE_TRANSIENT );
	_locale    = ( unsigned long      )(*global)( LWLOCALEINFO_GLOBAL, GFUSE_TRANSIENT );
	_panelID   = PAN_CREATE( _panfunc, PROGRAM_NAME );

	if (options->status == DX_STATUS_MODELER)
	{
		const char *curfname = _query->object();
		int _obj_no;
		for(_obj_no = 0; _obj_no < _objfunc->numObjects(); _obj_no++) {
			if(strcmp(_objfunc->filename(_obj_no), curfname) == 0) break;
		}
		if (_obj_no >= _objfunc->numObjects()) {
			(*_msgfunc->error)( "Can't identify the current object.", "Please save the current object." );
		}
	}

	if (_obj_no >= 0) {
		options->reverseUVCoords = TRUE;
		options->LoadConfig( global );
		Setup();
	}
}


/*
 *  ~dxUI       DESTRUCTOR
 */
dxUI::~dxUI()
{
}


/*
 *  Open         OPEN THE MODEL UI
 */
int dxUI::Open()
{
	int  btn = (*_panfunc->open)( _panelID, PANF_BLOCKING|PANF_CANCEL );
	if (btn) Export();

	return btn;
}


/*
 *  Close          CLOSE THE MODEL UI
 */
void dxUI::Close()
{
#ifndef _MACOS
	(*_panfunc->destroy)( _panelID );
#endif
	_options->SaveConfig( _global );
}




/*
 *  Setup          SETUP MAIN CONTROLS
 */
int dxUI::Setup()
{
	int             x_pos, y_pos, lw;

	char **__menu = (_locale == LANGID_JAPANESE ? menuj : menu);
	const char **__menuTBMN = (_locale == LANGID_JAPANESE ? menuTBMNj : menuTBMN);
	const char **__menuFORM = (_locale == LANGID_JAPANESE ? menuFORMj : menuFORM);
	const char **__menuMATF = (_locale == LANGID_JAPANESE ? menuMATFj : menuMATF);
	const char **__menuIMGO = (_locale == LANGID_JAPANESE ? menuIMGOj : menuIMGO);
	const char **__menuIMGR = (_locale == LANGID_JAPANESE ? menuIMGRj : menuIMGR);
	const char **__menuTXNM = (_locale == LANGID_JAPANESE ? menuTXNMj : menuTXNM);
	const char **__menuFREX = (_locale == LANGID_JAPANESE ? menuFREXj : menuFREX);

	/*
	 *  CREATE CONTROLS
	 */
	_control_main[UI_MAIN_FILE] = STR_CTL        ( _panfunc, _panelID, __menu[UI_MAIN_FILE], 45 );
	_control_main[UI_MAIN_FBTN] = WBUTTON_CTL    ( _panfunc, _panelID, __menu[UI_MAIN_FBTN], 20  );
	_control_main[UI_MAIN_TXTD] = STR_CTL        ( _panfunc, _panelID, __menu[UI_MAIN_TXTD], 45 );
	_control_main[UI_MAIN_TBTN] = WBUTTON_CTL    ( _panfunc, _panelID, __menu[UI_MAIN_TBTN], 20  );
	_control_main[UI_MAIN_TBMN] = TABCHOICE_CTL  ( _panfunc, _panelID, __menu[UI_MAIN_TBMN], __menuTBMN );
	_control_main[UI_MAIN_VERS] = WPOPUP_CTL     ( _panfunc, _panelID, __menu[UI_MAIN_VERS], menuVERS, 80 );
	_control_main[UI_MAIN_FORM] = WPOPUP_CTL     ( _panfunc, _panelID, __menu[UI_MAIN_FORM], __menuFORM, 80 );
	_control_main[UI_MAIN_MATE] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_MATE] );
	_control_main[UI_MAIN_ANIM] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_ANIM] );
	_control_main[UI_MAIN_FRAM] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_FRAM] );
	_control_main[UI_MAIN_PREF] = STR_CTL        ( _panfunc, _panelID, __menu[UI_MAIN_PREF], 20 );
	_control_main[UI_MAIN_NORM] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_NORM] );
	_control_main[UI_MAIN_VCOL] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_VCOL] );
	_control_main[UI_MAIN_TXUV] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_TXUV] );
	_control_main[UI_MAIN_REVV] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_REVV] );
	_control_main[UI_MAIN_SKWF] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_SKWF] );
	_control_main[UI_MAIN_MXWV] = INT_CTL        ( _panfunc, _panelID, __menu[UI_MAIN_MXWV] );
	_control_main[UI_MAIN_MXWF] = INT_CTL        ( _panfunc, _panelID, __menu[UI_MAIN_MXWF] );
	_control_main[UI_MAIN_TRPL] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_TRPL] );
	_control_main[UI_MAIN_MATF] = WPOPUP_CTL     ( _panfunc, _panelID, __menu[UI_MAIN_MATF], __menuMATF, 110 );
	_control_main[UI_MAIN_DECL] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_DECL] );
	_control_main[UI_MAIN_IMGO] = WPOPUP_CTL     ( _panfunc, _panelID, __menu[UI_MAIN_IMGO], __menuIMGO, 160 );
	_control_main[UI_MAIN_IMGF] = CUSTPOPUP_CTL  ( _panfunc, _panelID, __menu[UI_MAIN_IMGF], 120, 
													(LWConNameFunc)  UI_IMGF_Name, 
													(LWConCountFunc) UI_IMGF_Count );
	_control_main[UI_MAIN_IMGR] = WPOPUP_CTL     ( _panfunc, _panelID, __menu[UI_MAIN_IMGR], __menuIMGR, 80 );
	_control_main[UI_MAIN_RESX] = INT_CTL        ( _panfunc, _panelID, __menu[UI_MAIN_RESX] );
	_control_main[UI_MAIN_RESY] = INT_CTL        ( _panfunc, _panelID, __menu[UI_MAIN_RESY] );
	_control_main[UI_MAIN_TXNM] = WPOPUP_CTL     ( _panfunc, _panelID, __menu[UI_MAIN_TXNM], __menuTXNM, 100 );
	_control_main[UI_MAIN_FREX] = WPOPUP_CTL     ( _panfunc, _panelID, __menu[UI_MAIN_FREX], __menuFREX, 140 );
	_control_main[UI_MAIN_STEP] = INT_CTL        ( _panfunc, _panelID, __menu[UI_MAIN_STEP] );
	_control_main[UI_MAIN_FSCL] = INT_CTL        ( _panfunc, _panelID, __menu[UI_MAIN_FSCL] );
	_control_main[UI_MAIN_RMUK] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_RMUK] );
	_control_main[UI_MAIN_MXKY] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_MXKY] );
	_control_main[UI_MAIN_CAMO] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_CAMO] );
	_control_main[UI_MAIN_LGTO] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_LGTO] );
	_control_main[UI_MAIN_BKIK] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_BKIK] );
	_control_main[UI_MAIN_VIEW] = BOOL_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_VIEW] );
	_control_main[UI_MAIN_VWNM] = FILE_CTL       ( _panfunc, _panelID, __menu[UI_MAIN_VWNM], 40 );

	/*
	 *  LAYOUT CONTROLS
	 */
	x_pos  = UI_CON_BASE_X;
	y_pos  = UI_CON_BASE_Y;
	MOVE_CON( _control_main[UI_MAIN_FILE], x_pos, y_pos );

	x_pos  = UI_CON_BASE_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_TXTD], x_pos, y_pos );
	lw     = CON_LW( _control_main[UI_MAIN_TXTD] );
	UI_UtilVAlign( lw, _control_main[UI_MAIN_FILE] );

	UI_UtilOneLine( _control_main[UI_MAIN_TXTD], 
	                _control_main[UI_MAIN_TBTN], 
	                (LWControl *) NULL, 
	                (LWControl *) NULL, 
	                (LWControl *) NULL, 
	                (LWControl *) NULL );
	UI_UtilOneLine( _control_main[UI_MAIN_FILE], 
	                _control_main[UI_MAIN_FBTN], 
	                (LWControl *) NULL, 
	                (LWControl *) NULL, 
	                (LWControl *) NULL, 
	                (LWControl *) NULL );

	x_pos  = UI_CON_BASE_X;
	y_pos += UI_CON_HIGH + 5;
	MOVE_CON( _control_main[UI_MAIN_TBMN], x_pos, y_pos );

											//  EXPORT
	x_pos  = UI_CON_PARM_X;
	y_pos  = UI_CON_BASE_Y + UI_CON_HIGH * 3 + 15;
	MOVE_CON( _control_main[UI_MAIN_VERS], x_pos, y_pos );
	UI_UtilOneLinePlus( _control_main[UI_MAIN_VERS], 
	                    _control_main[UI_MAIN_FORM], 
	                    (LWControl *) NULL, 
	                    (LWControl *) NULL, 
	                    (LWControl *) NULL, 
	                    (LWControl *) NULL, 10 );

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH + 10;
	MOVE_CON( _control_main[UI_MAIN_MATE], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_ANIM], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_FRAM], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X + UI_CON_OFF1;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_PREF], x_pos, y_pos );

											//  MESH
	x_pos  = UI_CON_PARM_X;
	y_pos  = UI_CON_BASE_Y + UI_CON_HIGH * 3 + 15;
	MOVE_CON( _control_main[UI_MAIN_NORM], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_VCOL], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_TXUV], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X + UI_CON_OFF1;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_REVV], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_SKWF], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X + UI_CON_OFF1;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_MXWV], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X + UI_CON_OFF1;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_MXWF], x_pos, y_pos );

	lw     = CON_LW( _control_main[UI_MAIN_MXWV] );
	UI_UtilVAlign( lw, _control_main[UI_MAIN_MXWF] );

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_TRPL], x_pos, y_pos );

											//  MATERIAL
	x_pos  = UI_CON_PARM_X;
	y_pos  = UI_CON_BASE_Y + UI_CON_HIGH * 3 + 15;
	MOVE_CON( _control_main[UI_MAIN_MATF], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_DECL], x_pos, y_pos );

											//  TEXTURE
	x_pos  = UI_CON_PARM_X;
	y_pos  = UI_CON_BASE_Y + UI_CON_HIGH * 3 + 15;
	MOVE_CON( _control_main[UI_MAIN_IMGO], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X + UI_CON_OFF1;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_IMGF], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X + UI_CON_OFF1;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_IMGR], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X + UI_CON_OFF1;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_RESX], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X + UI_CON_OFF1;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_RESY], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X + UI_CON_OFF1;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_TXNM], x_pos, y_pos );

											//  ANIMATION
	x_pos  = UI_CON_PARM_X;
	y_pos  = UI_CON_BASE_Y + UI_CON_HIGH * 3 + 15;
	MOVE_CON( _control_main[UI_MAIN_FREX], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_STEP], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_FSCL], x_pos, y_pos );

	if(_locale == LANGID_JAPANESE) {
		lw     = CON_LW( _control_main[UI_MAIN_STEP] );
		UI_UtilVAlign( lw, _control_main[UI_MAIN_FSCL] );
	}
	else {
		lw     = CON_LW( _control_main[UI_MAIN_FSCL] );
		UI_UtilVAlign( lw, _control_main[UI_MAIN_STEP] );
	}

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_RMUK], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_MXKY], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_CAMO], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_LGTO], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_BKIK], x_pos, y_pos );

											//  PREVIEW
	x_pos  = UI_CON_PARM_X;
	y_pos  = UI_CON_BASE_Y + UI_CON_HIGH * 3 + 15;
	MOVE_CON( _control_main[UI_MAIN_VIEW], x_pos, y_pos );

	x_pos  = UI_CON_PARM_X + UI_CON_OFF1;
	y_pos += UI_CON_HIGH;
	MOVE_CON( _control_main[UI_MAIN_VWNM], x_pos, y_pos );

	/*
	 *  CONTROL EVENT FUNCTIONS
	 */
	CON_SETEVENT( _control_main[UI_MAIN_FILE], UI_FILE_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_FBTN], UI_FBTN_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_TXTD], UI_TXTD_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_TBTN], UI_TBTN_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_TBMN], UI_TBMN_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_VERS], UI_VERS_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_FORM], UI_FORM_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_MATE], UI_MATE_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_ANIM], UI_ANIM_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_FRAM], UI_FRAM_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_PREF], UI_PREF_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_NORM], UI_NORM_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_TXUV], UI_TXUV_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_REVV], UI_REVV_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_VCOL], UI_VCOL_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_SKWF], UI_SKWF_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_MXWV], UI_MXWV_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_MXWF], UI_MXWF_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_TRPL], UI_TRPL_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_MATF], UI_MATF_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_DECL], UI_DECL_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_IMGO], UI_IMGO_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_IMGF], UI_IMGF_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_IMGR], UI_IMGR_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_RESX], UI_RESX_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_RESY], UI_RESY_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_TXNM], UI_TXNM_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_FREX], UI_FREX_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_STEP], UI_STEP_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_RMUK], UI_RMUK_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_FSCL], UI_FSCL_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_MXKY], UI_MXKY_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_CAMO], UI_CAMO_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_LGTO], UI_LGTO_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_BKIK], UI_BKIK_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_VIEW], UI_VIEW_Event, this );
	CON_SETEVENT( _control_main[UI_MAIN_VWNM], UI_VWNM_Event, this );

	/*
	 *  PANEL EVENT FUNCTIONS
	 */
	(*_panfunc->set)( _panelID, PAN_USERDATA, this );
	(*_panfunc->set)( _panelID, PAN_USERDRAW, UI_MAIN_Draw );

	PAN_SETH( _panfunc, _panelID, 350 );

	/*
	 *  UPDATE CONTROL PARAMETERS
	 */
	Update();

	return TRUE;
}

#define RENDERORREDRAW(c) (_last_tab_no == _tab_no ? REDRAW_CON(c) : RENDER_CON(c))

/*
 *  Update          UPDATE CONTROL PARAMETERS
 */
void dxUI::Update()
{
	if (_options->status == DX_STATUS_LAYOUT) {
		_options->outputFrames = TRUE;
	}

	SET_INT  ( _control_main[UI_MAIN_TBMN], _tab_no );

	SET_STR  ( _control_main[UI_MAIN_FILE], _options->outputFile, sizeof(DX_FNAM) );
	SET_STR  ( _control_main[UI_MAIN_TXTD], _options->textureDirectory, sizeof(DX_FNAM) );

	SET_INT  ( _control_main[UI_MAIN_VERS], _options->version );
	SET_INT  ( _control_main[UI_MAIN_FORM], _options->format );
	SET_INT  ( _control_main[UI_MAIN_MATE], _options->outputMaterials );
	SET_INT  ( _control_main[UI_MAIN_FRAM], _options->outputFrames );
	SET_STR  ( _control_main[UI_MAIN_PREF], _options->framePrefix, sizeof(DX_STRING) );
	
	SET_INT  ( _control_main[UI_MAIN_NORM], _options->outputMeshNormals );
	SET_INT  ( _control_main[UI_MAIN_TXUV], _options->outputTextureCoords );
	SET_INT  ( _control_main[UI_MAIN_REVV], _options->reverseUVCoords );
	SET_INT  ( _control_main[UI_MAIN_VCOL], _options->outputVertexColors );
	SET_INT  ( _control_main[UI_MAIN_TRPL], _options->tripleAll );

	SET_INT  ( _control_main[UI_MAIN_MATF], _options->materialType );
	SET_INT  ( _control_main[UI_MAIN_DECL], _options->enbaleDecalTexture );
	
	SET_INT  ( _control_main[UI_MAIN_IMGO], _options->outputTextureImages );
	SET_INT  ( _control_main[UI_MAIN_IMGF], _options->imageSaver );
	SET_INT  ( _control_main[UI_MAIN_IMGR], _options->resizeTextures );
	SET_INT  ( _control_main[UI_MAIN_RESX], _options->textureMaxResX );
	SET_INT  ( _control_main[UI_MAIN_RESY], _options->textureMaxResY );
	SET_INT  ( _control_main[UI_MAIN_TXNM], _options->textureFilename );

	if (_options->status == DX_STATUS_LAYOUT) {
		SET_INT  ( _control_main[UI_MAIN_ANIM], _options->outputAnimations );
		SET_INT  ( _control_main[UI_MAIN_FREX], _options->frameToExport );
		SET_INT  ( _control_main[UI_MAIN_STEP], _options->frameSteps );
		SET_INT  ( _control_main[UI_MAIN_RMUK], _options->removeUnusableKeyframes );
		SET_INT  ( _control_main[UI_MAIN_FSCL], _options->frameTimeScale );
		SET_INT  ( _control_main[UI_MAIN_MXKY], _options->useMatrixKey );
		SET_INT  ( _control_main[UI_MAIN_CAMO], _options->outputCameraFrames );
		SET_INT  ( _control_main[UI_MAIN_LGTO], _options->outputLightFrames );
		SET_INT  ( _control_main[UI_MAIN_BKIK], _options->bakeIKMotions );
		SET_INT  ( _control_main[UI_MAIN_SKWF], _options->useSkinWeightsPerVertex );
		SET_INT  ( _control_main[UI_MAIN_MXWV], _options->nMaxSkinWeightsPerVertex );
		SET_INT  ( _control_main[UI_MAIN_MXWF], _options->nMaxSkinWeightsPerFace );
	}

	SET_INT  ( _control_main[UI_MAIN_VIEW], _options->enableViewer );
	SET_STR  ( _control_main[UI_MAIN_VWNM], _options->viewerName, sizeof(DX_STRING) );

	UI_IMGF_Event( _control_main[UI_MAIN_IMGF], this );

											//  EXPORT
	if (_tab_no == UI_TAB_EXPORT)
	{
		RENDERORREDRAW( _control_main[UI_MAIN_VERS] );
		RENDERORREDRAW( _control_main[UI_MAIN_FORM] );
		RENDERORREDRAW( _control_main[UI_MAIN_MATE] );
		RENDERORREDRAW( _control_main[UI_MAIN_ANIM] );
		RENDERORREDRAW( _control_main[UI_MAIN_FRAM] );
		RENDERORREDRAW( _control_main[UI_MAIN_PREF] );

		if (_options->outputFrames) {
			UNGHOST_CON( _control_main[UI_MAIN_PREF] );
		} else {
			GHOST_CON  ( _control_main[UI_MAIN_PREF] );
		}
		if (_options->status == DX_STATUS_LAYOUT) {
			UNGHOST_CON( _control_main[UI_MAIN_ANIM] );
		} else {
			GHOST_CON  ( _control_main[UI_MAIN_ANIM] );
		}
	}
	else if(_last_tab_no == -1 || _last_tab_no == UI_TAB_EXPORT)
	{
		ERASE_CON( _control_main[UI_MAIN_VERS] );
		ERASE_CON( _control_main[UI_MAIN_FORM] );
		ERASE_CON( _control_main[UI_MAIN_MATE] );
		ERASE_CON( _control_main[UI_MAIN_ANIM] );
		ERASE_CON( _control_main[UI_MAIN_FRAM] );
		ERASE_CON( _control_main[UI_MAIN_PREF] );
	}

											//  MESH
	if (_tab_no == UI_TAB_MESH)
	{
		RENDERORREDRAW( _control_main[UI_MAIN_NORM] );
		RENDERORREDRAW( _control_main[UI_MAIN_TXUV] );
		RENDERORREDRAW( _control_main[UI_MAIN_REVV] );
		RENDERORREDRAW( _control_main[UI_MAIN_VCOL] );
		RENDERORREDRAW( _control_main[UI_MAIN_SKWF] );
		RENDERORREDRAW( _control_main[UI_MAIN_MXWV] );
		RENDERORREDRAW( _control_main[UI_MAIN_MXWF] );
		RENDERORREDRAW( _control_main[UI_MAIN_TRPL] );
	
		if (_options->outputTextureCoords == TRUE) {
			UNGHOST_CON( _control_main[UI_MAIN_REVV] );
		} else {
			GHOST_CON  ( _control_main[UI_MAIN_REVV] );
		}
		if (_options->status  == DX_STATUS_LAYOUT) {
			UNGHOST_CON( _control_main[UI_MAIN_SKWF] );
			if (_options->useSkinWeightsPerVertex == TRUE) {
				UNGHOST_CON( _control_main[UI_MAIN_MXWV] );
				UNGHOST_CON( _control_main[UI_MAIN_MXWF] );
			} else {
				GHOST_CON  ( _control_main[UI_MAIN_MXWV] );
				GHOST_CON  ( _control_main[UI_MAIN_MXWF] );
			}
		} else {
			GHOST_CON  ( _control_main[UI_MAIN_SKWF] );
			GHOST_CON  ( _control_main[UI_MAIN_MXWV] );
			GHOST_CON  ( _control_main[UI_MAIN_MXWF] );
		}
	}
	else if(_last_tab_no == -1 || _last_tab_no == UI_TAB_MESH)
	{
		ERASE_CON( _control_main[UI_MAIN_NORM] );
		ERASE_CON( _control_main[UI_MAIN_TXUV] );
		ERASE_CON( _control_main[UI_MAIN_REVV] );
		ERASE_CON( _control_main[UI_MAIN_VCOL] );
		ERASE_CON( _control_main[UI_MAIN_SKWF] );
		ERASE_CON( _control_main[UI_MAIN_MXWV] );
		ERASE_CON( _control_main[UI_MAIN_MXWF] );
		ERASE_CON( _control_main[UI_MAIN_TRPL] );
	}

											//  MATERIAL
	if (_tab_no == UI_TAB_MATERIAL)
	{
		RENDERORREDRAW( _control_main[UI_MAIN_MATF] );
		RENDERORREDRAW( _control_main[UI_MAIN_DECL] );
		if (_options->outputMaterials) {
			UNGHOST_CON( _control_main[UI_MAIN_MATF] );
			UNGHOST_CON( _control_main[UI_MAIN_DECL] );
		} else {
			GHOST_CON  ( _control_main[UI_MAIN_MATF] );
			GHOST_CON  ( _control_main[UI_MAIN_DECL] );
		}
	}
	else if(_last_tab_no == -1 || _last_tab_no == UI_TAB_MATERIAL)
	{
		ERASE_CON( _control_main[UI_MAIN_MATF] );
		ERASE_CON( _control_main[UI_MAIN_DECL] );
	}


											//  TEXTURE
	if (_tab_no == UI_TAB_TEXTURE)
	{
		RENDERORREDRAW( _control_main[UI_MAIN_IMGO] );
		RENDERORREDRAW( _control_main[UI_MAIN_IMGF] );
		RENDERORREDRAW( _control_main[UI_MAIN_IMGR] );
		RENDERORREDRAW( _control_main[UI_MAIN_RESX] );
		RENDERORREDRAW( _control_main[UI_MAIN_RESY] );
		RENDERORREDRAW( _control_main[UI_MAIN_TXNM] );

		if (_options->outputTextureImages == DX_TXTOUT_CONVERT) {
			UNGHOST_CON( _control_main[UI_MAIN_IMGF] );
			UNGHOST_CON( _control_main[UI_MAIN_IMGR] );
			UNGHOST_CON( _control_main[UI_MAIN_RESX] );
			UNGHOST_CON( _control_main[UI_MAIN_RESY] );
		} else {
			GHOST_CON  ( _control_main[UI_MAIN_IMGF] );
			GHOST_CON  ( _control_main[UI_MAIN_IMGR] );
			GHOST_CON  ( _control_main[UI_MAIN_RESX] );
			GHOST_CON  ( _control_main[UI_MAIN_RESY] );
		}
	}
	else if(_last_tab_no == -1 || _last_tab_no == UI_TAB_TEXTURE)
	{
		ERASE_CON( _control_main[UI_MAIN_IMGO] );
		ERASE_CON( _control_main[UI_MAIN_IMGF] );
		ERASE_CON( _control_main[UI_MAIN_IMGR] );
		ERASE_CON( _control_main[UI_MAIN_RESX] );
		ERASE_CON( _control_main[UI_MAIN_RESY] );
		ERASE_CON( _control_main[UI_MAIN_TXNM] );
	}

											//  ANIMATION
	if (_tab_no == UI_TAB_ANIMATION)
	{
		RENDERORREDRAW( _control_main[UI_MAIN_FREX] );
		RENDERORREDRAW( _control_main[UI_MAIN_STEP] );
		RENDERORREDRAW( _control_main[UI_MAIN_RMUK] );
		RENDERORREDRAW( _control_main[UI_MAIN_FSCL] );
		RENDERORREDRAW( _control_main[UI_MAIN_MXKY] );
		RENDERORREDRAW( _control_main[UI_MAIN_CAMO] );
		RENDERORREDRAW( _control_main[UI_MAIN_LGTO] );
		RENDERORREDRAW( _control_main[UI_MAIN_BKIK] );

		if (_options->status == DX_STATUS_LAYOUT) {
			UNGHOST_CON( _control_main[UI_MAIN_CAMO] );
			UNGHOST_CON( _control_main[UI_MAIN_LGTO] );
			UNGHOST_CON( _control_main[UI_MAIN_BKIK] );
			if (_options->outputAnimations) {
				UNGHOST_CON( _control_main[UI_MAIN_FREX] );
				UNGHOST_CON( _control_main[UI_MAIN_RMUK] );
				if (_options->frameToExport == DX_FRM2EXP_KEYFRAMEONLY) {
					GHOST_CON  ( _control_main[UI_MAIN_STEP] );
				} else {
					UNGHOST_CON( _control_main[UI_MAIN_STEP] );
				}
			} else {
				GHOST_CON  ( _control_main[UI_MAIN_FREX] );
				GHOST_CON  ( _control_main[UI_MAIN_STEP] );
				GHOST_CON  ( _control_main[UI_MAIN_RMUK] );
			}
		} else {
			GHOST_CON  ( _control_main[UI_MAIN_FREX] );
			GHOST_CON  ( _control_main[UI_MAIN_STEP] );
			GHOST_CON  ( _control_main[UI_MAIN_RMUK] );
			GHOST_CON  ( _control_main[UI_MAIN_FSCL] );
			GHOST_CON  ( _control_main[UI_MAIN_MXKY] );
			GHOST_CON  ( _control_main[UI_MAIN_CAMO] );
			GHOST_CON  ( _control_main[UI_MAIN_LGTO] );
			GHOST_CON  ( _control_main[UI_MAIN_BKIK] );
		}
	}
	else if(_last_tab_no == -1 || _last_tab_no == UI_TAB_ANIMATION)
	{
		ERASE_CON( _control_main[UI_MAIN_FREX] );
		ERASE_CON( _control_main[UI_MAIN_STEP] );
		ERASE_CON( _control_main[UI_MAIN_RMUK] );
		ERASE_CON( _control_main[UI_MAIN_FSCL] );
		ERASE_CON( _control_main[UI_MAIN_MXKY] );
		ERASE_CON( _control_main[UI_MAIN_CAMO] );
		ERASE_CON( _control_main[UI_MAIN_LGTO] );
		ERASE_CON( _control_main[UI_MAIN_BKIK] );
	}

											//  PREVIEW
	if (_tab_no == UI_TAB_PREVIEW)
	{
		RENDERORREDRAW( _control_main[UI_MAIN_VIEW] );
		RENDERORREDRAW( _control_main[UI_MAIN_VWNM] );

		if (_options->enableViewer) {
			UNGHOST_CON( _control_main[UI_MAIN_VWNM] );
		} else {
			GHOST_CON  ( _control_main[UI_MAIN_VWNM] );
		}
	}
	else if(_last_tab_no == -1 || _last_tab_no == UI_TAB_PREVIEW)
	{
		ERASE_CON( _control_main[UI_MAIN_VIEW] );
		ERASE_CON( _control_main[UI_MAIN_VWNM] );
	}

	_last_tab_no = _tab_no;
}


/*
 *  UI_MAIN_Draw            PANEL DRAW FUNCTION
 */
static void UI_MAIN_Draw( LWPanelID panelID, dxUI* ui, int draw )
{
	DrawFuncs		*drawfunc;
	int				x_pos, y_pos, x_siz, y_siz;

	drawfunc = ui->_panfunc->drawFuncs;

	x_pos = 0;
	y_pos = CON_HOTY( UI_MAIN_CON(ui,UI_MAIN_TBMN) ) + 19;
	x_siz = PAN_GETW( ui->_panfunc, panelID );;
	y_siz = 0;
	(*drawfunc->drawBorder)( panelID, 0, x_pos, y_pos, x_siz, y_siz );
}


/*
 *  UI_FILE_Event           FILE EVENT FUNCTION
 */
static void UI_FILE_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_STR( control, opts->outputFile, sizeof(DX_FNAM) );
}


/*
 *  UI_FBTN_Event           FBTN EVENT FUNCTION
 */
static void UI_FBTN_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts    = ui->_options;
	LWFileActivateFunc* filereq = ui->_filereq;
	LWFileReqLocal      frloc;
	PathConvParam       param;
	DX_FNAM             name;
	int                 result;

	param.path_in   = opts->outputFile;
	param.directory = opts->exportDirectory;
	param.name      = name;
	PathConvert( PATHCONV_SPLITPATH, &param );

	frloc.reqType  = FREQ_SAVE;
	if (opts->status  == DX_STATUS_LAYOUT) {
		frloc.title = "Save Scene As...";
	} else {
		frloc.title = "Save Object As...";
	}
	frloc.bufLen   = sizeof(DX_FNAM);
	frloc.pickName = 0;
	frloc.fileType = "XFile (.x)";
	frloc.path     = opts->exportDirectory;
	frloc.baseName = name;
	frloc.fullName = opts->outputFile;
	result = filereq( LWFILEREQ_VERSION, &frloc );
	if (result == AFUNC_OK && frloc.result > 0 ) {
		SET_STR( UI_MAIN_CON(ui,UI_MAIN_FILE), opts->outputFile, sizeof(DX_FNAM) );
	}
}


/*
 *  UI_TXTD_Event           TXTD EVENT FUNCTION
 */
static void UI_TXTD_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_STR( control, opts->textureDirectory, sizeof(DX_FNAM) );
}


/*
 *  UI_TBTN_Event           TBTN EVENT FUNCTION
 */
static void UI_TBTN_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts    = ui->_options;
	LWFileActivateFunc* filereq = ui->_filereq;
	LWFileReqLocal      frloc;
	DX_FNAM             file, name;
	int                 result;

	file[0] = 0x00;
	name[0] = 0x00;
	frloc.reqType  = FREQ_DIRECTORY; 
	frloc.title    = "Set Texture Directory";
	frloc.bufLen   = sizeof(DX_FNAM);
	frloc.pickName = 0;
	frloc.fileType = "Directory";
	frloc.path     = opts->textureDirectory;
	frloc.baseName = name;
	frloc.fullName = file;
	result = filereq( LWFILEREQ_VERSION, &frloc );
	if (result == AFUNC_OK && frloc.result > 0 ) {
		SET_STR( UI_MAIN_CON(ui,UI_MAIN_TXTD), opts->textureDirectory, sizeof(DX_FNAM) );
	}
}


/*
 *  UI_TBMN_Event           TBMN EVENT FUNCTION
 */
static void UI_TBMN_Event( LWControl* control, dxUI* ui )
{
	GET_INT( control, ui->_tab_no );
	ui->Update();
}


/*
 *  UI_VERS_Event           VERS EVENT FUNCTION
 */
static void UI_VERS_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->version );
}


/*
 *  UI_FORM_Event           FORM EVENT FUNCTION
 */
static void UI_FORM_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->format );
}


/*
 *  UI_MATE_Event           MATE EVENT FUNCTION
 */
static void UI_MATE_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->outputMaterials );
}



/*
 *  UI_ANIM_Event           ANIM EVENT FUNCTION
 */
static void UI_ANIM_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->outputAnimations );
	ui->Update();
}


/*
 *  UI_FRAM_Event           FRAM EVENT FUNCTION
 */
static void UI_FRAM_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->outputFrames );
	ui->Update();
}


/*
 *  UI_PREF_Event           PREF EVENT FUNCTION
 */
static void UI_PREF_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_STR( control, opts->framePrefix, sizeof(DX_STRING) );
}

/*
 *  UI_NORM_Event           NORM EVENT FUNCTION
 */
static void UI_NORM_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->outputMeshNormals );
}


/*
 *  UI_TXUV_Event           TXUV EVENT FUNCTION
 */
static void UI_TXUV_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->outputTextureCoords );
	ui->Update();
}


/*
 *  UI_REVV_Event           REVV EVENT FUNCTION
 */
static void UI_REVV_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->reverseUVCoords );
}


/*
 *  UI_VCOL_Event           VCOL EVENT FUNCTION
 */
static void UI_VCOL_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->outputVertexColors );
}


/*
 *  UI_SKWF_Event           SKWF EVENT FUNCTION
 */
static void UI_SKWF_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->useSkinWeightsPerVertex );
	ui->Update();
}


/*
 *  UI_MXWV_Event           MXWV EVENT FUNCTION
 */
static void UI_MXWV_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->nMaxSkinWeightsPerVertex );
	if (opts->nMaxSkinWeightsPerVertex < 1) opts->nMaxSkinWeightsPerVertex = 1;
}


/*
 *  UI_MXWF_Event           MXWF EVENT FUNCTION
 */
static void UI_MXWF_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->nMaxSkinWeightsPerFace );
	if (opts->nMaxSkinWeightsPerFace < 1) opts->nMaxSkinWeightsPerFace = 1;
}


/*
 *  UI_TRPL_Event           TRPL EVENT FUNCTION
 */
static void UI_TRPL_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->tripleAll );
}


/*
 *  UI_MATF_Event           MATF EVENT FUNCTION
 */
static void UI_MATF_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->materialType );
}


/*
 *  UI_DECL_Event           DECL EVENT FUNCTION
 */
static void UI_DECL_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->enbaleDecalTexture );
}



/*
 *  UI_IMGO_Event           IMGO EVENT FUNCTION
 */
static void UI_IMGO_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->outputTextureImages );
	if (opts->outputTextureImages == FALSE) {
		opts->imageExtension[0] = 0x00;
	}
	ui->Update();
}


/*
 *  UI_IMGF_Event           IMGF EVENT FUNCTION
 */
static void UI_IMGF_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts    = ui->_options;
	LWImageUtil*        imgutil = ui->_imgutil;
	const char*         saver;
	const char*         p;

	GET_INT( control, opts->imageSaver );
	saver = (*imgutil->saverName)( opts->imageSaver );
	if ((p = strrchr( saver, '.' ))) {
		strcpy( opts->imageExtension, p );
		if ((p = strrchr( opts->imageExtension, '.'))) *(char *)p = '.';
		if ((p = strrchr( opts->imageExtension, ')'))) *(char *)p = '\0';
	}
}


/*
 *  UI_IMGF_Count           IMGF COUNT FUNCTION
 */
static int  UI_IMGF_Count( dxUI* ui )
{
	LWImageUtil*        imgutil = ui->_imgutil;

	return (*imgutil->saverCount)();
}


/*
 *  UI_IMGF_Name           IMGF NAME FUNCTION
 */
static char *UI_IMGF_Name( dxUI* ui, int n )
{
	LWImageUtil*        imgutil = ui->_imgutil;

	return (char *) (*imgutil->saverName)(n);
}


/*
 *  UI_IMGR_Event           IMGR EVENT FUNCTION
 */
static void UI_IMGR_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->resizeTextures );
}


/*
 *  UI_RESX_Event           RESX EVENT FUNCTION
 */
static void UI_RESX_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;
	int                 size;

	GET_INT( control, size );

	if (UI_TextureResolution( &size )) {
		opts->textureMaxResX = size;
	} else {
		ui->Update();
	}
}


/*
 *  UI_RESY_Event           RESY EVENT FUNCTION
 */
static void UI_RESY_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;
	int                 size;

	GET_INT( control, size );

	if (UI_TextureResolution( &size )) {
		opts->textureMaxResY = size;
	} else {
		ui->Update();
	}
}


/*
 *  UI_TXNM_Event           TXNM EVENT FUNCTION
 */
static void UI_TXNM_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->textureFilename );
}


/*
 *  UI_FREX_Event           FREX EVENT FUNCTION
 */
static void UI_FREX_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->frameToExport );
	if (opts->frameToExport == DX_FRM2EXP_KEYFRAMEONLY) {
		opts->bakeIKMotions = FALSE;
	}
	ui->Update();
}


/*
 *  UI_STEP_Event           STEP EVENT FUNCTION
 */
static void UI_STEP_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->frameSteps );
	if (opts->frameSteps < 1) opts->frameSteps = 1;
}


/*
 *  UI_RMUK_Event           RMUK EVENT FUNCTION
 */
static void UI_RMUK_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->removeUnusableKeyframes );
}


/*
 *  UI_FSCL_Event           FSCL EVENT FUNCTION
 */
static void UI_FSCL_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->frameTimeScale );
}



/*
 *  UI_MXKY_Event           MXKY EVENT FUNCTION
 */
static void UI_MXKY_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->useMatrixKey );
}


/*
 *  UI_CAMO_Event           CAMO EVENT FUNCTION
 */
static void UI_CAMO_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->outputCameraFrames );
}


/*
 *  UI_LGTO_Event           LGTO EVENT FUNCTION
 */
static void UI_LGTO_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->outputLightFrames );
}


/*
 *  UI_BKIK_Event           BKIK EVENT FUNCTION
 */
static void UI_BKIK_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->bakeIKMotions );
	if (opts->bakeIKMotions && opts->frameToExport == DX_FRM2EXP_KEYFRAMEONLY) {
		opts->frameToExport = DX_FRM2EXP_KEYANDSAMPLE;
	}
	ui->Update();
}


/*
 *  UI_VIEW_Event           VIEW EVENT FUNCTION
 */
static void UI_VIEW_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_INT( control, opts->enableViewer );
	ui->Update();
}


/*
 *  UI_VWNM_Event           VWNM EVENT FUNCTION
 */
static void UI_VWNM_Event( LWControl* control, dxUI* ui )
{
	dxOptions*          opts = ui->_options;

	GET_STR( control, opts->viewerName, sizeof(DX_STRING) );
}


/*
 *  UI_TextureResolution    CHECK TEXTURE RESOLUTION
 */
static bool UI_TextureResolution( int *res )
{
	int    n = 0, size, count = 31;

	if (*res < 1) return false;

	while (count--) {
		size = 1 << n; n++;
		if (size == (*res)) {
			return true;
		}
		else if (size > (*res)) {
			*res = size;
			return false;
		}
	}
	return false;
}

