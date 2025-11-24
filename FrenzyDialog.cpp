// FrenzyDialog.cpp: 구현 파일
//

#include "pch.h"
#include "afxdialogex.h"
#include "FrenzyDialog.h"
#include "resource.h"


// CFrenzyDialog 대화 상자

IMPLEMENT_DYNAMIC(CFrenzyDialog, CDialogEx)

CFrenzyDialog::CFrenzyDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FRENZY, pParent)
{

}

CFrenzyDialog::~CFrenzyDialog()
{
}

void CFrenzyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFrenzyDialog, CDialogEx)
	ON_BN_CLICKED(IDC_FRENZY, &CFrenzyDialog::OnBnClickedFrenzy)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CFrenzyDialog 메시지 처리기

void CFrenzyDialog::OnBnClickedFrenzy()
{
	if (m_onClickCallback) {
		m_onClickCallback();
	}
    KillTimer(1);
	ShowWindow(SW_HIDE);
}

BOOL CFrenzyDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetTimer(1, 13000, nullptr); // 13 seconds
	return TRUE;
}

void CFrenzyDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {
		KillTimer(1);
		ShowWindow(SW_HIDE);
	}
	CDialogEx::OnTimer(nIDEvent);
}
