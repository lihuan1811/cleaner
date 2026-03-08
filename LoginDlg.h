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
    virtual BOOL OnEraseBkgnd(CDC* pDC);
    virtual HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

    DECLARE_MESSAGE_MAP()

    CEdit m_editCardNo;       // 卡密输入框
    CComboBox m_comboService; // 客服选择框
    CButton m_btnLogin;       // 登录按钮

    CFont m_fontTitle;        // 标题字体
    CFont m_fontLabel;        // 标签字体
    CFont m_fontEdit;         // 输入框字体
    CFont m_fontBtn;          // 按钮字体
    CBrush m_bgBrush;         // 背景画刷
    CBrush m_editBrush;       // 输入框画刷

public:
    afx_msg void OnEnChangeEditCardno();
    afx_msg void OnBnClickedOk();
};