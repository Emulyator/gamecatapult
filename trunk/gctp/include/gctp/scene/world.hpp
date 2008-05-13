#ifndef _GCTP_SCENE_WORLD_HPP_
#define _GCTP_SCENE_WORLD_HPP_
/** @file
 * GameCatapult ワールドクラスヘッダファイル
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
#include <gctp/scene/renderer.hpp>
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
	/** ワールドクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class World : public Renderer
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

		/// コンストラクタ
		World();

		/// ファイルからセットアップ
		virtual void setUp(const _TCHAR *filename);

		bool onUpdate(float delta);
		/// 更新スロット
		MemberSlot1<World, float /*delta*/, &World::onUpdate> update_slot;

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		/// カレントステージ（そのステージのupdate、draw…などの間だけ有効）
		World &current() { return *current_; }

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(World)
	
	protected:
		virtual bool doOnUpdate(float delta);

		bool setUp(luapp::Stack &L);
		void load(luapp::Stack &L);
		void activate(luapp::Stack &L);

	private:
#ifdef _MT
		mutable Mutex monitor_;
#endif
		mutable World* backup_current_;
		Pointer<DefaultSB> dsb_;
		GCTP_TLS static World* current_;
	};

}} // namespace gctp::scene

#endif // _GCTP_SCENE_WORLD_HPP_