#ifndef _GCTP_URI_HPP_
#define _GCTP_URI_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult リソース名解析クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:33:23
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <string>
#include <iosfwd>

namespace gctp {

	/** GameCatapult リソース名クラス
	 *
	 * リソース名（一応URI風に記述）の解析ユーティリティー。
	 * 拡張子部分を抜き出したり、パスとリーフに分割したり。
	 *
	 * マルチバイトの判定にisleadbyteを使っているので、setlocaleしておく必要あり。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:20:37
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class URI {
	public:
		URI(const char *rn) : rn_(rn) { convertBackSlashToSlash(); }
		URI(const std::string &rn) : rn_(rn) { convertBackSlashToSlash(); }

		/** スキームを抽出
		 *
		 * c:/xxx/yyy/zzz.ee.ext だったら、c が
		 * http://xxx/yyy/zzz.ext だったら、http が
		 * 帰ってくる。
		 */
		inline std::string scheme() const
		{
			std::string::size_type end = rn_.find_first_of(':');
			if(end != std::string::npos) return rn_.substr(0, end);
			return "";
		}

		/** パスを抽出
		 *
		 * c:/xxx/yyy/zzz.ee.ext だったら、c:/xxx/yyy が
		 * http://xxx/yyy/zzz.ext だったら、http://xxx/yyy が
		 * 帰ってくる。
		 */
		inline std::string path() const
		{
			std::string::size_type end = rn_.find_last_of('/');
			if(end != std::string::npos) return rn_.substr(0, end);
			return "";
		}

		/** ディレクトリを抽出
		 *
		 * c:/xxx/yyy/zzz.ee.ext だったら、/xxx/yyy が
		 * http://xxx/yyy/zzz.ext だったら、//xxx/yyy が
		 * 帰ってくる。
		 */
		inline std::string directory() const
		{
			std::string::size_type begin = rn_.find_first_of(':');
			if(begin == std::string::npos) begin = 0;
			else begin++;
			std::string::size_type end = rn_.find_last_of('/');
			if(end != std::string::npos) return rn_.substr(begin, end-begin);
			return "";
		}

		/** リーフを抽出
		 *
		 * c:/xxx/yyy/zzz.ee.extだったら、zzz.ee.ext が
		 * http://xxx/yyy/zzz.ext だったら、zzz.ext が
		 * 帰ってくる。
		 */
		inline std::string leaf() const
		{
			std::string::size_type begin = rn_.find_last_of('/');
			if(begin == std::string::npos) {
				begin = rn_.find_first_of(':');
				if(begin == std::string::npos) return rn_;
				begin++;
			}
			else begin++;
			return rn_.substr(begin);
		}

		/** ベース名を抽出
		 *
		 * c:/xxx/yyy/zzz.ee.ext だったら、zzz が
		 * http://xxx/yyy/zzz.ext だったら、zzz が
		 * 帰ってくる。
		 */
		inline std::string basename() const
		{
			std::string::size_type begin = rn_.find_last_of('/');
			if(begin == std::string::npos) {
				begin = rn_.find_first_of(':');
				if(begin == std::string::npos) begin = 0;
				else begin++;
			}
			else begin++;

			std::string::size_type end = rn_.find_first_of('.', begin);
			if(begin != std::string::npos) {
				if(end != std::string::npos) return rn_.substr(begin, end-begin);
				else return rn_.substr(begin);
			}
			return "";
		}

		/** 拡張子を抽出
		 *
		 * c:/xxx/yyy/zzz.ee.ext だったら、ee.ext が
		 * http://xxx/yyy/zzz.ext だったら、ext が
		 * 帰ってくる。
		 */
		inline std::string extension() const
		{
			std::string::size_type lower = rn_.find_last_of('/');
			std::string::size_type begin = rn_.find_last_of('.');
			while(begin != std::string::npos && begin > 0) {
				std::string::size_type _next = rn_.find_last_of('.', begin-1);
				if(_next == std::string::npos || (lower != std::string::npos && _next<lower)) return rn_.substr(begin+1);
				begin = _next;
			}
			return "";
		}

		/// 生ストリングを返す
		inline const std::string &raw() const
		{
			return rn_;
		}

		/// すべてを小文字に変換
		inline void convertLower()
		{
			for(std::string::size_type i = 0; i < rn_.size(); i++) {
				if(isleadbyte(rn_[i])) i++;
				else rn_[i] = tolower(rn_[i]);
			}
		}

	private:
		void convertBackSlashToSlash()
		{
			for(std::string::size_type i = 0; i < rn_.size(); i++) {
				if(isleadbyte(rn_[i])) i++;
				else if(rn_[i] == '\\') rn_[i] = '/';
			}
		}
		std::string rn_;
	};
	
	template<class E, class T>
	std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, URI const & uri)
	{
		return os << uri.raw();
	}

} //namespace gctp

#endif //_GCTP_URI_HPP_