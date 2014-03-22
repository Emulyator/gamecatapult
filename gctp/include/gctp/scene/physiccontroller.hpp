#ifndef _GCTP_SCENE_PHYSICCONTROLLER_HPP_
#define _GCTP_SCENE_PHYSICCONTROLLER_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ������񃂃f���N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:42:22
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/object.hpp>

class btCollisionObject;
class btManifoldPoint;
struct btCollisionObjectWrapper;

namespace gctp { namespace scene {

	/** �����V�~���I�u�W�F�N�g���ʊ��N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2008/06/21 17:55:36
	 * Copyright (C) 2008 SAM (T&GG, Org.). All rights reserved.
	 */
	class PhysicController : public Object {
	public:
		virtual bool onContactAdded(btManifoldPoint &cp,
			const btCollisionObjectWrapper *colObj0Wrap, int partId0, int index0,
			const btCollisionObjectWrapper *colObj1Wrap, int partId1, int index1);

		virtual bool onContactProcessed(btManifoldPoint &cp, void *body0, void *body1);
		
		virtual bool onContactDestroyed(void *user_persistent_data);

		GCTP_DECLARE_TYPEINFO;
	};

}} //namespace gctp::scene

#endif //_GCTP_SCENE_PHYSICCONTROLLER_HPP_