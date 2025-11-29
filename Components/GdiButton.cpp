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
	m_gdiText.SetColor(Gdiplus::Color(255, 0, 0, 0));
	m_gdiText.SetFont(_T("맑은 고딕"), 12.0f, Gdiplus::FontStyleRegular);
	m_gdiText.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_gdiText.SetLineAlignment(Gdiplus::StringAlignmentCenter);
}

CGdiButton::~CGdiButton()
{
	if (m_bOwnsImage && m_pImage) {
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
}

BOOL CGdiButton::Create(const CString& caption, const CRect& rect, CWnd* pParentWnd, UINT nID)
{
	BOOL result = CButton::Create(caption, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rect, pParentWnd, nID);
	
	if (result) {
		ModifyStyleEx(0, WS_EX_TRANSPARENT);
		
		if (caption.IsEmpty() == FALSE) {
			m_gdiText.SetText(caption);
		}
	}
	
	return result;
}

void CGdiButton::SetImage(const CString& imagePath)
{
	if (m_bOwnsImage && m_pImage) {
		delete m_pImage;
		m_pImage = nullptr;
	}

	if (!imagePath.IsEmpty()) {
		CString fullPath = PathResolver::GetInstance().GetResourcePath(imagePath);
		m_pImage = Gdiplus::Image::FromFile(fullPath);
		m_bOwnsImage = true;
	}
}

void CGdiButton::SetImage(Gdiplus::Image* pImage, bool bOwnsImage)
{
	if (m_bOwnsImage && m_pImage) {
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
	if (m_pImage && m_pImage->GetLastStatus() == Gdiplus::Ok) {
		graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		graphics.DrawImage(m_pImage, rect.left, rect.top, rect.Width(), rect.Height());
	} else {
		Gdiplus::Color fillColor(255, 240, 240, 240);
		Gdiplus::SolidBrush brush(fillColor);
		graphics.FillRectangle(&brush, rect.left, rect.top, rect.Width(), rect.Height());

		Gdiplus::Pen pen(Gdiplus::Color(255, 128, 128, 128), 1.0f);
		graphics.DrawRectangle(&pen, rect.left, rect.top, rect.Width() - 1, rect.Height() - 1);
	}

	Gdiplus::RectF layoutRect((Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top, (Gdiplus::REAL)rect.Width(), (Gdiplus::REAL)rect.Height());
	m_gdiText.Draw(graphics, layoutRect);
}

void CGdiButton::OnPaint()
{
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
	
	if (rect.PtInRect(point)) {
		if (m_onClickCallback) {
			m_onClickCallback();
		} else {
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
		}
	}
}

void CGdiButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bMouseTracking) {
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
	return TRUE;
}

void CGdiButton::Draw(Gdiplus::Graphics& graphics, CPoint offset, float deltaTime)
{
	Gdiplus::Matrix matrix;
	graphics.GetTransform(&matrix);
	graphics.TranslateTransform((float)offset.x, (float)offset.y);

	CRect rect;
	GetClientRect(&rect);
	DrawButton(graphics, rect);

	graphics.SetTransform(&matrix);
}

void CGdiButton::Draw(Gdiplus::Graphics& graphics, float deltaTime)
{
	if (!IsWindowVisible()) {
		return;
	}

	CRect rect;
	GetWindowRect(&rect);
	
	CWnd* pParent = GetParent();
	if (pParent) {
		pParent->ScreenToClient(&rect);
	}

	CPoint offset(rect.left, rect.top);
	Draw(graphics, offset, deltaTime);
}
