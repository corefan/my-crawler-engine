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

	bool isMaxWnd_;		// �Ƿ����
	bool isDraging_;	// �Ƿ��϶�

	CPoint ptMove_;		// �϶����������
	
	WTL::CWindowPlacement m_wndPlace;	// ����λ����Ϣ

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);


protected:
	void SetShape(const CRect &rc);		// ���ô�����״
	void MaxWnd();						// ��󻯴���
	void Restore();						// �ָ�������Ϣ

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


