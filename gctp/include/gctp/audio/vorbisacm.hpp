#ifndef _GCTP_AUDIO_VORBISACM_HPP_
#define _GCTP_AUDIO_VORBISACM_HPP_
/** @file
 * VorbisAcmDriverクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/20 19:15:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <mmreg.h>
#include <msacm.h>
#pragma comment(lib, "msacm32.lib")

namespace gctp { namespace audio {

	/** VorbisAcmDriverをロードするクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/20 19:16:17
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class VorbisAcmDriver {
	public:
		VorbisAcmDriver() : hndl_(0), id_(0), drv_(0) {}
		explicit VorbisAcmDriver(const _TCHAR * driver_path) : hndl_(0), id_(0), drv_(0) { initialize(driver_path); }
		~VorbisAcmDriver() { finalize(); }

		bool initialize(const _TCHAR * driver_path = _T("vorbis.acm"))
		{
			// ローカルドライバとして追加
			MMRESULT h;
			hndl_ = LoadLibrary(driver_path);
			if(!hndl_) return false;
			h = acmDriverAdd(&id_, hndl_, LPARAM(GetProcAddress(hndl_, "DriverProc")), 0, ACM_DRIVERADDF_FUNCTION);
			if(FAILED(h)) return false;
			h = acmDriverOpen(&drv_, id_, 0);
			if(FAILED(h)) return false;
			return true;
		}

		void finalize()
		{
			if(drv_) {
				acmDriverClose(drv_, 0);
				drv_ = 0;
			}
			if(id_) {
				acmDriverRemove(id_, 0);
				id_ = 0;
			}
			if(hndl_) {
				FreeLibrary(hndl_);
				hndl_ = 0;
			}
		}
	private:
		HMODULE			hndl_;
		HACMDRIVERID	id_;
		HACMDRIVER		drv_;
	};

}} // namespace gctp

#endif //_GCTP_AUDIO_VORBISACM_HPP_