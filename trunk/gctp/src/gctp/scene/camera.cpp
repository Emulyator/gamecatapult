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

	void Camera::setToSystem() const
	{
		graphic::setView(view());
		graphic::setProjection(projection());
		viewprojection_ = view()*projection();
	}

	void Camera::begin() const
	{
		backup_current_ = current_;
		current_ = const_cast<Camera *>(this);
		current_->update();
		setToSystem();
	}

	void Camera::end() const
	{
		current_ = backup_current_;
		if(current_) current_->setToSystem();
	}

	bool Camera::onReach(float delta) const
	{
		begin();
		return true;
	}

	bool Camera::onLeave(float delta) const
	{
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
		if(isFittedToScreen()) return Vector2C(graphic::getViewPort().size());
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

	void Camera::setPosition(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			Stance newstance = stance();
			newstance.position.x = (float)L[1].toNumber();
			newstance.position.y = (float)L[2].toNumber();
			newstance.position.z = (float)L[3].toNumber();
			setStance(newstance);
		}
	}

	int Camera::getPosition(luapp::Stack &L)
	{
		Stance nowstance = stance();
		L << nowstance.position.x << nowstance.position.y << nowstance.position.z;
		return 3;
	}

	void Camera::setPosture(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			Stance newstance = stance();
			newstance.posture = QuatC((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber());
			setStance(newstance);
		}
	}

	int Camera::getPosture(luapp::Stack &L)
	{
		Quat q = stance().posture;
		L << q.yaw() << q.pitch() << q.roll();
		return 3;
	}

	void Camera::setDirection(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			Stance newstance = stance();
			VectorC dir((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber());
			math::Matrix3x3<float> mat = newstance.posture.toMatrix3x3();
			Vector c = mat.up3d()%dir;
			if(c.length2() > FLT_EPSILON*10) {
				newstance.posture.set(c, mat.up3d(), dir);
			}
			else {
				c = mat.right3d()%dir;
				newstance.posture.set(-mat.right3d(), c, dir);
			}
			setStance(newstance);
		}
	}

	int Camera::getDirection(luapp::Stack &L)
	{
		Vector at = stance().posture.toMatrix().at();
		L << at.x << at.y << at.z;
		return 3;
	}

	void Camera::setClip(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			nearclip_ = (float)L[1].toNumber();
			farclip_ = (float)L[2].toNumber();
		}
	}

	int Camera::getClip(luapp::Stack &L)
	{
		L << nearclip_ << farclip_;
		return 2;
	}

	void Camera::setFov(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			fov_ = (float)L[1].toNumber();
		}
	}

	int Camera::getFov(luapp::Stack &L)
	{
		L << fov_;
		return 1;
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, Camera, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, Camera)
		TUKI_METHOD(Camera, setPosition)
		TUKI_METHOD(Camera, getPosition)
		TUKI_METHOD(Camera, setPosture)
		TUKI_METHOD(Camera, getPosture)
		TUKI_METHOD(Camera, setDirection)
		TUKI_METHOD(Camera, getDirection)
		TUKI_METHOD(Camera, setFov)
		TUKI_METHOD(Camera, getFov)
		TUKI_METHOD(Camera, setClip)
		TUKI_METHOD(Camera, getClip)
	TUKI_IMPLEMENT_END(Camera)

}} // namespace gctp::scene
