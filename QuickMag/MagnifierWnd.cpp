// MagnifierWnd.cpp : implementation file
//

#include "stdafx.h"
#include "QuickMag.h"
#include "MagnifierWnd.h"

// CMagnifierWnd

IMPLEMENT_DYNAMIC(CMagnifierWnd, CWnd)

CMagnifierWnd::CMagnifierWnd()
{
	m_nRatio = 8;
#ifdef _DEBUG
	m_bBufferedDraw = FALSE;
#else
	m_bBufferedDraw = TRUE;
#endif // _DEBUG
	m_bReserve1PixelForGrid = TRUE;
}

CMagnifierWnd::~CMagnifierWnd()
{
}

#define WC_MAGNIFIER	_T("QuickMagWnd")

BOOL CMagnifierWnd::Register()
{
	WNDCLASS wndcls ={ 0 };
	if (GetClassInfo(AfxGetInstanceHandle(), WC_MAGNIFIER, &wndcls))
		return TRUE;
	wndcls.lpfnWndProc = ::DefWindowProc;
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.hCursor = ::LoadCursor(NULL, IDC_ARROW); //afxData.hcurArrow;
	wndcls.style = CS_DBLCLKS;// | CS_HREDRAW | CS_VREDRAW;
	wndcls.lpszClassName = WC_MAGNIFIER;
	wndcls.hbrBackground = nullptr;
	return AfxRegisterClass(&wndcls);
}

void CMagnifierWnd::PreSubclassWindow()
{
	CWnd::PreSubclassWindow();
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if (pThreadState->m_pWndInit == NULL)
	{
		Init();
	}
}

#define TIMERID_REFRESH			1024
#define TIMER_REFRESH_ELAPSE	100

void CMagnifierWnd::Init()
{
	SetTimer(TIMERID_REFRESH, TIMER_REFRESH_ELAPSE, nullptr);
}

BEGIN_MESSAGE_MAP(CMagnifierWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CMagnifierWnd message handlers

BOOL CMagnifierWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

BOOL DrawMagnifiedDesktopArea(CDC& dc, CRect rcDraw, CPoint ptCursor, int nRatio, COLORREF& crfCursor, BOOL bReserve1PixelForGrid)
{
	if (nRatio <= 0 || nRatio > 16)
		return FALSE;
	CWnd* pWndDesktop = CWnd::GetDesktopWindow();
	if (pWndDesktop->GetSafeHwnd())
	{
		CSize szDraw = rcDraw.Size();
		CDC* pDCDesktop = pWndDesktop->GetDC();

		if (1 == nRatio)
		{
			crfCursor = pDCDesktop->GetPixel(ptCursor);

			CPoint ptCaptureTopLeft = ptCursor;
			ptCaptureTopLeft.x -= szDraw.cx / 2;
			ptCaptureTopLeft.y -= szDraw.cy / 2;
			dc.BitBlt(rcDraw.left, rcDraw.top, szDraw.cx, szDraw.cy, pDCDesktop, ptCaptureTopLeft.x, ptCaptureTopLeft.y, SRCCOPY | CAPTUREBLT);
		}
		else
		{
			BOOL bDrawGridlines = nRatio >= 8;

			CSize szCaptureBmp;
			szCaptureBmp.cx = (szDraw.cx + nRatio - 1) / nRatio;
			szCaptureBmp.cy = (szDraw.cy + nRatio - 1) / nRatio;
			// make odd number such that the cursor point can be centered
			if ( !(szCaptureBmp.cx & 1) )
				++szCaptureBmp.cx;
			if ( !(szCaptureBmp.cy & 1) )
				++szCaptureBmp.cy;

			CPoint ptBmpCenter;
			ptBmpCenter.x = szCaptureBmp.cx / 2;
			ptBmpCenter.y = szCaptureBmp.cy / 2;

			CPoint ptCaptureTopLeft = ptCursor - CSize(szCaptureBmp.cx / 2, szCaptureBmp.cy / 2);
			CRect rcCapture = CRect(ptCaptureTopLeft, szCaptureBmp);

			LONG ulBitmapWidth = szCaptureBmp.cx;
			LONG ulBitmapHeight = szCaptureBmp.cy;
			LPVOID pvBits = nullptr;
			BITMAPINFO bmi;
			// zero the memory for the bitmap info
			ZeroMemory(&bmi, sizeof(BITMAPINFO));

			// setup bitmap info
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = ulBitmapWidth;
			bmi.bmiHeader.biHeight = ulBitmapHeight;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = ulBitmapWidth * ulBitmapHeight * 4;

			// create our DIB section and select the bitmap into the dc
			CDC dcMem;
			dcMem.CreateCompatibleDC(&dc);
			HBITMAP hBitmap = CreateDIBSection(dcMem, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);			
			if ( !hBitmap || !pvBits )
			{
				ASSERT(FALSE);
				return FALSE;
			}
			CBitmap bmp;
			bmp.Attach(hBitmap);
			auto oldBmp = dcMem.SelectObject(&bmp);

			dcMem.BitBlt(0, 0, szCaptureBmp.cx, szCaptureBmp.cy, pDCDesktop, ptCaptureTopLeft.x, ptCaptureTopLeft.y, SRCCOPY | CAPTUREBLT);

			CSize szCanvas = szDraw;
			CRect rcCanvas = rcDraw;

			if ( bDrawGridlines && bReserve1PixelForGrid)
			{
				// fill + grid lines (1 pixel)
				szCanvas.cx = szCaptureBmp.cx * nRatio + szCaptureBmp.cx - 1;
				szCanvas.cy = szCaptureBmp.cy * nRatio + szCaptureBmp.cy - 1;

				CPoint ptDrawCenter = rcDraw.CenterPoint();
				CPoint ptCanvasTopLeft = ptDrawCenter - CSize(szCanvas.cx / 2, szCanvas.cy / 2);
				rcCanvas = CRect(ptCanvasTopLeft, szCanvas);
			}

			LPDWORD pBmpBits = (LPDWORD)pvBits;
			
			DWORD* pColors = (DWORD*)pvBits;

			CPen* oldPen = nullptr;
			COLORREF crfGrid = RGB(128,128,128);
			CPen penGrid;
			if ( bDrawGridlines )
			{
				LOGBRUSH brGrid = {0};
				brGrid.lbStyle = BS_SOLID;
				brGrid.lbColor = crfGrid;
				penGrid.CreatePen(PS_COSMETIC|PS_ALTERNATE, 1, &brGrid);
				oldPen = dc.SelectObject(&penGrid);
			}
			dc.IntersectClipRect(rcDraw);

			dc.FillSolidRect(rcDraw, RGB(192,192,192));
			
			// NOTE: BMP is upside down
			DWORD* pClrCur = pColors;
			
			int nY = rcCanvas.top + (szCaptureBmp.cy - 1) * nRatio;
			if ( bDrawGridlines && bReserve1PixelForGrid )
				nY += szCaptureBmp.cy - 1;

			for (int nBmpY = 0; nBmpY < szCaptureBmp.cy; ++nBmpY)
			{
				for (int nBmpX = 0; nBmpX < szCaptureBmp.cx; ++nBmpX)
				{
					int nX = rcCanvas.left + nBmpX * nRatio;
					if ( bDrawGridlines && bReserve1PixelForGrid )
						nX += nBmpX;
					CRect rcPoint(nX, nY, nX + nRatio, nY + nRatio);

					COLORREF crf = *pClrCur & 0x00ffffff;
					crf = ((crf & 0xff) << 16) | (crf & 0xff00) | ((crf & 0xff0000) >> 16);

					dc.FillSolidRect(rcPoint, crf);

					//*pClrCur &= 0x00ffffff;
					++pClrCur;

					if ( bDrawGridlines && nBmpX == ptBmpCenter.x && nBmpY == ptBmpCenter.y )
					{
						CPen penCenter(PS_SOLID, 3, RGB(160,160,160));
						auto pOldPen = dc.SelectObject(&penCenter);
						auto pOldBrush = dc.SelectObject(GetStockObject(NULL_BRUSH));
						dc.Rectangle(rcPoint);
						dc.SelectObject(pOldBrush);
						dc.SelectObject(pOldPen);
					}
				}
				// horizontal grid lines
				if (bDrawGridlines && nBmpY < szCaptureBmp.cy - 1)
				{
					int nYGrid = bReserve1PixelForGrid ? nY - 1 : nY;
					dc.MoveTo(rcCanvas.left, nYGrid);
					dc.LineTo(rcCanvas.right, nYGrid);
				}
				
				nY -= nRatio;

				if ( bDrawGridlines && bReserve1PixelForGrid )
					--nY;
			}

			// vertical grid lines
			if (bDrawGridlines)
			{
				int nX = rcCanvas.left + nRatio;
				for (int nBmpX = 1; nBmpX < szCaptureBmp.cx; ++nBmpX)
				{
					dc.MoveTo(nX, rcCanvas.top);
					dc.LineTo(nX, rcCanvas.bottom);
					nX += nRatio;
					if (bReserve1PixelForGrid)
						++nX;
				}
			}

			dc.SelectClipRgn(nullptr);
			if (bDrawGridlines)
			{
				dc.SelectObject(oldPen);
			}

			int nBmpPos = ptBmpCenter.x + (szCaptureBmp.cy - ptBmpCenter.y - 1) * szCaptureBmp.cx;
			COLORREF crf = pColors[nBmpPos];
			crf &= 0x00ffffff;
			crf = ((crf & 0xff) << 16) | (crf & 0xff00) | ((crf & 0xff0000) >> 16);
			
			crfCursor = crf;

			dcMem.SelectObject(oldBmp);
		}

		pDCDesktop->DeleteDC();
		return TRUE;
	}
	return FALSE;
}

class CMyMemDC : public CMemDC
{
public:
	CMyMemDC(CDC& dc, CWnd* pWnd)
		: CMemDC(dc, pWnd)
	{
	}
	CMyMemDC(CDC& dc, const CRect& rect)
		: CMemDC(dc, rect)
	{
	}
public:
	CBitmap& GetBitmap()
	{
		return m_bmp;
	}
};

void CMagnifierWnd::DoPaint(CDC& dc)
{
	CRect rcClient;
	GetClientRect(rcClient);

	CRect rcDraw = rcClient;

	CDC* pDCDraw = &dc;

	CMyMemDC dcDraw(dc, rcDraw);
	pDCDraw = &dcDraw.GetDC();

	CFont* pFont = GetFont();
	auto pOldFont = pDCDraw->SelectObject(pFont);

	pDCDraw->FillSolidRect(rcDraw, GetSysColor(COLOR_3DFACE));

	CPoint ptCursor = CPoint(0, 0);
	GetCursorPos(&ptCursor);

	const int nInfoPanelWidth = 80;

	CRect rcMagnify = rcDraw;
	rcMagnify.right -= nInfoPanelWidth;
	COLORREF crfCursor = 0;
	DrawMagnifiedDesktopArea(*pDCDraw, rcMagnify, ptCursor, m_nRatio, crfCursor, m_bReserve1PixelForGrid);

	CRect rcInfoPane = rcDraw;
	rcInfoPane.left = rcInfoPane.right - nInfoPanelWidth;
	DrawInfoPanel(pDCDraw, rcInfoPane, crfCursor, ptCursor);

	pDCDraw->SelectObject(pOldFont);
}

void CMagnifierWnd::OnPaint()
{
	CPaintDC dc(this);
	DoPaint(dc);
}

void CMagnifierWnd::DrawInfoPanel(CDC* pDCDraw, CRect rcInfoPane, COLORREF crfCursor, CPoint ptCursor)
{
	rcInfoPane.DeflateRect(3, 3);

	CPen pen(PS_SOLID, 1, GetSysColor(COLOR_ACTIVEBORDER));
	auto pOldPen = pDCDraw->SelectObject(&pen);
	auto pOldBrush = pDCDraw->SelectObject(GetStockObject(NULL_BRUSH));

	pDCDraw->Rectangle(rcInfoPane);

	pDCDraw->SelectObject(pOldBrush);
	pDCDraw->SelectObject(pOldPen);

	rcInfoPane.DeflateRect(3, 3);

	auto pOldFont = pDCDraw->SelectObject(&GetGlobalData()->fontRegular);

	CRect rcColorElem = rcInfoPane;
	const int nColorElemHeight = 20;
	rcColorElem.bottom = rcColorElem.top + nColorElemHeight;

	int nGap = 5;
	CString str;
	// RGB component rectangles
	for (int elem = 0; elem < 3; ++elem)
	{
		COLORREF crfFill = 0xff << (elem * 8);
		pDCDraw->FillSolidRect(rcColorElem, crfFill);

		int nElemVal = ((crfCursor & 0xffffff) & crfFill) >> (elem * 8);
		str.Format(_T("%d"), nElemVal);

		COLORREF crfText = (~crfFill) & 0xffffff;
		pDCDraw->SetTextColor(crfText);
		pDCDraw->DrawText(str, rcColorElem, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);

		rcColorElem.OffsetRect(0, rcColorElem.Height() + nGap);
	}
	// RGB hex
	CRect rcInfo = rcColorElem;
	pDCDraw->SetBkMode(TRANSPARENT);
	pDCDraw->SetTextColor(RGB(0, 0, 0));
	pDCDraw->DrawText(_T("Hex"), rcInfo, DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);

	rcInfo.OffsetRect(0, rcInfo.Height() + nGap);

	str.Format(_T("%06X"), crfCursor);
	pDCDraw->SetTextColor(RGB(0, 0, 255));
	pDCDraw->DrawText(str, rcInfo, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);

	rcInfo.OffsetRect(0, rcInfo.Height() + nGap);
	rcInfo.bottom = rcInfo.top + rcInfo.Width();
	// The actual color box
	CBrush br(crfCursor);
	pOldBrush = pDCDraw->SelectObject(&br);
	pDCDraw->Rectangle(rcInfo);
	pDCDraw->SelectObject(pOldBrush);

	// additional info
	rcInfo.OffsetRect(0, rcInfo.Height() + nGap);
	rcInfo.bottom = rcInfoPane.bottom;
	pDCDraw->SetTextColor(RGB(0, 0, 0));

	CString strInfo;
	double dH = 0, dS = 0, dL = 0;
	CDrawingManager::RGBtoHSL(crfCursor, &dH, &dS, &dL);

	const int HSLMAX = 240;
	str.Format(_T("H: %d\r\nS: %d\r\nL: %d"), 
		(int)(dH * HSLMAX), (int)(dS * HSLMAX), (int)(dL * HSLMAX));
	strInfo += str;

	str.Format(_T("X = %d\r\nY = %d"), ptCursor.x, ptCursor.y);
	strInfo += _T("\r\n\r\n");
	strInfo += str;

	str.Format(_T("Ratio: %d"), m_nRatio);
	strInfo += _T("\r\n\r\n");
	strInfo += str;

	pDCDraw->DrawText(strInfo, rcInfo, DT_LEFT | DT_NOPREFIX);

	pDCDraw->SelectObject(pOldFont);
}

int CMagnifierWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	Init();
	return 0;
}

void CMagnifierWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMERID_REFRESH == nIDEvent)
	{
		Invalidate();
	}
	CWnd::OnTimer(nIDEvent);
}

void CMagnifierWnd::OnDestroy()
{
	KillTimer(TIMERID_REFRESH);

	CWnd::OnDestroy();

	// TODO: Add your message handler code here
}
