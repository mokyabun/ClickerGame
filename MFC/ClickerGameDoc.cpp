
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

	m_gameCore.GetState().Reset();

	return TRUE;
}

// CClickerGameDoc serialization helpers

CString CClickerGameDoc::SerializeGameState(const GameState& state) const
{
	CString data = _T("GAMESTATE\n");
	data += GenerateBasicStatsString(state);
	data += GenerateUpgradesString(state);
	data += GenerateSpecialUpgradesString(state);
	return data;
}

CString CClickerGameDoc::GenerateBasicStatsString(const GameState& state) const
{
	CString data;
	CString line;

	line.Format(_T("TOTAL_CLICKS=%.2f\n"), state.GetTotalClicks());
	data += line;

	line.Format(_T("CLICK_POWER=%.2f\n"), state.GetClickPower());
	data += line;

	return data;
}

CString CClickerGameDoc::GenerateUpgradesString(const GameState& state) const
{
	CString data = _T("UPGRADES_START\n");
	CString line;

	for (const auto& upgrade : state.GetUpgrades())
	{
		line.Format(_T("%d:%d\n"), upgrade.id, upgrade.owned);
		data += line;
	}

	data += _T("UPGRADES_END\n");
	return data;
}

CString CClickerGameDoc::GenerateSpecialUpgradesString(const GameState& state) const
{
	CString data = _T("SPECIAL_UPGRADES_START\n");
	CString line;

	for (const auto& specialUpgrade : state.GetSpecialUpgrades())
	{
		if (specialUpgrade.isPurchased)
		{
			line.Format(_T("%d:1\n"), specialUpgrade.id);
			data += line;
		}
	}

	data += _T("SPECIAL_UPGRADES_END\n");
	return data;
}

void CClickerGameDoc::DeserializeGameState(GameState& state, const CString& data) const
{
	int pos = 0;
	bool inUpgrades = false;
	bool inSpecialUpgrades = false;

	while (pos < data.GetLength())
	{
		int newlinePos = data.Find(_T('\n'), pos);
		if (newlinePos < 0)
		{
			newlinePos = data.GetLength();
		}

		CString line = data.Mid(pos, newlinePos - pos);
		line.Trim();

		if (!line.IsEmpty())
		{
			if (line == _T("UPGRADES_START"))
			{
				inUpgrades = true;
				inSpecialUpgrades = false;
			}
			else if (line == _T("UPGRADES_END"))
			{
				inUpgrades = false;
			}
			else if (line == _T("SPECIAL_UPGRADES_START"))
			{
				inSpecialUpgrades = true;
				inUpgrades = false;
			}
			else if (line == _T("SPECIAL_UPGRADES_END"))
			{
				inSpecialUpgrades = false;
			}
			else if (inUpgrades)
			{
				ParseUpgradeLine(state, line);
			}
			else if (inSpecialUpgrades)
			{
				ParseSpecialUpgradeLine(state, line);
			}
			else
			{
				ParseBasicStatLine(state, line);
			}
		}

		pos = newlinePos + 1;
	}
}

void CClickerGameDoc::ParseUpgradeLine(GameState& state, const CString& line) const
{
	int colonPos = line.Find(_T(':'));
	if (colonPos > 0)
	{
		CString idStr = line.Left(colonPos);
		CString ownedStr = line.Mid(colonPos + 1);
		
		int id = _wtoi(idStr);
		int owned = _wtoi(ownedStr);

		auto& upgrades = state.GetUpgrades();
		if (id >= 0 && id < static_cast<int>(upgrades.size()))
		{
			upgrades[id].owned = owned;
		}
	}
}

void CClickerGameDoc::ParseSpecialUpgradeLine(GameState& state, const CString& line) const
{
	int colonPos = line.Find(_T(':'));
	if (colonPos > 0)
	{
		CString idStr = line.Left(colonPos);
		
		int id = _wtoi(idStr);

		auto& specialUpgrades = state.GetSpecialUpgrades();
		if (id >= 0 && id < static_cast<int>(specialUpgrades.size()))
		{
			specialUpgrades[id].isPurchased = true;
		}
	}
}

void CClickerGameDoc::ParseBasicStatLine(GameState& state, const CString& line) const
{
	int equalPos = line.Find(_T('='));
	if (equalPos > 0)
	{
		CString key = line.Left(equalPos);
		CString value = line.Mid(equalPos + 1);
		
		if (key == _T("TOTAL_CLICKS"))
		{
			state.SetTotalClicks(_wtof(value));
		}
		else if (key == _T("CLICK_POWER"))
		{
			state.SetClickPower(_wtof(value));
		}
	}
}

// CClickerGameDoc serialization

void CClickerGameDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		CString data = SerializeGameState(m_gameCore.GetState());
		
		int len = data.GetLength();
		ar << len;
		ar.WriteString(data);
	}
	else
	{
		int len = 0;
		ar >> len;
		
		if (len > 0 && len < 1000000)
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
