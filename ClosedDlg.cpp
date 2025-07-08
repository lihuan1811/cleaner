// ClosedDlg.cpp: 实现文件
//

#include "pch.h"
#include "WinCleaner.h"
#include "afxdialogex.h"
#include "ClosedDlg.h"
#include "WinCleanerUtils.h"


// ClosedDlg 对话框

IMPLEMENT_DYNAMIC(ClosedDlg, CDialogEx)

ClosedDlg::ClosedDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CLOSED, pParent)
{

}

ClosedDlg::~ClosedDlg()
{
}

void ClosedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ClosedDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &ClosedDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// ClosedDlg 消息处理程序

BOOL ClosedDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置静态文本字体为10pt
	m_pStaticText = (CStatic*)GetDlgItem(IDC_STATIC);
	if (m_pStaticText)
	{
		LOGFONT lf = { 0 };
		CFont* pOldFont = m_pStaticText->GetFont();
		if (pOldFont)
		{
			pOldFont->GetLogFont(&lf);
		}
		lf.lfHeight = -MulDiv(10, GetDeviceCaps(::GetDC(m_pStaticText->m_hWnd), LOGPIXELSY), 72); // 10pt
		_tcscpy_s(lf.lfFaceName, _T("MS Shell Dlg"));
		m_staticFont.CreateFontIndirect(&lf);
		m_pStaticText->SetFont(&m_staticFont);
	}

	return TRUE;
}

void ClosedDlg::OnBnClickedOk()
{
	// 弹出我的电脑
	OpenThisPC();

	// 删除当前程序自身和相对logs文件目录
	TCHAR szModule[MAX_PATH] = { 0 };
	if (GetModuleFileName(NULL, szModule, MAX_PATH))
	{
		// 构造删除自身的命令
		CString strCmd;
		CString strModule(szModule);

		// 构造logs目录路径
		CString strExeDir = strModule.Left(strModule.ReverseFind(_T('\\')));
		CString strLogsDir = strExeDir + _T("\\logs");

		// 删除logs目录（递归）
		CString strDelLogsCmd;
		strDelLogsCmd.Format(_T("rd /s /q \"%s\""), strLogsDir);

		// 删除自身（延迟删除）
		CString strDelSelfCmd;
		strDelSelfCmd.Format(_T("cmd /c ping 127.0.0.1 -n 2 >nul && del /f /q \"%s\""), strModule);

		// 合并命令
		strCmd.Format(_T("cmd /c %s && %s"), strDelLogsCmd, strDelSelfCmd);

		// 启动删除命令
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;
		if (CreateProcess(NULL, strCmd.GetBuffer(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}

	CDialogEx::OnOK();
}
