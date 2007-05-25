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

	namespace {
		// オリジナルのIOProc
		LRESULT PASCAL ioprocForWavfile(LPSTR lpmmioinfo, UINT wMsg, LPARAM lParam1, LPARAM lParam2)
		{
			MMIOINFO *mmioinfo = (MMIOINFO *)lpmmioinfo;
			AbstractFilePtr *__self = (AbstractFilePtr *)mmioinfo->adwInfo;
			switch(wMsg) {
			case MMIOM_OPEN:
				mmioinfo->lDiskOffset = 0;
				break;
			case MMIOM_CLOSE:
				(*__self) = 0;
				break;
			case MMIOM_READ:{
				int ret = (*__self)->read((void *)lParam1, lParam2);
				if(ret > 0) mmioinfo->lDiskOffset += ret;
				return ret;
							}
			//case MMIOM_WRITE:
			case MMIOM_SEEK:
				switch (lParam2) {
				case SEEK_SET:
					mmioinfo->lDiskOffset = (LONG)lParam1;
					break;

				case SEEK_CUR:
					mmioinfo->lDiskOffset += (LONG)lParam1;
					break;

				case SEEK_END:
					mmioinfo->lDiskOffset = (LONG)((*__self)->size() - 1 - lParam1);
					break;
				}
				(*__self)->seek(mmioinfo->lDiskOffset);
				return mmioinfo->lDiskOffset;
			}
			return 0;
		}

		void installIOProc()
		{
			static bool installed = false;
			if(!installed) {
				installed = true;
				mmioInstallIOProc(mmioFOURCC('g', 'c', 't', 'p'), (LPMMIOPROC)ioprocForWavfile, MMIO_INSTALLPROC);
			}
		}
	}

	WavFile::WavFile() : wfx_(0), hmmio_(0), size_(0), flags_(0), buffer_(0) {}

	WavFile::WavFile(const _TCHAR *fname)
		: wfx_(0), hmmio_(0), size_(0), flags_(0), buffer_(0) {
		open(fname);
	}

	WavFile::WavFile(const _TCHAR *fname, const WAVEFORMATEX * const wfx)
		: wfx_(0), hmmio_(0), size_(0), flags_(0), buffer_(0) {
		open(fname, wfx);
	}

	WavFile::WavFile(const void * const src, std::size_t size)
		: wfx_(0), hmmio_(0), size_(0), flags_(0), buffer_(0) {
		open(src, size);
	}

	WavFile::WavFile(AbstractFilePtr fileptr)
		: wfx_(0), hmmio_(0), size_(0), flags_(0), buffer_(0) {
		open(fileptr);
	}

	WavFile::~WavFile() {
		close();
	}

	/** ファイルからの読み込みモードでオープン
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 1:51:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WavFile::open(const _TCHAR *fname) {
		if(!fname) return E_INVALIDARG;
		if(isOpen()) close();
		flags_ = READ;

		HRslt hr;
		_TCHAR fn[_MAX_PATH];
		_tcsncpy(fn, fname, _MAX_PATH);
		hmmio_ = mmioOpen(fn, NULL, MMIO_ALLOCBUF | MMIO_READ | MMIO_COMPAT);
		if(!hmmio_) {
			// Loading it as a file failed, so try it as a resource
			HRSRC hResInfo = FindResource(NULL, fn, TEXT("WAVE"));
			if(!hResInfo) {
				hResInfo = FindResource(NULL, fn, TEXT("WAV"));
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

			hmmio_ = mmioOpen(NULL, &mmioinfo, MMIO_ALLOCBUF | MMIO_READ | MMIO_COMPAT);
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
	HRslt WavFile::open(const _TCHAR *fname, const WAVEFORMATEX * const wfx) {
		if(isOpen()) close();

		_TCHAR fn[_MAX_PATH];
		_tcsncpy(fn, fname, _MAX_PATH);
		hmmio_ = mmioOpen(fn, NULL, MMIO_ALLOCBUF|MMIO_READWRITE|MMIO_CREATE);
		if(!hmmio_) return DXTRACE_ERR(TEXT("mmioOpen"), E_FAIL);
		flags_ = WRITE;

		HRslt hr;
		if(!( hr = writeMMIO(wfx) )) {
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
	HRslt WavFile::open(const void * const src, std::size_t size) {
		if(isOpen()) close();
		flags_ = READ;

		HRslt hr;
		MMIOINFO mmioinfo;
		memset(&mmioinfo, 0, sizeof(mmioinfo));
		mmioinfo.fccIOProc = FOURCC_MEM;
		mmioinfo.cchBuffer = (DWORD)size;
		mmioinfo.pchBuffer = const_cast<char *>(reinterpret_cast<const char *>(src));
		hmmio_ = mmioOpen(NULL, &mmioinfo, MMIO_ALLOCBUF | MMIO_READ | MMIO_COMPAT);

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

	/** 抽象ファイルからの読み込み用にオープン
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 2:32:45
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WavFile::open(AbstractFilePtr fileptr) {
		if(isOpen()) close();
		flags_ = READ;
		HRslt hr;
		MMIOINFO mmioinfo;
		memset(&mmioinfo, 0, sizeof(mmioinfo));
		AbstractFilePtr *__self = (AbstractFilePtr *)mmioinfo.adwInfo;
		(*__self) = fileptr;
		installIOProc();
		mmioinfo.fccIOProc = mmioFOURCC('g', 'c', 't', 'p');
		hmmio_ = mmioOpen(NULL, &mmioinfo, MMIO_ALLOCBUF | MMIO_READ | MMIO_COMPAT);

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

	/** ストリームを閉じる
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/21 3:32:04
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WavFile::close() {
		if( !hmmio_ ) return CO_E_NOTINITIALIZED;
		if( flags_ & READ ) {
			mmioClose( hmmio_, 0 );
			hmmio_ = NULL;
		}
		else {
			write_mmioinfo_.dwFlags |= MMIO_DIRTY;
			if( 0 != mmioSetInfo( hmmio_, &write_mmioinfo_, 0 ) )
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
		SAFE_FREE( buffer_ );
		SAFE_FREE( wfx_ );

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

			if( 0 != mmioGetInfo( hmmio_, &write_mmioinfo_, 0 ) )
				return DXTRACE_ERR( TEXT("mmioGetInfo"), E_FAIL );
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
		if( !hmmio_ ) return CO_E_NOTINITIALIZED;
		if( !dst ) return E_INVALIDARG;

		if( readsize ) *readsize = 0;

		MMIOINFO mmioinfo; // current status of hmmio_
		if( 0 != mmioGetInfo( hmmio_, &mmioinfo, 0 ) )
			return DXTRACE_ERR( TEXT("mmioGetInfo"), E_FAIL );

		DWORD cbDataIn = (DWORD)size;
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
			if( write_mmioinfo_.pchNext == write_mmioinfo_.pchEndWrite ) {
				write_mmioinfo_.dwFlags |= MMIO_DIRTY;
				if( 0 != mmioAdvance( hmmio_, &write_mmioinfo_, MMIO_WRITE ) )
					return DXTRACE_ERR( TEXT("mmioAdvance"), E_FAIL );
			}

			*write_mmioinfo_.pchNext = *((BYTE*)src+cT);
			write_mmioinfo_.pchNext++;

			if(wrotesize) (*wrotesize)++;
		}

		return S_OK;
	}

}}} // namespace gctp
