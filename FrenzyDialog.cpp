// FrenzyDialog.cpp: 구현 파일
//

#include "pch.h"
#include "afxdialogex.h"
#include "FrenzyDialog.h"
#include "resource.h"


// CFrenzyDialog 대화 상자

IMPLEMENT_DYNAMIC(CFrenzyDialog, CDialogEx)

CFrenzyDialog::CFrenzyDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FRENZY, pParent)
	, m_gdiplusToken(0)
{
	// GDI+ 초기화
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

CFrenzyDialog::~CFrenzyDialog()
{
	// GDI+ 종료
	if (m_gdiplusToken)
	{
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	}
}

void CFrenzyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFrenzyDialog, CDialogEx)
	ON_BN_CLICKED(IDC_FRENZY, &CFrenzyDialog::OnBnClickedFrenzy)
    ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CFrenzyDialog 메시지 처리기

void CFrenzyDialog::OnBnClickedFrenzy()
{
	if (m_onClickCallback) {
		m_onClickCallback();
	}
    KillTimer(1);
	ShowWindow(SW_HIDE);
}

BOOL CFrenzyDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// 기존 표준 버튼 숨기기
	CWnd* pButton = GetDlgItem(IDC_FRENZY);
	if (pButton)
	{
		CRect rect;
		pButton->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pButton->ShowWindow(SW_HIDE);
		
		// GDI 버튼 생성 (기존 버튼과 같은 위치 및 크기)
		m_btnGoldVirus.Create(L"", rect, this, IDC_FRENZY);
		m_btnGoldVirus.SetImage(_T("Main\\goldvirus.png"));
		m_btnGoldVirus.SetOnClickCallback([this]() {
			OnBnClickedFrenzy();
		});
	}
	
	SetTimer(1, 13000, nullptr); // 13 seconds
	return TRUE;
}

void CFrenzyDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {
		KillTimer(1);
		ShowWindow(SW_HIDE);
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CFrenzyDialog::OnPaint()
{
	CPaintDC dc(this);
	
	CRect clientRect;
	GetClientRect(&clientRect);
	
	// 메모리 DC 생성
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	
	CBitmap memBitmap;
	memBitmap.CreateCompatibleBitmap(&dc, clientRect.Width(), clientRect.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);
	
	// GDI+ Graphics 객체 생성
	Gdiplus::Graphics graphics(memDC.m_hDC);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	
	// 배경 채우기 (투명 또는 흰색)
	Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 240, 240, 240));
	graphics.FillRectangle(&whiteBrush, 0, 0, clientRect.Width(), clientRect.Height());
	
	// GDI 버튼 그리기
	m_btnGoldVirus.Draw(graphics, 0.0f);
	
	// 메모리 DC를 화면에 복사
	dc.BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);
	
	// 정리
	memDC.SelectObject(pOldBitmap);
}

BOOL CFrenzyDialog::OnEraseBkgnd(CDC* pDC)
{
	return TRUE; // 깜박임 방지
}
