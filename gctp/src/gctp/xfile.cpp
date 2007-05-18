/** @file
 * GameCatapult Xファイルインターフェース実装
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 22:50:19
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/pointer.hpp>
#include <gctp/vector.hpp>
#include <gctp/xfile.hpp>
#include <gctp/uri.hpp>
#ifdef UNICODE
#include <gctp/wuri.hpp>
#endif
#include <rmxfguid.h>
#include <rmxftmpl.h>

using namespace std;

namespace gctp {

	XFileEnv::XFileEnv()
	{
		D3DXFileCreate(&(*this));
	}

	HRslt XFileEnv::registerDefaultTemplate()
	{
		return (*this)->RegisterTemplates((LPVOID)D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES);
	}

	HRslt XFileEnv::registerSkinTemplate()
	{
		return (*this)->RegisterTemplates((LPVOID)XSKINEXP_TEMPLATES, sizeof( XSKINEXP_TEMPLATES )-1);
	}
	
	HRslt XFileEnv::registerExTemplate()
	{
		return (*this)->RegisterTemplates((LPVOID)XEXTENSIONS_TEMPLATES GCTPXFILE_TEMPLATES, sizeof( XEXTENSIONS_TEMPLATES GCTPXFILE_TEMPLATES)-1);
	}
	
	HRslt XFileEnv::registerTemplate(const class XFileReader &is)
	{
		return (*this)->RegisterEnumTemplates(is);
	}

	HRslt XFileEnv::registerTemplate(const void *def, std::size_t size)
	{
		return (*this)->RegisterTemplates(def, size);
	}

	HRslt XFileEnv::registerTemplate(const char *def)
	{
		return (*this)->RegisterTemplates((LPVOID)def, strlen(def));
	}

	XSaver &XSaver::operator<<(const XData &data)
	{
		XSaveData w = newData(data);
		if(w) for(uint i = 0; i < data.size(); i++) w << data[i];
		return *this;
	}

	HRslt XFileReader::open(XFileEnv &env, const _TCHAR *fn)
	{
		if(env) {
			HRslt hr;
			// Create enum object.
#ifdef UNICODE
			hr = env->CreateEnumObject((LPVOID)fn, D3DXF_FILELOAD_FROMWFILE, &(*this));
#else
			hr = env->CreateEnumObject((LPVOID)fn, D3DXF_FILELOAD_FROMFILE, &(*this));
#endif
			if(!hr) {
#ifdef UNICODE
				std::string base = CStr(WURI(fn).basename().c_str()).c_str();
#else
				std::string base = URI(fn).basename();
#endif
				PRNN(_T("Win32リソース検索 ") << base);
/*				HRSRC rc = FindResource(NULL, "PEDITUIX", "XFILE");
				if(rc) {
					HGLOBAL gl = LoadResource(NULL, rc);
					if(gl) {
						const char *x = (const char *)LockResource(gl);// 開放しなくていいんだっけ？
						if(x) {
							DXFILELOADMEMORY mem = { (LPVOID)x, 27640 };
							mem.dSize = strlen(x);
							if(mem.dSize>0) {
								mem.dSize--;
								hr = env->CreateEnumObject((LPVOID)&mem, DXFILELOAD_FROMRESOURCE, &(*this));
							}
						}
					}
				}*/
				D3DXF_FILELOADRESOURCE rsc = { NULL, base.c_str(), "XFILE" };
				hr = env->CreateEnumObject((LPVOID)&rsc, D3DXF_FILELOAD_FROMRESOURCE, &(*this));
//				if(!hr) {
//					hr = env->CreateEnumObject((LPVOID)fn, D3DXF_FILELOAD_FROMURL, &(*this));
//				}
			}
			return hr;
		}
		else return E_FAIL;
		return S_OK;
	}

	HRslt XFileReader::open(XFileEnv &env, const void *memory, size_t size)
	{
		if(env) {
			HRslt hr;
			// Create enum object.
			D3DXF_FILELOADMEMORY arg = { const_cast<void *>(memory), (DWORD)size };
			hr = env->CreateEnumObject(&arg, D3DXF_FILELOAD_FROMMEMORY, &(*this));
			return hr;
		}
		else return E_FAIL;
		return S_OK;
	}

	HRslt XFileReader::open(const _TCHAR *fn)
	{
		HRslt hr;
		XFileEnv env;
		// Register templates for d3drm.
		hr = env.registerDefaultTemplate();
		if(!hr) return hr;
		return open(env, fn);
	}

	HRslt XFileReader::open(const void *memory, size_t size)
	{
		HRslt hr;
		XFileEnv env;
		// Register templates for d3drm.
		hr = env.registerDefaultTemplate();
		if(!hr) return hr;
		return open(env, memory, size);
	}

#ifdef UNICODE
# define FILESAVEOPTION D3DXF_FILESAVE_TOWFILE
#else
# define FILESAVEOPTION D3DXF_FILESAVE_TOFILE
#endif
	// ファイルオープン
	HRslt XFileWriter::open(XFileEnv &env, const _TCHAR *fn, Option option)
	{
		if(env) {
			HRslt hr;
			// Create enum object.
			switch(option) {
			case TEXT: hr = env->CreateSaveObject(fn, FILESAVEOPTION, DXFILEFORMAT_TEXT, &(*this)); break;
			case COMPRESSEDTEXT: hr = env->CreateSaveObject(fn, FILESAVEOPTION, DXFILEFORMAT_TEXT|DXFILEFORMAT_COMPRESSED, &(*this)); break;
			case BINARY: hr = env->CreateSaveObject(fn, FILESAVEOPTION, DXFILEFORMAT_BINARY, &(*this)); break;
			case COMPRESSEDBINARY: hr = env->CreateSaveObject(fn, FILESAVEOPTION, DXFILEFORMAT_BINARY|DXFILEFORMAT_COMPRESSED, &(*this)); break;
			default: return E_INVALIDARG;
			}
			if(!hr) return hr;
		}
		else return E_FAIL;
		return S_OK;
	}

	namespace xext {

		// {00E4D09B-7FB7-4e48-838B-EB3A6D452DB4}
		const GUID TID_Line = { 0xe4d09b, 0x7fb7, 0x4e48, { 0x83, 0x8b, 0xeb, 0x3a, 0x6d, 0x45, 0x2d, 0xb4 } };
		// {F562F616-410D-44d8-A1FF-2ADFB722F4DD}
		const GUID TID_Wire = { 0xf562f616, 0x410d, 0x44d8, { 0xa1, 0xff, 0x2a, 0xdf, 0xb7, 0x22, 0xf4, 0xdd } };

		Wire::Wire() : vert(0), idx(0), buf_(0), size_(0)
		{
		}

		Wire::Wire(ulong vnum, ulong inum)
		{
			buf_ = new ulong[1+vnum*3+1+inum*2];
			size_ = (1+vnum*3+1+inum*2)*4;
			vert = reinterpret_cast<Verticies *>(buf_);
			vert->num = vnum;
			idx = reinterpret_cast<Indicies *>(&vert->verticies[vert->num]);
			idx->num = inum;
		}

		Wire::~Wire()
		{
			if(buf_) delete[] buf_;
		}

		void Wire::read(XData &cur)
		{
			buf_ = new ulong[(cur.size()+3)/4];
			memcpy(buf_, cur.data(), cur.size());
			vert = reinterpret_cast<Verticies *>(buf_);
			idx = reinterpret_cast<Indicies *>(&vert->verticies[vert->num]);
		}

		XSaveData Wire::write(const char *name, XSaver &cur)
		{
			return cur.newData(TID_Wire, size_, buf_, name);
		}

	}

} // namespace gctp
