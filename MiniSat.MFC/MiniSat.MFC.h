
// MiniSat.MFC.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CMiniSatApp:
// このクラスの実装については、MiniSat.MFC.cpp を参照してください
//

class CMiniSatApp : public CWinApp
{
public:
	CMiniSatApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CMiniSatApp theApp;
