#pragma once
#include <gdiplus.h>
#include <atlstr.h>

// 텍스트 렌더링을 위한 헬퍼 클래스
// 드롭 섀도우, 폰트, 색상, 정렬 등을 쉽게 설정하고 그릴 수 있음

class GdiText
{
public:
	// 드롭 섀도우 스타일
	enum class ShadowStyle
	{
		None,           // 그림자 없음
		DropShadow,     // 기본 드롭 섀도우 (오른쪽 아래)
		Outline         // 외곽선 효과 (4방향)
	};

	GdiText();
	~GdiText();

	// 텍스트 설정
	void SetText(const CString& text) { m_text = text; }
	
	// 폰트 설정
	void SetFont(const CString& fontFamily, float size, Gdiplus::FontStyle style = Gdiplus::FontStyleRegular);
	
	// 색상 설정
	void SetColor(Gdiplus::Color color) { m_color = color; }
	void SetColor(BYTE a, BYTE r, BYTE g, BYTE b) { m_color = Gdiplus::Color(a, r, g, b); }
	
	// 정렬 설정
	void SetAlignment(Gdiplus::StringAlignment align) { m_alignment = align; }
	void SetLineAlignment(Gdiplus::StringAlignment align) { m_lineAlignment = align; }
	
	// 그림자 설정
	void SetShadowStyle(ShadowStyle style) { m_shadowStyle = style; }
	void SetShadowColor(Gdiplus::Color color) { m_shadowColor = color; }
	void SetShadowColor(BYTE a, BYTE r, BYTE g, BYTE b) { m_shadowColor = Gdiplus::Color(a, r, g, b); }
	void SetShadowOffset(float x, float y) { m_shadowOffsetX = x; m_shadowOffsetY = y; }
	
	// 그리기
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
