/** @file
 * GameCatapult �O���t�B�b�N���\�[�X���N���X
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: rsrc.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/graphic/rsrc.hpp>
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/stateblock.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {
	
	GCTP_IMPLEMENT_CLASS_NS(gctp, Rsrc, Object);

	Rsrc::Rsrc()
	{
		if(!device().isOpen()) {
			GCTP_TRACE_MSGBOX("graphic::Device���Z�b�g�A�b�v����Ă��Ȃ����A\n"
				"�J�����g�f�o�C�X���ݒ肳��Ă��Ȃ���ԂŃf�o�C�X�Ǘ����\�[�X�����삳��Ă��܂��B\n"
				"�ÓI�I�u�W�F�N�g�Ƃ��ăf�o�C�X�Ǘ����\�[�X�����������ꍇ�́A\n"
				"�|�C���^�[�ɂ���Ȃǂ��āA�f�o�C�X�̃I�[�v����ɐ��삳���悤�ɂ��Ă��������B");
		}
		else device().registerRsrc(Handle<Rsrc>(this));
	}

	void Rsrc::cleanUp()
	{
	}

	HRslt Rsrc::restore()
	{
		return S_OK;
	}

}} // namespace gctp
