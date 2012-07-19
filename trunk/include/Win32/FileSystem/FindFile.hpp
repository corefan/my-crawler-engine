#ifndef __FIND_FILE_HPP
#define __FIND_FILE_HPP



#include <cassert>
#include "../../Extend STL/UnicodeStl.h"



namespace filesystem
{
	// --------------------------------------------
	// class CFindFile

	// ¼ìË÷ÎÄ¼þ
	class CFindFile
	{
	public:
		// Data members
		WIN32_FIND_DATA m_fd;
		TCHAR m_lpszRoot[MAX_PATH];
		TCHAR m_chDirSeparator;
		HANDLE m_hFind;
		BOOL m_bFound;

		// Constructor/destructor
		CFindFile() : m_hFind(NULL), m_chDirSeparator(_T('\\')), m_bFound(FALSE)
		{ }

		~CFindFile()
		{
			Close();
		}

		// Attributes
		ULONGLONG GetFileSize() const
		{
			assert(m_hFind != NULL);

			ULARGE_INTEGER nFileSize = { 0 };

			if(m_bFound)
			{
				nFileSize.LowPart = m_fd.nFileSizeLow;
				nFileSize.HighPart = m_fd.nFileSizeHigh;
			}
			else
			{
				nFileSize.QuadPart = 0;
			}

			return nFileSize.QuadPart;
		}

		BOOL GetFileName(LPTSTR lpstrFileName, int cchLength) const
		{
			assert(m_hFind != NULL);
			if(lstrlen(m_fd.cFileName) >= cchLength)
				return FALSE;

			if(m_bFound)
				_tcscpy_s(lpstrFileName, cchLength, m_fd.cFileName);

			return m_bFound;
		}

		BOOL GetFilePath(LPTSTR lpstrFilePath, int cchLength) const
		{
			assert(m_hFind != NULL);

			int nLen = lstrlen(m_lpszRoot);

			assert(nLen > 0);
			if(nLen == 0)
				return FALSE;

			bool bAddSep = (m_lpszRoot[nLen - 1] != _T('\\') && m_lpszRoot[nLen - 1] !=_T('/'));

			if((lstrlen(m_lpszRoot) + (bAddSep ?  1 : 0)) >= cchLength)
				return FALSE;

			_tcscpy_s(lpstrFilePath, cchLength, m_lpszRoot);

			if(bAddSep)
			{
				TCHAR szSeparator[2] = { m_chDirSeparator, 0 };
				_tcscat_s(lpstrFilePath, cchLength, szSeparator);
			}

			_tcscat_s(lpstrFilePath, cchLength, m_fd.cFileName);

			return TRUE;
		}


		BOOL GetFileTitle(LPTSTR lpstrFileTitle, int cchLength) const
		{
			assert(m_hFind != NULL);

			TCHAR szBuff[MAX_PATH] = { 0 };
			if(!GetFileName(szBuff, MAX_PATH))
				return FALSE;

			if(lstrlen(szBuff) >= cchLength || cchLength < 1)
				return FALSE;

			// find the last dot
			LPTSTR pstrDot  = (LPTSTR)_cstrrchr(szBuff, _T('.'));
			if(pstrDot != NULL)
				*pstrDot = 0;

			_tcscpy_s(lpstrFileTitle, cchLength, szBuff);

			return TRUE;
		}

		BOOL GetFileURL(LPTSTR lpstrFileURL, int cchLength) const
		{
			assert(m_hFind != NULL);

			TCHAR szBuff[MAX_PATH] = { 0 };
			if(!GetFilePath(szBuff, MAX_PATH))
				return FALSE;
			LPCTSTR lpstrFileURLPrefix = _T("file://");
			if(lstrlen(szBuff) + lstrlen(lpstrFileURLPrefix) >= cchLength)
				return FALSE;
			_tcscpy_s(lpstrFileURL, cchLength, lpstrFileURLPrefix);
			_tcscat_s(lpstrFileURL, cchLength, szBuff);

			return TRUE;
		}

		BOOL GetRoot(LPTSTR lpstrRoot, int cchLength) const
		{
			assert(m_hFind != NULL);
			if(lstrlen(m_lpszRoot) >= cchLength)
				return FALSE;

			_tcscpy_s(lpstrRoot, cchLength, m_lpszRoot);

			return TRUE;
		}


		stdex::tString GetFileName() const
		{
			assert(m_hFind != NULL);

			stdex::tString ret;

			if(m_bFound)
				ret = m_fd.cFileName;
			return ret;
		}

		stdex::tString GetFilePath() const
		{
			assert(m_hFind != NULL);

			stdex::tString strResult = m_lpszRoot;
			int nLen = strResult.length();

			assert(nLen > 0);
			if(nLen == 0)
				return strResult;

			if((strResult[nLen - 1] != _T('\\')) && (strResult[nLen - 1] != _T('/')))

				strResult += m_chDirSeparator;
			strResult += GetFileName();
			return strResult;
		}


		stdex::tString GetFileTitle() const
		{
			assert(m_hFind != NULL);

			TCHAR fileTitle[MAX_PATH] = {0};
			GetFileTitle(fileTitle, MAX_PATH);

			return fileTitle;
		}


		stdex::tString GetFileURL() const
		{
			assert(m_hFind != NULL);

			stdex::tString strResult(_T("file://"));
			strResult += GetFilePath();
			return strResult;
		}

		stdex::tString GetRoot() const
		{
			assert(m_hFind != NULL);

			stdex::tString str = m_lpszRoot;
			return str;
		}

		BOOL GetLastWriteTime(FILETIME* pTimeStamp) const
		{
			assert(m_hFind != NULL);
			assert(pTimeStamp != NULL);

			if(m_bFound && pTimeStamp != NULL)
			{
				*pTimeStamp = m_fd.ftLastWriteTime;
				return TRUE;
			}

			return FALSE;
		}

		BOOL GetLastAccessTime(FILETIME* pTimeStamp) const
		{
			assert(m_hFind != NULL);
			assert(pTimeStamp != NULL);

			if(m_bFound && pTimeStamp != NULL)
			{
				*pTimeStamp = m_fd.ftLastAccessTime;
				return TRUE;
			}

			return FALSE;
		}

		BOOL GetCreationTime(FILETIME* pTimeStamp) const
		{
			assert(m_hFind != NULL);

			if(m_bFound && pTimeStamp != NULL)
			{
				*pTimeStamp = m_fd.ftCreationTime;
				return TRUE;
			}

			return FALSE;
		}

		BOOL MatchesMask(DWORD dwMask) const
		{
			assert(m_hFind != NULL);

			if(m_bFound)
				return ((m_fd.dwFileAttributes & dwMask) != 0);

			return FALSE;
		}

		BOOL IsDots() const
		{
			assert(m_hFind != NULL);

			// return TRUE if the file name is "." or ".." and
			// the file is a directory

			BOOL bResult = FALSE;
			if(m_bFound && IsDirectory())
			{
				if(m_fd.cFileName[0] == _T('.') && (m_fd.cFileName[1] == _T('\0') || (m_fd.cFileName[1] == _T('.') && m_fd.cFileName[2] == _T('\0'))))
					bResult = TRUE;
			}

			return bResult;
		}

		BOOL IsReadOnly() const
		{
			return MatchesMask(FILE_ATTRIBUTE_READONLY);
		}

		BOOL IsDirectory() const
		{
			return MatchesMask(FILE_ATTRIBUTE_DIRECTORY);
		}

		BOOL IsCompressed() const
		{
			return MatchesMask(FILE_ATTRIBUTE_COMPRESSED);
		}

		BOOL IsSystem() const
		{
			return MatchesMask(FILE_ATTRIBUTE_SYSTEM);
		}

		BOOL IsHidden() const
		{
			return MatchesMask(FILE_ATTRIBUTE_HIDDEN);
		}

		BOOL IsTemporary() const
		{
			return MatchesMask(FILE_ATTRIBUTE_TEMPORARY);
		}

		BOOL IsNormal() const
		{
			return MatchesMask(FILE_ATTRIBUTE_NORMAL);
		}

		BOOL IsArchived() const
		{
			return MatchesMask(FILE_ATTRIBUTE_ARCHIVE);
		}

		// Operations
		BOOL FindFile(LPCTSTR pstrName = NULL)
		{
			Close();

			if(pstrName == NULL)
			{
				pstrName = _T("*.*");
			}
			else if(lstrlen(pstrName) >= MAX_PATH)
			{
				assert(FALSE);
				return FALSE;
			}

			_tcscpy_s(m_fd.cFileName, _countof(m_fd.cFileName), pstrName);

			m_hFind = ::FindFirstFile(pstrName, &m_fd);

			if(m_hFind == INVALID_HANDLE_VALUE)
				return FALSE;

			bool bFullPath = (::GetFullPathName(pstrName, MAX_PATH, m_lpszRoot, NULL) != 0);


			// passed name isn't a valid path but was found by the API
			assert(bFullPath);
			if(!bFullPath)
			{
				Close();
				::SetLastError(ERROR_INVALID_NAME);
				return FALSE;
			}
			else
			{
				// find the last forward or backward whack
				LPTSTR pstrBack  = (LPTSTR)_cstrrchr(m_lpszRoot, _T('\\'));
				LPTSTR pstrFront = (LPTSTR)_cstrrchr(m_lpszRoot, _T('/'));

				if(pstrFront != NULL || pstrBack != NULL)
				{
					if(pstrFront == NULL)
						pstrFront = m_lpszRoot;
					if(pstrBack == NULL)
						pstrBack = m_lpszRoot;

					// from the start to the last whack is the root

					if(pstrFront >= pstrBack)
						*pstrFront = _T('\0');
					else
						*pstrBack = _T('\0');
				}
			}

			m_bFound = TRUE;

			return TRUE;
		}

		BOOL FindNextFile()
		{
			assert(m_hFind != NULL);

			if(m_hFind == NULL)
				return FALSE;

			if(!m_bFound)
				return FALSE;

			m_bFound = ::FindNextFile(m_hFind, &m_fd);

			return m_bFound;
		}

		void Close()
		{
			m_bFound = FALSE;

			if(m_hFind != NULL && m_hFind != INVALID_HANDLE_VALUE)
			{
				::FindClose(m_hFind);
				m_hFind = NULL;
			}
		}

		// Helper
		static const TCHAR* _cstrrchr(const TCHAR* p, TCHAR ch)
		{
			return _tcsrchr(p, ch);
		}

	};
}







#endif