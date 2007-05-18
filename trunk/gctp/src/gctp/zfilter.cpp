/** @file
 * GameCatapult zipアーカイブファイルクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/12/30
 * $Id: archive.cpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/zfilter.hpp>
#ifdef GCTP_USE_ZLIB
#include "zlib.h"

using namespace std;

namespace gctp {

	/// zlibのエラーを投げる例外クラス
	class zlib_exception : public std::runtime_error {
	public:
		zlib_exception(const char* s) : std::runtime_error(s) {}
		~zlib_exception() {}
	};

	struct ZStreamImpl {
		z_stream stream_;
		int flush_;
		bool infrate_;
	};

	ZStream::ZStream() : impl_(new ZStreamImpl)
	{
	}

	ZStream::~ZStream()
	{
		delete impl_;
	}

	bool ZStream::open(bool infrate)
	{
		impl_->infrate_ = infrate;
		impl_->flush_ = Z_NO_FLUSH;
		if(infrate) {
			// zlib 初期化
			impl_->stream_.zalloc		= Z_NULL;
			impl_->stream_.zfree		= Z_NULL;
			impl_->stream_.opaque		= Z_NULL;
			impl_->stream_.next_in		= Z_NULL;
			impl_->stream_.avail_in	= 0;
			return (inflateInit(&impl_->stream_)==Z_OK);
		}
		else {
			// zlib 初期化
			impl_->stream_.zalloc		= Z_NULL;
			impl_->stream_.zfree		= Z_NULL;
			impl_->stream_.opaque		= Z_NULL;
			impl_->stream_.next_in		= Z_NULL;
			impl_->stream_.avail_in	= 0;
			return (deflateInit(&impl_->stream_, Z_DEFAULT_COMPRESSION) == Z_OK);
		}
	}

	void ZStream::close()
	{
		if(impl_->infrate_){
			if(inflateEnd(&impl_->stream_)!=Z_OK){
				throw zlib_exception(impl_->stream_.msg);
			}
		}
		else {
			if(deflateEnd(&impl_->stream_)!=Z_OK){
				throw zlib_exception(impl_->stream_.msg);
			}
		}
	}

	ZStream::Status ZStream::process()
	{
		int status;
		if(impl_->infrate_) {
			status = inflate(&impl_->stream_, impl_->flush_);
			if(status == Z_OK) return STATUS_OK;
			if(status == Z_STREAM_END) return STATUS_END;
		}
		else {
			status = deflate(&impl_->stream_, impl_->flush_);
			if(status == Z_OK) return STATUS_OK;
			if(status == Z_STREAM_END) return STATUS_END;
		}
		throw zlib_exception(impl_->stream_.msg);
	}

	void ZStream::setInput(unsigned char *p, unsigned int s)
	{
		impl_->stream_.next_in	= p;
		impl_->stream_.avail_in = s;
	}

	void ZStream::setOutput(unsigned char *p, unsigned int s)
	{
		impl_->stream_.next_out = p;
		impl_->stream_.avail_out = s;
	}

	void ZStream::setFlushMode(FlushMode mode)
	{
		switch(mode) {
		case FLUSHMODE_NONE:
			impl_->flush_ = Z_NO_FLUSH;
			break;
		case FLUSHMODE_FINISH:
			impl_->flush_ = Z_FINISH;
			break;
		}
	}

	ZStream::FlushMode ZStream::getFlushMode()
	{
		switch(impl_->flush_) {
		case Z_FINISH:
			return FLUSHMODE_FINISH;
		}
		return FLUSHMODE_NONE;
	}

	unsigned int ZStream::inputAvail()
	{
		return impl_->stream_.avail_in;
	}

	unsigned int ZStream::outputAvail()
	{
		return impl_->stream_.avail_out;
	}

}
#else
namespace gctp {
}
#endif // GCTP_USE_ZLIB
