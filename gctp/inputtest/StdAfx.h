// stdafx.h : 標準のシステム インクルード ファイル、
//            または参照回数が多く、かつあまり変更されない
//            プロジェクト専用のインクルード ファイルを記述します。
//

#if !defined(AFX_STDAFX_H__0B36D003_CA8D_42AC_9C2D_E4CB9639889E__INCLUDED_)
#define AFX_STDAFX_H__0B36D003_CA8D_42AC_9C2D_E4CB9639889E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning(disable:4786 4503)
#endif // _MSC_VER > 1000

#include "SmartWin.h"

#define DIRECTINPUT_VERSION (0x0800)
#include <d3d9.h>
#include <d3d9types.h>
#include <d3dx9.h>
#include <dxfile.h>
#include <dinput.h>
#include <gctp/def.hpp>

#if defined(_DEBUG) && !defined(DEBUG)
#define DEBUG	1
#endif

#if _MSC_VER < 1300
#define for if(0); else for
#else
#pragma conform(forScope, on)
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_STDAFX_H__0B36D003_CA8D_42AC_9C2D_E4CB9639889E__INCLUDED_)
