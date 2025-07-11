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
END_MESSAGE_MAP()

BOOL CLoginDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 设置窗口标题
    SetWindowText(_T("工程师专用工具登录系统"));

    // 添加客服选项
	CString strService = _T("C盘清理，系统优化，扩容分区，数据迁移，DLL修复，桌面美化，清理流氓软件，重装系统，清理其他盘");
	// 按逗号分割字符串
	CStringArray serviceArray;
	int nStart = 0;
	int nEnd = strService.Find(_T("，"));
	while (nEnd != -1)
	{
		CString strItem = strService.Mid(nStart, nEnd - nStart);
		serviceArray.Add(strItem.Trim());
		nStart = nEnd + 1;
		nEnd = strService.Find(_T("，"), nStart);
	}

    for (int i = 0; i < serviceArray.GetCount(); i++)
    {
        m_comboService.AddString(serviceArray[i]);
	}
    
    m_comboService.SetCurSel(0);

    return TRUE;
}
void CLoginDlg::OnEnChangeEditCardno()
{

}

void CLoginDlg::OnBnClickedOk()
{
    // 获取m_editCardNo值
    CString cardNo;
    m_editCardNo.GetWindowText(cardNo);
    // 可以在这里添加其他验证逻辑，比如格式检查等
    // 例如，如果卡密必须是数字，可以使用正则表达式或其他方法验证
    // 这里仅为示例，实际验证逻辑可能更复杂
    //LogMessage(_T("卡密输入框内容已更改: ") + cardNo);
    // 校验卡密是否正确，与 字体串进行比较
    if (cardNo == _T("SMZ@2025#")) // 假设正确的卡密是"admin@2025#SMZ"
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
