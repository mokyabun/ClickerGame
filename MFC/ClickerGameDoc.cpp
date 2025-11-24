
// ClickerGameDoc.cpp: CClickerGameDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "ClickerGame.h"
#endif

#include "ClickerGameDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CClickerGameDoc

IMPLEMENT_DYNCREATE(CClickerGameDoc, CDocument)

BEGIN_MESSAGE_MAP(CClickerGameDoc, CDocument)
END_MESSAGE_MAP()


// CClickerGameDoc 생성/소멸

CClickerGameDoc::CClickerGameDoc() noexcept
{
}

CClickerGameDoc::~CClickerGameDoc()
{
}

BOOL CClickerGameDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// 새 문서 시작 시 게임 상태 초기화
	m_gameCore.GetState().Reset();

	return TRUE;
}

// CClickerGameDoc serialization helpers

CString CClickerGameDoc::SerializeGameState(const GameState& state) const {
    CString data;
    data = _T("GAMESTATE\n");
    
    // 기본 게임 상태 저장
    CString line;
    line.Format(_T("TOTAL_CLICKS=%.2f\n"), state.GetTotalClicks());
    data += line;
    
    line.Format(_T("CLICK_POWER=%.2f\n"), state.GetClickPower());
    data += line;
    
    // 업그레이드 데이터 저장
    data += _T("UPGRADES_START\n");
    const auto& upgrades = state.GetUpgrades();
    for (const auto& upgrade : upgrades) {
        line.Format(_T("%d:%d\n"), upgrade.id, upgrade.owned);
        data += line;
    }
    data += _T("UPGRADES_END\n");
    
    // 특별 업그레이드 데이터 저장
    data += _T("SPECIAL_UPGRADES_START\n");
    const auto& specialUpgrades = state.GetSpecialUpgrades();
    for (const auto& specialUpgrade : specialUpgrades) {
        if (specialUpgrade.isPurchased) {
            line.Format(_T("%d:1\n"), specialUpgrade.id);
            data += line;
        }
    }
    data += _T("SPECIAL_UPGRADES_END\n");
    
    return data;
}

void CClickerGameDoc::DeserializeGameState(GameState& state, const CString& data) const {
    // 라인 단위로 파싱
    int pos = 0;
    bool inUpgrades = false;
    bool inSpecialUpgrades = false;
    
    while (pos < data.GetLength()) {
        // 다음 줄바꿈 찾기
        int newlinePos = data.Find(_T('\n'), pos);
        if (newlinePos < 0) {
            newlinePos = data.GetLength();
        }
        
        // 현재 라인 추출
        CString line = data.Mid(pos, newlinePos - pos);
        line.Trim();
        
        if (!line.IsEmpty()) {
            if (line == _T("UPGRADES_START")) {
                inUpgrades = true;
                inSpecialUpgrades = false;
            }
            else if (line == _T("UPGRADES_END")) {
                inUpgrades = false;
            }
            else if (line == _T("SPECIAL_UPGRADES_START")) {
                inSpecialUpgrades = true;
                inUpgrades = false;
            }
            else if (line == _T("SPECIAL_UPGRADES_END")) {
                inSpecialUpgrades = false;
            }
            else if (inUpgrades) {
                // 업그레이드 데이터 파싱: id:owned
                int colonPos = line.Find(_T(':'));
                if (colonPos > 0) {
                    CString idStr = line.Left(colonPos);
                    CString ownedStr = line.Mid(colonPos + 1);
                    idStr.Trim();
                    ownedStr.Trim();
                    
                    int id = _wtoi(idStr);
                    int owned = _wtoi(ownedStr);
                    
                    auto& upgrades = state.GetUpgrades();
                    if (id >= 0 && id < static_cast<int>(upgrades.size())) {
                        upgrades[id].owned = owned;
                    }
                }
            }
            else if (inSpecialUpgrades) {
                // 특별 업그레이드 데이터 파싱: id:1
                int colonPos = line.Find(_T(':'));
                if (colonPos > 0) {
                    CString idStr = line.Left(colonPos);
                    idStr.Trim();
                    
                    int id = _wtoi(idStr);
                    
                    auto& specialUpgrades = state.GetSpecialUpgrades();
                    if (id >= 0 && id < static_cast<int>(specialUpgrades.size())) {
                        specialUpgrades[id].isPurchased = true;
                    }
                }
            }
            else {
                // 키=값 형식 파싱
                int equalPos = line.Find(_T('='));
                if (equalPos > 0) {
                    CString key = line.Left(equalPos);
                    CString value = line.Mid(equalPos + 1);
                    key.Trim();
                    value.Trim();
                    
                    if (key == _T("TOTAL_CLICKS")) {
                        state.SetTotalClicks(_wtof(value));
                    }
                    else if (key == _T("CLICK_POWER")) {
                        state.SetClickPower(_wtof(value));
                    }
                }
            }
        }
        
        pos = newlinePos + 1;
    }
}

// CClickerGameDoc serialization

void CClickerGameDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// 커스텀 포맷으로 직렬화하여 저장
		CString data = SerializeGameState(m_gameCore.GetState());
		
		// CString을 UTF-16으로 저장
		int len = data.GetLength();
		ar << len;
		ar.WriteString(data);
	}
	else
	{
		// 커스텀 포맷 역직렬화하여 로드
		int len = 0;
		ar >> len;
		
		if (len > 0 && len < 1000000) // 안전성 체크 (1MB 미만)
		{
			CString data;
			TCHAR* buffer = data.GetBuffer(len + 1);
			ar.Read(buffer, len * sizeof(TCHAR));
			buffer[len] = L'\0';
			data.ReleaseBuffer();
			
			DeserializeGameState(m_gameCore.GetState(), data);
		}
	}
}

// CClickerGameDoc 진단

#ifdef _DEBUG
void CClickerGameDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CClickerGameDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
