#pragma once
#include "GdiButton.h"

class CGdiAtlasButton : public CGdiButton
{
	DECLARE_DYNAMIC(CGdiAtlasButton)

public:
	CGdiAtlasButton();
	virtual ~CGdiAtlasButton();

	// 아틀라스 이미지 설정 (세로 방향: Normal, Hover, Pressed 순서)
	void SetAtlasImage(const CString& atlasImagePath, int buttonWidth, int buttonHeight);
	void SetAtlasImageFromMemory(Gdiplus::Image* pAtlasImage, int buttonWidth, int buttonHeight);

private:
	void ExtractImagesFromAtlas();
	void CleanupExtractedImages();

	Gdiplus::Image* m_pAtlasImage;
	bool m_bOwnsAtlasImage;
	int m_buttonWidth;
	int m_buttonHeight;
	
	Gdiplus::Image* m_pExtractedNormal;
	Gdiplus::Image* m_pExtractedHover;
	Gdiplus::Image* m_pExtractedPressed;
};
