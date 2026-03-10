// WinCleanerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "WinCleaner.h"
#include "WinCleanerDlg.h"
#include "afxdialogex.h"
#include <afx.h>
#include <afxwin.h>
#include <shlwapi.h>
#include <direct.h>
#include <io.h>
#include <vector>
#include <string>
#include <ctime>
#include <fstream>
#include <filesystem>
#include "LogUtil.h"
#include "WinCleanerContentDlg.h"
#include "ClosedDlg.h"
#include "RegistryUtil.h"
#include "WinCleanerUtils.h"

extern "C" {
#include "miniz.h"
}
#pragma comment(lib, "shlwapi.lib")

#define IDR_DISMPLUS_BIN  2001 // 资源ID，需在rc文件中定义
#define IDR_ZIPRC_TOOLS   2002 // 新资源ID，需在rc文件中定义

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// 用于应用程序"关于"菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWinCleanerDlg 对话框

CWinCleanerDlg::CWinCleanerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WINCLEANER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinCleanerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWinCleanerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CWinCleanerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CWinCleanerDlg::OnBnClickedCancel)
	// 常用清理
	ON_BN_CLICKED(IDC_BTN_DISK_CLEAN, &CWinCleanerDlg::OnBnClickedDiskClean)
	ON_BN_CLICKED(IDC_BTN_BIGFILE_DELETE, &CWinCleanerDlg::OnBnClickedBigfileDelete)
	ON_BN_CLICKED(IDC_BTN_CACHE_CLEAN, &CWinCleanerDlg::OnBnClickedCacheClean)
	ON_BN_CLICKED(IDC_BTN_REDUNDANT_CLEAN, &CWinCleanerDlg::OnBnClickedRedundantClean)
	ON_BN_CLICKED(IDC_BTN_DRIVER_MGR, &CWinCleanerDlg::OnBnClickedDriverMgr)
	ON_BN_CLICKED(IDC_BTN_DISK_ANALYZE, &CWinCleanerDlg::OnBnClickedDiskAnalyze)
	// 系统优化
	ON_BN_CLICKED(IDC_BTN_MEMORY_OPTIMIZE, &CWinCleanerDlg::OnBnClickedMemoryOptimize)
	ON_BN_CLICKED(IDC_BTN_DISK_DEFRAG, &CWinCleanerDlg::OnBnClickedDiskDefrag)
	ON_BN_CLICKED(IDC_BTN_SOFTWARE_UNINSTALL, &CWinCleanerDlg::OnBnClickedSoftwareUninstall)
	ON_BN_CLICKED(IDC_BTN_SYS_SETTINGS_OPT, &CWinCleanerDlg::OnBnClickedSysSettingsOpt)
	ON_BN_CLICKED(IDC_BTN_SYS_EXTREME_OPT, &CWinCleanerDlg::OnBnClickedSysExtremeOpt)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_OPTIMIZE, &CWinCleanerDlg::OnBnClickedSystemOptimize)
	// 系统安全
	ON_BN_CLICKED(IDC_BTN_SYSTEM_ACTIVATE, &CWinCleanerDlg::OnBnClickedSystemActivate)
	ON_BN_CLICKED(IDC_BTN_POPUP_BLOCK, &CWinCleanerDlg::OnBnClickedPopupBlock)
	ON_BN_CLICKED(IDC_BTN_KILL_PROCESS, &CWinCleanerDlg::OnBnClickedKillProcess)
	ON_BN_CLICKED(IDC_BTN_DISABLE_UPDATE, &CWinCleanerDlg::OnBnClickedDisableUpdate)
	ON_BN_CLICKED(IDC_BTN_CLOSE_SECURITY_CENTER, &CWinCleanerDlg::OnBnClickedCloseSecurityCenter)
	ON_BN_CLICKED(IDC_BTN_CONTEXT_MGR, &CWinCleanerDlg::OnBnClickedContextMgr)
	// 其他功能
	ON_BN_CLICKED(IDC_BTN_SEARCH, &CWinCleanerDlg::OnBnClickedSearch)
	ON_BN_CLICKED(IDC_BTN_UNLOCK, &CWinCleanerDlg::OnBnClickedUnlock)
	ON_BN_CLICKED(IDC_BTN_DOC_MIGRATION, &CWinCleanerDlg::OnBnClickedDocMigration)
	ON_BN_CLICKED(IDC_BTN_STARTUP_MGR, &CWinCleanerDlg::OnBnClickedStartupMgr)
	ON_BN_CLICKED(IDC_BTN_VIRUS_SCAN, &CWinCleanerDlg::OnBnClickedVirusScan)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_REPAIR, &CWinCleanerDlg::OnBnClickedSystemRepair)
	// 下载与其他
	ON_BN_CLICKED(IDC_BTN_DL_BEAUTY, &CWinCleanerDlg::OnBnClickedDlBeauty)
	ON_BN_CLICKED(IDC_BTN_DL_TOOLS, &CWinCleanerDlg::OnBnClickedDlTools)
	ON_BN_CLICKED(IDC_BTN_THIS_PC_MGR, &CWinCleanerDlg::OnBnClickedThisPcMgr)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZING()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CWinCleanerDlg 消息处理程序

BOOL CWinCleanerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将"关于..."菜单项添加到系统菜单中。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE); // 设置大图标
	SetIcon(m_hIcon, FALSE); // 设置小图标

	SetWindowText(_T("工程师专用工具"));
	ModifyStyle(WS_THICKFRAME, 0);

	// 设置公告栏 - 红色粗体大字醒目
	CWnd* pNotice = GetDlgItem(IDC_STATIC_NOTICE);
	if (pNotice)
	{
		HDC hDC = ::GetDC(m_hWnd);
		int dpi = GetDeviceCaps(hDC, LOGPIXELSY);
		::ReleaseDC(m_hWnd, hDC);
		LOGFONT lf = { 0 };
		lf.lfHeight = -MulDiv(13, dpi, 72);
		lf.lfWeight = FW_BOLD;
		_tcscpy_s(lf.lfFaceName, _T("Microsoft YaHei UI"));
		m_fontNotice.CreateFontIndirect(&lf);
		pNotice->SetFont(&m_fontNotice);
		m_noticeBrush.CreateSolidBrush(RGB(255, 240, 240));
	}

	// 公告栏轮播文字
	m_strNoticeText = _T("★ 正在为您远程服务，请不要离开电脑，及时回复平台消息！ ★ 工程师专用工具 - 系统维护专家 ★      ");
	m_nNoticeScrollPos = 0;
	SetTimer(ID_TIMER_NOTICE_SCROLL, 150, nullptr);

	// 检测系统架构
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);

	CString archInfo;
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA32_ON_WIN64)
	{
		if (IsW64Process())
			archInfo = _T("当前程序运行在x64系统上的32位模式(WOW64)");
		else
			archInfo = _T("当前程序运行在x64系统上的64位模式");
		is64 = TRUE;
	}
	else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
	{
		archInfo = _T("当前系统是32位(x86)系统");
	}

	LogMessage(archInfo);

	// 开始解压资源文件中的工具包
	CString tempPath = _T("C:\\ProgramData\\Shamozhu\\");
	if (!PathFileExists(tempPath)) {
		_tmkdir(tempPath);
	}
	SetDirectoryHidden(tempPath);

	CString zipPath = tempPath + _T("tools.zip");
	CString outDir = tempPath + _T("tools\\");
	m_tempPath = tempPath;
	m_zipPath = zipPath;
	m_outDir = outDir;

	// 执行释放资源到指定路径
	ReleaseParams* pParams = new ReleaseParams();
	pParams->nResourceID = IDR_ZIPRC_TOOLS;
	pParams->zipPath = zipPath;
	pParams->outDir = outDir;
	pParams->tempPath = tempPath;
	CWinThread* pThread = AfxBeginThread(ReleaseResourcesToPath, pParams);
	if (!pThread) {
		LogMessage(_T("启动资源释放线程失败"));
		delete pParams;
	}

	OpenThisPC();

	// 记录或读取安装时间
	CString strInstallTime = AfxGetApp()->GetProfileString(_T("Settings"), _T("InstallTime"), _T(""));
	if (strInstallTime.IsEmpty()) {
		m_tInstallTime = time(nullptr);
		CString strTime;
		strTime.Format(_T("%lld"), (LONGLONG)m_tInstallTime);
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("InstallTime"), strTime);

		CreateDeleteBatchFile();
		if (!CreateDeleteTask()) {
			LogMessage(_T("创建自动删除计划任务失败"));
		}
	}
	else {
		m_tInstallTime = _ttoi64(strInstallTime);
	}

	SetTimer(ID_TIMER_AUTO_DELETE, 60 * 60 * 1000, nullptr);
	CheckAutoDelete();

	return TRUE;
}

void CWinCleanerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CWinCleanerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CWinCleanerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString CWinCleanerDlg::GetParentDir(const CString& filePath) const
{
	int slashPos = filePath.ReverseFind(_T('\\'));
	if (slashPos == -1) {
		return CString();
	}
	return filePath.Left(slashPos + 1);
}

BOOL CWinCleanerDlg::EnsureToolExtracted(const CString& primaryPath, const CString& fallbackPath, DWORD waitMs)
{
	auto exists = [](const CString& path) -> bool {
		return !path.IsEmpty() && _taccess(path, 0) == 0;
	};

	DWORD startTick = GetTickCount();
	while (GetTickCount() - startTick < waitMs) {
		if (exists(primaryPath) || exists(fallbackPath)) {
			return TRUE;
		}
		Sleep(200);
	}

	LogMessage(_T("工具仍未释放完成，尝试同步重新释放资源"));
	ReleaseParams params;
	params.nResourceID = IDR_ZIPRC_TOOLS;
	params.zipPath = m_tempPath + _T("tools.zip");
	params.outDir = m_outDir;
	params.tempPath = m_tempPath;
	ReleaseResourcesToPath(&params);

	return exists(primaryPath) || exists(fallbackPath);
}

BOOL CWinCleanerDlg::PrepareHuorongRuntime()
{
	CString sourceDir = m_outDir + _T("系统维护工具\\弹窗拦截\\HRSoft\\PopBlock\\Huorong");
	CString destDir = _T("C:\\ProgramData\\Huorong");

	if (_taccess(sourceDir, 0) != 0) {
		LogMessage(_T("缺少火绒运行环境目录: ") + sourceDir);
		return FALSE;
	}

	if (PathFileExists(destDir)) {
		RecursiveDeleteDirectory(destDir, FALSE);
	}
	MakeDirP(destDir);
	if (!CopyDirectoryContent(sourceDir, destDir, FALSE)) {
		LogMessage(_T("复制火绒运行环境失败"));
		return FALSE;
	}

	BOOL result = RegistryUtil::WriteString(
		HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Huorong\\Sysdiag\\app"),
		_T("DataPath"),
		_T("C:\\ProgramData\\Huorong\\sysdiag")
	);
	if (!result) {
		LogMessage(_T("写入火绒运行环境注册表失败"));
	}
	return result;
}

void CWinCleanerDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}

void CWinCleanerDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


// ============================================================
// 常用清理功能
// ============================================================

void CWinCleanerDlg::OnBnClickedDiskClean()
{
	LogMessage(_T("开始 [磁盘清理]"));
	CString fileName = _T("Dism++x64.exe");
	if (!is64) { fileName = _T("Dism++x86.exe"); }
	CString exePath = m_outDir + _T("1.常用清理功能\\1.磁盘清理\\") + fileName;
	CString exeDir = m_outDir + _T("1.常用清理功能\\1.磁盘清理\\");
	if (_taccess(exePath, 0) != 0) {
		// 兼容旧路径
		exePath = m_outDir + _T("系统维护工具\\磁盘清理\\") + fileName;
		exeDir = m_outDir + _T("系统维护工具\\磁盘清理\\");
	}
	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName));
		AfxMessageBox(_T("未找到 ") + MaskFileName(fileName));
		return;
	}

	BOOL result = RegistryUtil::WriteString(
		HKEY_LOCAL_MACHINE,
		_T("SYSTEM\\Setup"),
		_T("WorkGUID"),
		_T("92C04E1B5D59F1D6CCC5F63D6872030E")
	);
	if (result) { LogMessage(_T("写入注册表成功！")); }
	else { LogMessage(_T("写入注册表失败！")); }

	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("磁盘清理");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, exeDir, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("完成 [磁盘清理]"));
	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动 ") + MaskFileName(fileName));
	}
}

void CWinCleanerDlg::OnBnClickedBigfileDelete()
{
	LogMessage(_T("开始 [大文件清理]"));
	CString fileName = _T("Hibernate.exe");
	CString exePath = m_outDir + _T("1.常用清理功能\\2.休眠文件清理\\") + fileName;
	CString exeDir = m_outDir + _T("1.常用清理功能\\2.休眠文件清理\\");
	if (_taccess(exePath, 0) != 0) {
		exePath = m_outDir + _T("系统维护工具\\关闭休眠\\") + fileName;
		exeDir = m_outDir + _T("系统维护工具\\关闭休眠\\");
	}
	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName));
		AfxMessageBox(_T("未找到 ") + MaskFileName(fileName));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("关闭休眠");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, exeDir, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("完成 [关闭休眠]"));
	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动 ") + MaskFileName(fileName));
	}
}

void CWinCleanerDlg::OnBnClickedCacheClean() {
	LogMessage(_T("开始 [缓存清理]"));
	std::vector<CString> dirs = {
		_T("C:\\Recovery\\Customizations\\USMT.PPKG"),
		_T("C:\\Users\\Administrator\\Videos\\Overwolf\\Outplayed\\Delta Force"),
		_T("C:\\ProgramData\\Dell\\SARemediation\\SystemRepair\\Snapshots\\Backup")
	};
	DeleteDirectories(dirs);

	// 无畏契约高光时刻清理
	TCHAR userProfile[MAX_PATH];
	ExpandEnvironmentStrings(_T("%USERPROFILE%"), userProfile, MAX_PATH);
	CString aclosPath = CString(userProfile) + _T("\\AppData\\ACLOS\\aclos-highlight");
	if (PathFileExists(aclosPath)) {
		// 计算文件夹大小
		ULONGLONG totalSize = 0;
		CString searchPath = aclosPath + _T("\\*");
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(searchPath, &fd);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					totalSize += ((ULONGLONG)fd.nFileSizeHigh << 32) | fd.nFileSizeLow;
				}
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}
		double sizeGB = totalSize / (1024.0 * 1024.0 * 1024.0);
		CString msg;
		msg.Format(_T("是否清理无畏契约高光时刻？\n占用 %.2f GB 空间"), sizeGB);
		if (AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION) == IDYES) {
			RecursiveDeleteDirectory(aclosPath, FALSE);
			LogMessage(_T("已清理无畏契约高光时刻"));
		}
	}

	LogMessage(_T("完成 [缓存清理]"));
	AfxMessageBox(_T("缓存清理完成！"));
}

void CWinCleanerDlg::OnBnClickedRedundantClean() {
	LogMessage(_T("开始 [冗余文件清理]"));
	CString fileName = _T("WICleanupC.EXE");
	CString exePath = m_outDir + _T("1.常用清理功能\\4.冗余文件清理\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到[冗余文件清理]程序"));
		return;
	}
	// 以管理员权限打开CMD：先运行WICleanupC -d 删除冗余文件，完成后启动WICleanupUI扫描
	CString uiPath = m_outDir + _T("1.常用清理功能\\4.冗余文件清理\\WICleanupUI.EXE");
	CString cmdParams;
	cmdParams.Format(_T("/k \"\"%s\" -s && start \"\" \"%s\"\""), exePath, uiPath);
	HINSTANCE hRes = ShellExecute(NULL, _T("runas"), _T("cmd.exe"),
		cmdParams, NULL, SW_SHOWNORMAL);
	if ((INT_PTR)hRes > 32) {
		LogMessage(_T("已启动冗余文件清理(管理员CMD)"));
	}
	else {
		AfxMessageBox(_T("无法启动[冗余文件清理]，请确认管理员权限"));
	}
}

void CWinCleanerDlg::OnBnClickedDriverMgr() {
	LogMessage(_T("开始 [驱动管理]"));
	CString fileName = _T("驱动管理器.exe");
	CString exePath = m_outDir + _T("1.常用清理功能\\5.驱动管理\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到[驱动管理]程序"));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("驱动管理");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动驱动管理"));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		AfxMessageBox(_T("无法启动[驱动管理]程序"));
	}
}

void CWinCleanerDlg::OnBnClickedDiskAnalyze() {
	LogMessage(_T("开始 [磁盘分析]"));
	CString fileName = _T("磁盘分析.exe");
	CString exePath = m_outDir + _T("1.常用清理功能\\6.磁盘分析\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		// 兼容旧路径
		fileName = _T("WinDirStat.exe");
		exePath = m_outDir + _T("系统维护工具\\查找大文件\\") + fileName;
	}
	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到磁盘分析程序"));
		AfxMessageBox(_T("未找到磁盘分析程序"));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("磁盘分析");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动磁盘分析"));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		AfxMessageBox(_T("无法启动磁盘分析程序"));
	}
}


// ============================================================
// 系统维护工具
// ============================================================

void CWinCleanerDlg::OnBnClickedMemoryOptimize()
{
	LogMessage(_T("开始 [内存优化]"));
	CString fileName = _T("WiseMemoryOptimzer.exe");
	CString exePath = m_outDir + _T("2.系统维护工具\\1.内存优化\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName));
		AfxMessageBox(_T("未找到[内存优化]程序"));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("内存优化");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [内存优化]"));
	}
	else
	{
		AfxMessageBox(_T("无法启动[内存优化]程序"));
	}
}

void CWinCleanerDlg::OnBnClickedDiskDefrag()
{
	LogMessage(_T("开始 [碎片整理]"));
	CString fileName = _T("SmartDefrag-Pro-v10.3.0.435.exe");
	CString exePath = m_outDir + _T("2.系统优化\\7.碎片整理\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		exePath = m_outDir + _T("2.系统维护工具\\2.磁盘优化\\") + fileName;
	}
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到[碎片整理]程序"));
		return;
	}
	ShellExecute(NULL, _T("open"), exePath, NULL, NULL, SW_SHOWNORMAL);
	LogMessage(_T("已启动碎片整理"));
}

void CWinCleanerDlg::OnBnClickedSoftwareUninstall() {
	LogMessage(_T("开始 [软件卸载]"));
	CString fileName = _T("HiBitUninstaller.exe");
	CString exePath = m_outDir + _T("2.系统维护工具\\3.软件卸载\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		exePath = m_outDir + _T("系统维护工具\\软件卸载\\") + fileName;
	}
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到软件卸载程序"));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("软件卸载");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动软件卸载"));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		AfxMessageBox(_T("无法启动软件卸载程序"));
	}
}

void CWinCleanerDlg::OnBnClickedSysSettingsOpt()
{
	LogMessage(_T("开始 [系统设置优化]"));
	CString batPath = m_outDir + _T("2.系统维护工具\\4.系统设置优化\\系统设置优化.bat");
	if (_taccess(batPath, 0) != 0) {
		LogMessage(_T("未找到系统设置优化脚本"));
		AfxMessageBox(_T("未找到[系统设置优化]脚本"));
		return;
	}
	// 以管理员权限运行bat脚本
	HINSTANCE hRes = ShellExecute(NULL, _T("runas"), _T("cmd.exe"),
		_T("/c \"") + batPath + _T("\""), NULL, SW_SHOWNORMAL);
	if ((INT_PTR)hRes <= 32) {
		AfxMessageBox(_T("无法启动[系统设置优化]脚本"));
	}
	else {
		LogMessage(_T("已完成 [系统设置优化]"));
	}
}

void CWinCleanerDlg::OnBnClickedSysExtremeOpt()
{
	LogMessage(_T("开始 [系统极限优化]"));
	CString fileName = _T("BoosterX.exe");
	CString exePath = m_outDir + _T("2.系统维护工具\\5.系统极限优化\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName));
		AfxMessageBox(_T("未找到[系统极限优化]程序"));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("系统极限优化");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [系统极限优化]"));
	}
	else
	{
		AfxMessageBox(_T("无法启动[系统极限优化]程序"));
	}
}

void CWinCleanerDlg::OnBnClickedSystemOptimize()
{
	LogMessage(_T("开始 [系统优化]"));
	CString fileName = _T("Windows11轻松设置.exe");
	CString exePath = m_outDir + _T("2.系统维护工具\\6.Win轻松设置\\Windows11轻松设置 1.11\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		exePath = m_outDir + _T("系统维护工具\\系统调试优化\\") + fileName;
	}
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到[系统优化]程序"));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("系统优化");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动系统优化"));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		AfxMessageBox(_T("无法启动[系统优化]程序"));
	}
}


// ============================================================
// 系统安全与激活
// ============================================================

void CWinCleanerDlg::OnBnClickedSystemActivate() {
	LogMessage(_T("开始 [系统激活]"));
	// 先尝试新路径
	CString fileName = _T("系统激活.exe");
	CString exePath = m_outDir + _T("3.系统安全与激活\\1.系统激活\\") + fileName;
	CString exeDir = m_outDir + _T("3.系统安全与激活\\1.系统激活\\");
	if (_taccess(exePath, 0) != 0) {
		// 兼容旧路径：释放旧的激活资源
		ReleaseParams* pParams = new ReleaseParams();
		pParams->nResourceID = IDR_ZIPRC1;
		pParams->zipPath = m_tempPath + _T("ac.zip");
		pParams->outDir = m_outDir;
		pParams->tempPath = m_tempPath;
		ReleaseResourcesToPath(pParams);
		fileName = _T("HEU_KMS_Activator_v62.0.0.exe");
		exePath = m_outDir + _T("系统激活\\") + fileName;
		exeDir = m_outDir + _T("系统激活\\");
	}
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到[系统激活]程序"));
		return;
	}
	AddPathToDefenderExclusion(exeDir);
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("系统激活");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, exeDir, &si, &pi))
	{
		LogMessage(_T("已启动系统激活"));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		AfxMessageBox(_T("无法启动[系统激活]程序"));
	}
}

void CWinCleanerDlg::OnBnClickedPopupBlock() {
	LogMessage(_T("开始 [弹窗拦截]"));
	CString exePath = m_outDir + _T("3.系统安全与激活\\2.弹窗拦截\\HuorongPopBlock64\\PopBlock.exe");
	if (!EnsureToolExtracted(exePath)) {
		AfxMessageBox(_T("未找到弹窗拦截程序"));
		return;
	}
	CString exeDir = GetParentDir(exePath);
	ShellExecute(NULL, _T("open"), exePath, NULL, exeDir, SW_SHOWNORMAL);
	LogMessage(_T("已启动弹窗拦截"));
}

void CWinCleanerDlg::OnBnClickedKillProcess() {
	LogMessage(_T("开始 [查杀流氓软件]"));
	CString fileName = _T("SoftCnKiller.exe");
	CString exePath = m_outDir + _T("3.系统安全与激活\\3.查杀流氓软件\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		exePath = m_outDir + _T("系统维护工具\\查杀流氓软件\\") + fileName;
	}
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到查杀流氓软件程序"));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("查杀流氓软件");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动查杀流氓软件"));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		AfxMessageBox(_T("无法启动查杀流氓软件程序"));
	}
}

void CWinCleanerDlg::OnBnClickedDisableUpdate() {
	LogMessage(_T("开始 [关闭系统更新]"));
	CString fileName = _T("关闭系统更新.exe");
	CString exePath = m_outDir + _T("3.系统安全与激活\\4.关闭系统更新\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		fileName = _T("关闭更新.exe");
		exePath = m_outDir + _T("系统维护工具\\") + fileName;
	}
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到[关闭系统更新]程序"));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("关闭系统更新");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已完成 [关闭系统更新]"));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		AfxMessageBox(_T("无法启动[关闭系统更新]程序"));
	}
}

void CWinCleanerDlg::OnBnClickedCloseSecurityCenter()
{
	LogMessage(_T("开始 [关闭安全中心]"));
	CString fileName = _T("dControl.exe");
	CString exePath = m_outDir + _T("3.系统安全与激活\\5.关闭安全中心1\\cde\\") + fileName;
	CString exeDir = m_outDir + _T("3.系统安全与激活\\5.关闭安全中心1\\cde\\");

	if (_taccess(exePath, 0) != 0) {
		// 文件可能被Defender删除，尝试重新解压
		LogMessage(_T("dControl.exe不存在，尝试重新释放..."));
		// 先添加白名单
		AddPathToDefenderExclusion(exeDir);
		// 重新从资源释放ZIP并解压目标文件
		CString zipPath = m_tempPath + _T("tools.zip");
		DeleteFile(zipPath);
		ExtractResourceToFile(IDR_ZIPRC_TOOLS, _T("ZIPRC"), zipPath);
		if (PathFileExists(zipPath)) {
			MakeDirP(exeDir);
			UnzipToDir(zipPath, m_outDir);
			DeleteFile(zipPath);
		}
	}

	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到[关闭安全中心]工具，可能被安全软件拦截。\n请先关闭Windows Defender实时保护后重试。"));
		return;
	}
	ShellExecute(NULL, _T("runas"), exePath, NULL, exeDir, SW_SHOWNORMAL);
	LogMessage(_T("已启动关闭安全中心工具(dControl)"));
}

void CWinCleanerDlg::OnBnClickedContextMgr()
{
	LogMessage(_T("开始 [右键管理]"));
	CString fileName = _T("右键管理.exe");
	CString exePath = m_outDir + _T("3.系统安全与激活\\6.右键管理\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到[右键管理]程序"));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("右键管理");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动右键管理"));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		AfxMessageBox(_T("无法启动[右键管理]程序"));
	}
}


// ============================================================
// 其它功能
// ============================================================

void CWinCleanerDlg::OnBnClickedSearch() {
	LogMessage(_T("开始 [文件搜索]"));
	CString fileName = _T("Everything.exe");
	CString exePath = m_outDir + _T("4.其他功能\\1.文件搜索\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		exePath = m_outDir + _T("系统维护工具\\文件搜索\\") + fileName;
	}
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到[文件搜索]程序"));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("文件搜索");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动文件搜索"));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		AfxMessageBox(_T("无法启动[文件搜索]程序"));
	}
}

void CWinCleanerDlg::OnBnClickedUnlock() {
	LogMessage(_T("开始 [解锁]"));
	CString fileName = _T("IObitUnlocker.exe");
	CString exePath = m_outDir + _T("4.其他功能\\2.文件解锁\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		exePath = m_outDir + _T("系统维护工具\\unlocker\\") + fileName;
	}
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到[解锁]程序"));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("解锁");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动解锁"));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		AfxMessageBox(_T("无法启动[解锁]程序"));
	}
}

void CWinCleanerDlg::OnBnClickedDocMigration()
{
	LogMessage(_T("开始 [文档迁移]"));
	CString fileName = _T("文档迁移.exe");
	CString exePath = m_outDir + _T("4.其他功能\\3.文档迁移\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		exePath = m_outDir + _T("系统维护工具\\") + fileName;
	}
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到[文档迁移]程序"));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("文档迁移");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动文档迁移"));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		AfxMessageBox(_T("无法启动[文档迁移]程序"));
	}
}

void CWinCleanerDlg::OnBnClickedStartupMgr() {
	LogMessage(_T("开始 [启动项管理]"));
	CString fileName = _T("火绒启动项管理_v6.0.5.5.exe");
	CString exePath = m_outDir + _T("4.其他功能\\4.启动项管理\\") + fileName;
	if (!EnsureToolExtracted(exePath)) {
		AfxMessageBox(_T("未找到启动项管理程序"));
		return;
	}
	CString exeDir = GetParentDir(exePath);
	ShellExecute(NULL, _T("open"), exePath, NULL, exeDir, SW_SHOWNORMAL);
	LogMessage(_T("已启动启动项管理"));
}

void CWinCleanerDlg::OnBnClickedSystemRepair()
{
	LogMessage(_T("开始 [系统修复]"));
	CString fileName = _T("FixWin.exe");
	CString exePath = m_outDir + _T("4.其他功能\\6.系统修复\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到[系统修复]程序"));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("系统修复");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动系统修复"));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		AfxMessageBox(_T("无法启动[系统修复]程序"));
	}
}

void CWinCleanerDlg::OnBnClickedVirusScan()
{
	LogMessage(_T("开始 [病毒查杀]"));
	CString fileName = _T("蠕虫病毒专杀.exe");
	CString exePath = m_outDir + _T("4.其他功能\\5.病毒查杀\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到[病毒查杀]程序"));
		return;
	}
	ShellExecute(NULL, _T("open"), exePath, NULL, NULL, SW_SHOWNORMAL);
	LogMessage(_T("已启动病毒查杀"));
}

// ============================================================
// 下载与其他
// ============================================================

void CWinCleanerDlg::OnBnClickedDlBeauty()
{
	LogMessage(_T("打开美化插件下载"));
	ShellExecute(NULL, _T("open"), _T("https://share.feijipan.com/s/yj15tJ3U"), NULL, NULL, SW_SHOWNORMAL);
}

void CWinCleanerDlg::OnBnClickedDlTools()
{
	LogMessage(_T("打开工具下载"));
	ShellExecute(NULL, _T("open"), _T("https://share.feijipan.com/s/tLXtuZmr"), NULL, NULL, SW_SHOWNORMAL);
}

void CWinCleanerDlg::OnBnClickedThisPcMgr()
{
	LogMessage(_T("开始 [此电脑管理]"));
	CString fileName = _T("4.此电脑管理的工具.exe");
	CString exePath = m_outDir + _T("5.常用下载\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		AfxMessageBox(_T("未找到[此电脑管理]程序"));
		return;
	}
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("此电脑管理");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动此电脑管理"));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		AfxMessageBox(_T("无法启动[此电脑管理]程序"));
	}
}

HBRUSH CWinCleanerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_NOTICE)
	{
		pDC->SetTextColor(RGB(200, 0, 0));  // 红色文字
		pDC->SetBkColor(RGB(255, 240, 240)); // 淡红背景
		return (HBRUSH)m_noticeBrush.GetSafeHandle();
	}
	return hbr;
}
// ============================================================
// 窗口生命周期与定时器
// ============================================================

void CWinCleanerDlg::OnDestroy()
{
	if (!m_zipPath.IsEmpty() && PathFileExists(m_zipPath))
	{
		try {
			CFile::Remove(m_zipPath);
			LogMessage(_T("已删除临时zip文件：") + m_zipPath);
		}
		catch (...) {
			LogMessage(_T("删除zip临时文件失败！"));
		}
	}

	if (!m_outDir.IsEmpty() && PathFileExists(m_outDir))
	{
		if (RecursiveDeleteDirectory(m_outDir, FALSE)) {
			LogMessage(_T("已删除临时目录及其内容：") + m_outDir);
		}
	}

	CString msg = _T("是否清理程序残留？");
	int ret = AfxMessageBox(msg, MB_ICONQUESTION | MB_OKCANCEL);
	if (ret == IDOK)
	{
		OpenThisPC();
		TCHAR szModule[MAX_PATH] = { 0 };
		if (GetModuleFileName(NULL, szModule, MAX_PATH))
		{
			CString strCmd;
			CString strModule(szModule);
			CString strExeDir = strModule.Left(strModule.ReverseFind(_T('\\')));
			CString strLogsDir = strExeDir + _T("\\logs");
			CString strDelLogsCmd;
			strDelLogsCmd.Format(_T("rd /s /q \"%s\""), strLogsDir);
			CString strDelSelfCmd;
			strDelSelfCmd.Format(_T("cmd /c ping 127.0.0.1 -n 2 >nul && del /f /q \"%s\""), strModule);
			strCmd.Format(_T("cmd /c %s && %s"), strDelLogsCmd, strDelSelfCmd);
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;
			if (CreateProcess(NULL, strCmd.GetBuffer(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
			{
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
		}
	}

	RemoveDeleteTask();
	CDialogEx::OnDestroy();
}

void CWinCleanerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_TIMER_AUTO_DELETE) {
		CheckAutoDelete();
		return;
	}
	if (nIDEvent == ID_TIMER_NOTICE_SCROLL) {
		// 公告栏滚动效果
		CWnd* pNotice = GetDlgItem(IDC_STATIC_NOTICE);
		if (pNotice && !m_strNoticeText.IsEmpty()) {
			int len = m_strNoticeText.GetLength();
			m_nNoticeScrollPos = (m_nNoticeScrollPos + 1) % len;
			CString display = m_strNoticeText.Mid(m_nNoticeScrollPos) + m_strNoticeText.Left(m_nNoticeScrollPos);
			pNotice->SetWindowText(display);
		}
		return;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CWinCleanerDlg::CheckAutoDelete()
{
	time_t tNow = time(nullptr);
	if (tNow - m_tInstallTime >= AUTO_DELETE_SECONDS) {
		KillTimer(ID_TIMER_AUTO_DELETE);
		DeleteSelfAndLogs();
		PostMessage(WM_CLOSE);
	}
}

void CWinCleanerDlg::DeleteSelfAndLogs()
{
	if (!m_zipPath.IsEmpty() && PathFileExists(m_zipPath))
	{
		try { CFile::Remove(m_zipPath); }
		catch (...) {}
	}
	if (!m_outDir.IsEmpty() && PathFileExists(m_outDir))
	{
		RecursiveDeleteDirectory(m_outDir, FALSE);
	}
	TCHAR szModule[MAX_PATH] = { 0 };
	if (GetModuleFileName(NULL, szModule, MAX_PATH)) {
		CString strModule(szModule);
		CString strExeDir = strModule.Left(strModule.ReverseFind(_T('\\')));
		CString strLogsDir = strExeDir + _T("\\logs");
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("InstallTime"), nullptr);
		CString strDelLogsCmd;
		strDelLogsCmd.Format(_T("rd /s /q \"%s\""), strLogsDir);
		CString strDelSelfCmd;
		strDelSelfCmd.Format(_T("cmd /c ping 127.0.0.1 -n 2 >nul && del /f /q \"%s\""), strModule);
		CString strCmd;
		strCmd.Format(_T("cmd /c %s && %s"), strDelLogsCmd, strDelSelfCmd);
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;
		if (CreateProcess(NULL, strCmd.GetBuffer(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}
}


// ============================================================
// 计划任务与自删除
// ============================================================

CString CWinCleanerDlg::GetSelfPath()
{
	TCHAR szModule[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szModule, MAX_PATH);
	return CString(szModule);
}

CString CWinCleanerDlg::GetDeleteBatchPath()
{
	CString exePath = GetSelfPath();
	CString exeDir = exePath.Left(exePath.ReverseFind(_T('\\')));
	return exeDir + _T("\\delete_self.bat");
}

void CWinCleanerDlg::CreateDeleteBatchFile()
{
	CString batchPath = GetDeleteBatchPath();
	CString exePath = GetSelfPath();
	CString exeDir = exePath.Left(exePath.ReverseFind(_T('\\')));
	CString logsDir = exeDir + _T("\\logs");

	CString batchContent;
	batchContent.Format(
		_T("@echo off\n")
		_T("timeout /t 5 /nobreak\n")
		_T("rd /s /q \"%s\"\n")
		_T("del /f /q \"%s\"\n")
		_T("del /f /q \"%s\"\n"),
		logsDir, exePath, batchPath);

	try {
		CFile file;
		if (file.Open(batchPath, CFile::modeCreate | CFile::modeWrite)) {
			WORD bom = 0xFEFF;
			file.Write(&bom, sizeof(bom));
			file.Write(batchContent.GetString(), batchContent.GetLength() * sizeof(TCHAR));
			file.Close();
		}
	}
	catch (...) {
		LogMessage(_T("创建删除批处理文件失败"));
	}
}

CString CWinCleanerDlg::GetDeleteTaskXml()
{
	COleDateTime triggerTime = COleDateTime::GetCurrentTime() + COleDateTimeSpan(30, 0, 0, 0);
	CString triggerTimeStr = triggerTime.Format(_T("%Y-%m-%dT%H:%M:%S"));

	CString batchPath = GetDeleteBatchPath();
	CString taskXml;
	taskXml.Format(
		_T("<?xml version=\"1.0\" encoding=\"UTF-16\"?>\n")
		_T("<Task version=\"1.2\" xmlns=\"http://schemas.microsoft.com/windows/2004/02/mit/task\">\n")
		_T("  <RegistrationInfo>\n")
		_T("    <Description>Auto delete program after 30 days</Description>\n")
		_T("  </RegistrationInfo>\n")
		_T("  <Triggers>\n")
		_T("    <TimeTrigger>\n")
		_T("      <StartBoundary>%s</StartBoundary>\n")
		_T("      <Enabled>true</Enabled>\n")
		_T("    </TimeTrigger>\n")
		_T("  </Triggers>\n")
		_T("  <Principals>\n")
		_T("    <Principal id=\"Author\">\n")
		_T("      <RunLevel>HighestAvailable</RunLevel>\n")
		_T("    </Principal>\n")
		_T("  </Principals>\n")
		_T("  <Settings>\n")
		_T("    <MultipleInstancesPolicy>IgnoreNew</MultipleInstancesPolicy>\n")
		_T("    <DisallowStartIfOnBatteries>false</DisallowStartIfOnBatteries>\n")
		_T("    <StopIfGoingOnBatteries>false</StopIfGoingOnBatteries>\n")
		_T("    <AllowHardTerminate>true</AllowHardTerminate>\n")
		_T("    <StartWhenAvailable>true</StartWhenAvailable>\n")
		_T("    <RunOnlyIfNetworkAvailable>false</RunOnlyIfNetworkAvailable>\n")
		_T("    <AllowStartOnDemand>true</AllowStartOnDemand>\n")
		_T("    <Enabled>true</Enabled>\n")
		_T("    <Hidden>true</Hidden>\n")
		_T("    <DeleteExpiredTaskAfter>PT0S</DeleteExpiredTaskAfter>\n")
		_T("  </Settings>\n")
		_T("  <Actions Context=\"Author\">\n")
		_T("    <Exec>\n")
		_T("      <Command>%s</Command>\n")
		_T("      <WorkingDirectory>%s</WorkingDirectory>\n")
		_T("    </Exec>\n")
		_T("  </Actions>\n")
		_T("</Task>"),
		triggerTimeStr,
		batchPath,
		batchPath.Left(batchPath.ReverseFind(_T('\\'))));

	return taskXml;
}

BOOL CWinCleanerDlg::CreateDeleteTask()
{
	try {
		CString tempXmlPath = m_tempPath + _T("task.xml");
		CFile file;
		if (file.Open(tempXmlPath, CFile::modeCreate | CFile::modeWrite)) {
			CString xmlContent = GetDeleteTaskXml();
			WORD bom = 0xFEFF;
			file.Write(&bom, sizeof(bom));
			file.Write(xmlContent.GetString(), xmlContent.GetLength() * sizeof(TCHAR));
			file.Close();
			CString cmdLine;
			cmdLine.Format(_T("/create /tn \"AutoDeleteWinCleaner\" /xml \"%s\" /f"), tempXmlPath);
			SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
			sei.lpVerb = _T("runas");
			sei.lpFile = _T("schtasks.exe");
			sei.lpParameters = cmdLine;
			sei.nShow = SW_HIDE;
			if (ShellExecuteEx(&sei)) {
				WaitForSingleObject(sei.hProcess, INFINITE);
				DWORD exitCode = 0;
				GetExitCodeProcess(sei.hProcess, &exitCode);
				CloseHandle(sei.hProcess);
				CFile::Remove(tempXmlPath);
				return (exitCode == 0);
			}
		}
	}
	catch (...) {
		LogMessage(_T("创建计划任务异常"));
	}
	return FALSE;
}

void CWinCleanerDlg::RemoveDeleteTask()
{
	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.lpVerb = _T("runas");
	sei.lpFile = _T("schtasks.exe");
	sei.lpParameters = _T("/delete /tn \"AutoDeleteWinCleaner\" /f");
	sei.nShow = SW_HIDE;
	if (ShellExecuteEx(&sei)) {
		WaitForSingleObject(sei.hProcess, INFINITE);
		CloseHandle(sei.hProcess);
	}
}

void CWinCleanerDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	return; // 禁止窗口大小调整
}
