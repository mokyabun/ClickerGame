#pragma once
#include "afxdialogex.h"
#include "GdiButton.h"
#include <vector>
#include <gdiplus.h>

// GDI+ ë Œë”ë§ì„ ìœ„í•œ ë‹¤ì´ì–¼ë¡œê·¸ ë² ì´ìŠ¤ í´ëž˜ìŠ¤
// 1. CDialogExë¥¼ ìƒì†
// 2. SetTimerì„ ì‚¬ìš©í•´ì„œ 1ì´ˆì— 60ë²ˆ Draw (16ms ê°„ê²©)
// 3. ê·¸ë¦¬ëŠ” ìˆœì„œì— ë”°ë¼ Layer ì²˜ë¦¬ (childLayersëŠ” ì—†ì´)

class GdiDialogBase : public CDialogEx
{
	DECLARE_DYNAMIC(GdiDialogBase)

public:
	GdiDialogBase(UINT nIDTemplate, CWnd* pParent = nullptr);
	virtual ~GdiDialogBase();

protected:
	// íŒŒìƒ í´ëž˜ìŠ¤ì—ì„œ ìž¬ì •ì˜í•  ê°€ìƒ í•¨ìˆ˜ë“¤
	
	// ì´ˆê¸°í™” ì‹œ í˜¸ì¶œ (ë‹¤ì´ì–¼ë¡œê·¸ í¬ê¸°, ìœˆë„ìš° ìŠ¤íƒ€ì¼ ì„¤ì • ë“±)
	virtual void OnGdiInitialize() {}
	
	// ë©”ì¸ ë Œë”ë§ í•¨ìˆ˜ - íŒŒìƒ í´ëž˜ìŠ¤ì—ì„œ ì‹¤ì œ ê·¸ë¦¬ê¸° ë‚´ìš© êµ¬í˜„
	// graphics: GDI+ ê·¸ë¦¬ê¸° ê°ì²´
	// deltaTime: ì´ì „ í”„ë ˆìž„ê³¼ì˜ ì‹œê°„ì°¨ (ì´ˆ ë‹¨ìœ„)
	virtual void OnRenderContent(Gdiplus::Graphics& graphics, float deltaTime) = 0;
	
	// ì—…ë°ì´íŠ¸ ë¡œì§ (ë Œë”ë§ ì „ì— í˜¸ì¶œë¨)
	virtual void OnUpdateLogic(float deltaTime) {}

	// ë©”ëª¨ë¦¬ DC ë° ë¹„íŠ¸ë§µ ì´ˆê¸°í™”
	void InitializeRenderBuffer(int width, int height);

	// MFC ê°€ìƒ í•¨ìˆ˜
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;

	DECLARE_MESSAGE_MAP()

	// ë©”ì‹œì§€ í•¸ë“¤ëŸ¬
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

private:
	// ë Œë”ë§ ë²„í¼
	CDC m_memDC;              // ë©”ëª¨ë¦¬ DC
	CBitmap m_memBitmap;      // ë©”ëª¨ë¦¬ ë¹„íŠ¸ë§µ
	int m_bufferWidth;        // ë²„í¼ ë„ˆë¹„
	int m_bufferHeight;       // ë²„í¼ ë†’ì´
	
	// íƒ€ì´ë¨¸
	UINT_PTR m_nTimerID;      // 60 FPS íƒ€ì´ë¨¸ (16ms)
	DWORD m_lastFrameTime;    // ë§ˆì§€ë§‰ í”„ë ˆìž„ ì‹œê°„ (deltaTime ê³„ì‚°ìš©)
	
	// ë Œë”ë§ í•¨ìˆ˜
	void RenderFrame();
};
