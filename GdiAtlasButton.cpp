#include "pch.h"
#include "GdiAtlasButton.h"
#include "PathResolver.h"

IMPLEMENT_DYNAMIC(CGdiAtlasButton, CGdiButton)

CGdiAtlasButton::CGdiAtlasButton()
	: m_pAtlasImage(nullptr)
	, m_bOwnsAtlasImage(false)
	, m_buttonWidth(0)
	, m_buttonHeight(0)
	, m_pExtractedNormal(nullptr)
	, m_pExtractedHover(nullptr)
	, m_pExtractedPressed(nullptr)
{
}

CGdiAtlasButton::~CGdiAtlasButton()
{
	CleanupExtractedImages();
	
	if (m_bOwnsAtlasImage && m_pAtlasImage)
	{
		delete m_pAtlasImage;
		m_pAtlasImage = nullptr;
	}
}

void CGdiAtlasButton::SetAtlasImage(const CString& atlasImagePath, int buttonWidth, int buttonHeight)
{
	CleanupExtractedImages();
	
	if (m_bOwnsAtlasImage && m_pAtlasImage)
	{
		delete m_pAtlasImage;
		m_pAtlasImage = nullptr;
	}

	CString fullPath = PathResolver::GetInstance().GetResourcePath(atlasImagePath);
	m_pAtlasImage = Gdiplus::Image::FromFile(fullPath);
	m_bOwnsAtlasImage = true;
	m_buttonWidth = buttonWidth;
	m_buttonHeight = buttonHeight;

	if (m_pAtlasImage && m_pAtlasImage->GetLastStatus() == Gdiplus::Ok)
	{
		ExtractImagesFromAtlas();
	}
}

void CGdiAtlasButton::SetAtlasImageFromMemory(Gdiplus::Image* pAtlasImage, int buttonWidth, int buttonHeight)
{
	CleanupExtractedImages();
	
	if (m_bOwnsAtlasImage && m_pAtlasImage)
	{
		delete m_pAtlasImage;
	}

	m_pAtlasImage = pAtlasImage;
	m_bOwnsAtlasImage = false;
	m_buttonWidth = buttonWidth;
	m_buttonHeight = buttonHeight;

	if (m_pAtlasImage && m_pAtlasImage->GetLastStatus() == Gdiplus::Ok)
	{
		ExtractImagesFromAtlas();
	}
}

void CGdiAtlasButton::ExtractImagesFromAtlas()
{
	if (!m_pAtlasImage || m_buttonWidth <= 0 || m_buttonHeight <= 0)
		return;

	// 아틀라스 높이 확인 (2-state 또는 3-state 자동 감지)
	UINT atlasHeight = m_pAtlasImage->GetHeight();
	bool is2State = (atlasHeight == m_buttonHeight * 2);

	// Normal 이미지 추출 (0, 0, width, height)
	m_pExtractedNormal = new Gdiplus::Bitmap(m_buttonWidth, m_buttonHeight, PixelFormat32bppARGB);
	Gdiplus::Graphics gNormal(m_pExtractedNormal);
	gNormal.DrawImage(m_pAtlasImage, 0, 0, 0, 0, m_buttonWidth, m_buttonHeight, Gdiplus::UnitPixel);

	if (is2State)
	{
		// 2-state 아틀라스: Normal, Click만 있음
		// Hover는 Normal 사용, Pressed는 Click(두 번째) 사용
		m_pExtractedHover = new Gdiplus::Bitmap(m_buttonWidth, m_buttonHeight, PixelFormat32bppARGB);
		Gdiplus::Graphics gHover(m_pExtractedHover);
		gHover.DrawImage(m_pAtlasImage, 0, 0, 0, 0, m_buttonWidth, m_buttonHeight, Gdiplus::UnitPixel);

		m_pExtractedPressed = new Gdiplus::Bitmap(m_buttonWidth, m_buttonHeight, PixelFormat32bppARGB);
		Gdiplus::Graphics gPressed(m_pExtractedPressed);
		gPressed.DrawImage(m_pAtlasImage, 0, 0, 0, m_buttonHeight, m_buttonWidth, m_buttonHeight, Gdiplus::UnitPixel);
	}
	else
	{
		// 3-state 아틀라스: Normal, Hover, Pressed
		m_pExtractedHover = new Gdiplus::Bitmap(m_buttonWidth, m_buttonHeight, PixelFormat32bppARGB);
		Gdiplus::Graphics gHover(m_pExtractedHover);
		gHover.DrawImage(m_pAtlasImage, 0, 0, 0, m_buttonHeight, m_buttonWidth, m_buttonHeight, Gdiplus::UnitPixel);

		m_pExtractedPressed = new Gdiplus::Bitmap(m_buttonWidth, m_buttonHeight, PixelFormat32bppARGB);
		Gdiplus::Graphics gPressed(m_pExtractedPressed);
		gPressed.DrawImage(m_pAtlasImage, 0, 0, 0, m_buttonHeight * 2, m_buttonWidth, m_buttonHeight, Gdiplus::UnitPixel);
	}

	// 부모 클래스에 추출된 이미지 설정
	SetImageFromPath(m_pExtractedNormal, m_pExtractedHover, m_pExtractedPressed);
}

void CGdiAtlasButton::CleanupExtractedImages()
{
	if (m_pExtractedNormal)
	{
		delete m_pExtractedNormal;
		m_pExtractedNormal = nullptr;
	}
	
	if (m_pExtractedHover)
	{
		delete m_pExtractedHover;
		m_pExtractedHover = nullptr;
	}
	
	if (m_pExtractedPressed)
	{
		delete m_pExtractedPressed;
		m_pExtractedPressed = nullptr;
	}
}
