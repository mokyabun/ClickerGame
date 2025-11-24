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

    // ë¦¬ì†ŒìŠ¤ ê²½ë¡œ ì–»ê¸°
    CString GetResourcePath(const CString& relativePath)
    {
        return m_resourceBasePath + relativePath;
    }

    // ì ˆëŒ€ ê²½ë¡œë¡œ ë³€í™˜
    CString GetAbsolutePath(const CString& relativePath)
    {
        return m_basePath + relativePath;
    }

    // ë² ì´ìŠ¤ ê²½ë¡œ ì„¤ì • (í…ŒìŠ¤íŠ¸ìš©)
    void SetBasePath(const CString& basePath)
    {
        m_basePath = basePath;
        if (!m_basePath.IsEmpty() && m_basePath.Right(1) != _T("\\"))
            m_basePath += _T("\\");

        m_resourceBasePath = m_basePath + _T("Resources\\");
    }

    // í˜„ìž¬ ë² ì´ìŠ¤ ê²½ë¡œ ê°€ì ¸ì˜¤ê¸°
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

        // ë””ë²„ê·¸ ëª¨ë“œì¸ì§€ í™•ì¸ (x64\Debug ë˜ëŠ” x64\Release ê²½ë¡œ í™•ì¸)
        if (exeDir.Find(_T("\\x64\\Debug")) != -1 || exeDir.Find(_T("\\x64\\Release")) != -1)
        {
            // ê°œë°œ í™˜ê²½: í”„ë¡œì íŠ¸ ë£¨íŠ¸ë¡œ ì´ë™
            m_basePath = exeDir + _T("..\\..\\");
        }
        else
        {
            // ë°°í¬ í™˜ê²½: ì‹¤í–‰ íŒŒì¼ê³¼ ê°™ì€ í´ë”
            m_basePath = exeDir;
        }

        // ì •ê·œí™”ëœ ê²½ë¡œë¡œ ë³€í™˜
        WCHAR fullPath[MAX_PATH];
        GetFullPathName(m_basePath, MAX_PATH, fullPath, NULL);
        m_basePath = fullPath;
        
        if (!m_basePath.IsEmpty() && m_basePath.Right(1) != _T("\\"))
            m_basePath += _T("\\");

        m_resourceBasePath = m_basePath + _T("Resources\\");
    }

    PathResolver(const PathResolver&) = delete;
    PathResolver& operator=(const PathResolver&) = delete;

private:
    CString m_basePath;
    CString m_resourceBasePath;
};
