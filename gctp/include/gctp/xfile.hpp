#ifndef _GCTP_XFILE_HPP_
#define _GCTP_XFILE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult XFile操作サポートクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 21:29:23
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/def.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/cstr.hpp>
#include <gctp/com_ptr.hpp>
#include <tchar.h>

namespace gctp {
	TYPEDEF_DXCOMPTR(ID3DXFile);
	TYPEDEF_DXCOMPTR(ID3DXFileData);
	TYPEDEF_DXCOMPTR(ID3DXFileEnumObject);
	TYPEDEF_DXCOMPTR(ID3DXFileSaveData);
	TYPEDEF_DXCOMPTR(ID3DXFileSaveObject);
}

namespace gctp {

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4200)
#endif

	/// Xファイル環境
	class XFileEnv : public ID3DXFilePtr {
	public:
		XFileEnv();
		HRslt registerDefaultTemplate();
		HRslt registerSkinTemplate();
		HRslt registerExTemplate();
		HRslt registerTemplate(const class XFileReader &is);
		HRslt registerTemplate(const void *def, std::size_t size);
		HRslt registerTemplate(const char *def);

		// 代入演算子を外部にさらす
		XFileEnv &operator =(XFileEnv &src) {
			ID3DXFilePtr::operator=(src);
			return *this;
		}
		XFileEnv &operator =(ID3DXFile *src) {
			ID3DXFilePtr::operator=(src);
			return *this;
		}
	};

	/** @name アニメーションデータ構造体
	 * Xファイルからの読み取り用型定義
	 */
	/**@{*/
	/// TimedFloatKeyチャンクの型
	template<typename _T>
	struct XKey {
		DWORD	time;
		DWORD	length; // ※ダブルワード長サイズ
		_T		val;
	};

	enum XKeyType {
		XKEY_POSTURE	= 0,
		XKEY_SCALE		= 1,
		XKEY_POSITION	= 2,
		XKEY_YPR		= 3,	// ３が謎だが…オイラー角用だったのかな？
		XKEY_MATRIX		= 4		// SkinnedMeshのソースでは4になっている……が、ヘルプでは３だ 4でイイらしい……
	};

	struct XKeys {
		XKeyType	type;
		DWORD		num;
	};

	/// AnimationKeyチャンク
	template<typename _T>
	struct TXKeys : public XKeys {
		_T		keys[];
	};

	struct XVector {
		float		x;
		float		y;
		float		z;
	};

	struct XQuaternion {
		float		w;
		float		x;
		float		y;
		float		z;
	};

	typedef XKey<XQuaternion>	XPostureKey;
	typedef XKey<D3DXVECTOR3>	XScaleKey, XPositionKey, XYPRKey;
	typedef XKey<D3DXMATRIX>	XMatrixKey;
	typedef TXKeys<XPostureKey>	XPostureKeys;
	typedef TXKeys<XScaleKey>	XScaleKeys, XPositionKeys, XYPRKeys;
	typedef TXKeys<XMatrixKey>	XMatrixKeys;

	struct XVerticies {
		DWORD num;
		XVector verticies[];
	};

	struct XIndicies {
		DWORD num;
		DWORD indicies[];
	};
	
	struct XFaces {
		DWORD num;
		XIndicies face;
	};

	struct XAnimOption {
		DWORD	openclosed;
		DWORD	positionquality;
	};
	/**@}*/

	/// MeshMaterialListの型
	struct XMeshMaterialList {
		DWORD mtrlnum;
		DWORD mtrlnonum;
		DWORD mtrlno[];
	};

	/// Materialの型
	struct XMaterial {
		D3DCOLORVALUE face_color;
		float power;
		float specular_r;
		float specular_g;
		float specular_b;
		float emissive_r;
		float emissive_g;
		float emissive_b;
	};

	/// D3DXファイルデータハンドル
	class XData : public ID3DXFileDataPtr {
	public:
		class ScopedLock : public Object {
		public:
			const void *data() const { return data_; }
			ulong size() const { return size_; }
			explicit ScopedLock(const ID3DXFileDataPtr &src) : ptr_(src) { ptr_->Lock(&size_, &data_); }
			~ScopedLock() { ptr_->Unlock(); }
		private:
			ID3DXFileDataPtr ptr_;
			const void *data_;
			ulong size_;
		};

		/// チャンク名取得
		CStr name() const {
			DWORD size = 0;
			(*this)->GetName(NULL, &size);
			if(size) {
				CStr ret(size+1);
				(*this)->GetName((char *)ret.c_str(), &size);
				return ret;
			}
			return CStr();
		}

		/// チャンクのGUIDを取得
		GUID id() const {
			GUID ret;
			(*this)->GetId(&ret);
			return ret;
		}
		
		/// タイプ取得
		GUID type() const {
			GUID ret;
			if(*this) (*this)->GetType(&ret);
			return ret;
		}
		
		/** 参照か？
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/28 18:05:03
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		bool isRef() const
		{
			return (*this)->IsReference() == TRUE;
		}

		/** 子データの数を取得
		 *
		 */
		size_t numChildren() const {
			if(*this) {
				SIZE_T ret;
				HRslt hr;
				if(hr = (*this)->GetChildren(&ret)) return ret;
			}
			return 0;
		}

		// @{
		/** 子データを取得
		 *
		 * 0～size()までの数を指定する
		 * @return ID3DXFileObjectのポインタ。失敗した場合は、NULLが返る
		 */
		XData getChild(uint i) const {
			XData ret;
			if(*this) (*this)->GetChild(i, &ret);
			return ret;
		}
		XData operator[](uint i) const { return getChild(i); }
		// @}
		
		/// ロック解除
		void unlock() const {
			const_cast<XData *>(this)->lock_ = 0;
		}

		/// ロック
		const void *lock() const {
			unlock();
			const_cast<XData *>(this)->lock_ = new ScopedLock(*this);
			if(lock_) return lock_->data();
			return 0;
		}
		
		/// データ取得
		const void *data(ulong &size) const {
			if(!lock_) lock();
			if(lock_) {
				size = lock_->size();
				return lock_->data();
			}
			return 0;
		}

		/// データサイズ取得
		ulong size() const {
			ulong ret = 0;
			if(!lock_) {
				lock();
				if(lock_) {
					ret = lock_->size();
					unlock();
				}
			}
			else ret = lock_->size();
			return ret;
		}
		
		/// データ取得
		const void *data() const {
			if(!lock_) lock();
			if(lock_) return lock_->data();
			return 0;
		}

		// 代入演算子を外部にさらす
		XData &operator =(const XData &src) {
			ID3DXFileDataPtr::operator=(src);
			return *this;
		}
		XData &operator =(ID3DXFileData *src) {
			ID3DXFileDataPtr::operator=(src);
			return *this;
		}

		const XAnimOption *animoption() const { return reinterpret_cast<const XAnimOption *>(data()); }
		const XKeys *keys() const { return reinterpret_cast<const XKeys *>(data()); }
		const XScaleKeys *sclkeys() const { return reinterpret_cast<const XScaleKeys *>(data()); }
		const XPostureKeys *posturekeys() const { return reinterpret_cast<const XPostureKeys *>(data()); }
		const XYPRKeys *yprkeys() const { return reinterpret_cast<const XYPRKeys *>(data()); }
		const XPositionKeys *positionkeys() const { return reinterpret_cast<const XPositionKeys *>(data()); }
		const XMatrixKeys *matkeys() const { return reinterpret_cast<const XMatrixKeys *>(data()); }
		const XVerticies *verticies() const { return reinterpret_cast<const XVerticies *>(data()); }
		DWORD faceNum() const {
			const XVerticies *vs = verticies();
			const XFaces *faces = reinterpret_cast<const XFaces *>(&vs->verticies[vs->num]);
			return faces->num;
		}
		const XIndicies *face(uint idx) const {
			const XVerticies *vs = verticies();
			const XFaces *faces = reinterpret_cast<const XFaces *>(&vs->verticies[vs->num]);
			if(idx < faces->num) {
				const XIndicies *ret = &faces->face;
				for(uint i = 0; i < idx && i < faces->num && idx < faces->num; i++) {
					ret = reinterpret_cast<const XIndicies *>(&ret->indicies[ret->num]);
				}
				return ret;
			}
			return NULL;
		}
	private:
		Pointer<ScopedLock> lock_;
	};

	/// Xファイル読み込み
	class XFileReader : public ID3DXFileEnumObjectPtr {
	public:
		XFileReader() {}
		explicit XFileReader(const _TCHAR* fn) { open(fn); }
		XFileReader(XFileEnv &env, const _TCHAR* fn) { open(env, fn); }
		
		// ファイルオープン
		HRslt open(XFileEnv &env, const _TCHAR *fn);
		HRslt open(XFileEnv &env, const void *memory, size_t size);
		HRslt open(const _TCHAR *fn);
		HRslt open(const void *memory, size_t size);

		bool isOpen() { return (*this) ? true : false; }
		
		void close()
		{
			(*this) = 0;
		}

		/** 子データの数を取得
		 *
		 */
		size_t size() {
			if(*this) {
				SIZE_T ret;
				HRslt hr;
				if(hr = (*this)->GetChildren(&ret)) return ret;
			}
			return 0;
		}

		// @{
		/** 子データを取得
		 *
		 * 0～size()までの数を指定する
		 * @return ID3DXFileObjectのポインタ。失敗した場合は、NULLが返る
		 */
		XData getChild(uint i) const {
			XData ret;
			if(*this) (*this)->GetChild(i, &ret);
			return ret;
		}
		XData operator[](uint i) const { return getChild(i); }
		// @}
		
		// データ取得
		// 失敗した場合は、NULLが返る
		XData find(const GUID &guid) const {
			XData ret;
			if(*this) (*this)->GetDataObjectById(guid, &ret);
			return ret;
		}
		
		// データ取得
		// 失敗した場合は、NULLが返る
		XData find(const char *name) const {
			XData ret;
			if(*this) (*this)->GetDataObjectByName(name, &ret);
			return ret;
		}
		
		// 代入演算子を外部にさらす
		XFileReader &operator =(XFileReader &src) {
			ID3DXFileEnumObjectPtr::operator=(src);
			return *this;
		}
		XFileReader &operator =(ID3DXFileEnumObject *src) {
			ID3DXFileEnumObjectPtr::operator=(src);
			return *this;
		}
	};

	class XSaveData;
	/// D3DXファイルセーバ抽象ハンドル
	class XSaver {
	public:
		// @{
		/// 子オブジェクト制作
		virtual XSaveData newData(const GUID &type, ulong size, const void *buf, const char *name = NULL, const GUID *id = NULL) = 0;
		virtual XSaveData newData(const XData &data, ulong size, const void *buf) = 0;
		virtual XSaveData newData(const XData &data) = 0;
		// @}

		/// 子参照オブジェクト制作
		virtual HRslt addRefData(const char *name, const GUID *objguid = NULL) = 0;

		/// そのままスルー
		XSaver &operator<<(const XData &data);
	};

	/// D3DXファイルセーブデータハンドル
	class XSaveData : public XSaver, public ID3DXFileSaveDataPtr {
	public:
		/// チャンク名取得
		CStr name() const {
			DWORD size = 0;
			(*this)->GetName(NULL, &size);
			if(size) {
				CStr ret(size+1);
				(*this)->GetName((char *)ret.c_str(), &size);
				return ret;
			}
			return CStr();
		}
		/// チャンクのGUIDを取得
		GUID id() const {
			GUID ret;
			(*this)->GetId(&ret);
			return ret;
		}
		
		/// タイプ取得
		GUID type() const {
			GUID ret;
			if(*this) (*this)->GetType(&ret);
			return ret;
		}
		
		// 代入演算子を外部にさらす
		XSaveData &operator =(XSaveData &src) {
			ID3DXFileSaveDataPtr::operator=(src);
			return *this;
		}
		XSaveData &operator =(ID3DXFileData *src) {
			ID3DXFileSaveDataPtr::operator=(src);
			return *this;
		}

		// @{
		/// 子オブジェクト制作
		XSaveData newData(const GUID &type, ulong size, const void *buf, const char *name = NULL, const GUID *id = NULL)
		{
			XSaveData ret;
			if(*this) (*this)->AddDataObject(type, name, id, size, buf, &ret);
			return ret;
		}

		XSaveData newData(const XData &data, ulong size, const void *buf)
		{
			if(*this) {
				if(!data.isRef()) return newData(data.type(), size, buf, data.name().c_str(), &data.id());
				else addRefData(data.name().c_str(), &data.id());
			}
			return XSaveData();
		}

		XSaveData newData(const XData &data)
		{
			if(*this) {
				if(!data.isRef()) {
					ulong _size;
					const void *_buf = data.data(_size);
					return newData(data, _size, _buf);
				}
				else addRefData(data.name().c_str(), &data.id());
			}
			return XSaveData();
		}
		// @}

		/// 子参照オブジェクト制作
		HRslt addRefData(const char *name, const GUID *objguid = NULL)
		{
			if(*this) return (*this)->AddDataReference(name, objguid);
			return E_POINTER;
		}

		/// そのままスルー
		XSaveData &operator<<(const XData &data)
		{
			XSaveData w = newData(data);
			if(w) for(uint i = 0; i < data.numChildren(); i++) w << data[i];
			return *this;
		}
	};

	/// Xファイル書き出し
	class XFileWriter : public XSaver, public ID3DXFileSaveObjectPtr {
	public:
		enum Option {
			TEXT,
			COMPRESSEDTEXT,
			BINARY,
			COMPRESSEDBINARY,
		};
		XFileWriter() {}
		XFileWriter(XFileEnv &env, const _TCHAR *fn, Option option = TEXT) { open(env, fn, option); }
		~XFileWriter() { close(); }
		
		// ファイルオープン
		HRslt open(XFileEnv &env, const _TCHAR *fn, Option option = TEXT);

		bool isOpen() { return (*this) ? true : false; }
		
		void close()
		{
			if(*this) (*this)->Save();
			(*this) = 0;
		}
		
		// @{
		/// 子オブジェクト制作
		XSaveData newData(const GUID &type, ulong size, const void *buf, const char *name = NULL, const GUID *id = NULL)
		{
			XSaveData ret;
			if(*this) (*this)->AddDataObject(type, name, id, size, buf, &ret);
			return ret;
		}
		
		XSaveData newData(const XData &data, ulong size, const void *buf)
		{
			if(*this && !data.isRef()) return newData(data.type(), size, buf, data.name().c_str(), &data.id());
			return XSaveData();
		}

		XSaveData newData(const XData &data)
		{
			if(*this && !data.isRef()) {
				ulong _size;
				const void *_buf = data.data(_size);
				return newData(data, _size, _buf);
			}
			return XSaveData();
		}
		// @}

		/// 子参照オブジェクト制作(必ず失敗する)
		HRslt addRefData(const char *name, const GUID *objguid = NULL)
		{
			return E_NOINTERFACE;
		}

		// 代入演算子を外部にさらす
		XFileWriter &operator =(XFileWriter &src) {
			ID3DXFileSaveObjectPtr::operator=(src);
			return *this;
		}
		XFileWriter &operator =(ID3DXFileSaveObject *src) {
			ID3DXFileSaveObjectPtr::operator=(src);
			return *this;
		}

		/// そのままスルー
		XFileWriter &operator<<(const XData &data)
		{
			XSaveData w = newData(data);
			if(w) for(uint i = 0; i < data.numChildren(); i++) w << data[i];
			return *this;
		}
	};

	///////////////////////////
	// GameCatapult 拡張チャンク

	namespace xext {

		struct LineIndex {
			ulong start, end;
		};

		struct Verticies {
			ulong num;
			Vector verticies[];
		};

		struct Indicies {
			ulong num;
			LineIndex indicies[];
		};

		class Wire {
		public:
			Verticies *vert;
			Indicies *idx;

			Wire();
			Wire(ulong vnum, ulong inum);
			~Wire();

			void read(XData &data);
			XSaveData write(const char *name, XSaver &cur);

		private:
			ulong *buf_;
			uint  size_;
		};

#define GCTPXFILE_TEMPLATES \
       "template Line \
        { \
		    <00E4D09B-7FB7-4e48-838B-EB3A6D452DB4> \
            DWORD start; \
            DWORD end; \
        } \
        template Wire \
        { \
			<F562F616-410D-44d8-A1FF-2ADFB722F4DD> \
            DWORD nVerticies; \
            array Vector verticies[nVerticies]; \
            DWORD nLines; \
            array Line lines[nLines]; \
			[...] \
        } "
		extern const GUID TID_Wire;
		extern const GUID TID_Line;

	}


#ifdef _MSC_VER
# pragma warning(pop)
#endif

} //namespace gctp

#endif //_GCTP_XFILE_HPP_
