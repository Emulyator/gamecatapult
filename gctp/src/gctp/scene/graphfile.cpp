/** @file
 * GameCatapult シーングラフコンテントファイルクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 22:56:37
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/pointer.hpp>
#include <gctp/vector.hpp>
#include <gctp/xfile.hpp>
#include <gctp/scene/graphfile.hpp>
#include <gctp/graphic/model.hpp>
#include <gctp/scene/flesh.hpp>
#include <gctp/scene/body.hpp>
#include <gctp/scene/motion.hpp>
#include <gctp/extension.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/uri.hpp>
#include <rmxfguid.h>

using namespace std;

template<class E, class T>
std::basic_ostream<E, T> &operator<<(std::basic_ostream<E, T> &os, const GUID &g)
{
	char w[128];
	sprintf(w, "<%04X-%02X-%02x-%01X%01X-%01X%01X%01X%01X%01X%01X>", g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1],
		g.Data4[2], g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
	return os << w;
}

bool operator < (const GUID &lhs, const GUID &rhs)
{
	return memcmp(&lhs, &rhs, sizeof(GUID)) < 0;
}

namespace gctp { namespace scene {

	///////////////////
	// XFileHandler
	//

	/** テンプレート毎の読み込みイベントハンドラーを登録
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:19:25
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	XFileHandler::XFileHandler(const GUID &type, const XFileHandleMethod f)
	{
		handlers()[type] = f;
		me_ = handlers().find(type);
	}

	/** 登録を抹消
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:19:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	XFileHandler::~XFileHandler()
	{
		handlers().erase(me_);
	}

	/** チャンクに対応したハンドラがあるか？
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:19:37
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool XFileHandler::hasHandler(const GUID &type)
	{
		HandlerMap::iterator i = handlers().find(type);
		return i != handlers().end();
	}

	/** ハンドラを取得
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:19:44
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	XFileHandleMethod XFileHandler::get(const GUID &type)
	{
		HandlerMap::iterator i = handlers().find(type);
		if(i != handlers().end()) return i->second;
		return NULL;
	}

	/// HandlerMapのシングルトンを返す
	XFileHandler::HandlerMap &XFileHandler::handlers()
	{
		static HandlerMap _handlers_;
		return _handlers_;
	}

	namespace {
		GCTP_REGISTER_REALIZER2(x, GraphFile, &GraphFile::setUpFromX);
		//Extention extention_x("x", Realizer<XFile>);
		
		/** XFileの読みこみ
		 */
		HRslt loadX(
			const char *filename, /**< ファイル名 */
			GraphFile &self, /**< 対象のXfileオブジェクト*/
			XFileReadingWork &work,
			const XData &cur, /**< カレントの位置を示すXDataオブジェクト */
			Body *body=NULL, /**< カレントの階層ツリー */
			Body::NodePtr cnode=NULL /**< カレントのノード */
		) {
			//PRNN("chunk name : "<<cur.name());
			if(TID_D3DRMMesh == cur.type()) {
				//PRNN("TID_D3DRMMesh found");
				Pointer<graphic::Model> w = new graphic::Model;
				if(w) {
					HRslt hr = w->setUp(cur);
					if(hr) {
						self.push_back(w);
						if(body) {
							Pointer<Flesh> ww = new Flesh;
							if(ww) {
								ww->setUp(w, body, cnode);
								body->fleshies().push_back(ww);
							}
						}
					}
					else GCTP_TRACE(hr);
				}
				//PRNN("TID_D3DRMMesh end");
			}
			else if(xext::TID_Wire == cur.type()) {
				//PRNN("xext::TID_Wire found");
				Pointer<graphic::Model> w = new graphic::Model;
				if(w) {
					HRslt hr = w->setUpWire(cur);
					if(hr) {
						self.push_back(w);
						if(body) {
							Pointer<Flesh> ww = new Flesh;
							if(ww) {
								ww->setUp(w, body, cnode);
								body->fleshies().push_back(ww);
							}
						}
					}
					else GCTP_TRACE(hr);
				}
				//PRNN("xext::TID_Wire end");
			}
			else if(TID_D3DRMFrameTransformMatrix == cur.type()) {
				//PRNN("TID_D3DRMFrameTransformMatrix found");
				if(cnode) cnode->val.getLCM() = MatrixC(*(D3DXMATRIX *)cur.data());
				else PRN("cnode is NULL!"<<endl);
			}
			else if(DXFILEOBJ_AnimTicksPerSecond == cur.type()) {
				//PRNN("DXFILEOBJ_AnimTicksPerSecond found");
				work.ticks_per_sec = *(DWORD *)cur.data();
			}
			else if(TID_D3DRMAnimationSet == cur.type()) {
				//PRNN("TID_D3DRMAnimationSet found");
				Pointer<Motion> w = new Motion;
				if(w) {
					GCTP_TRACE(w);
					w->setUp(cur);
					w->setTicksPerSec(static_cast<float>(work.ticks_per_sec));
					self.push_back(w);
				}
				//PRNN("TID_D3DRMAnimationSet end");
			}
			else if(TID_D3DRMFrame == cur.type()) {
				//PRNN("TID_D3DRMFrame found");
				if(body) cnode = body->add(*cnode, cur.name().c_str());
				else {
					body = new Body;
					if(body) {
						self.push_back(body);
						body->setUp(cur.name().c_str());
						cnode = *body;
					}
				}
				
				for(uint i = 0; i < cur.size(); i++) loadX(filename, self, work, cur[i], body, cnode);
				//PRNN("TID_D3DRMFrame end");
			}
			else {
				PRNN(cur.type() << " : Parse Error or Unknown chunk");
#define UNKNOWN_CHUNK(_guid) if(_guid == cur.type()) PRNN(#_guid << " : Unknown chunk")
				UNKNOWN_CHUNK(TID_DXFILEHeader);
				UNKNOWN_CHUNK(TID_D3DRMInfo);
				UNKNOWN_CHUNK(TID_D3DRMMesh);
				UNKNOWN_CHUNK(TID_D3DRMVector);
				UNKNOWN_CHUNK(TID_D3DRMMeshFace);
				UNKNOWN_CHUNK(TID_D3DRMMaterial);
				UNKNOWN_CHUNK(TID_D3DRMMaterialArray);
				UNKNOWN_CHUNK(TID_D3DRMFrame);
				UNKNOWN_CHUNK(TID_D3DRMFrameTransformMatrix);
				UNKNOWN_CHUNK(TID_D3DRMMeshMaterialList);
				UNKNOWN_CHUNK(TID_D3DRMMeshTextureCoords);
				UNKNOWN_CHUNK(TID_D3DRMMeshNormals);
				UNKNOWN_CHUNK(TID_D3DRMCoords2d);
				UNKNOWN_CHUNK(TID_D3DRMMatrix4x4);
				UNKNOWN_CHUNK(TID_D3DRMAnimation);
				UNKNOWN_CHUNK(TID_D3DRMAnimationSet);
				UNKNOWN_CHUNK(TID_D3DRMAnimationKey);
				UNKNOWN_CHUNK(TID_D3DRMFloatKeys);
				UNKNOWN_CHUNK(TID_D3DRMMaterialAmbientColor);
				UNKNOWN_CHUNK(TID_D3DRMMaterialDiffuseColor);
				UNKNOWN_CHUNK(TID_D3DRMMaterialSpecularColor);
				UNKNOWN_CHUNK(TID_D3DRMMaterialEmissiveColor);
				UNKNOWN_CHUNK(TID_D3DRMMaterialPower);
				UNKNOWN_CHUNK(TID_D3DRMColorRGBA);
				UNKNOWN_CHUNK(TID_D3DRMColorRGB);
				UNKNOWN_CHUNK(TID_D3DRMGuid);
				UNKNOWN_CHUNK(TID_D3DRMTextureFilename);
				UNKNOWN_CHUNK(TID_D3DRMTextureReference);
				UNKNOWN_CHUNK(TID_D3DRMIndexedColor);
				UNKNOWN_CHUNK(TID_D3DRMMeshVertexColors);
				UNKNOWN_CHUNK(TID_D3DRMMaterialWrap);
				UNKNOWN_CHUNK(TID_D3DRMBoolean);
				UNKNOWN_CHUNK(TID_D3DRMMeshFaceWraps);
				UNKNOWN_CHUNK(TID_D3DRMBoolean2d);
				UNKNOWN_CHUNK(TID_D3DRMTimedFloatKeys);
				UNKNOWN_CHUNK(TID_D3DRMAnimationOptions);
				UNKNOWN_CHUNK(TID_D3DRMFramePosition);
				UNKNOWN_CHUNK(TID_D3DRMFrameVelocity);
				UNKNOWN_CHUNK(TID_D3DRMFrameRotation);
				UNKNOWN_CHUNK(TID_D3DRMLight);
				UNKNOWN_CHUNK(TID_D3DRMCamera);
				UNKNOWN_CHUNK(TID_D3DRMAppData);
				UNKNOWN_CHUNK(TID_D3DRMLightUmbra);
				UNKNOWN_CHUNK(TID_D3DRMLightRange);
				UNKNOWN_CHUNK(TID_D3DRMLightPenumbra);
				UNKNOWN_CHUNK(TID_D3DRMLightAttenuation);
				UNKNOWN_CHUNK(TID_D3DRMInlineData);
				UNKNOWN_CHUNK(TID_D3DRMUrl);
				UNKNOWN_CHUNK(TID_D3DRMProgressiveMesh);
				UNKNOWN_CHUNK(TID_D3DRMExternalVisual);
				UNKNOWN_CHUNK(TID_D3DRMStringProperty);
				UNKNOWN_CHUNK(TID_D3DRMPropertyBag);
				UNKNOWN_CHUNK(TID_D3DRMRightHanded);
				UNKNOWN_CHUNK(DXFILEOBJ_XSkinMeshHeader);
				UNKNOWN_CHUNK(DXFILEOBJ_VertexDuplicationIndices);
				UNKNOWN_CHUNK(DXFILEOBJ_SkinWeights);
#undef UNKNOWN_CHUNK
			}
			return S_OK;
		}

	} // namespace anonymous

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, GraphFile, Object);

	/** Xファイルからシーンファイルリソースを生成する。
	 *
	 * @return エラー値
	 */
	bool GraphFile::setUpFromX(const char *fn/**< ファイル名 */)
	{
		HRslt hr;
		XFileReader file;
		hr = file.open(fn);
		// これが、なぜかE_OUTOFMEMORYを返すことがある。
		// 一体どこでロックすればいいんだ？
		if(hr == E_OUTOFMEMORY) { // …めんどいんで３回試行することにする…
			::Sleep(1);
			hr = file.open(fn);
		}
		if(hr == E_OUTOFMEMORY) { // …めんどいんで３回試行することにする…
			::Sleep(1);
			hr = file.open(fn);
		}
		if(hr == E_OUTOFMEMORY) { // …めんどいんで３回試行することにする…
			::Sleep(1);
			hr = file.open(fn);
		}
		if(hr) {
			PRN("Begin read Xfile : "<<fn<<endl);
			URI _fn = fn;
			XFileReadingWork work;
			work.ticks_per_sec = 30;
#ifdef UNICODE
			wchar_t cur_dir[MAX_PATH];
			::GetCurrentDirectory(MAX_PATH, cur_dir);
			string __dir = _fn.path();
			wchar_t _dir[MAX_PATH];
			if(0 == MultiByteToWideChar(932, 0, __dir.c_str(), -1, _dir, 256)) MultiByteToWideChar(CP_ACP, 0, __dir.c_str(), -1, _dir, 256);
			::SetCurrentDirectory(_dir);
#else
			char cur_dir[MAX_PATH];
			::GetCurrentDirectory(MAX_PATH, cur_dir);
			::SetCurrentDirectory(_fn.path().c_str());
#endif
			for(uint i = 0; i < file.size(); i++) {
				hr = loadX(fn, *this, work, file[i]);
				if(hr == E_OUTOFMEMORY) { // …めんどいんで３回試行することにする…
					::Sleep(1);
					hr = loadX(fn, *this, work, file[i]);
				}
				if(hr == E_OUTOFMEMORY) { // …めんどいんで３回試行することにする…
					::Sleep(1);
					hr = loadX(fn, *this, work, file[i]);
				}
				if(hr == E_OUTOFMEMORY) { // …めんどいんで３回試行することにする…
					::Sleep(1);
					hr = loadX(fn, *this, work, file[i]);
				}
				if(!hr) break;
			}
#ifdef UNICODE
			::SetCurrentDirectory(cur_dir);
			PRN(L"End read Xfile : "<<fn<<endl);
#else
			::SetCurrentDirectory(cur_dir);
			PRN("End read Xfile : "<<fn<<endl);
#endif
			if(!hr) GCTP_TRACE(hr);
		}
		else PRNN("Xfile "<<fn<<" : "<<hr);
		return hr;
	}

}} // namespace gctp::scene
