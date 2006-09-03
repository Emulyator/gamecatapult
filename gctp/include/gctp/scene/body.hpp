#ifndef _GCTP_SCENE_BODY_HPP_
#define _GCTP_SCENE_BODY_HPP_
/** @file
 * GameCatapult �{�f�B�[�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:42:22
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/skeleton.hpp>
#include <gctp/pointerlist.hpp>
#include <gctp/class.hpp>

namespace gctp { namespace scene {

	class Flesh;
	/** �{�f�B�N���X
	 *
	 * Skeleton��Flesh�̃Z�b�g
	 *
	 * �V�[���t�@�C������\������܂��BFlesh�̏��L�҂ł���_�ɒ��ӁB
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/11/21 17:55:36
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Body : public Skeleton {
	public:
		PointerList<Flesh> &fleshies() { return fleshies_; }
		const PointerList<Flesh> &fleshies() const { return fleshies_; }

		/// ����
		Pointer<Body> dup() const;
		
		/// ���E��
		const Sphere &bs() const { return bs_; }
		/// ���E���̍X�V
		void update();

		/// �`��
		bool draw() const;

	GCTP_DECLARE_CLASS

	private:
		PointerList<Flesh> fleshies_;
		Sphere bs_;
	};

}} //namespace gctp::scene

#endif //_GCTP_SCENE_BODY_HPP_