#ifndef _GCTP_ARCHIVEEDITABLE_HPP_
#define _GCTP_ARCHIVEEDITABLE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapultアーカイブファイル編集クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/05 15:59:05
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/archive.hpp>

namespace gctp {

	/** 編集用アーカイブファイルクラス
	 *
	 * add、removeなどで操作を行った後、commitで実際に反映される。
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 * $Id: archive.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class ArchiveEditable : public Archive
	{
	public:
		class EntryAttr : public ArchiveEntry {
		public:
			enum Attr {ALREADY, NEW, UPDATE, REMOVE};
			/// エントリのパス
			TCStr name;
			/// 属性
			Attr attr;

			/// デフォルトコンストラクタ
			EntryAttr() : attr(ALREADY) {}
			/// コンストラクタ
			EntryAttr(const _TCHAR *name, ArchiveEntry &entry, Attr attr = ALREADY) : ArchiveEntry(entry), name(name), attr(attr) {}
			bool operator==(const EntryAttr &rhs) const { return pos==rhs.pos; }
			bool operator!=(const EntryAttr &rhs) const { return pos!=rhs.pos; }
			bool operator<(const EntryAttr &rhs) const { return pos<rhs.pos; }
			bool operator<=(const EntryAttr &rhs) const { return pos<=rhs.pos; }
			bool operator>(const EntryAttr &rhs) const { return pos>rhs.pos; }
			bool operator>=(const EntryAttr &rhs) const { return pos>=rhs.pos; }
		};
		/// 編集中エントリのリスト
		typedef std::list<EntryAttr> List;
		/// Listのiterator
		typedef List::iterator ListItr;

		/// デフォルトコンストラクタ
		ArchiveEditable() : alignment_(4) {}
		/// コンストラクタ
		explicit ArchiveEditable(const _TCHAR *fn);
		/// アライメントを指定(0にそろえられる下位ビット数で指定)
		void setAlign(int bit) { alignment_ = 1<<bit; }
		/// 開く
		void open(const _TCHAR *fn);
		/// エントリ追加
		void add(const _TCHAR *fn);
		/// 更新日付を無視し、強制再構築
		void forceUpdate(const _TCHAR *fn);
		/// 再構築
		void update();
		/// エントリ削除
		void remove(const _TCHAR *fn);
		/// 確定
		void commit();
		/// 編集中リストを出力
		void printList(std::ostream &os);
		//void printList(std::basic_ostream<_TCHAR> &os);
	protected:
		List list_; // 書き込みのときのリスト
		void setUpList();
		long calcIndexLen();
		long commitEntrys(int index_size);
		void commitIndex(int index_size);
		ListItr find(const _TCHAR *fn) {
			TLStr wfn(fn);
			ListItr i;
			for(i = list_.begin(); i != list_.end(); ++i) {
				if(i->name == wfn) break;
			}
			return i;
		}
		int alignment_;
		int align(int n) { return Archive::align(n, alignment_); }
	private:
		int index_page_size_;
	};

} // namespace gctp

#endif // _GCTP_ARCHIVEEDITABLE_HPP_
