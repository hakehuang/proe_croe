// Midi.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "Midi.h"
#include "CreateMidiCurveDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ��  ����ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CMidiApp

BEGIN_MESSAGE_MAP(CMidiApp, CWinApp)
END_MESSAGE_MAP()


// CMidiApp ����

CMidiApp::CMidiApp()
{
	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CMidiApp ����

CMidiApp theApp;


// CMidiApp ��ʼ��

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
