#ifndef __EXCEPT_MINIDUMP_HPP
#define __EXCEPT_MINIDUMP_HPP


#include <DbgHelp.h>
#include <Shlwapi.h>
#include "../../Utility/utility.h"

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "shlwapi.lib")


namespace minidump
{
	// -----------------------------------------------
	// class MiniDump 

	class MiniDump
	{
	public:
		MiniDump()
		{
			::SetUnhandledExceptionFilter(LocalUnhandledExceptionFilter);
		}

	private:
		static void DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo)
		{
			if (excpInfo == NULL) 
			{
				__try 
				{
					::RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
				} 
				__except(DumpMiniDump(hFile, GetExceptionInformation()),
					EXCEPTION_CONTINUE_EXECUTION) 
				{
				}
			} 
			else
			{
				MINIDUMP_EXCEPTION_INFORMATION eInfo;
				eInfo.ThreadId = ::GetCurrentThreadId();
				eInfo.ExceptionPointers = excpInfo;
				eInfo.ClientPointers = FALSE;
				::MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
					hFile, MiniDumpNormal, excpInfo ? &eInfo : NULL, NULL, NULL);
			}
		}

		static LONG WINAPI LocalUnhandledExceptionFilter(PEXCEPTION_POINTERS pExcept)
		{
			TCHAR szDir[MAX_PATH] = {0};
			TCHAR szExe[MAX_PATH] = {0};
			::GetModuleFileName(NULL, szDir, _countof(szDir));
			stdex::tString szVer = utility::GetFileVersion(szDir);

			LPTSTR pFile = ::PathFindFileName(szDir);
			LPTSTR pExt  = ::PathFindExtension(szDir);
			if (pFile != szDir && pExt != NULL)
			{
				*pExt = 0;
				lstrcpy(szExe, pFile);
			}
			else
			{
				lstrcpy(szExe, TEXT("Crash"));
			}

			::PathRemoveFileSpec(szDir);
			::PathAddBackslash(szDir);
			lstrcat(szDir, TEXT("Dump\\"));
			::CreateDirectory(szDir, NULL);

			SYSTEMTIME st = {0};
			::GetLocalTime(&st);
			TCHAR szModuleName[MAX_PATH] = {0};
			_stprintf_s(szModuleName, TEXT("%s%s-%s-%04d%02d%02d%02d%02d%02d.dmp"), szDir, szExe, szVer.c_str(), 
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

			HANDLE hFile = ::CreateFile(szModuleName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				DumpMiniDump(hFile, pExcept);
				::CloseHandle(hFile);
			}
			return 0;
		}
	};
}



#endif