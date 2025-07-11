// WinCleaner.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "WinCleaner.h"
#include "WinCleanerDlg.h"
#include "LoginDlg.h"
#include "WinCleanerUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinCleanerApp

BEGIN_MESSAGE_MAP(CWinCleanerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWinCleanerApp 构造

CWinCleanerApp::CWinCleanerApp()
{
	m_bLoggedIn = FALSE;  // 初始化为未登录状态

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CWinCleanerApp 对象

CWinCleanerApp theApp;


// CWinCleanerApp 初始化

BOOL CWinCleanerApp::InitInstance()
{
	// 如果应用程序存在以下情况，Windows XP 上需要 InitCommonControlsEx()
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	if (!IsAdmin()) {
		if (!RestartAsAdmin()) {
			// 以管理员权限重新启动成功，退出当前实例
			AfxMessageBox(_T("无法以管理员权限重新启动应用程序。"));
			return FALSE;  // 退出当前应用程序实例
		} else {
			return FALSE;  // 无法重新启动，退出应用程序
		}
	}

	// 显示登录对话框
	CLoginDlg dlg;
	INT_PTR nResponse = dlg.DoModal();
	
	if (nResponse == IDOK)
	{
		m_bLoggedIn = TRUE;  // 登录成功
	}
	else 
	{
		return FALSE;  // 登录失败或取消,退出应用程序
	}

	// 登录成功后显示主窗口
	CWinCleanerDlg dlgMain;
	m_pMainWnd = &dlgMain;
	nResponse = dlgMain.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: 处理主窗口关闭...
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

