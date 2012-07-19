#pragma once

#include "../../Extend STL/UnicodeStl.h"
#include "../../Extend STL/StringAlgorithm.h"

namespace i8desk
{
    template<class TYPE>
    inline TYPE GetIniConfig(LPCTSTR szSection, LPCTSTR szKeyName, TYPE DefValue, LPCTSTR szFileName)
    {
		TCHAR atValue[MAX_PATH] = {0};
		::GetPrivateProfileString(szSection, szKeyName, stdex::ToString<stdex::tString, TYPE>(DefValue).c_str(), atValue, MAX_PATH, szFileName);
        return stdex::ToNumber<TYPE, TCHAR>(atValue);
    }
    inline stdex::tString GetIniConfig(LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR szDefValue, LPCTSTR szFileName)
    {
        TCHAR atValue[MAX_PATH] = {0};
        ::GetPrivateProfileString(szSection, szKeyName, szDefValue, atValue, MAX_PATH, szFileName);
        return stdex::tString(atValue);
    }

    inline bool SetIniConfig(LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR szValue, LPCTSTR szFileName)
    {
		return ::WritePrivateProfileString(szSection, szKeyName, szValue, szFileName) == TRUE;
    }

    template<class TYPE>
    inline bool SetIniConfig(LPCTSTR szSection, LPCTSTR szKeyName, TYPE Value, LPCTSTR szFileName)
    {
		return ::WritePrivateProfileString(szSection, szKeyName, stdex::ToString<stdex::tString, TYPE>(Value).c_str(), szFileName) == TRUE;
    }
}
