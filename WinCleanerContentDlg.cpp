// WinCleanerContentDlg.cpp: 实现文件
//

#include "pch.h"
#include "WinCleaner.h"
#include "afxdialogex.h"
#include "WinCleanerContentDlg.h"


// WinCleanerContentDlg 对话框

IMPLEMENT_DYNAMIC(WinCleanerContentDlg, CDialogEx)

WinCleanerContentDlg::WinCleanerContentDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CONTENT, pParent)
{
	
}

WinCleanerContentDlg::~WinCleanerContentDlg()
{
}

void WinCleanerContentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILES, m_fileList);
	DDX_Control(pDX, IDC_BTN_SCAN, m_btnScan);
	DDX_Control(pDX, IDC_BTN_CLEAN, m_btnClean);
}


BEGIN_MESSAGE_MAP(WinCleanerContentDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SCAN, &WinCleanerContentDlg::OnBnClickedScan)
	ON_BN_CLICKED(IDC_BTN_CLEAN, &WinCleanerContentDlg::OnBnClickedClean)
END_MESSAGE_MAP()


// WinCleanerContentDlg 消息处理程序

BOOL WinCleanerContentDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// 初始化列表控件
	InitializeFileList();
	
	return TRUE;
}

void WinCleanerContentDlg::InitializeFileList()
{
	// 设置列表控件的扩展风格
	m_fileList.SetExtendedStyle(m_fileList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	
	// 添加列
	m_fileList.InsertColumn(0, _T("大小"), LVCFMT_LEFT, 100);
	m_fileList.InsertColumn(1, _T("路径"), LVCFMT_LEFT, 400);
}

void WinCleanerContentDlg::OnBnClickedScan()
{
	// 在这里实现扫描功能
}

void WinCleanerContentDlg::OnBnClickedClean()
{
	// 在这里实现清理功能
}
