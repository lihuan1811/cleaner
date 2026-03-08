#pragma once
#include "afxdialogex.h"
#include "resource.h"

// WinCleanerContentDlg 对话框
class WinCleanerContentDlg : public CDialogEx
{
    DECLARE_DYNAMIC(WinCleanerContentDlg)

public:
    WinCleanerContentDlg(CWnd* pParent = nullptr);
    virtual ~WinCleanerContentDlg();

    enum { IDD = IDD_DIALOG_CONTENT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

    CListCtrl m_fileList;
    CButton m_btnScan;
    CButton m_btnClean;

    void InitializeFileList();

public:
    afx_msg void OnBnClickedScan();
    afx_msg void OnBnClickedClean();
};
