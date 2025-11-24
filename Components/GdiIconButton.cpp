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
	if (m_bOwnsIconImage && m_pIconImage)
	{
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
	// ë¶€ëª¨ í´ëž˜ìŠ¤ì˜ Create í˜¸ì¶œ
	BOOL result = CGdiButton::Create(caption, rect, pParentWnd, nID);
	
	if (result)
	{
		// ë”ë¸”í´ë¦­ì„ ë°›ì„ ìˆ˜ ìžˆë„ë¡ ìŠ¤íƒ€ì¼ ì¶”ê°€
		ModifyStyle(0, BS_NOTIFY);
	}
	
	return result;
}

void CGdiIconButton::SetIconImage(const CString& iconImagePath)
{
	if (m_bOwnsIconImage && m_pIconImage)
	{
		delete m_pIconImage;
		m_pIconImage = nullptr;
	}

	CString fullPath = PathResolver::GetInstance().GetResourcePath(iconImagePath);
	m_pIconImage = Gdiplus::Image::FromFile(fullPath);
	m_bOwnsIconImage = true;

	if (m_pIconImage == nullptr || m_pIconImage->GetLastStatus() != Gdiplus::Ok)
	{
		CString msg;
		msg.Format(_T("ì•„ì´ì½˜ ì´ë¯¸ì§€ë¥¼ ë¡œë“œí•  ìˆ˜ ì—†ìŠµë‹ˆë‹¤!\nê²½ë¡œ: %s"), fullPath);
		AfxMessageBox(msg);
	}

	Invalidate();
}

void CGdiIconButton::SetIconImageFromPath(Gdiplus::Image* pIconImage)
{
	if (m_bOwnsIconImage && m_pIconImage)
	{
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
	// GdiTextì—ë„ ì„¤ì • (ê·¸ë¦¼ìž íš¨ê³¼ ë“±ì„ ìœ„í•´)
	m_gdiText.SetText(name);
	m_gdiText.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_gdiText.SetLineAlignment(Gdiplus::StringAlignmentNear);
	m_gdiText.SetShadowStyle(GdiText::ShadowStyle::DropShadow); // ì•„ì´ì½˜ ë²„íŠ¼ì€ ê¸°ë³¸ì ìœ¼ë¡œ ê·¸ë¦¼ìž ì ìš©
	
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
	if (!m_pIconImage)
		return;

	// ì•„ì´ì½˜ ì˜ì—­ (48x48)
	const int iconWidth = 48;
	const int iconHeight = 48;
	
	// ì•„ì´ì½˜ì„ ì¤‘ì•™ ìƒë‹¨ì— ê·¸ë¦¬ê¸°
	int iconX = rect.left + (rect.Width() - iconWidth) / 2;
	int iconY = rect.top + 5;  // ìœ„ì—ì„œ 5px ë–¨ì–´ì§„ ìœ„ì¹˜

	// ì„ íƒ ìƒíƒœì¼ ë•Œ ì „ì²´ ë²„íŠ¼ ì˜ì—­ì— íŒŒëž€ìƒ‰ ë°°ê²½ ê·¸ë¦¬ê¸° (Windows ìŠ¤íƒ€ì¼)
	if (m_bSelected)
	{
		Gdiplus::SolidBrush selectionBrush(Gdiplus::Color(100, 51, 153, 255)); // ë°˜íˆ¬ëª… íŒŒëž€ìƒ‰
		Gdiplus::RectF selectionRect((Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top, 
			(Gdiplus::REAL)rect.Width(), (Gdiplus::REAL)rect.Height());
		graphics.FillRectangle(&selectionBrush, selectionRect);
	}

	// ë‹¨ì¼ ì´ë¯¸ì§€ ì „ì²´ë¥¼ ê·¸ë¦¬ê¸°
	Gdiplus::RectF destRect((Gdiplus::REAL)iconX, (Gdiplus::REAL)iconY, 
		(Gdiplus::REAL)iconWidth, (Gdiplus::REAL)iconHeight);

	graphics.DrawImage(m_pIconImage, destRect);

	// í…ìŠ¤íŠ¸ ê·¸ë¦¬ê¸° (ì•„ì´ì½˜ ì•„ëž˜)
	if (!m_name.IsEmpty())
	{
		// í…ìŠ¤íŠ¸ ì˜ì—­ ê³„ì‚°
		Gdiplus::RectF textRect((Gdiplus::REAL)rect.left, 
			(Gdiplus::REAL)(iconY + iconHeight + 2),
			(Gdiplus::REAL)rect.Width(), 
			(Gdiplus::REAL)(rect.Height() - (iconY - rect.top) - iconHeight - 2));

		// GdiTextë¥¼ ì‚¬ìš©í•˜ì—¬ ê·¸ë¦¬ê¸° (ê·¸ë¦¼ìž íš¨ê³¼ ìžë™ ì ìš©)
		m_gdiText.Draw(graphics, textRect);
	}
}

void CGdiIconButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// í•œ ë²ˆ í´ë¦­ ì‹œ ì„ íƒ í† ê¸€
	m_bSelected = !m_bSelected;
	Invalidate();

	// ë¶€ëª¨ í´ëž˜ìŠ¤ì˜ OnClick ì½œë°±ë„ í˜¸ì¶œ
	CGdiButton::OnLButtonDown(nFlags, point);
}

void CGdiIconButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// ë”ë¸”í´ë¦­ ì‹œ íŠ¸ë¦¬ê±° ì½œë°± í˜¸ì¶œ
	if (m_onTriggerCallback)
	{
		m_onTriggerCallback();
	}

	CButton::OnLButtonDblClk(nFlags, point);
}
