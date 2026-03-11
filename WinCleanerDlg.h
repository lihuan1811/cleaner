#pragma once
#include "afxdialogex.h"
#include "LogUtil.h"

class CWinCleanerDlg : public CDialogEx
{
public:
	CWinCleanerDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINCLEANER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

protected:
	HICON m_hIcon;
	CString m_tempPath, m_zipPath, m_outDir;
	BOOLEAN is64 = FALSE;
	CFont m_fontNotice;
	CBrush m_noticeBrush;
	CString m_strNoticeText;
	int m_nNoticeScrollPos = 0;

	enum { ID_TIMER_NOTICE_SCROLL = 1002 };
	BOOL EnsureToolExtracted(const CString& primaryPath, const CString& fallbackPath = _T(""), DWORD waitMs = 5000);
	CString GetParentDir(const CString& filePath) const;
	BOOL PrepareHuorongRuntime();
	void RemoveDeleteTask();
	CString GetSelfPath();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	// 常用清理
	afx_msg void OnBnClickedDiskClean();
	afx_msg void OnBnClickedBigfileDelete();
	afx_msg void OnBnClickedCacheClean();
	afx_msg void OnBnClickedRedundantClean();
	afx_msg void OnBnClickedDriverMgr();
	afx_msg void OnBnClickedDiskAnalyze();

	// 系统优化
	afx_msg void OnBnClickedMemoryOptimize();
	afx_msg void OnBnClickedDiskDefrag();
	afx_msg void OnBnClickedSoftwareUninstall();
	afx_msg void OnBnClickedSysSettingsOpt();
	afx_msg void OnBnClickedSysExtremeOpt();
	afx_msg void OnBnClickedSystemOptimize();

	// 系统安全
	afx_msg void OnBnClickedSystemActivate();
	afx_msg void OnBnClickedPopupBlock();
	afx_msg void OnBnClickedKillProcess();
	afx_msg void OnBnClickedDisableUpdate();
	afx_msg void OnBnClickedCloseSecurityCenter();
	afx_msg void OnBnClickedContextMgr();

	// 其他功能
	afx_msg void OnBnClickedSearch();
	afx_msg void OnBnClickedUnlock();
	afx_msg void OnBnClickedDocMigration();
	afx_msg void OnBnClickedStartupMgr();
	afx_msg void OnBnClickedVirusScan();
	afx_msg void OnBnClickedSystemRepair();

	// 下载与其他
	afx_msg void OnBnClickedDlBeauty();
	afx_msg void OnBnClickedDlTools();
	afx_msg void OnBnClickedThisPcMgr();

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
};
