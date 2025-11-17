#pragma once
#include "GdiButton.h"

// 클리커 게임용 애니메이션 버튼
class CClickerButton : public CGdiButton
{
	DECLARE_DYNAMIC(CClickerButton)

public:
	CClickerButton();
	virtual ~CClickerButton();

	// 애니메이션 설정
	void SetHoverScale(float scale) { m_hoverScale = scale; }
	void SetAnimationDuration(DWORD duration) { m_animDuration = duration; }
	void SetClickScale(float scale) { m_clickScale = scale; }
	void SetClickAnimDuration(DWORD duration) { m_clickAnimDuration = duration; }

	// Draw 함수 선언
	void Draw(Gdiplus::Graphics& graphics, CPoint offset, float deltaTime);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

private:
	float EaseOutElastic(float t);  // 꿀렁이는 느낌의 easing
	float EaseOutBack(float t);     // 클릭 애니메이션용 easing

	// 애니메이션
	float m_currentScale;      // 현재 스케일
	float m_targetScale;       // 목표 스케일
	float m_animElapsed;       // 애니메이션 경과 시간
	bool m_isClickAnim;        // 클릭 애니메이션 실행 중인지 여부
	
	// 조절 가능한 상수들
	float m_normalScale = 1.0f;
	float m_hoverScale = 1.1f;        // 호버 시 크기 배율 (1.0 = 변화 없음)
	float m_clickScale = 0.85f;       // 클릭 시 최소 크기 배율
	float m_animDuration = 0.6f;      // 애니메이션 지속 시간 (초 단위)
	float m_clickAnimDuration = 0.4f; // 클릭 애니메이션 지속 시간 (초 단위)
};
