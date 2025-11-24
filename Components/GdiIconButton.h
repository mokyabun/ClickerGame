#pragma once
#include "GdiButton.h"

class CGdiIconButton : public CGdiButton
{
	DECLARE_DYNAMIC(CGdiIconButton)

public:
	CGdiIconButton();
	virtual ~CGdiIconButton();

	void SetIconImage(const CString& iconImagePath);
	void SetIconImageFromPath(Gdiplus::Image* pIconImage);

	void SetName(const CString& name);

	bool IsSelected() const { return m_bSelected; }
	void SetSelected(bool selected);
	void Deselect() { SetSelected(false); }

	using OnTriggerCallback = std::function<void()>;
	void SetOnTriggerCallback(OnTriggerCallback callback);

	BOOL Create(const CString& caption, const CRect& rect, CWnd* pParentWnd, UINT nID);

protected:
	virtual void DrawButton(Gdiplus::Graphics& graphics, const CRect& rect) override;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

private:
	Gdiplus::Image* m_pIconImage;
	bool m_bOwnsIconImage;
	bool m_bSelected;
	CString m_name;
	OnTriggerCallback m_onTriggerCallback;
};
