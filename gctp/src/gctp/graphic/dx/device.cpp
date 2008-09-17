/** @file
 * GameCatapult DirectX Deviceクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: device.cpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/dx/stateblock.hpp>
#include <gctp/graphic/light.hpp>
#include <gctp/graphic/material.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/dbgout.hpp>
#include <boost/scoped_array.hpp>

using namespace std;

namespace gctp { namespace graphic { namespace dx {

	IDirect3DPtr Adapter::api_;

	void initialize(bool with_WHQL)
	{
		Adapter::initialize(with_WHQL);
	}

	void Adapter::initialize(bool with_WHQL)
	{
		if(!api_) api_.attach(Direct3DCreate9(D3D_SDK_VERSION));
		if(Adapter::adapters_.size() > 0) Adapter::adapters_.clear();
		int num = api_->GetAdapterCount();
		for(int i = 0; i < num; i++) {
			Adapter::adapters_.push_back(Adapter());
			Adapter::adapters_.back().set(i, with_WHQL);
		}
	}

	void Adapter::set(UINT adpt, bool with_WHQL)
	{
#define __ADDMODE(fmt)	{\
	uint num = api_->GetAdapterModeCount(adpt, fmt);\
	for(uint i = 0, width = 0, height = 0; i < num; i++) {\
		D3DDISPLAYMODE mode;\
		api_->EnumAdapterModes(adpt, fmt, i, &mode);\
		if(width != mode.Width || height != mode.Height) {\
			D3DDISPLAYMODE default_mode = mode;\
			default_mode.RefreshRate = D3DPRESENT_RATE_DEFAULT;\
			modes.push_back(default_mode);\
		}\
		width = mode.Width; height = mode.Height;\
		modes.push_back(mode);\
	}\
}
		api_->GetAdapterIdentifier(adpt, (with_WHQL)?D3DENUM_WHQL_LEVEL:0, this);
		__ADDMODE(D3DFMT_R5G6B5);
		__ADDMODE(D3DFMT_X1R5G5B5);
		__ADDMODE(D3DFMT_A1R5G5B5);
		__ADDMODE(D3DFMT_X8R8G8B8);
		__ADDMODE(D3DFMT_A8R8G8B8);
		__ADDMODE(D3DFMT_A2R10G10B10);
#undef __ADDMODE
	}

	AdapterList Adapter::adapters_;

	// 設定フラグ
	uint Device::fsaa_level_;
	Device::FSAAType Device::fsaa_ = Device::FSAA_NONE;
	bool Device::is_threaded_ = false;
	bool Device::is_autodepthbuffer_ = true;
	bool Device::is_allow_REF_ = false;
	bool Device::is_allow_HVP_ = false;
	bool Device::is_allow_MVP_ = true;
	bool Device::copy_when_swap_ = false;
	int Device::interval_time_ = 0;

	void Device::allowFSAA(FSAAType type, uint level)
	{
		fsaa_ = type;
		fsaa_level_ = level;
	}

	void Device::allowStrictMultiThreadSafe(bool yes)
	{
		is_threaded_ = yes;
	}

	void Device::allowAutoDepthBuffer(bool yes)
	{
		is_autodepthbuffer_ = yes;
	}

	void Device::allowReferenceDriver(bool yes)
	{
		is_allow_REF_ = yes;
	}

	void Device::allowHardwareVertexProcessing(bool yes)
	{
		is_allow_HVP_ = yes;
	}

	void Device::allowMixedVertexProcessing(bool yes)
	{
		is_allow_MVP_ = yes;
	}

	void Device::setSwapMode(bool copy)
	{
		copy_when_swap_ = copy;
	}

	void Device::setIntervalTime(int time)
	{
		interval_time_ = time;
	}

	Point2 Device::getScreenSize() const {
		IDirect3DSurfacePtr sp;
		D3DSURFACE_DESC desc;
		ptr_->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &sp);
		if(sp) {
			sp->GetDesc(&desc);
			return Point2C(desc.Width, desc.Height);
		}
		return Point2C(0, 0);
	}

	D3DVIEWPORT Device::getViewPort() const {
		D3DVIEWPORT ret;
		ptr_->GetViewport(&ret);
		return ret;
	}

	HRslt Device::setViewPort(const D3DVIEWPORT9 &vp) {
		return ptr_->SetViewport(&vp);
	}

	void Device::setCursorVisible(bool yes)
	{
		::ShowCursor(yes);
		ptr_->ShowCursor(yes);
	}
	
	void Device::setCursorPosition(const Point2 &pos)
	{
		ptr_->SetCursorPosition(pos.x, pos.y, 0);
	}

	Device::Device() : clear_color_(89/255.0f,135/255.0f,179/255.0f), cursor_backup_(NULL)
	{
	}

	Device::~Device()
	{
		cleanUp();
	}

	/** DirectDeviceデバイス製作
	 *
	 * すでに製作されていたらリセット
	 */
	HRslt Device::open(
		HWND h_focus_wnd,	///< フォーカスウィンドウ
		HWND h_device_wnd,	///< デバイスウィンドウ（表示の対象になるターゲット。フォーカスウィンドウと別な場合のみ指定)
		bool is_fs,			///< フルスクリーンか？
		uint adpt,			///< アダプタ番号
		uint mode,			///< 画面モード番号
		D3DDEVTYPE type		///< デバイスの種類
	) {
		if(!Adapter::api_ || adapters().size()==0) {
			PRNN(_T("システムが初期化されていません。graphic::initializeを呼び出してください。"));
			return CO_E_NOTINITIALIZED;
		}
		if(is_fs) {
			if(mode >= adapters()[adpt].modes.size()) return E_FAIL;
			PRNN(_T("フルスクリーンでデバイス製作"));
		}
		else PRNN(_T("ウィンドウモードでデバイス製作"));
		HRslt ret;
		D3DCAPS caps;
		ret = Adapter::api_->GetDeviceCaps(adpt, type, &caps);
		if(!ret) return ret;
		D3DPRESENT_PARAMETERS d3dpp;
		memset( &d3dpp, 0, sizeof(d3dpp) );
		DWORD additional_flag = is_threaded_ ? D3DCREATE_MULTITHREADED : 0;
		if(is_fs) {
			const D3DDISPLAYMODE &d3ddm = adapters()[adpt].modes[mode];
			d3dpp.BackBufferWidth	= d3ddm.Width;
			d3dpp.BackBufferHeight	= d3ddm.Height;
			d3dpp.BackBufferFormat	= d3ddm.Format;
			d3dpp.FullScreen_RefreshRateInHz = d3ddm.RefreshRate;
			d3dpp.hDeviceWindow = h_device_wnd;
			d3dpp.SwapEffect = copy_when_swap_ ? D3DSWAPEFFECT_COPY : D3DSWAPEFFECT_DISCARD;
			switch(interval_time_) {
			default:
			case 0: d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; break;
			case 1: d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT; break;
			case 2: d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; break;
			case 3: d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_TWO; break;
			case 4: d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_THREE; break;
			case 5: d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_FOUR; break;
			}
			d3dpp.EnableAutoDepthStencil = (is_autodepthbuffer_)?TRUE:FALSE;
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		}
		else {
			d3dpp.Windowed = TRUE;
			d3dpp.hDeviceWindow = h_device_wnd;
			d3dpp.SwapEffect = copy_when_swap_ ? D3DSWAPEFFECT_COPY : D3DSWAPEFFECT_DISCARD;
			if(interval_time_==0) d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			else d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
			d3dpp.EnableAutoDepthStencil = (is_autodepthbuffer_)?TRUE:FALSE;
			d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		}
		if(fsaa_ != FSAA_NONE) {
			D3DMULTISAMPLE_TYPE mstype;
			switch(fsaa_) {
			case FSAA_MSx2: mstype = D3DMULTISAMPLE_2_SAMPLES; break;
			case FSAA_MSx3: mstype = D3DMULTISAMPLE_3_SAMPLES; break;
			case FSAA_MSx4: mstype = D3DMULTISAMPLE_4_SAMPLES; break;
			}
			DWORD qlv_num;
			int _mstype;
			for(_mstype = mstype; _mstype>0; _mstype--) {
				ret = Adapter::api_->CheckDeviceMultiSampleType(adpt, type, d3dpp.BackBufferFormat, d3dpp.Windowed, (D3DMULTISAMPLE_TYPE)_mstype, &qlv_num);
				if(ret) {
					d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)_mstype;
					d3dpp.MultiSampleQuality = (min)((DWORD)fsaa_level_ , (DWORD)(qlv_num-1));
					break;
				}
			}
			if(ret) {
				if(mstype != _mstype) PRNN(_T("マルチサンプリングが受理されましたが、要求のサンプル数より落としました。"));
				if(fsaa_level_ != d3dpp.MultiSampleQuality) PRNN(_T("マルチサンプリングが受理されましたが、要求のクォリティーより落としました。"));
				PRNN(_T("マルチサンプリング:")<<d3dpp.MultiSampleType<<","<<d3dpp.MultiSampleQuality);
			}
			else PRNN(_T("マルチサンプリングが要求されましたが、デバイスが対応していないため、マルチサンプリングなしで製作します。"));
		}
		if(ptr_) {
			ret = ptr_->Reset(&d3dpp); PRNN("Reset!");
			if( !ret ) GCTP_TRACE(ret);
		}
		else {
			if(is_allow_HVP_) ret = Adapter::api_->CreateDevice( adpt, type, h_focus_wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING|additional_flag, &d3dpp, &ptr_ );
			else ret = E_FAIL;
			if(!ret) {
				if(is_allow_HVP_) PRNN(_T("ハードウェア頂点処理での製作失敗:")<<ret);
				if(is_allow_MVP_) ret = Adapter::api_->CreateDevice( adpt, type, h_focus_wnd, D3DCREATE_MIXED_VERTEXPROCESSING|additional_flag, &d3dpp, &ptr_ );
				if(!ret) {
					if(is_allow_MVP_) PRNN(_T("ハードウェア・ソフトウェア混合頂点処理での製作失敗:")<<ret);
					ret = Adapter::api_->CreateDevice( adpt, type, h_focus_wnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING|additional_flag, &d3dpp, &ptr_ );
					if(!ret) PRNN(_T("ソフトウェア頂点処理での製作失敗:")<<ret);
				}
			}
			if(ret) PRNN(_T("グラフィックデバイス製作成功"));
			light_num_ = 0;
		}
		{
			D3DDISPLAYMODE dm;
			ptr_->GetDisplayMode(0, &dm);
			PRNN(_T("現在のディスプレイモード : Format = ")<<dm.Format);
		}
		return ret;
	}

	/** マルチヘッドでデバイス製作
	 *
	 * マルチヘッドはフルスクリーンのみ
	 *
	 * ウィンドウモードでやるなら、普通にウィンドウを二枚出す
	 *
	 * すでに製作されていたらリセット
	 */
	HRslt Device::open(
		HWND h_focus_wnd,		///< フォーカスウィンドウ（マルチヘッドのプライマリ）
		HWND h_first_wnd,		///< マルチヘッドのプライマリデバイスウィンドウ（通常h_focus_wndと一緒でいい）
		HWND h_second_wnd,	///< デバイスウィンドウ（マルチヘッドのセカンダリ）
		uint adpt,			///< アダプタ番号
		uint mode,			///< 画面モード番号
		D3DDEVTYPE type		///< デバイスの種類
	) {
		if(!Adapter::api_ || adapters().size()==0) {
			PRNN(_T("システムが初期化されていません。graphic::initializeを呼び出してください。"));
			return CO_E_NOTINITIALIZED;
		}
		if(mode >= adapters()[adpt].modes.size()) return E_FAIL;
		PRNN(_T("フルスクリーンでデバイス製作"));
		HRslt ret;
		D3DCAPS9 caps;
		ret = Adapter::api_->GetDeviceCaps(adpt, type, &caps);
		if(!ret) return ret;
		boost::scoped_array<D3DPRESENT_PARAMETERS> d3dpp(new D3DPRESENT_PARAMETERS[caps.NumberOfAdaptersInGroup]);
		ZeroMemory( d3dpp.get(), sizeof(D3DPRESENT_PARAMETERS)*caps.NumberOfAdaptersInGroup );
		DWORD additional_flag = is_threaded_ ? D3DCREATE_MULTITHREADED : 0;
		std::size_t n = 1;
		if(caps.NumberOfAdaptersInGroup > 1) {
			additional_flag |= D3DCREATE_ADAPTERGROUP_DEVICE;
			n = caps.NumberOfAdaptersInGroup;
		}
		else {
			PRNN(_T("マルチヘッドが要求されましたが、マルチヘッドアダプターでありません。"));
			return E_FAIL;
		}
		const D3DDISPLAYMODE &d3ddm = adapters()[adpt].modes[mode];
		for(std::size_t i = 0; i < n; i++) {
			d3dpp[i].BackBufferWidth	= d3ddm.Width;
			d3dpp[i].BackBufferHeight	= d3ddm.Height;
			d3dpp[i].BackBufferFormat	= d3ddm.Format;
			d3dpp[i].FullScreen_RefreshRateInHz = d3ddm.RefreshRate;
			d3dpp[i].hDeviceWindow = (i==0 ? h_first_wnd : h_second_wnd);
			d3dpp[i].SwapEffect = copy_when_swap_ ? D3DSWAPEFFECT_COPY : D3DSWAPEFFECT_DISCARD;
			switch(interval_time_) {
			default:
			case 0: d3dpp[i].PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; break;
			case 1: d3dpp[i].PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT; break;
			case 2: d3dpp[i].PresentationInterval = D3DPRESENT_INTERVAL_ONE; break;
			case 3: d3dpp[i].PresentationInterval = D3DPRESENT_INTERVAL_TWO; break;
			case 4: d3dpp[i].PresentationInterval = D3DPRESENT_INTERVAL_THREE; break;
			case 5: d3dpp[i].PresentationInterval = D3DPRESENT_INTERVAL_FOUR; break;
			}
			d3dpp[i].EnableAutoDepthStencil = (is_autodepthbuffer_)?TRUE:FALSE;
			d3dpp[i].AutoDepthStencilFormat = D3DFMT_D16;
		}
		if(fsaa_ != FSAA_NONE) {
			D3DMULTISAMPLE_TYPE mstype;
			switch(fsaa_) {
			case FSAA_MSx2: mstype = D3DMULTISAMPLE_2_SAMPLES; break;
			case FSAA_MSx3: mstype = D3DMULTISAMPLE_3_SAMPLES; break;
			case FSAA_MSx4: mstype = D3DMULTISAMPLE_4_SAMPLES; break;
			}
			DWORD qlv_num;
			int _mstype;
			for(_mstype = mstype; _mstype>0; _mstype--) {
				ret = Adapter::api_->CheckDeviceMultiSampleType(adpt, type, d3dpp[0].BackBufferFormat, d3dpp[0].Windowed, (D3DMULTISAMPLE_TYPE)_mstype, &qlv_num);
				if(ret) {
					d3dpp[0].MultiSampleType = (D3DMULTISAMPLE_TYPE)_mstype;
					d3dpp[0].MultiSampleQuality = (min)((DWORD)fsaa_level_ , (DWORD)(qlv_num-1));
					break;
				}
			}
			if(ret) {
				if(mstype != _mstype) PRNN(_T("マルチサンプリングが受理されましたが、要求のサンプル数より落としました。"));
				if(fsaa_level_ != d3dpp[0].MultiSampleQuality) PRNN(_T("マルチサンプリングが受理されましたが、要求のクォリティーより落としました。"));
				PRNN(_T("マルチサンプリング:")<<d3dpp[0].MultiSampleType<<","<<d3dpp[0].MultiSampleQuality);
			}
			else PRNN(_T("マルチサンプリングが要求されましたが、デバイスが対応していないため、マルチサンプリングなしで製作します。"));
		}
		if(ptr_) {
			ret = ptr_->Reset(d3dpp.get()); PRNN("Reset!");
			if( !ret ) PRNN(ret);
		}
		else {
			if(is_allow_HVP_) ret = Adapter::api_->CreateDevice( adpt, type, h_focus_wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING|additional_flag, d3dpp.get(), &ptr_ );
			else ret = E_FAIL;
			if(!ret) {
				if(is_allow_HVP_) PRNN(_T("ハードウェア頂点処理での製作失敗:")<<ret);
				if(is_allow_MVP_) ret = Adapter::api_->CreateDevice( adpt, type, h_focus_wnd, D3DCREATE_MIXED_VERTEXPROCESSING|additional_flag, d3dpp.get(), &ptr_ );
				if(!ret) {
					if(is_allow_MVP_) PRNN(_T("ハードウェア・ソフトウェア混合頂点処理での製作失敗:")<<ret);
					ret = Adapter::api_->CreateDevice( adpt, type, h_focus_wnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING|additional_flag, d3dpp.get(), &ptr_ );
					if(!ret) PRNN(_T("ソフトウェア頂点処理での製作失敗:")<<ret);
				}
			}
			if(ret) PRNN(_T("グラフィックデバイス製作成功"));
			light_num_ = 0;
		}
		return ret;
	}

	/** 既存のデバイスをgctp::graphic:Deviceとして使えるようにする
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/13 10:49:25
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Device::open(IDirect3DDevice9 *device)
	{
		if(!device) return E_POINTER;
		ptr_ = device;
		return S_OK;
	}

	void Device::close()
	{
		cleanUp();
		ptr_ = 0;
	}

	HRslt Device::setCurrent() const
	{
		if( !this || !ptr_ ) return E_FAIL;
		IDirect3DSurfacePtr surf;
		HRslt hr = ptr_->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &surf);
		if(!hr) return hr;
		return ptr_->SetRenderTarget(0, surf);
		return S_OK;
	}

	HRslt Device::present()
	{
		if(ptr_) return ptr_->Present(NULL, NULL, NULL, NULL);
		return E_FAIL;
	}

	void Device::setClearColor(const Color &cc)
	{
		clear_color_ = cc;
	}

	HRslt Device::clear(DWORD flag)
	{
		if(ptr_) return ptr_->Clear(0, NULL, flag, clear_color_, 1.0f, 0);
		return E_FAIL;
	}

	void Device::registerRsrc(Handle<Rsrc> rsrc)
	{
		if(this && rsrc) {
			for(RsrcList::iterator i = rsrcs_.begin(); i != rsrcs_.end();) {
				if((*i)) {
					if(*i == rsrc) return;
					++i;
				}
				else i = rsrcs_.erase(i);
			}
			rsrcs_.push_back(rsrc);
		}
	}

	void Device::cleanUp()
	{
		for(RsrcList::iterator i = rsrcs_.begin(); i != rsrcs_.end();) {
			if(*i) {
				//Pointer<Rsrc> p = (*i).lock();
				//p->cleanUp();
				(*i)->cleanUp();
				++i;
			}
			else i = rsrcs_.erase(i);
		}
	}

	void Device::restore()
	{
		for(RsrcList::iterator i = rsrcs_.begin(); i != rsrcs_.end();) {
			//Pointer<Rsrc> p = (*i).lock(); // 将来的にはこうするべきか…
			//if(p) {
			if(*i) {
				HRslt hr = (*i)->restore();
				if(!hr) GCTP_TRACE(hr);
				++i;
			}
			else i = rsrcs_.erase(i);
		}
	}

	HRslt Device::checkFormat(DWORD usage, D3DRESOURCETYPE rtype, D3DFORMAT format)
	{
		D3DDEVICE_CREATION_PARAMETERS parameters;
		ptr_->GetCreationParameters(&parameters);
		D3DDISPLAYMODE dm;
		ptr_->GetDisplayMode(0, &dm);
		return Adapter::api_->CheckDeviceFormat(parameters.AdapterOrdinal, parameters.DeviceType, dm.Format, usage, rtype, format);
	}

	uint Device::maxLightNum() const
	{
		D3DCAPS caps;
		if(ptr_ && SUCCEEDED(ptr_->GetDeviceCaps(&caps))) return caps.MaxActiveLights;
		return 0;
	}

	HRslt Device::setAmbientColor(Color32 col)
	{
		return ptr_->SetRenderState( D3DRS_AMBIENT, col );
	}

	Color32 Device::getAmbientColor() const
	{
		DWORD ret;
		ptr_->GetRenderState( D3DRS_AMBIENT, &ret );
		return Color32(ret);
	}

	HRslt Device::pushLight(const D3DLIGHT9 &lgt)
	{
		HRslt hr;
		if(light_num_ == 0) ptr_->SetRenderState( D3DRS_LIGHTING, TRUE );
		if(light_num_+1 < maxLightNum()) {
			if(hr = ptr_->SetLight(light_num_, &lgt)) {
				hr = ptr_->LightEnable(light_num_, TRUE);
				light_num_++;
			}
		}
		return hr;
	}

	HRslt Device::pushLight(const DirectionalLight &lgt)
	{
		D3DLIGHT light;
		ZeroMemory(&light, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Ambient = lgt.ambient;
		light.Diffuse = lgt.diffuse;
		light.Specular = lgt.specular;
		light.Direction = lgt.dir;
		return pushLight(light);
	}

	HRslt Device::pushLight(const PointLight &lgt)
	{
		D3DLIGHT light;
		ZeroMemory(&light, sizeof(light));
		light.Type = D3DLIGHT_POINT;
		light.Ambient = lgt.ambient;
		light.Diffuse = lgt.diffuse;
		light.Specular = lgt.specular;
		light.Position = lgt.pos;
		light.Range = lgt.range;
		light.Attenuation0 = lgt.attenuation[0];
		light.Attenuation1 = lgt.attenuation[1];
		light.Attenuation2 = lgt.attenuation[2];
		return pushLight(light);
	}

	HRslt Device::pushLight(const SpotLight &lgt)
	{
		D3DLIGHT light;
		ZeroMemory(&light, sizeof(light));
		light.Type = D3DLIGHT_SPOT;
		light.Ambient = lgt.ambient;
		light.Diffuse = lgt.diffuse;
		light.Specular = lgt.specular;
		light.Position = lgt.pos;
		light.Direction = lgt.dir;
		light.Range = lgt.range;
		light.Attenuation0 = lgt.attenuation[0];
		light.Attenuation1 = lgt.attenuation[1];
		light.Attenuation2 = lgt.attenuation[2];
		light.Falloff = lgt.falloff;
		light.Theta = lgt.theta;
		light.Phi = lgt.phi;
		return pushLight(light);
	}

	HRslt Device::popLight()
	{
		if(light_num_ > 0) return ptr_->LightEnable(--light_num_, FALSE);
		return E_FAIL;
	}

	void Device::clearLight()
	{
		while(popLight()) {}
		light_num_ = 0;
		ptr_->SetRenderState( D3DRS_LIGHTING, FALSE );
	}

	/** アクティブでないライト番号を指定した場合、D3DERR_INVALIDCALLが返る。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/10 1:00:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Device::getLight(uint no, D3DLIGHT9 &lgt) const
	{
		if(no < light_num_) return ptr_->GetLight(no, &lgt);
		return D3DERR_INVALIDCALL;
	}

	/** アクティブでないライト番号を指定した場合、E_INVALIDARGが返る。
	 *
	 * ディレクショナルでなかった場合、E_FAILが返る。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/10 1:00:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Device::getLight(uint no, DirectionalLight &lgt) const
	{
		HRslt hr;
		D3DLIGHT light;
		hr = getLight(no, light);
		if(hr) {
			if(light.Type == D3DLIGHT_DIRECTIONAL) {
				lgt.ambient = light.Ambient;
				lgt.diffuse = light.Diffuse;
				lgt.specular = light.Specular;
				lgt.dir = VectorC(light.Direction);
			}
			else return E_FAIL;
		}
		return hr;
	}

	/** アクティブでないライト番号を指定した場合、E_INVALIDARGが返る。
	 *
	 * 点光源でなかった場合、E_FAILが返る。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/10 1:00:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Device::getLight(uint no, PointLight &lgt) const
	{
		HRslt hr;
		D3DLIGHT light;
		hr = getLight(no, light);
		if(hr) {
			if(light.Type == D3DLIGHT_POINT) {
				lgt.ambient = light.Ambient;
				lgt.diffuse = light.Diffuse;
				lgt.specular = light.Specular;
				lgt.pos = VectorC(light.Position);
				lgt.range = light.Range;
				lgt.attenuation[0] = light.Attenuation0;
				lgt.attenuation[1] = light.Attenuation1;
				lgt.attenuation[2] = light.Attenuation2;
			}
			else return E_FAIL;
		}
		return hr;
	}

	/** アクティブでないライト番号を指定した場合、E_INVALIDARGが返る。
	 *
	 * スポットライトでなかった場合、E_FAILが返る。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/10 1:00:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Device::getLight(uint no, SpotLight &lgt) const
	{
		HRslt hr;
		D3DLIGHT light;
		hr = getLight(no, light);
		if(hr) {
			if(light.Type == D3DLIGHT_SPOT) {
				lgt.ambient = light.Ambient;
				lgt.diffuse = light.Diffuse;
				lgt.specular = light.Specular;
				lgt.pos = VectorC(light.Position);
				lgt.dir = VectorC(light.Direction);
				lgt.range = light.Range;
				lgt.attenuation[0] = light.Attenuation0;
				lgt.attenuation[1] = light.Attenuation1;
				lgt.attenuation[2] = light.Attenuation2;
				lgt.falloff = light.Falloff;
				lgt.theta = light.Theta;
				lgt.phi = light.Phi;
			}
			else return E_FAIL;
		}
		return hr;
	}

	void Device::setMaterial(const Material &mtrl)
	{
		D3DMATERIAL dxmtrl;
		dxmtrl.Diffuse  = mtrl.diffuse;
		dxmtrl.Ambient  = mtrl.ambient;
		dxmtrl.Specular = mtrl.specular;
		dxmtrl.Emissive = mtrl.emissive;
		dxmtrl.Power    = mtrl.power;
		ptr_->SetMaterial(&dxmtrl);

		Pointer<Texture> tex = mtrl.tex.lock();
		if(tex) ptr_->SetTexture(0, *tex);
		else ptr_->SetTexture(0, NULL);
		tex = mtrl.tex1.lock();
		if(tex) ptr_->SetTexture(1, *tex);
		else ptr_->SetTexture(1, NULL);
		tex = mtrl.tex2.lock();
		if(tex) ptr_->SetTexture(2, *tex);
		else ptr_->SetTexture(2, NULL);

		setBlendState(mtrl);
	}

	void Device::setBlendState(const Material &mtrl)
	{
		switch(mtrl.blend) {
		case Material::OPEQUE:
			ptr_->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			ptr_->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			ptr_->SetRenderState( D3DRS_ALPHAREF,   0x80 );
			ptr_->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATER );
			break;
		case Material::TRANSLUCENT:
			ptr_->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
			ptr_->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
			ptr_->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			ptr_->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			ptr_->SetRenderState( D3DRS_ALPHAREF,   0x00 );
			ptr_->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATER );
			break;
		case Material::ADD:
			ptr_->SetRenderState( D3DRS_SRCBLEND,	 D3DBLEND_SRCCOLOR );
			ptr_->SetRenderState( D3DRS_DESTBLEND,	 D3DBLEND_ONE );
			ptr_->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			ptr_->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			ptr_->SetRenderState( D3DRS_ALPHAREF,   0x00 );
			ptr_->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATER );
			break;
		case Material::SUB:
			ptr_->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_INVDESTCOLOR );
			ptr_->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_ZERO );
			ptr_->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			ptr_->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			ptr_->SetRenderState( D3DRS_ALPHAREF,   0x00 );
			ptr_->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATER );
			break;
		case Material::MUL:
			ptr_->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_ZERO );
			ptr_->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_SRCCOLOR );
			ptr_->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			ptr_->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			ptr_->SetRenderState( D3DRS_ALPHAREF,   0x00 );
			ptr_->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATER );
			break;
		}
		if(mtrl.double_side) {
			ptr_->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		}
		else {
#ifdef GCTP_COORD_RH
			ptr_->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
#else
			ptr_->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
#endif
		}
	}

	HRslt Device::beginRecord()
	{
		return ptr_->BeginStateBlock();
	}

	Pointer<StateBlock> Device::endRecord()
	{
		HRslt hr;
		IDirect3DStateBlockPtr sb;
		hr = ptr_->EndStateBlock(&sb);
		if(hr) {
			Pointer<StateBlock> ret(new StateBlock);
			hr = ret->setUp(sb, D3DSBT_ALL);
			if(hr) return ret;
		}
		GCTP_TRACE(hr);
		return NULL;
	}

	Pointer<StateBlock> Device::createState(D3DSTATEBLOCKTYPE type)
	{
		HRslt hr;
		IDirect3DStateBlockPtr sb;
		hr = ptr_->CreateStateBlock(type, &sb);
		if(hr) {
			Pointer<StateBlock> ret(new StateBlock);
			hr = ret->setUp(sb, type);
			if(hr) return ret;
		}
		GCTP_TRACE(hr);
		return NULL;
	}

	/* ステートブロックスタックに現在のステートを積む
	 *
	 * popStateでこの時点のステートに復帰できる。
	 * begin()、end()でこのスタックはクリアされる点に注意
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/14 15:49:15
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
//	HRslt Device::pushState(D3DSTATEBLOCKTYPE type)
//	{
//		return states_.push(ptr_, type);
//	}

	/* スタックトップのステートブロックを適用して、ポップする
	 *
	 * popStateでこの時点のステートに復帰できる。
	 * begin()、end()でこのスタックはクリアされる
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/14 15:49:15
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
//	HRslt Device::popState()
//	{
//		return states_.pop();
//	}

	/** 新規ビューを取得
	 *
	 * DirectXの用語で言うところの、追加スワップチェーンを制作
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/14 22:51:09
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Pointer<View> Device::createView(uint adpt, HWND hwnd)
	{
		PRNN(_T("追加スワップチェーン製作"));
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory( &d3dpp, sizeof(d3dpp) );
		d3dpp.hDeviceWindow = hwnd;
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.EnableAutoDepthStencil = (is_autodepthbuffer_)?TRUE:FALSE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		HRslt hr;
		IDirect3DSwapChainPtr psc;
		hr = ptr_->CreateAdditionalSwapChain(&d3dpp, &psc);
		if(hr && psc) {
			Pointer<View> ret(new View);
			hr = ret->setUp(psc);
			if(hr) {
				registerRsrc(Handle<Rsrc>(ret.get()));
				return ret;
			}
		}
		GCTP_TRACE(hr);
		return NULL;
	}

	/** 新規ビューを取得
	 *
	 * DirectXの用語で言うところの、追加スワップチェーンを制作
	 * ただしこれはマルチウィンドウ用ではなく、マルチヘッド用のもの
	 * マルチヘッドでデバイスを制作後、これでサブのスワップチェーンをViewとして
	 * 取得できる
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/14 22:51:09
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Pointer<View> Device::createView(uint adpt, uint idx)
	{
		PRNN(_T("セカンダリスワップチェーン取得"));
		Pointer<View> ret(new View);
		HRslt hr;
		if(ret) {
			hr = ret->setUp(ptr_, idx);
			if(hr) {
				registerRsrc(Handle<Rsrc>(ret.get()));
				return ret;
			}
		}
		else hr = E_OUTOFMEMORY;
		GCTP_TRACE(hr);
		return NULL;
	}

}}} // namespace gctp
