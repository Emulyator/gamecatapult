/** @file
 * GameCatapult オーディオクリップクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/25 19:21:07
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/audio/clip.hpp>
#include <gctp/audio/dx/wavfile.hpp>
#include <gctp/audio/oggvorbis.hpp>
#include <gctp/turi.hpp>
#include <gctp/file.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace audio {

	int Clip::on_memory_streaming_threshold__ = 128*1024; // 128k

	Clip::Clip()
	{
		synchronize(true);
	}

	bool Clip::open(const _TCHAR *path)
	{
		stream_ = 0;
		AbstractFilePtr filei = fileserver().getFileInterface(path);
		TURI uri(path);
		if(uri.extension()==_T("wav")) {
			Pointer<dx::WavFile> wav = new dx::WavFile;
			wav->synchronize(true);
			stream_ = wav;
			if(wav) {
				if(filei) {
					if(filei->size() < on_memory_streaming_threshold__) {
						on_memory_ = fileserver().getFile(path);
						return wav->open(on_memory_->buf(), on_memory_->size()) ? true : false;
					}
					else return wav->open(filei) ? true : false;
				}
				else {
					File f(path);
					if(f.is_open()) {
						if(f.length() < on_memory_streaming_threshold__) {
							on_memory_ = f.load();
							return wav->open(on_memory_->buf(), on_memory_->size()) ? true : false;
						}
						else return wav->open(path) ? true : false;
					}
				}
			}
		}
		else if(uri.extension()==_T("ogg")) {
			Pointer<OggVorbis> ogg = new OggVorbis;
			ogg->synchronize(true);
			stream_ = ogg;
			if(ogg) {
				if(filei) {
					if(filei->size() < on_memory_streaming_threshold__) {
						on_memory_ = fileserver().getFile(path);
						return ogg->open(on_memory_->buf(), on_memory_->size()) ? true : false;
					}
					else return ogg->open(filei) ? true : false;
				}
				else {
					File f(path);
					if(f.is_open()) {
						if(f.length() < on_memory_streaming_threshold__) {
							on_memory_ = f.load();
							return ogg->open(on_memory_->buf(), on_memory_->size()) ? true : false;
						}
						else return ogg->open(path) ? true : false;
					}
				}
			}
		}
		return false;
	}

	bool Clip::isOpen()
	{
		Pointer<dx::WavFile> wav = stream_;
		if(wav) return wav->isOpen();
		Pointer<OggVorbis> ogg = stream_;
		if(ogg) return ogg->isOpen();
		return false;
	}

	HRslt Clip::read(void *dst, std::size_t size, std::size_t * const readsize)
	{
		Pointer<dx::WavFile> wav = stream_;
		if(wav) return wav->read(dst, size, readsize);
		Pointer<OggVorbis> ogg = stream_;
		if(ogg) return ogg->read(dst, size, readsize);
		return false;
	}

	HRslt Clip::rewind()
	{
		Pointer<dx::WavFile> wav = stream_;
		if(wav) return wav->rewind();
		Pointer<OggVorbis> ogg = stream_;
		if(ogg) return ogg->rewind();
		return E_POINTER;
	}

	std::size_t Clip::size() const
	{
		Pointer<dx::WavFile> wav = stream_;
		if(wav) return wav->size();
		Pointer<OggVorbis> ogg = stream_;
		if(ogg) return ogg->size();
		return false;
	}

	const WAVEFORMATEX *Clip::format() const
	{
		Pointer<dx::WavFile> wav = stream_;
		if(wav) return wav->format();
		Pointer<OggVorbis> ogg = stream_;
		if(ogg) return ogg->format();
		return 0;
	}

}} // namespace gctp
