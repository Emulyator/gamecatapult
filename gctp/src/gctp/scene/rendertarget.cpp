/** @file
 * GameCatapult レンダーターゲットレンダラークラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/rendertarget.hpp>
#include <gctp/graphic.hpp>
#include <gctp/graphic/rendertarget.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/aabox.hpp>
#include <gctp/app.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/context.hpp>

using namespace std;

namespace gctp { namespace scene {

	RenderTarget::RenderTarget()
	{
	}

	void RenderTarget::begin() const
	{
		gctp::HRslt hr = rt_->begin();
		if(!hr) GCTP_TRACE(hr);
		gctp::graphic::device().clear();
	}

	void RenderTarget::end() const
	{
		gctp::HRslt hr = rt_->resolve();
		if(!hr) GCTP_TRACE(hr);
		hr = rt_->end();
		if(!hr) GCTP_TRACE(hr);
	}

	bool RenderTarget::onReach(float delta) const
	{
		begin();
		return true;
	}

	bool RenderTarget::onLeave(float delta) const
	{
		end();
		return true;
	}
	
	bool RenderTarget::LuaCtor(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void RenderTarget::setUp(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<graphic::RenderTarget> rt = tuki_cast<graphic::RenderTarget>(L[1]);
			if(rt) {
				rt_ = rt;
			}
		}
	}

	int RenderTarget::texture(luapp::Stack &L)
	{
		TukiRegister::push(L, Hndl(&(rt_->texture())));
		return 1;
	}

	int RenderTarget::setUpRT(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			Pointer<graphic::RenderTarget> rt = tuki_cast<graphic::RenderTarget>(L[1]);
			if(rt) {
				HRslt hr = rt->setUp(graphic::RenderTarget::NORMAL, L[2].toInteger(), L[3].toInteger(), D3DFMT_A8R8G8B8);
				if(hr) {
					TukiRegister::push(L, Hndl(&(rt->texture())));
					return 1;
				}
			}
		}
		return 0;
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, RenderTarget, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, RenderTarget)
		TUKI_METHOD(RenderTarget, setUp)
		TUKI_METHOD(RenderTarget, texture)
		TUKI_METHOD(RenderTarget, setUpRT)
	TUKI_IMPLEMENT_END(RenderTarget)

}} // namespace gctp::scene
