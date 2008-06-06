#ifndef _GCTP_SCENE_WORLDRENDERER_HPP_
#define _GCTP_SCENE_WORLDRENDERER_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 簡易効果ノードクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/tuki.hpp>
#include <gctp/handlelist.hpp>
#include <gctp/scene/renderer.hpp>
#include <gctp/scene/drawpacket.hpp>

namespace gctp {
	namespace graphic {
		namespace dx {
			class StateBlockRsrc;
		}
	}
}

namespace gctp { namespace scene {

	class World;
	
	/** ソート済み描画パケットを保持するノードクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class WorldSorter : public Renderer
	{
	public:
		WorldSorter();

		virtual bool onReach(float delta) const;

		/// 描画対象追加
		void add(Handle<World> world);
		/// 描画対象削除
		void remove(Handle<World> world);

		mutable DrawPacketVector packets;

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(WorldSorter)
	
	protected:
		HandleList<World> worlds_;

		bool setUp(luapp::Stack &L);
		void add(luapp::Stack &L);
		void remove(luapp::Stack &L);
	};

	/** 簡易効果ノードクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class WorldRenderer : public WorldSorter
	{
	public:
		WorldRenderer();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(WorldRenderer)
	
	private:
		Pointer<graphic::dx::StateBlockRsrc> sb_;
	};

	/** 不透明体描画効果ノードクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class OpequeWorldRenderer : public Renderer
	{
	public:
		OpequeWorldRenderer();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		void attach(Handle<WorldSorter> target)
		{
			target_ = target;
		}

	protected:
		Handle<WorldSorter> target_;

		bool setUp(luapp::Stack &L);
		void attach(luapp::Stack &L);
	
	private:
		Pointer<graphic::dx::StateBlockRsrc> sb_;

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(OpequeWorldRenderer)
	};

	/** 半透明体描画ノードクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class TranslucentWorldRenderer : public Renderer
	{
	public:
		TranslucentWorldRenderer();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		void attach(Handle<WorldSorter> target)
		{
			target_ = target;
		}

	protected:
		Handle<WorldSorter> target_;

		bool setUp(luapp::Stack &L);
		void attach(luapp::Stack &L);
	
	private:
		Pointer<graphic::dx::StateBlockRsrc> sb_;

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(TranslucentWorldRenderer)
	};

	/** Zフィル用
	 *
	 * ソートしないでとにかく書く
	 */
	class ZFillRenderer : public Renderer
	{
	public:
		ZFillRenderer();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		/// 描画対象追加
		void add(Handle<World> world);
		/// 描画対象削除
		void remove(Handle<World> world);

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(ZFillRenderer)
	
	protected:
		HandleList<World> worlds_;
		mutable const char *backup_tech_;

		bool setUp(luapp::Stack &L);
		void add(luapp::Stack &L);
		void remove(luapp::Stack &L);

	private:
		Pointer<graphic::dx::StateBlockRsrc> sb_;
	};

}} // namespace gctp::scene

#endif // _GCTP_SCENE_GENERICEFFECTER_HPP_