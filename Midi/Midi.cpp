// Midi.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "Midi.h"
#include "CreateMidiCurveDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CMidiApp

BEGIN_MESSAGE_MAP(CMidiApp, CWinApp)
END_MESSAGE_MAP()


// CMidiApp 构造

CMidiApp::CMidiApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMidiApp 对象

CMidiApp theApp;


// CMidiApp 初始化

BOOL CMidiApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
extern "C" __declspec(dllexport)  void user_terminate()
{
}

extern "C" __declspec(dllexport) int user_initialize(
	int argc,
	char *argv[],
	char *version,
	char *build,
	wchar_t errbuf[80])
{
	ProError status;
	uiCmdCmdId cmd_id;
	status = ProMenubarMenuAdd((char*)"DesignTools", (char*)"DesignTools", (char*)"Utilities", PRO_B_TRUE, USER_MENU_FILE);
	status = ProCmdActionAdd("CreateMidiCurve",
		(uiCmdCmdActFn)CreateMidiCurve,
		uiProe2ndImmediate, CreateMidiCurveAccess,
		PRO_B_TRUE, PRO_B_TRUE, &cmd_id);


	status = ProMenubarmenuPushbuttonAdd(
		"DesignTools", "CreateMidiCurve", "CreateMidiCurve", "CreateMidiCurveHelp",
		NULL, PRO_B_TRUE, cmd_id, USER_MENU_FILE);

	return 0;
}
 /* Copyright (C) 2017 Shanghai Yuanxue Machinery Technology Co., Ltd.
 *
 * All rights reserved
 */

int CreateMidiCurve()
{
	CCreateMidiCurveDlg *pDlg = CCreateMidiCurveDlg::GetInstance(AfxGetMainWnd());
	pDlg->ShowWindow(SW_SHOW);
	return 0;
}

uiCmdAccessState CreateMidiCurveAccess(uiCmdAccessMode access_mode)
{
	return ACCESS_AVAILABLE;
}
