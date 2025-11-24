
// ClickerGameView.cpp: CClickerGameView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ClickerGame.h"
#endif

#include "ClickerGameDoc.h"
#include "ClickerGameView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClickerGameView

IMPLEMENT_DYNCREATE(CClickerGameView, CView)

BEGIN_MESSAGE_MAP(CClickerGameView, CView)
END_MESSAGE_MAP()

// CClickerGameView 생성/소멸

CClickerGameView::CClickerGameView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CClickerGameView::~CClickerGameView()
{
}

BOOL CClickerGameView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CClickerGameView 그리기

void CClickerGameView::OnDraw(CDC* /*pDC*/)
{
	CClickerGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CClickerGameView 진단

#ifdef _DEBUG
void CClickerGameView::AssertValid() const
{
	CView::AssertValid();
}

void CClickerGameView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CClickerGameDoc* CClickerGameView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CClickerGameDoc)));
	return (CClickerGameDoc*)m_pDocument;
}
#endif //_DEBUG


// CClickerGameView 메시지 처리기
