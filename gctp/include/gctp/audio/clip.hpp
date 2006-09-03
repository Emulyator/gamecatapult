#ifndef _GCTP_AUDIO_CLIP_HPP_
#define _GCTP_AUDIO_CLIP_HPP_
/** @file
 * GameCatapult �I�[�f�B�I�N���b�v�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/25 19:11:31
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/def.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/class.hpp>
#include <boost/scoped_ptr.hpp>

namespace gctp { namespace audio {
	namespace dx{
		class WavFile;
	}

	/** �I�[�f�B�I�N���b�v�N���X
	 *
	 * �E�F�[�u�t�@�C���Ɗy�ȏ��(���[�v�|�C���g�⎩�R�ɐ؂�ւ��\�ȃ|�C���g�ABPM)
	 */
	class Clip : public Object {
	public:
		Clip();
		~Clip();
		HRslt setUp(const char *name);
		void tearDown();
		GCTP_DECLARE_FACTORY;
	private:
		boost::scoped_ptr<dx::WavFile> wav_;
	};

}} // namespace gctp

#endif //_GCTP_AUDIO_CLIP_HPP_