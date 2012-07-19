#ifndef __win32svr_inc__
#define __win32svr_inc__

#include <wtypes.h>
#include <tchar.h>
#include <WinSvc.h>

namespace i8desk
{
	
	class CWin32Svr
	{
	public:
		CWin32Svr(LPCTSTR lpszSvrName, LPCTSTR lpszSvrDispName = NULL, LPCTSTR lpszsvrPath = NULL, LPCTSTR lpszSvrDesc = NULL, 
			DWORD dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
		virtual ~CWin32Svr();
	public:
		static DWORD InstallService(LPCTSTR lpszSvrName, LPCTSTR lpszSvrDispName);
		static DWORD RemoveService(LPCTSTR lpszSvrName);
		static bool  HasService(LPCTSTR lpszSvrName);
		static bool  IsServiceRunning(LPCTSTR lpszSvrName);
		static DWORD StartService(LPCTSTR lpszSvrName);
		static DWORD StopService(LPCTSTR lpszSvrName);	
		static DWORD GetWin32ErrorText(LPTSTR buf, DWORD dwSize, DWORD dwError = GetLastError());
        static BOOL  CheckService(LPCTSTR lpszSvrName, LPCTSTR lpszSvrDispName, LPCTSTR lpszSvrExePath = NULL);
	public:
		inline DWORD InstallService()		{ return InstallService(m_szSvrName, m_szSvrDispName);}
		inline DWORD RemoveService()		{ return RemoveService(m_szSvrName); }
		inline DWORD StartService()			{ return StartService(m_szSvrName);	 }
		inline DWORD StopService()			{ return StopService(m_szSvrName);	 }

		static CWin32Svr* GetSvrInstance()	{ return m_pThis;	  }
		inline LPCTSTR GetSvrName()			{ return m_szSvrName; }
		inline LPCTSTR GetSvrDispName()		{ return m_szSvrDispName; }
		inline LPCTSTR GetSvrDesc()			{ return m_szSvrDesc; }

		BOOL   IsService()					{ return !m_bDebug;	}
		DWORD  RunService();
		void   DebugService();
		void   ShowHelp();
		DWORD  ProcessCmdLine(int argc, TCHAR *argv[]);
		DWORD  ChangeConfig();

		DWORD  ReportStatusToSCM(DWORD dwStatus);
		DWORD  AddLogToEvent(LPCTSTR fmt, ...);
		DWORD  AddLogToEvent(DWORD dwError = GetLastError());
		DWORD  Stop()
		{
			DWORD dwRet = OnStop();
			if (m_hExited)
			{
				SetEvent(m_hExited);
			}
			return dwRet;
		}
	public:
		virtual DWORD OnStart()				{ return ERROR_SUCCESS; }
		virtual DWORD OnSuspend()			{ return ERROR_SUCCESS; }
		virtual DWORD OnResume()			{ return ERROR_SUCCESS; }
		virtual DWORD OnStop()				{ return ERROR_SUCCESS; }
		virtual DWORD OnInterrogate()		{ return ERROR_SUCCESS; }
		virtual DWORD OnShutdown()			{ return ERROR_SUCCESS; }
		virtual DWORD OnUserDefinedRequest(DWORD dwControl)	{ return ERROR_SUCCESS; }
		virtual DWORD Run();
	private:
		static void __stdcall ServiceCtrlHandler(DWORD dwControl);
		static void __stdcall ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
		static BOOL __stdcall ConsoleHandler(DWORD dwCtrlType);
	private:
		static CWin32Svr* m_pThis;
		TCHAR m_szSvrName[MAX_PATH];
		TCHAR m_szSvrDispName[MAX_PATH];
		TCHAR m_szSvrPath[MAX_PATH];
		TCHAR m_szSvrDesc[MAX_PATH];
		DWORD m_dwCtrlAccepted;
		BOOL  m_bDebug;
		HANDLE m_hExited;

		SERVICE_STATUS_HANDLE m_hStatus;
		DWORD   m_dwCurrentState; 
	};

	inline CWin32Svr* AfxGetWin32Svr()
	{
		return CWin32Svr::GetSvrInstance();
	}
}

#endif