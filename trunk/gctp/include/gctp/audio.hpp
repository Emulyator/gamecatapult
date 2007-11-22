#ifndef _GCTP_AUDIO_HPP_
#define _GCTP_AUDIO_HPP_
/** @file
 * GameCatapult DirectX Audio�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: audio.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/def.hpp>
#include <gctp/db.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/audio/player.hpp>
#include <gctp/audio/clip.hpp>
#include <gctp/audio/track.hpp>
#include <boost/scoped_ptr.hpp>

namespace gctp { namespace audio {

	namespace dx {
		class Device;
	}

	/** �I�[�f�B�I�f�o�C�X�N���X
	 *
	 * �~�L�T�[/�}�X�^�[�o�̓I�u�W�F�N�g�Ƃ��Ē��ۉ�\n
	 * DirectSound������킷�N���X�B\n
	 * �f�o�C�X�Ɉˑ����郊�\�[�X�̃f�[�^�x�[�X�����˂�B\n
	 * ���f�o�C�X�������l�����Ȃ��Ă���DirectSound��DXRsrcDB���m�ۂ���Ӗ��́AAudio���\�[�X��
	 * ���ׂĊJ�����Ă���o�Ȃ���Music�̊J�����o���Ȃ�����B\n
	 */
	class Device {
	public:
		Device();
		~Device();
		HRslt open(HWND hwnd, bool play_focus_lost = true, bool priority = true);
		void close();

		/// �I�[�v���ς݂��H
		bool isOpen() const;

		/** �J�����g�I�[�f�B�I�f�o�C�X�̎擾
		 *
		 * �J�����g�f�o�C�X��Ԃ��B
		 */
		inline static Device *getCurrent() { return current_; }
		/** �J�����g�I�[�f�B�I�f�o�C�X��ݒ�
		 *
		 * �J�����g�f�o�C�X��ݒ肷��B
		 */
		void setCurrent() { current_ = this; }

		/** @defgroup AudioPlayback �Đ��֘A
		 */
		/* @{ */
		/** �v���C���[�擾
		 * @{ */
		Player ready(const _TCHAR *fname, bool streaming = true);
		Player ready(Clip &clip, bool streaming = true);
		/* @} */
		/* @} */

		/** @defgroup AudioMasterOperation �}�X�^�[�o�͊֘A */
		/* @{ */
		/// �}�X�^�[�{�����[���ݒ�
		void setVolume(float volume);
		/// �}�X�^�[�{�����[��
		float volume();
		/// �}�X�^�[�o�͌`���ݒ�
		HRslt setFormat(int channel_num, int freq, int bitrate);
		/// �}�X�^�[�o�͌`���擾
		HRslt getFormat(int &channel_num, int &freq, int &bitrate);
		/* @} */

		dx::Device &impl() const { return *impl_.get(); }

	private:
		boost::scoped_ptr<dx::Device> impl_;
		GCTP_TLS static Device* current_;	///< �J�����g�f�o�C�X�C���X�^���X
	};

	/** Audio�̎擾
	 *
	 * �J�����g�f�o�C�X�̃C���X�^���X��Ԃ��B
	 */
	inline Device &device() { return *Device::getCurrent(); }

}} // namespace gctp

#endif //_GCTP_AUDIO_HPP_