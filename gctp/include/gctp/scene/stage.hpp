#ifndef _GCTP_SCENE_STAGE_HPP_
#define _GCTP_SCENE_STAGE_HPP_
/** @file
 * GameCatapult ステージクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/db.hpp>
#include <gctp/strutumtree.hpp>
#include <gctp/signal.hpp>
#include <gctp/tuki.hpp>
#ifdef _MT
#include <gctp/mutex.hpp>
#endif

namespace gctp {
	class Context;
}

namespace gctp { namespace scene {

	class Body;
	class Light;
	class DefaultSB;
	/** ステージクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Stage : public Object, public DB
	{
	public:
		/// 階層ツリー
		StrutumTree strutum_tree;
		/// Bodyリスト
		HandleList<Body> body_list;
		// Lightリスト(ModifierListと抽象化すべき？)
		HandleList<Light> light_list;

		/** 更新シグナル
		 *
		 * 更新スロットはこれに接続する
		 */
		Signal1<false, float /*delta*/> update_signal;
		/** 更新後シグナル
		 *
		 * アニメーションによるローカル階層のセットアップ、およびltmセットアップ後の処理\n
		 * コリジョンスロットはこれに接続する
		 */
		Signal1<false, float /*delta*/> postupdate_signal;
		/** 描画シグナル
		 *
		 * 描画スロットはこれに接続する
		 */
		Signal1<false, float /*delta*/> draw_signal;

		/// コンストラクタ
		Stage();

		/// DBに登録
		bool insert(const Hndl hndl, const _TCHAR *key);

		/// ファイルからセットアップ
		virtual void setUp(const _TCHAR *filename);

		bool onUpdate(float delta);
		/// 更新スロット
		MemberSlot1<Stage, float /*delta*/, &Stage::onUpdate> update_slot;

		bool onDraw(float delta) const;
		/// 描画スロット
		ConstMemberSlot1<const Stage, float /*delta*/, &Stage::onDraw> draw_slot;

		/// カレントステージ（そのステージのupdate、draw…などの間だけ有効）
		Stage &current() { return *current_; }

		/// ノード追加
		Hndl newNode(Context &context, const char *classname, const _TCHAR *name = 0, const _TCHAR *srcfilename = 0);
		/// ノード追加
		Hndl newNode(Context &context, const GCTP_TYPEINFO &typeinfo, const _TCHAR *name = 0, const _TCHAR *srcfilename = 0);

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(Stage)
	
	protected:
		virtual bool doOnUpdate(float delta);
		virtual bool doOnDraw(float delta) const;

		bool setUp(luapp::Stack &L);
		void load(luapp::Stack &L);
		void newNode(luapp::Stack &L);
		void activate(luapp::Stack &L);
		void show(luapp::Stack &L);
		void hide(luapp::Stack &L);

	private:
#ifdef _MT
		mutable Mutex monitor_;
#endif
		mutable Stage* backup_current_;
		Pointer<DefaultSB> dsb_;
		GCTP_TLS static Stage* current_;
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_STAGE_HPP_