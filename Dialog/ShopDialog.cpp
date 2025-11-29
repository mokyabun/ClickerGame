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
	for (auto* pBtn : m_shopButtons) {
		if (pBtn) {
			delete pBtn;
		}
	}
	m_shopButtons.clear();

	if (m_pBackgroundImage) {
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
	ModifyStyle(WS_THICKFRAME | WS_SYSMENU, 0);
	ModifyStyleEx(0, WS_EX_LAYERED);

	::SetLayeredWindowAttributes(m_hWnd, RGB(255, 0, 255), 0, LWA_COLORKEY);

	SetWindowPos(nullptr, 0, 0, 200, 600, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
	CenterWindow();

	CString imagePath = PathResolver::GetInstance().GetResourcePath(_T("Shop\\Background.png"));
	m_pBackgroundImage = Gdiplus::Image::FromFile(imagePath);
	if (m_pBackgroundImage == nullptr || m_pBackgroundImage->GetLastStatus() != Gdiplus::Ok) {
		CString msg;
		msg.Format(_T("상점 배경 이미지를 로드할 수 없습니다!\n경로: %s"), imagePath);
		MessageBox(msg, _T("오류"), MB_OK | MB_ICONERROR);
	}

	const int btnWidth = 21;
	const int btnHeight = 21;
	const int yMargin = 5;
	const int xMargin = 5;
	
	CRect btnRect(200 - btnWidth - xMargin, yMargin, 200 - xMargin, yMargin + btnHeight);
	m_btnClose.Create(L"", btnRect, this, 3000);
	m_btnClose.SetAtlasImage(_T("Shared\\X.png"), btnWidth, btnHeight);

	const int itemBtnWidth = 178;
	const int itemBtnHeight = 48;
	const int startX = 11;
	const int startY = 100;
	const int spacingY = 48;
	const int columns = 1;

	if (m_pDoc) {
		const auto& upgrades = m_pDoc->GetGameCore().GetUpgrades();

		for (int i = 0; i < upgrades.size(); i++) {
			int col = i % columns;
			int row = i / columns;
			int x = startX;
			int y = startY + row * spacingY;

			CRect btnRect(x, y, x + itemBtnWidth, y + itemBtnHeight);
			CGdiAtlasButton* pBtn = new CGdiAtlasButton();
			pBtn->Create(L"", btnRect, this, 2000 + i);
			
			pBtn->SetAtlasImage(_T("Shop\\Button.png"), itemBtnWidth, itemBtnHeight);

			pBtn->SetOnClickCallback([this, i]() { OnItemButtonClick(i); });

			m_shopButtons.push_back(pBtn);
		}
	}

	m_tooltipTitle.SetFont(_T("Malgun Gothic"), 11.0f, Gdiplus::FontStyleBold);
	m_tooltipTitle.SetColor(255, 255, 220, 120);
	m_tooltipTitle.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipTitle.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipTitle.SetShadowColor(180, 0, 0, 0);

	m_tooltipStats.SetFont(_T("Malgun Gothic"), 9.0f, Gdiplus::FontStyleBold);
	m_tooltipStats.SetColor(255, 100, 255, 100);
	m_tooltipStats.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipStats.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipStats.SetShadowColor(180, 0, 0, 0);

	m_tooltipDesc.SetFont(_T("Malgun Gothic"), 9.0f, Gdiplus::FontStyleRegular);
	m_tooltipDesc.SetColor(255, 200, 200, 200);
	m_tooltipDesc.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipDesc.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipDesc.SetShadowColor(180, 0, 0, 0);

	m_quantityText.SetFont(_T("Malgun Gothic"), 10.0f, Gdiplus::FontStyleBold);
	m_quantityText.SetColor(130, 0, 0, 0);
	m_quantityText.SetAlignment(Gdiplus::StringAlignmentFar);
	m_quantityText.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	m_quantityText.SetShadowStyle(GdiText::ShadowStyle::None);

	m_itemNameText.SetFont(_T("Malgun Gothic"), 11.0f, Gdiplus::FontStyleBold);
	m_itemNameText.SetColor(255, 255, 255, 255);
	m_itemNameText.SetAlignment(Gdiplus::StringAlignmentNear);
	m_itemNameText.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	m_itemNameText.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_itemNameText.SetShadowColor(255, 0, 0, 0);
	m_itemNameText.SetShadowOffset(1.0f, 1.0f);

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
	if (m_pBackgroundImage) {
		delete m_pBackgroundImage;
		m_pBackgroundImage = nullptr;
	}

	DestroyWindow();
}

void CShopDialog::PostNcDestroy()
{
	GdiDialogBase::PostNcDestroy();
}

void CShopDialog::OnUpdateLogic(float deltaTime)
{
	CPoint cursorPos;
	if (GetCursorPos(&cursorPos)) {
		ScreenToClient(&cursorPos);
		UpdateHoveredItem(cursorPos);
	}

	if (m_hoveredItemId >= 0 && m_pDoc) {
		const UpgradeData* pUpgrade = m_pDoc->GetGameCore().GetUpgrade(m_hoveredItemId);
		if (pUpgrade) {
			m_tooltipTitle.SetText(CString(pUpgrade->name.c_str()));
			
			CString stats;
			stats.Format(_T("가격: %.0f ZPC | 생산: +%.1f/s"), pUpgrade->GetCurrentCost(), pUpgrade->productionPerSecond);
			m_tooltipStats.SetText(stats);

			m_tooltipDesc.SetText(CString(pUpgrade->description.c_str()));
		}
	}
}

void CShopDialog::OnRenderContent(Gdiplus::Graphics& graphics, float deltaTime)
{
	if (m_pBackgroundImage && m_pBackgroundImage->GetLastStatus() == Gdiplus::Ok) {
		graphics.DrawImage(m_pBackgroundImage, 0, 0, 200, 600);
	}

	m_btnClose.Draw(graphics, deltaTime);

	for (size_t i = 0; i < m_shopButtons.size(); i++) {
		auto* pBtn = m_shopButtons[i];
		if (pBtn && pBtn->IsWindowVisible()) {
			pBtn->Draw(graphics, deltaTime);

			if (m_pDoc) {
				const UpgradeData* pUpgrade = m_pDoc->GetGameCore().GetUpgrade((int)i);
				if (pUpgrade) {
					CRect btnRect;
					pBtn->GetWindowRect(&btnRect);
					ScreenToClient(&btnRect);

					CString itemName(pUpgrade->name.c_str());
					m_itemNameText.SetText(itemName);
					Gdiplus::RectF nameTextRect(
						(float)btnRect.left + 4.0f, (float)btnRect.top,
						(float)btnRect.Width() - 16.0f, (float)btnRect.Height());
					m_itemNameText.Draw(graphics, nameTextRect);

					CString qtyStr;
					qtyStr.Format(_T("x%d"), pUpgrade->owned);
					m_quantityText.SetText(qtyStr);
					Gdiplus::RectF qtyTextRect(
						(float)btnRect.left, (float)btnRect.top,
						(float)btnRect.Width() - 10.0f, (float)btnRect.Height());
					m_quantityText.Draw(graphics, qtyTextRect);
				}
			}
		}
	}

	if (m_hoveredItemId >= 0 && m_pDoc) {
		m_tooltipTitle.Draw(graphics, 100.0f, 38.0f);
		m_tooltipStats.Draw(graphics, 100.0f, 53.0f);
		m_tooltipDesc.Draw(graphics, 100.0f, 68.0f);
	}
}

void CShopDialog::OnItemButtonClick(int itemId)
{
	if (!m_pDoc) {
		return;
	}

	bool success = m_pDoc->GetGameCore().PurchaseUpgrade(itemId);
	
	if (success) {
		TRACE(_T("아이템 %d 구매 성공!\n"), itemId);

	} else {
		TRACE(_T("아이템 %d 구매 실패 - 재화 부족\n"), itemId);
		MessageBeep(MB_ICONEXCLAMATION);
	}
}



void CShopDialog::OnCloseButtonClick()
{
	PostMessage(WM_CLOSE);
}

void CShopDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (point.y >= 0 && point.y <= 40) {
		CRect closeRect;
		m_btnClose.GetWindowRect(&closeRect);
		ScreenToClient(&closeRect);
		
		if (!closeRect.PtInRect(point)) {
			m_bDragging = true;
			m_dragStartPoint = point;
			SetCapture();
		}
	}
	
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CShopDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging) {
		m_bDragging = false;
		ReleaseCapture();
	}
	
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CShopDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging) {
		CPoint screenPoint = point;
		ClientToScreen(&screenPoint);
		
		CRect windowRect;
		GetWindowRect(&windowRect);
		
		int newX = screenPoint.x - m_dragStartPoint.x;
		int newY = screenPoint.y - m_dragStartPoint.y;
		
		SetWindowPos(nullptr, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	} else {
		UpdateHoveredItem(point);
	}
	
	CDialogEx::OnMouseMove(nFlags, point);
}

void CShopDialog::UpdateHoveredItem(CPoint point)
{
	int newHoveredId = -1;

	for (size_t i = 0; i < m_shopButtons.size(); i++) {
		auto* pBtn = m_shopButtons[i];
		if (pBtn && pBtn->IsWindowVisible()) {
			CRect rect;
			pBtn->GetWindowRect(&rect);
			ScreenToClient(&rect);

			if (rect.PtInRect(point)) {
				newHoveredId = (int)i;
				break;
			}
		}
	}

	m_hoveredItemId = newHoveredId;
}
