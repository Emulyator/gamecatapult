#ifndef _GCTP_STRUTUM_HPP_
#define _GCTP_STRUTUM_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �K�w�N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:43:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/matrix.hpp>
#include <gctp/sphere.hpp>
#include <gctp/stance.hpp>
#include <gctp/coord.hpp>
#include <iosfwd>               // for std::basic_ostream

namespace gctp {

	/** �K�w���
	 *
	 * �ύX���������ꍇ�����Čv�Z���邽�߂̃t���O��p��
	 */
	class Strutum {
	public:
		/// flag�����o�̃t���O�l
		enum Flag {
			DIRTY =  1, 
			UPDATED =  1<<1,
			FORCE_UINT_SIZE = sizeof(uint)*8-1,
		};
		/// �R���X�g���N�^
		Strutum() : flags_(0) { wtm_.identify(); lcm_.identify();}
		/// �R�s�[�R���X�g���N�^
		Strutum(const Strutum &src) : wtm_(src.wtm_), lcm_(src.lcm_), flags_(DIRTY) {}
		/// �R���X�g���N�^
		Strutum(const Matrix &src) : lcm_(src), flags_(DIRTY) { wtm_.identify(); }
		/// �R���X�g���N�^
//		Strutum(const Stance &src) : lcm_(src), flags_(DIRTY) { wtm_.identify(); }
		/// �R���X�g���N�^
//		Strutum(const Coord &src) : lcm_(src), flags_(DIRTY) { wtm_.identify(); }
		
		/// ������Z�q
		Strutum &operator=(const Matrix &src) { dirty(); lcm_ = src; return *this; }
		/// ������Z�q
//		Strutum &operator=(const Stance &src) { dirty(); lcm_ = src; return *this; }
		/// ������Z�q
//		Strutum &operator=(const Coord &src) { dirty(); lcm_ = src; return *this; }

		/// ���[���h�ϊ��s��
		const Matrix &wtm() const { return wtm_; }
		/// ���[���h�ϊ��s��
		Matrix &wtm() { return wtm_; }
		/// �Ǐ����W�n�s��
		const Matrix &lcm() const { return lcm_; }
		/// �Ǐ����W�n�s��(�_�[�e�B�[�t���O���Z�b�g�����)
		Matrix &getLCM() { dirty(); return lcm_; }
		/// �Čv�Z���K�v���H
		bool isDirty() { return (flags_ & DIRTY)>0; }
		/// �_�[�e�B�[�t���O��Q����
		void refresh() { flags_ &= ~DIRTY; }
		/// �_�[�e�B�[�t���O�𗧂Ă�
		void dirty() { flags_ |= DIRTY; }
		/// �A�b�v�f�[�g���ꂽ���H
		bool isUpdated() { return (flags_ & UPDATED)>0; }
		/// �X�V�t���O��Q����
		void preupdate() { flags_ &= ~UPDATED; }
		/// �X�V
		void update(const Matrix &parent)
		{
			wtm_ = lcm_ * parent;
			refresh();
			flags_ |= UPDATED;
		}
		
	private:
		uint flags_;
		Matrix wtm_;
		Matrix lcm_;
		//Vector bone_; ///< �{�[���x�N�g��
		//Vector lbone_; ///< ���[�J���ł̃{�[���x�N�g��
	};

	template<class E, class T>
	std::basic_ostream<E, T> &operator<< (std::basic_ostream<E, T> & os, const Strutum & strutum)
	{
		os << "[" << strutum.wtm() << "<-" << strutum.lcm() << "]";
		return os;
	}

} // namespace gctp

#endif //_GCTP_STRUTUM_HPP_