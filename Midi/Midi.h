// Midi.h : Midi DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include <ProUICmd.h>
#include <ProMenuBar.h>
#include <ProUtil.h>
#include <ProMode.h>

#define  USER_MESSAGE_FILE L"DesignToolsmsg.txt"
#define  USER_MENU_FILE L"usermenu.txt"
// CMidiApp
// 有关此类实现的信息，请参阅 Midi.cpp
//

class CMidiApp : public CWinApp
{
public:
	CMidiApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
int CreateMidiCurve();
uiCmdAccessState CreateMidiCurveAccess(uiCmdAccessMode access_mode);
