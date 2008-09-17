#ifndef _GCTP_SCENE_SPEAKER_HPP_
#define _GCTP_SCENE_SPEAKER_HPP_
/** @file
 * GameCatapult �X�s�[�J�[�i�R�c�T�E���h�����j�I�u�W�F�N�g�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/tuki.hpp>
#include <gctp/strutumnode.hpp>
#include <gctp/audio/speaker.hpp>
#include <gctp/audio/player.hpp>

namespace gctp { namespace scene {

	class World;

	/** �V�[�����̉����I�u�W�F�N�g
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 2:11:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Speaker : public Object
	{
	public:
		/// ������
		enum Type {
			AMBIENT,	///< ���w���̊���
			OMNI,		///< ���w��������
			SPOT		///< �X�|�b�g����
		};

		float  range;
		float  min_distance;
		float  theta;
		float  phi;
		
		Speaker();

		void set(const audio::Player &ambient);
		void set(const audio::Speaker &omni_or_spot);

		/// �V�K�Ƀm�[�h������
		void newNode();
		/// �����m�[�h�ɃA�^�b�`
		void attach(Handle<StrutumNode> node);

		void enter(World &world);
		void exit(World &world);

		/// ������
		Type type() const { return type_; }

		/// �A�^�b�`���Ă���m�[�h
		Handle<StrutumNode> node() const { return node_; }
		/// ���E��
		const Sphere &bs() const { return bs_; }
		
		bool play(int times = 1);
		bool isPlaying();
		void stop();
		void release();
		void reset();

		/// �I�[�f�B�I�f�o�C�X�ɉ��������Z�b�g
		void apply() const;
		/// ���E���̍X�V
		void update();
		
	protected:
		audio::Speaker speaker_;
		audio::Player player_;
		Handle<StrutumNode> node_;
		Pointer<StrutumNode> own_node_;
		Sphere bs_;
		
		bool setUp(luapp::Stack &L);
		void newNode(luapp::Stack &L);
		void attach(luapp::Stack &L);
		void enter(luapp::Stack &L);
		void exit(luapp::Stack &L);
		int isPlaying(luapp::Stack &L);
		int play(luapp::Stack &L);
		int stop(luapp::Stack &L);
		void setSource(luapp::Stack &L);
		int getType(luapp::Stack &L);
		void setPosition(luapp::Stack &L);
		int getPosition(luapp::Stack &L);
		void setPosture(luapp::Stack &L);
		int getPosture(luapp::Stack &L);
		void setRange(luapp::Stack &L);

	private:
		Type   type_;

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(Speaker);
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_SPEAKER_HPP_