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

    CEdit m_editCardNo;
    CComboBox m_comboService;
    CButton m_btnLogin;

public:
    afx_msg void OnEnChangeEditCardno();
    afx_msg void OnBnClickedOk();
};