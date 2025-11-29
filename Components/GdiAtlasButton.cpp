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
	, m_state(Normal)
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

BEGIN_MESSAGE_MAP(CGdiAtlasButton, CGdiButton)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

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
		UpdateImageForState();
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
		UpdateImageForState();
	}
}

void CGdiAtlasButton::ExtractImagesFromAtlas()
{
	if (!m_pAtlasImage || m_buttonWidth <= 0 || m_buttonHeight <= 0)
		return;

	// 아틀라스 높이 확인 (2-state 또는 3-state 자동 감지)
	UINT atlasHeight = m_pAtlasImage->GetHeight();
	
	// 안전 장치: 높이가 버튼 높이보다 작으면 실패 처리
	if (atlasHeight < (UINT)m_buttonHeight)
		return;

	bool is2State = (atlasHeight < (UINT)m_buttonHeight * 3); // 3배보다 작으면 2-state로 간주 (또는 1-state)

	// Normal 이미지 추출 (0, 0, width, height)
	m_pExtractedNormal = new Gdiplus::Bitmap(m_buttonWidth, m_buttonHeight, PixelFormat32bppARGB);
	Gdiplus::Graphics gNormal(m_pExtractedNormal);
	gNormal.DrawImage(m_pAtlasImage, 0, 0, 0, 0, m_buttonWidth, m_buttonHeight, Gdiplus::UnitPixel);

	if (is2State)
	{
		// 2-state 아틀라스: Normal, Click (Pressed)
		// Hover는 Normal과 동일하게 처리
		m_pExtractedHover = nullptr; // Hover는 Normal 공유

		if (atlasHeight >= (UINT)m_buttonHeight * 2)
		{
			m_pExtractedPressed = new Gdiplus::Bitmap(m_buttonWidth, m_buttonHeight, PixelFormat32bppARGB);
			Gdiplus::Graphics gPressed(m_pExtractedPressed);
			gPressed.DrawImage(m_pAtlasImage, 0, 0, 0, m_buttonHeight, m_buttonWidth, m_buttonHeight, Gdiplus::UnitPixel);
		}
		else
		{
			// 1-state인 경우 Pressed도 Normal 공유
			m_pExtractedPressed = nullptr;
		}
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
}

void CGdiAtlasButton::CleanupExtractedImages()
{
	if (m_pExtractedNormal)
	{
		delete m_pExtractedNormal;
		m_pExtractedNormal = nullptr;
	}
	
	// Hover가 Normal과 같은 포인터를 공유하는 경우가 있으므로 확인
	if (m_pExtractedHover && m_pExtractedHover != m_pExtractedNormal)
	{
		delete m_pExtractedHover;
		m_pExtractedHover = nullptr;
	}
	else if (m_pExtractedHover)
	{
		m_pExtractedHover = nullptr;
	}
	
	if (m_pExtractedPressed)
	{
		delete m_pExtractedPressed;
		m_pExtractedPressed = nullptr;
	}
}

void CGdiAtlasButton::UpdateImageForState()
{
	Gdiplus::Image* pCurrentImage = nullptr;
	
	switch (m_state) {
		case Hover: {
			pCurrentImage = m_pExtractedHover ? m_pExtractedHover : m_pExtractedNormal;
			break;
		}
		case Pressed: {
			pCurrentImage = m_pExtractedPressed ? m_pExtractedPressed : m_pExtractedNormal;
			break;
		}
		case Disabled: {
			pCurrentImage = m_pExtractedPressed ? m_pExtractedPressed : m_pExtractedNormal;
			break;
		}
		default: {
			pCurrentImage = m_pExtractedNormal;
			break;
		}
	}

	CGdiButton::SetImage(pCurrentImage, false);
}

void CGdiAtlasButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_state == Disabled)
		return;

	CGdiButton::OnMouseMove(nFlags, point);

	ButtonState newState = m_state;

	if (nFlags & MK_LBUTTON)
	{
		CRect rect;
		GetClientRect(&rect);
		newState = rect.PtInRect(point) ? Pressed : Hover;
	}
	else if (m_bMouseTracking && m_state == Normal)
	{
		newState = Hover;
	}

	if (newState != m_state)
	{
		m_state = newState;
		UpdateImageForState();
	}
}

void CGdiAtlasButton::OnMouseLeave()
{
	if (m_state == Disabled)
		return;

	CGdiButton::OnMouseLeave();

	if (m_state != Pressed)
	{
		m_state = Normal;
		UpdateImageForState();
	}
}

void CGdiAtlasButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_state == Disabled)
		return;

	// 부모 클래스의 처리 호출
	CGdiButton::OnLButtonDown(nFlags, point);

	m_state = Pressed;
	UpdateImageForState();
}

void CGdiAtlasButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_state == Disabled)
		return;

	// 부모 클래스의 처리 호출 (클릭 이벤트 발생)
	CGdiButton::OnLButtonUp(nFlags, point);

	CRect rect;
	GetClientRect(&rect);
	m_state = rect.PtInRect(point) ? Hover : Normal;
	UpdateImageForState();
}

void CGdiAtlasButton::OnEnable(BOOL bEnable)
{
	CGdiButton::OnEnable(bEnable);

	if (bEnable)
	{
		// 활성화되면 Normal 상태로 복귀 (마우스 위치 체크는 별도로 필요할 수 있음)
		m_state = Normal;
	}
	else
	{
		// 비활성화되면 Disabled 상태로 변경
		m_state = Disabled;
	}
	UpdateImageForState();
}
