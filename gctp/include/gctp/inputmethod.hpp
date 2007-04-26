#ifndef _GCTP_INPUTMETHOD_HPP_
#define _GCTP_INPUTMETHOD_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file
 * IMEクラス ヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/21 13:34:41
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <imm.h>
#pragma comment(lib, "imm32.lib")

namespace gctp {

	/** IMEの情報を保持
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/21 13:37:52
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class InputMethod {
	public:
		InputMethod() : hwnd_(0), is_open_(false) {}

		struct Context {
			Context(HWND hwnd) : hwnd_(hwnd), himc_(ImmGetContext(hwnd))
			{
			}
			~Context()
			{
				ImmReleaseContext(hwnd_, himc_);
			}
			operator HIMC() const { return himc_; }
			HWND hwnd_;
			HIMC himc_;
		};

		void setUp(HWND hwnd)
		{
			hwnd_ = hwnd;
		}

		bool open()
		{
			if(IsWindow(hwnd_)) {
				Context himc(hwnd_);
				ImmSetOpenStatus(himc, TRUE);
				is_open_ = ImmGetOpenStatus(himc) ? true : false;
			}
			return is_open_;
		}

		void close()
		{
			if(IsWindow(hwnd_)) {
				Context himc(hwnd_);
				ImmSetOpenStatus(himc, FALSE);
				is_open_ = ImmGetOpenStatus(himc) ? true : false;
			}
		}

		void setPos(Point2 pos)
		{
			if(IsWindow(hwnd_)) {
				Context himc(hwnd_);
				COMPOSITIONFORM form = { CFS_POINT, { pos.x, pos.y }, { 0, 0, 0, 0} };
				ImmSetCompositionWindow(himc, &form);
			}
		}

		void setFont(const LOGFONT &font)
		{
			if(IsWindow(hwnd_)) {
				Context himc(hwnd_);
				ImmSetCompositionFont(himc, (LOGFONT *)&font);
			}
		}

		bool onComposition(int status)
		{
			if(IsWindow(hwnd_)) {
				Context himc(hwnd_);
				if(!(status & GCS_RESULTSTR)){
					cur_ = ImmGetCompositionString(himc, GCS_CURSORPOS, NULL, 0);
					// 属性情報読み取り
					DWORD size = ImmGetCompositionString(himc, GCS_COMPATTR, NULL, 0);
					attrs_.resize(size);
					ImmGetCompositionString(himc, GCS_COMPATTR, &attrs_[0], size);
					// 現在の変換候補を取得
					size = ImmGetCompositionString(himc, GCS_COMPSTR, NULL, 0);
					str_.resize(size/sizeof(_TCHAR)+1);
					ImmGetCompositionString(himc, GCS_COMPSTR, (void *)str_.data(), size);
					str_[size/sizeof(_TCHAR)] = 0;
					return true;
				}
				else {
					cur_ = 0;
					str_.clear();
					attrs_.clear();
					return false;
				}
			}
			return false;
		}

		bool onNotify(int status)
		{
			if(IsWindow(hwnd_)) {
				Context himc(hwnd_);
				if(status == IMN_SETOPENSTATUS) {
					is_open_ = ImmGetOpenStatus(himc) ? true : false;
					ImmReleaseContext(hwnd_, himc);
					return false;
				}
			}
			return true;
		}

		const std::basic_string<_TCHAR> &str() const { return str_; }
		const std::vector<uint8_t> &attrs() const { return attrs_; }
		uint cursor() const { return cur_; }

		/// IMEがONか？
		bool isOpen() const { return is_open_; }

		static void setAlwaysOff(HWND hwnd)
		{
			ImmAssociateContext(hwnd, NULL);
		}

	private:
		HWND hwnd_;
		bool is_open_;
		uint cur_;
		std::vector<uint8_t> attrs_;
		std::basic_string<_TCHAR> str_; // 現在の編集内容
	};

}

#endif //_GCTP_INPUTMETHOD_HPP_