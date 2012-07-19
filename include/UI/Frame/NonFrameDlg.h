#pragma once

#include "../WTL/atlwinmisc.h"

// CNonFrameDlg

class CNonFrameDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNonFrameDlg)

public:
	CNonFrameDlg(CWnd *child, UINT uID, CWnd* pParent = NULL);
	virtual ~CNonFrameDlg();

private:
	CWnd *child_;

	bool isMaxWnd_;		// 是否最大化
	bool isDraging_;	// 是否拖动

	CPoint ptMove_;		// 拖动窗体的坐标
	
	WTL::CWindowPlacement m_wndPlace;	// 窗口位置信息

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);


protected:
	void SetShape(const CRect &rc);		// 设置窗口型状
	void MaxWnd();						// 最大化窗口
	void Restore();						// 恢复窗口信息

public:
	void MaxOrRestore();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	
	
};


