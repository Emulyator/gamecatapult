#ifndef _GCTP_SCENE_ENTITY_HPP_
#define _GCTP_SCENE_ENTITY_HPP_
/** @file
 * GameCatapult エンティティクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/signal.hpp>
#include <gctp/scene/renderer.hpp>
#include <gctp/scene/body.hpp>
#include <gctp/tuki.hpp>

namespace gctp {
	class Context;
}

namespace gctp { namespace scene {

	class Stage;
	class Motion;
	class MotionMixer;
	/** エンティティクラス
	 *
	 * Bodyにモーションを与える主体
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/16 1:06:15
	 */
	class Entity : public Object
	{
	public:
		/// コンストラクタ
		Entity(int16_t update_pri = 0);

		/// セットアップ
		void setUp(Pointer<Body> src);
		/// セットアップ
		void setUp(const _TCHAR *filename);

		/// 登場
		void enter(Stage &stage);
		/// プライオリティーを指定してステージ登場
		void enter(Stage &stage, int16_t update_pri);
		/// 退場
		void exit(Stage &stage);

		/// スケルトンがあるか？
		bool hasSkeleton() { return target_; }
		/// スケルトン
		Skeleton &skeleton() { return *target_; }
		/// スケルトン
		const Skeleton &skeleton() const { return *target_; }
		/// モーションミキサーがあるか？
		bool hasMotionMixer() { return mixer_; }
		/// モーションミキサー
		MotionMixer &mixer() { return *mixer_; }
		/// モーションミキサー
		const MotionMixer &mixer() const { return *mixer_; }

		/// 操作対象
		Handle<Body> target() { return target_; }

		/// ターゲットのローカル座標位置
		const Vector &lpos() const { return (*target_)->val.lcm().position(); }
		/// ターゲットのローカル座標位置
		Vector &getLpos() { return (*target_)->val.getLCM().position(); }
		/// ターゲットのグローバル座標位置
		const Vector &pos() const { return (*target_)->val.wtm().position(); }
		/// ターゲットのグローバル座標位置
		Vector &pos() { return (*target_)->val.wtm().position(); }
		/// ターゲットのローカル座標
		const Matrix &lcm() const { return (*target_)->val.lcm(); }
		/// ターゲットのローカル座標
		Matrix &getLCM() { return (*target_)->val.getLCM(); }
		/// ターゲットのグローバル座標
		const Matrix &wtm() const { return (*target_)->val.wtm(); }
		/// ターゲットのグローバル座標
		Matrix &wtm() { return (*target_)->val.wtm(); }

		/// 更新
		bool onUpdate(float delta);
		/// 更新スロット
		MemberSlot1<Entity, float /*delta*/, &Entity::onUpdate> update_slot;

	protected:
		bool setUp(luapp::Stack &L);
		void enter(luapp::Stack &L);
		void exit(luapp::Stack &L);
		void setPos(luapp::Stack &L);
		void getPos(luapp::Stack &L);
		
		virtual bool doOnUpdate(float delta);
		Pointer<MotionMixer> mixer_;
		Pointer<Body> target_;

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(Entity)
	};

	/// エンティティーをシーンに追加
	extern Handle<Entity> newEntity(Context &context, Stage &stage, const char *classname, const _TCHAR *name, const _TCHAR *srcfilename);
	/// エンティティーをシーンに追加
	extern Handle<Entity> newEntity(Context &context, Stage &stage, const std::type_info &typeinfo, const _TCHAR *name, const _TCHAR *srcfilename);

}} // namespace gctp::scene

#endif //_GCTP_SCENE_ENTITY_HPP_