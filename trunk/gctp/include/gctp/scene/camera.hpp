#ifndef _GCTP_SCENE_CAMERA_HPP_
#define _GCTP_SCENE_CAMERA_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult カメラクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/types.hpp>
#include <gctp/strutumnode.hpp>
#include <gctp/graphic.hpp>
#include <gctp/frustum.hpp>
#include <gctp/tuki.hpp>
#include <gctp/color.hpp>
#include <gctp/scene/renderer.hpp>
#include <gctp/scene/aspectstrutumnode.hpp>

namespace gctp { namespace scene {

	class World;

	/** カメラ
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 8:10:04
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Camera : public AspectStrutumNode<Renderer>
	{
	public:
		/// コンストラクタ
		Camera();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		/// ニアクリップ
		const float &nearclip() const { return nearclip_; }
		/// ニアクリップ
		float &nearclip() { return nearclip_; }
		/// ファークリップ
		const float &farclip() const { return farclip_; }
		/// ファークリップ
		float &farclip() { return farclip_; }
		/// 視野角
		const float &fov() const { return fov_; }
		/// 視野角
		float &fov() { return fov_; }
		/// アスペクト比明示
		void setAspectRatio(float aspect_ratio) { aspect_ratio_ = aspect_ratio; }
		/// アスペクト比
		float aspectRatio() const;
		/// 描画領域矩形
		const Rectf &renderRect() const { return render_rect_; }
		/// 描画領域矩形
		Rectf &renderRect() { return render_rect_; }
		/// 現在のスクリーンからアスペクトを設定(デフォルト)
		void fitToScreen() { aspect_ratio_ = 0; }
		/// 投影サイズがレンダーターゲットと一致しているか？
		bool isFittedToScreen() const { return aspect_ratio_ == 0; }
		/// 投影モードか？
		bool isPerspective() const { return fov_ > 0; }

		/// ヴューマトリクスを返す
		Matrix view() const;
		/// 投影マトリクスを返す
		Matrix projection() const;

		const Matrix &viewprojection() const { return viewprojection_; }

		/// カメラをグラフィックシステムに設定
		void setToSystem() const;
		/// カメラ設定開始
		void begin() const;
		/// カメラ設定終了
		void end() const;

		/// カレントカメラ（このカメラの子レンダリング要素にのみ有効）
		static Camera &current() { return *current_; }

		/// ヴューフラスタム内か判定
		bool isVisible(const Sphere &bs) const
		{
			return frustum_.isColliding(bs);
		}

		/// 視錘台
		const Frustum &frustum() { return frustum_; }

		/// 視錘台を更新
		void update();

	protected:
		bool LuaCtor(luapp::Stack &L);
		void setClip(luapp::Stack &L);
		int getClip(luapp::Stack &L);
		void setAspectRatio(luapp::Stack &L);
		int getAspectRatio(luapp::Stack &L);
		void setRenderRect(luapp::Stack &L);
		int getRenderRect(luapp::Stack &L);
		void setFov(luapp::Stack &L);
		int getFov(luapp::Stack &L);
		void setFogColor(luapp::Stack &L);
		void setFogParam(luapp::Stack &L);
		void setViewPort(luapp::Stack &L);

	private:
		float nearclip_;
		float farclip_;
		float fov_;
		float aspect_ratio_; // 0はスクリーンサイズから求める
		graphic::ViewPort view_port_;
		mutable graphic::ViewPort view_port_bak_;
		Rectf render_rect_; // これいるのか？
		Frustum frustum_;
		mutable Matrix viewprojection_;

		// フォグ関連（暫定）
		bool fog_enable_;
		float fog_start_;
		float fog_end_;
		Color32 fog_color_;

		mutable Camera *backup_current_;
		GCTP_TLS static Camera *current_;

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(Camera);
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_CAMERA_HPP_