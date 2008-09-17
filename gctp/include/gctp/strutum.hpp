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
	 */
	class Strutum {
		/// flag�����o�̃t���O�l
		enum Flag {
			DIRTY			=  1,
			UPDATED			=  1<<1,
			INDEPENDENT		=  1<<2,
			CONTINUITY		=  1<<3,
			FORCE_UINT_SIZE	= sizeof(uint)*8-1,
		};
	public:
		/// �R���X�g���N�^
		Strutum() : bone_(VectorC(0,0,0)), flags_(0) { wtm_.identify(); lcm_.identify();}
		/// �R�s�[�R���X�g���N�^
		Strutum(const Strutum &src) : wtm_(src.wtm_), lcm_(src.lcm_), bone_(src.bone_), flags_(DIRTY) {}
		/// �R���X�g���N�^
		Strutum(const Matrix &src) : lcm_(src), bone_(VectorC(0,0,0)), flags_(DIRTY) { wtm_.identify(); }
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
		/// �{�[���x�N�g��
		const Vector &bone() const { return bone_; }
		/// �{�[���x�N�g��
		Vector &bone() { return bone_; }
		/// �Čv�Z���K�v���H
		bool isDirty() const { return (flags_ & DIRTY) > 0; }
		/// �_�[�e�B�[�t���O��Q����
		void refresh() { flags_ &= ~DIRTY; }
		/// �_�[�e�B�[�t���O�𗧂Ă�
		void dirty() { flags_ |= DIRTY; }
		/// �A�b�v�f�[�g���ꂽ���H
		bool isUpdated() const { return (flags_ & UPDATED) > 0; }
		/// �X�V�t���O��Q����
		void preupdate() { flags_ &= ~UPDATED; }
		/// �X�V
		void update(const Matrix &parent)
		{
			if(!isDependent()) {
				if(isContinuos()) prev_wtm_ = wtm_;
				wtm_ = lcm_ * parent;
				if(!isContinuos()) prev_wtm_ = wtm_;
				flags_ = UPDATED|CONTINUITY;
			}
		}
		/// wtm�������Z�b�g
		void updateWTM(const Matrix &wtm)
		{
			if(isContinuos()) prev_wtm_ = wtm_;
			wtm_ = wtm;
			if(!isContinuos()) prev_wtm_ = wtm_;
			flags_ = UPDATED|INDEPENDENT|CONTINUITY;
		}

		/// �O�񃏁[���h�ϊ��s��
		const Matrix &prevWTM() const { return isContinuos() ? prev_wtm_ : wtm_; }
		/// �O�񃏁[���h�ϊ��s��
		Matrix &prevWTM() { return isContinuos() ? prev_wtm_ : wtm_; }
		/// �A�����H(�܂�A�O��wtm�ɗL���Ȓl�������Ă��邩�H)
		bool isContinuos() const { return (flags_ & CONTINUITY) > 0; }
		/// �A���t���O��Q����
		void discreet() { flags_ &= ~CONTINUITY; }
		/// �Ɨ����H
		bool isDependent() { return (flags_ & INDEPENDENT) > 0; }
		/// �Ɨ��t���O��Q����
		void depend() { flags_ &= ~INDEPENDENT; }

	private:
		Matrix prev_wtm_;
		Matrix wtm_;
		Matrix lcm_;
		Vector bone_; // �{�[���x�N�g��
		uint flags_;
	};

	template<class E, class T>
	std::basic_ostream<E, T> &operator<< (std::basic_ostream<E, T> & os, const Strutum & strutum)
	{
		os << "[" << strutum.wtm() << "<-" << strutum.lcm() << "]";
		return os;
	}

} // namespace gctp

#endif //_GCTP_STRUTUM_HPP_