#pragma once
#include <gdiplus.h>
#include <functional>

#include "GdiText.h"

class CGdiButton : public CButton
{
	DECLARE_DYNAMIC(CGdiButton)

public:
	CGdiButton();
	virtual ~CGdiButton();

	// 버튼 생성
	BOOL Create(const CString& caption, const CRect& rect, CWnd* pParentWnd, UINT nID);
	
	// 이미지 설정
	void SetImage(const CString& normalImage, const CString& hoverImage = L"", const CString& pressedImage = L"");
	void SetImageFromPath(Gdiplus::Image* pNormalImage, Gdiplus::Image* pHoverImage = nullptr, Gdiplus::Image* pPressedImage = nullptr);
	
	// 텍스트 설정 (GdiText 위임)
	void SetText(const CString& text);
	void SetTextColor(Gdiplus::Color color);
	void SetFont(const CString& fontName, float fontSize, INT fontStyle = Gdiplus::FontStyleRegular);
	
	// GdiText 접근자
	GdiText& GetTextObject() { return m_gdiText; }

	// 위치 및 크기
	void SetPosition(int x, int y);
	void SetSize(int width, int height);
	void SetRect(const CRect& rect);

	// 클릭 이벤트 콜백
	using OnClickCallback = std::function<void()>;
	void SetOnClickCallback(OnClickCallback callback);

	// BS_OWNERDRAW 스타일을 위한 DrawItem 오버라이드
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;

	// 중앙 렌더링용 Draw 메서드
	virtual void Draw(Gdiplus::Graphics& graphics, CPoint offset, float deltaTime);
	void Draw(Gdiplus::Graphics& graphics, float deltaTime);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	void LoadImages();
	
	// 리팩토링된 DrawButton: 그릴 영역(rect)을 인자로 받음
	virtual void DrawButton(Gdiplus::Graphics& graphics, const CRect& rect);
	
	enum ButtonState
	{
		Normal,
		Hover,
		Pressed
	};

	ButtonState m_state;
	bool m_bMouseTracking;

	// 이미지
	CString m_normalImagePath;
	CString m_hoverImagePath;
	CString m_pressedImagePath;
	
	Gdiplus::Image* m_pNormalImage;
	Gdiplus::Image* m_pHoverImage;
	Gdiplus::Image* m_pPressedImage;
	bool m_bOwnsImages;  // 이미지 메모리 소유 여부

	// 텍스트 (GdiText로 통합)
	GdiText m_gdiText;

private:
	// 콜백
	OnClickCallback m_onClickCallback;
};
