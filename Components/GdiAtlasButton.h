#pragma once
#include "GdiButton.h"

class CGdiAtlasButton : public CGdiButton
{
	DECLARE_DYNAMIC(CGdiAtlasButton)

public:
	CGdiAtlasButton();
	virtual ~CGdiAtlasButton();

	void SetAtlasImage(const CString& atlasImagePath, int buttonWidth, int buttonHeight);
	void SetAtlasImageFromMemory(Gdiplus::Image* pAtlasImage, int buttonWidth, int buttonHeight);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

private:
	void ExtractImagesFromAtlas();
	void CleanupExtractedImages();
	void UpdateImageForState();

	enum ButtonState
	{
		Normal,
		Hover,
		Pressed,
		Disabled
	};

	Gdiplus::Image* m_pAtlasImage;
	bool m_bOwnsAtlasImage;
	int m_buttonWidth;
	int m_buttonHeight;
	
	Gdiplus::Image* m_pExtractedNormal;
	Gdiplus::Image* m_pExtractedHover;
	Gdiplus::Image* m_pExtractedPressed;

	ButtonState m_state;

public:
	afx_msg void OnEnable(BOOL bEnable);
};
