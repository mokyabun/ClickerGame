#include "pch.h"
#include "GdiIconButton.h"
#include "PathResolver.h"

IMPLEMENT_DYNAMIC(CGdiIconButton, CGdiButton)

CGdiIconButton::CGdiIconButton()
	: m_pIconImage(nullptr)
	, m_bOwnsIconImage(false)
	, m_bSelected(false)
	, m_onTriggerCallback(nullptr)
{
}

CGdiIconButton::~CGdiIconButton()
{
	if (m_bOwnsIconImage && m_pIconImage) {
		delete m_pIconImage;
		m_pIconImage = nullptr;
	}
}

BEGIN_MESSAGE_MAP(CGdiIconButton, CGdiButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

BOOL CGdiIconButton::Create(const CString& caption, const CRect& rect, CWnd* pParentWnd, UINT nID)
{
	BOOL result = CGdiButton::Create(caption, rect, pParentWnd, nID);
	
	if (result) {
		ModifyStyle(0, BS_NOTIFY);
	}
	
	return result;
}

void CGdiIconButton::SetIconImage(const CString& iconImagePath)
{
	if (m_bOwnsIconImage && m_pIconImage) {
		delete m_pIconImage;
		m_pIconImage = nullptr;
	}

	CString fullPath = PathResolver::GetInstance().GetResourcePath(iconImagePath);
	m_pIconImage = Gdiplus::Image::FromFile(fullPath);
	m_bOwnsIconImage = true;

	if (m_pIconImage == nullptr || m_pIconImage->GetLastStatus() != Gdiplus::Ok) {
		CString msg;
		msg.Format(_T("Failed to load icon image!\nPath: %s"), fullPath);
		AfxMessageBox(msg);
	}

	Invalidate();
}

void CGdiIconButton::SetIconImageFromPath(Gdiplus::Image* pIconImage)
{
	if (m_bOwnsIconImage && m_pIconImage) {
		delete m_pIconImage;
		m_pIconImage = nullptr;
	}

	m_bOwnsIconImage = false;
	m_pIconImage = pIconImage;
	Invalidate();
}

void CGdiIconButton::SetName(const CString& name)
{
	m_name = name;
	m_gdiText.SetText(name);
	m_gdiText.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_gdiText.SetLineAlignment(Gdiplus::StringAlignmentNear);
	m_gdiText.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	
	Invalidate();
}

void CGdiIconButton::SetSelected(bool selected)
{
	m_bSelected = selected;
	Invalidate();
}

void CGdiIconButton::SetOnTriggerCallback(OnTriggerCallback callback)
{
	m_onTriggerCallback = callback;
}

void CGdiIconButton::DrawButton(Gdiplus::Graphics& graphics, const CRect& rect)
{
	if (!m_pIconImage) {
		return;
	}

	const int iconWidth = 48;
	const int iconHeight = 48;
	
	int iconX = rect.left + (rect.Width() - iconWidth) / 2;
	int iconY = rect.top + 5;

	if (m_bSelected) {
		Gdiplus::SolidBrush selectionBrush(Gdiplus::Color(100, 51, 153, 255));
		Gdiplus::RectF selectionRect((Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top, 
			(Gdiplus::REAL)rect.Width(), (Gdiplus::REAL)rect.Height());
		graphics.FillRectangle(&selectionBrush, selectionRect);
	}

	Gdiplus::RectF destRect((Gdiplus::REAL)iconX, (Gdiplus::REAL)iconY, 
		(Gdiplus::REAL)iconWidth, (Gdiplus::REAL)iconHeight);

	graphics.DrawImage(m_pIconImage, destRect);

	if (!m_name.IsEmpty()) {
		Gdiplus::RectF textRect((Gdiplus::REAL)rect.left, 
			(Gdiplus::REAL)(iconY + iconHeight + 2),
			(Gdiplus::REAL)rect.Width(), 
			(Gdiplus::REAL)(rect.Height() - (iconY - rect.top) - iconHeight - 2));

		m_gdiText.Draw(graphics, textRect);
	}
}

void CGdiIconButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bSelected = !m_bSelected;
	Invalidate();

	CGdiButton::OnLButtonDown(nFlags, point);
}

void CGdiIconButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_onTriggerCallback) {
		m_onTriggerCallback();
	}

	CButton::OnLButtonDblClk(nFlags, point);
}
