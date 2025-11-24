#pragma once
#include <gdiplus.h>
#include <atlstr.h>

class GdiText
{
public:
	enum class ShadowStyle
	{
		None,
		DropShadow,
		Outline
	};

	GdiText();
	~GdiText();

	// Using some builder pattern
	void SetText(const CString& text) { m_text = text; }

	void SetFont(const CString& fontFamily, float size, Gdiplus::FontStyle style = Gdiplus::FontStyleRegular);

	void SetColor(Gdiplus::Color color) { m_color = color; }
	void SetColor(BYTE a, BYTE r, BYTE g, BYTE b) { m_color = Gdiplus::Color(a, r, g, b); }

	void SetAlignment(Gdiplus::StringAlignment align) { m_alignment = align; }
	void SetLineAlignment(Gdiplus::StringAlignment align) { m_lineAlignment = align; }

	void SetShadowStyle(ShadowStyle style) { m_shadowStyle = style; }
	void SetShadowColor(Gdiplus::Color color) { m_shadowColor = color; }
	void SetShadowColor(BYTE a, BYTE r, BYTE g, BYTE b) { m_shadowColor = Gdiplus::Color(a, r, g, b); }
	void SetShadowOffset(float x, float y) { m_shadowOffsetX = x; m_shadowOffsetY = y; }

	void Draw(Gdiplus::Graphics& graphics, float x, float y);
	void Draw(Gdiplus::Graphics& graphics, const Gdiplus::PointF& point);
	void Draw(Gdiplus::Graphics& graphics, const Gdiplus::RectF& rect);

private:
	CString m_text;
	Gdiplus::Font* m_pFont;
	Gdiplus::Color m_color;
	Gdiplus::Color m_shadowColor;
	Gdiplus::StringAlignment m_alignment;
	Gdiplus::StringAlignment m_lineAlignment;
	ShadowStyle m_shadowStyle;
	float m_shadowOffsetX;
	float m_shadowOffsetY;

	void DrawWithShadow(Gdiplus::Graphics& graphics, const Gdiplus::PointF& point, Gdiplus::StringFormat* format);
};
