// GdiDialogBase.cpp: êµ¬í˜„ íŒŒì¼
//

#include "pch.h"
#include "GdiDialogBase.h"

IMPLEMENT_DYNAMIC(GdiDialogBase, CDialogEx)

GdiDialogBase::GdiDialogBase(UINT nIDTemplate, CWnd* pParent /*=nullptr*/)
	: CDialogEx(nIDTemplate, pParent)
	, m_nTimerID(0)
	, m_lastFrameTime(0)
	, m_bufferWidth(0)
	, m_bufferHeight(0)
{
}

GdiDialogBase::~GdiDialogBase()
{
	// ë²„í¼ ì •ë¦¬
	if (m_memBitmap.GetSafeHandle())
		m_memBitmap.DeleteObject();
	if (m_memDC.GetSafeHdc())
		m_memDC.DeleteDC();
}

void GdiDialogBase::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL GdiDialogBase::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// íŒŒìƒ í´ëž˜ìŠ¤ ì´ˆê¸°í™” í˜¸ì¶œ
	OnGdiInitialize();

	// 60 FPS íƒ€ì´ë¨¸ ì‹œìž‘ (16ms)
	m_nTimerID = SetTimer(1, 16, nullptr);
	m_lastFrameTime = GetTickCount();

	return TRUE;
}

BEGIN_MESSAGE_MAP(GdiDialogBase, CDialogEx)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// GdiDialogBase ë©”ì‹œì§€ ì²˜ë¦¬ê¸°



void GdiDialogBase::InitializeRenderBuffer(int width, int height)
{
	m_bufferWidth = width;
	m_bufferHeight = height;

	// ê¸°ì¡´ ë²„í¼ ì •ë¦¬
	if (m_memBitmap.GetSafeHandle())
		m_memBitmap.DeleteObject();
	if (m_memDC.GetSafeHdc())
		m_memDC.DeleteDC();

	// ìƒˆ ë²„í¼ ìƒì„±
	CClientDC dc(this);
	m_memDC.CreateCompatibleDC(&dc);
	m_memBitmap.CreateCompatibleBitmap(&dc, width, height);
	m_memDC.SelectObject(&m_memBitmap);
}

BOOL GdiDialogBase::OnEraseBkgnd(CDC* pDC)
{
	// ê¹œë°•ìž„ ë°©ì§€
	return TRUE;
}

void GdiDialogBase::OnPaint()
{
	CPaintDC dc(this);

	// ì¤‘ì•™ ë Œë”ë§ ë²„í¼ë¥¼ í™”ë©´ì— ë³µì‚¬ë§Œ ìˆ˜í–‰
	if (m_memDC.GetSafeHdc() && m_bufferWidth > 0 && m_bufferHeight > 0)
	{
		dc.BitBlt(0, 0, m_bufferWidth, m_bufferHeight, &m_memDC, 0, 0, SRCCOPY);
	}
}

void GdiDialogBase::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_nTimerID)
	{
		RenderFrame();
	}

	CDialogEx::OnTimer(nIDEvent);
}

void GdiDialogBase::OnDestroy()
{
	if (m_nTimerID)
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}

	CDialogEx::OnDestroy();
}

void GdiDialogBase::RenderFrame()
{
	if (!m_memDC.GetSafeHdc() || m_bufferWidth == 0 || m_bufferHeight == 0)
		return;

	// deltaTime ê³„ì‚° (ì´ˆ ë‹¨ìœ„)
	DWORD currentTime = GetTickCount();
	float deltaTime = (currentTime - m_lastFrameTime) / 1000.0f;
	m_lastFrameTime = currentTime;

	// ì—…ë°ì´íŠ¸ ë¡œì§ í˜¸ì¶œ
	OnUpdateLogic(deltaTime);

	// íˆ¬ëª… í‚¤ ìƒ‰ìƒìœ¼ë¡œ ë°°ê²½ ì±„ìš°ê¸° (RGB(255, 0, 255) ë§ˆì  íƒ€)
	m_memDC.FillSolidRect(0, 0, m_bufferWidth, m_bufferHeight, RGB(255, 0, 255));

	// GDI+ Graphics ìƒì„±
	Gdiplus::Graphics graphics(m_memDC.GetSafeHdc());
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

	// íŒŒìƒ í´ëž˜ìŠ¤ì˜ ë Œë”ë§ í˜¸ì¶œ
	OnRenderContent(graphics, deltaTime);

	// í™”ë©´ ê°±ì‹ 
	InvalidateRect(nullptr, FALSE);
}
