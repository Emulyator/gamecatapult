/** @file
 * GameCatapult クローンボディクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/scene/body.hpp>
#include <gctp/scene/flesh.hpp>
#include <gctp/scene/camera.hpp>

using namespace std;

namespace gctp { namespace scene {

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, Body, Skeleton);

	/** Bodyをインスタンス用にディープコピー返す
	 *
	 * CloneModelを複製し、自分のスケルトンの対応するノードにアタッチして保持。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright &copy; 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/27 16:35:05
	 */
	Pointer<Body> Body::dup() const
	{
		Pointer<Body> ret = new Body();
		if(ret) {
			ret->copy(*this);
			ConstTraverseItr sit = beginTraverse();
			for(TraverseItr dit = ret->beginTraverse(); dit != ret->endTraverse(); ++dit, ++sit) {
				for(PointerList<Flesh>::const_iterator i = fleshies_.begin(); i != fleshies_.end(); ++i) {
					if((*i)->node().lock().get() == &*sit) {
						Pointer<Flesh> flesh = new Flesh;
						if(flesh) {
							flesh->setUp((*i)->model(), ret, &*dit);
							ret->fleshies_.push_back(flesh);
						}
					}
				}
			}
		}
		return ret;
	}

	/** 所有しているFleshの境界球をすべてアップデートし、その総和を、自分の境界球とする。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright &copy; 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/27 16:35:05
	 */
	void Body::update()
	{
		for(PointerList<Flesh>::iterator i = fleshies_.begin(); i != fleshies_.end(); ++i) {
			(*i)->update();
			if(i == fleshies_.begin()) bs_ = (*i)->bs();
			else bs_ |= (*i)->bs();
		}
	}

	bool Body::draw() const
	{
		if(Camera::current().isVisible(bs_)) {
			for(PointerList<Flesh>::const_iterator i = fleshies_.begin(); i != fleshies_.end(); ++i) {
				(*i)->draw();
			}
			return true;
		}
		return false;
	}

	void Body::pushPackets(DrawPacketVector &packets) const
	{
		if(Camera::current().isVisible(bs_)) {
			for(PointerList<Flesh>::const_iterator i = fleshies_.begin(); i != fleshies_.end(); ++i) {
				(*i)->pushPackets(packets);
			}
		}
	}

}} // namespace gctp::scene
