/*
 * UI_UTIL.CPP -- DirectX Export
 *
 * Copyright (c) 2000-2001, D-STORM, Inc. 
 *
 * written by Yoshiaki Tazaki
 * revision  history  10/02/2001
 */

#include "ui_util.h"

/*****  DECLARATIONS FOR STATIC FUNCTIONS   *****/


/*
 *  UI_UtilOneLine          ALIGN CONTROLS TO ONE LINE
 */
void UI_UtilOneLine( LWControl *con1, LWControl *con2, LWControl *con3,
                     LWControl *con4, LWControl *con5, LWControl *con6 )
{
	int          x, y;

	if(con1 && con2)
	{
		x = CON_X(con1);
		y = CON_Y(con1);
		x += CON_W(con1);
		MOVE_CON(con2,x,y);
		if(!con3) return;
		x += CON_W(con2);
		MOVE_CON(con3,x,y);
		if(!con4) return;
		x += CON_W(con3);
		MOVE_CON(con4,x,y);
		if(!con5) return;
		x += CON_W(con4);
		MOVE_CON(con5,x,y);
		if(!con6) return;
		x += CON_W(con5);
		MOVE_CON(con6,x,y);
	}
}



/*
 *  UI_UtilOneLinePlus       ALIGN CONTROLS TO ONE LINE
 */
void UI_UtilOneLinePlus( LWControl *con1, LWControl *con2, LWControl *con3,
                         LWControl *con4, LWControl *con5, LWControl *con6, int spc )
{
	int          x, y;

	if(con1 && con2)
	{
		x = CON_X(con1);
		y = CON_Y(con1);
		x += CON_W(con1) + spc;
		MOVE_CON(con2,x,y);
		if(!con3) return;
		x += CON_W(con2) + spc;
		MOVE_CON(con3,x,y);
		if(!con4) return;
		x += CON_W(con3) + spc;
		MOVE_CON(con4,x,y);
		if(!con5) return;
		x += CON_W(con4) + spc;
		MOVE_CON(con5,x,y);
		if(!con6) return;
		x += CON_W(con5) + spc;
		MOVE_CON(con6,x,y);
	}
}



/*
 *  UI_UtilEndLine        ALIGN CONTROLS TO ONE LINE FROM END
 */
void UI_UtilEndLine( LWControl *con1, LWControl *con2, LWControl *con3,
                     LWControl *con4, LWControl *con5, LWControl *con6, int xw )
{
	int          x, y;

	if(con1 && con2)
	{
		x  = xw;
		x -= CON_W(con1);
		y  = CON_Y(con1);
		MOVE_CON(con1,x,y);
		if(!con2) return;
		x -= CON_W(con2);
		MOVE_CON(con2,x,y);
		if(!con3) return;
		x -= CON_W(con3);
		MOVE_CON(con3,x,y);
		if(!con4) return;
		x -= CON_W(con4);
		MOVE_CON(con4,x,y);
		if(!con5) return;
		x -= CON_W(con5);
		MOVE_CON(con5,x,y);
		if(!con6) return;
		x -= CON_W(con6);
		MOVE_CON(con6,x,y);
	}
}




/*
 *  UI_UtilEndLine        ALIGN CONTROLS TO ONE LINE FROM END
 */
void UI_UtilVAlign( int lw, LWControl *con )
{
	int          x, y;

	if(con)
	{
		x = CON_X(con);
		x += lw - CON_LW(con);
		y = CON_Y(con);
		MOVE_CON(con,x,y);
	}
}



/*
 *  UI_UtilString1        QUERY ONE STRING
 */
int UI_UtilString1( LWPanelFuncs *panfunc, char *title, char *caption, char *strin, char *strout )
{
	LWPanelID			panelID;
	LWControl           *control;
	int                 btn;

	panelID = (*panfunc->create)( title, NULL );
	control = STR_CTL( panfunc, panelID, caption, 40 );
	SET_STR( control, strin, 255 );
	btn     = (*panfunc->open)( panelID, PANF_BLOCKING|PANF_CANCEL );
	if (btn) {
		GET_STR( control, strout, 255 );
	}
	PAN_KILL( panfunc, panelID );

	return btn;
}
