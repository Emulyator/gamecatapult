#ifndef _GCTP_SCENE_CAMERA_HPP_
#define _GCTP_SCENE_CAMERA_HPP_
/** @file
 * GameCatapult カメラシーンノードクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/signal.hpp>
#include <gctp/types.hpp>
#include <gctp/strutumnode.hpp>
#include <gctp/frustum.hpp>
#include <gctp/scene/renderingnode.hpp>
#include <gctp/tuki.hpp>

namespace gctp { namespace scene {

	class Stage;
	/** カメラシーンノード
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 8:10:04
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Camera : public Renderer
	{
	public:
		/// コンストラクタ
		Camera();

		/// 独自Strutumを作る
		void setUp();
		/// 登場
		void enter(Stage &stage);
		/// 退場
		void exit(Stage &stage);

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
		/// サブウィンドウ
		const Rectf &subwindow() const { return subwindow_; }
		/// サブウィンドウ
		Rectf &subwindow() { return subwindow_; }
		/// アタッチしているノードを返す
		Pointer<StrutumNode> node() const { return node_; }
		/// アタッチしているノードを返す
		Pointer<StrutumNode> &node() { return node_; }
		/// 独自にウィンドウサイズを指定
		void setWindow(float w, float h)
		{
			window_.x = w;
			window_.y = h;
		}
		/// 現在のスクリーンからアスペクトを設定(デフォルト)
		void fitToScreen() { window_.x = 0; }
		/// 投影サイズがレンダーターゲットと一致しているか？
		bool doFitToScreen() const { return window_.x == 0 || window_.y == 0; }
		/// 投影モードか？
		bool isPerspective() const { return fov_ > 0; }

		/// ヴューマトリクスを返す
		Matrix view() const;
		/// 投影マトリクスを返す
		Matrix projection() const;
		/// 投影スクリーンサイズを返す
		Size2f screen() const;

		/// カメラをグラフィックシステムに設定
		void setToSystem() const;
		/// カメラ設定開始
		bool onEnter() const;
		/// カメラ設定終了
		bool onLeave() const;

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
		bool setUp(luapp::Stack &L);
		void enter(luapp::Stack &L);
		void exit(luapp::Stack &L);

	private:
		Pointer<StrutumNode> node_;
		
		float nearclip_;
		float farclip_;
		float fov_;
		Size2f window_;
		Rectf subwindow_;
		Frustum frustum_;

		mutable Camera* backup_current_;
		GCTP_TLS static Camera* current_;

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(Camera)
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_CAMERA_HPP_