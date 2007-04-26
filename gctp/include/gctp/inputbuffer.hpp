#ifndef _GCTP_INPUTBUFFER_HPP_
#define _GCTP_INPUTBUFFER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult キーボード入力バッファクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/12 18:39:23
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/utils.hpp>

namespace gctp {

	/** キーボードからの入力受付バッファクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/14 0:53:33
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class InputBuffer {
	public:
		typedef std::basic_string<_TCHAR> StringType;

		InputBuffer() : cur_(0), do_insert_(true) {}
		/// inputKeyで受け付けるキーコードのシノニム
		enum {
			PGUP   = VK_PRIOR,
			PGDN   = VK_NEXT,
			END    = VK_END,
			HOME   = VK_HOME,
			LEFT   = VK_LEFT,
			UP     = VK_UP,
			RIGHT  = VK_RIGHT,
			DOWN   = VK_DOWN,
			INSERT = VK_INSERT,
			DEL    = VK_DELETE
		};

		/** 入力（挿入モードなら挿入、上書きモードなら上書き）
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/14 0:56:43
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		void input(int c)
		{
			if(_istcntrl(c)) {
				switch(c) {
				case '\b': // BS
					if(cur_ <= buf_.length()) {
						cur_ = backCur();
						erase();
					}
					return;
				case '\x7f': // DEL
					erase();
					return;
				case '\n':
				case '\r':
				case '\t':
					break;
				default:
					return;
				}
			}
			if(do_insert_) insert(c);
			else replace(c);
		}

		/** キー入力（ASCIIで表現できないものを受け取る）
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/14 0:56:43
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		bool inputKey(int key)
		{
			switch(key) {
			case INSERT:
				do_insert_ = !do_insert_;
				break;
			case DEL:
				erase();
				break;
			case LEFT:
				cur_ = backCur();
				break;
			case RIGHT:
				cur_ = nextCur();
				break;
			case HOME:
				cur_ = 0;
				break;
			case END:
				cur_ = buf_.length();
				break;
			default:
				return false;
			}
			return true;
		}

		/** 入力（挿入モードなら挿入、上書きモードなら上書き）
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/14 0:56:43
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		void input(const _TCHAR *s)
		{
			int c;
			while(s && *s) {
				s += getChar(c, s);
				input(c);
			}
		}

		/** Ｃ文字列として入力バッファを取得
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/14 1:41:51
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		const _TCHAR *getCStr()
		{
			return buf_.c_str();
		}

		/** 入力バッファを取得
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/14 1:41:51
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		const StringType &get()
		{
			return buf_;
		}

		/** カーソルの現在位置
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/14 3:14:23
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		StringType::size_type cursor() { return cur_; }

		/** カーソル位置の文字を返す
		 *
		 * マルチバイトを考慮しない。
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/14 3:14:23
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		int cursorChar() { return cur_ <= buf_.length() ? buf_.c_str()[cur_] : 0; }

		/** 挿入モードか？
		 *
		 * false なら上書きモード。
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/14 7:18:29
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		bool isInserting() { return do_insert_; }

	protected:
		inline void insert(int c)
		{
			StringType _c = charToStr(c);
			buf_.insert(cur_, _c);
			cur_ += _c.length();
		}
		inline void replace(int c)
		{
			StringType _c = charToStr(c);
			buf_.replace(cur_, nextCur()-cur_, _c);
			cur_ += _c.length();
		}
		inline void erase()
		{
			buf_.erase(cur_, nextCur()-cur_);
		}
		inline StringType::size_type backCur()
		{
			StringType::size_type len = buf_.length();// size_typeはどこでもunsigedであるはずだ！！だから､ゼロ以下のチェックはしなくていいはず
#ifdef UNICODE
			if(cur_-1 <= len) return cur_ - 1;
#else
#ifdef GCTP_SHIFT_JIS
			// SJISは二文字目もisleadbyteがTRUEになるし、アスキー文字に見える文字もある。
			// 一文字戻るには、正しい境界にあることを保証された地点から順に見ていくしかない。（なるほどワイド文字は必要なはずだ）
			if(cur_ == 1) return 0;
			if(cur_-1 <= len) {
				mblen(NULL, 0);
				const char *str = buf_.c_str();
				StringType::size_type cur = 0;
				int n = 0;
				do {
					str+=n; cur+=n;
					n = mblen(str, (std::max)((int)MB_CUR_MAX, (int)(len-cur)));
					if(n <= 0) n = 1;
				} while(cur+n < cur_);
				return cur;
			}
#else
			if(cur_ == 1) return 0;
			if(cur_-1 <= len) {
				mblen(NULL, 0);
				const char *str = buf_.c_str();
				StringType::size_type cur = 0;
				int n = 0;
				do {
					str+=n; cur+=n;
					n = mblen(str, (std::max)((int)MB_CUR_MAX, (int)(len-cur)));
					if(n <= 0) n = 1;
				} while(cur+n < cur_);
				return cur;
			}
//			for(int i = MB_CUR_MAX; i > 0; i--) {
//				if(cur_-i <= len) {
//					mblen(NULL, 0);
//					int n = mblen(&buf_[cur_-i], i);
//					if(n == i) return cur_-i;
//				}
//			}
//			if(cur_-1 <= len) return cur_-1;
#endif
#endif
			return cur_;
		}
		inline StringType::size_type nextCur()
		{
			StringType::size_type len = buf_.length();
#ifdef UNICODE
			if(cur_+1 <= len) return cur_ + 1;
#else
			if(cur_+1 <= len) {
				mblen(NULL, 0); // 順に進む分には問題ないだろう
				int n = mblen(&buf_[cur_], (std::max)((int)MB_CUR_MAX, (int)(len-cur_)));
				if(n > 0 && cur_+n <= len) return cur_+n;
			}
#endif
			return cur_;
		}

		/// 入力を確保するバッファ
		StringType buf_; // これしかないか？？
		/// カーソル位置
		StringType::size_type cur_;
		bool do_insert_;
	};

} // namespace gctp

#endif //_GCTP_INPUTBUFFER_HPP_