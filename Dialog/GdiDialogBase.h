#pragma once
#include "afxdialogex.h"
#include "GdiButton.h"
#include <vector>
#include <gdiplus.h>

class GdiDialogBase : public CDialogEx
{
	DECLARE_DYNAMIC(GdiDialogBase)

public:
	GdiDialogBase(UINT nIDTemplate, CWnd* pParent = nullptr);
	virtual ~GdiDialogBase();

protected:
	virtual void OnGdiInitialize() {}
	virtual void OnRenderContent(Gdiplus::Graphics& graphics, float deltaTime) = 0;
	virtual void OnUpdateLogic(float deltaTime) {}
	void InitializeRenderBuffer(int width, int height);
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;

	DECLARE_MESSAGE_MAP()

	// Message Handlers
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

private:
	CDC m_memDC;
	CBitmap m_memBitmap;
	int m_bufferWidth;
	int m_bufferHeight;
	
	// Timer
	UINT_PTR m_nTimerID;
	DWORD m_lastFrameTime;
	
	// Rendering Function
	void RenderFrame();
};
