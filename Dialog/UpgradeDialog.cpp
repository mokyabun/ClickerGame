// UpgradeDialog.cpp: 구현 파일
//

#include "pch.h"
#include "ClickerGame.h"
#include "afxdialogex.h"
#include "UpgradeDialog.h"
#include "ClickerGameDoc.h"
#include "PathResolver.h"


// CUpgradeDialog 대화 상자

IMPLEMENT_DYNAMIC(CUpgradeDialog, GdiDialogBase)

CUpgradeDialog::CUpgradeDialog(CWnd* pParent /*=nullptr*/)
	: GdiDialogBase(IDD_UPGRADE, pParent)
	, m_pBackgroundImage(nullptr)
	, m_pDoc(nullptr)
	, m_hoveredItemId(-1)
	, m_bDragging(false)
{
}

CUpgradeDialog::~CUpgradeDialog()
{
	// 버튼 정리
	for (auto* pBtn : m_upgradeButtons)
	{
		if (pBtn)
		{
			delete pBtn;
		}
	}
	m_upgradeButtons.clear();

	// 배경 이미지 정리
	if (m_pBackgroundImage)
	{
		delete m_pBackgroundImage;
		m_pBackgroundImage = nullptr;
	}
}

void CUpgradeDialog::DoDataExchange(CDataExchange* pDX)
{
	GdiDialogBase::DoDataExchange(pDX);
}

BOOL CUpgradeDialog::OnInitDialog()
{
	return GdiDialogBase::OnInitDialog();
}

void CUpgradeDialog::OnGdiInitialize()
{
	// 윈도우 스타일 변경 (레이어드 윈도우)
	ModifyStyle(WS_THICKFRAME | WS_SYSMENU, 0);
	ModifyStyleEx(0, WS_EX_LAYERED);

	// 투명 키 설정 (RGB(255, 0, 255) 마젠타 색상을 투명하게)
	::SetLayeredWindowAttributes(m_hWnd, RGB(255, 0, 255), 0, LWA_COLORKEY);

	// 윈도우 크기 설정 (400x600 - 2열 레이아웃을 위해 너비 확장)
	SetWindowPos(nullptr, 0, 0, 400, 600, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
	CenterWindow();

	// GDI+ 배경 이미지 로드
	CString imagePath = PathResolver::GetInstance().GetResourcePath(L"Upgrade\\Background.png");
	m_pBackgroundImage = Gdiplus::Image::FromFile(imagePath);
	if (m_pBackgroundImage == nullptr || m_pBackgroundImage->GetLastStatus() != Gdiplus::Ok)
	{
		CString msg;
		msg.Format(L"업그레이드 배경 이미지를 로드할 수 없습니다!\n경로: %s", imagePath);
		MessageBox(msg, L"오류", MB_OK | MB_ICONERROR);
	}

	// 닫기 버튼 생성 (오른쪽 위 21x21 크기)
	const int btnWidth = 21;
	const int btnHeight = 21;
	const int yMargin = 5;
	const int xMargin = 5;
	
	CRect btnRect(400 - btnWidth - xMargin, yMargin, 400 - xMargin, yMargin + btnHeight);
	m_btnClose.Create(L"", btnRect, this, 4000);
	m_btnClose.SetAtlasImage(L"Main\\X.png", btnWidth, btnHeight);

	// 특별 업그레이드 버튼 생성 (12개, 2열 6행)
	const int itemBtnWidth = 178;
	const int itemBtnHeight = 48;
	const int startX = 11;
	const int startY = 100;
	const int spacingX = 189;  // 버튼 너비 + 간격
	const int spacingY = 48;
	const int columns = 2;

	if (m_pDoc)
	{
		const auto& specialUpgrades = m_pDoc->GetGameCore().GetSpecialUpgrades();
		int numItems = min(12, (int)specialUpgrades.size());

		for (int i = 0; i < numItems; i++)
		{
			int col = i % columns;
			int row = i / columns;
			int x = startX + col * spacingX;
			int y = startY + row * spacingY;

			CRect btnRect(x, y, x + itemBtnWidth, y + itemBtnHeight);
			CGdiAtlasButton* pBtn = new CGdiAtlasButton();
			pBtn->Create(L"", btnRect, this, 3000 + i);
			
			// 2-state 아틀라스 설정
			pBtn->SetAtlasImage(L"Upgrade\\Button.png", itemBtnWidth, itemBtnHeight);

			// 아이템 이름 설정
			const SpecialUpgradeData* pUpgrade = m_pDoc->GetGameCore().GetSpecialUpgrade(i);
			if (pUpgrade)
			{
				CString itemName(pUpgrade->name.c_str());
				pBtn->GetTextObject().SetText(itemName);
				pBtn->GetTextObject().SetFont(L"맑은 고딕", 9.0f, Gdiplus::FontStyleBold);
				pBtn->GetTextObject().SetColor(255, 255, 255, 255);
				pBtn->GetTextObject().SetShadowStyle(GdiText::ShadowStyle::DropShadow);
				pBtn->GetTextObject().SetShadowColor(255, 0, 0, 0);
				pBtn->GetTextObject().SetShadowOffset(1.0f, 1.0f);
				pBtn->GetTextObject().SetAlignment(Gdiplus::StringAlignmentNear);
				pBtn->GetTextObject().SetLineAlignment(Gdiplus::StringAlignmentCenter);
			}

			// 클릭 콜백
			pBtn->SetOnClickCallback([this, i]() { OnItemButtonClick(i); });

			m_upgradeButtons.push_back(pBtn);
		}
	}

	// 툴팁 텍스트 초기화
	m_tooltipTitle.SetFont(L"Malgun Gothic", 11.0f, Gdiplus::FontStyleBold);
	m_tooltipTitle.SetColor(255, 255, 220, 120);
	m_tooltipTitle.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipTitle.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipTitle.SetShadowColor(180, 0, 0, 0);

	m_tooltipCategory.SetFont(L"Malgun Gothic", 9.0f, Gdiplus::FontStyleItalic);
	m_tooltipCategory.SetColor(255, 150, 150, 255); // 연한 보라색
	m_tooltipCategory.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipCategory.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipCategory.SetShadowColor(180, 0, 0, 0);

	m_tooltipPrice.SetFont(L"Malgun Gothic", 9.0f, Gdiplus::FontStyleBold);
	m_tooltipPrice.SetColor(255, 255, 215, 0); // 금색
	m_tooltipPrice.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipPrice.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipPrice.SetShadowColor(180, 0, 0, 0);

	// 고정 크기 버퍼 생성 (400x600)
	InitializeRenderBuffer(400, 600);
}


BEGIN_MESSAGE_MAP(CUpgradeDialog, GdiDialogBase)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(4000, &CUpgradeDialog::OnCloseButtonClick)
END_MESSAGE_MAP()


// CUpgradeDialog 메시지 처리기

BOOL CUpgradeDialog::OnEraseBkgnd(CDC* pDC)
{
	return GdiDialogBase::OnEraseBkgnd(pDC);
}

void CUpgradeDialog::OnDestroy()
{
	GdiDialogBase::OnDestroy();
}

void CUpgradeDialog::OnClose()
{
	// 리소스 정리
	if (m_pBackgroundImage)
	{
		delete m_pBackgroundImage;
		m_pBackgroundImage = nullptr;
	}

	// Modeless 다이얼로그이므로 DestroyWindow 호출
	DestroyWindow();
}

void CUpgradeDialog::PostNcDestroy()
{
	GdiDialogBase::PostNcDestroy();
	// Modeless 다이얼로그는 부모가 메모리 관리
}

void CUpgradeDialog::OnUpdateLogic(float deltaTime)
{
	// 마우스 위치를 기반으로 호버 중인 아이템 감지
	CPoint cursorPos;
	if (GetCursorPos(&cursorPos))
	{
		ScreenToClient(&cursorPos);
		UpdateHoveredItem(cursorPos);
	}

	// 툴팁 텍스트 업데이트
	if (m_hoveredItemId >= 0 && m_pDoc)
	{
		const SpecialUpgradeData* pUpgrade = m_pDoc->GetGameCore().GetSpecialUpgrade(m_hoveredItemId);
		if (pUpgrade)
		{
			m_tooltipTitle.SetText(CString(pUpgrade->name.c_str()));
			m_tooltipCategory.SetText(CString(L"[") + CString(pUpgrade->category.c_str()) + CString(L"]"));
			
			CString price;
			price.Format(L"가격: %.0f ZPC%s", 
				pUpgrade->cost,
				pUpgrade->isPurchased ? L" (구매완료)" : L"");
			m_tooltipPrice.SetText(price);
		}
	}
}

void CUpgradeDialog::OnRenderContent(Gdiplus::Graphics& graphics, float deltaTime)
{
	// 배경 이미지 그리기
	if (m_pBackgroundImage && m_pBackgroundImage->GetLastStatus() == Gdiplus::Ok)
	{
		graphics.DrawImage(m_pBackgroundImage, 0, 0, 400, 600);
	}

	// 닫기 버튼 그리기
	m_btnClose.Draw(graphics, deltaTime);

	// 모든 업그레이드 버튼 그리기
	for (auto* pBtn : m_upgradeButtons)
	{
		if (pBtn && pBtn->IsWindowVisible())
		{
			pBtn->Draw(graphics, deltaTime);
		}
	}

	// 호버 중인 아이템 설명 표시 (상단)
	if (m_hoveredItemId >= 0 && m_pDoc)
	{
		// 배경 박스 (상단) - 3줄 텍스트를 위해 높이 증가
		Gdiplus::SolidBrush bgBrush(Gdiplus::Color(230, 0, 0, 0));
		Gdiplus::RectF bgRect(5, 32, 390, 60);
		graphics.FillRectangle(&bgBrush, bgRect);

		// 테두리
		Gdiplus::Pen borderPen(Gdiplus::Color(255, 120, 120, 120), 1);
		graphics.DrawRectangle(&borderPen, bgRect);

		// 툴팁 텍스트 그리기
		m_tooltipTitle.Draw(graphics, 200.0f, 38.0f);
		m_tooltipCategory.Draw(graphics, 200.0f, 53.0f);
		m_tooltipPrice.Draw(graphics, 200.0f, 68.0f);
	}
}

void CUpgradeDialog::OnItemButtonClick(int itemId)
{
	if (!m_pDoc)
		return;

	// 특별 업그레이드 구매 시도
	bool success = m_pDoc->GetGameCore().PurchaseSpecialUpgrade(itemId);
	
	if (success)
	{
		// 구매 성공 효과음이나 피드백 추가 가능
		TRACE(L"특별 업그레이드 %d 구매 성공!\n", itemId);
	}
	else
	{
		// 구매 실패 (돈 부족 또는 이미 구매함)
		TRACE(L"특별 업그레이드 %d 구매 실패\n", itemId);
		MessageBeep(MB_ICONEXCLAMATION);
	}
}



void CUpgradeDialog::OnCloseButtonClick()
{
	// X 버튼 클릭 시 다이얼로그 닫기
	PostMessage(WM_CLOSE);
}

void CUpgradeDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 상단 영역(0~40px)을 클릭하면 드래그 시작
	if (point.y >= 0 && point.y <= 40)
	{
		// 닫기 버튼 영역은 제외
		CRect closeRect;
		m_btnClose.GetWindowRect(&closeRect);
		ScreenToClient(&closeRect);
		
		if (!closeRect.PtInRect(point))
		{
			m_bDragging = true;
			
			// 마우스의 창 내 상대 위치 저장
			m_dragStartPoint = point;
			
			// 마우스 캡처
			SetCapture();
		}
	}
	
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CUpgradeDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		m_bDragging = false;
		
		// 마우스 캡처 해제
		ReleaseCapture();
	}
	
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CUpgradeDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	// 드래그 중이면 창 이동
	if (m_bDragging)
	{
		CPoint screenPoint = point;
		ClientToScreen(&screenPoint);
		
		CRect windowRect;
		GetWindowRect(&windowRect);
		
		int newX = screenPoint.x - m_dragStartPoint.x;
		int newY = screenPoint.y - m_dragStartPoint.y;
		
		SetWindowPos(nullptr, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
	else
	{
		// 항상 호버 상태 업데이트
		UpdateHoveredItem(point);
	}
	
	CDialogEx::OnMouseMove(nFlags, point);
}

void CUpgradeDialog::UpdateHoveredItem(CPoint point)
{
	int newHoveredId = -1;

	// 각 버튼 영역 확인
	for (size_t i = 0; i < m_upgradeButtons.size(); i++)
	{
		auto* pBtn = m_upgradeButtons[i];
		if (pBtn && pBtn->IsWindowVisible())
		{
			CRect rect;
			pBtn->GetWindowRect(&rect);
			ScreenToClient(&rect);

			if (rect.PtInRect(point))
			{
				newHoveredId = (int)i;
				break;
			}
		}
	}

	// 호버 상태 변경
	m_hoveredItemId = newHoveredId;
}
