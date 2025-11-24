// UpgradeDialog.cpp: êµ¬í˜„ íŒŒì¼
//

#include "pch.h"
#include "ClickerGame.h"
#include "afxdialogex.h"
#include "UpgradeDialog.h"
#include "ClickerGameDoc.h"
#include "PathResolver.h"


// CUpgradeDialog ëŒ€í™” ìƒìž

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
	// ë²„íŠ¼ ì •ë¦¬
	for (auto* pBtn : m_upgradeButtons)
	{
		if (pBtn)
		{
			delete pBtn;
		}
	}
	m_upgradeButtons.clear();

	// ë°°ê²½ ì´ë¯¸ì§€ ì •ë¦¬
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
	// ìœˆë„ìš° ìŠ¤íƒ€ì¼ ë³€ê²½ (ë ˆì´ì–´ë“œ ìœˆë„ìš°)
	ModifyStyle(WS_THICKFRAME | WS_SYSMENU, 0);
	ModifyStyleEx(0, WS_EX_LAYERED);

	// íˆ¬ëª… í‚¤ ì„¤ì • (RGB(255, 0, 255) ë§ˆì  íƒ€ ìƒ‰ìƒì„ íˆ¬ëª…í•˜ê²Œ)
	::SetLayeredWindowAttributes(m_hWnd, RGB(255, 0, 255), 0, LWA_COLORKEY);

	// ìœˆë„ìš° í¬ê¸° ì„¤ì • (400x600 - 2ì—´ ë ˆì´ì•„ì›ƒì„ ìœ„í•´ ë„ˆë¹„ í™•ìž¥)
	SetWindowPos(nullptr, 0, 0, 400, 600, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
	CenterWindow();

	// GDI+ ë°°ê²½ ì´ë¯¸ì§€ ë¡œë“œ
	CString imagePath = PathResolver::GetInstance().GetResourcePath(_T("Shop\\UpgradeBackground.png"));
	m_pBackgroundImage = Gdiplus::Image::FromFile(imagePath);
	if (m_pBackgroundImage == nullptr || m_pBackgroundImage->GetLastStatus() != Gdiplus::Ok)
	{
		CString msg;
		msg.Format(_T("ì—…ê·¸ë ˆì´ë“œ ë°°ê²½ ì´ë¯¸ì§€ë¥¼ ë¡œë“œí•  ìˆ˜ ì—†ìŠµë‹ˆë‹¤!\nê²½ë¡œ: %s"), imagePath);
		MessageBox(msg, _T("ì˜¤ë¥˜"), MB_OK | MB_ICONERROR);
	}

	// ë‹«ê¸° ë²„íŠ¼ ìƒì„± (ì˜¤ë¥¸ìª½ ìœ„ 21x21 í¬ê¸°)
	const int btnWidth = 21;
	const int btnHeight = 21;
	const int yMargin = 5;
	const int xMargin = 5;
	
	CRect btnRect(400 - btnWidth - xMargin, yMargin, 400 - xMargin, yMargin + btnHeight);
	m_btnClose.Create(L"", btnRect, this, 4000);
	m_btnClose.SetAtlasImage(_T("Shared\\X.png"), btnWidth, btnHeight);

	// íŠ¹ë³„ ì—…ê·¸ë ˆì´ë“œ ë²„íŠ¼ ìƒì„± (12ê°œ, 2ì—´ 6í–‰)
	const int itemBtnWidth = 178;
	const int itemBtnHeight = 48;
	const int startX = 11;
	const int startY = 100;
	const int spacingX = 189;  // ë²„íŠ¼ ë„ˆë¹„ + ê°„ê²©
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
			
			// 2-state ì•„í‹€ë¼ìŠ¤ ì„¤ì •
			pBtn->SetAtlasImage(_T("Upgrade\\Button.png"), itemBtnWidth, itemBtnHeight);

			// ì•„ì´í…œ ì´ë¦„ ì„¤ì •
			const SpecialUpgradeData* pUpgrade = m_pDoc->GetGameCore().GetSpecialUpgrade(i);
			if (pUpgrade)
			{
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

			// í´ë¦­ ì½œë°±
			pBtn->SetOnClickCallback([this, i]() { OnItemButtonClick(i); });

			m_upgradeButtons.push_back(pBtn);
		}
	}

	// íˆ´íŒ í…ìŠ¤íŠ¸ ì´ˆê¸°í™”
	m_tooltipTitle.SetFont(_T("Malgun Gothic"), 11.0f, Gdiplus::FontStyleBold);
	m_tooltipTitle.SetColor(255, 255, 220, 120);
	m_tooltipTitle.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipTitle.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipTitle.SetShadowColor(180, 0, 0, 0);

	m_tooltipCategory.SetFont(_T("Malgun Gothic"), 9.0f, Gdiplus::FontStyleItalic);
	m_tooltipCategory.SetColor(255, 150, 150, 255); // ì—°í•œ ë³´ë¼ìƒ‰
	m_tooltipCategory.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipCategory.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipCategory.SetShadowColor(180, 0, 0, 0);

	m_tooltipPrice.SetFont(_T("Malgun Gothic"), 9.0f, Gdiplus::FontStyleBold);
	m_tooltipPrice.SetColor(255, 255, 215, 0); // ê¸ˆìƒ‰
	m_tooltipPrice.SetAlignment(Gdiplus::StringAlignmentCenter);
	m_tooltipPrice.SetShadowStyle(GdiText::ShadowStyle::DropShadow);
	m_tooltipPrice.SetShadowColor(180, 0, 0, 0);

	// ê³ ì • í¬ê¸° ë²„í¼ ìƒì„± (400x600)
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


// CUpgradeDialog ë©”ì‹œì§€ ì²˜ë¦¬ê¸°

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
	// Modeless 대화상자는 부모가 메모리 관리
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

	// 버튼 상태 업데이트 (구매 여부에 따라 비활성화)
	if (m_pDoc)
	{
		for (size_t i = 0; i < m_upgradeButtons.size(); i++)
		{
			auto* pBtn = m_upgradeButtons[i];
			const SpecialUpgradeData* pUpgrade = m_pDoc->GetGameCore().GetSpecialUpgrade((int)i);
			
			if (pBtn && pUpgrade)
			{
				// 구매했다면 비활성화 (GdiAtlasButton::OnEnable에서 Disabled 상태로 전환됨 -> Pressed 이미지 표시)
				bool shouldEnable = !pUpgrade->isPurchased;
				
				// 상태가 변경될 때만 EnableWindow 호출 (불필요한 다시 그리기 방지)
				if (pBtn->IsWindowEnabled() != shouldEnable)
				{
					pBtn->EnableWindow(shouldEnable);
				}
			}
		}
	}

	// 툴팁 텍스트 업데이트
	if (m_hoveredItemId >= 0 && m_pDoc)
	{
		const SpecialUpgradeData* pUpgrade = m_pDoc->GetGameCore().GetSpecialUpgrade(m_hoveredItemId);
		if (pUpgrade)
		{
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
	// ë°°ê²½ ì´ë¯¸ì§€ ê·¸ë¦¬ê¸°
	if (m_pBackgroundImage && m_pBackgroundImage->GetLastStatus() == Gdiplus::Ok)
	{
		graphics.DrawImage(m_pBackgroundImage, 0, 0, 400, 600);
	}


	// ë‹«ê¸° ë²„íŠ¼ ê·¸ë¦¬ê¸°
	m_btnClose.Draw(graphics, deltaTime);

	// ëª¨ë“  ì—…ê·¸ë ˆì´ë“œ ë²„íŠ¼ ê·¸ë¦¬ê¸°
	for (auto* pBtn : m_upgradeButtons)
	{
		if (pBtn && pBtn->IsWindowVisible())
		{
			pBtn->Draw(graphics, deltaTime);
		}
	}

	// í˜¸ë²„ ì¤‘ì¸ ì•„ì´í…œ ì„¤ëª… í‘œì‹œ (ìƒë‹¨)
	if (m_hoveredItemId >= 0 && m_pDoc)
	{
		m_tooltipTitle.Draw(graphics, 200.0f, 38.0f);
		m_tooltipCategory.Draw(graphics, 200.0f, 53.0f);
		m_tooltipPrice.Draw(graphics, 200.0f, 68.0f);
	}
}

void CUpgradeDialog::OnItemButtonClick(int itemId)
{
	if (!m_pDoc)
		return;

	// íŠ¹ë³„ ì—…ê·¸ë ˆì´ë“œ êµ¬ë§¤ ì‹œë„
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
	// X ë²„íŠ¼ í´ë¦­ ì‹œ ë‹¤ì´ì–¼ë¡œê·¸ ë‹«ê¸°
	PostMessage(WM_CLOSE);
}

void CUpgradeDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// ìƒë‹¨ ì˜ì—­(0~40px)ì„ í´ë¦­í•˜ë©´ ë“œëž˜ê·¸ ì‹œìž‘
	if (point.y >= 0 && point.y <= 40)
	{
		// ë‹«ê¸° ë²„íŠ¼ ì˜ì—­ì€ ì œì™¸
		CRect closeRect;
		m_btnClose.GetWindowRect(&closeRect);
		ScreenToClient(&closeRect);
		
		if (!closeRect.PtInRect(point))
		{
			m_bDragging = true;
			
			// ë§ˆìš°ìŠ¤ì˜ ì°½ ë‚´ ìƒëŒ€ ìœ„ì¹˜ ì €ìž¥
			m_dragStartPoint = point;
			
			// ë§ˆìš°ìŠ¤ ìº¡ì²˜
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
		
		// ë§ˆìš°ìŠ¤ ìº¡ì²˜ í•´ì œ
		ReleaseCapture();
	}
	
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CUpgradeDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	// ë“œëž˜ê·¸ ì¤‘ì´ë©´ ì°½ ì´ë™
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
		// í•­ìƒ í˜¸ë²„ ìƒíƒœ ì—…ë°ì´íŠ¸
		UpdateHoveredItem(point);
	}
	
	CDialogEx::OnMouseMove(nFlags, point);
}

void CUpgradeDialog::UpdateHoveredItem(CPoint point)
{
	int newHoveredId = -1;

	// ê° ë²„íŠ¼ ì˜ì—­ í™•ì¸
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

	// í˜¸ë²„ ìƒíƒœ ë³€ê²½
	m_hoveredItemId = newHoveredId;
}
