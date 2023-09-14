
// QuickMagDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuickMag.h"
#include "QuickMagDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum IDHotkey
{
	IDH_CURSOR_FIRST = 1000,

	IDH_CURSOR_UP = IDH_CURSOR_FIRST,
	IDH_CURSOR_DOWN,
	IDH_CURSOR_LEFT,
	IDH_CURSOR_RIGHT,

	IDH_CURSOR_LAST,
};

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CQuickMagDlg dialog



CQuickMagDlg::CQuickMagDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_QUICKMAG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//m_szDlg = CSize(0, 0);
}

void CQuickMagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CANVAS, m_wndMagnifier);
	DDX_Control(pDX, IDC_SLIDER_RATIO, m_sliderRatio);
	DDX_Control(pDX, IDC_CHECK_ALWAYS_ON_TOP, m_chkAlwaysOnTop);
}

BEGIN_MESSAGE_MAP(CQuickMagDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_NCHITTEST()
	ON_WM_HSCROLL()
	ON_WM_HOTKEY()
	ON_BN_CLICKED(IDC_CHECK_CONTROL_CURSOR, &CQuickMagDlg::OnBnClickedCheckControlCursor)
	ON_BN_CLICKED(IDC_CHECK_ALWAYS_ON_TOP, &CQuickMagDlg::OnBnClickedCheckAlwaysOnTop)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CQuickMagDlg message handlers

BOOL CQuickMagDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//CRect rect;
	//GetClientRect(rect);
	//m_szDlg = rect.Size();
	
	m_sliderRatio.SetRange(1, 16);
	m_sliderRatio.SetTicFreq(2);
	m_sliderRatio.SetPos(m_wndMagnifier.GetRatio());

	m_chkAlwaysOnTop.SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CQuickMagDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CQuickMagDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CQuickMagDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CQuickMagDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

// 	CSize szDlg = CSize(cx, cy);
// 	if ( m_wndMagnifier.GetSafeHwnd() )
// 	{
// 		CSize szDiff = szDlg - m_szDlg;
// 
// 		CRect rect;
// 		m_wndMagnifier.GetWindowRect(rect); ScreenToClient(rect);
// 		rect.right += szDiff.cx;
// 		rect.bottom += szDiff.cy;
// 		m_wndMagnifier.MoveWindow(rect);
// 	}
// 	m_szDlg = szDlg;
}


LRESULT CQuickMagDlg::OnNcHitTest(CPoint point)
{
	return CDialogEx::OnNcHitTest(point);
	//return HTCAPTION;	// enable drag the window from any point
}


void CQuickMagDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	m_wndMagnifier.SetRatio(m_sliderRatio.GetPos());
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CQuickMagDlg::RegisterCursorHotkey(BOOL bRegister)
{
	if ( bRegister )
	{
		RegisterHotKey(GetSafeHwnd(), IDH_CURSOR_UP, 0, VK_UP);
		RegisterHotKey(GetSafeHwnd(), IDH_CURSOR_DOWN, 0, VK_DOWN);
		RegisterHotKey(GetSafeHwnd(), IDH_CURSOR_LEFT, 0, VK_LEFT);
		RegisterHotKey(GetSafeHwnd(), IDH_CURSOR_RIGHT, 0, VK_RIGHT);
	} 
	else
	{
		UnregisterHotKey(GetSafeHwnd(), IDH_CURSOR_UP);
		UnregisterHotKey(GetSafeHwnd(), IDH_CURSOR_DOWN);
		UnregisterHotKey(GetSafeHwnd(), IDH_CURSOR_LEFT);
		UnregisterHotKey(GetSafeHwnd(), IDH_CURSOR_RIGHT);
	}
}

void CQuickMagDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	if ( IDH_CURSOR_FIRST <= nHotKeyId && nHotKeyId < IDH_CURSOR_LAST )
	{
		CPoint ptCursor(0,0);
		GetCursorPos(&ptCursor);
		CSize szOffset(0,0);
		switch (nHotKeyId)
		{
		case IDH_CURSOR_UP:
			--szOffset.cy;
			break;
		case IDH_CURSOR_DOWN:
			++szOffset.cy;
			break;
		case IDH_CURSOR_LEFT:
			--szOffset.cx;
			break;
		case IDH_CURSOR_RIGHT:
			++szOffset.cx;
			break;
		}
		ptCursor += szOffset;
		SetCursorPos(ptCursor.x, ptCursor.y);
	}
	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


void CQuickMagDlg::OnBnClickedCheckControlCursor()
{
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CHECK_CONTROL_CURSOR);
	if ( pCheckBox )
	{
		RegisterCursorHotkey(pCheckBox->GetCheck() == BST_CHECKED);
	}
}


void CQuickMagDlg::OnBnClickedCheckAlwaysOnTop()
{
	auto pWnd = m_chkAlwaysOnTop.GetCheck() ? &wndTopMost : &wndNoTopMost;
	SetWindowPos(pWnd, -1, -1, -1, -1, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
}

void CQuickMagDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CHECK_CONTROL_CURSOR);
	if ( pCheckBox->GetCheck() )
	{
		RegisterCursorHotkey(FALSE);
	}
}
