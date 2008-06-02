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
#include <gctp/graphic/shader.hpp>
#include <gctp/graphic/model.hpp>
#include <gctp/graphic/brush.hpp>

using namespace std;

namespace gctp { namespace scene {

	void DrawPacketVector::draw() const
	{
		bool first = true;
		const graphic::Model *current_model = 0;
		const graphic::Shader *current_shader = 0;
		for(DrawPacketVector::const_iterator i = begin(); i != end(); ++i)
		{
			if(i->flesh && i->model) {
				if(first || current_shader != i->shader.get()) {
					if(current_shader) current_shader->end();
					current_shader = i->shader.get();
					if(current_shader) current_shader->begin();
				}
				if(first || current_model != i->model.get()){
					if(current_model) {
						current_model->end();
					}
					current_model = i->model.get();
					if(current_model) {
						current_model->begin();
					}
				}
				first = false;
				if(i->model->isSkin()) {
					Pointer<Skeleton> skl = i->flesh->skeleton().lock();
					if(skl) {
						if(i->model->brush()) i->model->brush()->begin(const_cast<graphic::Shader *>(current_shader), *skl);
						i->model->draw(i->subset_no, *skl);
						if(i->model->brush()) i->model->brush()->end();
					}
					// エラー表示したほうがいいのかな
				}
				else if(i->flesh->node()) {
					Pointer<StrutumNode> node = i->flesh->node().lock();
					if(node) {
						//if(i->model->brush()) i->model->brush()->begin(const_cast<graphic::Shader *>(current_shader));
						i->model->draw(i->subset_no, node->val.wtm());
						//if(i->model->brush()) i->model->brush()->end();
					}
					// エラー表示したほうがいいのかな
				}
			}
		}
		if(current_model) current_model->end();
		if(current_shader) current_shader->end();
	}

}} // namespace gctp::scene
