#include "pch.h"
#include "GdiText.h"

GdiText::GdiText()
	: m_pFont(nullptr)
	, m_color(255, 255, 255, 255)
	, m_shadowColor(180, 0, 0, 0)
	, m_alignment(Gdiplus::StringAlignmentNear)
	, m_lineAlignment(Gdiplus::StringAlignmentNear)
	, m_shadowStyle(ShadowStyle::None)
	, m_shadowOffsetX(1.0f)
	, m_shadowOffsetY(1.0f)
{
	m_pFont = new Gdiplus::Font(L"Tahoma", 12.0f, Gdiplus::FontStyleRegular);
}

GdiText::~GdiText()
{
	// Dispose loaded font
	if (m_pFont)
	{
		delete m_pFont;
		m_pFont = nullptr;
	}
}

void GdiText::SetFont(const CString& fontFamily, float size, Gdiplus::FontStyle style)
{
	// Dispose previous font
	if (m_pFont)
	{
		delete m_pFont;
	}
	
	m_pFont = new Gdiplus::Font(fontFamily, size, style);
}

void GdiText::Draw(Gdiplus::Graphics& graphics, float x, float y)
{
	Draw(graphics, Gdiplus::PointF(x, y));
}

void GdiText::Draw(Gdiplus::Graphics& graphics, const Gdiplus::PointF& point)
{
	// To avoid crash
	// Maybe this won't happen
	if (!m_pFont)
		return;

	Gdiplus::StringFormat format;
	format.SetAlignment(m_alignment);
	format.SetLineAlignment(m_lineAlignment);
	
	DrawWithShadow(graphics, point, &format);
}

void GdiText::Draw(Gdiplus::Graphics& graphics, const Gdiplus::RectF& rect)
{
	if (!m_pFont)
		return;

	Gdiplus::StringFormat format;
	format.SetAlignment(m_alignment);
	format.SetLineAlignment(m_lineAlignment);

	// 사각형 영역에 그릴 때는 왼쪽 위 점을 기준으로
	Gdiplus::PointF point(rect.X, rect.Y);
	
	// 중앙 정렬이면 사각형 중심으로
	if (m_alignment == Gdiplus::StringAlignmentCenter)
	{
		point.X = rect.X + rect.Width / 2.0f;
	}
	else if (m_alignment == Gdiplus::StringAlignmentFar)
	{
		point.X = rect.X + rect.Width;
	}

	if (m_lineAlignment == Gdiplus::StringAlignmentCenter)
	{
		point.Y = rect.Y + rect.Height / 2.0f;
	}
	else if (m_lineAlignment == Gdiplus::StringAlignmentFar)
	{
		point.Y = rect.Y + rect.Height;
	}

	DrawWithShadow(graphics, point, &format);
}

void GdiText::DrawWithShadow(Gdiplus::Graphics& graphics, const Gdiplus::PointF& point, Gdiplus::StringFormat* format)
{
	Gdiplus::SolidBrush shadowBrush(m_shadowColor);
	Gdiplus::SolidBrush textBrush(m_color);

	switch (m_shadowStyle)
	{
	case ShadowStyle::DropShadow:
	{
		// 드롭 섀도우: 오른쪽 아래에 그림자
		Gdiplus::PointF shadowPoint(point.X + m_shadowOffsetX, point.Y + m_shadowOffsetY);
		graphics.DrawString(m_text, -1, m_pFont, shadowPoint, format, &shadowBrush);
		break;
	}

	case ShadowStyle::Outline:
	{
		// 외곽선: 4방향에 그림자
		graphics.DrawString(m_text, -1, m_pFont, Gdiplus::PointF(point.X - m_shadowOffsetX, point.Y), format, &shadowBrush);
		graphics.DrawString(m_text, -1, m_pFont, Gdiplus::PointF(point.X + m_shadowOffsetX, point.Y), format, &shadowBrush);
		graphics.DrawString(m_text, -1, m_pFont, Gdiplus::PointF(point.X, point.Y - m_shadowOffsetY), format, &shadowBrush);
		graphics.DrawString(m_text, -1, m_pFont, Gdiplus::PointF(point.X, point.Y + m_shadowOffsetY), format, &shadowBrush);
		break;
	}

	case ShadowStyle::None:
	default:
		// 그림자 없음
		break;
	}

	// 실제 텍스트 그리기
	graphics.DrawString(m_text, -1, m_pFont, point, format, &textBrush);
}
