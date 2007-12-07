#ifndef _GCTP_MOVIE_DX_PLAYER_HPP_
#define _GCTP_MOVIE_DX_PLAYER_HPP_
/** @file
 * GameCatapult DirectShowクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/25 19:35:25
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/def.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/pointer.hpp>

namespace gctp { namespace graphic {
	class Texture;
}}

namespace gctp { namespace movie { namespace dx {

	TYPEDEF_DXCOMPTR(IGraphBuilder);
	TYPEDEF_DXCOMPTR(IMediaControl);
	TYPEDEF_DXCOMPTR(IMediaPosition);
	TYPEDEF_DXCOMPTR(IMediaEventEx);
	TYPEDEF_DXCOMPTR(IBasicAudio);
	TYPEDEF_DXCOMPTR(IBaseFilter);

	/** DirectShowラッパークラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/19 19:29:37
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Player : public Object {
	public:
		Player() : hwnd_(0), loop_(0), loop_count_(1) {}

		~Player();

		HRslt openForTexture(const _TCHAR *name, bool audio_on = true);
		HRslt open(HWND hwnd, int notify_msgid, const _TCHAR *name, bool audio_on = true);

		bool isPlaying();
		HRslt play(int loop);
		HRslt stop();
		void setVolume(float volume);
		float getVolume();
		void setPan(float pan);
		float getPan();

		HRslt checkStatus();

		Handle<graphic::Texture> getTexture();

		int width();
		int height();

	private:
		IGraphBuilderPtr graph_builder_;
		IMediaControlPtr media_control_;
		IMediaPositionPtr media_position_;
		IMediaEventExPtr media_event_;
		IBasicAudioPtr audio_;
		IBaseFilterPtr renderer_;
		HWND hwnd_;
		Ptr archive_source_;
		int loop_;
		int loop_count_;
		DWORD invalidate_timer_;
	};

}}} // namespace gctp

#endif //_GCTP_MOVIE_DX_PLAYER_HPP_