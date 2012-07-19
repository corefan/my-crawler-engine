// Implementation of the CNotifyIconData class and the CTrayIconImpl template.
#pragma once

#include "atlmisc.h"
#include "atluser.h"
#include <cstring>


// Wrapper class for the Win32 NOTIFYICONDATA structure
class CNotifyIconData 
	: public NOTIFYICONDATA
{
public:	
	CNotifyIconData()
	{
		memset(this, 0, sizeof(NOTIFYICONDATA));
		cbSize = sizeof(NOTIFYICONDATA);
	}
};

// Template used to support adding an icon to the taskbar.
// This class will maintain a taskbar icon and associated context menu.
template <class T>
class CTrayIconImpl
{
private:
	CNotifyIconData m_nid;
	bool m_bInstalled;
	UINT m_nDefault;
	UINT WM_TRAYICON;

public:	
	CTrayIconImpl(UINT uMsg) 
		: m_bInstalled(false)
		, m_nDefault(0)
		, WM_TRAYICON(uMsg)
	{}
	
	~CTrayIconImpl()
	{
		// Remove the icon
		RemoveIcon();
	}

	// Install a taskbar icon
	// 	lpszToolTip 	- The tooltip to display
	//	hIcon 		- The icon to display
	// 	nID		- The resource ID of the context menu
	/// returns true on success
	bool InstallIcon(LPCTSTR lpszToolTip, HICON hIcon, UINT nID)
	{
		T* pT = static_cast<T*>(this);
		// Fill in the data		
		m_nid.hWnd = pT->m_hWnd;
		m_nid.uID = nID;
		m_nid.hIcon = hIcon;
		m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		m_nid.uCallbackMessage = WM_TRAYICON;
		::_tcscpy_s(m_nid.szTip, lpszToolTip);
		// Install
		m_bInstalled = Shell_NotifyIcon(NIM_ADD, &m_nid) ? true : false;
		// Done
		return m_bInstalled;
	}

	bool ChangeIcon(HICON hIcon)
	{
		m_nid.hIcon = hIcon;
		m_nid.uFlags = NIF_ICON;
		// Change
		return Shell_NotifyIcon(NIM_MODIFY, &m_nid) ? true : false;
	}

	// Remove taskbar icon
	// returns true on success
	bool RemoveIcon()
	{
		if (!m_bInstalled)
			return false;
		// Remove
		m_nid.uFlags = 0;
		return Shell_NotifyIcon(NIM_DELETE, &m_nid) ? true : false;
	}

	// Set the icon tooltip text
	// returns true on success
	bool SetTooltipText(LPCTSTR pszTooltipText)
	{
		if (pszTooltipText == NULL)
			return FALSE;
		// Fill the structure
		m_nid.uFlags = NIF_TIP;
		_tcscpy(m_nid.szTip, pszTooltipText);
		// Set
		return Shell_NotifyIcon(NIM_MODIFY, &m_nid) ? true : false;
	}

	// Set the default menu item ID
	void SetDefaultItem(UINT nID) 
	{ 
		m_nDefault = nID; 
	}



	LRESULT OnTrayIcon(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
	{
		// Is this the ID we want?
		if (wParam != m_nid.uID)
			return 0;
		T* pT = static_cast<T*>(this);
		// Was the right-button clicked?
		if (LOWORD(lParam) == WM_RBUTTONUP)
		{
			// Load the menu
			WTL::CMenu oMenu;
			if (!oMenu.LoadMenu(m_nid.uID))
				return 0;
			// Get the sub-menu
			WTL::CMenuHandle oPopup(oMenu.GetSubMenu(0));
			// Prepare
			pT->PrepareMenu(oPopup);
			// Get the menu position
			WTL::CPoint pos;
			GetCursorPos(&pos);
			// Make app the foreground
			SetForegroundWindow(pT->m_hWnd);
			// Set the default menu item
			if (m_nDefault == 0)
				oPopup.SetMenuDefaultItem(0, TRUE);
			else
				oPopup.SetMenuDefaultItem(m_nDefault);
			// Track
			oPopup.TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, pT->m_hWnd);
			// BUGFIX: See "PRB: Menus for Notification Icons Don't Work Correctly"
			pT->PostMessage(WM_NULL);
			// Done
			oMenu.DestroyMenu();
		}
		else if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
		{
			// Make app the foreground
			SetForegroundWindow(pT->m_hWnd);
			// Load the menu
			WTL::CMenu oMenu;
			if (!oMenu.LoadMenu(m_nid.uID))
				return 0;
			// Get the sub-menu
			WTL::CMenuHandle oPopup(oMenu.GetSubMenu(0));			
			// Get the item
			if (m_nDefault)
			{
				// Send
				pT->SendMessage(WM_COMMAND, m_nDefault, 0);
			}
			else
			{
				UINT nItem = oPopup.GetMenuItemID(0);
				// Send
				pT->SendMessage(WM_COMMAND, nItem, 0);
			}
			// Done
			oMenu.DestroyMenu();
		}
		return 0;
	}

	// Allow the menu items to be enabled/checked/etc.
	virtual void PrepareMenu(HMENU hMenu)
	{
		// Stub
	}
};
