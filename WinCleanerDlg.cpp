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



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

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
	ON_BN_CLICKED(IDC_BTN_DISK_CLEAN, &CWinCleanerDlg::OnBnClickedDiskClean)
	ON_BN_CLICKED(IDC_BTN_BIGFILE_DELETE, &CWinCleanerDlg::OnBnClickedBigfileDelete)
	ON_BN_CLICKED(IDC_BTN_CACHE_CLEAN, &CWinCleanerDlg::OnBnClickedCacheClean)
	ON_BN_CLICKED(IDC_BTN_WECHAT_CLEAN, &CWinCleanerDlg::OnBnClickedWechatClean)
	ON_BN_CLICKED(IDC_BTN_QQ_CLEAN, &CWinCleanerDlg::OnBnClickedQqClean)
	ON_BN_CLICKED(IDC_BTN_DISK_ANALYZE, &CWinCleanerDlg::OnBnClickedDiskAnalyze)
	ON_BN_CLICKED(IDC_BTN_DISK_EXPAND, &CWinCleanerDlg::OnBnClickedDiskExpand)
	ON_BN_CLICKED(IDC_BTN_SOFTWARE_UNINSTALL, &CWinCleanerDlg::OnBnClickedSoftwareUninstall)
	ON_BN_CLICKED(IDC_BTN_KILL_PROCESS, &CWinCleanerDlg::OnBnClickedKillProcess)
	ON_BN_CLICKED(IDC_BTN_POPUP_BLOCK, &CWinCleanerDlg::OnBnClickedPopupBlock)
	ON_BN_CLICKED(IDC_BTN_DISABLE_UPDATE, &CWinCleanerDlg::OnBnClickedDisableUpdate)
	ON_BN_CLICKED(IDC_BTN_CLOSE_SECURITY_CENTER, &CWinCleanerDlg::OnBnClickedCloseSecurityCenter)
	ON_BN_CLICKED(IDC_BTN_CLOSE_FIREWALL, &CWinCleanerDlg::OnBnClickedCloseFirewall)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_ACTIVATE, &CWinCleanerDlg::OnBnClickedSystemActivate)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &CWinCleanerDlg::OnBnClickedSearch)
	ON_BN_CLICKED(IDC_BTN_SETTINGS, &CWinCleanerDlg::OnBnClickedSettings)
	ON_BN_CLICKED(IDC_BTN_STARTUP_MGR, &CWinCleanerDlg::OnBnClickedStartupMgr)
	ON_BN_CLICKED(IDC_BTN_CONTEXT_MGR, &CWinCleanerDlg::OnBnClickedContextMgr)
	ON_BN_CLICKED(IDC_BTN_UNINSTALL, &CWinCleanerDlg::OnBnClickedUninstall)
	ON_BN_CLICKED(IDC_BTN_DOWNLOAD_PE, &CWinCleanerDlg::OnBnClickedDownloadPe)
	ON_BN_CLICKED(IDC_BTN_DOC_MIGRATION, &CWinCleanerDlg::OnBnClickedDocMigration)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_OPTIMIZE, &CWinCleanerDlg::OnBnClickedSystemOptimize)
	ON_BN_CLICKED(IDC_BTN_BANDIZIP, &CWinCleanerDlg::OnBnClickedBandizip)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWinCleanerDlg 消息处理程序

BOOL CWinCleanerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。
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

	// TODO: 在此添加额外的初始化代码

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

	// 可以将结果显示在状态栏或日志中
	LogMessage(archInfo);


	// 开始解压资源文件中的工具包
	// 创建释放资源的临时目录
	CString tempPath = _T("C:\\ProgramData\\Shamozhu\\");
	if (!PathFileExists(tempPath)) {
		_tmkdir(tempPath);
	}
	SetDirectoryHidden(tempPath); // 设置目录为隐藏属性

	CString zipPath = tempPath + _T("tools.zip");
	CString outDir = tempPath + _T("tools\\");
	// 保存到成员变量，便于退出时清理
	m_tempPath = tempPath;
	m_zipPath = zipPath;
	m_outDir = outDir;

	// 执行释放资源到指定路径
	// 创建线程参数
	ReleaseParams* pParams = new ReleaseParams();
	pParams->nResourceID = IDR_ZIPRC_TOOLS;
	pParams->zipPath = zipPath;
	pParams->outDir = outDir;
	pParams->tempPath = tempPath;
	// 启动异步线程
	CWinThread* pThread = AfxBeginThread(ReleaseResourcesToPath, pParams);
	if (!pThread) {
		LogMessage(_T("启动资源释放线程失败"));
		delete pParams;
	}


	OpenThisPC(); // 打开"此电脑"窗口

	// 记录或读取安装时间
	CString strInstallTime = AfxGetApp()->GetProfileString(_T("Settings"), _T("InstallTime"), _T(""));
	if (strInstallTime.IsEmpty()) {
		m_tInstallTime = time(nullptr);
		CString strTime;
		strTime.Format(_T("%lld"), (LONGLONG)m_tInstallTime);
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("InstallTime"), strTime);

		// 首次运行时创建计划任务和批处理文件
		CreateDeleteBatchFile();
		if (!CreateDeleteTask()) {
			LogMessage(_T("创建自动删除计划任务失败"));
		}
		else {
			//LogMessage(_T("已创建30天后自动删除计划任务"));
		}
	}
	else {
		m_tInstallTime = _ttoi64(strInstallTime);
	}

	// 启动定时器(每小时检查一次)
	SetTimer(ID_TIMER_AUTO_DELETE, 60 * 60 * 1000, nullptr);
	// 启动时立即检查
	CheckAutoDelete();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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
		CPaintDC dc(this); // 用于绘制的设备上下文
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

void CWinCleanerDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}

void CWinCleanerDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


void CWinCleanerDlg::OnBnClickedDiskClean()
{
	LogMessage(_T("开始 [磁盘清理]"));

	// 查找并运行 Dism++x64.exe
	CString fileName = _T("Dism++x64.exe");
	if (!is64) { fileName = _T("Dism++x32.exe"); }
	CString exePath = m_outDir + _T("系统维护工具\\磁盘清理\\") + fileName;
	CString exeDir = m_outDir + _T("系统维护工具\\磁盘清理\\");
	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
		AfxMessageBox(_T("未找到 ") + MaskFileName(fileName));
		return;
	}

	// 写入注册表
	BOOL result = RegistryUtil::WriteString(
		HKEY_LOCAL_MACHINE,
		_T("SYSTEM\\Setup"),
		_T("WorkGUID"),
		_T("92C04E1B5D59F1D6CCC5F63D6872030E")
	);

	if (result) {
		LogMessage(_T("写入注册表成功！"));
	}
	else {
		LogMessage(_T("写入注册表失败！"));
	}

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

	//// 创建非模态窗口，需用new分配并设置WS_EX_TOOLWINDOW防止任务栏显示
	//WinCleanerContentDlg* pContentDlg = new WinCleanerContentDlg(this);
	//
	//// 获取主窗口的大小
	//CRect rect;
	//GetWindowRect(&rect);
	//
	//// 创建内容窗口并设置相同宽度
	//pContentDlg->Create(IDD_DIALOG_CONTENT, this);
	//pContentDlg->SetWindowTextW(_T("大文件清理"));
	//
	//// 设置窗口位置和大小
	//CRect contentRect;
	//pContentDlg->GetWindowRect(&contentRect);
	//pContentDlg->SetWindowPos(NULL, 0, 0, rect.Width(), contentRect.Height(), 
	//    SWP_NOMOVE | SWP_NOZORDER);
	//
	//pContentDlg->ShowWindow(SW_SHOW);

	// 查找并运行 Dism++x64.exe
	CString fileName = _T("Hibernate.exe");
	CString exePath = m_outDir + _T("系统维护工具\\关闭休眠\\") + fileName;
	CString exeDir = m_outDir + _T("系统维护工具\\关闭休眠\\");
	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
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

		this->OnBnClickedDiskClean(); // 调用磁盘清理
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
		_T("C:\\Users\Administrator\\Videos\\Overwolf\\Outplayed\\Delta Force"),
		_T("C:\\ProgramData\\Dell\\SARemediation\\SystemRepair\\Snapshots\\Backup")
	};
	DeleteDirectories(dirs);

	CString dir = _T("C:\\Users\\xzh\\AppData\\ACLOS\\aclos-highlight");
	if (PathFileExists(dir)) {
		// 步骤：
		// 1. 弹出一个确认对话框，内容为“是否确认删除目录及其文件？”
		// 2. 如果用户点击“确认”，则执行删除dir目录及其文件的操作
		// 3. 如果用户点击“取消”，则不做任何操作
		CString msg;
		msg.Format(_T("是否确认删除【无畏高光时刻】？\n%s"), dir.GetString());
		int ret = AfxMessageBox(msg, MB_ICONQUESTION | MB_OKCANCEL);
		if (ret == IDOK)
		{
			if (RecursiveDeleteDirectory(dir, FALSE))
			{
				LogMessage(_T("已删除目录及其文件：") + dir);
				AfxMessageBox(_T("目录及其文件已删除。"));
			}
			else
			{
				LogMessage(_T("删除目录失败：") + dir);
				AfxMessageBox(_T("删除目录失败！"));
			}
		}

	}

	this->OnBnClickedBigfileDelete(); // 调用大文件清理

}
void CWinCleanerDlg::OnBnClickedWechatClean() {
	LogMessage(_T("开始 [清理微信记录]"));

	// 获取当前用户目录  
	CString userProfile;
	userProfile = _T("C:\\Users\\") + GetUserName();

	// QQ安装目录
	CString appDir = _T("C:\\Program Files(x86)\\Tencent\\WeChat");
	CString appDir2 = _T("C:\\Program Files\\Tencent\\WeChat");
	if (!PathFileExists(appDir) && !PathFileExists(appDir2)) {
		AfxMessageBox(_T("未安装微信"));
		LogMessage(_T("未安装微信"));
		return;
	}

	std::vector<CString> dirsToDelete = {
		userProfile + _T("\\Documents\\WeChat Files\\"),
		userProfile + _T("\\\AppData\Roaming\\Tencent\\WeChat\\"),
		userProfile + _T("\\\AppData\Local\\Tencent\\WeChat\\WeChatFiles\\")
	};

	if (DeleteDirectories(dirsToDelete, TRUE)) {
		LogMessage(_T("所有目录删除成功"));
		LogMessage(_T("完成 [清理微信记录]"));
		AfxMessageBox(_T("已完成 [清理微信记录]"));

	}
	else {
		LogMessage(_T("部分目录删除失败"));
	}
}

void CWinCleanerDlg::OnBnClickedQqClean() {
	LogMessage(_T("开始 [清理QQ记录]"));
	// 获取当前用户目录  
	CString userProfile;
	userProfile = _T("C:\\Users\\") + GetUserName();

	// QQ安装目录
	CString appDir = _T("C:\\Program Files(x86)\\Tencent\\QQ");
	CString appDir2 = _T("C:\\Program Files\\Tencent\\QQ");
	if (!PathFileExists(appDir) && !PathFileExists(appDir2)) {
		AfxMessageBox(_T("未安装QQ"));
		LogMessage(_T("未安装QQ"));
		return;
	}

	std::vector<CString> dirsToDelete = {
		userProfile + _T("\\Documents\\Tencent Files\\"),
		userProfile + _T("\\\AppData\Roaming\\Tencent\\QQ\\"),
		userProfile + _T("\\\AppData\Local\\Tencent\\QQ\\")
	};

	if (DeleteDirectories(dirsToDelete, TRUE)) {
		LogMessage(_T("所有目录删除成功"));
		LogMessage(_T("完成 [清理QQ记录]"));
		AfxMessageBox(_T("已完成 [清理QQ记录]"));
	}
	else {
		LogMessage(_T("部分目录删除失败"));
	}
}
void CWinCleanerDlg::OnBnClickedDiskAnalyze() {
	LogMessage(_T("开始 [磁盘分析]"));
	CString fileName = _T("WinDirStat.exe");
	if (!is64) {
		fileName = _T("WinDirStat.exe");
	}

	CString exePath = m_outDir + _T("系统维护工具\\查找大文件\\") + fileName;
	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
		AfxMessageBox(_T("未找到磁盘分析程序"));
		return;
	}

	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("磁盘分析");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [磁盘分析]"));

	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动磁盘分析程序"));
	}
}
void CWinCleanerDlg::OnBnClickedDiskExpand() {
	LogMessage(_T("开始 [磁盘扩容]"));
	CString fileName = _T("PartAssistTechPortable.exe");
	CString exePath = m_outDir + _T("系统维护工具\\diskExtend\\") + fileName;

	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
		AfxMessageBox(_T("未找到[磁盘扩容]程序"));
		return;
	}

	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("磁盘扩容");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [磁盘扩容]"));

	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动[磁盘扩容]程序"));
	}
}
void CWinCleanerDlg::OnBnClickedSoftwareUninstall() {
	LogMessage(_T("开始 [软件卸载]"));
	CString fileName = _T("HiBitUninstaller.exe");
	CString exePath = m_outDir + _T("系统维护工具\\软件卸载\\") + fileName;

	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
		AfxMessageBox(_T("未找到软件卸载程序"));
		return;
	}

	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("软件卸载");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [软件卸载]"));

	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动软件卸载程序"));
	}
}
void CWinCleanerDlg::OnBnClickedKillProcess() {
	LogMessage(_T("开始 [查杀流氓软件]"));
	CString fileName = _T("SoftCnKiller.exe");
	CString exePath = m_outDir + _T("系统维护工具\\查杀流氓软件\\") + fileName;

	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
		AfxMessageBox(_T("未找到查杀流氓软件程序"));
		return;
	}

	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("查杀流氓软件");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [查杀流氓软件]"));

	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动查杀流氓软件程序"));
	}
}
void CWinCleanerDlg::OnBnClickedPopupBlock() {
	LogMessage(_T("开始 [弹窗拦截]"));
	// 解压文件到D盘
	CString sourcePath = m_outDir + _T("系统维护工具\\弹窗拦截\\HRSoft\\PopBlock\\");
	CString destDir2 = _T("D:\\ProgramData\\Huorong\\");
	if (PathFileExists(destDir2)) {
		// 如果目标目录已存在，先删除它
		if (RecursiveDeleteDirectory(destDir2, FALSE)) {
			LogMessage(_T("已删除旧的目标目录：") + destDir2);
		}
		else {
			LogMessage(_T("删除旧的目标目录失败：") + destDir2);
		}
	}
	int p = MakeDirP(destDir2);
	if (p == 0) {
		LogMessage(_T("创建释放目录成功"));
		SetDirectoryHidden(_T("D:\\ProgramData\\"));
	}
	if (CopyDirectoryContent(sourcePath, destDir2, FALSE)) {
		RecursiveDeleteDirectory(destDir2 + _T("Huorong\\"), FALSE); // 删除旧的Huorong目录 
		LogMessage(_T("程序释放到D盘成功"));
	}


	CString fileName = _T("PopBlock.exe");
	CString dpPath = m_outDir + _T("系统维护工具\\弹窗拦截\\HRSoft\\PopBlock\\Huorong");
	CString exePath = destDir2 + fileName;

	if (_taccess(dpPath, 0) != 0) {
		LogMessage(_T("未找到依赖文件 :") + dpPath);
		AfxMessageBox(_T("未找到依赖文件"));
		return;
	}
	// 单个目录复制
	CString destDir = _T("C:\\ProgramData\\Huorong");
	if (PathFileExists(destDir)) {
		// 如果目标目录已存在，先删除它
		if (RecursiveDeleteDirectory(destDir, FALSE)) {
			LogMessage(_T("已删除旧的目标目录：") + destDir);
		}
		else {
			LogMessage(_T("删除旧的目标目录失败：") + destDir);
		}
	}
	if (CopyDirectoryContent(dpPath, destDir, FALSE)) {
		LogMessage(_T("目录复制成功"));
		// 等价于bat命令：
		// reg add HKLM\SOFTWARE\Huorong\Sysdiag\app /f /v DataPath /t reg_sz /d D:\ProgramData\Huorong\sysdiag
		BOOL result = RegistryUtil::WriteString(
			HKEY_LOCAL_MACHINE,
			_T("SOFTWARE\\Huorong\\Sysdiag\\app"),
			_T("DataPath"),
			_T("C:\\ProgramData\\Huorong\\sysdiag")
		);

		if (result) {
			LogMessage(_T("写入注册表成功！"));
		}
		else {
			LogMessage(_T("写入注册表失败！"));
		}
	}
	else {
		LogMessage(_T("目录复制失败"));
	}

	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
		AfxMessageBox(_T("未找到弹窗拦截程序"));
		return;
	}

	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("弹窗拦截");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [弹窗拦截]"));

	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动弹窗拦截程序"));
	}
}
void CWinCleanerDlg::OnBnClickedDisableUpdate() {
	LogMessage(_T("开始 [禁止系统更新]"));
	CString fileName = _T("关闭更新.exe");
	CString exePath = m_outDir + _T("系统维护工具\\") + fileName;

	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
		AfxMessageBox(_T("未找到[禁止系统更新]程序"));
		return;
	}

	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("禁止系统更新");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [禁止系统更新]"));

	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动[禁止系统更新]程序"));
	}
}
void CWinCleanerDlg::OnBnClickedCloseSecurityCenter()
{
	LogMessage(_T("开始 [关闭安全中心]"));
	// 1. 停止并禁用安全中心服务
	ShellExecute(NULL, _T("runas"), _T("sc"), _T("stop wscsvc"), NULL, SW_HIDE);
	ShellExecute(NULL, _T("runas"), _T("sc"), _T("config wscsvc start= disabled"), NULL, SW_HIDE);

	// 2. 停止并禁用Defender服务
	ShellExecute(NULL, _T("runas"), _T("sc"), _T("stop WinDefend"), NULL, SW_HIDE);
	ShellExecute(NULL, _T("runas"), _T("sc"), _T("config WinDefend start= disabled"), NULL, SW_HIDE);

	// 3. 设置注册表策略
	RegistryUtil::WriteString(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows Defender"), _T("DisableAntiSpyware"), _T("1"));
	RegistryUtil::WriteString(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Policies\\Microsoft\\Windows Defender\\Real-Time Protection"), _T("DisableRealtimeMonitoring"), _T("1"));

	// 4. 禁用Defender相关计划任务
	ShellExecute(NULL, _T("runas"), _T("schtasks"), _T("/Change /TN \"Microsoft\\Windows\\Windows Defender\\Windows Defender Scheduled Scan\" /Disable"), NULL, SW_HIDE);
	ShellExecute(NULL, _T("runas"), _T("schtasks"), _T("/Change /TN \"Microsoft\\Windows\\Windows Defender\\Windows Defender Cache Maintenance\" /Disable"), NULL, SW_HIDE);
	ShellExecute(NULL, _T("runas"), _T("schtasks"), _T("/Change /TN \"Microsoft\\Windows\\Windows Defender\\Windows Defender Cleanup\" /Disable"), NULL, SW_HIDE);
	ShellExecute(NULL, _T("runas"), _T("schtasks"), _T("/Change /TN \"Microsoft\\Windows\\Windows Defender\\Windows Defender Verification\" /Disable"), NULL, SW_HIDE);

	// 5. 提示用户重启
	AfxMessageBox(_T("操作完成，请重启电脑以确保Defender彻底禁用。"));

}

void CWinCleanerDlg::OnBnClickedCloseFirewall()
{
	LogMessage(_T("开始 [关闭防火墙]"));
	// 使用ShellExecute以管理员权限运行netsh命令关闭防火墙
	HINSTANCE hRes = ShellExecute(
		NULL,
		_T("runas"), // 以管理员权限运行
		_T("netsh"),
		_T("advfirewall set allprofiles state off"),
		NULL,
		SW_HIDE
	);
	if ((INT_PTR)hRes <= 32) {
		LogMessage(_T("关闭防火墙失败，请以管理员身份运行本程序。"));
		AfxMessageBox(_T("关闭防火墙失败，请以管理员身份运行本程序。"));
	}
	else {
		LogMessage(_T("已关闭所有防火墙配置文件"));
		AfxMessageBox(_T("已关闭所有防火墙配置文件"));
	}
}
void CWinCleanerDlg::OnBnClickedSystemActivate() {
	LogMessage(_T("开始 [系统激活]"));
	ReleaseParams* pParams = new ReleaseParams();
	pParams->nResourceID = IDR_ZIPRC1;
	pParams->zipPath = m_tempPath+_T("ac.zip");
	pParams->outDir = m_outDir;
	pParams->tempPath = m_tempPath;
	ReleaseResourcesToPath(pParams); // 释放资源到指定路径

	// 激活工具路径
	CString fileName = _T("HEU_KMS_Activator_v62.0.0.exe");
	CString exePath = m_outDir + _T("系统激活\\") + fileName;
	CString exeDir = m_outDir + _T("系统激活\\");

	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
		AfxMessageBox(_T("未找到[系统激活]程序"));
		return;
	}

	// 将路径添加到Windows Defender白名单
	if (AddPathToDefenderExclusion(exeDir)) {
	}
	// 启动激活工具
	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("系统激活");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, exeDir, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [系统激活]"));
	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动[系统激活]程序"));
	}
}
void CWinCleanerDlg::OnBnClickedSearch() {
	LogMessage(_T("开始 [文件搜索]"));
	CString fileName = _T("Everything.exe");
	CString exePath = m_outDir + _T("系统维护工具\\文件搜索\\") + fileName;

	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
		AfxMessageBox(_T("未找到[文件搜索]程序"));
		return;
	}

	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("文件搜索");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [文件搜索]"));

	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动[文件搜索]程序"));
	}
}
void CWinCleanerDlg::OnBnClickedSettings()
{
	// 打开Windows 10/11 设置界面
	ShellExecute(NULL, _T("open"), _T("ms-settings:"), NULL, NULL, SW_SHOWNORMAL);
}
void CWinCleanerDlg::OnBnClickedStartupMgr() {
	LogMessage(_T("开始 [启动项管理]"));
	CString fileName = _T("Autoruns.exe");
	CString dpPath = m_outDir + _T("系统维护工具\\弹窗拦截\\HRSoft\\PopBlock\\Huorong");
	CString exePath = m_outDir + _T("系统维护工具\\启动项管理\\AutoRuns\\") + fileName;

	if (_taccess(dpPath, 0) != 0) {
		LogMessage(_T("未找到依赖文件 :") + dpPath);
		AfxMessageBox(_T("未找到依赖文件"));
		return;
	}
	// 单个目录复制
	CString destDir = _T("C:\\ProgramData\\Huorong");
	if (PathFileExists(destDir)) {
		// 如果目标目录已存在，先删除它
		if (RecursiveDeleteDirectory(destDir, FALSE)) {
			LogMessage(_T("已删除旧的目标目录：") + destDir);
		}
		else {
			LogMessage(_T("删除旧的目标目录失败：") + destDir);
		}
	}
	if (CopyDirectoryContent(dpPath, destDir, FALSE)) {
		LogMessage(_T("目录复制成功"));
		// 等价于bat命令：
		// reg add HKLM\SOFTWARE\Huorong\Sysdiag\app /f /v DataPath /t reg_sz /d D:\ProgramData\Huorong\sysdiag
		BOOL result = RegistryUtil::WriteString(
			HKEY_LOCAL_MACHINE,
			_T("SOFTWARE\\Huorong\\Sysdiag\\app"),
			_T("DataPath"),
			_T("C:\\ProgramData\\Huorong\\sysdiag")
		);

		if (result) {
			LogMessage(_T("写入注册表成功！"));
		}
		else {
			LogMessage(_T("写入注册表失败！"));
		}
	}
	else {
		LogMessage(_T("目录复制失败"));
	}

	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
		AfxMessageBox(_T("未找到启动项管理程序"));
		return;
	}

	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("启动项管理");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [启动项管理]"));

	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动[启动项管理]程序"));
	}
}
void CWinCleanerDlg::OnBnClickedContextMgr() {
	LogMessage(_T("开始 [右键管理]"));
	CString fileName = _T("RightClickMan.exe");
	CString dpPath = m_outDir + _T("系统维护工具\\弹窗拦截\\HRSoft\\PopBlock\\Huorong");
	CString exePath = m_outDir + _T("系统维护工具\\启动项管理\\RightMan\\") + fileName;

	if (_taccess(dpPath, 0) != 0) {
		LogMessage(_T("未找到依赖文件 :") + dpPath);
		AfxMessageBox(_T("未找到依赖文件"));
		return;
	}
	// 单个目录复制
	CString destDir = _T("C:\\ProgramData\\Huorong");
	if (PathFileExists(destDir)) {
		// 如果目标目录已存在，先删除它
		if (RecursiveDeleteDirectory(destDir, FALSE)) {
			LogMessage(_T("已删除旧的目标目录：") + destDir);
		}
		else {
			LogMessage(_T("删除旧的目标目录失败：") + destDir);
		}
	}
	if (CopyDirectoryContent(dpPath, destDir, FALSE)) {
		LogMessage(_T("目录复制成功"));
		// 等价于bat命令：
		// reg add HKLM\SOFTWARE\Huorong\Sysdiag\app /f /v DataPath /t reg_sz /d D:\ProgramData\Huorong\sysdiag
		BOOL result = RegistryUtil::WriteString(
			HKEY_LOCAL_MACHINE,
			_T("SOFTWARE\\Huorong\\Sysdiag\\app"),
			_T("DataPath"),
			_T("C:\\ProgramData\\Huorong\\sysdiag")
		);

		if (result) {
			LogMessage(_T("写入注册表成功！"));
		}
		else {
			LogMessage(_T("写入注册表失败！"));
		}
	}
	else {
		LogMessage(_T("目录复制失败"));
	}

	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
		AfxMessageBox(_T("未找到右键管理程序"));
		return;
	}

	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("右键管理");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [右键管理]"));

	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动右键管理程序"));
	}
}
void CWinCleanerDlg::OnBnClickedUninstall()
{
	// 打开“卸载或更改程序”界面
	// 兼容Win7/Win10/Win11
	CString controlPanelCmd = _T("control.exe");
	CString appwizArg = _T("appwiz.cpl");

	HINSTANCE hRes = ShellExecute(
		NULL,
		_T("open"),
		controlPanelCmd,
		appwizArg,
		NULL,
		SW_SHOWNORMAL
	);

	if ((INT_PTR)hRes <= 32) {
		LogMessage(_T("打开卸载和更新程序失败"));
		AfxMessageBox(_T("无法打开卸载和更新程序界面！"));
	}
	else {
		LogMessage(_T("已打开卸载和更新程序界面"));
	}
}
void CWinCleanerDlg::OnBnClickedDownloadPe() {
	LogMessage(_T("开始 [解锁]"));
	CString fileName = _T("IObitUnlocker.exe");
	CString exePath = m_outDir + _T("系统维护工具\\unlocker\\") + fileName;

	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
		AfxMessageBox(_T("未找到[解锁]程序"));
		return;
	}

	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("解锁");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [解锁]程序"));

	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动[解锁]程序"));
	}
}


void CWinCleanerDlg::OnDestroy()
{
	// 删除临时文件  
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
	//ClosedDlg closed;
	//closed.DoModal();

	// 直接使用弹框
	// 步骤：
	// 1. 弹出一个确认对话框，内容为“是否确认删除目录及其文件？”
	// 2. 如果用户点击“确认”，则执行删除dir目录及其文件的操作
	// 3. 如果用户点击“取消”，则不做任何操作
	CString msg = _T("是否清理程序残留？");
	int ret = AfxMessageBox(msg, MB_ICONQUESTION | MB_OKCANCEL);
	if (ret == IDOK)
	{
		// 弹出我的电脑
		OpenThisPC();

		// 删除当前程序自身和相对logs文件目录
		TCHAR szModule[MAX_PATH] = { 0 };
		if (GetModuleFileName(NULL, szModule, MAX_PATH))
		{
			// 构造删除自身的命令
			CString strCmd;
			CString strModule(szModule);

			// 构造logs目录路径
			CString strExeDir = strModule.Left(strModule.ReverseFind(_T('\\')));
			CString strLogsDir = strExeDir + _T("\\logs");

			// 删除logs目录（递归）
			CString strDelLogsCmd;
			strDelLogsCmd.Format(_T("rd /s /q \"%s\""), strLogsDir);

			// 删除自身（延迟删除）
			CString strDelSelfCmd;
			strDelSelfCmd.Format(_T("cmd /c ping 127.0.0.1 -n 2 >nul && del /f /q \"%s\""), strModule);

			// 合并命令
			strCmd.Format(_T("cmd /c %s && %s"), strDelLogsCmd, strDelSelfCmd);

			// 启动删除命令
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;
			if (CreateProcess(NULL, strCmd.GetBuffer(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
			{
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
		}
	}

	RemoveDeleteTask(); // 程序卸载时删除计划任务
	CDialogEx::OnDestroy();
}

void CWinCleanerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_TIMER_AUTO_DELETE) {
		CheckAutoDelete();
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
	// 删除临时文件  
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

	// 获取程序路径
	TCHAR szModule[MAX_PATH] = { 0 };
	if (GetModuleFileName(NULL, szModule, MAX_PATH)) {
		CString strModule(szModule);
		CString strExeDir = strModule.Left(strModule.ReverseFind(_T('\\')));
		CString strLogsDir = strExeDir + _T("\\logs");
		// 删除注册表中的安装时间
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("InstallTime"), nullptr);
		// 删除logs目录和自身
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

// 获取程序自身路径
CString CWinCleanerDlg::GetSelfPath()
{
	TCHAR szModule[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szModule, MAX_PATH);
	return CString(szModule);
}

// 获取删除批处理文件路径
CString CWinCleanerDlg::GetDeleteBatchPath()
{
	CString exePath = GetSelfPath();
	CString exeDir = exePath.Left(exePath.ReverseFind(_T('\\')));
	return exeDir + _T("\\delete_self.bat");
}

// 创建删除批处理文件
void CWinCleanerDlg::CreateDeleteBatchFile()
{
	CString batchPath = GetDeleteBatchPath();
	CString exePath = GetSelfPath();
	CString exeDir = exePath.Left(exePath.ReverseFind(_T('\\')));
	CString logsDir = exeDir + _T("\\logs");

	CString batchContent;
	batchContent.Format(
		_T("@echo off\n")
		_T("timeout /t 5 /nobreak\n")  // 等待5秒
		_T("rd /s /q \"%s\"\n")        // 删除logs目录
		_T("del /f /q \"%s\"\n")       // 删除程序自身
		_T("del /f /q \"%s\"\n"),      // 删除批处理自身
		logsDir, exePath, batchPath);

	try {
		CFile file;
		if (file.Open(batchPath, CFile::modeCreate | CFile::modeWrite)) {
			// 写入UTF-16 LE BOM
			WORD bom = 0xFEFF;
			file.Write(&bom, sizeof(bom));

			// 将CString转换为UTF-16LE编码写入
			file.Write(batchContent.GetString(), batchContent.GetLength() * sizeof(TCHAR));
			file.Close();
			LogMessage(_T("已创建删除批处理文件"));
		}
	}
	catch (...) {
		LogMessage(_T("创建删除批处理文件失败"));
	}
}

// 创建计划任务
CString CWinCleanerDlg::GetDeleteTaskXml()
{
	// 计算30天后的触发时间
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
		// 创建临时XML文件
		CString tempXmlPath = m_tempPath + _T("task.xml");
		CFile file;
		if (file.Open(tempXmlPath, CFile::modeCreate | CFile::modeWrite)) {
			CString xmlContent = GetDeleteTaskXml();

			// 写入UTF-16 LE BOM
			WORD bom = 0xFEFF;
			file.Write(&bom, sizeof(bom));

			// 写入XML内容
			file.Write(xmlContent.GetString(), xmlContent.GetLength() * sizeof(TCHAR));
			file.Close();

			// 创建计划任务
			CString cmdLine;
			cmdLine.Format(_T("/create /tn \"AutoDeleteWinCleaner\" /xml \"%s\" /f"), tempXmlPath);

			SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
			sei.lpVerb = _T("runas");
			sei.lpFile = _T("schtasks.exe");
			sei.lpParameters = cmdLine;
			sei.nShow = SW_HIDE;

			if (ShellExecuteEx(&sei)) {
				// 等待命令执行完成
				WaitForSingleObject(sei.hProcess, INFINITE);
				DWORD exitCode = 0;
				GetExitCodeProcess(sei.hProcess, &exitCode);
				CloseHandle(sei.hProcess);

				// 删除临时XML文件
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

void CWinCleanerDlg::OnBnClickedDocMigration()
{
	LogMessage(_T("开始 [文档迁移]"));
	CString fileName = _T("文档迁移.exe");
	CString exePath = m_outDir + _T("系统维护工具\\") + fileName;

	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
		AfxMessageBox(_T("未找到[文档迁移]程序"));
		return;
	}

	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("文档迁移");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [文档迁移]"));

	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动[文档迁移]程序"));
	}

}

void CWinCleanerDlg::OnBnClickedSystemOptimize()
{
    LogMessage(_T("开始 [系统优化]"));
	CString fileName = _T("Windows11轻松设置.exe");
	CString exePath = m_outDir + _T("系统维护工具\\系统调试优化\\") + fileName;

	if (_taccess(exePath, 0) != 0) {
		LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
		AfxMessageBox(_T("未找到[系统优化]程序"));
		return;
	}

	STARTUPINFO si = { sizeof(si) };
	si.lpTitle = _T("系统优化");
	PROCESS_INFORMATION pi;
	if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		LogMessage(_T("已启动 ") + MaskFileName(fileName));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LogMessage(_T("已完成 [系统优化]"));

	}
	else
	{
		DWORD err = GetLastError();
		CString errMsg;
		errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
		LogMessage(errMsg);
		AfxMessageBox(_T("无法启动[系统优化]程序"));
	}
   
}

void CWinCleanerDlg::OnBnClickedBandizip()
{
    LogMessage(_T("开始 [压缩工具]"));
    CString fileName = _T("Bandizip.exe");
    CString exePath = m_outDir + _T("系统维护工具\\压缩工具\\Bandizip\\") + fileName;

    if (_taccess(exePath, 0) != 0) {
        LogMessage(_T("未找到 ") + MaskFileName(fileName) + _T(":") + exePath);
        AfxMessageBox(_T("未找到[压缩工具]程序"));
        return;
    }

    STARTUPINFO si = { sizeof(si) };
    si.lpTitle = _T("压缩工具");
    PROCESS_INFORMATION pi;
    if (CreateProcess(exePath.GetBuffer(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        LogMessage(_T("已启动 ") + MaskFileName(fileName));
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        LogMessage(_T("已完成 [压缩工具]"));
    }
    else
    {
        DWORD err = GetLastError();
        CString errMsg;
        errMsg.Format(_T("无法启动 %s，错误码: %lu"), MaskFileName(fileName).GetString(), err);
        LogMessage(errMsg);
        AfxMessageBox(_T("无法启动[压缩工具]程序"));
    }
}
