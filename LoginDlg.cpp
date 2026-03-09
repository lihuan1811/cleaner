#include "pch.h"
#include "WinCleaner.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "LogUtil.h"

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG_LOGIN, pParent)
{
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_CARDNO, m_editCardNo);
    DDX_Control(pDX, IDC_COMBO_SERVICE, m_comboService);
    DDX_Control(pDX, IDOK, m_btnLogin);
}

BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
    ON_EN_CHANGE(IDC_EDIT_CARDNO, &CLoginDlg::OnEnChangeEditCardno)
    ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CLoginDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // ========== 窗口设置 ==========
    SetWindowText(_T("工程师专用工具登录系统"));

    // 调整窗口大小（更大更舒适）
    CRect dlgRect;
    GetWindowRect(&dlgRect);
    int newWidth = 420;
    int newHeight = 280;
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenW - newWidth) / 2;
    int posY = (screenH - newHeight) / 2;
    MoveWindow(posX, posY, newWidth, newHeight);

    // ========== 创建字体 ==========
    HDC hDC = ::GetDC(m_hWnd);
    int dpi = GetDeviceCaps(hDC, LOGPIXELSY);
    ::ReleaseDC(m_hWnd, hDC);

    // 标题字体 - 粗体大字
    LOGFONT lfTitle = { 0 };
    lfTitle.lfHeight = -MulDiv(16, dpi, 72);
    lfTitle.lfWeight = FW_BOLD;
    _tcscpy_s(lfTitle.lfFaceName, _T("Microsoft YaHei UI"));
    m_fontTitle.CreateFontIndirect(&lfTitle);

    // 标签字体
    LOGFONT lfLabel = { 0 };
    lfLabel.lfHeight = -MulDiv(10, dpi, 72);
    lfLabel.lfWeight = FW_NORMAL;
    _tcscpy_s(lfLabel.lfFaceName, _T("Microsoft YaHei UI"));
    m_fontLabel.CreateFontIndirect(&lfLabel);

    // 输入框字体
    LOGFONT lfEdit = { 0 };
    lfEdit.lfHeight = -MulDiv(11, dpi, 72);
    lfEdit.lfWeight = FW_NORMAL;
    _tcscpy_s(lfEdit.lfFaceName, _T("Microsoft YaHei UI"));
    m_fontEdit.CreateFontIndirect(&lfEdit);

    // 按钮字体
    LOGFONT lfBtn = { 0 };
    lfBtn.lfHeight = -MulDiv(12, dpi, 72);
    lfBtn.lfWeight = FW_BOLD;
    _tcscpy_s(lfBtn.lfFaceName, _T("Microsoft YaHei UI"));
    m_fontBtn.CreateFontIndirect(&lfBtn);

    // ========== 背景画刷 ==========
    m_bgBrush.CreateSolidBrush(RGB(245, 247, 250));     // 浅灰蓝背景
    m_editBrush.CreateSolidBrush(RGB(255, 255, 255));    // 白色输入框背景

    // ========== 获取客户区域并布局控件 ==========
    CRect clientRect;
    GetClientRect(&clientRect);
    int cx = clientRect.Width();
    int margin = 30;
    int inputW = cx - margin * 2;
    int inputH = 28;
    int labelH = 18;
    int startY = 70;  // 标题下方开始

    // 设置标签位置和字体
    CWnd* pLabelCard = GetDlgItem(IDC_STATIC);
    // 我们需要用两个静态文本，但rc只定义了一套
    // 通过遍历子窗口来设置字体
    CWnd* pChild = GetWindow(GW_CHILD);
    while (pChild)
    {
        TCHAR className[64] = { 0 };
        GetClassName(pChild->m_hWnd, className, 64);
        if (_tcsicmp(className, _T("Static")) == 0)
        {
            pChild->SetFont(&m_fontLabel);
        }
        pChild = pChild->GetNextWindow();
    }

    // 重新布局 - 卡密标签
    CWnd* pStatic1 = GetDlgItem(IDC_STATIC);

    // 重新定位控件
    int labelY = startY;
    // 查找所有静态文本并重新定位
    int ctrlIndex = 0;
    pChild = GetWindow(GW_CHILD);
    while (pChild)
    {
        TCHAR className[64] = { 0 };
        GetClassName(pChild->m_hWnd, className, 64);
        if (_tcsicmp(className, _T("Static")) == 0)
        {
            if (ctrlIndex == 0)
            {
                // "登录卡密:" 标签
                pChild->MoveWindow(margin, startY, 70, labelH);
                pChild->SetWindowText(_T("登录卡密"));
            }
            else if (ctrlIndex == 1)
            {
                // "客户服务:" 标签
                pChild->MoveWindow(margin, startY + inputH + 20, 70, labelH);
                pChild->SetWindowText(_T("服务类型"));
            }
            ctrlIndex++;
        }
        pChild = pChild->GetNextWindow();
    }

    // 卡密输入框
    m_editCardNo.MoveWindow(margin + 75, startY - 4, inputW - 75, inputH);
    m_editCardNo.SetFont(&m_fontEdit);

    // 服务选择框
    int comboY = startY + inputH + 16;
    m_comboService.MoveWindow(margin + 75, comboY, inputW - 75, 200);
    m_comboService.SetFont(&m_fontEdit);

    // 登录按钮 - 全宽大按钮
    int btnY = comboY + inputH + 25;
    int btnH = 38;
    m_btnLogin.MoveWindow(margin, btnY, inputW, btnH);
    m_btnLogin.SetFont(&m_fontBtn);
    m_btnLogin.SetWindowText(_T("登  录"));

    // ========== 添加客服选项 ==========
    m_comboService.ResetContent();
    CString strService = _T("C盘清理，系统优化，扩容分区，数据迁移，DLL修复，桌面美化，清理流氓软件，重装系统，清理其他盘");
    int nStart = 0;
    int nEnd = strService.Find(_T("，"));
    while (nEnd != -1)
    {
        CString strItem = strService.Mid(nStart, nEnd - nStart);
        m_comboService.AddString(strItem.Trim());
        nStart = nEnd + 1;
        nEnd = strService.Find(_T("，"), nStart);
    }
    // 添加最后一项
    CString lastItem = strService.Mid(nStart);
    if (!lastItem.IsEmpty())
        m_comboService.AddString(lastItem.Trim());

    m_comboService.SetCurSel(0);

    // 给输入框设置提示文字
    m_editCardNo.SetCueBanner(_T("请输入登录卡密"));

    return TRUE;
}

BOOL CLoginDlg::OnEraseBkgnd(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);

    // 绘制渐变背景（从浅灰蓝到白色）
    for (int y = 0; y < rect.Height(); y++)
    {
        int r = 240 + (int)((255 - 240) * y / (double)rect.Height());
        int g = 244 + (int)((255 - 244) * y / (double)rect.Height());
        int b = 248 + (int)((255 - 248) * y / (double)rect.Height());
        CPen pen(PS_SOLID, 1, RGB(r, g, b));
        CPen* pOldPen = pDC->SelectObject(&pen);
        pDC->MoveTo(0, y);
        pDC->LineTo(rect.Width(), y);
        pDC->SelectObject(pOldPen);
    }

    // 绘制顶部深色标题栏区域
    CRect titleRect(0, 0, rect.Width(), 55);
    CBrush titleBrush(RGB(47, 84, 150));   // 深蓝色
    pDC->FillRect(&titleRect, &titleBrush);

    // 绘制标题文字
    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(RGB(255, 255, 255));
    CFont* pOldFont = pDC->SelectObject(&m_fontTitle);
    CRect textRect(20, 12, rect.Width() - 20, 50);
    pDC->DrawText(_T("🔧 工程师专用工具"), &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    pDC->SelectObject(pOldFont);

    // 绘制底部分隔线
    CPen linePen(PS_SOLID, 1, RGB(220, 224, 230));
    CPen* pOldPen2 = pDC->SelectObject(&linePen);
    int lineY = rect.Height() - 1;
    pDC->MoveTo(0, lineY);
    pDC->LineTo(rect.Width(), lineY);
    pDC->SelectObject(pOldPen2);

    return TRUE;
}

HBRUSH CLoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    if (nCtlColor == CTLCOLOR_STATIC)
    {
        // 静态文本控件 - 透明背景 + 深灰色文字
        pDC->SetBkMode(TRANSPARENT);
        pDC->SetTextColor(RGB(51, 65, 85));
        return (HBRUSH)GetStockObject(NULL_BRUSH);
    }

    if (nCtlColor == CTLCOLOR_EDIT)
    {
        // 输入框 - 白色背景
        pDC->SetBkColor(RGB(255, 255, 255));
        pDC->SetTextColor(RGB(30, 30, 30));
        return (HBRUSH)m_editBrush.GetSafeHandle();
    }

    return hbr;
}

void CLoginDlg::OnEnChangeEditCardno()
{
}

void CLoginDlg::OnBnClickedOk()
{
    CString cardNo;
    m_editCardNo.GetWindowText(cardNo);

    if (cardNo == _T("admin.2026"))
    {
        LogMessage(_T("登录验证成功！"));
        CDialogEx::OnOK();
    }
    else
    {
        LogMessage(_T("登录验证失败，请检查输入！"));
        AfxMessageBox(_T("登录验证失败，请检查输入！"), MB_ICONERROR);
    }
}
