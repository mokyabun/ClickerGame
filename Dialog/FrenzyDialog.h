#pragma once
#include "afxdialogex.h"
#include "GdiButton.h"
#include <functional>


// CFrenzyDialog 대화 상자

class CFrenzyDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CFrenzyDialog)

public:
	CFrenzyDialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFrenzyDialog();

    void SetOnClickCallback(std::function<void()> callback) { m_onClickCallback = callback; }

private:
    std::function<void()> m_onClickCallback;
    CGdiButton m_btnGoldVirus;
    ULONG_PTR m_gdiplusToken;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FRENZY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedFrenzy();
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
};
