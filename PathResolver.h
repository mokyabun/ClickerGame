#pragma once
#include <atlstr.h>

class PathResolver
{
public:
    static PathResolver& GetInstance()
    {
        static PathResolver instance;
        return instance;
    }

    // 리소스 경로 얻기
    CString GetResourcePath(const CString& relativePath)
    {
        return m_resourceBasePath + relativePath;
    }

    // 절대 경로로 변환
    CString GetAbsolutePath(const CString& relativePath)
    {
        return m_basePath + relativePath;
    }

    // 베이스 경로 설정 (테스트용)
    void SetBasePath(const CString& basePath)
    {
        m_basePath = basePath;
        if (!m_basePath.IsEmpty() && m_basePath.Right(1) != L"\\")
            m_basePath += L"\\";

        m_resourceBasePath = m_basePath + L"Resources\\";
    }

    // 현재 베이스 경로 가져오기
    CString GetBasePath() const
    {
        return m_basePath;
    }

private:
    PathResolver()
    {
        InitializePaths();
    }

    void InitializePaths()
    {
        WCHAR exePath[MAX_PATH];
        GetModuleFileName(NULL, exePath, MAX_PATH);
        
        CString exeDir = exePath;
        int pos = exeDir.ReverseFind('\\');
        if (pos != -1)
            exeDir = exeDir.Left(pos + 1);

        // 디버그 모드인지 확인 (x64\Debug 또는 x64\Release 경로 확인)
        if (exeDir.Find(L"\\x64\\Debug") != -1 || exeDir.Find(L"\\x64\\Release") != -1)
        {
            // 개발 환경: 프로젝트 루트로 이동
            m_basePath = exeDir + L"..\\..\\";
        }
        else
        {
            // 배포 환경: 실행 파일과 같은 폴더
            m_basePath = exeDir;
        }

        // 정규화된 경로로 변환
        WCHAR fullPath[MAX_PATH];
        GetFullPathName(m_basePath, MAX_PATH, fullPath, NULL);
        m_basePath = fullPath;
        
        if (!m_basePath.IsEmpty() && m_basePath.Right(1) != L"\\")
            m_basePath += L"\\";

        m_resourceBasePath = m_basePath + L"Resources\\";
    }

    PathResolver(const PathResolver&) = delete;
    PathResolver& operator=(const PathResolver&) = delete;

private:
    CString m_basePath;
    CString m_resourceBasePath;
};
