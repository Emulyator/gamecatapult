#ifndef _LWPP_FORM_HPP_
#define _LWPP_FORM_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file LightWave API XPanel ViewƒNƒ‰ƒX
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 * @date 2006/04/10 4:54:36
 */
#include <lwxpanel.h>

namespace lwpp {

	namespace XPanel {
		
		class View {
			View(GlobalFunc *global) : interface_(global(LWXPANELFUNCS_GLOBAL, GFUSE_TRANSIENT)), id_(0)
			{
			}

			~View()
			{
				if(id_ > 0) interface_->destroy(id_);
			}

			void create()
			{
				if(controls_.size() > 0) id_ = interface_->create(LWXP_VIEW, &(*controls_.begin()));
				else id_ = interface_->create(LWXP_VIEW, NULL);
			}

		private:
			LWXPanelFuncs *interface_;
			LWXPanelID id_;
			std::vector<LWXPanelControl> controls_;
		};

	}

}

#endif //_LWPP_FORM_HPP_
