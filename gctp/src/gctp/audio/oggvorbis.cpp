/**@file
 * wavファイルクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/21 1:43:30
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/audio/oggvorbis.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace audio {

	namespace {
		struct OggFileCB {
			typedef OggFileCB Self;
			AbstractFilePtr file;
			long position;

			static size_t read(void *ptr, size_t size, size_t nmemb, void *datasource)
			{
				Self *self = (Self *)datasource;
				int ret = self->file->read(ptr, size);
				if(ret > 0) self->position += ret;
				return ret;
			}
			static int seek(void *datasource, ogg_int64_t offset, int whence)
			{
				Self *self = (Self *)datasource;
				switch (whence) {
				case SEEK_SET:
					self->position = (long)offset;
					break;

				case SEEK_CUR:
					self->position += (long)offset;
					break;

				case SEEK_END:
					self->position = (long)(self->file->size() - offset);
					break;
				}
				return self->file->seek(self->position);
			}
			static int close(void *datasource)
			{
				Self *self = (Self *)datasource;
				delete self;
				return 0;
			}
			static long tell(void *datasource)
			{
				Self *self = (Self *)datasource;
				return self->position;
			}

			static ov_callbacks cbs;
		};
		
		ov_callbacks OggFileCB::cbs = { read, seek, close, tell };

		struct OggMemFileCB {
			typedef OggMemFileCB Self;
			const void *buffer;
			std::size_t bufsize;
			long position;

			static size_t read(void *ptr, size_t size, size_t nmemb, void *datasource)
			{
				Self *self = (Self *)datasource;
				int ret = std::min<int>((int)size, (int)self->bufsize-self->position);
				::memcpy(ptr, (char *)self->buffer+self->position, ret);
				if(ret > 0) self->position += ret;
				return ret;
			}
			static int seek(void *datasource, ogg_int64_t offset, int whence)
			{
				Self *self = (Self *)datasource;
				switch (whence) {
				case SEEK_SET:
					self->position = (long)offset;
					break;

				case SEEK_CUR:
					self->position += (long)offset;
					break;

				case SEEK_END:
					self->position = (long)(self->bufsize - offset);
					break;
				}
				if(self->position < 0) self->position = 0;
				if(self->position > (long)self->bufsize) self->position = (long)self->bufsize;
				return self->position;
			}
			static int close(void *datasource)
			{
				Self *self = (Self *)datasource;
				delete self;
				return 0;
			}
			static long tell(void *datasource)
			{
				Self *self = (Self *)datasource;
				return self->position;
			}

			static ov_callbacks cbs;
		};
		
		ov_callbacks OggMemFileCB::cbs = { read, seek, close, tell };
	}

	/** ファイルからの読み込みモードでオープン
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 1:51:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt OggVorbis::open(const _TCHAR *fname)
	{
		if(!fname) return E_INVALIDARG;
		if(isOpen()) close();
		
		HRslt hr;
		// open File
		fp_ = _tfopen( fname, _T("rb") );
		if( fp_ == NULL ) return E_FAIL;

		// open vorbis file
		if( ov_open( fp_, &ogg_vorbis_, NULL, 0 ) < 0 ) {
			fclose( fp_ );
			fp_ = 0;
			return E_FAIL;
		}

		// get vorbis info
		vorbis_info_ = ov_info( &ogg_vorbis_, -1 );
		if( vorbis_info_ == NULL )
		{
			ov_clear( &ogg_vorbis_ );
			fclose( fp_ );
			fp_ = 0;
			return E_FAIL;
		}
		
		size_ = (size_t)ov_pcm_total(&ogg_vorbis_, -1) * vorbis_info_->channels * 2;

		ZeroMemory( &wfx_, sizeof(wfx_) );
		wfx_.wFormatTag = WAVE_FORMAT_PCM;
		wfx_.nChannels = vorbis_info_->channels;
		wfx_.wBitsPerSample = 16;
		wfx_.nBlockAlign = wfx_.nChannels * wfx_.wBitsPerSample / 8;
		wfx_.nSamplesPerSec = vorbis_info_->rate;
		wfx_.nAvgBytesPerSec = wfx_.nSamplesPerSec * wfx_.nBlockAlign;
		//m_ulPlayPosition = 0;
		return S_OK;
	}

	/** メモリからの読み込み用にオープン
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 2:32:45
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt OggVorbis::open(const void * const src, std::size_t size) {
		if(!src || size == 0) return E_INVALIDARG;
		if(isOpen()) close();

		// open vorbis file
		OggMemFileCB *self = new OggMemFileCB;
		self->buffer = src;
		self->bufsize = size;
		self->position = 0;
		if( ov_open_callbacks( self, &ogg_vorbis_, NULL, 0, OggMemFileCB::cbs ) < 0 ) {
			delete self;
			return E_FAIL;
		}

		// get vorbis info
		vorbis_info_ = ov_info( &ogg_vorbis_, -1 );
		if( vorbis_info_ == NULL ) {
			ov_clear( &ogg_vorbis_ );
			return E_FAIL;
		}
		size_ = (size_t)ov_pcm_total(&ogg_vorbis_, -1) * vorbis_info_->channels * 2;

		wfx_.wFormatTag=WAVE_FORMAT_PCM;
		wfx_.nChannels=vorbis_info_->channels;
		wfx_.wBitsPerSample=16;
		wfx_.nBlockAlign=wfx_.nChannels * wfx_.wBitsPerSample/8;
		wfx_.nSamplesPerSec=vorbis_info_->rate;
		wfx_.nAvgBytesPerSec=wfx_.nSamplesPerSec * wfx_.nBlockAlign;
		return S_OK;
	}

	/** 抽象ファイルからの読み込み用にオープン
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 2:32:45
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt OggVorbis::open(AbstractFilePtr fileptr)
	{
		if(!fileptr) return E_INVALIDARG;
		if(isOpen()) close();

		// open vorbis file
		OggFileCB *self = new OggFileCB;
		self->file = fileptr;
		self->position = 0;
		if( ov_open_callbacks( self, &ogg_vorbis_, NULL, 0, OggFileCB::cbs ) < 0 ) {
			delete self;
			return E_FAIL;
		}

		// get vorbis info
		vorbis_info_ = ov_info( &ogg_vorbis_, -1 );
		if( vorbis_info_ == NULL ) {
			ov_clear( &ogg_vorbis_ );
			return E_FAIL;
		}
		size_ = (size_t)ov_pcm_total(&ogg_vorbis_, -1) * vorbis_info_->channels * 2;

		wfx_.wFormatTag=WAVE_FORMAT_PCM;
		wfx_.nChannels=vorbis_info_->channels;
		wfx_.wBitsPerSample=16;
		wfx_.nBlockAlign=wfx_.nChannels * wfx_.wBitsPerSample/8;
		wfx_.nSamplesPerSec=vorbis_info_->rate;
		wfx_.nAvgBytesPerSec=wfx_.nSamplesPerSec * wfx_.nBlockAlign;
		return S_OK;
	}

	/** ストリームを閉じる
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 3:32:04
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt OggVorbis::close()
	{
		if(isOpen()) {
			ov_clear(&ogg_vorbis_);
			vorbis_info_ = 0;
			if(fp_) {
				fclose( fp_ );
				fp_ = 0;
			}
			return S_OK;
		}
		return S_FALSE;
	}

	/** カーソルを巻き戻す
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 3:08:18
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt OggVorbis::rewind()
	{
		if(!isOpen()) return CO_E_NOTINITIALIZED;

		ov_pcm_seek( &ogg_vorbis_, 0 );

		return S_OK;
	}

	/** 読み込み
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 3:20:24
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt OggVorbis::read(void *dst, size_t size, size_t * const readsize)
	{
		if( !isOpen() ) return CO_E_NOTINITIALIZED;
		if( !dst ) return E_INVALIDARG;
		int current_section;

		if( readsize ) *readsize = 0;
		int ret;
		char *_dst = (char *)dst;
		while(size > 0 && (ret = ov_read(&ogg_vorbis_, _dst, (int)size, 0, 2, 1, &current_section))>0) {
			_dst += ret;
			size -= ret;
			if(readsize) *readsize += ret;
		}
		//PRNN("Ogg "<<(readsize ? *readsize : 0)<<","<<ret<<","<<current_section);
		//if( readsize ) *readsize = ret;
		if(ret < 0) return E_FAIL;
		if(ret == 0) return S_FALSE;
		return S_OK;
	}

}} // namespace gctp
