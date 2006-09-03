/*
 * UI_UTIL.H -- DirectX Export
 *
 * Copyright (c) 2000-2001, D-STORM, Inc. 
 *
 * written by Yoshiaki Tazaki
 * revision  history  8/08/2001
 */

#ifndef _UI_UTIL_H
#define _UI_UTIL_H

#include <lwpanel.h>

///////////////////////////////////////////////  PANEL CONTROL MACROS

#ifndef CON_SETDRAW
#define CON_SETDRAW(ctl,f,d) ( \
	(ival.intv.value=(int)d, (*ctl->set)(ctl,CTL_USERDATA,&ival)), \
	(ival.intv.value=(int)f,(*ctl->set)(ctl,CTL_USERDRAW,&ival)) )
#endif

#ifndef WPOPDOWN_CTL
#define WPOPDOWN_CTL(pfunc,pan,tit,ls,w) ( \
	desc.type=LWT_CHOICE,desc.choice.vertical=w, desc.choice.items=ls,\
    (*pfunc->addControl) (pan, "PopDownControl", &desc, tit))
#endif

#define REDRAW_PANEL(pfunc,pan)	((*pfunc->draw)(pan,DR_REFRESH))
#define RENDER_PANEL(pfunc,pan)	((*pfunc->draw)(pan,DR_RENDER))
#define ERASE_PANEL(pfunc,pan)	((*pfunc->draw)(pan,DR_ERASE))
#define MOVE_TO_FRONT(pfunc,pan) \
	((*pfunc->set)(pan,PAN_TO_FRONT,(void *)&(ival.intv.value)),ival.intv.value)

static LWPanControlDesc   desc;
static LWValue 	ival={LWT_INTEGER}, ivecval={LWT_VINT},
				fval={LWT_FLOAT}, fvecval={LWT_VFLOAT}, sval={LWT_STRING};



extern void UI_UtilOneLine     ( LWControl *con1, LWControl *con2, LWControl *con3, LWControl *con4, 
                                 LWControl *con5, LWControl *con6 );
extern void UI_UtilOneLinePlus ( LWControl *con1, LWControl *con2, LWControl *con3, LWControl *con4,
                                 LWControl *con5, LWControl *con6, int spc );
extern void UI_UtilEndLine     ( LWControl *con1, LWControl *con2, LWControl *con3, LWControl *con4,
                                 LWControl *con5, LWControl *con6, int xw );
extern void UI_UtilVAlign      ( int lw, LWControl *con ) ;
extern int  UI_UtilString1     ( LWPanelFuncs *panfunc, char *title, char *caption, char *strin, char *strout );

#endif /* _UI_UTIL_H  */

