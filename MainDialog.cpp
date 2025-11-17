// MainDialog.cpp: 구현 파일
//

#include "pch.h"
#include "ClickerGame.h"
#include "afxdialogex.h"
#include "MainDialog.h"
#include "ShopDialog.h"
#include "ClickerGameDoc.h"
#include "PathResolver.h"
#include <shlobj.h>


// CMainDialog 대화 상자

IMPLEMENT_DYNAMIC(CMainDialog, CDialogEx)

CMainDialog::CMainDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN, pParent)
	, m_pBackgroundImage(nullptr)
	, m_pDoc(nullptr)
	, m_nTimerID(0)
	, m_nAutoSaveTimerID(0)
	, m_lastFrameTime(0)
	, m_bDragging(false)
	, m_pShopDialog(nullptr)
{
}

CMainDialog::~CMainDialog()
{
	if (m_pShopDialog)
	{
		if (m_pShopDialog->GetSafeHwnd())
			m_pShopDialog->DestroyWindow();
		delete m_pShopDialog;
		m_pShopDialog = nullptr;
	}
	
	if (m_pDoc)
	{
		delete m_pDoc;
		m_pDoc = nullptr;
	}
	
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

void CMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CMainDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 윈도우 스타일 변경 (레이어드 윈도우, WS_CAPTION은 MFC 내부 처리를 위해 유지)
	ModifyStyle(WS_THICKFRAME | WS_SYSMENU, 0);
	ModifyStyleEx(0, WS_EX_LAYERED);
	
	// 투명 키 설정 (RGB(255, 0, 255) 마젠타 색상을 투명하게)
	::SetLayeredWindowAttributes(m_hWnd, RGB(255, 0, 255), 0, LWA_COLORKEY);

	// 윈도우 크기 설정
	SetWindowPos(nullptr, 0, 0, 800, 600, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
	CenterWindow();

	// GDI+ 배경 이미지 로드
	CString imagePath = PathResolver::GetInstance().GetResourcePath(L"Main\\Background.png");
	m_pBackgroundImage = Gdiplus::Image::FromFile(imagePath);
	if (m_pBackgroundImage == nullptr || m_pBackgroundImage->GetLastStatus() != Gdiplus::Ok)
	{
		CString msg;
		msg.Format(L"배경 이미지를 로드할 수 없습니다!\n경로: %s", imagePath);
		MessageBox(msg, L"오류", MB_OK | MB_ICONERROR);
	}

	// 닫기 버튼 생성 (좌표 기반, 오른쪽 위 21x21 크기)
	const int btnWidth = 21;
	const int btnHeight = 21;
	const int yMargin = 5;
	const int xMargin = 5;
	
	CRect btnRect(800 - btnWidth - xMargin, yMargin, 800 - xMargin, yMargin + btnHeight);
	m_btnClose.Create(L"", btnRect, this, 1002);
	m_btnClose.SetAtlasImage(L"Main\\X.png", btnWidth, btnHeight);

	// 상점 버튼 생성 (왼쪽 위, 48x48 아이콘 + 텍스트 영역)
	const int shopBtnWidth = 75;
	const int shopBtnHeight = 75;
	const int shopX = 20;
	const int shopY = 40;
	CRect shopBtnRect(shopX, shopY, shopX + shopBtnWidth, shopY + shopBtnHeight);
	m_btnShop.Create(L"", shopBtnRect, this, 1004);
	m_btnShop.SetIconImage(L"Main\\Shop.png");
	m_btnShop.SetName(L"Shop");
	m_btnShop.SetTextColor(Gdiplus::Color(255, 255, 255, 255));
	m_btnShop.SetFont(L"Tahoma", 10.0f, Gdiplus::FontStyleRegular);
	m_btnShop.SetOnClickCallback([this]() { OnShopButtonClick(); });
	m_btnShop.SetOnTriggerCallback([this]() { OnShopButtonTrigger(); });

	// 업그레이드 버튼 생성 (상점 버튼 아래)
	const int upgradeX = shopX;
	const int upgradeY = shopY + shopBtnHeight + 10;
	CRect upgradeBtnRect(upgradeX, upgradeY, upgradeX + shopBtnWidth, upgradeY + shopBtnHeight);
	m_btnUpgrade.Create(L"", upgradeBtnRect, this, 1005);
	m_btnUpgrade.SetIconImage(L"Main\\Upgrade.png");
	m_btnUpgrade.SetName(L"Upgrade");
	m_btnUpgrade.SetTextColor(Gdiplus::Color(255, 255, 255, 255));
	m_btnUpgrade.SetFont(L"Tahoma", 10.0f, Gdiplus::FontStyleRegular);
	m_btnUpgrade.SetOnClickCallback([this]() { OnUpgradeButtonClick(); });
	m_btnUpgrade.SetOnTriggerCallback([this]() { OnUpgradeButtonTrigger(); });

	// 설정 버튼 생성 (업그레이드 버튼 아래)
	const int settingsX = shopX;
	const int settingsY = upgradeY + shopBtnHeight + 10;
	CRect settingsBtnRect(settingsX, settingsY, settingsX + shopBtnWidth, settingsY + shopBtnHeight);
	m_btnSettings.Create(L"", settingsBtnRect, this, 1006);
	m_btnSettings.SetIconImage(L"Main\\Settings.png");
	m_btnSettings.SetName(L"Settings");
	m_btnSettings.SetTextColor(Gdiplus::Color(255, 255, 255, 255));
	m_btnSettings.SetFont(L"Tahoma", 10.0f, Gdiplus::FontStyleRegular);
	m_btnSettings.SetOnClickCallback([this]() { OnSettingsButtonClick(); });
	m_btnSettings.SetOnTriggerCallback([this]() { OnSettingsButtonTrigger(); });

	// 바이러스 버튼 생성 (화면 중앙, 확대를 고려해 20% 더 큰 영역으로 생성)
	const int virusBtnWidth = 245;
	const int virusBtnHeight = 245;
	const float expandMargin = 1.2f; // hover 시 최대 크기
	const int expandedWidth = (int)(virusBtnWidth * expandMargin);
	const int expandedHeight = (int)(virusBtnHeight * expandMargin);
	const int centerX = 800 / 2 - expandedWidth / 2;
	const int centerY = 600 / 2 - expandedHeight / 2;
	
	CRect virusBtnRect(centerX, centerY, centerX + expandedWidth, centerY + expandedHeight);
	m_btnVirus.Create(L"", virusBtnRect, this, 1003);
	m_btnVirus.SetImage(L"Main\\Virus.png");

	// 고정 크기 버퍼 생성 (800x600)
	CClientDC dc(this);
	m_memDC.CreateCompatibleDC(&dc);
	m_memBitmap.CreateCompatibleBitmap(&dc, 800, 600);
	m_memDC.SelectObject(&m_memBitmap);
	
	// 자식 레이어 등록 (그리기 순서대로)
	m_childLayers.push_back(&m_btnVirus);
	m_childLayers.push_back(&m_btnShop);
	m_childLayers.push_back(&m_btnUpgrade);
	m_childLayers.push_back(&m_btnSettings);
	m_childLayers.push_back(&m_btnClose);
	
	// Document 생성 및 GameCore 초기화
	m_pDoc = new CClickerGameDoc();
	m_pDoc->OnNewDocument();
	
	// 자동 저장 파일 경로 설정
	TCHAR appDataPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appDataPath)))
	{
		m_saveFilePath.Format(L"%s\\ClickerGame", appDataPath);
		// 디렉토리 생성
		CreateDirectory(m_saveFilePath, NULL);
		m_saveFilePath += L"\\autosave.json";
		
		// 기존 저장 파일 로드
		LoadGameState();
	}
	
	// GameCore 콜백 등록
	m_pDoc->GetGameCore().SetOnClicksChangedCallback([this](double clicks) {
		// 클릭 수 변경 시 화면 갱신 (다음 프레임에서 자동 갱신됨)
	});
	
	// 60 FPS 타이머 시작 (16ms)
	m_nTimerID = SetTimer(1, 16, nullptr);
	m_lastFrameTime = GetTickCount();
	
	// 30초 자동 저장 타이머 시작
	m_nAutoSaveTimerID = SetTimer(2, 30000, nullptr);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CMainDialog, CDialogEx)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(1002, &CMainDialog::OnCloseButtonClick)
	ON_BN_CLICKED(1003, &CMainDialog::OnVirusButtonClick)
END_MESSAGE_MAP()


// CMainDialog 메시지 처리기

BOOL CMainDialog::OnEraseBkgnd(CDC* pDC)
{
	// 깜박임 방지
	return TRUE;
}

void CMainDialog::OnPaint()
{
	CPaintDC dc(this);
	
	// 중앙 렌더링 버퍼를 화면에 복사만 수행
	if (m_memDC.GetSafeHdc())
	{
		dc.BitBlt(0, 0, 800, 600, &m_memDC, 0, 0, SRCCOPY);
	}
}

void CMainDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_nTimerID)
	{
		// deltaTime는 RenderAllLayers에서 계산됨
		RenderAllLayers();
	}
	else if (nIDEvent == m_nAutoSaveTimerID)
	{
		// 30초마다 자동 저장
		SaveGameState();
	}
	
	CDialogEx::OnTimer(nIDEvent);
}

void CMainDialog::OnDestroy()
{
	if (m_nTimerID)
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}
	
	if (m_nAutoSaveTimerID)
	{
		KillTimer(m_nAutoSaveTimerID);
		m_nAutoSaveTimerID = 0;
	}
	
	CDialogEx::OnDestroy();
}

void CMainDialog::OnClose()
{
	// 타이머 정리
	if (m_nTimerID)
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}
	
	if (m_nAutoSaveTimerID)
	{
		KillTimer(m_nAutoSaveTimerID);
		m_nAutoSaveTimerID = 0;
	}
	
	// 최종 저장
	SaveGameState();
	
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
	
	// 다이얼로그 종료
	CDialogEx::OnClose();
	
	// 애플리케이션 종료
	AfxGetMainWnd()->PostMessage(WM_QUIT, 0, 0);
}

void CMainDialog::RenderAllLayers()
{
	// deltaTime 계산 (초 단위)
	DWORD currentTime = GetTickCount();
	float deltaTime = (currentTime - m_lastFrameTime) / 1000.0f;
	m_lastFrameTime = currentTime;
	
	// GameCore 업데이트 (자동 생산)
	if (m_pDoc)
	{
		m_pDoc->GetGameCore().Update(deltaTime);
	}
	
	// 파티클 시스템 업데이트
	m_particleSystem.Update(deltaTime);
	
	// 투명 키 색상으로 배경 채우기
	m_memDC.FillSolidRect(0, 0, 800, 600, RGB(255, 0, 255));
	
	// GDI+ Graphics 생성
	Gdiplus::Graphics graphics(m_memDC.GetSafeHdc());
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
	
	// 배경 이미지 그리기
	if (m_pBackgroundImage && m_pBackgroundImage->GetLastStatus() == Gdiplus::Ok)
	{
		graphics.DrawImage(m_pBackgroundImage, 0, 0, 800, 600);
	}
	
	// 모든 자식 레이어 그리기
	for (auto* pChild : m_childLayers)
	{
		if (pChild && pChild->IsWindowVisible())
		{
			CRect rect;
			pChild->GetWindowRect(&rect);
			ScreenToClient(&rect);
			
			CPoint offset(rect.left, rect.top);
			pChild->Draw(graphics, offset, deltaTime);
		}
	}
	
	// 오른쪽 위에 "Windows XP" 텍스트 그리기 (그림자 효과)
	{
		Gdiplus::Font font(L"Tahoma", 10, Gdiplus::FontStyleBold);

		const float textX = 29;
		const float textY = 6;
		
		// 그림자 그리기 (검은색, 약간 오프셋)
		Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(180, 0, 0, 0)); // 반투명 검은색
		Gdiplus::PointF shadowPoint(textX + 1, textY + 1);
		graphics.DrawString(L"Windows XP", -1, &font, shadowPoint, &shadowBrush);
		
		// 실제 텍스트 그리기 (흰색)
		Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
		Gdiplus::PointF point(textX, textY);
		graphics.DrawString(L"Windows XP", -1, &font, point, &brush);
	}
	
	// 파티클 렌더링 (ZPC 텍스트 아래에 그려짐)
	m_particleSystem.Draw(graphics);
	
	// 화면 중앙 위에 ZPC 재화 표시 (그림자 효과)
	{
		CString currencyText;
		if (m_pDoc)
		{
			currencyText.Format(L"%.0f ZPC", m_pDoc->GetGameCore().GetTotalClicks());
		}
		else
		{
			currencyText = L"0 ZPC";
		}
		
		Gdiplus::Font font(L"Tahoma", 20, Gdiplus::FontStyleBoldItalic);
		Gdiplus::StringFormat format;
		format.SetAlignment(Gdiplus::StringAlignmentCenter);
		
		const float centerX = 400.0f;
		const float topY = 55.0f;
		
		// Shadow
		Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(200, 0, 0, 0));
		Gdiplus::PointF shadowPoint(centerX + 1, topY + 1);
		graphics.DrawString(currencyText, -1, &font, shadowPoint, &format, &shadowBrush);
		
		// Text
		Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
		Gdiplus::PointF point(centerX, topY);
		graphics.DrawString(currencyText, -1, &font, point, &format, &brush);
	}
	
	// 오른쪽 아래에 "시작" 텍스트 그리기 (그림자 효과)
	{
		Gdiplus::Font font(L"맑은 고딕", 13, Gdiplus::FontStyleBold);

		const float textX = 35;
		const float textY = 568;
		
		// 그림자 그리기 (검은색, 약간 오프셋)
		Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(180, 0, 0, 0)); // 반투명 검은색
		Gdiplus::PointF shadowPoint(textX + 1, textY + 1);
		graphics.DrawString(L"시작", -1, &font, shadowPoint, &shadowBrush);
		
		// 실제 텍스트 그리기 (흰색)
		Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
		Gdiplus::PointF point(textX, textY);
		graphics.DrawString(L"시작", -1, &font, point, &brush);
	}
	
	// 오른쪽 아래에 시계 그리기 (그림자 효과)
	{
		// 현재 시간 가져오기
		SYSTEMTIME st;
		GetLocalTime(&st);
		
		// 시간 문자열 생성 (오후 3:45 형식)
		CString timeStr;
		int hour12 = st.wHour % 12;
		if (hour12 == 0) hour12 = 12;
		timeStr.Format(L"%d:%02d %s", hour12, st.wMinute, st.wHour >= 12 ? L"PM" : L"AM");
		
		Gdiplus::Font font(L"Tahoma", 8, Gdiplus::FontStyleBold);
		
		const float textX = 738;
		const float textY = 574;
		
		// 실제 텍스트 그리기 (흰색)
		Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
		Gdiplus::PointF point(textX, textY);
		graphics.DrawString(timeStr, -1, &font, point, &brush);
	}
	
	// 화면 갱신
	InvalidateRect(nullptr, FALSE);
}

void CMainDialog::OnCloseButtonClick()
{
	// X 버튼 클릭 시 WM_CLOSE 메시지 전송 (정상 종료 프로세스)
	PostMessage(WM_CLOSE);
}

void CMainDialog::OnMouseMove(UINT nFlags, CPoint point)
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
	
	CDialogEx::OnMouseMove(nFlags, point);
}

void CMainDialog::OnLButtonDown(UINT nFlags, CPoint point)
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
			m_dragStartPoint = point;
			SetCapture();
			return;
		}
	}
	
	// 배경 클릭 시 모든 아이콘 버튼 포커스 해제
	CRect shopRect, upgradeRect, settingsRect;
	m_btnShop.GetWindowRect(&shopRect);
	m_btnUpgrade.GetWindowRect(&upgradeRect);
	m_btnSettings.GetWindowRect(&settingsRect);
	ScreenToClient(&shopRect);
	ScreenToClient(&upgradeRect);
	ScreenToClient(&settingsRect);
	
	if (!shopRect.PtInRect(point))
	{
		m_btnShop.Deselect();
	}
	if (!upgradeRect.PtInRect(point))
	{
		m_btnUpgrade.Deselect();
	}
	if (!settingsRect.PtInRect(point))
	{
		m_btnSettings.Deselect();
	}
	
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CMainDialog::OnVirusButtonClick()
{
	// 바이러스 버튼 클릭 시 클릭 수 증가
	if (m_pDoc)
	{
		m_pDoc->GetGameCore().PerformClick();
		
		// 현재 마우스 위치에서 파티클 방출
		CPoint mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(&mousePos);
		
		// 마우스 위치에서 10~15개의 파티클 방출
		int particleCount = 10 + (rand() % 6);
		m_particleSystem.Emit((float)mousePos.x, (float)mousePos.y, particleCount);
	}
}

void CMainDialog::SaveGameState()
{
	if (!m_pDoc || m_saveFilePath.IsEmpty())
		return;
	
	// Document의 SaveToFile 메서드 사용
	m_pDoc->SaveToFile(m_saveFilePath);
}

void CMainDialog::LoadGameState()
{
	if (!m_pDoc || m_saveFilePath.IsEmpty())
		return;
	
	// Document의 LoadFromFile 메서드 사용
	m_pDoc->LoadFromFile(m_saveFilePath);
}

void CMainDialog::OnShopButtonClick()
{
	// 상점 버튼 클릭 시 다른 아이콘 버튼 선택 해제
	if (m_btnShop.IsSelected())
	{
		m_btnUpgrade.Deselect();
		m_btnSettings.Deselect();
	}
	
	CString msg;
	msg.Format(L"상점 버튼 클릭 - 선택 상태: %s", 
		m_btnShop.IsSelected() ? L"선택됨" : L"선택 안됨");
	TRACE(msg + L"\n");
}

void CMainDialog::OnShopButtonTrigger()
{
	// 상점 버튼 더블클릭 시 (트리거)
	TRACE(L"상점 버튼 더블클릭 - 상점 열기\n");
	
	// 이미 열려있으면 포커스만 주기
	if (m_pShopDialog && m_pShopDialog->GetSafeHwnd())
	{
		m_pShopDialog->SetForegroundWindow();
		return;
	}
	
	// 새로 생성
	m_pShopDialog = new CShopDialog(this);
	m_pShopDialog->SetDocument(m_pDoc);
	
	// Modeless 다이얼로그 생성
	if (m_pShopDialog->Create(IDD_SHOP, this))
	{
		// 메인 창 오른쪽에 위치 설정
		CRect mainRect;
		GetWindowRect(&mainRect);
		
		// 상점 창은 200x600 크기
		int shopX = mainRect.right;
		int shopY = mainRect.top;
		
		m_pShopDialog->SetWindowPos(nullptr, shopX, shopY, 200, 600, SWP_NOZORDER);
		m_pShopDialog->ShowWindow(SW_SHOW);
	}
	else
	{
		delete m_pShopDialog;
		m_pShopDialog = nullptr;
	}
}

void CMainDialog::OnUpgradeButtonClick()
{
	// 업그레이드 버튼 클릭 시 다른 아이콘 버튼 선택 해제
	if (m_btnUpgrade.IsSelected())
	{
		m_btnShop.Deselect();
		m_btnSettings.Deselect();
	}
	
	CString msg;
	msg.Format(L"업그레이드 버튼 클릭 - 선택 상태: %s", 
		m_btnUpgrade.IsSelected() ? L"선택됨" : L"선택 안됨");
	TRACE(msg + L"\n");
}

void CMainDialog::OnUpgradeButtonTrigger()
{
	// 업그레이드 버튼 더블클릭 시 (트리거)
	AfxMessageBox(L"업그레이드 열기!");
	TRACE(L"업그레이드 버튼 더블클릭 - 업그레이드 열기\n");
}

void CMainDialog::OnSettingsButtonClick()
{
	// 설정 버튼 클릭 시 다른 아이콘 버튼 선택 해제
	if (m_btnSettings.IsSelected())
	{
		m_btnShop.Deselect();
		m_btnUpgrade.Deselect();
	}
	
	CString msg;
	msg.Format(L"설정 버튼 클릭 - 선택 상태: %s", 
		m_btnSettings.IsSelected() ? L"선택됨" : L"선택 안됨");
	TRACE(msg + L"\n");
}

void CMainDialog::OnSettingsButtonTrigger()
{
	// 설정 버튼 더블클릭 시 (트리거)
	AfxMessageBox(L"설정 열기!");
	TRACE(L"설정 버튼 더블클릭 - 설정 열기\n");
}

void CMainDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		m_bDragging = false;
		ReleaseCapture();
	}
	
	CDialogEx::OnLButtonUp(nFlags, point);
}
