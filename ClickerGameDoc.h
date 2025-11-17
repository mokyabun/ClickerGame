
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

	// Save/Load 기능
	BOOL SaveToFile(const CString& filePath);
	BOOL LoadFromFile(const CString& filePath);

private:
	GameCore m_gameCore;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
