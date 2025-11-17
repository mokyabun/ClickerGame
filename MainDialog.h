#pragma once
#include "afxdialogex.h"
#include "GdiAtlasButton.h"
#include "GdiIconButton.h"
#include "ClickerButton.h"
#include "GameCore.h"
#include "ParticleSystem.h"
#include <vector>

// 전방 선언
class CClickerGameDoc;
class CShopDialog;

// CMainDialog 대화 상자

class CMainDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDialog)

public:
	CMainDialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMainDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	Gdiplus::Image* m_pBackgroundImage;
	CGdiAtlasButton m_btnClose;
	CGdiIconButton m_btnShop;
	CGdiIconButton m_btnUpgrade;
	CGdiIconButton m_btnSettings;
	CClickerButton m_btnVirus;
	
	// 게임 코어 (문서로부터 참조)
	CClickerGameDoc* m_pDoc;
	CString m_saveFilePath;
	
	// Modeless 다이얼로그
	CShopDialog* m_pShopDialog;
	
	// 중앙 렌더링 시스템
	CDC m_memDC;              // 고정 메모리 DC (800x600)
	CBitmap m_memBitmap;      // 고정 메모리 비트맵
	UINT_PTR m_nTimerID;      // 60 FPS 타이머
	UINT_PTR m_nAutoSaveTimerID; // 30초 자동 저장 타이머
	DWORD m_lastFrameTime;    // 마지막 프레임 시간 (deltaTime 계산용)
	std::vector<CGdiButton*> m_childLayers;  // 그리기 순서대로 자식 레이어
	
	// 파티클 시스템
	ParticleSystem m_particleSystem;
	
	// 드래그 관련
	bool m_bDragging;
	CPoint m_dragStartPoint;
	
	void RenderAllLayers();   // 모든 레이어 렌더링
	void SaveGameState();     // 게임 상태 저장
	void LoadGameState();     // 게임 상태 로드
	
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCloseButtonClick();
	afx_msg void OnVirusButtonClick();
	afx_msg void OnShopButtonClick();
	afx_msg void OnShopButtonTrigger();
	afx_msg void OnUpgradeButtonClick();
	afx_msg void OnUpgradeButtonTrigger();
	afx_msg void OnSettingsButtonClick();
	afx_msg void OnSettingsButtonTrigger();
};
