#ifndef _GCTP_SCENE_RENDERINGNODE_HPP_
#define _GCTP_SCENE_RENDERINGNODE_HPP_
/** @file
 * GameCatapult レンダリングノードクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:43:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/scene/renderer.hpp>
#include <gctp/tree.hpp>

namespace gctp { namespace scene {

	/// 階層ノード
	typedef TreeNode< Pointer<Renderer> > RenderingNode;

}} // namespace gctp::scene

#endif //_GCTP_SCENE_RENDERINGNODE_HPP_