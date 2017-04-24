#pragma once


// CMagnifierWnd

class CMagnifierWnd : public CWnd
{
	DECLARE_DYNAMIC(CMagnifierWnd)

public:
	CMagnifierWnd();
	virtual ~CMagnifierWnd();	
public:
	static BOOL Register();

	int GetRatio() const { return m_nRatio; }
	void SetRatio(int val) { m_nRatio = val; Invalidate(); }
protected:
	void PreSubclassWindow() override;

	void Init();

	void DrawInfoPanel(CDC* pDCDraw, CRect rcDraw, COLORREF crfCursor, CPoint ptCursor);

	void DoPaint(CDC& dc);
private:
	int m_nRatio;
	BOOL m_bBufferedDraw;
	BOOL m_bReserve1PixelForGrid;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
};


