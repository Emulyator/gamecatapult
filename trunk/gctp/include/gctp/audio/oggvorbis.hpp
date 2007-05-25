#ifndef _GCTP_AUDIO_OGGVORBIS_HPP_
#define _GCTP_AUDIO_OGGVORBIS_HPP_
/**@file
 * OggVorbisファイルクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/20 19:17:07
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/object.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/fileserver.hpp>
#include <vorbis/vorbisfile.h>

namespace gctp { namespace audio {

	/** OggVorbisファイルクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/20 19:18:45
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class OggVorbis : public Object {
	public:
		OggVorbis() : vorbis_info_(0), fp_(0), size_(0) {}
		~OggVorbis() { close(); }
		
		HRslt open(const _TCHAR *fname);
		HRslt open(const void * const src, std::size_t size);
		HRslt open(AbstractFilePtr fileptr);
		HRslt close();

		bool isOpen() { return vorbis_info_ ? true : false; }
		HRslt read(void *dst, std::size_t size, std::size_t * const readsize = NULL);
		HRslt write(const void *src, std::size_t size, std::size_t * const wrotesize = NULL);
		HRslt rewind();

		std::size_t size() const { return size_; }
		const WAVEFORMATEX *format() const { return &wfx_; }

	private:
		FILE			*fp_;
		OggVorbis_File	ogg_vorbis_;		///< oggVorbisファイル
		vorbis_info		*vorbis_info_;		///< vorbis情報
		WAVEFORMATEX	wfx_;				///< Pointer to WAVEFORMATEX structure
		size_t			size_;
	};

}} // namespace gctp

#endif //_GCTP_AUDIO_OGGVORBIS_HPP_