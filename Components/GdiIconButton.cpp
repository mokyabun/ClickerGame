#include "pch.h"
#include "GdiIconButton.h"
#include "PathResolver.h"

IMPLEMENT_DYNAMIC(CGdiIconButton, CGdiButton)

CGdiIconButton::CGdiIconButton()
	: m_pIconImage(nullptr)
	, m_bOwnsIconImage(false)
	, m_bSelected(false)
	, m_onTriggerCallback(nullptr)
{
}

CGdiIconButton::~CGdiIconButton()
{
	if (m_bOwnsIconImage && m_pIconImage)
	{
		delete m_pIconImage;
		m_pIconImage = nullptr;
	}
}

BEGIN_MESSAGE_MAP(CGdiIconButton, CGdiButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

BOOL CGdiIconButton::Create(const CString& caption, const CRect& rect, CWnd* pParentWnd, UINT nID)
{
	// 부모 클래스의 Create 호출
	BOOL result = CGdiButton::Create(caption, rect, pParentWnd, nID);
	
	if (result)
	{
		// 더블클릭을 받을 수 있도록 스타일 추가
		ModifyStyle(0, BS_NOTIFY);
	}
	
	return result;
}

void CGdiIconButton::SetIconImage(const CString& iconImagePath)
{
	if (m_bOwnsIconImage && m_pIconImage)
	{
		delete m_pIconImage;
		m_pIconImage = nullptr;
	}

	CString fullPath = PathResolver::GetInstance().GetResourcePath(iconImagePath);
	m_pIconImage = Gdiplus::Image::FromFile(fullPath);
	m_bOwnsIconImage = true;

	if (m_pIconImage == nullptr || m_pIconImage->GetLastStatus() != Gdiplus::Ok)
	{
		CString msg;
		msg.Format(L"아이콘 이미지를 로드할 수 없습니다!\n경로: %s", fullPath);
		AfxMessageBox(msg);
	}

	Invalidate();
}

void CGdiIconButton::SetIconImageFromPath(Gdiplus::Image* pIconImage)
{
	if (m_bOwnsIconImage && m_pIconImage)
	{
		delete m_pIconImage;
		m_pIconImage = nullptr;
	}

	m_bOwnsIconImage = false;
	m_pIconImage = pIconImage;
	Invalidate();
}

void CGdiIconButton::SetName(const CString& name)
{
	m_name = name;
	// GdiText에도 설정 (그림자 효과 등을 위해)
	m_gdiText.SetText(name);
	m_gdiText.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_gdiText.SetLineAlignment(Gdiplus::StringAlignmentNear);
	m_gdiText.SetShadowStyle(GdiText::ShadowStyle::DropShadow); // 아이콘 버튼은 기본적으로 그림자 적용
	
	Invalidate();
}

void CGdiIconButton::SetSelected(bool selected)
{
	m_bSelected = selected;
	Invalidate();
}

void CGdiIconButton::SetOnTriggerCallback(OnTriggerCallback callback)
{
	m_onTriggerCallback = callback;
}

void CGdiIconButton::DrawButton(Gdiplus::Graphics& graphics, const CRect& rect)
{
	if (!m_pIconImage)
		return;

	// 아이콘 영역 (48x48)
	const int iconWidth = 48;
	const int iconHeight = 48;
	
	// 아이콘을 중앙 상단에 그리기
	int iconX = rect.left + (rect.Width() - iconWidth) / 2;
	int iconY = rect.top + 5;  // 위에서 5px 떨어진 위치

	// 선택 상태일 때 전체 버튼 영역에 파란색 배경 그리기 (Windows 스타일)
	if (m_bSelected)
	{
		Gdiplus::SolidBrush selectionBrush(Gdiplus::Color(100, 51, 153, 255)); // 반투명 파란색
		Gdiplus::RectF selectionRect((Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top, 
			(Gdiplus::REAL)rect.Width(), (Gdiplus::REAL)rect.Height());
		graphics.FillRectangle(&selectionBrush, selectionRect);
	}

	// 단일 이미지 전체를 그리기
	Gdiplus::RectF destRect((Gdiplus::REAL)iconX, (Gdiplus::REAL)iconY, 
		(Gdiplus::REAL)iconWidth, (Gdiplus::REAL)iconHeight);

	graphics.DrawImage(m_pIconImage, destRect);

	// 텍스트 그리기 (아이콘 아래)
	if (!m_name.IsEmpty())
	{
		// 텍스트 영역 계산
		Gdiplus::RectF textRect((Gdiplus::REAL)rect.left, 
			(Gdiplus::REAL)(iconY + iconHeight + 2),
			(Gdiplus::REAL)rect.Width(), 
			(Gdiplus::REAL)(rect.Height() - (iconY - rect.top) - iconHeight - 2));

		// GdiText를 사용하여 그리기 (그림자 효과 자동 적용)
		m_gdiText.Draw(graphics, textRect);
	}
}

void CGdiIconButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 한 번 클릭 시 선택 토글
	m_bSelected = !m_bSelected;
	Invalidate();

	// 부모 클래스의 OnClick 콜백도 호출
	CGdiButton::OnLButtonDown(nFlags, point);
}

void CGdiIconButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// 더블클릭 시 트리거 콜백 호출
	if (m_onTriggerCallback)
	{
		m_onTriggerCallback();
	}

	CButton::OnLButtonDblClk(nFlags, point);
}
