/** @file
 * GameCatapult カメラシーンノードクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/camera.hpp>
#include <gctp/scene/stage.hpp>
#include <gctp/graphic.hpp>
#include <gctp/aabox.hpp>
#include <gctp/app.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	Camera::Camera() : nearclip_(0.1f), farclip_(1000.0f), fov_(g_pi/4)
	{
		draw_slot.bind(this);
		window_.set(0, 0);
		subwindow_.set(0,0,1,1);
	}

	Camera* Camera::current_ = NULL;	///< カレントカメラ（そのシーンのupdate、draw…などの間だけ有効）

	void Camera::newNode(Stage &stage)
	{
		Pointer<StrutumNode> node = StrutumNode::create();
		node_ = node;
		stage.strutum_tree.root()->push(node);
	}

	void Camera::activate(bool yes)
	{
		if(yes) app().draw_signal.connectOnce(draw_slot);
		else app().draw_signal.disconnect(draw_slot);
	}

	void Camera::setToSystem() const
	{
		graphic::setView(view());
		graphic::setProjection(projection());
	}

	bool Camera::begin() const
	{
		backup_current_ = current_;
		current_ = const_cast<Camera *>(this);
		current_->update();
		setToSystem();
		return true;
	}

	bool Camera::end() const
	{
		current_ = backup_current_;
		if(current_) current_->setToSystem();
		return true;
	}

	bool Camera::onDraw(float delta) const
	{
		begin();
		const_cast<Camera *>(this)->draw_signal(delta);
		end();
		return true;
	}

	Matrix Camera::view() const
	{
		return Matrix().setView(node_->val.wtm().right(), node_->val.wtm().up(), node_->val.wtm().at(), node_->val.wtm().position());
	}

	Matrix Camera::projection() const
	{
		Matrix ret;
		Size2f size = screen();
		if(isPerspective()) ret.setFOV(fov_, size.x/size.y, subwindow_.left, subwindow_.top, subwindow_.right, subwindow_.bottom, nearclip_, farclip_);
		else ret.setOrtho(-size.x/2*(2*subwindow_.left-1), -size.y/2*(2*subwindow_.top-1), size.x/2*(2*subwindow_.right-1), size.y/2*(2*subwindow_.bottom-1), nearclip_, farclip_);
		return ret;
	}

	Size2f Camera::screen() const
	{
		if(doFitToScreen()) return Vector2C(graphic::getViewPort().size());
		else return window_;
	}

	void Camera::update()
	{
		node_->val.wtm().orthoNormalize();
		frustum_.set(view()*projection());
	}
	
	bool Camera::setUp(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<Stage> stage = tuki_cast<Stage>(L[1]);
			if(stage) newNode(*stage);
		}
		return true;
	}

	void Camera::activate(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			activate(L[1].toBoolean());
		}
	}

	GCTP_IMPLEMENT_CLASS_NS(gctp, Camera, Object);
	TUKI_IMPLEMENT_BEGIN_NS(Scene, Camera)
		TUKI_METHOD(Camera, activate)
	TUKI_IMPLEMENT_END(Camera)

}} // namespace gctp::scene
