/** @file
 * GameCatapult 描画パケットクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: modeltag.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/scene/drawpacket.hpp>
#include <gctp/scene/flesh.hpp>
#include <gctp/scene/world.hpp>
#include <gctp/graphic/shader.hpp>
#include <gctp/graphic/model.hpp>
#include <gctp/graphic/brush.hpp>

#include <gctp/app.hpp>

using namespace std;

namespace gctp { namespace scene {

	void DrawPacketVector::draw() const
	{
		draw(begin(), end());
	}

	void DrawPacketVector::drawOpeque() const
	{
		DrawPacket val;
		val.z = -2;
		draw(begin(), std::lower_bound(begin(), end(), val));
	}

	void DrawPacketVector::drawTranslucent() const
	{
		DrawPacket val;
		val.z = -2;
		draw(std::lower_bound(begin(), end(), val), end());
	}

	void DrawPacketVector::draw(const_iterator first, const_iterator last) const
	{
		bool first_draw = true;
		const graphic::Model *current_model = 0;
		const graphic::Shader *current_shader = 0;
		for(const_iterator i = first; i != last; ++i)
		{
			gctp::Profiling prof("draw a packet");
			if(i->flesh && i->model) {
				if(first_draw || current_shader != i->shader.get()) {
					if(current_shader) current_shader->end();
					current_shader = i->shader.get();
					if(current_shader) current_shader->begin();
				}
				if(first_draw || current_model != i->model.get()){
					if(current_model) {
						current_model->end();
					}
					current_model = i->model.get();
					if(current_model) {
						current_model->begin();
					}
				}
				first_draw = false;
				if(i->model->isSkin()) {
					Pointer<Skeleton> skl = i->flesh->skeleton().lock();
					if(skl) {
						if(i->model->brush()) i->model->brush()->begin(const_cast<graphic::Shader *>(current_shader), *skl);
						if(World::isActive()) World::current().applyLights(i->flesh->bs(), 2, 3, 0);
						i->model->draw(i->subset_no, *skl);
						if(i->model->brush()) i->model->brush()->end();
					}
					// エラー表示したほうがいいのかな
				}
				else if(i->flesh->node()) {
					Pointer<StrutumNode> node = i->flesh->node().lock();
					if(node) {
						if(i->model->brush()) i->model->brush()->begin(const_cast<graphic::Shader *>(current_shader), node->val.wtm());
						if(World::isActive()) World::current().applyLights(i->flesh->bs(), 2, 3, 0);
						i->model->draw(i->subset_no, node->val.wtm());
						if(i->model->brush()) i->model->brush()->end();
					}
					// エラー表示したほうがいいのかな
				}
			}
		}
		if(current_model) current_model->end();
		if(current_shader) current_shader->end();
	}

}} // namespace gctp::scene
