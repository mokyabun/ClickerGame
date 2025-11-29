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
    CString GetResourcePath(const CString& relativePath)
    {
        return m_resourceBasePath + relativePath;
    }

    CString GetAbsolutePath(const CString& relativePath)
    {
        return m_basePath + relativePath;
    }

    void SetBasePath(const CString& basePath)
    {
        m_basePath = basePath;
        NormalizeBasePath();
        m_resourceBasePath = m_basePath + _T("Resources\\");
    }

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

        if (IsDevEnvironment(exeDir))
        {
            m_basePath = exeDir + _T("..\\..\\");
        }
        else
        {
            m_basePath = exeDir;
        }

        NormalizeBasePath();
        m_resourceBasePath = m_basePath + _T("Resources\\");
    }

    bool IsDevEnvironment(const CString& path) const
    {
        return path.Find(_T("\\x64\\Debug")) != -1 || path.Find(_T("\\x64\\Release")) != -1;
    }

    void NormalizeBasePath()
    {
        WCHAR fullPath[MAX_PATH];
        GetFullPathName(m_basePath, MAX_PATH, fullPath, NULL);
        m_basePath = fullPath;
        
        if (!m_basePath.IsEmpty() && m_basePath.Right(1) != _T("\\"))
            m_basePath += _T("\\");
    }

    PathResolver(const PathResolver&) = delete;
    PathResolver& operator=(const PathResolver&) = delete;

private:
    CString m_basePath;
    CString m_resourceBasePath;
};
