#ifndef _GCTP_SCENE_CAMERA_HPP_
#define _GCTP_SCENE_CAMERA_HPP_
/** @file
 * GameCatapult �J�����N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/types.hpp>
#include <gctp/strutumnode.hpp>
#include <gctp/frustum.hpp>
#include <gctp/tuki.hpp>
#include <gctp/scene/renderer.hpp>

namespace gctp { namespace scene {

	/** �J����
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 8:10:04
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Camera : public Renderer
	{
	public:
		/// �R���X�g���N�^
		Camera();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		/// �J�����ʒu�ݒ�
		void setStance(Stance &src);
		/// �K�w�m�[�h�ɃA�^�b�`
		void attach(Handle<StrutumNode> node);

		/// �J�����ʒu
		const Stance &stance() const { return stance_; }
		/// �j�A�N���b�v
		const float &nearclip() const { return nearclip_; }
		/// �j�A�N���b�v
		float &nearclip() { return nearclip_; }
		/// �t�@�[�N���b�v
		const float &farclip() const { return farclip_; }
		/// �t�@�[�N���b�v
		float &farclip() { return farclip_; }
		/// ����p
		const float &fov() const { return fov_; }
		/// ����p
		float &fov() { return fov_; }
		/// �T�u�E�B���h�E
		const Rectf &subwindow() const { return subwindow_; }
		/// �T�u�E�B���h�E
		Rectf &subwindow() { return subwindow_; }
		/// �A�^�b�`���Ă���m�[�h��Ԃ�
		Handle<StrutumNode> node() const { return node_; }
		/// �Ǝ��ɃE�B���h�E�T�C�Y���w��
		void setWindow(float w, float h)
		{
			window_.x = w;
			window_.y = h;
		}
		/// ���݂̃X�N���[������A�X�y�N�g��ݒ�(�f�t�H���g)
		void fitToScreen() { window_.x = 0; }
		/// ���e�T�C�Y�������_�[�^�[�Q�b�g�ƈ�v���Ă��邩�H
		bool isFittedToScreen() const { return window_.x == 0 || window_.y == 0; }
		/// ���e���[�h���H
		bool isPerspective() const { return fov_ > 0; }

		/// �����[�}�g���N�X��Ԃ�
		Matrix view() const;
		/// ���e�}�g���N�X��Ԃ�
		Matrix projection() const;
		/// ���e�X�N���[���T�C�Y��Ԃ�
		Size2f screen() const;

		const Matrix &viewprojection() const { return viewprojection_; }

		/// �J�������O���t�B�b�N�V�X�e���ɐݒ�
		void setToSystem() const;
		/// �J�����ݒ�J�n
		void begin() const;
		/// �J�����ݒ�I��
		void end() const;

		/// �J�����g�J�����i���̃J�����̎q�����_�����O�v�f�ɂ̂ݗL���j		
		static Camera &current() { return *current_; }

		/// �����[�t���X�^����������
		bool isVisible(const Sphere &bs) const
		{
			return frustum_.isColliding(bs);
		}

		/// ������
		const Frustum &frustum() { return frustum_; }

		/// ��������X�V
		void update();

	protected:
		bool setUp(luapp::Stack &L);
		void activate(luapp::Stack &L);
		void setPosition(luapp::Stack &L);
		int getPosition(luapp::Stack &L);
		void setPosture(luapp::Stack &L);
		int getPosture(luapp::Stack &L);
		void setClip(luapp::Stack &L);
		int getClip(luapp::Stack &L);
		void setFov(luapp::Stack &L);
		int getFov(luapp::Stack &L);

	private:
		Stance stance_;
		float nearclip_;
		float farclip_;
		float fov_;
		Size2f window_;
		Rectf subwindow_;
		Frustum frustum_;
		mutable Matrix viewprojection_;

		Handle<StrutumNode> node_;

		mutable Camera* backup_current_;
		GCTP_TLS static Camera* current_;

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(Camera)
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_CAMERA_HPP_