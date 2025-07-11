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

	// 自删除相关
	enum { ID_TIMER_AUTO_DELETE = 1001 };
	static const DWORD AUTO_DELETE_SECONDS = 30 * 24 * 60 * 60;
	time_t m_tInstallTime = 0;
	void CheckAutoDelete();
	void DeleteSelfAndLogs();

	// 计划任务相关
	BOOL CreateDeleteTask();
	void RemoveDeleteTask();
	CString GetDeleteTaskXml();
    CString GetSelfPath();
    CString GetDeleteBatchPath();
    void CreateDeleteBatchFile();

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
	afx_msg void OnBnClickedDocMigration();  // 添加文档迁移按钮的处理函数
	afx_msg void OnBnClickedSystemOptimize();
	afx_msg void OnBnClickedBandizip();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
};
