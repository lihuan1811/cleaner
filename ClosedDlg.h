#pragma once
#include "afxdialogex.h"


// ClosedDlg 对话框

class ClosedDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ClosedDlg)

public:
	ClosedDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ClosedDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CLOSED };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	CFont m_staticFont;
	CStatic* m_pStaticText = nullptr;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
