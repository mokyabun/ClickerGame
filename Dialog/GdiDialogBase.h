#pragma once
#include "afxdialogex.h"
#include "GdiButton.h"
#include <vector>
#include <gdiplus.h>

// GDI+ 렌더링을 위한 다이얼로그 베이스 클래스
// 1. CDialogEx를 상속
// 2. SetTimer을 사용해서 1초에 60번 Draw (16ms 간격)
// 3. 그리는 순서에 따라 Layer 처리 (childLayers는 없이)

class GdiDialogBase : public CDialogEx
{
	DECLARE_DYNAMIC(GdiDialogBase)

public:
	GdiDialogBase(UINT nIDTemplate, CWnd* pParent = nullptr);
	virtual ~GdiDialogBase();

protected:
	// 파생 클래스에서 재정의할 가상 함수들
	
	// 초기화 시 호출 (다이얼로그 크기, 윈도우 스타일 설정 등)
	virtual void OnGdiInitialize() {}
	
	// 메인 렌더링 함수 - 파생 클래스에서 실제 그리기 내용 구현
	// graphics: GDI+ 그리기 객체
	// deltaTime: 이전 프레임과의 시간차 (초 단위)
	virtual void OnRenderContent(Gdiplus::Graphics& graphics, float deltaTime) = 0;
	
	// 업데이트 로직 (렌더링 전에 호출됨)
	virtual void OnUpdateLogic(float deltaTime) {}

	// 메모리 DC 및 비트맵 초기화
	void InitializeRenderBuffer(int width, int height);

	// MFC 가상 함수
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;

	DECLARE_MESSAGE_MAP()

	// 메시지 핸들러
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

private:
	// 렌더링 버퍼
	CDC m_memDC;              // 메모리 DC
	CBitmap m_memBitmap;      // 메모리 비트맵
	int m_bufferWidth;        // 버퍼 너비
	int m_bufferHeight;       // 버퍼 높이
	
	// 타이머
	UINT_PTR m_nTimerID;      // 60 FPS 타이머 (16ms)
	DWORD m_lastFrameTime;    // 마지막 프레임 시간 (deltaTime 계산용)
	
	// 렌더링 함수
	void RenderFrame();
};
