// ShopDialog.cpp: 구현 파일
//

#include "pch.h"
#include "ClickerGame.h"
#include "afxdialogex.h"
#include "ShopDialog.h"
#include "ClickerGameDoc.h"
#include "PathResolver.h"


// CShopDialog 대화 상자

IMPLEMENT_DYNAMIC(CShopDialog, CDialogEx)

CShopDialog::CShopDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SHOP, pParent)
	, m_pBackgroundImage(nullptr)
	, m_pDoc(nullptr)
	, m_nTimerID(0)
	, m_lastFrameTime(0)
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

	// 버퍼 정리
	if (m_memBitmap.GetSafeHandle())
		m_memBitmap.DeleteObject();
	if (m_memDC.GetSafeHdc())
		m_memDC.DeleteDC();
}

void CShopDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CShopDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	// 고정 크기 버퍼 생성 (200x600)
	CClientDC dc(this);
	m_memDC.CreateCompatibleDC(&dc);
	m_memBitmap.CreateCompatibleBitmap(&dc, 200, 600);
	m_memDC.SelectObject(&m_memBitmap);

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

			// 클릭 콜백
			pBtn->SetOnClickCallback([this, i]() { OnItemButtonClick(i); });

			// 호버 콜백 (CGdiButton의 SetOnHoverCallback 사용)
			// CGdiButton에 호버 콜백이 없다면 마우스 이벤트로 처리 필요

			m_shopButtons.push_back(pBtn);
		}
	}

	// 60 FPS 타이머 시작 (16ms)
	m_nTimerID = SetTimer(1, 16, nullptr);
	m_lastFrameTime = GetTickCount();

	return TRUE;
}


BEGIN_MESSAGE_MAP(CShopDialog, CDialogEx)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
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
	// 깜박임 방지
	return TRUE;
}

void CShopDialog::OnPaint()
{
	CPaintDC dc(this);

	// 중앙 렌더링 버퍼를 화면에 복사만 수행
	if (m_memDC.GetSafeHdc())
	{
		dc.BitBlt(0, 0, 200, 600, &m_memDC, 0, 0, SRCCOPY);
	}
}

void CShopDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_nTimerID)
	{
		RenderAllLayers();
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CShopDialog::OnDestroy()
{
	if (m_nTimerID)
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}

	CDialogEx::OnDestroy();
}

void CShopDialog::OnClose()
{
	// 타이머 정리
	if (m_nTimerID)
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}

	// 리소스 정리
	if (m_pBackgroundImage)
	{
		delete m_pBackgroundImage;
		m_pBackgroundImage = nullptr;
	}

	if (m_memBitmap.GetSafeHandle())
		m_memBitmap.DeleteObject();
	if (m_memDC.GetSafeHdc())
		m_memDC.DeleteDC();

	// Modeless 다이얼로그이므로 DestroyWindow 호출
	DestroyWindow();
}

void CShopDialog::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	// Modeless 다이얼로그는 부모가 메모리 관리
}

void CShopDialog::RenderAllLayers()
{
	// deltaTime 계산 (초 단위)
	DWORD currentTime = GetTickCount();
	float deltaTime = (currentTime - m_lastFrameTime) / 1000.0f;
	m_lastFrameTime = currentTime;

	// 투명 키 색상으로 배경 채우기
	m_memDC.FillSolidRect(0, 0, 200, 600, RGB(255, 0, 255));

	// GDI+ Graphics 생성
	Gdiplus::Graphics graphics(m_memDC.GetSafeHdc());
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

	// 배경 이미지 그리기
	if (m_pBackgroundImage && m_pBackgroundImage->GetLastStatus() == Gdiplus::Ok)
	{
		graphics.DrawImage(m_pBackgroundImage, 0, 0, 200, 600);
	}

	// 닫기 버튼 그리기
	if (m_btnClose.IsWindowVisible())
	{
		CRect rect;
		m_btnClose.GetWindowRect(&rect);
		ScreenToClient(&rect);
		
		CPoint offset(rect.left, rect.top);
		m_btnClose.Draw(graphics, offset, deltaTime);
	}

	// 모든 상점 버튼 그리기
	for (size_t i = 0; i < m_shopButtons.size(); i++)
	{
		auto* pBtn = m_shopButtons[i];
		if (pBtn && pBtn->IsWindowVisible())
		{
			CRect rect;
			pBtn->GetWindowRect(&rect);
			ScreenToClient(&rect);

			CPoint offset(rect.left, rect.top);
			pBtn->Draw(graphics, offset, deltaTime);

			// 버튼 위에 아이템 이름만 그리기
			if (m_pDoc)
			{
				const UpgradeData* pUpgrade = m_pDoc->GetGameCore().GetUpgrade((int)i);
				if (pUpgrade)
				{
					// 아이템 이름 (드롭 섀도우 포함)
					Gdiplus::FontFamily fontFamily(L"맑은 고딕");
					Gdiplus::Font nameFont(&fontFamily, 11, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
					
					CString itemName(pUpgrade->name.c_str());
					
					// Shadow
					Gdiplus::SolidBrush nameShadowBrush(Gdiplus::Color(180, 0, 0, 0));
					Gdiplus::PointF nameShadowPos((float)rect.left + 9, (float)rect.top + 17);
					graphics.DrawString(itemName, itemName.GetLength(), &nameFont, nameShadowPos, &nameShadowBrush);
					
					// Text
					Gdiplus::SolidBrush nameBrush(Gdiplus::Color(255, 255, 255, 255));
					Gdiplus::PointF namePos((float)rect.left + 8, (float)rect.top + 16);
					graphics.DrawString(itemName, itemName.GetLength(), &nameFont, namePos, &nameBrush);
				}
			}
		}
	}

	// 호버 중인 아이템 설명 표시 (상단)
	if (m_hoveredItemId >= 0 && m_pDoc)
	{
		const UpgradeData* pUpgrade = m_pDoc->GetGameCore().GetUpgrade(m_hoveredItemId);
		if (pUpgrade)
		{
			// 배경 박스 (상단)
			Gdiplus::SolidBrush bgBrush(Gdiplus::Color(230, 0, 0, 0));
			Gdiplus::RectF bgRect(5, 32, 190, 50);
			graphics.FillRectangle(&bgBrush, bgRect);

			// 테두리
			Gdiplus::Pen borderPen(Gdiplus::Color(255, 120, 120, 120), 1);
			graphics.DrawRectangle(&borderPen, bgRect);

			// 아이템 이름 (드롭 섀도우 포함)
			Gdiplus::FontFamily fontFamily(L"Malgun Gothic");
			Gdiplus::Font titleFont(&fontFamily, 11, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
			Gdiplus::StringFormat centerFormat;
			centerFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
			
			CString itemName(pUpgrade->name.c_str());
			CString itemDesc(pUpgrade->description.c_str());
			
			// Shadow
			Gdiplus::SolidBrush titleShadowBrush(Gdiplus::Color(180, 0, 0, 0));
			Gdiplus::PointF titleShadowPos(101, 41);
			graphics.DrawString(itemName, itemName.GetLength(), &titleFont, titleShadowPos, &centerFormat, &titleShadowBrush);
			
			// Text
			Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 255, 220, 120));
			Gdiplus::PointF titlePos(100, 40);
			graphics.DrawString(itemName, itemName.GetLength(), &titleFont, titlePos, &centerFormat, &titleBrush);

			// 아이템 설명 (드롭 섀도우 포함)
			Gdiplus::FontFamily descFontFamily(L"Malgun Gothic");
			Gdiplus::Font descFont(&descFontFamily, 9, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
			
			// Shadow
			Gdiplus::SolidBrush descShadowBrush(Gdiplus::Color(180, 0, 0, 0));
			Gdiplus::PointF descShadowPos(101, 58);
			graphics.DrawString(itemDesc, itemDesc.GetLength(), &descFont, descShadowPos, &centerFormat, &descShadowBrush);
			
			// Text
			Gdiplus::SolidBrush descBrush(Gdiplus::Color(255, 200, 200, 200));
			Gdiplus::PointF descPos(100, 57);
			graphics.DrawString(itemDesc, itemDesc.GetLength(), &descFont, descPos, &centerFormat, &descBrush);
		}
	}

	// 화면 갱신
	InvalidateRect(nullptr, FALSE);
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

void CShopDialog::OnItemButtonHover(int itemId, bool isHovering)
{
	if (isHovering)
	{
		m_hoveredItemId = itemId;
	}
	else
	{
		if (m_hoveredItemId == itemId)
		{
			m_hoveredItemId = -1;
		}
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
