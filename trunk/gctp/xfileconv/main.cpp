/**@file
 * Ｘファイル変換ユーティリティー
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 3:41:23
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "StdAfx.h"
#define GC_LOGFILE 0
#include <gctp/dbgout.hpp>
#include <gctp/argmap.hpp>
#include <gctp/pointer.hpp>
#include <gctp/xfilei.hpp>
#include <gctp/archive.hpp>
#include <gctp/matrix.hpp>
#include <gctp/quat.hpp>
#include <gctp/color.hpp>
#include <set>

#include <rmxfguid.h>

using namespace gctp;
using namespace std;

// レコード生成トークンは次のように定義される。
#define TOKEN_NAME         1
#define TOKEN_STRING       2
#define TOKEN_INTEGER      3
#define TOKEN_GUID         5
#define TOKEN_INTEGER_LIST 6
#define TOKEN_FLOAT_LIST   7

// スタンドアロン トークンは次のように定義される。
#define TOKEN_OBRACE      10
#define TOKEN_CBRACE      11
#define TOKEN_OPAREN      12
#define TOKEN_CPAREN      13
#define TOKEN_OBRACKET    14
#define TOKEN_CBRACKET    15
#define TOKEN_OANGLE      16
#define TOKEN_CANGLE      17
#define TOKEN_DOT         18
#define TOKEN_COMMA       19
#define TOKEN_SEMICOLON   20
#define TOKEN_TEMPLATE    31
#define TOKEN_WORD        40
#define TOKEN_DWORD       41
#define TOKEN_FLOAT       42
#define TOKEN_DOUBLE      43
#define TOKEN_CHAR        44
#define TOKEN_UCHAR       45
#define TOKEN_SWORD       46
#define TOKEN_SDWORD      47
#define TOKEN_VOID        48
#define TOKEN_LPSTR       49
#define TOKEN_UNICODE     50
#define TOKEN_CSTRING     51
#define TOKEN_ARRAY       52

void filtering(XSaver &os, const XData &cur)
{
	if(TID_D3DRMMesh == cur.type()) {
		PRNN("TID_D3DRMMesh found");
		const XVerticies *vs = cur.verticies();
		for(uint i = 0; i < vs->num; i++) {
			//vs->verticies[i].z *= -1.0f;
		}
		uint fnum = cur.faceNum();
		for(uint i = 0; i < fnum; i++) {
			const XIndicies *ids = cur.face(i);
			for(uint j = 0; j < ids->num/2; j++) {
				//swap(ids->indicies[j], ids->indicies[ids->num-1-j]);
			}
		}
		XSaveData savedata = os.newData(cur);
		for(uint i = 0; i < cur.size(); i++) {
			XData dat = cur[i];
			if(!dat.isRef()) filtering(savedata, dat);
			else savedata << dat;
		}
		PRNN("TID_D3DRMMesh end");
	}
	else if(TID_D3DRMMeshNormals == cur.type()) {
		PRNN("TID_D3DRMMeshNormals found");
		const XVerticies *vs = cur.verticies();
		for(uint i = 0; i < vs->num; i++) {
			//vs->verticies[i].z *= -1.0f;
		}
		uint fnum = cur.faceNum();
		for(uint i = 0; i < fnum; i++) {
			const XIndicies *ids = cur.face(i);
			for(uint j = 0; j < ids->num/2; j++) {
				//swap(ids->indicies[j], ids->indicies[ids->num-1-j]);
			}
		}
		os << cur;
		PRNN("TID_D3DRMMeshNormals end");
	}
/*	else if(TID_D3DRMFrameTransformMatrix == *cur.type()) {
		PRNN("TID_D3DRMFrameTransformMatrix found");
		PRNN("TID_D3DRMFrameTransformMatrix end");
	}*/
	else if(TID_D3DRMAnimationSet == cur.type()) {
		PRNN("TID_D3DRMAnimationSet found");
		XSaveData savedata = os.newData(cur);
		for(uint i = 0; i < cur.size(); i++) {
			XData dat = cur[i];
			if(!dat.isRef()) filtering(savedata, dat);
			else savedata << dat;
		}
		PRNN("TID_D3DRMAnimationSet end");
	}
	else if(TID_D3DRMAnimation == cur.type()) {
		PRNN("TID_D3DRMAnimation found");
		XSaveData savedata = os.newData(cur);
		for(uint i = 0; i < cur.size(); i++) {
			XData dat = cur[i];
			if(!dat.isRef()) filtering(savedata, dat);
			else savedata << dat;
		}
		PRNN("TID_D3DRMAnimation end");
	}
	else if(TID_D3DRMAnimationKey == cur.type()) {
		PRNN("TID_D3DRMAnimationKey found");
		ulong _size;
		const void *_data = cur.data(_size);
		void *replace_data = NULL;
		const XKeys *keys = cur.keys();
		if(keys) {
			if(keys->type == XKEY_SCALING) {
				//XScaleKeys *keys = cur.poskeys();
				//XScaleKey newkey = keys->keys[0]; newkey.time = 7;
				//XData adddata = os.newData(TID_D3DRMTimedFloatKeys, sizeof(XScaleKey), &newkey);
				//cur << adddata;
			}
			else if(keys->type == XKEY_ORIENTATION) {
				replace_data = malloc(_size);
				memcpy(replace_data, _data, _size);
				XOrientationKeys *keys = reinterpret_cast<XOrientationKeys *>(replace_data);
				for(uint i = 0; i < keys->num; i++) {
					keys->keys[i].val.x *= -1.0f;
					keys->keys[i].val.y *= -1.0f;
					keys->keys[i].val.z *= -1.0f;
				}
				//XRotationKey newkey = keys->keys[0]; newkey.time = 7;
				//XData adddata = os.newData(TID_D3DRMTimedFloatKeys, sizeof(XRotationKey), &newkey);
				//cur << adddata;
			}
			else if(keys->type == XKEY_POSITION) {
				//XPositionKeys *keys = cur.poskeys();
				//XPositionKey newkey = keys->keys[0]; newkey.time = 7;
				//XData adddata = os.newData(TID_D3DRMTimedFloatKeys, sizeof(XPositionKey), &newkey);
				//cur << adddata;
			}
		}
		XSaveData savedata = os.newData(cur, _size, (replace_data ? replace_data : _data));
		if(replace_data) free(replace_data);
		for(uint i = 0; i < cur.size(); i++) {
			XData dat = cur[i];
			if(!dat.isRef()) filtering(savedata, dat);
			else savedata << dat;
		}
		PRNN("TID_D3DRMAnimationKey end");
	}
	else if(TID_D3DRMFrame == cur.type()) {
		PRNN("TID_D3DRMFrame found");
		XSaveData savedata = os.newData(cur);
		for(uint i = 0; i < cur.size(); i++) {
			XData dat = cur[i];
			if(!dat.isRef()) filtering(savedata, dat);
			else savedata << dat;
		}
		PRNN("TID_D3DRMFrame end");
	}
	else os << cur;
}

struct _XFrame {
	_XFrame() {}
	explicit _XFrame(const XData &src)
	{
		name = src.name();
		mat = *(D3DXMATRIX *)src[(uint)0].data();
	}
	CStr name;
	Matrix mat;
};
typedef std::vector<_XFrame> XFrameVector; // 直接XDataは格納できない…

struct X1FrameKeyPos {
	X1FrameKeyPos(const Vector &src)
	{
		keyType = XKEY_POSITION;
		nKeys = 1;
		time = 0;
		nVals = 3;
		x = src.x; y = src.y; z = src.z;
	}
	DWORD keyType;
    DWORD nKeys;
    DWORD time;
    DWORD nVals;
	float x;
	float y;
	float z;
};

struct X1FrameKeyOrt {
	X1FrameKeyOrt(const Quat &src)
	{
		keyType = XKEY_ORIENTATION;
		nKeys = 1;
		time = 0;
		nVals = 4;
		w = src.w; x = src.x; y = src.y; z = src.z;
	}
	DWORD keyType;
    DWORD nKeys;
    DWORD time;
    DWORD nVals;
	float w;
	float x;
	float y;
	float z;
};

struct X1FrameKeyScl {
	X1FrameKeyScl(const Vector &src)
	{
		keyType = XKEY_SCALING;
		nKeys = 1;
		time = 0;
		nVals = 3;
		x = src.x; y = src.y; z = src.z;
	}
	DWORD keyType;
    DWORD nKeys;
    DWORD time;
    DWORD nVals;
	float x;
	float y;
	float z;
};

bool addAnim(XSaveData animset, XData cur, std::set<std::string> excludeframes, XFrameVector &frames)
{
	bool ret = false;
	for(XFrameVector::iterator i = frames.begin(); i != frames.end(); ++i) {
		std::string animname = i->name.c_str();
		animname.replace(0, 3, "anim");
		std::set<std::string>::const_iterator f = excludeframes.find(animname);
		if(f == excludeframes.end()) {
			Vector pos = i->mat.getTrans();
			Quat ort = i->mat;
			Vector scl = i->mat.getScaling();
			XSaveData anim;
			if(!(  0.999999f < scl.x && scl.x < 1.000001f
				&& 0.999999f < scl.y && scl.y < 1.000001f
				&& 0.999999f < scl.z && scl.z < 1.000001f)) {
				X1FrameKeyScl key(scl);
				if(!anim) {
					anim = animset.newData(TID_D3DRMAnimation, 0, NULL, animname.c_str());
					anim.addRefData(i->name.c_str());
				}
				XSaveData keys = anim.newData(TID_D3DRMAnimationKey, sizeof(key), &key);
			}
			if(!(   0.999999f < ort.w && ort.w < 1.000001f
				&& -0.000001f < ort.x && ort.x < 0.000001f
				&& -0.000001f < ort.y && ort.y < 0.000001f
				&& -0.000001f < ort.z && ort.z < 0.000001f)) {
				if(!anim) {
					anim = animset.newData(TID_D3DRMAnimation, 0, NULL, animname.c_str());
					anim.addRefData(i->name.c_str());
				}
				X1FrameKeyOrt key(ort);
				XSaveData keys = anim.newData(TID_D3DRMAnimationKey, sizeof(key), &key);
			}
			if(pos.length() > 0.000001f) {
				if(!anim) {
					anim = animset.newData(TID_D3DRMAnimation, 0, NULL, animname.c_str());
					anim.addRefData(i->name.c_str());
				}
				X1FrameKeyPos key(pos);
				XSaveData keys = anim.newData(TID_D3DRMAnimationKey, sizeof(key), &key);
			}
			if(anim) ret = true;
		}
	}
	return ret;
}

void SImerge(XSaver &os, const XData &cur, XFrameVector &frames)
{
	if(TID_D3DRMAnimationSet == cur.type()) {
		PRNN("TID_D3DRMAnimationSet found");
		
		std::set<std::string> animframes;
		for(uint i = 0; i < cur.size(); i++) {
			XData dat = cur[i];
			if(TID_D3DRMAnimation == dat.type()) {
				animframes.insert(dat.name().c_str());
			}
		}
		XSaveData animset = os.newData(cur);
		for(uint i = 0; i < cur.size(); i++) {
			addAnim(animset, cur[i], animframes, frames);
		}
		frames.clear();
		PRNN("TID_D3DRMAnimationSet end");
	}
	else if(TID_D3DRMFrame == cur.type()) {
		PRNN("TID_D3DRMFrame found");
		frames.push_back(_XFrame(cur));
		for(uint i = 0; i < cur.size(); i++) {
			XData dat = cur[i];
			if(!dat.isRef()) SImerge(os, dat, frames);
		}
		PRNN("TID_D3DRMFrame end");
	}
	else os << cur;
}

void x_print(BYTE x, int &c)
{
	if(c == 0) printf("\t"); else printf(" ");
	printf("0x%X,", x);
	if(++c == 8) { printf("\n"); c = 0; }
}

void x_print(WORD x, int &c)
{
	x_print((BYTE)(x>>8), c);
	x_print((BYTE)(x&0xFF), c);
}

void x_print(DWORD x, int &c)
{
	x_print((BYTE)((x>>24)&0xFF), c);
	x_print((BYTE)((x>>16)&0xFF), c);
	x_print((BYTE)((x>>8)&0xFF), c);
	x_print((BYTE)(x&0xFF), c);
}

int main(int argc, char *argv[])
{
	ArgMap arg;
	arg.addHasValKey("output");
	arg.defAlias('o', "output");
	arg.defAlias('t', "text");
	arg.defAlias('b', "bin");
	arg.defAlias('c', "compress");
	arg.defAlias('p', "printtemplate");
	arg.defAlias('f', "filter");
	arg.defAlias('m', "merge");
	arg.parse(argc, argv);
	if(arg.hasKey("") && arg.hasKey("printtemplate")) {
		File f(arg[""][0].c_str());
		if(f.is_open()) {
			f.seek(0x8);
			char fmt[5];
			f.read(fmt, 4);
			fmt[4] = '\0';
			if(string("bin ") == fmt) {
				f.seek(0x10);
				int c = 0;
				int nest_level;
				BYTE data;
				WORD token;
				DWORD size, data32;
				for(f >> token; !f.eof() && token == TOKEN_TEMPLATE; f >> token) {
					x_print(token, c);
					for(nest_level = 0, f >> token; !f.eof(); f >> token) {
						x_print(token, c);
						if(token == TOKEN_NAME || token == TOKEN_STRING) {
							f >> size;
							x_print(size, c);
							std::string str;
							for(DWORD i = 0; i < size; i++) {
								f >> data;
								x_print(data, c);
								str += data;
							}
							PRNN(str);
						}
						else if(token == TOKEN_INTEGER) {
							f >> data32;
							x_print(size, c);
						}
						else if(token == TOKEN_GUID) {
							for(DWORD i = 0; i < 16; i++) {
								f >> data;
								x_print(data, c);
							}
						}
						else if(token == TOKEN_INTEGER_LIST || token == TOKEN_FLOAT_LIST) {
							f >> size;
							x_print(size, c);
							for(DWORD i = 0; i < size; i++) {
								f >> data32;
								x_print(data32, c);
							}
						}
						else if(token == TOKEN_OBRACE) nest_level++;
						else if(token == TOKEN_CBRACE) {
							nest_level--;
							if(nest_level == 0) break;
						}
					}
					PRNN("End 1 Template");
				}
				if(c > 0) printf("\n");
			}
		}
	}

	{// 実験
		XFileEnv env;
		env.registerDefaultTemplate();
		env.registerSkinTemplate();
		XFileWriter os(env, "test.x", XFileWriter::TEXT);
		struct Color3 {
			float r, g, b;
		};
		struct Mtrl {
			Color diffuse;
			float power;
			Color3 specular;
			Color3 emissive;
		} mtrl;
		mtrl.diffuse = Color(1,2,3,4);
		mtrl.power = 5;
		mtrl.specular.r = 6;
		mtrl.specular.g = 7;
		mtrl.specular.b = 8;
		mtrl.emissive.r = 9;
		mtrl.emissive.g = 10;
		mtrl.emissive.b = 11;
		XSaveData _mtrl = os.newData(TID_D3DRMMaterial, sizeof(Mtrl), &mtrl, "wwwwww");
		if(_mtrl) {
			PRNN("oi");
			const char *texname = "TextureName";
			_mtrl.newData(TID_D3DRMTextureFilename, strlen(texname)+1, texname);
		}
//		mtrl.newData(TID_D3DRMColorRGBA, 4*4, &Color(1,2,3,4));
//		float power = 5;
//		mtrl.newData(TID_D3DRMMaterialPower, 4, &power);
//		mtrl.newData(TID_D3DRMColorRGB, 4*3, &Color(6,7,8,9));
//		mtrl.newData(TID_D3DRMColorRGB, 4*3, &Color(10,11,12,13));
	}

	if(0 && arg.hasKey("output") && arg.hasKey("")) {
		HRslt hr;
		XFileEnv env;
		if(arg.hasKey("merge")) {
			XFileWriter::Option option = XFileWriter::TEXT;
			if(arg.hasKey("bin")) option = XFileWriter::BINARY;
			if(arg.hasKey("compress")) option = XFileWriter::COMPRESSED;
			if(arg.hasKey("text")) option = XFileWriter::TEXT;
			env.registerDefaultTemplate();
			env.registerSkinTemplate();
			XFileWriter os(env, arg["output"][0].c_str(), option);
			std::size_t n = arg[""].size();
			for(std::size_t i = 0; i < n; i++) {
				XFileReader is(env, arg[""][i].c_str());
				if(i == 0) {
					for(uint i = 0; i < is.size(); i++) {
						os << is[i];
					}
				}
				else {
					XFrameVector frames;
					for(uint i = 0; i < is.size(); i++) {
						XData cur = is[i];
						SImerge(os, cur, frames);
						if(frames.size()>0) {
							XSaveData animset = os.newData(TID_D3DRMAnimationSet, 0, NULL);
							std::set<std::string> excludeframe;
							addAnim(animset, cur, excludeframe, frames);
						}
					}
				}
				hr = env.registerTemplate(is); // これ、どうやれば動くんだ？
				if(!hr) GTRACE(hr);
				PRN("End read Xfile : "<<arg[""][i].c_str()<<endl);
			}
		}
		else {
			XFileReader is(arg[""][0].c_str());
			env.registerDefaultTemplate();
			env.registerSkinTemplate();
			if(!hr) GTRACE(hr);
			XFileWriter::Option option = XFileWriter::TEXT;
			if(arg.hasKey("bin")) option = XFileWriter::BINARY;
			if(arg.hasKey("compress")) option = XFileWriter::COMPRESSED;
			if(arg.hasKey("text")) option = XFileWriter::TEXT;
			XFileWriter os(env, arg["output"][0].c_str(), option);
			for(uint i = 0; i < is.size(); i++) {
				XData cur = is[i];
				if(arg.hasKey("filter")) filtering(os, cur);
				else os << cur;
			}
			hr = env.registerTemplate(is); // これ、どうやれば動くんだ？
			if(!hr) GTRACE(hr);
			PRNN("End read Xfile : "<<arg[""][0].c_str()<<"=>"<<arg["output"][0].c_str());
		}
		return hr?0:hr.i;
	}
	return 1;
}

#pragma comment(lib, "DxErr9.lib")
#pragma comment(lib, "d3dxof.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "quartz.lib")
#pragma comment(lib, "d3dx9vc6.lib")
