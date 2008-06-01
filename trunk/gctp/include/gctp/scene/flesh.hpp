#ifndef _GCTP_SCENE_FLESH_HPP_
#define _GCTP_SCENE_FLESH_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ���f���C���X�^���X�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:42:22
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/sphere.hpp>
#include <gctp/strutumnode.hpp>
#include <gctp/class.hpp>
#include <gctp/scene/drawpacket.hpp>

namespace gctp {
	class Skeleton;
	namespace graphic {
		class Model;
	}
}

namespace gctp { namespace scene {

	class ModelMixer;
	/** ���f���C���X�^���X
	 *
	 * Model��`��\�ɂ�������
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/11/21 17:55:36
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Flesh : public Object {
	public:
		/// �Z�b�g�A�b�v
		HRslt setUp(Handle<graphic::Model> model, Handle<StrutumNode> node);
		/// �Z�b�g�A�b�v
		HRslt setUp(Handle<graphic::Model> model, Handle<Skeleton> skl);
		/// �Z�b�g�A�b�v
		HRslt setUp(Handle<graphic::Model> model, Handle<Skeleton> skl, Handle<StrutumNode> node);

		/// �`��
		bool draw() const;
		/// �`��p�P�b�g�o�^
		void push(DrawPacketVector &packets) const;

		/// �i�}�e���A���Ƃ͕ʂ́j�����x�ݒ�B0�Ŕ�\���B�i�V�F�[�_�[�ɂ���ẮA�����Ȃ������j
		float setDissolveRate(float dissolve_rate) { std::swap(dissolve_rate, dissolve_rate_); return dissolve_rate; }
		/// �i�}�e���A���Ƃ͕ʂ́j�����x�ݒ�l�B0�Ŕ�\���B
		float dissolveRate() const { return dissolve_rate_; }

		/// �Q�Ƃ��Ă��郂�f���ւ̃|�C���^
		Handle<graphic::Model> model() const { return model_ ; }
		/// �A�^�b�`���Ă���m�[�h
		Handle<StrutumNode> node() const { return node_; }
		/// ��������X�P���g��
		Handle<Skeleton> skeleton() const { return skl_; }

		/// ���E��
		const Sphere &bs() const { return bs_; }
		/// ���E���̍X�V
		bool update();

	GCTP_DECLARE_CLASS
	private:
		void calcBS();
		Pointer<ModelMixer>		modelmixer_;
		Handle<graphic::Model>	model_;
		Handle<StrutumNode>		node_;
		Handle<Skeleton>		skl_;
		Sphere bs_;
		float dissolve_rate_;
	};

}} //namespace gctp::scene

#endif //_GCTP_SCENE_FLESH_HPP_