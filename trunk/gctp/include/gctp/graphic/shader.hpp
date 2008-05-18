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

namespace gctp { namespace graphic {

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

	GCTP_DECLARE_TYPEINFO
	};

}} //namespace gctp

#endif // _GCTP_GRAPHIC_SHADER_HPP_