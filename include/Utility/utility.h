#ifndef __UTILITY_HPP
#define __UTILITY_HPP

#include <map>
#include <vector>
#include <memory>
#include <string>
#include <cstdio>
#include <cassert>
#include <ctime>

#include <atlbase.h>
#include <atlconv.h>
#include <atlenc.h>

#include <ShellAPI.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <algorithm> 



#include "../Extend STL/UnicodeStl.h"
#include "SmartHandle.hpp"

#include <Winver.h>
#pragma comment(lib, "Version.lib")

#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

#include <rpcdce.h>
#pragma comment(lib, "rpcrt4.lib")

#include <IPHlpApi.h>
#pragma comment(lib, "iphlpapi")

#include <Psapi.h>
#pragma comment(lib, "Psapi")

#include <Userenv.h>
#pragma comment(lib, "Userenv")

namespace utility 
{

	// ¹¤¾ßº¯Êý

	inline const stdex::tString& GetAppPath()
	{
		static struct Helper 
		{
			stdex::tString strPath;
			Helper() 
			{
				TCHAR szFilePath[MAX_PATH] = {0};
				::GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));
				strPath = szFilePath;
				int pos = (int)strPath.find_last_of('\\');
				strPath = strPath.substr(0, pos + 1);
			}
		} helper;

		return helper.strPath;
	}

    inline const stdex::tString& GetSystemDir()
    {
        static struct Helper 
        {
            stdex::tString strPath;
            Helper() 
            {
                TCHAR path[MAX_PATH] = {0};
                GetSystemDirectory(path, MAX_PATH);
                PathRemoveBackslash(path);
                PathAddBackslash(path);
                strPath = path;
            }
        } helper;

        return helper.strPath;
    }
	

	inline stdex::tString GetFileVersion(const stdex::tString& szFile)
	{
		TCHAR version[MAX_PATH] = _T("0.0.0.0");
		DWORD dwSize = GetFileVersionInfoSize(szFile.c_str(), NULL);
		if(dwSize)
		{
			LPTSTR pblock = new TCHAR[dwSize+1];
			GetFileVersionInfo(szFile.c_str(), 0, dwSize, pblock);
			UINT nQuerySize;
			DWORD* pTransTable = NULL;
			VerQueryValue(pblock, _T("\\VarFileInfo\\Translation"), (void **)&pTransTable, &nQuerySize);
			LONG m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
			TCHAR SubBlock[MAX_PATH] = {0};
			_stprintf_s(SubBlock, _T("\\StringFileInfo\\%08lx\\FileVersion"), m_dwLangCharset);	
			LPTSTR lpData;
			VerQueryValue(pblock, SubBlock, (PVOID*)&lpData, &nQuerySize);
			_tcscpy_s(version, lpData);
			delete[] pblock;
		}
		stdex::tString str(version);

		// replace ',' to '.'
		std::replace_if(str.begin(), str.end(), 
			std::bind2nd(std::equal_to<stdex::tString::value_type>(), ','), '.');

		// delete [Space]
		stdex::tString::iterator iter = std::remove_if(str.begin(), str.end(),
			std::bind2nd(std::equal_to<stdex::tString::value_type>(), ' '));

		// remove redundant character
		if( iter != str.end() )
			str.erase(iter, str.end());

		return str;
	}


	// ---------------------------------------------------------------------
	// ×Ö·û´®²Ù×÷

	template<size_t N1, size_t N2>
	inline int Stricmp(const char (&arr1)[N1], const char (&arr2)[N2])
	{
		return ::_strnicmp(arr1, arr2, std::min(N1, N2));
	}

	template<size_t N1, size_t N2>
	inline int Stricmp(const wchar_t (&arr1)[N1], const wchar_t (&arr2)[N2])
	{
		return ::_wcsnicmp(arr1, arr2, std::min(N1, N2));
	}

	inline int Stricmp(const char *str1, const char *str2)
	{
		return ::_stricmp(str1, str2);
	}

	inline int Stricmp(const wchar_t *str1, const wchar_t *str2)
	{
		return ::_wcsicmp(str1, str2);
	}

	template < typename CharT >
	inline int Stricmp(const CharT *str1, const std::basic_string<CharT> &str2)
	{
		return Stricmp(str1, str2.c_str());
	}


	template < size_t N1, size_t N2 >
	inline int Strcmp(const char (&arr1)[N1], const char (&arr2)[N2])
	{
		return ::strncmp(arr1, arr2, std::min(N1, N2));
	}

	template < size_t N1, size_t N2 >
	inline int Strcmp(const wchar_t (&arr1)[N1], const wchar_t (&arr2)[N2])
	{
		return ::wcsncmp(arr1, arr2, std::min(N1, N2));
	}

	inline int Strcmp(const char *str1, const char *str2)
	{
		return ::strcmp(str1, str2);
	}

	inline int Strcmp(const wchar_t *str1, const wchar_t *str2)
	{
		return ::wcscmp(str1, str2);
	}



	namespace detail
	{
		inline void Strcpy(char *dest, size_t cnt, const char *src)
		{
			::strcpy_s(dest, cnt, src);
		}
		inline void Strcpy(wchar_t *dest, size_t cnt, const wchar_t *src)
		{
			::wcscpy_s(dest, cnt, src);
		}
	}

	inline void Strcpy(wchar_t *dest, size_t cnt, const wchar_t *src)
	{
		::wcscpy_s(dest, cnt, src);
	}

	template < size_t _Size >
	inline void Strcpy(char (&des)[_Size], const char *src)
	{
		strncpy(des, src, _Size - 1);
	}

	template < size_t _Size >
	inline void Strcpy(wchar_t (&des)[_Size], const wchar_t *src)
	{
		wcsncpy_s(des, src, _Size - 1);
	}

	template< typename CharT, size_t N >
	inline void Strcpy(CharT (&arr)[N], const std::basic_string<CharT> &str)
	{
		Strcpy(arr, N, str.c_str());
	}
	


	namespace detail
	{
		template < size_t N1, size_t N2 >
		inline void Strcat(char (&des)[N1], const char (&src)[N2])
		{
			assert(::strlen(des) + ::strlen(src) < N1);
			::strcat_s(des, src);
		}

		template < size_t N1, size_t N2 >
		inline void Strcat(wchar_t (&des)[N1], const wchar_t (&src)[N2])
		{
			assert(::wcslen(des) + ::wcslen(src) < N1);
			::wcscat_s(des, src);
		}
	}

	template < typename CharT, size_t N1, size_t N2 >
	inline void Strcat(CharT (&des)[N1], const CharT (&src)[N2])
	{
		detail::Strcat(des, src);
	}


	namespace detail
	{
		inline size_t StrLen(const char *des)
		{
			return ::strlen(des);
		}

		inline size_t Strcat(const wchar_t *des)
		{
			return ::wcslen(des);
		}
	}

	template < typename CharT, size_t N >
	inline size_t StrLen(const CharT (&des)[N])
	{
		return detail::StrLen(des);
	}

	template < typename CharT >
	inline size_t StrLen(const CharT *des)
	{
		return detail::StrLen(des);
	}


	// ClearString
	template<typename CharT, size_t N>
	inline void ClearString(CharT (&str)[N])
	{
		::memset(str, 0, sizeof(CharT) * N);
	}

	// IsEmpty
	inline bool IsEmpty(const char *str)
	{
		return str[0] == 0;
	}
	inline bool IsEmpty(const wchar_t *str)
	{
		return str[0] == 0;
	}


	// LastChar
	template<typename CharT, size_t N>
	inline CharT LastChar(const CharT (&arr)[N])
	{
		return arr[_tcslen(arr) - 1];
	}
	template<typename CharT>
	inline CharT LastChar(const CharT *str)
	{
		return str[_tcslen(CharT) - 1];
	}



	inline stdex::tString CreateGuidString()
	{
		GUID guid;
		UuidCreate(&guid);

#ifdef _UNICODE
		typedef RPC_WSTR RPC_TSTR;
#else
		typedef RPC_CSTR RPC_TSTR;
#endif

		RPC_TSTR rpcStr;
		UuidToString(&guid, &rpcStr);

		stdex::tString str = reinterpret_cast<TCHAR *>(rpcStr);

		RpcStringFree(&rpcStr);


		str.insert(0, _T("{")); 
		str += _T("}");

		return str;
	}




	// URL Encode
	//inline stdex::tString URLEncode(const stdex::tString &url)
	//{
	//	std::string source = CT2A(url.c_str());
	//	char element[4] = {0};
	//	unsigned char chr = 0;
	//	std::string tmp;

	//	for(size_t i=0; i<source.length(); ++i)
	//	{
	//		chr = source[i];
	//		if (chr > 127)		//ºº×Ö
	//		{
	//			sprintf_s(element, "%%%02X", chr);				
	//			tmp += element;
	//			chr = source[++i];
	//			sprintf_s(element, "%%%02X", chr);
	//			tmp += element;
	//		}
	//		else if ( (chr >= '0' && chr <= '9') || 
	//			(chr >= 'a' && chr <= 'z') ||
	//			(chr >= 'A' && chr <= 'Z') )
	//		{
	//			tmp += chr;
	//		}
	//		else
	//		{
	//			sprintf_s( element, "%%%02X", chr); 
	//			tmp += element;
	//		}
	//	}	

	//	stdex::tString res(CA2T(tmp.c_str()));
	//	return res;
	//}
	
    

} 

#endif 