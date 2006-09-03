/**@file
 * wavファイルクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/21 1:43:30
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/audio/dx/wavfile.hpp>
#include <gctp/dbgout.hpp>

#define SAFE_FREE(_p) { if(_p) free(_p); (_p) = 0; }

using namespace std;

namespace gctp { namespace audio { namespace dx {

	WavFile::WavFile() : wfx_(0), hmmio_(0), size_(0), flags_(0), buffer_(0) {}

	WavFile::WavFile(const char *fname)
		: wfx_(0), hmmio_(0), size_(0), flags_(0), buffer_(0) {
		open(fname);
	}

	WavFile::WavFile(const char *fname, const WAVEFORMATEX * const wfx)
		: wfx_(0), hmmio_(0), size_(0), flags_(0), buffer_(0) {
		open(fname, wfx);
	}

	WavFile::WavFile(const void * const src, std::size_t size, const WAVEFORMATEX *wfx)
		: wfx_(0), hmmio_(0), size_(0), flags_(0), buffer_(0) {
		open(src, size, wfx);
	}

	WavFile::~WavFile() {
		close();
		if(!(flags_&MEMORY) && wfx_) free(wfx_);
	}

	/** ファイルからの読み込みモードでオープン
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 1:51:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WavFile::open(const char *fname) {
		if(!fname) return E_INVALIDARG;
		if( (flags_ & MEMORY) && isOpen() ) {
			if(fname_ == fname) return S_FALSE;
			close();
		}
		fname_ = fname;

		SAFE_FREE(wfx_);
		flags_ = READ;

		HRslt hr;
		hmmio_ = mmioOpen(const_cast<char *>(fname), NULL, MMIO_ALLOCBUF | MMIO_READ);
		if(!hmmio_) {
			// Loading it as a file failed, so try it as a resource
			HRSRC hResInfo = FindResource(NULL, fname, TEXT("WAVE"));
			if(!hResInfo) {
				hResInfo = FindResource(NULL, fname, TEXT("WAV"));
				if(!hResInfo) return DXTRACE_ERR(TEXT("FindResource"), ERROR_FILE_NOT_FOUND);
			}

			HGLOBAL hResData = LoadResource(NULL, hResInfo);
			if(!hResData) return DXTRACE_ERR(TEXT("LoadResource"), E_FAIL);

			DWORD dwSize = SizeofResource(NULL, hResInfo);
			if(dwSize == 0) return DXTRACE_ERR(TEXT("SizeofResource"), E_FAIL);

			void *pvRes = LockResource(hResData);
			if(!pvRes) return DXTRACE_ERR(TEXT("LockResource"), E_FAIL);

			buffer_ = malloc(dwSize);
			memcpy(buffer_, pvRes, dwSize);

			MMIOINFO mmioinfo;
			memset(&mmioinfo, 0, sizeof(mmioinfo));
			mmioinfo.fccIOProc = FOURCC_MEM;
			mmioinfo.cchBuffer = dwSize;
			mmioinfo.pchBuffer = reinterpret_cast<char *>(buffer_);

			hmmio_ = mmioOpen(NULL, &mmioinfo, MMIO_ALLOCBUF | MMIO_READ);
		}

		if(!( hr = readMMIO() )) {
			// ReadMMIO will fail if its an not a wave file
			mmioClose(hmmio_, 0);
			return DXTRACE_ERR(TEXT("readMMIO"), hr.i);
		}

		if(!( hr = rewind() )) return DXTRACE_ERR( TEXT("ResetFile"), hr.i );

		// After the rewind, the size of the wav file is m_ck.cksize so store it now
		size_ = ck_.cksize;

		return S_OK;
	}

	/** 書き込み用にオープン
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 2:05:58
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WavFile::open(const char *fname, const WAVEFORMATEX * const wfx) {
		if( (flags_ & MEMORY) && isOpen() ) {
			if(fname_ == fname) return S_FALSE;
			close();
		}
		fname_ = fname;

		hmmio_ = mmioOpen(const_cast<char *>(fname), NULL, MMIO_ALLOCBUF|MMIO_READWRITE|MMIO_CREATE);
		if(!hmmio_) return DXTRACE_ERR(TEXT("mmioOpen"), E_FAIL);
		flags_ = WRITE;

		HRslt hr;
		if(!( hr = writeMMIO(wfx_) )) {
			mmioClose(hmmio_, 0 );
			return DXTRACE_ERR(TEXT("writeMMIO"), hr.i);
		}
		if(!( hr = rewind() )) return DXTRACE_ERR(TEXT("rewind"), hr.i);
		return hr;
	}

	/** メモリからの読み込み用にオープン
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 2:32:45
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WavFile::open(const void * const src, std::size_t size, const WAVEFORMATEX * const wfx) {
		if(flags_ & MEMORY) {
			if(isOpen()) {
				close();
				fname_.resize(0);
			}
			SAFE_FREE(wfx_);
		}
		wfx_ = reinterpret_cast<WAVEFORMATEX *>(const_cast<void *>(src));
		size_ = size;
		datacur_ = data_ = const_cast<uint8_t * const>(reinterpret_cast<const uint8_t * const>(src));
		flags_ = READ|MEMORY;
		return S_OK;
	}

	/** ストリームを閉じる
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 3:32:04
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WavFile::close() {
		if( flags_ & READ ) {
			mmioClose( hmmio_, 0 );
			hmmio_ = NULL;
			SAFE_FREE( buffer_ );
		}
		else {
			if( !hmmio_ ) return CO_E_NOTINITIALIZED;

			mmioinfo_.dwFlags |= MMIO_DIRTY;
			if( 0 != mmioSetInfo( hmmio_, &mmioinfo_, 0 ) )
				return DXTRACE_ERR( TEXT("mmioSetInfo"), E_FAIL );
    
			// Ascend the output file out of the 'data' chunk -- this will cause
			// the chunk size of the 'data' chunk to be written.
			if( 0 != mmioAscend( hmmio_, &ck_, 0 ) )
				return DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
    
			// Do this here instead...
			if( 0 != mmioAscend( hmmio_, &ckriff_, 0 ) )
				return DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
        
			mmioSeek( hmmio_, 0, SEEK_SET );

			if( 0 != (int)mmioDescend( hmmio_, &ckriff_, NULL, 0 ) )
				return DXTRACE_ERR( TEXT("mmioDescend"), E_FAIL );
    
			ck_.ckid = mmioFOURCC('f','a','c','t');

			if( 0 == mmioDescend( hmmio_, &ck_, &ckriff_, MMIO_FINDCHUNK ) ) {
				DWORD dwSamples = 0;
				mmioWrite( hmmio_, (HPSTR)&dwSamples, sizeof(dwSamples) );
				mmioAscend( hmmio_, &ck_, 0 ); 
			}
    
			// Ascend the output file out of the 'RIFF' chunk -- this will cause
			// the chunk size of the 'RIFF' chunk to be written.
			if( 0 != mmioAscend( hmmio_, &ckriff_, 0 ) )
				return DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
    
			mmioClose( hmmio_, 0 );
			hmmio_ = NULL;
		}

		return S_OK;
	}

	/** カーソルを巻き戻す
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 3:08:18
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WavFile::rewind()
	{
		if( flags_&MEMORY ) datacur_ = data_;
		else {
			if(!hmmio_) return CO_E_NOTINITIALIZED;

			if( flags_&READ ) {
				// Seek to the data
				if( -1 == mmioSeek( hmmio_, ckriff_.dwDataOffset + sizeof(FOURCC), SEEK_SET ) )
					return DXTRACE_ERR( TEXT("mmioSeek"), E_FAIL );

				// Search the input file for the 'data' chunk.
				ck_.ckid = mmioFOURCC('d','a','t','a');
				if( 0 != mmioDescend( hmmio_, &ck_, &ckriff_, MMIO_FINDCHUNK ) )
				  return DXTRACE_ERR( TEXT("mmioDescend"), E_FAIL );
			}
			else {
				// Create the 'data' chunk that holds the waveform samples.  
				ck_.ckid = mmioFOURCC('d','a','t','a');
				ck_.cksize = 0;

				if( 0 != mmioCreateChunk( hmmio_, &ck_, 0 ) ) 
					return DXTRACE_ERR( TEXT("mmioCreateChunk"), E_FAIL );

				if( 0 != mmioGetInfo( hmmio_, &mmioinfo_, 0 ) )
					return DXTRACE_ERR( TEXT("mmioGetInfo"), E_FAIL );
			}
		}
    
		return S_OK;
	}

	/** MMIOストリームから読み込み
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 2:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WavFile::readMMIO() {
		MMCKINFO        ckin;           // chunk info. for general use.
		PCMWAVEFORMAT   pcmWaveFormat;  // Temp PCM structure to load in.

		wfx_ = NULL;

		if(mmioDescend(hmmio_, &ckriff_, NULL, 0)) return DXTRACE_ERR(TEXT("mmioDescend"), E_FAIL);

		// Check to make sure this is a valid wave file
		if( (ckriff_.ckid != FOURCC_RIFF) || (ckriff_.fccType != mmioFOURCC('W','A','V','E') ) )
			return DXTRACE_ERR( TEXT("mmioFOURCC"), E_FAIL );

		// Search the input file for for the 'fmt ' chunk.
		ckin.ckid = mmioFOURCC('f','m','t',' ');
		if(mmioDescend(hmmio_, &ckin, &ckriff_, MMIO_FINDCHUNK) )
			return DXTRACE_ERR( TEXT("mmioDescend"), E_FAIL );

		// Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
		// if there are extra parameters at the end, we'll ignore them
		if(ckin.cksize < sizeof(PCMWAVEFORMAT))
			return DXTRACE_ERR( TEXT("sizeof(PCMWAVEFORMAT)"), E_FAIL );

		// Read the 'fmt ' chunk into <pcmWaveFormat>.
		if( mmioRead( hmmio_, (HPSTR) &pcmWaveFormat, sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat) )
			return DXTRACE_ERR( TEXT("mmioRead"), E_FAIL );

		// Allocate the waveformatex, but if its not pcm format, read the next
		// word, and thats how many extra bytes to allocate.
		if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM ) {
			wfx_ = reinterpret_cast<WAVEFORMATEX *>(malloc( sizeof(WAVEFORMATEX) ));
			if(!wfx_) return DXTRACE_ERR( TEXT("wfx_"), E_OUTOFMEMORY );

			// Copy the bytes from the pcm structure to the waveformatex structure
			memcpy( wfx_, &pcmWaveFormat, sizeof(pcmWaveFormat) );
			wfx_->cbSize = 0;
		}
		else {
			// Read in length of extra bytes.
			WORD cbExtraBytes = 0L;
			if( mmioRead( hmmio_, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD) )
				return DXTRACE_ERR( TEXT("mmioRead"), E_FAIL );

			wfx_ = reinterpret_cast<WAVEFORMATEX *>(malloc( sizeof(WAVEFORMATEX) + cbExtraBytes ));
			if(!wfx_) return DXTRACE_ERR( TEXT("new"), E_OUTOFMEMORY );

			// Copy the bytes from the pcm structure to the waveformatex structure
			memcpy( wfx_, &pcmWaveFormat, sizeof(pcmWaveFormat) );
			wfx_->cbSize = cbExtraBytes;

			// Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
			if( mmioRead( hmmio_, (CHAR*)(((BYTE*)&(wfx_->cbSize))+sizeof(WORD)), cbExtraBytes ) != cbExtraBytes ) {
				SAFE_FREE( wfx_ );
				return DXTRACE_ERR( TEXT("mmioRead"), E_FAIL );
			}
		}

		// Ascend the input file out of the 'fmt ' chunk.
		if( 0 != mmioAscend( hmmio_, &ckin, 0 ) ) {
			SAFE_FREE( wfx_ );
			return DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
		}

		return S_OK;
	}

	/** 読み込み
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 3:20:24
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WavFile::read( void *dst, size_t size, size_t * const readsize ) {
		if( flags_ & MEMORY ) {
			if( datacur_ == NULL ) return CO_E_NOTINITIALIZED;
			if( readsize ) *readsize = 0;

			if( datacur_ + size > data_ + size_ ) size = size_ - static_cast<size_t>(datacur_ - data_);
        
			memcpy( dst, datacur_, size );
        
			if( readsize ) *readsize = size;

			return S_OK;
		}
		else {
			MMIOINFO mmioinfo; // current status of m_hmmio

			if( !hmmio_ ) return CO_E_NOTINITIALIZED;
			if( !dst ) return E_INVALIDARG;

			if( readsize ) *readsize = 0;

			if( 0 != mmioGetInfo( hmmio_, &mmioinfo, 0 ) )
				return DXTRACE_ERR( TEXT("mmioGetInfo"), E_FAIL );
                
			size_t cbDataIn = size;
			if( cbDataIn > ck_.cksize ) cbDataIn = ck_.cksize;

			ck_.cksize -= cbDataIn;
    
			for( size_t cT = 0; cT < cbDataIn; cT++ ) {
				// Copy the bytes from the io to the buffer.
				if( mmioinfo.pchNext == mmioinfo.pchEndRead ) {
					if( 0 != mmioAdvance( hmmio_, &mmioinfo, MMIO_READ ) )
						return DXTRACE_ERR( TEXT("mmioAdvance"), E_FAIL );

					if( mmioinfo.pchNext == mmioinfo.pchEndRead )
						return DXTRACE_ERR( TEXT("mmioinfo.pchNext"), E_FAIL );
				}

				// Actual copy.
				*((uint8_t *)dst+cT) = *mmioinfo.pchNext;
				mmioinfo.pchNext++;
			}

			if( 0 != mmioSetInfo( hmmio_, &mmioinfo, 0 ) )
				return DXTRACE_ERR( TEXT("mmioSetInfo"), E_FAIL );

			if( readsize ) *readsize = cbDataIn;

			return S_OK;
		}
	}

	/** MMIOストリームへ書き込み
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 3:37:12
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WavFile::writeMMIO( const WAVEFORMATEX * src ) {
		DWORD    dwFactChunk; // Contains the actual fact chunk. Garbage until WaveCloseWriteFile.
		MMCKINFO ckOut1;
    
		dwFactChunk = (DWORD)-1;

		// Create the output file RIFF chunk of form type 'WAVE'.
		ckriff_.fccType = mmioFOURCC('W','A','V','E');
		ckriff_.cksize = 0;

		if( 0 != mmioCreateChunk( hmmio_, &ckriff_, MMIO_CREATERIFF ) )
			return DXTRACE_ERR( TEXT("mmioCreateChunk"), E_FAIL );
    
		// We are now descended into the 'RIFF' chunk we just created.
		// Now create the 'fmt ' chunk. Since we know the size of this chunk,
		// specify it in the MMCKINFO structure so MMIO doesn't have to seek
		// back and set the chunk size after ascending from the chunk.
		ck_.ckid = mmioFOURCC('f','m','t',' ');
		ck_.cksize = sizeof(PCMWAVEFORMAT);

		if( 0 != mmioCreateChunk( hmmio_, &ck_, 0 ) )
			return DXTRACE_ERR( TEXT("mmioCreateChunk"), E_FAIL );

		// Write the PCMWAVEFORMAT structure to the 'fmt ' chunk if its that type. 
		if( src->wFormatTag == WAVE_FORMAT_PCM ) {
			if( mmioWrite( hmmio_, (HPSTR)src, sizeof(PCMWAVEFORMAT)) != sizeof(PCMWAVEFORMAT))
				return DXTRACE_ERR( TEXT("mmioWrite"), E_FAIL );
		}   
		else {
			// Write the variable length size.
			if( (UINT)mmioWrite( hmmio_, (HPSTR)src, sizeof(*src) + src->cbSize ) != ( sizeof(*src) + src->cbSize ) )
				return DXTRACE_ERR( TEXT("mmioWrite"), E_FAIL );
		}

		// Ascend out of the 'fmt ' chunk, back into the 'RIFF' chunk.
		if( 0 != mmioAscend( hmmio_, &ck_, 0 ) )
			return DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
    
		// Now create the fact chunk, not required for PCM but nice to have.  This is filled
		// in when the close routine is called.
		ckOut1.ckid = mmioFOURCC('f','a','c','t');
		ckOut1.cksize = 0;

		if( 0 != mmioCreateChunk( hmmio_, &ckOut1, 0 ) )
			return DXTRACE_ERR( TEXT("mmioCreateChunk"), E_FAIL );
		
		if( mmioWrite( hmmio_, (HPSTR)&dwFactChunk, sizeof(dwFactChunk)) != sizeof(dwFactChunk) )
			 return DXTRACE_ERR( TEXT("mmioWrite"), E_FAIL );
    
		// Now ascend out of the fact chunk...
		if( 0 != mmioAscend( hmmio_, &ckOut1, 0 ) )
			return DXTRACE_ERR( TEXT("mmioAscend"), E_FAIL );
		
		return S_OK;
	}

	/** 書き込み
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 4:48:29
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WavFile::write(const void *src, size_t size, size_t * const wrotesize) {
		if( !(flags_&WRITE) ) return E_FAIL;
		if( !hmmio_ ) return CO_E_NOTINITIALIZED;
		if( !src ) return E_INVALIDARG;

		if(wrotesize) *wrotesize = 0;

		for(size_t cT = 0; cT < size; cT++ ) {
			if( mmioinfo_.pchNext == mmioinfo_.pchEndWrite ) {
				mmioinfo_.dwFlags |= MMIO_DIRTY;
				if( 0 != mmioAdvance( hmmio_, &mmioinfo_, MMIO_WRITE ) )
					return DXTRACE_ERR( TEXT("mmioAdvance"), E_FAIL );
			}

			*mmioinfo_.pchNext = *((BYTE*)src+cT);
			mmioinfo_.pchNext++;

			if(wrotesize) (*wrotesize)++;
		}

		return S_OK;
	}

}}} // namespace gctp
