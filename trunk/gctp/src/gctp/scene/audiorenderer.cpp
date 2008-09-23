/** @file
 * GameCatapult 簡易オーディオレンダラークラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/audiorenderer.hpp>
#include <gctp/scene/camera.hpp>
#include <gctp/scene/speaker.hpp>
#include <gctp/scene/world.hpp>
#include <gctp/audio.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/app.hpp> // for gctp::Profiling

using namespace std;

namespace gctp { namespace scene {

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, AudioRenderer, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, AudioRenderer)
		TUKI_METHOD(AudioRenderer, attach)
	TUKI_IMPLEMENT_END(AudioRenderer)

	/** ワールドオーディオレンダリング
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool AudioRenderer::onReach(float delta) const
	{
		AudioRenderer *self = const_cast<AudioRenderer *>(this);
		audio::device().initListener();
		const Matrix &wtm = Camera::current().node()->val.wtm();
		audio::device().setListenerPosition(wtm.position());
		audio::device().setListenerPosture(wtm.at(), wtm.up());
		if(delta != 0) audio::device().setListenerVelocity((wtm.position()-Camera::current().node()->val.prevWTM().position())/delta);
		if(target_) {
			for(HandleList<Speaker>::iterator i = self->target_->speaker_list.begin(); i != self->target_->speaker_list.end();) {
				if(*i) {
					(*i)->apply(delta);
					++i;
				}
				else i = self->target_->speaker_list.erase(i);
			}
		}
		return false;
	}

	bool AudioRenderer::onLeave(float delta) const
	{
		audio::device().updateListener();
		return true;
	}

	bool AudioRenderer::setUp(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void AudioRenderer::attach(luapp::Stack &L)
	{
		if(L.top() >=1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			attach(world);
		}
	}

}} // namespace gctp::scene
