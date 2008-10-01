/** @file
 * GameCatapult カメラシーンノードクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/camera.hpp>
#include <gctp/scene/world.hpp>
#include <gctp/graphic.hpp>
#include <gctp/aabox.hpp>
#include <gctp/app.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/graphic/dx/device.hpp>

using namespace std;

namespace gctp { namespace scene {

	Camera::Camera() : nearclip_(1.0f), farclip_(100.0f), fov_(g_pi/4), fog_enable_(false)
	{
		window_.set(0, 0);
		subwindow_.set(0,0,1,1);
	}

	Camera* Camera::current_ = NULL;	///< カレントカメラ（そのシーンのupdate、draw…などの間だけ有効）

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
		graphic::clear(false, true);
		if(fog_enable_) {
			// Enable fog blending.
			graphic::device().impl()->SetRenderState(D3DRS_FOGENABLE, TRUE);
			// Set the fog color.
			graphic::device().impl()->SetRenderState(D3DRS_FOGCOLOR, fog_color_.i32);
			// Set fog parameters.
			graphic::device().impl()->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
			graphic::device().impl()->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&fog_start_));
			graphic::device().impl()->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&fog_end_));
		}
	}

	void Camera::end() const
	{
		if(fog_enable_) {
			// Disable fog blending.
			graphic::device().impl()->SetRenderState(D3DRS_FOGENABLE, FALSE);
		}
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
		if(node()) m = node()->val.wtm().orthoNormal();
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
	}
	
	bool Camera::setUp(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
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

	void Camera::setFogColor(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			if(L.top() >= 4) {
				fog_color_ = Color32(L[1].toInteger(), L[2].toInteger(), L[3].toInteger(), L[4].toInteger());
			}
			else if(L.top() >= 3) {
				fog_color_ = Color32(L[1].toInteger(), L[2].toInteger(), L[3].toInteger());
			}
			else {
				fog_color_ = Color32(L[1].toCStr());
			}
		}
	}

	void Camera::setFogParam(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			fog_enable_ = true;
			fog_start_ = (float)L[1].toNumber();
			fog_end_ = (float)L[2].toNumber();
		}
		else {
			fog_enable_ = false;
		}
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, Camera, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, Camera)
		GCTP_SCENE_ASPECTSTRUTUMNODE_TUKI_METHODS(Camera)
		TUKI_METHOD(Camera, setFov)
		TUKI_METHOD(Camera, getFov)
		TUKI_METHOD(Camera, setClip)
		TUKI_METHOD(Camera, getClip)
		TUKI_METHOD(Camera, getDirection)
		TUKI_METHOD(Camera, setFogColor)
		TUKI_METHOD(Camera, setFogParam)
	TUKI_IMPLEMENT_END(Camera)

}} // namespace gctp::scene
