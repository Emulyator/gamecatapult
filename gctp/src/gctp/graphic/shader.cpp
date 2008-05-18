/** @file
 * GameCatapult �V�F�[�_�[���\�[�X�N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 3:31:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/graphic/shader.hpp>
#include <gctp/extension.hpp>
#include <gctp/buffer.hpp>
#include <gctp/dxcomptrs.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {

	GCTP_IMPLEMENT_TYPEINFO(Shader, Object);

	//���܂��炾���ATypeInfo->Class->Factory�̏���ɒ����K�v����H
	// TypeInfo->Factory->Class ���͂���

	const char *Shader::current_technique_ = 0;

}} // namespace gctp
