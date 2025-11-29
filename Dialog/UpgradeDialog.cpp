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
	for (auto* pBtn : m_upgradeButtons) {
		if (pBtn) {
			delete pBtn;
		}
	}
	m_upgradeButtons.clear();

	if (m_pBackgroundImage) {
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
	ModifyStyle(WS_THICKFRAME | WS_SYSMENU, 0);
	ModifyStyleEx(0, WS_EX_LAYERED);

	::SetLayeredWindowAttributes(m_hWnd, RGB(255, 0, 255), 0, LWA_COLORKEY);

	SetWindowPos(nullptr, 0, 0, 383, 600, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
	CenterWindow();

	CString imagePath = PathResolver::GetInstance().GetResourcePath(_T("Shop\\UpgradeBackground.png"));
	m_pBackgroundImage = Gdiplus::Image::FromFile(imagePath);
	if (m_pBackgroundImage == nullptr || m_pBackgroundImage->GetLastStatus() != Gdiplus::Ok) {
		CString msg;
		msg.Format(_T("업그레이드 배경 이미지를 로드할 수 없습니다!\n경로: %s"), imagePath);
		MessageBox(msg, _T("오류"), MB_OK | MB_ICONERROR);
	}

	const int btnWidth = 21;
	const int btnHeight = 21;
	const int yMargin = 5;
	const int xMargin = 5;
	
	CRect btnRect(383 - btnWidth - xMargin, yMargin, 383 - xMargin, yMargin + btnHeight);
	m_btnClose.Create(L"", btnRect, this, 4000);
	m_btnClose.SetAtlasImage(_T("Shared\\X.png"), btnWidth, btnHeight);

	const int itemBtnWidth = 178;
	const int itemBtnHeight = 48;
	const int startX = 11;
	const int startY = 100;
	const int spacingX = 183;
	const int spacingY = 48;
	const int columns = 2;

	if (m_pDoc) {
		const auto& specialUpgrades = m_pDoc->GetGameCore().GetSpecialUpgrades();
		int numItems = min(12, (int)specialUpgrades.size());

		for (int i = 0; i < numItems; i++) {
			int col = i % columns;
			int row = i / columns;
			int x = startX + col * spacingX;
			int y = startY + row * spacingY;

			CRect btnRect(x, y, x + itemBtnWidth, y + itemBtnHeight);
			CGdiAtlasButton* pBtn = new CGdiAtlasButton();
			pBtn->Create(L"", btnRect, this, 3000 + i);
			
			pBtn->SetAtlasImage(_T("Shop\\Button.png"), itemBtnWidth, itemBtnHeight);

			const SpecialUpgradeData* pUpgrade = m_pDoc->GetGameCore().GetSpecialUpgrade(i);
			if (pUpgrade) {
				CString itemName(pUpgrade->name.c_str());
				pBtn->GetTextObject().SetText(itemName);
				pBtn->GetTextObject().SetFont(_T("Malgun Gothic"), 9.0f, Gdiplus::FontStyleBold);
				pBtn->GetTextObject().SetColor(255, 255, 255, 255);
				pBtn->GetTextObject().SetShadowStyle(GdiText::ShadowStyle::DropShadow);
				pBtn->GetTextObject().SetShadowColor(255, 0, 0, 0);
				pBtn->GetTextObject().SetShadowOffset(1.0f, 1.0f);
				pBtn->GetTextObject().SetAlignment(Gdiplus::StringAlignmentNear);
				pBtn->GetTextObject().SetLineAlignment(Gdiplus::StringAlignmentCenter);
			}

			pBtn->SetOnClickCallback([this, i]() { OnItemButtonClick(i); });

			m_upgradeButtons.push_back(pBtn);
		}
	}

	m_tooltipTitle.SetFont(_T("Malgun Gothic"), 11.0f, Gdiplus::FontStyleBold);
	m_tooltipTitle.SetColor(255, 255, 220, 120);
	m_tooltipTitle.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipTitle.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipTitle.SetShadowColor(180, 0, 0, 0);

	m_tooltipCategory.SetFont(_T("Malgun Gothic"), 9.0f, Gdiplus::FontStyleItalic);
	m_tooltipCategory.SetColor(255, 150, 150, 255);
	m_tooltipCategory.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipCategory.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipCategory.SetShadowColor(180, 0, 0, 0);

	m_tooltipPrice.SetFont(_T("Malgun Gothic"), 9.0f, Gdiplus::FontStyleBold);
	m_tooltipPrice.SetColor(255, 255, 215, 0);
	m_tooltipPrice.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipPrice.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipPrice.SetShadowColor(180, 0, 0, 0);

	InitializeRenderBuffer(383, 600);
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
	if (m_pBackgroundImage) {
		delete m_pBackgroundImage;
		m_pBackgroundImage = nullptr;
	}

	DestroyWindow();
}

void CUpgradeDialog::PostNcDestroy()
{
	GdiDialogBase::PostNcDestroy();
}

void CUpgradeDialog::OnUpdateLogic(float deltaTime)
{
	CPoint cursorPos;
	if (GetCursorPos(&cursorPos)) {
		ScreenToClient(&cursorPos);
		UpdateHoveredItem(cursorPos);
	}

	if (m_pDoc) {
		for (size_t i = 0; i < m_upgradeButtons.size(); i++) {
			auto* pBtn = m_upgradeButtons[i];
			const SpecialUpgradeData* pUpgrade = m_pDoc->GetGameCore().GetSpecialUpgrade((int)i);
			
			if (pBtn && pUpgrade) {
				bool shouldEnable = !pUpgrade->isPurchased;
				
				if (pBtn->IsWindowEnabled() != shouldEnable) {
					pBtn->EnableWindow(shouldEnable);
				}
			}
		}
	}

	if (m_hoveredItemId >= 0 && m_pDoc) {
		const SpecialUpgradeData* pUpgrade = m_pDoc->GetGameCore().GetSpecialUpgrade(m_hoveredItemId);
		if (pUpgrade) {
			m_tooltipTitle.SetText(CString(pUpgrade->name.c_str()));
			m_tooltipCategory.SetText(CString(_T("[")) + CString(pUpgrade->category.c_str()) + CString(_T("]")));
			
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
	if (m_pBackgroundImage && m_pBackgroundImage->GetLastStatus() == Gdiplus::Ok) {
		graphics.DrawImage(m_pBackgroundImage, 0, 0, 383, 600);
	}

	m_btnClose.Draw(graphics, deltaTime);

	for (auto* pBtn : m_upgradeButtons) {
		if (pBtn && pBtn->IsWindowVisible()) {
			pBtn->Draw(graphics, deltaTime);
		}
	}

	if (m_hoveredItemId >= 0 && m_pDoc) {
		m_tooltipTitle.Draw(graphics, 200.0f, 38.0f);
		m_tooltipCategory.Draw(graphics, 200.0f, 53.0f);
		m_tooltipPrice.Draw(graphics, 200.0f, 68.0f);
	}
}

void CUpgradeDialog::OnItemButtonClick(int itemId)
{
	if (!m_pDoc) {
		return;
	}

	bool success = m_pDoc->GetGameCore().PurchaseSpecialUpgrade(itemId);
	
	if (success) {
		TRACE(L"특별 업그레이드 %d 구매 성공!\n", itemId);
	} else {
		TRACE(L"특별 업그레이드 %d 구매 실패\n", itemId);
		MessageBeep(MB_ICONEXCLAMATION);
	}
}



void CUpgradeDialog::OnCloseButtonClick()
{
	PostMessage(WM_CLOSE);
}

void CUpgradeDialog::OnLButtonDown(UINT nFlags, CPoint point)
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

void CUpgradeDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging) {
		m_bDragging = false;
		ReleaseCapture();
	}
	
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CUpgradeDialog::OnMouseMove(UINT nFlags, CPoint point)
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

void CUpgradeDialog::UpdateHoveredItem(CPoint point)
{
	int newHoveredId = -1;

	for (size_t i = 0; i < m_upgradeButtons.size(); i++) {
		auto* pBtn = m_upgradeButtons[i];
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
