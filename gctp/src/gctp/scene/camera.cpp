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
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	Camera::Camera() : nearclip_(0.1f), farclip_(1000.0f), fov_(g_pi/4)
	{
		window_.set(0, 0);
		subwindow_.set(0,0,1,1);
	}

	Camera* Camera::current_ = NULL;	///< カレントカメラ（そのシーンのupdate、draw…などの間だけ有効）

	void Camera::setUp()
	{
		node_ = StrutumNode::create();
		node_->val.getLCM().identify();
	}

	void Camera::enter(Stage &stage)
	{
		stage.strutum_tree.root()->push(node_);
		Pointer<RenderingNode> rendering_node = RenderingNode::create(this);
		rendering_node->push(Pointer<Renderer>(new StageRenderer(&stage)));
		stage.rendering_tree.root()->push(rendering_node);
	}
	
	void Camera::exit(Stage &stage)
	{
		stage.strutum_tree.root()->erase(node_.get());
		RenderingNode::Itr it = stage.rendering_tree.root()->find(this);
		stage.rendering_tree.root()->erase(it);
	}

	void Camera::setToSystem() const
	{
		graphic::setView(view());
		graphic::setProjection(projection());
	}

	bool Camera::onEnter() const
	{
		backup_current_ = current_;
		current_ = const_cast<Camera *>(this);
		current_->update();
		setToSystem();
		return true;
	}

	bool Camera::onLeave() const
	{
		current_ = backup_current_;
		if(current_) current_->setToSystem();
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
		if(L.top()==0) setUp();
		return true;
	}

	void Camera::enter(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			Pointer<Stage> stage = tuki_cast<Stage>(L[2]);
			if(stage) enter(*stage);
		}
	}

	void Camera::exit(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			Pointer<Stage> stage = tuki_cast<Stage>(L[2]);
			if(stage) exit(*stage);
		}
	}

	GCTP_IMPLEMENT_CLASS_NS(gctp, Camera, Object);
	TUKI_IMPLEMENT_BEGIN_NS(Scene, Camera)
		TUKI_METHOD(Camera, enter)
		TUKI_METHOD(Camera, exit)
	TUKI_IMPLEMENT_END(Camera)

}} // namespace gctp::scene
