// stdafx.h : 標準のシステム インクルード ファイル、
//            または参照回数が多く、かつあまり変更されない
//            プロジェクト専用のインクルード ファイルを記述します。
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning(disable:4786 4503)
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから殆ど使用されないスタッフを除外します

#include <atlbase.h>
#include <atlapp.h>
extern CAppModule _Module;
#define _AtlBaseModule _Module
#include <atlcom.h>
#include <atlres.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <atlcrack.h>
#include <atlmisc.h>
#include <atlddx.h>

#include <d3d9.h>
#include <d3d9types.h>
#include <d3dx9.h>
#include <dxfile.h>
#include <dmusici.h>
#define DIRECTINPUT_VERSION (0x0800)
#include <dinput.h>

#if defined(_DEBUG) && !defined(DEBUG)
#define DEBUG	1
#endif

#ifdef _MSC_VER
#define for if(0); else for
#endif

#include <gctp/def.hpp>

// TODO: プログラムで必要なヘッダー参照を追加してください。

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
