/** @file
 * GameCatapult DirectX Graphicクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: graphic.cpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {

	/// 実装詳細
	class DeviceImpl : public Object, public dx::Device {
	public:
		DB db_;
	};

	namespace {
		typedef map<uint, Handle<DeviceImpl> > DeviceImplList;
		static DeviceImplList devicelist;
	}

	Device* Device::current_ = NULL;	///< カレントデバイスインスタンス

	void initialize(bool with_WHQL)
	{
		dx::initialize(with_WHQL);
	}

	/** すでに製作されたデバイスがあったらそれを使用。
	 * そうでなければ新規に作る
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/26 18:24:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Device::preSetUp(uint adpt, HWND hwnd)
	{
		if(!impl_) {
			if(!is_unique_ && devicelist[adpt].get()) {
				impl_ = devicelist[adpt].get();
				view_ = impl_->createView(adpt, hwnd);
				if(!view_) {
					impl_ = 0;
					PRNN(_T("追加チェーン製作失敗"));
					return E_FAIL;
				}
			}
			else {
				DeviceImpl *impl = new DeviceImpl;
				if(impl) {
					impl_ = impl;
					devicelist[adpt] = impl_;
				}
				else return E_OUTOFMEMORY;
			}
			return S_OK;
		}
		return S_FALSE;
	}

	Device::Device(bool is_unique) : is_unique_(is_unique)
	{
	}

	HRslt Device::open(HWND h_focus_wnd)
	{
		HRslt hr = preSetUp(0, h_focus_wnd);
		if(!hr) return hr;
		if(view_) return S_OK;
		return impl_->open(h_focus_wnd);
	}

	HRslt Device::open(HWND h_focus_wnd, HWND h_device_wnd)
	{
		HRslt hr = preSetUp(0, h_focus_wnd);
		if(!hr) return hr;
		if(view_) return S_OK;
		return impl_->open(h_focus_wnd, h_device_wnd);
	}

	HRslt Device::open(HWND h_focus_wnd, HWND h_device_wnd, uint adpt, uint mode)
	{
		HRslt hr = preSetUp(adpt, h_focus_wnd);
		if(!hr) return hr;
		if(view_) return S_OK;
		return impl_->open(h_focus_wnd, h_device_wnd, adpt, mode);
	}

	HRslt Device::open(HWND h_focus_wnd, uint adpt, uint mode)
	{
		HRslt hr = preSetUp(adpt, h_focus_wnd);
		if(!hr) return hr;
		if(view_) return S_OK;
		return impl_->open(h_focus_wnd, adpt, mode);
	}

	HRslt Device::open(HWND h_focus_wnd, HWND h_first_wnd, HWND h_second_wnd, uint adpt, uint mode)
	{
		HRslt hr = preSetUp(adpt, h_focus_wnd);
		if(!hr) return hr;
		if(view_) return S_OK;
		return impl_->open(h_focus_wnd, h_first_wnd, h_second_wnd, adpt, mode);
	}

	HRslt Device::open(uint adpt, uint idx)
	{
		if(devicelist[adpt].lock()) {
			impl_ = devicelist[adpt].lock();
			view_ = impl_->createView(adpt, idx);
			if(view_) return S_OK;
			else {
				impl_ = 0;
				PRNN(_T("セカンダリチェーン取得失敗"));
			}
		}
		return E_FAIL;
	}

	HRslt Device::open(IDirect3DDevice9 *device)
	{
		DeviceImpl *impl = new DeviceImpl;
		if(impl) {
			HRslt hr = impl->open(device);
			impl_ = 0;
			return hr;
		}
		return E_OUTOFMEMORY;
	}

	void Device::close()
	{
		view_ = 0;
		impl_ = 0;
	}

	dx::Device &Device::impl()
	{
		return *impl_;
	}

	const dx::Device &Device::impl() const
	{
		return *impl_;
	}

	bool Device::isOpen() const
	{
		return this && impl_ && impl_->ptr();
	}

	void Device::registerRsrc(Handle<Rsrc> rsrc)
	{
		impl_->registerRsrc(rsrc);
	}

	DB &Device::db()
	{
		return impl_->db_;
	}

	HRslt Device::setCurrent() const
	{
		current_ = const_cast<Device *>(this);
		if(view_) return view_->setCurrent();
		if(impl_) return impl_->setCurrent();
		return E_FAIL;
	}

	/// 描画開始
	HRslt Device::begin()
	{
		return impl_->begin();
	}

	/// 描画終了
	HRslt Device::end()
	{
		return impl_->end();
	}

	/// 描画反映
	HRslt Device::present()
	{
		if(view_) return view_->present();
		return impl_->present();
	}

	/// 画面のクリア
	HRslt Device::clear(bool target, bool zbuffer)
	{
		return impl_->clear((target ? D3DCLEAR_TARGET : 0)|(zbuffer ? D3DCLEAR_ZBUFFER : 0));
	}

	void Device::setCursorVisible(bool yes)
	{
		impl_->setCursorVisible(yes);
	}

	void Device::setClearColor(const Color &cc)
	{
		impl_->setClearColor(cc);
	}

	void Device::setWorld(const Matrix &mat)
	{
		impl_->setWorld(mat);
	}

	Matrix Device::getWorld()
	{
		return impl_->getWorld();
	}

	void Device::setProjection(const Matrix &mat)
	{
		impl_->setProjection(mat);
	}

	Matrix Device::getProjection()
	{
		return impl_->getProjection();
	}

	void Device::setView(const Matrix &mat)
	{
		impl_->setView(mat);
	}

	Matrix Device::getView()
	{
		return impl_->getView();
	}

	HRslt Device::setAmbient(const Color &col)
	{
		return impl_->setAmbient(col);
	}

	const Color &Device::getAmbient() const
	{
		return impl_->getAmbient();
	}

	HRslt Device::pushLight(const DirectionalLight &lgt)
	{
		return impl_->pushLight(lgt);
	}

	HRslt Device::pushLight(const PointLight &lgt)
	{
		return impl_->pushLight(lgt);
	}

	HRslt Device::pushLight(const SpotLight &lgt)
	{
		return impl_->pushLight(lgt);
	}

	HRslt Device::popLight()
	{
		return impl_->popLight();
	}

	void Device::clearLight()
	{
		impl_->clearLight();
	}

	uint Device::lightNum() const
	{
		return impl_->lightNum();
	}

	HRslt Device::getLight(uint no, DirectionalLight &lgt) const
	{
		return impl_->getLight(no, lgt);
	}

	HRslt Device::getLight(uint no, PointLight &lgt) const
	{
		return impl_->getLight(no, lgt);
	}

	HRslt Device::getLight(uint no, SpotLight &lgt) const
	{
		return impl_->getLight(no, lgt);
	}

	void Device::setMaterial(const Material &mtrl)
	{
		impl_->setMaterial(mtrl);
	}

	/// デバイス消失時のリソース解放
	void Device::cleanUp()
	{
		impl_->cleanUp();
	}

	/// デバイス消失時のリソース再構築
	void Device::restore()
	{
		impl_->restore();
	}

	/// スクリーンサイズを返す
	Point2 Device::getScreenSize() const
	{
		return impl_->getScreenSize();
	}
	
	/// ビューポートを取得
	ViewPort Device::getViewPort() const
	{
		D3DVIEWPORT9 vp = impl_->getViewPort();
//		Point2 ssize = getScreenSize();
//		return ViewPort(static_cast<float>(vp.X)/ssize.x, static_cast<float>(vp.Y)/ssize.y, static_cast<float>(vp.Width)/ssize.x, static_cast<float>(vp.Height)/ssize.y, vp.MinZ, vp.MaxZ);
		return ViewPort(vp.X, vp.Y, vp.Width, vp.Height, vp.MinZ, vp.MaxZ);
	}

	/// ビューポートを設定
	HRslt Device::setViewPort(const ViewPort &src)
	{
		D3DVIEWPORT9 vp;
//		Point2 ssize = getScreenSize();
//		vp.X = static_cast<DWORD>(ssize.x*src.x);
//		vp.Y = static_cast<DWORD>(ssize.y*src.y);
//		vp.Width = static_cast<DWORD>(ssize.x*src.width);
//		vp.Height = static_cast<DWORD>(ssize.y*src.height);
//		vp.MinZ = src.min_z;
//		vp.MaxZ = src.max_z;
		vp.X = src.x;
		vp.Y = src.y;
		vp.Width = src.width;
		vp.Height = src.height;
		vp.MinZ = src.min_z;
		vp.MaxZ = src.max_z;
		return impl_->setViewPort(vp);
	}

	/// ステートブロック記録開始
	HRslt Device::beginRecord()
	{
		return impl_->beginRecord();
	}

	/// ステートブロック記録終了
	Pointer<dx::StateBlock> Device::endRecord()
	{
		return impl_->endRecord();
	}

	/// 新規ステートブロック
	Pointer<dx::StateBlock> Device::createState(D3DSTATEBLOCKTYPE type)
	{
		return impl_->createState(type);
	}

//	/// ステート退避
//	HRslt Device::pushState()
//	{
//		return impl_->pushState();
//	}

//	/// ステート復帰
//	HRslt Device::popState()
//	{
//		return impl_->popState();
//	}

	void allowFSAA(FSAAType type, uint level)
	{
		dx::Device::allowFSAA((dx::Device::FSAAType)type, level);
	}

	void allowStrictMultiThreadSafe(bool yes)
	{
		dx::Device::allowStrictMultiThreadSafe(yes);
	}
	
	void allowAutoDepthBuffer(bool yes)
	{
		dx::Device::allowAutoDepthBuffer(yes);
	}
	
	void allowReferenceDriver(bool yes)
	{
		dx::Device::allowReferenceDriver(yes);
	}
	
	void allowHardwareVertexProcessing(bool yes)
	{
		dx::Device::allowHardwareVertexProcessing(yes);
	}
	
	void allowMixedVertexProcessing(bool yes)
	{
		dx::Device::allowMixedVertexProcessing(yes);
	}

	void setIntervalTime(int time)
	{
		dx::Device::setIntervalTime(time);
	}

	void setSwapMode(bool copy)
	{
		dx::Device::setSwapMode(copy);
	}

}} // namespace gctp
