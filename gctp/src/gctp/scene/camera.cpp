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

	Camera::Camera() : nearclip_(1.0f), farclip_(100.0f), fov_(g_pi/4), aspect_ratio_(0), fog_enable_(false)
	{
		render_rect_.set(0,0,1,1);
		view_port_.max_z = -1;
		view_port_.min_z = 0;
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
		view_port_bak_ = graphic::getViewPort();
		backup_current_ = current_;
		current_ = const_cast<Camera *>(this);
		current_->update();
		if(view_port_.max_z >= view_port_.min_z) graphic::setViewPort(view_port_);
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
		graphic::setViewPort(view_port_bak_);
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
		if(isPerspective()) ret.setFOV(fov_, aspectRatio(), render_rect_.left, render_rect_.top, render_rect_.right, render_rect_.bottom, nearclip_, farclip_);
		else {
			float height = (float)graphic::getViewPort().size().y;
			float aspect_ratio = aspectRatio();
			ret.setOrtho(-(height*aspect_ratio)/2*(2*render_rect_.left-1), -height/2*(2*render_rect_.top-1), (height*aspect_ratio)/2*(2*render_rect_.right-1), height/2*(2*render_rect_.bottom-1), nearclip_, farclip_);
		}
		return ret;
	}

	float Camera::aspectRatio() const
	{
		if(aspect_ratio_ == 0) {
			Vector2 sz = Vector2C(graphic::getViewPort().size());
			return sz.x/sz.y;
		}
		return aspect_ratio_;
	}

	void Camera::update()
	{
		frustum_.set(view()*projection());
	}
	
	bool Camera::LuaCtor(luapp::Stack &L)
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

	void Camera::setAspectRatio(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			aspect_ratio_ = (float)L[1].toNumber();
		}
		else {
			aspect_ratio_ = 0;
		}
	}

	int Camera::getAspectRatio(luapp::Stack &L)
	{
		L << aspectRatio();
		return 1;
	}

	void Camera::setRenderRect(luapp::Stack &L)
	{
		if(L.top() >= 4) {
			render_rect_.left = (float)L[1].toNumber();
			render_rect_.top = (float)L[2].toNumber();
			render_rect_.right = (float)L[3].toNumber();
			render_rect_.bottom = (float)L[4].toNumber();
		}
	}

	int Camera::getRenderRect(luapp::Stack &L)
	{
		L << render_rect_.left << render_rect_.top << render_rect_.right << render_rect_.bottom;
		return 4;
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

	void Camera::setViewPort(luapp::Stack &L)
	{
		if(L.top() >= 6) {
			view_port_.x = (uint)L[1].toNumber();
			view_port_.y = (uint)L[2].toNumber();
			view_port_.width = (uint)L[3].toNumber();
			view_port_.height = (uint)L[4].toNumber();
			view_port_.min_z = (float)L[5].toNumber();
			view_port_.max_z = (float)L[6].toNumber();
		}
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, Camera, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, Camera)
		GCTP_SCENE_ASPECTSTRUTUMNODE_TUKI_METHODS(Camera)
		TUKI_METHOD(Camera, setFov)
		TUKI_METHOD(Camera, getFov)
		TUKI_METHOD(Camera, setAspectRatio)
		TUKI_METHOD(Camera, getAspectRatio)
		TUKI_METHOD(Camera, setRenderRect)
		TUKI_METHOD(Camera, getRenderRect)
		TUKI_METHOD(Camera, setClip)
		TUKI_METHOD(Camera, getClip)
		TUKI_METHOD(Camera, getDirection)
		TUKI_METHOD(Camera, setFogColor)
		TUKI_METHOD(Camera, setFogParam)
		TUKI_METHOD(Camera, setViewPort)
	TUKI_IMPLEMENT_END(Camera)

}} // namespace gctp::scene
