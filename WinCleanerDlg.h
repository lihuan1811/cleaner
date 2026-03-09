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

	// 公告栏字体
	CFont m_fontNotice;

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

	// 常用清理功能
	afx_msg void OnBnClickedDiskClean();
	afx_msg void OnBnClickedBigfileDelete();
	afx_msg void OnBnClickedCacheClean();
	afx_msg void OnBnClickedWechatClean();
	afx_msg void OnBnClickedQqClean();
	afx_msg void OnBnClickedDiskAnalyze();

	// 系统维护工具
	afx_msg void OnBnClickedMemoryOptimize();
	afx_msg void OnBnClickedDiskDefrag();
	afx_msg void OnBnClickedSoftwareUninstall();
	afx_msg void OnBnClickedSysSettingsOpt();
	afx_msg void OnBnClickedSysExtremeOpt();
	afx_msg void OnBnClickedSystemOptimize();

	// 系统安全与激活
	afx_msg void OnBnClickedSystemActivate();
	afx_msg void OnBnClickedPopupBlock();
	afx_msg void OnBnClickedKillProcess();
	afx_msg void OnBnClickedDisableUpdate();
	afx_msg void OnBnClickedCloseSecurityCenter();
	afx_msg void OnBnClickedCloseFirewall();

	// 其它功能
	afx_msg void OnBnClickedSearch();
	afx_msg void OnBnClickedStartupMgr();
	afx_msg void OnBnClickedDownloadPe();
	afx_msg void OnBnClickedDocMigration();
	afx_msg void OnBnClickedDirectxFix();
	afx_msg void OnBnClickedGameRuntime();

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
};
