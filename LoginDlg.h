#pragma once
#include "afxwin.h"

class CLoginDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CLoginDlg)

public:
    CLoginDlg(CWnd* pParent = nullptr);
    virtual ~CLoginDlg();

    enum { IDD = IDD_DIALOG_LOGIN };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

    CEdit m_editCardNo;       // 卡密输入框
    CComboBox m_comboService; // 客服选择框
    CButton m_btnLogin;       // 登录按钮

    CFont m_font;            // 控件字体
public:
    afx_msg void OnEnChangeEditCardno();
    afx_msg void OnBnClickedOk();
};