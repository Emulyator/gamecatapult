#ifndef _GCTP_MOVIE_PLAYER_HPP_
#define _GCTP_MOVIE_PLAYER_HPP_
/** @file
 * GameCatapult �v���C���[�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: player.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/def.hpp>
#include <gctp/types.hpp>
#include <gctp/pointer.hpp>

namespace gctp {
	namespace graphic {
		class Texture;
		struct SpriteDesc;
	}
	namespace movie {
		namespace dx {
			class Player;
		}
	}
}

namespace gctp { namespace movie {

	/** �Đ��n���h���N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/20 3:31:53
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Player : public Object {
	public:
		Player();

		HRslt openForTexture(const _TCHAR *name, bool audio_on = true);
		HRslt open(HWND hwnd, const _TCHAR *name, bool audio_on = true);

		bool play(int loop = 1);
		bool isPlaying();
		void stop();
		void release();
		void reset();

		int width();
		int height();
		// ���삵���e�N�X�`����Ԃ�
		Handle<graphic::Texture> getTexture();
		// �e�N�X�`�����g�p����ꍇ�́A���[�r�[�̈�̂t�u��Ԃ�
		Rectf getUVRect();
		// SpriteBuffer���g���ꍇ�́A�X�v���C�g��`��Ԃ�
		graphic::SpriteDesc getDesc();

		HRslt checkStatus();

	private:
		Pointer<dx::Player> ptr_;
	};

}} // namespace gctp

#endif //_GCTP_MOVIE_PLAYER_HPP_