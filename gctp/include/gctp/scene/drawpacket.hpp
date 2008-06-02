#ifndef _GCTP_SCENE_DRAWPACKET_HPP_
#define _GCTP_SCENE_DRAWPACKET_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �`��p�P�b�g�\���̃w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:42:22
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointer.hpp>
#include <vector>

namespace gctp {
	
	namespace graphic {
		class Model;
		class Shader;
	}

	namespace scene {

		class Flesh;

		/// �`��p�P�b�g
		struct DrawPacket {
			Handle<graphic::Shader> shader;
			Handle<graphic::Model> model;
			Handle<Flesh> flesh;
			uint subset_no;
			uint z;
			bool operator<(const DrawPacket &rhs) const
			{
				if(z == rhs.z) {
					if(shader.get() == shader.get()) {
						 return model.get() < rhs.model.get();
					}
					else return shader.get() < rhs.shader.get();
				}
				else return z > rhs.z;
			}
		};

		/// �`��p�P�b�g�x�N�^
		class DrawPacketVector : public std::vector<DrawPacket>
		{
		public:
			void sort()
			{
				std::sort(begin(), end());
			}
			void draw() const;
		};
	}

} // namespace gctp

#endif // _GCTP_SCENE_DRAWPACKET_HPP_