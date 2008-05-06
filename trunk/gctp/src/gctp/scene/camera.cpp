/** @file
 * GameCatapult カメラシーンノードクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/camera.hpp>
#include <gctp/graphic.hpp>
#include <gctp/aabox.hpp>
#include <gctp/app.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	Camera::Camera() : nearclip_(1.0f), farclip_(100.0f), fov_(g_pi/4)
	{
		draw_slot.bind(this);
		window_.set(0, 0);
		subwindow_.set(0,0,1,1);
		stance_.setDefault();
	}

	Camera* Camera::current_ = NULL;	///< カレントカメラ（そのシーンのupdate、draw…などの間だけ有効）

	void Camera::setStance(Stance &src)
	{
		stance_ = src;
		if(node_) node_->val.getLCM() = stance_.toMatrix();
	}

	void Camera::attach(Handle<StrutumNode> node)
	{
		node_ = node;
		if(node_) stance_ = node_->val.lcm().orthoNormal();
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
		Matrix m;
		if(node_) m = node_->val.wtm().orthoNormal();
		else m = stance_.toMatrix();
		return Matrix().setView(m.right(), m.up(), m.at(), m.position());
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
		frustum_.set(view()*projection());
		if(node_) stance_ = node_->val.lcm().orthoNormal();
	}
	
	bool Camera::setUp(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void Camera::activate(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			activate(L[1].toBoolean());
		}
	}

	void Camera::setPos(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			Stance newstance = stance();
			newstance.position.x = (float)L[1].toNumber();
			newstance.position.y = (float)L[2].toNumber();
			newstance.position.z = (float)L[3].toNumber();
			setStance(newstance);
		}
	}

	int Camera::getPos(luapp::Stack &L)
	{
		Stance nowstance = stance();
		L << nowstance.position.x << nowstance.position.y << nowstance.position.z;
		return 3;
	}

	void Camera::setRot(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			Stance newstance = stance();
			newstance.posture = QuatC((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber());
			setStance(newstance);
		}
	}

	int Camera::getRot(luapp::Stack &L)
	{
		Quat q = stance().posture;
		L << q.yaw() << q.pitch() << q.roll();
		return 3;
	}

	GCTP_IMPLEMENT_CLASS_NS(gctp, Camera, Object);
	TUKI_IMPLEMENT_BEGIN_NS(Scene, Camera)
		TUKI_METHOD(Camera, activate)
		TUKI_METHOD(Camera, setPos)
		TUKI_METHOD(Camera, getPos)
		TUKI_METHOD(Camera, setRot)
		TUKI_METHOD(Camera, getRot)
	TUKI_IMPLEMENT_END(Camera)

}} // namespace gctp::scene
