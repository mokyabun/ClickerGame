// GdiDialogBase.cpp: 구현 파일
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
	// 버퍼 정리
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

	// 파생 클래스 초기화 호출
	OnGdiInitialize();

	// 60 FPS 타이머 시작 (16ms)
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

// GdiDialogBase 메시지 처리기



void GdiDialogBase::InitializeRenderBuffer(int width, int height)
{
	m_bufferWidth = width;
	m_bufferHeight = height;

	// 기존 버퍼 정리
	if (m_memBitmap.GetSafeHandle())
		m_memBitmap.DeleteObject();
	if (m_memDC.GetSafeHdc())
		m_memDC.DeleteDC();

	// 새 버퍼 생성
	CClientDC dc(this);
	m_memDC.CreateCompatibleDC(&dc);
	m_memBitmap.CreateCompatibleBitmap(&dc, width, height);
	m_memDC.SelectObject(&m_memBitmap);
}

BOOL GdiDialogBase::OnEraseBkgnd(CDC* pDC)
{
	// 깜박임 방지
	return TRUE;
}

void GdiDialogBase::OnPaint()
{
	CPaintDC dc(this);

	// 중앙 렌더링 버퍼를 화면에 복사만 수행
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

	// deltaTime 계산 (초 단위)
	DWORD currentTime = GetTickCount();
	float deltaTime = (currentTime - m_lastFrameTime) / 1000.0f;
	m_lastFrameTime = currentTime;

	// 업데이트 로직 호출
	OnUpdateLogic(deltaTime);

	// 투명 키 색상으로 배경 채우기 (RGB(255, 0, 255) 마젠타)
	m_memDC.FillSolidRect(0, 0, m_bufferWidth, m_bufferHeight, RGB(255, 0, 255));

	// GDI+ Graphics 생성
	Gdiplus::Graphics graphics(m_memDC.GetSafeHdc());
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

	// 파생 클래스의 렌더링 호출
	OnRenderContent(graphics, deltaTime);

	// 화면 갱신
	InvalidateRect(nullptr, FALSE);
}
