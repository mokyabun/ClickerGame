
// ClickerGameDoc.cpp: CClickerGameDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ClickerGame.h"
#endif

#include "ClickerGameDoc.h"

#include <propkey.h>

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
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

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




// CClickerGameDoc serialization

void CClickerGameDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// JSON으로 직렬화하여 저장
		CString json = m_gameCore.GetState().ToJson();
		
		// CString을 UTF-16으로 저장
		int len = json.GetLength();
		ar << len;
		ar.WriteString(json);
	}
	else
	{
		// JSON 역직렬화하여 로드
		int len = 0;
		ar >> len;
		
		if (len > 0 && len < 1000000) // 안전성 체크 (1MB 미만)
		{
			CString json;
			TCHAR* buffer = json.GetBuffer(len + 1);
			ar.Read(buffer, len * sizeof(TCHAR));
			buffer[len] = L'\0';
			json.ReleaseBuffer();
			
			m_gameCore.GetState().FromJson(json);
		}
	}
}

BOOL CClickerGameDoc::SaveToFile(const CString& filePath)
{
	try
	{
		// JSON 문자열 생성
		CString json = m_gameCore.GetState().ToJson();
		
		// 파일에 쓰기 (UTF-8 BOM)
		CFile file;
		if (file.Open(filePath, CFile::modeCreate | CFile::modeWrite))
		{
			// UTF-8 BOM 쓰기
			unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
			file.Write(bom, 3);
			
			// UTF-16 -> UTF-8 변환
			int utf8Len = WideCharToMultiByte(CP_UTF8, 0, json, -1, NULL, 0, NULL, NULL);
			if (utf8Len > 0)
			{
				char* utf8Buffer = new char[utf8Len];
				WideCharToMultiByte(CP_UTF8, 0, json, -1, utf8Buffer, utf8Len, NULL, NULL);
				file.Write(utf8Buffer, utf8Len - 1); // null terminator 제외
				delete[] utf8Buffer;
			}
			
			file.Close();
			return TRUE;
		}
	}
	catch (CFileException* e)
	{
		e->Delete();
	}
	
	return FALSE;
}

BOOL CClickerGameDoc::LoadFromFile(const CString& filePath)
{
	try
	{
		CFile file;
		if (file.Open(filePath, CFile::modeRead))
		{
			ULONGLONG fileSize = file.GetLength();
			if (fileSize > 0 && fileSize < 1000000) // 1MB 미만
			{
				// 파일 읽기
				char* buffer = new char[(size_t)fileSize + 1];
				file.Read(buffer, (UINT)fileSize);
				buffer[fileSize] = '\0';
				file.Close();
				
				// UTF-8 BOM 스킵
				char* jsonStart = buffer;
				if (fileSize >= 3 && 
					(unsigned char)buffer[0] == 0xEF && 
					(unsigned char)buffer[1] == 0xBB && 
					(unsigned char)buffer[2] == 0xBF)
				{
					jsonStart += 3;
				}
				
				// UTF-8 -> UTF-16 변환
				int wideLen = MultiByteToWideChar(CP_UTF8, 0, jsonStart, -1, NULL, 0);
				if (wideLen > 0)
				{
					WCHAR* wideBuffer = new WCHAR[wideLen];
					MultiByteToWideChar(CP_UTF8, 0, jsonStart, -1, wideBuffer, wideLen);
					
					CString json(wideBuffer);
					m_gameCore.GetState().FromJson(json);
					
					delete[] wideBuffer;
				}
				
				delete[] buffer;
				return TRUE;
			}
		}
	}
	catch (CFileException* e)
	{
		e->Delete();
	}
	
	return FALSE;
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CClickerGameDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CClickerGameDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CClickerGameDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

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


// CClickerGameDoc 명령
