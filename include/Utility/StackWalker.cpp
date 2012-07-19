#include "stdafx.h"
#include "StackWalker.h"

#include <cassert>
#include <vector>
#include <string>
#include <DbgHelp.h>

#pragma comment(lib, "dbghelp.lib")


namespace utility
{

	enum { ATL_MODULE_NAME_LEN = MAX_PATH, ATL_SYMBOL_NAME_LEN = 1024 };
	enum { _ATL_MAX_STACK_FRAMES = 20 };


	// Helper class for generating a stack dump
	// This is used internally by AtlDumpStack
	class CStackDumper
	{
	public:
		struct _ATL_SYMBOL_INFO;

		static LPVOID __stdcall FunctionTableAccess(_In_ HANDLE hProcess, _In_ ULONG_PTR dwPCAddress);
		static ULONG_PTR __stdcall GetModuleBase(_In_ HANDLE hProcess, _In_ ULONG_PTR dwReturnAddress);
		static BOOL ResolveSymbol(_In_ HANDLE hProcess, _In_ UINT_PTR dwAddress, _ATL_SYMBOL_INFO &siSymbol);
	};



	struct CStackDumper::_ATL_SYMBOL_INFO
	{
		ULONG_PTR dwAddress;
		ULONG_PTR dwOffset;
		size_t lineNum;
		CHAR	lineName[MAX_PATH];
		CHAR	szModule[ATL_MODULE_NAME_LEN];
		CHAR	szSymbol[ATL_SYMBOL_NAME_LEN];
	};

	LPVOID __stdcall CStackDumper::FunctionTableAccess(_In_ HANDLE hProcess, _In_ ULONG_PTR dwPCAddress)
	{
		return SymFunctionTableAccess(hProcess, (ULONG)dwPCAddress);
	}

	ULONG_PTR __stdcall CStackDumper::GetModuleBase(_In_ HANDLE hProcess, _In_ ULONG_PTR dwReturnAddress)
	{
		IMAGEHLP_MODULE moduleInfo;
		moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);

		if (SymGetModuleInfo(hProcess, (ULONG)dwReturnAddress, &moduleInfo))
			return moduleInfo.BaseOfImage;
		else
		{
			MEMORY_BASIC_INFORMATION memoryBasicInfo = {0};

			if (::VirtualQueryEx(hProcess, (LPVOID) dwReturnAddress, &memoryBasicInfo, sizeof(memoryBasicInfo)))
			{
				DWORD cch = 0;
				char szFile[MAX_PATH] = { 0 };

				cch = GetModuleFileNameA((HINSTANCE)memoryBasicInfo.AllocationBase,
					szFile, MAX_PATH);

				// Ignore the return code since we can't do anything with it.
				SymLoadModule(hProcess,
					NULL, ((cch) ? szFile : NULL),
					NULL, (DWORD)(DWORD_PTR)memoryBasicInfo.AllocationBase, 0);

				return (DWORD_PTR) memoryBasicInfo.AllocationBase;
			}
		}

		return 0;
	}

	BOOL CStackDumper::ResolveSymbol(_In_ HANDLE hProcess, _In_ UINT_PTR dwAddress, _ATL_SYMBOL_INFO &siSymbol)
	{
		BOOL fRetval = TRUE;

		siSymbol.dwAddress = dwAddress;

		CHAR szUndec[ATL_SYMBOL_NAME_LEN];
		CHAR szWithOffset[ATL_SYMBOL_NAME_LEN];
		LPSTR pszSymbol = NULL;
		IMAGEHLP_MODULE mi;

		memset(&siSymbol, 0, sizeof(_ATL_SYMBOL_INFO));
		mi.SizeOfStruct = sizeof(IMAGEHLP_MODULE);

		if( !SymGetModuleInfo(hProcess, (UINT)dwAddress, &mi) )
		{
			strcpy_s(siSymbol.szModule, _countof(siSymbol.szModule), "<no module>");
		}
		else
		{
			LPSTR pszModule = strchr(mi.ImageName, '\\');
			if (pszModule == NULL)
				pszModule = mi.ImageName;
			else
				pszModule++;

			strncpy_s(siSymbol.szModule, _countof(siSymbol.szModule), pszModule, _TRUNCATE);
		}

		__try
		{
			union 
			{
				CHAR rgchSymbol[sizeof(IMAGEHLP_SYMBOL) + ATL_SYMBOL_NAME_LEN];
				IMAGEHLP_SYMBOL  sym;
			} sym;
			memset(&sym.sym, 0x00, sizeof(sym.sym));
			sym.sym.SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
			sym.sym.Address = (DWORD)dwAddress;
			sym.sym.MaxNameLength = ATL_SYMBOL_NAME_LEN;


			if (SymGetSymFromAddr(hProcess, (DWORD)dwAddress, &(siSymbol.dwOffset), &sym.sym))
			{
				pszSymbol = sym.sym.Name;

				if (UnDecorateSymbolName(sym.sym.Name, szUndec, sizeof(szUndec)/sizeof(szUndec[0]), 
					UNDNAME_NO_MS_KEYWORDS | UNDNAME_NO_ACCESS_SPECIFIERS))
				{
					pszSymbol = szUndec;
				}
				else if (SymUnDName(&sym.sym, szUndec, sizeof(szUndec)/sizeof(szUndec[0])))
				{
					pszSymbol = szUndec;
				}
				if (siSymbol.dwOffset != 0)
				{
					sprintf_s(szWithOffset, ATL_SYMBOL_NAME_LEN, "%s", pszSymbol);

					// ensure null-terminated
					szWithOffset[ATL_SYMBOL_NAME_LEN-1] = '\0';

					pszSymbol = szWithOffset;
				}
			}
			else
				pszSymbol = "<no symbol>";

			DWORD displaceMent = 0;
			IMAGEHLP_LINE line = {0};

			if( SymGetLineFromAddr(hProcess, (DWORD)dwAddress, &displaceMent, &line) )
			{
				siSymbol.lineNum = line.LineNumber;
				strcpy_s(siSymbol.lineName, MAX_PATH, line.FileName);
			}
			else
			{
				siSymbol.lineNum = 0;
				strcpy_s(siSymbol.lineName, MAX_PATH, "<No FileName>");
			}
		}
		__except (EXCEPTION_ACCESS_VIOLATION == GetExceptionCode())
		{
			pszSymbol = "<EX: no symbol>";
			siSymbol.dwOffset = dwAddress - mi.BaseOfImage;
		}

		strncpy_s(siSymbol.szSymbol, _countof(siSymbol.szSymbol), pszSymbol, _TRUNCATE);
		return fRetval;
	}



	namespace detail
	{
		class _AtlThreadContextInfo
		{
		public:
			HANDLE             hThread; //Thread to get context for
			CONTEXT            context; //Where to put context
			IStackDumpHandler* pHandler;
			_AtlThreadContextInfo(IStackDumpHandler* p)
				: hThread(0)
				, pHandler(p)
			{
				memset(&context, 0, sizeof(context));
				context.ContextFlags = CONTEXT_FULL;
			}
			~_AtlThreadContextInfo()
			{
				if (hThread != NULL)
					CloseHandle(hThread);
			}
#pragma warning(push)
#pragma warning(disable:4793)
			HRESULT DumpStack()
			{
				//We can't just call GetThreadContext for the current thread.
				//The docs state this is incorrect and we see problems on x64/Itanium

				//We need to spin up another thread which suspends this thread and gets
				//this thread's context, then unsuspends this thread and signals it.

				HANDLE hThreadPseudo = ::GetCurrentThread();
				HANDLE hProcess = ::GetCurrentProcess();
				if (DuplicateHandle(hProcess, hThreadPseudo, hProcess, &hThread, 0, FALSE, DUPLICATE_SAME_ACCESS) == 0)
					return HRESULT_FROM_WIN32(GetLastError());

				DWORD dwID;
				HANDLE hWorkerThread = CreateThread(NULL, 0, ContextThreadProc, this, 0, &dwID);
				if (hWorkerThread == NULL)
					return HRESULT_FROM_WIN32(GetLastError());
				//Wait for thread to terminate which will indicate it is done.
				//On CE we can't suspend this thread while in the kernel, so loop
				//rather than just waiting.
				while (WaitForSingleObject(hWorkerThread, 0) != WAIT_OBJECT_0)
				{
				}
				CloseHandle(hWorkerThread);
				return S_OK;
			}

			static DWORD WINAPI ContextThreadProc(_In_ LPVOID pv)
			{
				_AtlThreadContextInfo* pThis = 
					reinterpret_cast< _AtlThreadContextInfo* >(pv); 
				return pThis->DoDumpStack();
			}

			DWORD DoDumpStack()
			{
				SuspendThread(hThread);
				GetThreadContext(hThread, &context);

				pHandler->OnBegin();

				std::vector<void *> adwAddress;
				HANDLE hProcess = ::GetCurrentProcess();
				if (SymInitialize(hProcess, NULL, TRUE))
				{
					// force undecorated names to get params
					DWORD dw = SymGetOptions();
					dw &= ~SYMOPT_UNDNAME;
					SymSetOptions(dw);

					STACKFRAME stackFrame;
					memset(&stackFrame, 0, sizeof(stackFrame));
					stackFrame.AddrPC.Mode = AddrModeFlat;
					stackFrame.AddrFrame.Mode = AddrModeFlat;
					stackFrame.AddrStack.Mode = AddrModeFlat;
					stackFrame.AddrReturn.Mode = AddrModeFlat;
					stackFrame.AddrBStore.Mode = AddrModeFlat;

					DWORD dwMachType = IMAGE_FILE_MACHINE_I386;

					// program counter, stack pointer, and frame pointer
					stackFrame.AddrPC.Offset     = context.Eip;
					stackFrame.AddrStack.Offset  = context.Esp;
					stackFrame.AddrFrame.Offset  = context.Ebp;

					adwAddress.reserve(16);

					int nFrame;
					for (nFrame = 0; nFrame < _ATL_MAX_STACK_FRAMES; nFrame++)
					{
						if (!StackWalk(dwMachType, hProcess, hThread,
							&stackFrame, &context, NULL,
							CStackDumper::FunctionTableAccess, CStackDumper::GetModuleBase, NULL))
						{
							break;
						}
						if (stackFrame.AddrPC.Offset != 0)
							adwAddress.push_back((void*)(DWORD_PTR)stackFrame.AddrPC.Offset);
					}
				}

				// dump it out now
				INT_PTR nAddress;
				INT_PTR cAddresses = adwAddress.size();
				for (nAddress = 0; nAddress < cAddresses; nAddress++)
				{
					CStackDumper::_ATL_SYMBOL_INFO info;
					UINT_PTR dwAddress = (UINT_PTR)adwAddress[nAddress];

					LPCSTR szModule = NULL;
					LPCSTR szSymbol = NULL;

					if (CStackDumper::ResolveSymbol(hProcess, dwAddress, info))
					{
						szModule = info.szModule;
						szSymbol = info.szSymbol;
					}

					
					
					static std::string exclude[] = 
					{
						"utility::DumpStack",
						"utility::detail::_AtlThreadContextInfo::DumpStack"
					};

					std::string fileName = info.lineName;

					bool isFind = false;
					for(size_t i = 0 ; i != _countof(exclude); ++ i)
					{
						if( szSymbol == exclude[i] )
						{
							isFind = true;
							break;
						}
					}

					if( !isFind )
						pHandler->OnEntry((void *) dwAddress, info.lineNum, info.lineName, szModule, szSymbol);
				}
				pHandler->OnEnd();
				ResumeThread(hThread);

				return 0;
			} 
		};
	}


	void DumpStack(_In_ IStackDumpHandler *pHandler)
	{
		assert(pHandler);
		detail::_AtlThreadContextInfo info(pHandler);
		info.DumpStack();
	}
}

