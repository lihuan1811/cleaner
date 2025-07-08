// WinCleanerDlg.h: 头文件
//

#pragma once

#include "afxdialogex.h"
#include "LogUtil.h"

// CWinCleanerDlg 对话框
class CWinCleanerDlg : public CDialogEx
{
// 构造
public:
	CWinCleanerDlg(CWnd* pParent = nullptr);   // 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINCLEANER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

// 实现
protected:
	HICON m_hIcon;
	// 临时目录和文件路径
	CString m_tempPath;
	CString m_zipPath;
	CString m_outDir;
	BOOLEAN is64 = FALSE;
	int currentStep = 0; // 当前步骤索引

	// 生成的消息映射函数
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedDiskClean();
	afx_msg void OnBnClickedBigfileDelete();
	afx_msg void OnBnClickedCacheClean();
	afx_msg void OnBnClickedWechatClean();
	afx_msg void OnBnClickedQqClean();
	afx_msg void OnBnClickedDiskAnalyze();
	afx_msg void OnBnClickedDiskExpand();
	afx_msg void OnBnClickedSoftwareUninstall();
	afx_msg void OnBnClickedKillProcess();
	afx_msg void OnBnClickedPopupBlock();
	afx_msg void OnBnClickedDisableUpdate();
	afx_msg void OnBnClickedCloseSecurityCenter();
	afx_msg void OnBnClickedCloseFirewall();
	afx_msg void OnBnClickedSystemActivate();
	afx_msg void OnBnClickedSearch();
	afx_msg void OnBnClickedSettings();
	afx_msg void OnBnClickedStartupMgr();
	afx_msg void OnBnClickedContextMgr();
	afx_msg void OnBnClickedUninstall();
	afx_msg void OnBnClickedDownloadPe();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};
