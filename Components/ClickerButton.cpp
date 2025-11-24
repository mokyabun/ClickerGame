#include "pch.h"
#include "ClickerButton.h"

IMPLEMENT_DYNAMIC(CClickerButton, CGdiButton)

CClickerButton::CClickerButton()
	: m_currentScale(1.0f)
	, m_targetScale(1.0f)
	, m_animElapsed(0.0f)
	, m_isClickAnim(false)
{
}

CClickerButton::~CClickerButton()
{
}

BEGIN_MESSAGE_MAP(CClickerButton, CGdiButton)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()
void CClickerButton::Draw(Gdiplus::Graphics& graphics, CPoint offset, float deltaTime)
{
	// 애니메이션 업데이트
	if (m_currentScale != m_targetScale)
	{
		m_animElapsed += deltaTime;
		
		float duration = m_isClickAnim ? m_clickAnimDuration : m_animDuration;
		
		if (m_animElapsed >= duration)
		{
			// 애니메이션 완료
			m_currentScale = m_targetScale;
			m_animElapsed = 0.0f;
			m_isClickAnim = false;
		}
		else
		{
			// easing 함수 선택
			float t = m_animElapsed / duration;
			
			if (m_isClickAnim)
			{
				// 클릭 애니메이션: 작아졌다가 -> 더 커지고 -> 원래대로
				// 0.0 ~ 0.5: 작은 크기(0.85) -> 큰 크기(1.1)
				// 0.5 ~ 1.0: 큰 크기(1.1) -> 목표 크기
				if (t < 0.5f)
				{
					float t1 = t / 0.5f;  // 0.0 ~ 1.0으로 정규화
					float eased = EaseOutBack(t1);
					m_currentScale = m_clickScale + (1.05f - m_clickScale) * eased;
				}
				else
				{
					float t2 = (t - 0.5f) / 0.5f;  // 0.0 ~ 1.0으로 정규화
					float eased = EaseOutBack(t2);
					m_currentScale = 1.05f + (m_targetScale - 1.05f) * eased;
				}
			}
			else
			{
				// 호버 애니메이션
				float eased = EaseOutElastic(t);
				float startScale = (m_targetScale == m_hoverScale) ? m_normalScale : m_hoverScale;
				m_currentScale = startScale + (m_targetScale - startScale) * eased;
			}
		}
	}
	
	// 렌더링
	CRect rect;
	GetClientRect(&rect);

	// hover scale을 고려한 기본 크기 계산
	const float baseScale = 1.0f / m_hoverScale;
	int baseWidth = (int)(rect.Width() * baseScale);
	int baseHeight = (int)(rect.Height() * baseScale);

	// 스케일 적용한 크기 계산
	int scaledWidth = (int)(baseWidth * m_currentScale);
	int scaledHeight = (int)(baseHeight * m_currentScale);
	int offsetX = (rect.Width() - scaledWidth) / 2;
	int offsetY = (rect.Height() - scaledHeight) / 2;

	// offset 적용
	Gdiplus::Matrix matrix;
	graphics.GetTransform(&matrix);
	graphics.TranslateTransform((Gdiplus::REAL)offset.x, (Gdiplus::REAL)offset.y);
	
	// 부모 클래스의 DrawButton 호출 (스케일링된 영역 전달)
	// 이제 이미지와 텍스트(GdiText)가 모두 여기서 처리됨
	CRect scaledRect(offsetX, offsetY, offsetX + scaledWidth, offsetY + scaledHeight);
	DrawButton(graphics, scaledRect);
	
	graphics.SetTransform(&matrix);
}

void CClickerButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// 부모 클래스의 마우스 추적 처리
	CGdiButton::OnMouseMove(nFlags, point);

	// Hover 상태이고 아직 hover 애니메이션이 실행되지 않았다면 시작
	if (m_bMouseTracking && m_targetScale != m_hoverScale)
	{
		m_targetScale = m_hoverScale;
		m_animElapsed = 0.0f;  // 애니메이션 재시작
	}
}

void CClickerButton::OnMouseLeave()
{
	// 부모 클래스 처리
	CGdiButton::OnMouseLeave();

	// hover 종료 애니메이션 시작
	m_targetScale = m_normalScale;
	m_animElapsed = 0.0f;  // 애니메이션 재시작
}

float CClickerButton::EaseOutElastic(float t)
{
	const float c4 = (2.0f * 3.14159265f) / 3.0f;

	if (t == 0.0f)
		return 0.0f;
	
	if (t == 1.0f)
		return 1.0f;

	return powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
}

float CClickerButton::EaseOutBack(float t)
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
}

void CClickerButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// 부모 클래스 처리 (클릭 이벤트 발생)
	CGdiButton::OnLButtonUp(nFlags, point);

	// 클릭 애니메이션 시작 (작아졌다가 다시 커지는 효과)
	m_currentScale = m_clickScale;  // 즉시 작아짐
	m_targetScale = m_bMouseTracking ? m_hoverScale : m_normalScale;  // 마우스가 여전히 위에 있으면 hover 크기로, 아니면 normal로
	m_animElapsed = 0.0f;
	m_isClickAnim = true;
}

void CClickerButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// 더블 클릭도 일반 클릭처럼 처리 (빠른 클릭 시 씹힘 방지)
	OnLButtonUp(nFlags, point);
	OnLButtonUp(nFlags, point);
}
