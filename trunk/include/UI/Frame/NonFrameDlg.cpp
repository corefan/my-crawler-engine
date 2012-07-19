// NonFrameDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "NonFrameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CNonFrameDlg

IMPLEMENT_DYNAMIC(CNonFrameDlg, CDialogEx)

CNonFrameDlg::CNonFrameDlg(CWnd *child, UINT uID, CWnd* pParent/* = NULL*/)
: CDialogEx(uID, pParent)
, child_(child)
, isMaxWnd_(false)
, isDraging_(false)
{

}

CNonFrameDlg::~CNonFrameDlg()
{
}


BEGIN_MESSAGE_MAP(CNonFrameDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



void CNonFrameDlg::SetShape(const CRect &rc)										
{
	CRgn rgn;

	rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 5, 5);
	SetWindowRgn(rgn, TRUE);
	InvalidateRect(rc);
}


void CNonFrameDlg::MaxWnd()													
{
	isMaxWnd_ = true;
	m_wndPlace.GetPosData(*this);	// 记录当前窗口的位置和尺寸

	WTL::CWindowRect rcWindow(GetSafeHwnd());

	// 任务栏
	CRect rcTaskBar;
	ATL::CWindow wndTaskBar = ::FindWindow(_T("Shell_TrayWnd"), 0);
	wndTaskBar.GetWindowRect(rcTaskBar);

	// 窗体
	rcWindow.top = 0;
	rcWindow.left = 0;
	if( (rcTaskBar.bottom - rcTaskBar.top) < (rcTaskBar.right - rcTaskBar.left) )
	{
		rcWindow.right = ::GetSystemMetrics(SM_CXSCREEN) + 1;

		if( rcTaskBar.top <= 0 )	// 任务栏在桌面顶端
		{
			rcWindow.top = rcTaskBar.bottom + 1;
			rcWindow.bottom = ::GetSystemMetrics(SM_CYSCREEN) - 1;
		}
		else
		{
			rcWindow.bottom = ::GetSystemMetrics(SM_CYSCREEN) - rcTaskBar.Height() + 5;
		}
	}
	else// 任务栏是竖的
	{
		rcWindow.right = ::GetSystemMetrics(SM_CXSCREEN) - rcTaskBar.Width() + 5;
		rcWindow.bottom = ::GetSystemMetrics(SM_CYSCREEN) + 1;
	}

	MoveWindow(rcWindow);
}

void CNonFrameDlg::Restore()										
{
	isMaxWnd_ = false;
	m_wndPlace.SetPosData(*this);
}

void CNonFrameDlg::MaxOrRestore()
{
	if( !isMaxWnd_ )
		MaxWnd();
	else
		Restore();
}


// CNonFrameDlg 消息处理程序

BOOL CNonFrameDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CNonFrameDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )     
	{     
		if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )     
			return TRUE;       
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CNonFrameDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if( isMaxWnd_ == true )
		return FALSE;

	HCURSOR cursor = 0;

	if( nHitTest == HTCAPTION || nHitTest == HTSYSMENU ||
		nHitTest == HTMENU || nHitTest == HTCLIENT )
		::LoadCursor(AfxGetInstanceHandle(), IDC_ARROW);
	else if( nHitTest == HTTOP || nHitTest == HTBOTTOM )
		::LoadCursor(AfxGetInstanceHandle(), IDC_SIZENS);
	else if( nHitTest == HTLEFT || nHitTest == HTRIGHT )
		::LoadCursor(AfxGetInstanceHandle(), IDC_SIZEWE);
	else if( nHitTest == HTTOPLEFT || nHitTest == HTBOTTOMRIGHT )
		::LoadCursor(AfxGetInstanceHandle(), IDC_SIZENWSE);
	else if( nHitTest == HTTOPRIGHT || nHitTest == HTBOTTOMLEFT )
		::LoadCursor(AfxGetInstanceHandle(), IDC_SIZENESW);
	else
		::LoadCursor(AfxGetInstanceHandle(), IDC_ARROW);

	::SetCursor(cursor);
	::DestroyCursor(cursor);

	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}

LRESULT CNonFrameDlg::OnNcHitTest(CPoint pt)
{
	CRect rect;
	child_->GetWindowRect(&rect);

	if( pt.x <= rect.left+3 )
	{
		return HTLEFT;
	}
	else if( pt.x >= rect.right-3 )
	{
		return HTRIGHT;
	}
	else if( pt.y <= rect.top+3 )
	{
		return HTTOP;
	}
	else if( pt.y >= rect.bottom-3 )
	{
		return HTBOTTOM;
	}    
	else if( pt.x <= rect.left+10 && pt.y <= rect.top+10 )
	{
		return HTTOPLEFT;
	}
	else if( pt.x >= rect.right-10 && pt.y <= rect.top+10 )
	{
		return HTTOPRIGHT;
	}
	else if( pt.x <= rect.left+10 && pt.y >= rect.bottom-10 )
	{
		return HTBOTTOMLEFT;
	}
	else if( pt.x >= rect.right-10 && pt.y >= rect.bottom-10 )
	{
		return HTBOTTOMRIGHT;
	}

	return HTCLIENT; //指示当前鼠标在客户区，将响应OnLButtonDown消息。

	//return CDialogEx::OnNcHitTest(point);
}

void CNonFrameDlg::OnNcLButtonDown(UINT nHitTest, CPoint pt)
{
	if( isMaxWnd_ == true )
		return;

	if( nHitTest == HTTOP )
		child_->SendMessage( WM_SYSCOMMAND, SC_SIZE | WMSZ_TOP, MAKELPARAM(pt.x, pt.y));
	else if( nHitTest == HTBOTTOM )
		child_->SendMessage( WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOM, MAKELPARAM(pt.x, pt.y));
	else if( nHitTest == HTLEFT )
		child_->SendMessage( WM_SYSCOMMAND, SC_SIZE | WMSZ_LEFT, MAKELPARAM(pt.x, pt.y));
	else if( nHitTest == HTRIGHT )
		child_->SendMessage( WM_SYSCOMMAND, SC_SIZE | WMSZ_RIGHT, MAKELPARAM(pt.x, pt.y));
	else if( nHitTest == HTTOPLEFT )
		child_->SendMessage( WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPLEFT, MAKELPARAM(pt.x, pt.y));
	else if( nHitTest == HTTOPRIGHT )
		child_->SendMessage( WM_SYSCOMMAND, SC_SIZE | WMSZ_TOPRIGHT, MAKELPARAM(pt.x, pt.y));
	else if( nHitTest == HTBOTTOMLEFT )
		child_->SendMessage( WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMLEFT, MAKELPARAM(pt.x, pt.y));
	else if( nHitTest == HTBOTTOMRIGHT )
		child_->SendMessage( WM_SYSCOMMAND, SC_SIZE | WMSZ_BOTTOMRIGHT, MAKELPARAM(pt.x, pt.y));


	CDialogEx::OnNcLButtonDown(nHitTest, pt);
}

void CNonFrameDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	MaxOrRestore();

	CDialogEx::OnNcLButtonDblClk(nHitTest, point);
}

void CNonFrameDlg::OnLButtonDblClk(UINT nFlags, CPoint pt)
{
	if( (pt.y > 0 && pt.y < 30) )
	{
		MaxOrRestore();
	}

	CDialogEx::OnLButtonDblClk(nFlags, pt);
}


void CNonFrameDlg::OnLButtonDown(UINT nFlags, CPoint pt)
{
	if( isMaxWnd_ == true )
		return;

	if( (pt.y > 0 && pt.y < 30) )
	{
		isDraging_ = true;

		child_->SetCapture();

		ptMove_ = pt;
	}


	CDialogEx::OnLButtonDown(nFlags, pt);
}

void CNonFrameDlg::OnLButtonUp(UINT nHitTest, CPoint pt)
{
	if( isMaxWnd_ == true )
		return;

	ReleaseCapture();

	isDraging_ = false;

	CDialogEx::OnLButtonUp(nHitTest, pt);
}

void CNonFrameDlg::OnMouseMove(UINT nFlags, CPoint pt)
{
	if( isMaxWnd_ == true )
		return;

	if( isDraging_ ) // 移动窗口
	{
		CRect rect;

		child_->GetWindowRect(&rect);

		rect.left += pt.x - ptMove_.x;
		rect.top += pt.y - ptMove_.y;

		child_->SetWindowPos(&wndTop, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}

	CDialogEx::OnMouseMove(nFlags, pt);
}

void CNonFrameDlg::OnSize(UINT nType, int cx, int cy)
{
	CRect rcClient;
	GetClientRect(rcClient);

	SetShape(rcClient);

	CDialogEx::OnSize(nType, cx, cy);
}



