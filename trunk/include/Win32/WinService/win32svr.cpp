#include "stdafx.h"
#include "win32svr.h"
#include <stdio.h>

namespace i8desk
{
	CWin32Svr* CWin32Svr::m_pThis = NULL;

	CWin32Svr::CWin32Svr(LPCTSTR lpszSvrName, LPCTSTR lpszSvrDispName, LPCTSTR lpszsvrPath,
		LPCTSTR lpszSvrDesc, DWORD dwControlsAccepted) :
	m_dwCtrlAccepted(dwControlsAccepted),
		m_bDebug(FALSE),
		m_hExited(NULL)
	{
		m_pThis = this;
		lstrcpy(m_szSvrName, lpszSvrName);
		if (lpszSvrDispName == NULL || lstrlen(lpszSvrDispName) == 0)
			lstrcpy(m_szSvrDispName, lpszSvrName);
		else
			lstrcpy(m_szSvrDispName, lpszSvrDispName);

		if(lpszsvrPath != NULL )
			lstrcpy(m_szSvrPath, lpszsvrPath);

		if (lpszSvrDesc != NULL)
			lstrcpy(m_szSvrDesc, lpszSvrDesc);
		m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	CWin32Svr::~CWin32Svr()
	{
		if (m_hExited != NULL)
		{
			SetEvent(m_hExited);
			CloseHandle(m_hExited);
		}
	}

	DWORD CWin32Svr::InstallService(LPCTSTR lpszSvrName, LPCTSTR lpszSvrDispName)
	{
		SC_HANDLE   schService;
		SC_HANDLE   schSCManager;

		TCHAR szPath[512] = {0};
		if (0 == GetModuleFileName(NULL, szPath, 512 ))
			return GetLastError();

		schSCManager = OpenSCManager(
			NULL,                   // machine (NULL == local)
			NULL,                   // database (NULL == default)
			SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE  // access required
			);
		if (schSCManager == NULL)
			return GetLastError();

		schService = CreateService(
			schSCManager,               // SCManager database
			lpszSvrName,				// name of service
			lpszSvrDispName,			// name to display
			SERVICE_ALL_ACCESS,         // desired access
			SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,  // service type
			SERVICE_AUTO_START,			// start type
			SERVICE_ERROR_NORMAL,       // error control type
			szPath,                     // service's binary
			NULL,                       // no load ordering group
			NULL,                       // no tag identifier
			NULL,						// dependencies
			NULL,                       // LocalSystem account
			NULL);                      // no password
		if (schService == NULL)
		{
			DWORD dwErr = GetLastError();
			CloseServiceHandle(schSCManager);
			return dwErr;
		}

		// 		SERVICE_FAILURE_ACTIONS action = {0};
		// 		action.dwResetPeriod = 30;
		// 		action.cActions = 3;
		// 		action.lpsaActions = new SC_ACTION[action.cActions];
		// 		for (DWORD idx=0; idx<action.cActions; idx++)
		// 		{
		// 			action.lpsaActions[idx].Type = SC_ACTION_RESTART;
		// 			action.lpsaActions[idx].Delay = 30 * 1000;
		// 		}
		// 		ChangeServiceConfig2(schService, SERVICE_CONFIG_FAILURE_ACTIONS, &action);
		// 		delete []action.lpsaActions;

		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);

		return ERROR_SUCCESS;
	}
    BOOL CWin32Svr::CheckService(LPCTSTR lpszSvrName, LPCTSTR lpszSvrDispName, LPCTSTR lpszSvrExePath)
    {
        if (lpszSvrName == NULL || lpszSvrDispName == NULL)
        {
            return FALSE;
        }
        TCHAR szExePath[512] = {0};
        if (lpszSvrExePath == NULL)
        {
            if (0 == GetModuleFileName(NULL, szExePath, _countof(szExePath) ))
                return FALSE;
            lpszSvrExePath = szExePath;
        }
        SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE | SERVICE_CHANGE_CONFIG);
        if (schSCManager == NULL)
        {
            return FALSE;
        }
        //如果没有安装服务，则安装服务
        SC_HANDLE schService = OpenService(schSCManager, lpszSvrName, SERVICE_ALL_ACCESS);	
        if (schService == NULL)
        {
            if (GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
            {
                schService = CreateService(schSCManager, lpszSvrName, lpszSvrDispName, SERVICE_ALL_ACCESS, 
                    SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
                    lpszSvrExePath, NULL, NULL, NULL, NULL, NULL);
            }
            if (schService == NULL)
            {
                CloseServiceHandle(schSCManager);
                return FALSE;
            }
        }
        else
        {
            //修改服务的配置。
            ChangeServiceConfig(schService, SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS, 
                SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
                lpszSvrExePath, NULL, NULL, NULL, NULL, NULL, lpszSvrName);
        }
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return TRUE;
    }
	DWORD CWin32Svr::RemoveService(LPCTSTR lpszSvrName)
	{
		DWORD dwError = ERROR_SUCCESS;

		SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
		if (schSCManager == NULL)
			return GetLastError();

		SC_HANDLE schService = OpenService(schSCManager, lpszSvrName, DELETE | SERVICE_STOP | SERVICE_QUERY_STATUS);
		if (schService == NULL)
		{
			dwError = GetLastError();
			CloseServiceHandle(schSCManager);
			return dwError;
		}

		// try to stop the service
		SERVICE_STATUS ssStatus;
		if ( ControlService( schService, SERVICE_CONTROL_STOP, &ssStatus ) )
		{
			_tprintf(TEXT("Stopping %s."), lpszSvrName);
			Sleep( 1000 );

			while ( QueryServiceStatus( schService, &ssStatus ) )
			{
				if ( ssStatus.dwCurrentState == SERVICE_STOP_PENDING )
				{
					_tprintf(TEXT("."));
					Sleep( 1000 );
				}
				else
					break;
			}

			if ( ssStatus.dwCurrentState != SERVICE_STOPPED )
				dwError = GetLastError();
		}
		if (dwError == ERROR_SUCCESS)
		{
			if (!::DeleteService(schService) )
				dwError = GetLastError();
		}

		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);

		return dwError;
	}

	bool CWin32Svr::HasService(LPCTSTR lpszSvrName)
	{
		SC_HANDLE hSCM = NULL;
		SC_HANDLE hService = NULL;

		bool suc = false;
		__try 
		{
			// Open the SCM database
			hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
			if (!hSCM) 
			{
				__leave;
			}

			// Open the specified service
			hService = OpenService(hSCM, lpszSvrName, SERVICE_START | SERVICE_QUERY_STATUS);
			if (!hService) 
			{
				__leave;
			}

			suc = true;	
		}
		__finally 
		{
			if (hService)
				CloseServiceHandle(hService);

			if (hSCM)
				CloseServiceHandle(hSCM);
		}

		return suc;
	}

	bool CWin32Svr::IsServiceRunning(LPCTSTR lpszSvrName)
	{

		SC_HANDLE hSCM = NULL;
		SC_HANDLE hService = NULL;

		bool suc = false;
		__try 
		{
			// Open the SCM database
			hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
			if (!hSCM) 
			{
				__leave;
			}

			// Open the specified service
			hService = OpenService(hSCM, lpszSvrName, SERVICE_START | SERVICE_QUERY_STATUS);
			if (!hService) 
			{
				__leave;
			}

			SERVICE_STATUS ss;
			// Check the status until the service is no longer start pending. 
			if (!QueryServiceStatus(hService,&ss))
			{
				__leave;
			}

			if (ss.dwCurrentState == SERVICE_RUNNING)
			{
				suc = true;
				__leave;
			}
		}
		__finally 
		{
			if (hService)
				CloseServiceHandle(hService);

			if (hSCM)
				CloseServiceHandle(hSCM);
		}

		return suc;
	}

	DWORD CWin32Svr::StartService(LPCTSTR lpszSvrName)
	{
		SC_HANDLE hSCM = NULL;
		SC_HANDLE hService = NULL;
		DWORD dwErrorCode = ERROR_SUCCESS;

		__try 
		{
			// Open the SCM database
			hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
			if (!hSCM) 
			{
				dwErrorCode = GetLastError();
				__leave;
			}

			// Open the specified service
			hService = OpenService(hSCM, lpszSvrName, SERVICE_START | SERVICE_QUERY_STATUS);
			if (!hService) 
			{
				dwErrorCode = GetLastError();
				__leave;
			}

			SERVICE_STATUS ss;
			// Check the status until the service is no longer start pending. 
			if (!QueryServiceStatus(hService,&ss))
			{
				dwErrorCode = GetLastError();
				__leave;
			}

			if (ss.dwCurrentState == SERVICE_RUNNING)
			{
				dwErrorCode = ERROR_SUCCESS;
				__leave;
			}

			if (ss.dwCurrentState != SERVICE_STOPPED)
			{
				dwErrorCode = E_UNEXPECTED;
				__leave;
			}

			if (!::StartService(hService, 0, NULL))
			{
				dwErrorCode = GetLastError();
				__leave;
			}		
		}
		__finally 
		{
			if (hService)
				CloseServiceHandle(hService);

			if (hSCM)
				CloseServiceHandle(hSCM);
		}
		return dwErrorCode;
	}

	DWORD CWin32Svr::StopService(LPCTSTR lpszSvrName)
	{
		DWORD dwError = 0;
		SC_HANDLE hSCM = NULL;
		SC_HANDLE hService = NULL;
		DWORD dwErrorCode = ERROR_SUCCESS;
		try 
		{
			hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
			if (hSCM == NULL) 
				throw GetLastError();
			hService = OpenService(hSCM, lpszSvrName, 
				SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS);
			if (hService == NULL) 
				throw GetLastError();

			SERVICE_STATUS ss;
			ss.dwWaitHint = 1000;
			if (!QueryServiceStatus(hService,&ss))
				throw GetLastError();

			if (ss.dwCurrentState == SERVICE_STOP)
			{

			}
			else if( ss.dwCurrentState == SERVICE_RUNNING)
			{
				// 停止服务
				if( ::ControlService( hService,	SERVICE_CONTROL_STOP, &ss) == FALSE)
				{				
					throw GetLastError();
				}
				// 等待服务停止
				while( ::QueryServiceStatus( hService, &ss) == TRUE)
				{
					::Sleep( ss.dwWaitHint);
					if( ss.dwCurrentState == SERVICE_STOPPED)
					{
						break;
					}
				}
			}
		}
		catch (DWORD Error)	{ dwError = Error; }
		catch (...)			{ dwError = GetLastError(); }

		if (hService != NULL)
			CloseServiceHandle(hService);

		if (hSCM != NULL)
			CloseServiceHandle(hSCM);	
		return dwError;
	}

	DWORD CWin32Svr::GetWin32ErrorText(LPTSTR buf, DWORD dwSize, DWORD dwError /* = GetLastError */)
	{
		if (dwSize == 0) return 0;
		LPTSTR lpszTemp = NULL;
		DWORD dwRet = FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
			NULL,
			dwError,
			LANG_NEUTRAL,
			(LPTSTR)&lpszTemp,
			0,
			NULL );

		if ( !dwRet || dwSize < dwRet )
			buf[0] = TEXT('\0');
		else
		{
			if (dwRet > 2)
			{
				lpszTemp[dwRet-2] = 0;
			}
			lstrcpy(buf, lpszTemp);			
		}

		if ( lpszTemp )
			LocalFree((HLOCAL) lpszTemp );

		return dwRet;
	}

	DWORD CWin32Svr::RunService()
	{
		SERVICE_TABLE_ENTRY dispatchTable[] =
		{
			{ m_szSvrName, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
			{ NULL, NULL}
		};

		m_bDebug = FALSE;
		if (!StartServiceCtrlDispatcher(dispatchTable))
			return GetLastError();
		return ERROR_SUCCESS;
	}

	void CWin32Svr::DebugService()
	{
		m_bDebug = TRUE;
		BOOL flag = SetConsoleCtrlHandler( ConsoleHandler, TRUE );	
		Run();
	}

	void CWin32Svr::ShowHelp()
	{

	}

	DWORD CWin32Svr::ChangeConfig()
	{
		SC_HANDLE schSCManager = OpenSCManager(
			NULL,                   // machine (NULL == local)
			NULL,                   // database (NULL == default)
			SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE | SERVICE_CHANGE_CONFIG // access required
			);
		if (schSCManager == NULL)
			return GetLastError();

		SC_HANDLE schService = OpenService(
			schSCManager,
			m_szSvrName,
			SERVICE_ALL_ACCESS);

		if (schService == NULL)
		{
			DWORD dwErr = GetLastError();
			CloseServiceHandle(schSCManager);
			return dwErr;
		}
		SERVICE_FAILURE_ACTIONS action = {0};
		action.dwResetPeriod = 30;
		action.cActions = 3;
		action.lpsaActions = new SC_ACTION[action.cActions];
		for (DWORD idx=0; idx<action.cActions; idx++)
		{
			action.lpsaActions[idx].Type = SC_ACTION_RESTART;
			action.lpsaActions[idx].Delay = 30 * 1000;
		}
		ChangeServiceConfig2(schService, SERVICE_CONFIG_FAILURE_ACTIONS, &action);

		//修改服务的配置。
		ChangeServiceConfig(schService, SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS, 
			SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
			m_szSvrPath, NULL, NULL, NULL, NULL, NULL, m_szSvrName);

// 		//保证服务运行起来。
// 		StartService(schService, 0, NULL);

		delete []action.lpsaActions;
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);

		return ERROR_SUCCESS;
	}

	DWORD CWin32Svr::ProcessCmdLine(int argc, TCHAR *argv[])
	{
		CWin32Svr* pSvr = AfxGetWin32Svr();
		DWORD dwError = ERROR_SUCCESS;
		TCHAR szErorr[1024] = {0};
		if ( (argc == 2) &&	((*argv[1] == _T('-')) || (*argv[1] == _T('/'))) )
		{
			if ( _tcsicmp(TEXT("install"), argv[1] + 1) == 0 ||
				_tcsicmp(TEXT("i"), argv[1] + 1) == 0)
			{
				dwError = pSvr->InstallService();
				if (dwError != 0)
				{
					GetWin32ErrorText(szErorr, 1024, dwError);
					_tprintf(szErorr);
				}
				else
				{
					_tprintf(TEXT("install service success."));
				}
				return dwError;
			}
			else if ( _tcsicmp( _T("remove"), argv[1]+1 ) == 0  ||
				_tcsicmp( _T("r"), argv[1] + 1) == 0)
			{
				dwError = pSvr->RemoveService();
				if (dwError != 0)
				{
					GetWin32ErrorText(szErorr, 1024, dwError);
					_tprintf(szErorr);
				}
				else
				{
					_tprintf(TEXT("remove service success."));
				}
				return dwError;
			}
			else if ( _tcsicmp( _T("debug"), argv[1]+1 ) == 0  ||
				_tcsicmp( _T("d"), argv[1] + 1) == 0)
			{
				pSvr->DebugService();
				if (dwError != 0)
				{
					GetWin32ErrorText(szErorr, 1024, dwError);
					_tprintf(szErorr);
				}
				return dwError;
			}			
		}

		LPCTSTR lpServerName = pSvr->GetSvrName();
		_tprintf( TEXT("%s -i(nstall)   to install the service\n"), lpServerName);
		_tprintf( TEXT("%s -r(emove)    to remove the service\n"),  lpServerName );
		_tprintf( TEXT("%s -d(ebug)	  to run as a console app for debugging\n"),	lpServerName);
		_tprintf( TEXT("\nStartServiceCtrlDispatcher being called.\n" ));
		_tprintf( TEXT("This may take several seconds.  Please wait.\n" ));

		return pSvr->RunService();
	}

	DWORD CWin32Svr::ReportStatusToSCM(DWORD dwStatus)
	{
		if (m_bDebug)
		{
			TCHAR buf[MAX_PATH] = {0};
			_stprintf_s(buf, TEXT("report service status: %d\r\n"), dwStatus);
			OutputDebugString(buf);
			return ERROR_SUCCESS;
		}
		SERVICE_STATUS ServiceStatus = {0};
		ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
		if (dwStatus == SERVICE_START_PENDING)
			ServiceStatus.dwControlsAccepted = 0;    
		else
			ServiceStatus.dwControlsAccepted = m_dwCtrlAccepted;

		ServiceStatus.dwCurrentState = dwStatus;
		ServiceStatus.dwWin32ExitCode = NO_ERROR;
		ServiceStatus.dwServiceSpecificExitCode = NO_ERROR;
		ServiceStatus.dwCheckPoint = 0;
		ServiceStatus.dwWaitHint = 60000;

		if (!SetServiceStatus(m_hStatus, &ServiceStatus))
			return GetLastError();

		return ERROR_SUCCESS;
	}

	DWORD  CWin32Svr::AddLogToEvent(LPCTSTR fmt, ...)
	{
		HANDLE hEvent = RegisterEventSource(NULL, m_szSvrName);

		TCHAR xbuf[1024] = {0};
		va_list marker;
		va_start(marker, fmt);
		int len = _vstprintf_s(xbuf, fmt, marker);
		va_end(marker);
		LPCTSTR p[1] = {xbuf};

		ReportEvent(hEvent, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, p, NULL);
		DeregisterEventSource(hEvent);

		return ERROR_SUCCESS;
	}

	DWORD  CWin32Svr::AddLogToEvent(DWORD dwError /* = GetLastError */)
	{
		TCHAR szError[1024] = {0};
		GetWin32ErrorText(szError, 1024, dwError);
		AddLogToEvent(TEXT("Error:%d,Description:%s"), dwError, szError);
		return ERROR_SUCCESS;
	}

	DWORD CWin32Svr::Run()
	{
		ReportStatusToSCM(SERVICE_START_PENDING);
		OnStart();
		ReportStatusToSCM(SERVICE_RUNNING);
		WaitForSingleObject(m_hExited, INFINITE);		
		OnStop();
		ReportStatusToSCM(SERVICE_STOPPED);		

		return ERROR_SUCCESS;
	}

	void __stdcall CWin32Svr::ServiceCtrlHandler(DWORD dwControl)
	{
		CWin32Svr* pSvr = AfxGetWin32Svr();
		switch (dwControl)
		{
		case SERVICE_CONTROL_STOP: 
			{
				pSvr->Stop();
				//pSvr->ReportStatusToSCM(SERVICE_STOPPED);
				break;
			}
		case SERVICE_CONTROL_PAUSE:
			{
				pSvr->OnSuspend();
				//pSvr->ReportStatusToSCM(SERVICE_PAUSED);
				break;
			}
		case SERVICE_CONTROL_CONTINUE:
			{
				pSvr->OnResume();
				//pSvr->ReportStatusToSCM(SERVICE_RUNNING);
				break;
			}
		case SERVICE_CONTROL_INTERROGATE:
			{
				pSvr->OnInterrogate();
				break;
			}
		case SERVICE_CONTROL_SHUTDOWN:
			{
				pSvr->OnShutdown();
				if (pSvr->m_hExited)
					SetEvent(pSvr->m_hExited);
				//pSvr->ReportStatusToSCM(SERVICE_STOPPED);
				break;
			}
		default:
			{
				pSvr->OnUserDefinedRequest(dwControl);
				break;
			}
		}
	}

	void __stdcall CWin32Svr::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
	{
		CWin32Svr* pSvr = AfxGetWin32Svr();

		pSvr->m_hStatus = RegisterServiceCtrlHandler(pSvr->m_szSvrName, ServiceCtrlHandler);
		pSvr->Run();
	}

	BOOL __stdcall CWin32Svr::ConsoleHandler(DWORD dwCtrlType)
	{
		switch (dwCtrlType)
		{
		case CTRL_BREAK_EVENT:
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			{
				CWin32Svr* pSvr = AfxGetWin32Svr();
				pSvr->Stop();
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}
