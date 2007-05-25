#ifndef _GCTP_AUDIO_HPP_
#define _GCTP_AUDIO_HPP_
/** @file
 * GameCatapult DirectX Audioクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: audio.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/def.hpp>
#include <gctp/db.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/audio/player.hpp>
#include <gctp/audio/clip.hpp>
#include <gctp/audio/track.hpp>
#include <boost/scoped_ptr.hpp>

namespace gctp { namespace audio {

	namespace dx {
		class Device;
	}

	/** オーディオデバイスクラス
	 *
	 * ミキサー/マスター出力オブジェクトとして抽象化\n
	 * DirectSoundをあらわすクラス。\n
	 * デバイスに依存するリソースのデータベースも兼ねる。\n
	 * ※デバイス消失を考慮しなくていいDirectSoundでDXRsrcDBを確保する意味は、Audioリソースを
	 * すべて開放してから出ないとMusicの開放が出来ないから。\n
	 */
	class Device : public DB {
	public:
		Device();
		~Device();
		HRslt open(HWND hwnd, bool play_focus_lost = true, bool priority = true);
		void close();

		/// オープン済みか？
		bool isOpen() const;

		/** カレントオーディオデバイスの取得
		 *
		 * カレントデバイスを返す。
		 */
		inline static Device *getCurrent() { return current_; }
		/** カレントオーディオデバイスを設定
		 *
		 * カレントデバイスを設定する。
		 */
		void setCurrent() { current_ = this; }

		/** @defgroup AudioPlayback 再生関連
		 */
		/* @{ */
		/** これらはトラックをその場で作る
		 * @{ */
		Player ready(const _TCHAR *fname);
		Player ready(const Clip &clip, uint repeat = 0);
		Player ready(int stance, const Clip &clip, uint repeat = 0);
		/* @} */
		/* @} */

		/** @defgroup AudioMasterOperation マスター出力関連 */
		/* @{ */
		/// マスターボリューム設定
		void setVolume(float volume);
		/// マスターボリューム
		float volume();
		/// マスター出力形式設定
		HRslt setFormat(int channel_num, int freq, int bitrate);
		/// マスター出力形式取得
		HRslt getFormat(int &channel_num, int &freq, int &bitrate);
		/* @} */

		dx::Device &impl() const { return *impl_.get(); }

	private:
		boost::scoped_ptr<dx::Device> impl_;
		GCTP_TLS static Device* current_;	///< カレントデバイスインスタンス
	};

	void setStreamingThreshold(ulong threshold);
	ulong streamingThreshold();

	/** Audioの取得
	 *
	 * カレントデバイスのインスタンスを返す。
	 */
	inline Device &device() { return *Device::getCurrent(); }


	/** データベースに登録しつつ製作
	 *
	 * すでにデータベースにあるならそれを返し、そうで無かったら新規製作しＤＢに登録する、という
	 * 操作のテンプレート\n
	 * DB登録名とsetUpに渡す文字列を別々に指定できる
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 14:03:54
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T>
	Pointer<_T> createOnDB(const char *name)
	{
		Handle<_T> h = device()[name];
		if(h) return h.lock();
		else {
			Pointer<_T> ret = new _T;
			if(ret) {
				if(ret->setUp(name)) {
					PRNN(_T("リソース'")<<name<<_T("'を製作"));
					h = ret;
					device().insert(h, name);
					return ret;
				}
			}
			PRNN(_T("リソース'")<<name<<_T("'の製作に失敗"));
		}
		return Pointer<_T>();
	}

	/** データベースに登録しつつ製作
	 *
	 * すでにデータベースにあるならそれを返し、そうで無かったら新規製作しＤＢに登録する、という
	 * 操作のテンプレート\n
	 * DB登録名とsetUpに渡す文字列を別々に指定できる\n
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 14:03:54
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T>
	Pointer<_T> createOnDB(const char *name, const char *setup)
	{
		Handle<_T> h = device()[name];
		if(h) return h.lock();
		else {
			Pointer<_T> ret = new _T;
			if(ret) {
				if(ret->setUp(setup)) {
					PRNN(_T("リソース'")<<name_T(<<"'を製作"));
					h = ret;
					device().insert(h, name);
					return ret;
				}
			}
			PRNN(_T("リソース'")<<name<<_T("'の製作に失敗"));
		}
		return Pointer<_T>();
	}

	/** @defgroup CurrentAudioDeviceOperation カレントに対する操作 */
	/* @{*/
	/// 指定ファイルから音声データを用意
	inline Player ready(const _TCHAR *fname) { return device().ready(fname); }
	/* @}*/

}} // namespace gctp

#endif //_GCTP_AUDIO_HPP_