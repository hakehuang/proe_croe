// Midi.h : Midi DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include <ProUICmd.h>
#include <ProMenuBar.h>
#include <ProUtil.h>
#include <ProMode.h>

#define  USER_MESSAGE_FILE L"DesignToolsmsg.txt"
#define  USER_MENU_FILE L"usermenu.txt"
// CMidiApp
// �йش���ʵ�ֵ���Ϣ������� Midi.cpp
//

class CMidiApp : public CWinApp
{
public:
	CMidiApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
int CreateMidiCurve();
uiCmdAccessState CreateMidiCurveAccess(uiCmdAccessMode access_mode);
