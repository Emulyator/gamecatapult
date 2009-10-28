#ifndef _GCTP_XFILE_HPP_
#define _GCTP_XFILE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult XFile����T�|�[�g�N���X
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

	/// X�t�@�C����
	class XFileEnv : public ID3DXFilePtr {
	public:
		XFileEnv();
		HRslt registerDefaultTemplate();
		HRslt registerSkinTemplate();
		HRslt registerExTemplate();
		HRslt registerTemplate(const class XFileReader &is);
		HRslt registerTemplate(const void *def, std::size_t size);
		HRslt registerTemplate(const char *def);

		// ������Z�q���O���ɂ��炷
		XFileEnv &operator =(XFileEnv &src) {
			ID3DXFilePtr::operator=(src);
			return *this;
		}
		XFileEnv &operator =(ID3DXFile *src) {
			ID3DXFilePtr::operator=(src);
			return *this;
		}
	};

	/** @name �A�j���[�V�����f�[�^�\����
	 * X�t�@�C������̓ǂݎ��p�^��`
	 */
	/**@{*/
	/// TimedFloatKey�`�����N�̌^
	template<typename _T>
	struct XKey {
		DWORD	time;
		DWORD	length; // ���_�u�����[�h���T�C�Y
		_T		val;
	};

	enum XKeyType {
		XKEY_POSTURE	= 0,
		XKEY_SCALE		= 1,
		XKEY_POSITION	= 2,
		XKEY_YPR		= 3,	// �R���䂾���c�I�C���[�p�p�������̂��ȁH
		XKEY_MATRIX		= 4		// SkinnedMesh�̃\�[�X�ł�4�ɂȂ��Ă���c�c���A�w���v�ł͂R�� 4�ŃC�C�炵���c�c
	};

	struct XKeys {
		XKeyType	type;
		DWORD		num;
	};

	/// AnimationKey�`�����N
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

	/// MeshMaterialList�̌^
	struct XMeshMaterialList {
		DWORD mtrlnum;
		DWORD mtrlnonum;
		DWORD mtrlno[];
	};

	/// Material�̌^
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

	/// D3DX�t�@�C���f�[�^�n���h��
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

		/// �`�����N���擾
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

		/// �`�����N��GUID���擾
		GUID id() const {
			GUID ret;
			(*this)->GetId(&ret);
			return ret;
		}
		
		/// �^�C�v�擾
		GUID type() const {
			GUID ret;
			if(*this) (*this)->GetType(&ret);
			return ret;
		}
		
		/** �Q�Ƃ��H
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/28 18:05:03
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		bool isRef() const
		{
			return (*this)->IsReference() == TRUE;
		}

		/** �q�f�[�^�̐����擾
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
		/** �q�f�[�^���擾
		 *
		 * 0�`size()�܂ł̐����w�肷��
		 * @return ID3DXFileObject�̃|�C���^�B���s�����ꍇ�́ANULL���Ԃ�
		 */
		XData getChild(uint i) const {
			XData ret;
			if(*this) (*this)->GetChild(i, &ret);
			return ret;
		}
		XData operator[](uint i) const { return getChild(i); }
		// @}
		
		/// ���b�N����
		void unlock() const {
			const_cast<XData *>(this)->lock_ = 0;
		}

		/// ���b�N
		const void *lock() const {
			unlock();
			const_cast<XData *>(this)->lock_ = new ScopedLock(*this);
			if(lock_) return lock_->data();
			return 0;
		}
		
		/// �f�[�^�擾
		const void *data(ulong &size) const {
			if(!lock_) lock();
			if(lock_) {
				size = lock_->size();
				return lock_->data();
			}
			return 0;
		}

		/// �f�[�^�T�C�Y�擾
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
		
		/// �f�[�^�擾
		const void *data() const {
			if(!lock_) lock();
			if(lock_) return lock_->data();
			return 0;
		}

		// ������Z�q���O���ɂ��炷
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

	/// X�t�@�C���ǂݍ���
	class XFileReader : public ID3DXFileEnumObjectPtr {
	public:
		XFileReader() {}
		explicit XFileReader(const _TCHAR* fn) { open(fn); }
		XFileReader(XFileEnv &env, const _TCHAR* fn) { open(env, fn); }
		
		// �t�@�C���I�[�v��
		HRslt open(XFileEnv &env, const _TCHAR *fn);
		HRslt open(XFileEnv &env, const void *memory, size_t size);
		HRslt open(const _TCHAR *fn);
		HRslt open(const void *memory, size_t size);

		bool isOpen() { return (*this) ? true : false; }
		
		void close()
		{
			(*this) = 0;
		}

		/** �q�f�[�^�̐����擾
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
		/** �q�f�[�^���擾
		 *
		 * 0�`size()�܂ł̐����w�肷��
		 * @return ID3DXFileObject�̃|�C���^�B���s�����ꍇ�́ANULL���Ԃ�
		 */
		XData getChild(uint i) const {
			XData ret;
			if(*this) (*this)->GetChild(i, &ret);
			return ret;
		}
		XData operator[](uint i) const { return getChild(i); }
		// @}
		
		// �f�[�^�擾
		// ���s�����ꍇ�́ANULL���Ԃ�
		XData find(const GUID &guid) const {
			XData ret;
			if(*this) (*this)->GetDataObjectById(guid, &ret);
			return ret;
		}
		
		// �f�[�^�擾
		// ���s�����ꍇ�́ANULL���Ԃ�
		XData find(const char *name) const {
			XData ret;
			if(*this) (*this)->GetDataObjectByName(name, &ret);
			return ret;
		}
		
		// ������Z�q���O���ɂ��炷
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
	/// D3DX�t�@�C���Z�[�o���ۃn���h��
	class XSaver {
	public:
		// @{
		/// �q�I�u�W�F�N�g����
		virtual XSaveData newData(const GUID &type, ulong size, const void *buf, const char *name = NULL, const GUID *id = NULL) = 0;
		virtual XSaveData newData(const XData &data, ulong size, const void *buf) = 0;
		virtual XSaveData newData(const XData &data) = 0;
		// @}

		/// �q�Q�ƃI�u�W�F�N�g����
		virtual HRslt addRefData(const char *name, const GUID *objguid = NULL) = 0;

		/// ���̂܂܃X���[
		XSaver &operator<<(const XData &data);
	};

	/// D3DX�t�@�C���Z�[�u�f�[�^�n���h��
	class XSaveData : public XSaver, public ID3DXFileSaveDataPtr {
	public:
		/// �`�����N���擾
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
		/// �`�����N��GUID���擾
		GUID id() const {
			GUID ret;
			(*this)->GetId(&ret);
			return ret;
		}
		
		/// �^�C�v�擾
		GUID type() const {
			GUID ret;
			if(*this) (*this)->GetType(&ret);
			return ret;
		}
		
		// ������Z�q���O���ɂ��炷
		XSaveData &operator =(XSaveData &src) {
			ID3DXFileSaveDataPtr::operator=(src);
			return *this;
		}
		XSaveData &operator =(ID3DXFileData *src) {
			ID3DXFileSaveDataPtr::operator=(src);
			return *this;
		}

		// @{
		/// �q�I�u�W�F�N�g����
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

		/// �q�Q�ƃI�u�W�F�N�g����
		HRslt addRefData(const char *name, const GUID *objguid = NULL)
		{
			if(*this) return (*this)->AddDataReference(name, objguid);
			return E_POINTER;
		}

		/// ���̂܂܃X���[
		XSaveData &operator<<(const XData &data)
		{
			XSaveData w = newData(data);
			if(w) for(uint i = 0; i < data.numChildren(); i++) w << data[i];
			return *this;
		}
	};

	/// X�t�@�C�������o��
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
		
		// �t�@�C���I�[�v��
		HRslt open(XFileEnv &env, const _TCHAR *fn, Option option = TEXT);

		bool isOpen() { return (*this) ? true : false; }
		
		void close()
		{
			if(*this) (*this)->Save();
			(*this) = 0;
		}
		
		// @{
		/// �q�I�u�W�F�N�g����
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

		/// �q�Q�ƃI�u�W�F�N�g����(�K�����s����)
		HRslt addRefData(const char *name, const GUID *objguid = NULL)
		{
			return E_NOINTERFACE;
		}

		// ������Z�q���O���ɂ��炷
		XFileWriter &operator =(XFileWriter &src) {
			ID3DXFileSaveObjectPtr::operator=(src);
			return *this;
		}
		XFileWriter &operator =(ID3DXFileSaveObject *src) {
			ID3DXFileSaveObjectPtr::operator=(src);
			return *this;
		}

		/// ���̂܂܃X���[
		XFileWriter &operator<<(const XData &data)
		{
			XSaveData w = newData(data);
			if(w) for(uint i = 0; i < data.numChildren(); i++) w << data[i];
			return *this;
		}
	};

	///////////////////////////
	// GameCatapult �g���`�����N

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
