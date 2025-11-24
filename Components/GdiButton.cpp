#include "pch.h"
#include "GdiButton.h"
#include "PathResolver.h"

IMPLEMENT_DYNAMIC(CGdiButton, CButton)

CGdiButton::CGdiButton()
	: m_state(Normal)
	, m_bMouseTracking(false)
	, m_pNormalImage(nullptr)
	, m_pHoverImage(nullptr)
	, m_pPressedImage(nullptr)
	, m_bOwnsImages(false)
	, m_onClickCallback(nullptr)
{
	// GdiText 초기화 (기본값 설정)
	m_gdiText.SetColor(Gdiplus::Color(255, 0, 0, 0));
	m_gdiText.SetFont(L"맑은 고딕", 12.0f, Gdiplus::FontStyleRegular);
	m_gdiText.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_gdiText.SetLineAlignment(Gdiplus::StringAlignmentCenter);
}

CGdiButton::~CGdiButton()
{
	if (m_bOwnsImages)
	{
		if (m_pNormalImage)
			delete m_pNormalImage;
		if (m_pHoverImage)
			delete m_pHoverImage;
		if (m_pPressedImage)
			delete m_pPressedImage;
	}
}

BEGIN_MESSAGE_MAP(CGdiButton, CButton)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CGdiButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// BS_OWNERDRAW 스타일을 위한 필수 메서드
	// WM_PAINT에서 그리기를 처리하므로 여기서는 아무것도 하지 않음
}

BOOL CGdiButton::Create(const CString& caption, const CRect& rect, CWnd* pParentWnd, UINT nID)
{
	// BS_OWNERDRAW 스타일로 버튼 생성 (WS_EX_TRANSPARENT 추가)
	BOOL result = CButton::Create(caption, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		rect, pParentWnd, nID);
	
	if (result)
	{
		// 투명 배경을 위한 확장 스타일 추가
		ModifyStyleEx(0, WS_EX_TRANSPARENT);
		
		if (caption.IsEmpty() == FALSE)
		{
			m_gdiText.SetText(caption);
		}
	}
	
	return result;
}

void CGdiButton::SetImage(const CString& normalImage, const CString& hoverImage, const CString& pressedImage)
{
	m_normalImagePath = normalImage;
	m_hoverImagePath = hoverImage;
	m_pressedImagePath = pressedImage;
	
	m_bOwnsImages = true;
	LoadImages();
}

void CGdiButton::SetImageFromPath(Gdiplus::Image* pNormalImage, Gdiplus::Image* pHoverImage, Gdiplus::Image* pPressedImage)
{
	if (m_bOwnsImages)
	{
		if (m_pNormalImage)
			delete m_pNormalImage;
		if (m_pHoverImage)
			delete m_pHoverImage;
		if (m_pPressedImage)
			delete m_pPressedImage;
	}

	m_bOwnsImages = false;
	m_pNormalImage = pNormalImage;
	m_pHoverImage = pHoverImage;
	m_pPressedImage = pPressedImage;
}

void CGdiButton::SetText(const CString& text)
{
	m_gdiText.SetText(text);
}

void CGdiButton::SetTextColor(Gdiplus::Color color)
{
	m_gdiText.SetColor(color);
}

void CGdiButton::SetFont(const CString& fontName, float fontSize, INT fontStyle)
{
	m_gdiText.SetFont(fontName, fontSize, (Gdiplus::FontStyle)fontStyle);
}

void CGdiButton::SetPosition(int x, int y)
{
	CRect rect;
	GetWindowRect(&rect);
	GetParent()->ScreenToClient(&rect);
	SetWindowPos(nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void CGdiButton::SetSize(int width, int height)
{
	SetWindowPos(nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

void CGdiButton::SetRect(const CRect& rect)
{
	SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
}

void CGdiButton::SetOnClickCallback(OnClickCallback callback)
{
	m_onClickCallback = callback;
}

void CGdiButton::LoadImages()
{
	// 기존 이미지 해제
	if (m_pNormalImage)
	{
		delete m_pNormalImage;
		m_pNormalImage = nullptr;
	}
	if (m_pHoverImage)
	{
		delete m_pHoverImage;
		m_pHoverImage = nullptr;
	}
	if (m_pPressedImage)
	{
		delete m_pPressedImage;
		m_pPressedImage = nullptr;
	}

	// 새 이미지 로드
	if (!m_normalImagePath.IsEmpty())
	{
		CString fullPath = PathResolver::GetInstance().GetResourcePath(m_normalImagePath);
		m_pNormalImage = Gdiplus::Image::FromFile(fullPath);
	}

	if (!m_hoverImagePath.IsEmpty())
	{
		CString fullPath = PathResolver::GetInstance().GetResourcePath(m_hoverImagePath);
		m_pHoverImage = Gdiplus::Image::FromFile(fullPath);
	}

	if (!m_pressedImagePath.IsEmpty())
	{
		CString fullPath = PathResolver::GetInstance().GetResourcePath(m_pressedImagePath);
		m_pPressedImage = Gdiplus::Image::FromFile(fullPath);
	}
}

void CGdiButton::DrawButton(Gdiplus::Graphics& graphics, const CRect& rect)
{
	// 현재 상태에 맞는 이미지 선택
	Gdiplus::Image* pCurrentImage = nullptr;
	switch (m_state)
	{
	case Hover:
		pCurrentImage = m_pHoverImage ? m_pHoverImage : m_pNormalImage;
		break;
	case Pressed:
		pCurrentImage = m_pPressedImage ? m_pPressedImage : m_pNormalImage;
		break;
	case Normal:
	default:
		pCurrentImage = m_pNormalImage;
		break;
	}

	// 이미지 그리기
	if (pCurrentImage && pCurrentImage->GetLastStatus() == Gdiplus::Ok)
	{
		graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		graphics.DrawImage(pCurrentImage, rect.left, rect.top, rect.Width(), rect.Height());
	}
	else
	{
		// 이미지가 없으면 기본 버튼 그리기
		Gdiplus::Color fillColor;
		switch (m_state)
		{
		case Hover:
			fillColor = Gdiplus::Color(255, 220, 220, 220);
			break;
		case Pressed:
			fillColor = Gdiplus::Color(255, 180, 180, 180);
			break;
		case Normal:
		default:
			fillColor = Gdiplus::Color(255, 240, 240, 240);
			break;
		}

		Gdiplus::SolidBrush brush(fillColor);
		graphics.FillRectangle(&brush, rect.left, rect.top, rect.Width(), rect.Height());

		// 테두리
		Gdiplus::Pen pen(Gdiplus::Color(255, 128, 128, 128), 1.0f);
		graphics.DrawRectangle(&pen, rect.left, rect.top, rect.Width() - 1, rect.Height() - 1);
	}

	// GdiText를 사용하여 텍스트 그리기
	Gdiplus::RectF layoutRect((Gdiplus::REAL)rect.left, (Gdiplus::REAL)rect.top, 
		(Gdiplus::REAL)rect.Width(), (Gdiplus::REAL)rect.Height());
	
	m_gdiText.Draw(graphics, layoutRect);
}

void CGdiButton::OnPaint()
{
	// 중앙 렌더링으로 이동하여 OnPaint 비활성화
	// CPaintDC는 생성해야 WM_PAINT 메시지 처리 완료
	CPaintDC dc(this);
}

void CGdiButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_state = Pressed;
	SetCapture();
}

void CGdiButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_state == Pressed)
	{
		CRect rect;
		GetClientRect(&rect);
		CPoint pt(point);

		m_state = rect.PtInRect(pt) ? Hover : Normal;

		// OnLButtonDown에서 눌렀다면 무조건 클릭으로 처리 (클릭이 씹히는 현상 방지)
		// 클릭 이벤트 발생
		if (m_onClickCallback)
		{
			m_onClickCallback();
		}
		else
		{
			// 부모에게 BN_CLICKED 메시지 전송
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
		}
	}
	
	ReleaseCapture();
}

void CGdiButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bMouseTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		tme.dwHoverTime = HOVER_DEFAULT;
		TrackMouseEvent(&tme);
		m_bMouseTracking = true;

		if (m_state == Normal)
		{
			m_state = Hover;
		}
	}

	// 마우스 버튼이 눌린 상태에서 이동
	if (nFlags & MK_LBUTTON)
	{
		CRect rect;
		GetClientRect(&rect);
		CPoint pt(point);
		
		ButtonState newState = rect.PtInRect(pt) ? Pressed : Hover;
		if (newState != m_state)
		{
			m_state = newState;
		}
	}

	// 기본 처리는 하지 않음 (커스텀 버튼이므로)
}

void CGdiButton::OnMouseLeave()
{
	m_bMouseTracking = false;
	
	if (m_state != Pressed)
	{
		m_state = Normal;
	}
}

BOOL CGdiButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;  // 깜박임 방지
}

void CGdiButton::Draw(Gdiplus::Graphics& graphics, CPoint offset, float deltaTime)
{
	// 좌표 저장 (그리기 오프셋)
	Gdiplus::Matrix matrix;
	graphics.GetTransform(&matrix);
	graphics.TranslateTransform((float)offset.x, (float)offset.y);

	// 버튼 그리기
	CRect rect;
	GetClientRect(&rect);
	DrawButton(graphics, rect);

	// 좌표 복원
	graphics.SetTransform(&matrix);
}

void CGdiButton::Draw(Gdiplus::Graphics& graphics, float deltaTime)
{
	if (!IsWindowVisible())
		return;

	CRect rect;
	GetWindowRect(&rect);
	
	CWnd* pParent = GetParent();
	if (pParent)
	{
		pParent->ScreenToClient(&rect);
	}

	CPoint offset(rect.left, rect.top);
	Draw(graphics, offset, deltaTime);
}
