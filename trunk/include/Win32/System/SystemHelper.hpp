#ifndef __SYSTEM_HELPER_HPP
#define __SYSTEM_HELPER_HPP


#include <vector>

#include <Psapi.h>
#include <Iphlpapi.h>
#include <atlbase.h>
#include <atlconv.h>

#include <Tlhelp32.h>
#include <Userenv.h>
#include <Sddl.h>
#include <minmax.h>
#include <atlimage.h>

#include "../../Utility/SmartHandle.hpp"
#include "../../Unicode/string.hpp"
#include "../../Extend STL/UnicodeStl.h"
#include "../../Extend STL/StringAlgorithm.h"

#pragma comment(lib, "Psapi")
#pragma comment(lib, "iphlpapi")
#pragma comment(lib, "Userenv.lib")


namespace win32
{
	namespace system
	{
		class CEvenyOneSD 
		{
		public:
			PVOID Build(PSECURITY_DESCRIPTOR pSD) 
			{
				PSID   psidEveryone = NULL;
				PACL   pDACL   = NULL;
				BOOL   bResult = FALSE;

				__try 
				{
					SID_IDENTIFIER_AUTHORITY siaWorld = SECURITY_WORLD_SID_AUTHORITY;
					//SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;

					if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
						__leave;

					if (!AllocateAndInitializeSid(&siaWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &psidEveryone)) 
						__leave;

					DWORD dwAclLength = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + GetLengthSid(psidEveryone);

					pDACL = (PACL) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAclLength);
					if (!pDACL) 
						__leave;

					if (!InitializeAcl(pDACL, dwAclLength, ACL_REVISION)) 
						__leave;

					if (!AddAccessAllowedAce(pDACL, ACL_REVISION, GENERIC_ALL, psidEveryone)) 
						__leave;

					if (!SetSecurityDescriptorDacl(pSD, TRUE, pDACL, FALSE)) 
						__leave;
					bResult = TRUE;
				} 
				__finally 
				{
					if (psidEveryone) 
						FreeSid(psidEveryone);
				}

				if (bResult == FALSE) 
				{
					if (pDACL) ::HeapFree(::GetProcessHeap(), 0, pDACL);
					pDACL = NULL;
				}

				return (PVOID) pDACL;
			}
			CEvenyOneSD()
			{
				ptr=NULL;
				sa.nLength = sizeof(sa);
				sa.lpSecurityDescriptor = &sd;
				sa.bInheritHandle = FALSE;
				ptr = Build(&sd);
			}
			~CEvenyOneSD()
			{
				if(ptr)
				{
					::HeapFree(::GetProcessHeap(), 0, ptr);
				}
			}
			SECURITY_ATTRIBUTES* GetSA()
			{
				return (ptr != NULL) ? &sa : NULL;
			}
		protected:
			PVOID  ptr;
			SECURITY_ATTRIBUTES sa;
			SECURITY_DESCRIPTOR sd;
		};


		template< typename CharT >
		inline std::basic_string<CharT> GetOsName(const std::basic_string<CharT> &osVersion)
		{
			assert(osVersion.empty());

			int iMajorVersion, iMinorVersion, iBuildNumber;
			int iCount = _stscanf_s(osVersion.c_str(), _T("%d.%d.%d"), &iMajorVersion, &iMinorVersion, &iBuildNumber);
			if (iCount != 3)
				return _T("");
	
			switch (iMajorVersion)
			{
			case 4:
				switch (iMinorVersion)
				{
				case 0:
					return _T("Windows95");
				case 1:
					return _T("Windows98");
				default:
					return _T("");
				}
				break;
			case 5:
				switch (iMinorVersion)
				{
				case 0:
					return _T("Windows2000");
				case 1:
					return _T("WindowsXP");
				case 2:
					return _T("Windows2003");
				default:
					return _T("");
				}
				break;
			case 6:
				switch (iMinorVersion)
				{
				case 0:
					return _T("WindowsVista");
				case 1:
					return _T("Windows7");
				default:
					return _T("");
				}
				break;
			default:
				return _T("");
			}
		}


		struct DiskInfo
		{
			TCHAR partion;							// 分区
			UINT type;								// 类型
			ULONGLONG capacity;						// 总大小
			ULONGLONG usedSize;						// 使用大小
			ULONGLONG freeSize;						// 空闲大小
		};
		inline void GetDiskInfo(std::vector<DiskInfo> &disksInfo)
		{
			TCHAR driver[MAX_PATH] = {0};
			DWORD len = ::GetLogicalDriveStrings(MAX_PATH, driver);
	
			TCHAR *p = driver;
			do 
			{
				DiskInfo disk;
				disk.partion = p[0];
				disk.type = ::GetDriveType(p);

				ULARGE_INTEGER total = {0};
				ULARGE_INTEGER free = {0};
	
				::GetDiskFreeSpaceEx(p, &free, &total, 0);
				disk.capacity = total.QuadPart;
				disk.freeSize = free.QuadPart;
				disk.usedSize = total.QuadPart - free.QuadPart;
				
				disksInfo.push_back(disk);

				while(*p++);	// Next String
			}while(*p);
			
		}
		
		

		inline bool KillProcess(LPCTSTR lpszSerive)
		{
			DWORD aProcesses[1024] = {0}, cbNeeded = 0, cProcesses = 0;
			TCHAR path[MAX_PATH] = {0};
			if ( !::EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
				return	false;
			cProcesses = cbNeeded / sizeof(DWORD);
			for (DWORD i = 0; i < cProcesses; i++ )
			{
				HANDLE hProcess = ::OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, aProcesses[i]);
				if (hProcess != NULL )
				{
					if(::GetModuleBaseName(hProcess, NULL, path, MAX_PATH) == 0)
					{
						CloseHandle( hProcess );
						continue;
					}
					if (lstrcmpi(path, lpszSerive) == 0)
					{
						TerminateProcess(hProcess, 0);
						CloseHandle( hProcess );
						return true;
					}
				}
			}
			return false;
		}

		inline HANDLE GetTokenFromProcName(LPCTSTR lpProcName = TEXT("explorer.exe"))
		{
			PROCESSENTRY32 pe32 = {0};
			utility::CAutoToolhelp hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
			if ( !hProcessSnap.IsValid() )
				return NULL; 
			pe32.dwSize = sizeof(PROCESSENTRY32);
			if( ::Process32First(hProcessSnap, &pe32) )
			{ 
				do
				{
					if(lstrcmpi(pe32.szExeFile, lpProcName) == 0)
					{
						utility::CAutoHandle hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,pe32.th32ProcessID);
						HANDLE hToken = NULL;
						::OpenProcessToken(hProcess,TOKEN_ALL_ACCESS, &hToken);

						return hToken;
					}
				} 
				while(::Process32Next(hProcessSnap, &pe32)); 
			}

			return NULL;
		}

		inline bool start_process(LPCTSTR lpszPath)
		{
			utility::CAutoToken hToken = GetTokenFromProcName();
			LPVOID lpEnv = NULL;
			if( !hToken.IsValid() )
				::CreateEnvironmentBlock(&lpEnv, hToken, FALSE);

			STARTUPINFO si = {0};
			PROCESS_INFORMATION pi = {0};
			si.cb = sizeof(STARTUPINFO);
			si.lpDesktop = TEXT("winsta0\\default");	

			TCHAR path[MAX_PATH] = {0};
			_tcscpy_s(path, lpszPath);

			BOOL ret = ::CreateProcess(NULL, path, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, lpEnv, NULL, &si, &pi);
			if( ret )
			{
				::CloseHandle(pi.hThread);
				::CloseHandle(pi.hProcess);
				return true;
			}

			return false;
		}


		inline stdex::tString GetCurrentUserForSID() 
		{
			HANDLE hToken = INVALID_HANDLE_VALUE;
			if( !::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &hToken ) ) 
				return _T("");

			DWORD dwSize = 0;
			if( !::GetTokenInformation(hToken, TokenUser, NULL, dwSize, &dwSize) ) 
			{
				DWORD dwResult = GetLastError();
				if( dwResult != ERROR_INSUFFICIENT_BUFFER )
					return _T("");
			}

			std::vector<char> buf(dwSize + 1);
			PTOKEN_USER pUserInfo = reinterpret_cast<PTOKEN_USER>(&buf[0]);

			if( !::GetTokenInformation(hToken, TokenUser, pUserInfo, dwSize, &dwSize ) ) 
				return _T("");

			LPTSTR StringSid = 0;
			::ConvertSidToStringSid(pUserInfo->User.Sid, &StringSid);
			
			stdex::tString SID = StringSid;
			return SID;
		}

        inline SYSTEMTIME TimeToSystemTime(time_t time)
        { 
            tm timeInfo;
            if (localtime_s(&timeInfo, &time) != 0)
            {
                assert(0 && "时间转化失败");
            }
            SYSTEMTIME sytemTime = { 1900 + timeInfo.tm_year, 1 + timeInfo.tm_mon, timeInfo.tm_wday, timeInfo.tm_mday, timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec, 0 };
            return sytemTime;
        }

        inline time_t SystemTimeToTime(const SYSTEMTIME& sytemTime)
        {
            tm timeInfo = { sytemTime.wSecond, sytemTime.wMinute, sytemTime.wHour, sytemTime.wDay, sytemTime.wMonth - 1, sytemTime.wYear - 1900, sytemTime.wDayOfWeek, 0, 0 };
            return mktime(&timeInfo);
        }

        inline time_t FileTimeToTime(const FILETIME& fileTime)
        {
            SYSTEMTIME systemTime = {0};
            BOOL result = ::FileTimeToSystemTime(&fileTime, &systemTime);
            assert(result && "FileTimeToTime失败");
            return SystemTimeToTime(systemTime);
        }

        inline FILETIME TimeToFileTime(time_t time)
        {
            SYSTEMTIME systemTime = TimeToSystemTime(time);
            FILETIME fileTime = {0};
            BOOL result = ::SystemTimeToFileTime(&systemTime, &fileTime);
            assert(result && "FileTimeToTime失败");
            return fileTime;
        }

        inline bool GetFixedTime(time_t& dateTime, time_t date, time_t time)
        {
            tm dateInfo;
            if (localtime_s(&dateInfo, &date) != 0)
            {
                return false;
            }
            tm timeInfo;
            if (localtime_s(&timeInfo, &time) != 0)
            {
                return false;
            }
            tm dateTimeInfo = dateInfo;
            dateTimeInfo.tm_hour = timeInfo.tm_hour;
            dateTimeInfo.tm_min = timeInfo.tm_min;
            dateTimeInfo.tm_sec = timeInfo.tm_sec;
            dateTime = mktime(&dateTimeInfo);
            return true;
        }

        class ReplaceWallpaper
        {
        public:
            enum DisplayMode
            {
                E_WDM_CENTER = 0, //居中
                E_WDM_TILE,       //平铺
                E_WDM_STRETCH,    //拉伸
                E_WDM_MAX
            };
        public:
            ReplaceWallpaper()
            {
            }
            bool operator()(const stdex::tString& picturePath, DisplayMode displayMode)
            {
                TCHAR path[MAX_PATH] = {0};
                GetWindowsDirectory(path, _countof(path));
                PathAddBackslash(path);
                _tcscat_s(path, _T("Wallpaper.bmp"));
                {
                    CImage image;
                    image.Load(picturePath.c_str());
                    image.Save(path);
                }
                CRegKey regKey;
                regKey.Open(HKEY_CURRENT_USER, _T("Control Panel\\Desktop"));
                switch (displayMode)
                {
                case E_WDM_CENTER:
                    regKey.SetStringValue(_T("TileWallpaper"), _T("0"));
                    regKey.SetStringValue(_T("WallpaperStyle"), _T("0"));
                    break;
                case E_WDM_TILE:
                    regKey.SetStringValue(_T("TileWallpaper"), _T("1"));
                    regKey.SetStringValue(_T("WallpaperStyle"), _T("0"));
                    break;
                case E_WDM_STRETCH:
                    regKey.SetStringValue(_T("TileWallpaper"), _T("0"));
                    regKey.SetStringValue(_T("WallpaperStyle"), _T("2"));
                    break;
                default:
                    break;
                }
                regKey.SetStringValue(_T("Wallpaper"), path);
                if (!SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, path, 0))
                {
                    return false;
                }
                return true;
            }
        };

        template<class CharT> inline void FormatSize(unsigned long long size, std::basic_string<CharT> &text)
		{
            // 按Byte\KB\MB\GB来划分
            struct SizeInfo
            {
                unsigned long long size;
                const CharT* name;
            };
            static const SizeInfo SIZE_INFO[] =
            {
                1024 * 1024 * 1024, unicode::aux::select<CharT>("GB", L"GB"),
                1024 * 1024,        unicode::aux::select<CharT>("MB", L"MB"),
                1024,               unicode::aux::select<CharT>("KB", L"KB"),
                1,                  unicode::aux::select<CharT>("B",  L"B"),
            };
            for (int i = 0; i < _countof(SIZE_INFO); ++i)
            {
                if (size >= SIZE_INFO[i].size)
                {
                    stdex::ToString(DOUBLE(size) / SIZE_INFO[i].size, text, 2);
                    text += SIZE_INFO[i].name;
                    return;
                }
            }
        }

        template<class CharT> inline std::basic_string<CharT> FormatSize(unsigned long long size)
        {
            std::basic_string<CharT> text;
            FormatSize(size, text);
            return text;
        }

	}
	
}



#endif