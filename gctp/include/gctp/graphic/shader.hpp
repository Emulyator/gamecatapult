#ifndef _GCTP_GRAPHIC_SHADER_HPP_
#define _GCTP_GRAPHIC_SHADER_HPP_
/** @file
 * GameCatapult �V�F�[�_�[�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:43:53
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/class.hpp>

namespace gctp {
	class Skeleton;
}

namespace gctp { namespace graphic {

	// ����ς�Brush�ɖ߂������Ȃ��B
	//!!
	// �V�F�[�_�[�ƃu���b�V���͕����Ȃ��Ⴞ�߁B
	// Brush�ɂ������Ȗ��O�ȊO�̈Ӌ`���o�Ă����B
	// Shader���V�F�[�_���̂��́B
	// Brush=���f�����Ƃ̃C���X�^���X�f�[�^�B
	// ���܂�ModelDetail=Brush

	class Model;

	/// �V�F�[�_�[���\�[�X�N���X
	class Shader : public Rsrc {
	public:
		virtual HRslt setUp(const _TCHAR *fname) = 0;

		/// �w��̃e�N�j�b�N�����邩�H
		virtual bool hasTechnique(const char *name) const = 0;
		/// �K�p�J�n
		virtual HRslt begin() const = 0;
		/// �K�p�I��
		virtual HRslt end() const = 0;
		/// �w��̃p�X��K�p�J�n
		virtual HRslt beginPass(uint passno) const = 0;
		/// �w��̃p�X��K�p�I��
		virtual HRslt endPass() const = 0;
		/// �p�X����Ԃ��ibegin~end�̊Ԃ̂ݗL���j
		virtual uint passnum() const = 0;

#if 0
		// �����炢��Ȃ�����
		// �ǂ��ɉ������邩�A��Brush�������Ă�A���Ă��Ƃł����񂶂�H

		/** ���f���C���X�^���X���Ƃł͂Ȃ��A�p�����[�^�̐ݒ�
		 *
		 * ViewProjectionMatrix��A���r�G���g�Ȃ�
		 *
		 * begin�O�ɐݒ�\�ȃp�����[�^
		 */
		virtual void setGlobalParameter() = 0;
		/** ���f���C���X�^���X���Ƃ̃p�����[�^�̐ݒ�
		 *
		 * ModelMatrix�iWorldMatrix�j�⓮�I���C�g�Ȃ�
		 *
		 * �`�撼�O�ɐݒ肷��p�����[�^
		 */
		virtual void setLocalParameter(const Model &model, int subsetno, const Matrix &mat) = 0;
		/** ���f���C���X�^���X���Ƃ̃p�����[�^�̐ݒ�
		 *
		 * ModelMatrix�iWorldMatrix�j�⓮�I���C�g�Ȃ�
		 *
		 * �`�撼�O�ɐݒ肷��p�����[�^
		 */
		virtual void setLocalParameter(const Model &model, int subsetno, const Skeleton &skel) = 0;
#endif

		// �Ƃ肠�����B�B�B
		// NULL�ɂ���Ɛ擪�̃e�N�j�b�N�ŕ`��
		static void setTechnique(const char *techname)
		{
			current_technique_ = techname;
		}

		static const char *getTechnique()
		{
			return current_technique_;
		}

	protected:
		static const char *current_technique_;
		// ����̓����_�[�c���[�����ׂ�

	GCTP_DECLARE_TYPEINFO
	};

}} //namespace gctp

#endif // _GCTP_GRAPHIC_SHADER_HPP_