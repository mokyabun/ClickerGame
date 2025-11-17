#pragma once
#include "afxdialogex.h"
#include "GdiAtlasButton.h"
#include <vector>

// 전방 선언
class CClickerGameDoc;

// CShopDialog 대화 상자

class CShopDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CShopDialog)

public:
	CShopDialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CShopDialog();

	// Document 설정
	void SetDocument(CClickerGameDoc* pDoc) { m_pDoc = pDoc; }

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHOP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	// 리소스
	Gdiplus::Image* m_pBackgroundImage;
	CDC m_memDC;
	CBitmap m_memBitmap;
	DWORD m_lastFrameTime;
	UINT_PTR m_nTimerID;

	// 게임 문서
	CClickerGameDoc* m_pDoc;

	// 상점 아이템 버튼 (6개)
	std::vector<CGdiAtlasButton*> m_shopButtons;
	int m_hoveredItemId;  // 현재 호버 중인 아이템 ID (-1: 없음)

	// 닫기 버튼
	CGdiAtlasButton m_btnClose;

	// 드래그 관련
	bool m_bDragging;
	CPoint m_dragStartPoint;

	// 메시지 핸들러
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	virtual void PostNcDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	// 렌더링
	void RenderAllLayers();
	void UpdateHoveredItem(CPoint point);

	// 버튼 콜백
	void OnItemButtonClick(int itemId);
	void OnItemButtonHover(int itemId, bool isHovering);
	afx_msg void OnCloseButtonClick();
};
