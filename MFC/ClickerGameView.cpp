
// ClickerGameView.cpp: CClickerGameView í´ëž˜ìŠ¤ì˜ êµ¬í˜„
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERSëŠ” ë¯¸ë¦¬ ë³´ê¸°, ì¶•ì†ŒíŒ ê·¸ë¦¼ ë° ê²€ìƒ‰ í•„í„° ì²˜ë¦¬ê¸°ë¥¼ êµ¬í˜„í•˜ëŠ” ATL í”„ë¡œì íŠ¸ì—ì„œ ì •ì˜í•  ìˆ˜ ìžˆìœ¼ë©°
// í•´ë‹¹ í”„ë¡œì íŠ¸ì™€ ë¬¸ì„œ ì½”ë“œë¥¼ ê³µìœ í•˜ë„ë¡ í•´ ì¤ë‹ˆë‹¤.
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

// CClickerGameView ìƒì„±/ì†Œë©¸

CClickerGameView::CClickerGameView() noexcept
{
	// TODO: ì—¬ê¸°ì— ìƒì„± ì½”ë“œë¥¼ ì¶”ê°€í•©ë‹ˆë‹¤.

}

CClickerGameView::~CClickerGameView()
{
}

BOOL CClickerGameView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT csë¥¼ ìˆ˜ì •í•˜ì—¬ ì—¬ê¸°ì—ì„œ
	//  Window í´ëž˜ìŠ¤ ë˜ëŠ” ìŠ¤íƒ€ì¼ì„ ìˆ˜ì •í•©ë‹ˆë‹¤.

	return CView::PreCreateWindow(cs);
}

// CClickerGameView ê·¸ë¦¬ê¸°

void CClickerGameView::OnDraw(CDC* /*pDC*/)
{
	CClickerGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ì—¬ê¸°ì— ì›ì‹œ ë°ì´í„°ì— ëŒ€í•œ ê·¸ë¦¬ê¸° ì½”ë“œë¥¼ ì¶”ê°€í•©ë‹ˆë‹¤.
}


// CClickerGameView ì§„ë‹¨

#ifdef _DEBUG
void CClickerGameView::AssertValid() const
{
	CView::AssertValid();
}

void CClickerGameView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CClickerGameDoc* CClickerGameView::GetDocument() const // ë””ë²„ê·¸ë˜ì§€ ì•Šì€ ë²„ì „ì€ ì¸ë¼ì¸ìœ¼ë¡œ ì§€ì •ë©ë‹ˆë‹¤.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CClickerGameDoc)));
	return (CClickerGameDoc*)m_pDocument;
}
#endif //_DEBUG


// CClickerGameView ë©”ì‹œì§€ ì²˜ë¦¬ê¸°
