#ifndef _GCTP_SCENE_WORLDRENDERER_HPP_
#define _GCTP_SCENE_WORLDRENDERER_HPP_
/** @file
 * GameCatapult �ȈՌ��ʃm�[�h�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/tuki.hpp>
#include <gctp/handlelist.hpp>
#include <gctp/scene/renderer.hpp>

namespace gctp { namespace scene {

	class World;
	class DefaultSB;

	/** �ȈՌ��ʃm�[�h�N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class WorldRenderer : public Renderer
	{
	public:
		WorldRenderer();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		/// �`��Ώےǉ�
		void add(Handle<World> world);
		/// �`��Ώۍ폜
		void remove(Handle<World> world);

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(WorldRenderer)
	
	protected:
		HandleList<World> worlds_;
		mutable const char *backup_tech_;

		bool setUp(luapp::Stack &L);
		void add(luapp::Stack &L);
		void remove(luapp::Stack &L);

	private:
		Pointer<DefaultSB> dsb_;
	};

}} // namespace gctp::scene

#endif // _GCTP_SCENE_GENERICEFFECTER_HPP_