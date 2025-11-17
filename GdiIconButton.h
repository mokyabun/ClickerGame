#pragma once
#include "GdiButton.h"

class CGdiIconButton : public CGdiButton
{
	DECLARE_DYNAMIC(CGdiIconButton)

public:
	CGdiIconButton();
	virtual ~CGdiIconButton();

	// 아이콘 이미지 설정 (48x48 단일 이미지)
	void SetIconImage(const CString& iconImagePath);
	void SetIconImageFromPath(Gdiplus::Image* pIconImage);

	// 이름 설정
	void SetName(const CString& name);

	// 선택 상태
	bool IsSelected() const { return m_bSelected; }
	void SetSelected(bool selected);
	void Deselect() { SetSelected(false); }

	// 더블클릭 트리거 콜백
	using OnTriggerCallback = std::function<void()>;
	void SetOnTriggerCallback(OnTriggerCallback callback);

	// Create 오버라이드 (BS_NOTIFY 추가)
	BOOL Create(const CString& caption, const CRect& rect, CWnd* pParentWnd, UINT nID);

protected:
	virtual void DrawButton(Gdiplus::Graphics& graphics) override;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

private:
	Gdiplus::Image* m_pIconImage;  // 48x96 아이콘 이미지
	bool m_bOwnsIconImage;         // 아이콘 이미지 메모리 소유 여부
	bool m_bSelected;              // 선택 상태
	CString m_name;                // 버튼 이름
	OnTriggerCallback m_onTriggerCallback;  // 더블클릭 콜백
};
