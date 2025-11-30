
// ClickerGameDoc.h: CClickerGameDoc 클래스의 인터페이스
//


#pragma once
#include "GameCore.h"


class CClickerGameDoc : public CDocument
{
	DECLARE_DYNCREATE(CClickerGameDoc)

public:
	CClickerGameDoc() noexcept;
	virtual ~CClickerGameDoc();

// 특성입니다.
public:
	GameCore& GetGameCore() { return m_gameCore; }

private:
	GameCore m_gameCore;

	// Serialization helpers
	CString SerializeGameState(const GameState& state) const;
	void DeserializeGameState(GameState& state, const CString& data) const;

	CString GenerateBasicStatsString(const GameState& state) const;
	CString GenerateUpgradesString(const GameState& state) const;
	CString GenerateSpecialUpgradesString(const GameState& state) const;

	void ParseUpgradeLine(GameState& state, const CString& line) const;
	void ParseSpecialUpgradeLine(GameState& state, const CString& line) const;
	void ParseBasicStatLine(GameState& state, const CString& line) const;

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};
