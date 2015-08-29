GameCatapult is a library, middleware, framework for constructing game authoring environment.
It contains Lua wrapper for C++ and C++ class binding, easy to build game authoring environment with Lua scripting and extended open scene graph file(.x or COLLADA file). This project forcuses to design a smart and flexible 'Game Object' model.
Currentry, this can render and animate skinned mesh with Direct3D API and blend multiple motion and these are all commanded from Lua script.

GameCatapultはオープンソース・フリーなゲーム開発環境構築ミドルウェアです。Luaスクリプトによるオーサリングを実現するC++ラッパーを備え、.xファイルまたはCOLLADAに独自の拡張を施してシーングラフを完全にデータとして表現する手段を提供します。

現在はDirectXに依存しています。（OpenGL/OpenAL/SDLも選択できるようにする予定です）
あるいは、グラフィックエンジンにはIrrlichtなどを採用して、自前で用意するのをやめるかもしれません。
物理エンジンにはODEを採用する予定です。



