#ifndef _GCTP_AUDIO_CLIP_HPP_
#define _GCTP_AUDIO_CLIP_HPP_
/** @file
 * GameCatapult オーディオクリップクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/25 19:11:31
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/def.hpp>
#include <gctp/pointer.hpp>
#include <gctp/buffer.hpp>

namespace gctp { namespace audio {

	/** オーディオクリップクラス
	 *
	 * 抽象化されたオーディオストリーム
	 *
	 * 今のところwavファイルとoggvorbisファイルを扱える
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/19 19:29:37
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Clip : public Object {
	public:
		bool open(const _TCHAR *path);
		bool isOpen();
		HRslt read(void *dst, std::size_t size, std::size_t * const readsize = NULL);
		HRslt rewind();

		std::size_t size() const;
		const WAVEFORMATEX *format() const;
	private:
		Ptr stream_;
		BufferPtr on_memory_;
	};

}} // namespace gctp

#endif //_GCTP_AUDIO_CLIP_HPP_