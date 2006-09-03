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
#include <gctp/scene/renderingtree.hpp>
#include <gctp/signal.hpp>
#include <gctp/tuki.hpp>
#ifdef _MT
#include <boost/thread/mutex.hpp>
#endif

namespace gctp {
	class Context;
}

namespace gctp { namespace scene {

	class Body;
	class Light;
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
		/// 描画ツリー
		RenderingTree rendering_tree;
		/// Bodyリスト
		HandleList<Body> body_list;
		// Cameraリスト
		//HandleList<Camera> camera_list;
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
		Stage();

		/// DBに登録
		bool insert(const Hndl hndl, const char *key);

		/// ファイルからセットアップ
		virtual void setUp(const char *filename);

		bool onUpdate(float delta);
		/// 更新スロット
		MemberSlot1<Stage, float /*delta*/, &Stage::onUpdate> update_slot;

		bool onDraw() const;
		/// 描画スロット
		MemberSlot0<const Stage, &Stage::onDraw> draw_slot;

		/// カレントシーン（そのシーンのupdate、draw…などの間だけ有効）		
		Stage &current() { return *current_; }

		/// ノード追加
		Hndl newNode(Context &context, const char *classname, const char *name = 0, const char *srcfilename = 0);
		/// ノード追加
		Hndl newNode(Context &context, const GCTP_TYPEINFO &typeinfo, const char *name = 0, const char *srcfilename = 0);

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(Stage)
	
	protected:
		virtual bool doOnUpdate(float delta);
		virtual bool doOnDraw() const;

		bool setUp(luapp::Stack &L);
		void load(luapp::Stack &L);

	private:
#ifdef _MT
		mutable boost::mutex monitor_;
#endif
		mutable Stage* backup_current_;
		GCTP_TLS static Stage* current_;
	};

	/** Stageを描画するレンダリングノード
	 *
	 * とりあえず…設計が固まらないので
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/03/23 3:26:15
	 */
	class StageRenderer : public Renderer
	{
	public:
		/// コンストラクタ
		StageRenderer() {}
		/// コンストラクタ
		StageRenderer(Handle<Stage> stage);
		/// ノード到達時の処理
		virtual bool onEnter() const;

		/// 描画対象のStage
		Handle<Stage> &stage() { return stage_; }
		/// 描画対象のStage
		Handle<Stage> stage() const { return stage_; }
	
	GCTP_DECLARE_CLASS

	private:
		Handle<Stage> stage_;
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_STAGE_HPP_