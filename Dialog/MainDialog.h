#pragma once
#include "GdiDialogBase.h"
#include "GdiAtlasButton.h"
#include "GdiIconButton.h"
#include "ClickerButton.h"
#include "GameCore.h"
#include "ParticleSystem.h"
#include "SnowfallParticle.h"
#include "FrenzyDialog.h"
#include <vector>

// 전방 선언
class CClickerGameDoc;
class CShopDialog;
class CUpgradeDialog;

// CMainDialog 대화 상자

class CMainDialog : public GdiDialogBase
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
	
	// GdiDialogBase 가상 함수 재정의
	virtual void OnGdiInitialize() override;
	virtual void OnRenderContent(Gdiplus::Graphics& graphics, float deltaTime) override;
	virtual void OnUpdateLogic(float deltaTime) override;

	DECLARE_MESSAGE_MAP()

private:
	Gdiplus::Image* m_pBackgroundImage;
	CGdiAtlasButton m_btnClose;
	CGdiIconButton m_btnShop;
	CGdiIconButton m_btnUpgrade;
	CGdiIconButton m_btnSettings;
	CClickerButton m_btnVirus;
	
	// 텍스트 객체
	GdiText m_titleText;    // "Windows XP"
	GdiText m_currencyText; // "0 ZPC"
	GdiText m_productionText; // "초당 X ZPC"
	GdiText m_startText;    // "시작"
	GdiText m_clockText;    // 시계
	
	// 게임 코어 (문서로부터 참조)
	CClickerGameDoc* m_pDoc;
	CString m_saveFilePath;
	
	// Modeless 다이얼로그
	// Modeless 다이얼로그
	CShopDialog* m_pShopDialog;
	CUpgradeDialog* m_pUpgradeDialog;
    CFrenzyDialog* m_pFrenzyDialog;
    
    // Frenzy Spawning
    double m_frenzySpawnTimer; // Accumulator for spawning
    void SpawnFrenzyDialog();
	
	// 자동 저장 타이머
	UINT_PTR m_nAutoSaveTimerID; // 30초 자동 저장 타이머
	
	// 파티클 시스템
	ParticleSystem m_particleSystem;
	SnowfallParticleSystem m_snowfallParticles;
	
	// 드래그 관련
	bool m_bDragging;
	CPoint m_dragStartPoint;
	
	void SaveGameState();     // 게임 상태 저장
	void LoadGameState();     // 게임 상태 로드
	
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
