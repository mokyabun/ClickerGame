// ShopDialog.cpp: 구현 파일
//

#include "pch.h"
#include "ClickerGame.h"
#include "afxdialogex.h"
#include "ShopDialog.h"
#include "ClickerGameDoc.h"
#include "PathResolver.h"


// CShopDialog 대화 상자

IMPLEMENT_DYNAMIC(CShopDialog, GdiDialogBase)

CShopDialog::CShopDialog(CWnd* pParent /*=nullptr*/)
	: GdiDialogBase(IDD_SHOP, pParent)
	, m_pBackgroundImage(nullptr)
	, m_pDoc(nullptr)
	, m_hoveredItemId(-1)
	, m_bDragging(false)
{
}

CShopDialog::~CShopDialog()
{
	// 버튼 정리
	for (auto* pBtn : m_shopButtons)
	{
		if (pBtn)
		{
			delete pBtn;
		}
	}
	m_shopButtons.clear();

	// 배경 이미지 정리
	if (m_pBackgroundImage)
	{
		delete m_pBackgroundImage;
		m_pBackgroundImage = nullptr;
	}
}

void CShopDialog::DoDataExchange(CDataExchange* pDX)
{
	GdiDialogBase::DoDataExchange(pDX);
}

BOOL CShopDialog::OnInitDialog()
{
	return GdiDialogBase::OnInitDialog();
}

void CShopDialog::OnGdiInitialize()
{
	// 윈도우 스타일 변경 (레이어드 윈도우)
	ModifyStyle(WS_THICKFRAME | WS_SYSMENU, 0);
	ModifyStyleEx(0, WS_EX_LAYERED);

	// 투명 키 설정 (RGB(255, 0, 255) 마젠타 색상을 투명하게)
	::SetLayeredWindowAttributes(m_hWnd, RGB(255, 0, 255), 0, LWA_COLORKEY);

	// 윈도우 크기 설정
	SetWindowPos(nullptr, 0, 0, 200, 600, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
	CenterWindow();

	// GDI+ 배경 이미지 로드
	CString imagePath = PathResolver::GetInstance().GetResourcePath(L"Shop\\Background.png");
	m_pBackgroundImage = Gdiplus::Image::FromFile(imagePath);
	if (m_pBackgroundImage == nullptr || m_pBackgroundImage->GetLastStatus() != Gdiplus::Ok)
	{
		CString msg;
		msg.Format(L"상점 배경 이미지를 로드할 수 없습니다!\n경로: %s", imagePath);
		MessageBox(msg, L"오류", MB_OK | MB_ICONERROR);
	}

	// 닫기 버튼 생성 (오른쪽 위 21x21 크기)
	const int btnWidth = 21;
	const int btnHeight = 21;
	const int yMargin = 5;
	const int xMargin = 5;
	
	CRect btnRect(200 - btnWidth - xMargin, yMargin, 200 - xMargin, yMargin + btnHeight);
	m_btnClose.Create(L"", btnRect, this, 3000);
	m_btnClose.SetAtlasImage(L"Main\\X.png", btnWidth, btnHeight);

	// 상점 아이템 버튼 생성 (6개, 1열 6행)
	const int itemBtnWidth = 178;
	const int itemBtnHeight = 48;
	const int startX = 11;
	const int startY = 100;
	const int spacingY = 48;
	const int columns = 1;

	if (m_pDoc)
	{
		const auto& upgrades = m_pDoc->GetGameCore().GetUpgrades();
		int numItems = min(6, (int)upgrades.size());

		for (int i = 0; i < numItems; i++)
		{
			int col = i % columns;
			int row = i / columns;
			int x = startX;
			int y = startY + row * spacingY;

			CRect btnRect(x, y, x + itemBtnWidth, y + itemBtnHeight);
			CGdiAtlasButton* pBtn = new CGdiAtlasButton();
			pBtn->Create(L"", btnRect, this, 2000 + i);
			
			// 2-state 아틀라스 설정 (178x96, normal 48px + click 48px)
			pBtn->SetAtlasImage(L"Shop\\Button.png", itemBtnWidth, itemBtnHeight);

			// 아이템 이름 설정 (GdiText 활용)
			const UpgradeData* pUpgrade = m_pDoc->GetGameCore().GetUpgrade(i);
			if (pUpgrade)
			{
				CString itemName(pUpgrade->name.c_str());
				pBtn->GetTextObject().SetText(itemName);
				pBtn->GetTextObject().SetFont(L"맑은 고딕", 11.0f, Gdiplus::FontStyleBold);
				pBtn->GetTextObject().SetColor(255, 255, 255, 255);
				pBtn->GetTextObject().SetShadowStyle(GdiText::ShadowStyle::DropShadow);
				pBtn->GetTextObject().SetShadowColor(255, 0, 0, 0);
				pBtn->GetTextObject().SetShadowOffset(1.0f, 1.0f);
				pBtn->GetTextObject().SetAlignment(Gdiplus::StringAlignmentNear);
				pBtn->GetTextObject().SetLineAlignment(Gdiplus::StringAlignmentCenter);
				
				// 텍스트 위치 조정 (왼쪽 여백)
				// GdiButton은 기본적으로 중앙 정렬이므로, 필요하면 GdiButton을 수정하거나 
				// 여기서는 정렬만 왼쪽으로 하고 패딩은 GdiText 내부 구현에 맡겨야 함.
				// 현재 GdiText는 패딩 기능이 없으므로, GdiButton::DrawButton에서 layoutRect를 조정하지 않는 한
				// 왼쪽 끝에 붙을 것임. 
				// 임시로 공백을 추가하거나, GdiButton을 더 개선해야 하지만, 
				// 기존 코드에서 (rect.left + 8) 위치에 그렸으므로, 
				// GdiButton의 텍스트 그리기가 전체 영역 기준임을 감안하면
				// 왼쪽 정렬 시 약간의 여백이 필요함.
				// 일단은 그대로 두고, 추후 GdiButton에 Padding 기능을 추가하는 것이 좋음.
			}

			// 클릭 콜백
			pBtn->SetOnClickCallback([this, i]() { OnItemButtonClick(i); });

			m_shopButtons.push_back(pBtn);
		}
	}

	// 툴팁 텍스트 초기화
	m_tooltipTitle.SetFont(L"Malgun Gothic", 11.0f, Gdiplus::FontStyleBold);
	m_tooltipTitle.SetColor(255, 255, 220, 120);
	m_tooltipTitle.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipTitle.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipTitle.SetShadowColor(180, 0, 0, 0);

	m_tooltipStats.SetFont(L"Malgun Gothic", 9.0f, Gdiplus::FontStyleBold);
	m_tooltipStats.SetColor(255, 100, 255, 100); // 연한 녹색
	m_tooltipStats.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipStats.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipStats.SetShadowColor(180, 0, 0, 0);

	m_tooltipDesc.SetFont(L"Malgun Gothic", 9.0f, Gdiplus::FontStyleRegular);
	m_tooltipDesc.SetColor(255, 200, 200, 200);
	m_tooltipDesc.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipDesc.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipDesc.SetShadowColor(180, 0, 0, 0);

	// 고정 크기 버퍼 생성 (200x600)
	InitializeRenderBuffer(200, 600);
}


BEGIN_MESSAGE_MAP(CShopDialog, GdiDialogBase)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(3000, &CShopDialog::OnCloseButtonClick)
END_MESSAGE_MAP()


// CShopDialog 메시지 처리기

BOOL CShopDialog::OnEraseBkgnd(CDC* pDC)
{
	return GdiDialogBase::OnEraseBkgnd(pDC);
}

void CShopDialog::OnDestroy()
{
	GdiDialogBase::OnDestroy();
}

void CShopDialog::OnClose()
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

void CShopDialog::PostNcDestroy()
{
	GdiDialogBase::PostNcDestroy();
	// Modeless 다이얼로그는 부모가 메모리 관리
}

void CShopDialog::OnUpdateLogic(float deltaTime)
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
		const UpgradeData* pUpgrade = m_pDoc->GetGameCore().GetUpgrade(m_hoveredItemId);
		if (pUpgrade)
		{
			m_tooltipTitle.SetText(CString(pUpgrade->name.c_str()));
			
			CString stats;
			stats.Format(L"가격: %.0f ZPC | 생산: +%.1f/s", 
				pUpgrade->GetCurrentCost(), pUpgrade->productionPerSecond);
			m_tooltipStats.SetText(stats);

			m_tooltipDesc.SetText(CString(pUpgrade->description.c_str()));
		}
	}
}

void CShopDialog::OnRenderContent(Gdiplus::Graphics& graphics, float deltaTime)
{
	// 배경 이미지 그리기
	if (m_pBackgroundImage && m_pBackgroundImage->GetLastStatus() == Gdiplus::Ok)
	{
		graphics.DrawImage(m_pBackgroundImage, 0, 0, 200, 600);
	}

	// 닫기 버튼 그리기
	m_btnClose.Draw(graphics, deltaTime);

	// 모든 상점 버튼 그리기
	for (auto* pBtn : m_shopButtons)
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
		Gdiplus::RectF bgRect(5, 32, 190, 60);
		graphics.FillRectangle(&bgBrush, bgRect);

		// 테두리
		Gdiplus::Pen borderPen(Gdiplus::Color(255, 120, 120, 120), 1);
		graphics.DrawRectangle(&borderPen, bgRect);

		// 툴팁 텍스트 그리기
		m_tooltipTitle.Draw(graphics, 100.0f, 38.0f);
		m_tooltipStats.Draw(graphics, 100.0f, 53.0f);
		m_tooltipDesc.Draw(graphics, 100.0f, 68.0f);
	}
}

void CShopDialog::OnItemButtonClick(int itemId)
{
	if (!m_pDoc)
		return;

	// 아이템 구매 시도
	bool success = m_pDoc->GetGameCore().PurchaseUpgrade(itemId);
	
	if (success)
	{
		// 구매 성공 효과음이나 피드백 추가 가능
		TRACE(L"아이템 %d 구매 성공!\n", itemId);
	}
	else
	{
		// 구매 실패 (돈 부족)
		TRACE(L"아이템 %d 구매 실패 - 재화 부족\n", itemId);
		MessageBeep(MB_ICONEXCLAMATION);
	}
}



void CShopDialog::OnCloseButtonClick()
{
	// X 버튼 클릭 시 다이얼로그 닫기
	PostMessage(WM_CLOSE);
}

void CShopDialog::OnLButtonDown(UINT nFlags, CPoint point)
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

void CShopDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		m_bDragging = false;
		
		// 마우스 캡처 해제
		ReleaseCapture();
	}
	
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CShopDialog::OnMouseMove(UINT nFlags, CPoint point)
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

void CShopDialog::UpdateHoveredItem(CPoint point)
{
	int newHoveredId = -1;

	// 각 버튼 영역 확인
	for (size_t i = 0; i < m_shopButtons.size(); i++)
	{
		auto* pBtn = m_shopButtons[i];
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
