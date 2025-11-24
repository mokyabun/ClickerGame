#include "pch.h"
#include "GdiButton.h"
#include "PathResolver.h"

IMPLEMENT_DYNAMIC(CGdiButton, CButton)

CGdiButton::CGdiButton()
	: m_bMouseTracking(false)
	, m_pImage(nullptr)
	, m_bOwnsImage(false)
	, m_onClickCallback(nullptr)
{
	// GdiText ì´ˆê¸°í™” (ê¸°ë³¸ê°’ ì„¤ì •)
	m_gdiText.SetColor(Gdiplus::Color(255, 0, 0, 0));
	m_gdiText.SetFont(_T("ë§‘ì€ ê³ ë”•"), 12.0f, Gdiplus::FontStyleRegular);
	m_gdiText.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_gdiText.SetLineAlignment(Gdiplus::StringAlignmentCenter);
}

CGdiButton::~CGdiButton()
{
	if (m_bOwnsImage && m_pImage)
	{
		delete m_pImage;
		m_pImage = nullptr;
	}
}

BEGIN_MESSAGE_MAP(CGdiButton, CButton)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CGdiButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// BS_OWNERDRAW ìŠ¤íƒ€ì¼ì„ ìœ„í•œ í•„ìˆ˜ ë©”ì„œë“œ
	// WM_PAINTì—ì„œ ê·¸ë¦¬ê¸°ë¥¼ ì²˜ë¦¬í•˜ë¯€ë¡œ ì—¬ê¸°ì„œëŠ” ì•„ë¬´ê²ƒë„ í•˜ì§€ ì•ŠìŒ
}

BOOL CGdiButton::Create(const CString& caption, const CRect& rect, CWnd* pParentWnd, UINT nID)
{
	// BS_OWNERDRAW ìŠ¤íƒ€ì¼ë¡œ ë²„íŠ¼ ìƒì„± (WS_EX_TRANSPARENT ì¶”ê°€)
	BOOL result = CButton::Create(caption, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		rect, pParentWnd, nID);
	
	if (result)
	{
		// íˆ¬ëª… ë°°ê²½ì„ ìœ„í•œ í™•ìž¥ ìŠ¤íƒ€ì¼ ì¶”ê°€
		ModifyStyleEx(0, WS_EX_TRANSPARENT);
		
		if (caption.IsEmpty() == FALSE)
		{
			m_gdiText.SetText(caption);
		}
	}
	
	return result;
}

void CGdiButton::SetImage(const CString& imagePath)
{
	if (m_bOwnsImage && m_pImage)
	{
		delete m_pImage;
		m_pImage = nullptr;
	}

	if (!imagePath.IsEmpty())
	{
		CString fullPath = PathResolver::GetInstance().GetResourcePath(imagePath);
		m_pImage = Gdiplus::Image::FromFile(fullPath);
		m_bOwnsImage = true;
	}
}

void CGdiButton::SetImage(Gdiplus::Image* pImage, bool bOwnsImage)
{
	if (m_bOwnsImage && m_pImage)
	{
		delete m_pImage;
	}

	m_pImage = pImage;
	m_bOwnsImage = bOwnsImage;
}

void CGdiButton::SetText(const CString& text)
{
	m_gdiText.SetText(text);
}

void CGdiButton::SetTextColor(Gdiplus::Color color)
{
	m_gdiText.SetColor(color);
}

void CGdiButton::SetFont(const CString& fontName, float fontSize, INT fontStyle)
{
	m_gdiText.SetFont(fontName, fontSize, (Gdiplus::FontStyle)fontStyle);
}

void CGdiButton::SetPosition(int x, int y)
{
	CRect rect;
	GetWindowRect(&rect);
	GetParent()->ScreenToClient(&rect);
	SetWindowPos(nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void CGdiButton::SetSize(int width, int height)
{
	SetWindowPos(nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

void CGdiButton::SetRect(const CRect& rect)
{
	SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
}

void CGdiButton::SetOnClickCallback(OnClickCallback callback)
{
	m_onClickCallback = callback;
}

void CGdiButton::DrawButton(Gdiplus::Graphics& graphics, const CRect& rect)
{
	// ì´ë¯¸ì§€ ê·¸ë¦¬ê¸°
	if (m_pImage && m_pImage->GetLastStatus() == Gdiplus::Ok)
	{
		graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		graphics.DrawImage(m_pImage, rect.left, rect.top, rect.Width(), rect.Height());
	}
	else
	{
		// ì´ë¯¸ì§€ê°€ ì—†ìœ¼ë©´ ê¸°ë³¸ ë²„íŠ¼ ê·¸ë¦¬ê¸°
		Gdiplus::Color fillColor(255, 240, 240, 240);
		Gdiplus::SolidBrush brush(fillColor);
		graphics.FillRectangle(&brush, rect.left, rect.top, rect.Width(), rect.Height());

		// í…Œë‘ë¦¬
		Gdiplus::Pen pen(Gdiplus::Color(255, 128, 128, 128), 1.0f);
		graphics.DrawRectangle(&pen, rect.left, rect.top, rect.Width() - 1, rect.Height() - 1);
	}

	// GdiTextë¥¼ ì‚¬ìš©í•˜ì—¬ í…ìŠ¤íŠ¸ ê·¸ë¦¬ê¸°
	Gdiplus::RectF layoutRect((Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top, 
		(Gdiplus::REAL)rect.Width(), (Gdiplus::REAL)rect.Height());
	
	m_gdiText.Draw(graphics, layoutRect);
}

void CGdiButton::OnPaint()
{
	// ì¤‘ì•™ ë Œë”ë§ìœ¼ë¡œ ì´ë™í•˜ì—¬ OnPaint ë¹„í™œì„±í™”
	// CPaintDCëŠ” ìƒì„±í•´ì•¼ WM_PAINT ë©”ì‹œì§€ ì²˜ë¦¬ ì™„ë£Œ
	CPaintDC dc(this);
}

void CGdiButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
}

void CGdiButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	CRect rect;
	GetClientRect(&rect);
	
	// ë²„íŠ¼ ì˜ì—­ ë‚´ì—ì„œ ë–¼ë©´ í´ë¦­ìœ¼ë¡œ ì²˜ë¦¬
	if (rect.PtInRect(point))
	{
		// í´ë¦­ ì´ë²¤íŠ¸ ë°œìƒ
		if (m_onClickCallback)
		{
			m_onClickCallback();
		}
		else
		{
			// ë¶€ëª¨ì—ê²Œ BN_CLICKED ë©”ì‹œì§€ ì „ì†¡
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
		}
	}
}

void CGdiButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bMouseTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		tme.dwHoverTime = HOVER_DEFAULT;
		TrackMouseEvent(&tme);
		m_bMouseTracking = true;
	}
}

void CGdiButton::OnMouseLeave()
{
	m_bMouseTracking = false;
}

BOOL CGdiButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;  // ê¹œë°•ìž„ ë°©ì§€
}

void CGdiButton::Draw(Gdiplus::Graphics& graphics, CPoint offset, float deltaTime)
{
	// ì¢Œí‘œ ì €ìž¥ (ê·¸ë¦¬ê¸° ì˜¤í”„ì…‹)
	Gdiplus::Matrix matrix;
	graphics.GetTransform(&matrix);
	graphics.TranslateTransform((float)offset.x, (float)offset.y);

	// ë²„íŠ¼ ê·¸ë¦¬ê¸°
	CRect rect;
	GetClientRect(&rect);
	DrawButton(graphics, rect);

	// ì¢Œí‘œ ë³µì›
	graphics.SetTransform(&matrix);
}

void CGdiButton::Draw(Gdiplus::Graphics& graphics, float deltaTime)
{
	if (!IsWindowVisible())
		return;

	CRect rect;
	GetWindowRect(&rect);
	
	CWnd* pParent = GetParent();
	if (pParent)
	{
		pParent->ScreenToClient(&rect);
	}

	CPoint offset(rect.left, rect.top);
	Draw(graphics, offset, deltaTime);
}
