
// QuickMagDlg.h : header file
//

#pragma once
#include "MagnifierWnd.h"
#include "afxcmn.h"


// CQuickMagDlg dialog
class CQuickMagDlg : public CDialogEx
{
// Construction
public:
	CQuickMagDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_QUICKMAG_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected:
	void RegisterCursorHotkey(BOOL bRegister);
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CMagnifierWnd	m_wndMagnifier;
	CSliderCtrl		m_sliderRatio;
	CButton			m_chkAlwaysOnTop;
	//CSize			m_szDlg;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnBnClickedCheckControlCursor();
	afx_msg void OnBnClickedCheckAlwaysOnTop();
	afx_msg void OnDestroy();
};
