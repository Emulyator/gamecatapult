#ifndef _GCTP_AUDIO_DX_WAVFILE_HPP_
#define _GCTP_AUDIO_DX_WAVFILE_HPP_
/**@file
 * wavファイルクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/20 19:17:07
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/object.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/fileserver.hpp>
#include <mmsystem.h>

namespace gctp { namespace audio { namespace dx {

	/** wavファイルクラス
	 *
	 * これ、WavFile a, b;とあってa = b とかしても大ジョブなのかな？
	 * HMMIOの実装次第だけど。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/20 19:18:45
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class WavFile : public Object {
	public:
		enum {
			READ = 1,
			WRITE = 1<<1
		};

		WavFile();
		explicit WavFile(const _TCHAR *fname);
		WavFile(const _TCHAR *fname, const WAVEFORMATEX * const wfx);
		WavFile(const void * const data, std::size_t size);
		explicit WavFile(AbstractFilePtr fileptr);
		~WavFile();

		HRslt open(const _TCHAR *fname);
		HRslt open(const _TCHAR *fname, const WAVEFORMATEX * const wfx);
		HRslt open(const void * const src, std::size_t size);
		HRslt open(AbstractFilePtr fileptr);
		HRslt close();

		bool isOpen() { return hmmio_ != NULL; }
		HRslt read(void *dst, std::size_t size, std::size_t * const readsize = NULL);
		HRslt write(const void *src, std::size_t size, std::size_t * const wrotesize = NULL);
		HRslt rewind();

		std::size_t size() const { return size_; }
		const WAVEFORMATEX *format() const { return wfx_; }
		
	protected:
		HRslt readMMIO();
		HRslt writeMMIO(const WAVEFORMATEX *src);

	private:
		WAVEFORMATEX              *wfx_;        // Pointer to WAVEFORMATEX structure
		HMMIO                     hmmio_;       // MM I/O handle for the WAVE
		MMCKINFO                  ck_;          // Multimedia RIFF chunk
		MMCKINFO                  ckriff_;      // Use in opening a WAVE file
		std::size_t               size_;        // The size of the wave file
		MMIOINFO                  write_mmioinfo_;
		uint32_t                  flags_;
		void                      *buffer_;
	};

}}} // namespace gctp

#endif //_GCTP_AUDIO_DX_WAVFILE_HPP_