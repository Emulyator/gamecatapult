/*
 * EXPORTER.CPP -- LightWave Plugins LayoutGenericClass
 *			   exporter source code for LayoutGenericClass.
 *
 *
 * ���̃\�[�X�R�[�h�́ALayoutGenericClass�̃v���O�C�����쐬���邽�߂�
 * ���`��񋟂��邵�Ă��܂��Bexporter�̐ړ��q�ŋL�q����Ă���֐���\����
 * �̖��͕̂K�v�ɉ����Ė��̕ύX���Ďg�p���܂��B
 *
 */
#include "pch.h"
#include <fstream>

using namespace std;

/*
 *    XPanel �̋L�q
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
 *	LayoutGeneric�̃G���g���[�֐�
 */

XCALL_(static int)
activateLayoutGeneric( long version, GlobalFunc *global, LWLayoutGeneric *local, void *serverData )
{
	if (version != LWLAYOUTGENERIC_VERSION)
		return (AFUNC_BADVERSION);

	/*
	 *  LayoutGeneric�́A�ėp�I�ȃN���X�ł��܂��܂ȗp�r�Ŏg�p���邱�Ƃ��\�ł��B
	 *  LayoutGeneric�̃��[�J���ł���LWLayoutGeneric�ɂ́A���̃N���X�Ŏg�p�\��
	 *  �ȉ��̊֐����p�ӂ���Ă��܂��B
	 *
	 *  saveScene		���ݍ�ƒ��̃V�[�����w�肵���t�@�C���ɕۑ����܂��B
	 *
	 *  loadScene		�w�肵���V�[���t�@�C�������C�A�E�g�ɓǂݍ��݂܂��B
	 *
	 *  lookup			���C�A�E�g�Ŏg�p�\�ȃR�}���h�̎��ʎq���Ăяo���܂��B
	 *					�g�p�\�ȃR�}���h�̈ꗗ�́ASaveCommandList�R�}���h�����s
	 *					���ăe�L�X�g�t�@�C���ɏ����o�����Ƃ��\�ł��B
	 *
	 *  execute			���C�A�E�g�R�}���h�����s���܂��B
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
 *	���̃v���O�C���T�[�o�[�̃N���X�Ɩ��̂�錾���邽�߂ɕK�v�ȃO���[�o���ϐ�
 */

extern "C" {
ServerTagInfo tag_info__[] = {
	{ "GameCatapult XFile Exporter", SRVTAG_USERNAME },
	{ "Export XFile for gctp", SRVTAG_BUTTONNAME|LANGID_USENGLISH },
	{ "�w�t�@�C���ɕۑ�", SRVTAG_BUTTONNAME|LANGID_JAPANESE },
	{ "file", SRVTAG_CMDGROUP },
	{ "GameCatapult XFile Exporter Plug-In", SRVTAG_DESCRIPTION|LANGID_USENGLISH },
	{ "GameCatapult �w�t�@�C���ۑ� �v���O�C��", SRVTAG_DESCRIPTION|LANGID_JAPANESE },
	{ NULL },
};
ServerRecord ServerDesc[] = {
	{ LWLAYOUTGENERIC_CLASS, "gctp_lwmod_export_xfile", ( ActivateFunc * )activateLayoutGeneric, tag_info__ },
	{ NULL }
};
}
