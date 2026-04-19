#include "pch.h"
#include "WinCleaner.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "LogUtil.h"

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG_LOGIN, pParent)
{
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_CARDNO, m_editCardNo);
    DDX_Control(pDX, IDC_COMBO_SERVICE, m_comboService);
    DDX_Control(pDX, IDOK, m_btnLogin);
}

BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
    ON_EN_CHANGE(IDC_EDIT_CARDNO, &CLoginDlg::OnEnChangeEditCardno)
    ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CLoginDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetWindowText(_T("工程师专用工具登录系统"));

    // 添加客服选项
    m_comboService.ResetContent();
    CString strService = _T("C盘清理，系统优化，扩容分区，数据迁移，DLL修复，桌面美化，清理流氓软件，重装系统，清理其他盘");
    int nStart = 0;
    int nEnd = strService.Find(_T("，"));
    while (nEnd != -1)
    {
        CString strItem = strService.Mid(nStart, nEnd - nStart);
        m_comboService.AddString(strItem.Trim());
        nStart = nEnd + 1;
        nEnd = strService.Find(_T("，"), nStart);
    }
    CString lastItem = strService.Mid(nStart);
    if (!lastItem.IsEmpty())
        m_comboService.AddString(lastItem.Trim());
    m_comboService.SetCurSel(0);

    m_editCardNo.SetCueBanner(_T("请输入登录密码"));

    CenterWindow();

    return TRUE;
}

BOOL CLoginDlg::OnEraseBkgnd(CDC* pDC)
{
    return CDialogEx::OnEraseBkgnd(pDC);
}

HBRUSH CLoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CLoginDlg::OnEnChangeEditCardno()
{
}

void CLoginDlg::OnBnClickedOk()
{
    CString cardNo;
    m_editCardNo.GetWindowText(cardNo);

    if (cardNo == _T("yyds.2026+"))
    {
        LogMessage(_T("登录验证成功！"));
        CDialogEx::OnOK();
    }
    else
    {
        LogMessage(_T("登录验证失败，请检查输入！"));
        AfxMessageBox(_T("登录验证失败，请检查输入！"), MB_ICONERROR);
    }
}
