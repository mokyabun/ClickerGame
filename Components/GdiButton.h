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

	BOOL Create(const CString& caption, const CRect& rect, CWnd* pParentWnd, UINT nID);
	
	void SetImage(const CString& imagePath);
	void SetImage(Gdiplus::Image* pImage, bool bOwnsImage = false);
	
	void SetText(const CString& text);
	void SetTextColor(Gdiplus::Color color);
	void SetFont(const CString& fontName, float fontSize, INT fontStyle = Gdiplus::FontStyleRegular);
	
	GdiText& GetTextObject() { return m_gdiText; }

	void SetPosition(int x, int y);
	void SetSize(int width, int height);
	void SetRect(const CRect& rect);

	using OnClickCallback = std::function<void()>;
	void SetOnClickCallback(OnClickCallback callback);

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;

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

	virtual void DrawButton(Gdiplus::Graphics& graphics, const CRect& rect);

	bool m_bMouseTracking;
	
	Gdiplus::Image* m_pImage;
	bool m_bOwnsImage;

	GdiText m_gdiText;

private:
	OnClickCallback m_onClickCallback;
};
