#ifndef _GCTP_GRAPHIC_RENDERTARGET_HPP_
#define _GCTP_GRAPHIC_RENDERTARGET_HPP_
/** @file
 * GameCatapult �����_�[�^�[�Q�b�g�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:43:53
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/class.hpp>
#include <gctp/buffwd.hpp> // for gctp::BufferPtr

namespace gctp { namespace graphic {

	class Texture;

	/// �����_�[�^�[�Q�b�g���\�[�X�N���X
	class RenderTarget : public Rsrc {
	public:
		enum Type {
			NORMAL, // ��Ńe�N�X�`���Ƃ��Ďg�������Ƃ����A���ʂ̎g����
			OFFSCREEN // �t�@�C���ɗ��Ƃ��������p
		};

		struct LockedRect {
			int pitch;
			void *buf;
			LockedRect(int _pitch, void *_buf) : pitch(_pitch), buf(_buf) {}
			bool operator !() { return buf ? false : true; }
			operator bool() { return buf ? true : false; }
		};

		RenderTarget();

		/// �����_�[�^�[�Q�b�g��p��
		HRslt setUp(Type type, int width, int height, int format, int miplevel=1);
		/// �f�v�X�X�e���V���o�b�t�@���ʂɗp��
		HRslt setUpDS(Type type, int width, int height, int format);

		HRslt restore();
		void cleanUp();

		operator dx::IDirect3DSurface *() const { return ptr_; }
		dx::IDirect3DSurface *get() const { return ptr_; }

		/// �J�����g�ɐݒ�
		HRslt begin(uint index = 0);
		/// �����_�[�^�[�Q�b�g�𖳌���
		HRslt end();
		/// �e�N�X�`���ɓ]��
		HRslt resolve();
		/// �e�N�X�`���擾
		const Texture &texture() const
		{
			return *tex_;
		}
		/// �e�N�X�`���擾
		Texture &texture()
		{
			return *tex_;
		}

		Point2 size() const;
		
		enum LockFlag {
			DISCARD = D3DLOCK_DISCARD,
			NO_DIRTY_UPDATE = D3DLOCK_NO_DIRTY_UPDATE,
			NO_SYSLOCK = D3DLOCK_NOSYSLOCK,
			READONLY = D3DLOCK_READONLY
		};
		LockedRect lock(const Rect &rc, uint32_t flag = 0L);
		LockedRect lock(uint32_t flag = 0L);
		HRslt unlock();
		HRslt save(const _TCHAR *fn);
		HRslt save(BufferPtr &ret, D3DXIMAGE_FILEFORMAT format);

	protected:
		dx::IDirect3DSurfacePtr ptr_;
		dx::IDirect3DSurfacePtr backup_;
		Pointer<Texture> tex_;

	private:
		int org_width_;
		int org_height_;
		int org_format_;
		int org_miplevel_;
		int index_for_system_;
		Type type_;

		GCTP_DECLARE_CLASS;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_RENDERTARGET_HPP_