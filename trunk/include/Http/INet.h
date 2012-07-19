/****************************************************** 
*	Title		: WinInet 
*	Description	: 对WinInet API系列封装				
*	Copyright	: Copyright(c)2009 
*	Company		: 
*	author		: 陈煜
*	date		: 2009/10/30
*	version		: 1.0 
*******************************************************/

#ifndef __WIN_INET_H
#define __WIN_INET_H



#include <wininet.h>
#include <shlwapi.h>
#include <cassert>


#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "shlwapi")


#ifndef ATLASSERT
#define ATLASSERT assert
#endif


namespace WinInet
{
	class CInternetHandle;
	class CInternetSession;
	class CInternetConnection;
	class CHttpConnection;
	class CFtpConnection;
	class CInternetFile;
	class CHttpFile;
	class CHttpsFile;
	class CFtpFile;
	class CFtpFindFile;


	class CInternetHandle
	{
	public:
		CInternetHandle() 
			: m_hHandle(NULL)
		{
		}

		CInternetHandle(CInternetHandle& Handle) 
			: m_hHandle(NULL)
		{
			Attach(Handle.Detach());
		}

		explicit CInternetHandle(HINTERNET hInternet) 
			: m_hHandle(hInternet)
		{
		}

		CInternetHandle& operator=(CInternetHandle& Handle)
		{
			if (this!=&Handle)
			{
				Close();
				Attach(Handle.Detach());
			}
			return *this;
		}

		virtual ~CInternetHandle()
		{
			Close();
		}

		void Attach(HINTERNET hHandle)
		{
			ATLASSERT(m_hHandle==NULL);
			m_hHandle=hHandle;
		}

		HINTERNET Detach()
		{
			HINTERNET hHandle=m_hHandle;
			m_hHandle=NULL;
			return hHandle;
		}

		void Close()
		{
			if (m_hHandle)
			{
				InternetCloseHandle(m_hHandle);
				m_hHandle=NULL;
			}
		}

		operator HINTERNET() const
		{
			return m_hHandle;
		}

		BOOL SetOption(DWORD dwOption,LPVOID pBuffer,DWORD dwLength)
		{
			ATLASSERT(m_hHandle);
			return InternetSetOption(m_hHandle,dwOption,pBuffer,dwLength);
		}

		BOOL SetOption(DWORD dwOption,DWORD dwValue)
		{
			return SetOption(dwOption,&dwValue,sizeof(dwValue));
		}

		BOOL SetOption(DWORD dwOption,LPCTSTR szBuffer)
		{
			return SetOption(dwOption,(LPVOID)szBuffer,lstrlen(szBuffer));
		}

		BOOL QueryOption(DWORD dwOption,LPVOID pBuffer,LPDWORD pdwLength) const
		{
			ATLASSERT(m_hHandle);
			return InternetQueryOption(m_hHandle,dwOption,pBuffer,pdwLength);
		}

		BOOL QueryOption(DWORD dwOption,DWORD& dwValue) const
		{
			DWORD dwLength=sizeof(dwValue);
			return QueryOption(dwOption,&dwValue,&dwLength);
		}



		INTERNET_STATUS_CALLBACK SetStatusCallback(INTERNET_STATUS_CALLBACK lpfnInternetCallback)
		{
			ATLASSERT(m_hHandle);
			return InternetSetStatusCallback(m_hHandle,lpfnInternetCallback);
		}

	public:
		HINTERNET m_hHandle;
	};

	class CInternetSession 
		: public CInternetHandle
	{
	public:
		CInternetSession(LPCTSTR szUserAgent=NULL,DWORD dwAccessType=INTERNET_OPEN_TYPE_PRECONFIG,LPCTSTR lpszProxy=NULL,LPCTSTR lpszProxyBypass=NULL,DWORD dwFlags=0)
		{
			TCHAR szFilename[MAX_PATH] = {0};
			if (!szUserAgent)
			{
				GetModuleFileName(NULL,szFilename,sizeof(szFilename)/sizeof(TCHAR));
				PathRemoveExtension(szFilename);
				szUserAgent=PathFindFileName(szFilename);
			}
			Attach(InternetOpen(szUserAgent,dwAccessType,lpszProxy,lpszProxyBypass,dwFlags));
		}
	};

	class CInternetConnection 
		: public CInternetHandle
	{
	public:
		CInternetConnection(HINTERNET hSession,LPCTSTR szServer,INTERNET_PORT nPort,LPCTSTR szUsername,LPCTSTR szPassword,DWORD dwService,DWORD dwFlags)
		{
			ATLASSERT(hSession);
			Attach(InternetConnect(hSession,szServer,nPort,szUsername,szPassword,dwService,dwFlags,(DWORD_PTR)(CInternetHandle*)this));
		}
	};

	class CHttpConnection 
		: public CInternetConnection
	{
	public:
		CHttpConnection(HINTERNET hSession,LPCTSTR szServer,INTERNET_PORT nPort=INTERNET_DEFAULT_HTTP_PORT,LPCTSTR szUsername=NULL,LPCTSTR szPassword=NULL,DWORD dwFlags=0) : CInternetConnection(hSession,szServer,nPort,szUsername,szPassword,INTERNET_SERVICE_HTTP,dwFlags)
		{
		}
	};

	class CFtpConnection 
		: public CInternetConnection
	{
	public:
		CFtpConnection(HINTERNET hSession,LPCTSTR szServer,INTERNET_PORT nPort=INTERNET_DEFAULT_FTP_PORT,LPCTSTR szUsername=NULL,LPCTSTR szPassword=NULL,DWORD dwFlags=INTERNET_FLAG_PASSIVE) : CInternetConnection(hSession,szServer,nPort,szUsername,szPassword,INTERNET_SERVICE_FTP,dwFlags)
		{
		}

		BOOL SetCurrentDirectory(LPCTSTR szDirectory)
		{
			ATLASSERT(m_hHandle);
			return FtpSetCurrentDirectory(m_hHandle,szDirectory);
		}

		BOOL GetCurrentDirectory(LPTSTR szDirectory,LPDWORD pdwLength) const
		{
			ATLASSERT(m_hHandle);
			return FtpGetCurrentDirectory(m_hHandle,szDirectory,pdwLength);
		}


		BOOL CreateDirectory(LPCTSTR szDirectory)
		{
			ATLASSERT(m_hHandle);
			return FtpCreateDirectory(m_hHandle,szDirectory);
		}

		BOOL RemoveDirectory(LPCTSTR szDirectory)
		{
			ATLASSERT(m_hHandle);
			return FtpRemoveDirectory(m_hHandle,szDirectory);
		}

		BOOL Rename(LPCTSTR szExisting,LPCTSTR szNew)
		{
			ATLASSERT(m_hHandle);
			return FtpRenameFile(m_hHandle,szExisting,szNew);
		}

		BOOL Remove(LPCTSTR szFilename)
		{
			ATLASSERT(m_hHandle);
			return FtpDeleteFile(m_hHandle,szFilename);
		}

		BOOL Upload(LPCTSTR szLocal,LPCTSTR szRemote,DWORD dwFlags=FTP_TRANSFER_TYPE_UNKNOWN)
		{
			ATLASSERT(m_hHandle);
			return FtpPutFile(m_hHandle,szLocal,szRemote,dwFlags,(DWORD_PTR)(CInternetHandle*)this);
		}

		BOOL Download(LPCTSTR szRemote,LPCTSTR szLocal,BOOL bFailIfExists=TRUE,DWORD dwAttributes=FILE_ATTRIBUTE_NORMAL,DWORD dwFlags=FTP_TRANSFER_TYPE_UNKNOWN)
		{
			ATLASSERT(m_hHandle);
			return FtpGetFile(m_hHandle,szRemote,szLocal,bFailIfExists,dwAttributes,dwFlags,(DWORD_PTR)(CInternetHandle*)this);
		}

		BOOL Command(LPCTSTR szCommand,DWORD dwFlags=FTP_TRANSFER_TYPE_BINARY)
		{
			ATLASSERT(m_hHandle);
			return FtpCommand(m_hHandle,FALSE,dwFlags,szCommand,(DWORD_PTR)(CInternetHandle*)this,NULL);
		}

		BOOL Command(LPCTSTR szCommand,CInternetHandle& Handle,DWORD dwFlags=FTP_TRANSFER_TYPE_BINARY)
		{
			ATLASSERT(m_hHandle);
			ATLASSERT(Handle.m_hHandle==NULL);
			return FtpCommand(m_hHandle,TRUE,dwFlags,szCommand,(DWORD_PTR)(CInternetHandle*)this,&Handle.m_hHandle);
		}

		BOOL FindFirst(CFtpFindFile& FF,LPCTSTR szSearch=_T("*.*"),DWORD dwFlags=0);
	};

	class CInternetFile 
		: public CInternetHandle
	{
	public:
		class CInfo
		{
		public:
			CInfo(DWORD dwLength=0)
			{
				Reset(dwLength);
			}

			CInfo(CHttpFile& File);
			CInfo(CFtpFile& File);

			void Reset(DWORD dwLength=0)
			{
				m_fLimit=0.0;
				m_dwDataLength=dwLength;
				m_dwSecondsLeft=0;
				m_fDownloadRate=m_fAverageDownloadRate=0.0;
				m_dwRead=0;
				m_dwTimingStart=m_dwTimingLast=GetTickCount();
			}

			void SetRateLimit(double fLimit)
			{
				m_fLimit=fLimit;
			}

			DWORD GetTotalDataLength() const
			{
				return m_dwDataLength;
			}

			DWORD GetTotalRead() const
			{
				return m_dwRead;
			}

			DWORD GetTotalTime() const
			{
				return (m_dwTimingLast-m_dwTimingStart)/1000;
			}

			DWORD GetTimeLeft() const
			{
				return m_dwSecondsLeft;
			}

			double GetDownloadRate() const
			{
				return m_fDownloadRate;
			}

			double GetAverageDownloadRate() const
			{
				return m_fAverageDownloadRate;
			}

		protected:
			DWORD m_dwTimingStart,m_dwTimingLast;
			DWORD m_dwRead;

			DWORD m_dwSecondsLeft;
			double m_fDownloadRate,m_fAverageDownloadRate;
			DWORD m_dwDataLength;

			double m_fLimit;

			friend class CInternetFile;
		};

		BOOL Read(LPVOID pBuffer,DWORD dwLength,DWORD& dwRead)
		{
			ATLASSERT(m_hHandle);
			return InternetReadFile(m_hHandle,pBuffer,dwLength,&dwRead);
		}

		BOOL Read(LPVOID pBuffer,DWORD dwLength,DWORD& dwRead,CInfo& Info)
		{
			ATLASSERT(m_hHandle);
			if (!InternetReadFile(m_hHandle,pBuffer,dwLength,&dwRead))
				return FALSE;
			DWORD dwTimingCurrent=GetTickCount();
			if (Info.m_fLimit>0.0f)
			{
				double fTotalTime=(double)(dwTimingCurrent-Info.m_dwTimingStart);
				double fRate=(double)((double)Info.m_dwRead/fTotalTime);
				if (fRate>Info.m_fLimit)
					Sleep((DWORD)(((fRate*fTotalTime)/Info.m_fLimit)-fTotalTime));
			}
			DWORD dwPreviousRead=Info.m_dwRead;
			Info.m_dwRead+=dwRead;
			DWORD dwTime=dwTimingCurrent-Info.m_dwTimingLast;
			if (dwTime)
			{
				Info.m_fDownloadRate=((double)(Info.m_dwRead)-(double)(dwPreviousRead))/((double)(dwTime));
				Info.m_fAverageDownloadRate=(double)(Info.m_dwRead)/(double)(dwTimingCurrent-Info.m_dwTimingStart);
				Info.m_dwTimingLast=dwTimingCurrent;
				if (Info.m_dwDataLength)
					Info.m_dwSecondsLeft=(DWORD)(((double)dwTimingCurrent-Info.m_dwTimingStart)/Info.m_dwRead*(Info.m_dwDataLength-Info.m_dwRead)/1000);
			}
			return TRUE;
		}

		/*DWORD AsyncRead(LPVOID *buf, size_t bufLen)
		{
			INTERNET_BUFFERS ib = { 0 };
			ib.dwStructSize		= sizeof(INTERNET_BUFFERS);
			ib.lpvBuffer		= buf;
			ib.dwBufferLength	= bufLen;
		
			BOOL ok = ::InternetReadFileEx(m_hHandle, &ib, IRF_ASYNC, 0);
			if( !ok && GetLastError() == ERROR_IO_PENDING )


		}*/

		BOOL Write(LPCVOID pBuffer,DWORD dwLength,DWORD& dwWritten)
		{
			ATLASSERT(m_hHandle);
			return InternetWriteFile(m_hHandle,pBuffer,dwLength,&dwWritten);
		}

		DWORD SetPosition(LONG nDistance,DWORD dwMethod=FILE_BEGIN)
		{
			ATLASSERT(m_hHandle);
			return InternetSetFilePointer(m_hHandle,nDistance,NULL,dwMethod,NULL);
		}

		DWORD GetPosition() const
		{
			ATLASSERT(m_hHandle);
			return InternetSetFilePointer(m_hHandle,0,NULL,FILE_CURRENT,NULL);
		}

		BOOL GetLength(DWORD& dwLength) const
		{
			ATLASSERT(m_hHandle);
			return InternetQueryDataAvailable(m_hHandle,&dwLength,0,0);
		}
	};

	class CHttpFile 
		: public CInternetFile
	{
	public:
		CHttpFile(HINTERNET hConnection,LPCTSTR szVerb,LPCTSTR szObject,LPCTSTR szVersion=NULL,LPCTSTR szReferer=NULL,LPCTSTR* szAcceptTypes=NULL,DWORD dwFlags=INTERNET_FLAG_KEEP_CONNECTION)
		{
			ATLASSERT(hConnection);
			static LPCTSTR szAcceptAll[]={_T("*/*"),NULL};
			Attach(HttpOpenRequest(hConnection,szVerb,szObject,szVersion,szReferer,szAcceptTypes ? szAcceptTypes : szAcceptAll,dwFlags,(DWORD_PTR)(CInternetHandle*)this));
		}

		CHttpFile(HINTERNET hSession,LPCTSTR szURL,LPCTSTR szHeaders=NULL,DWORD dwHeadersLength=(DWORD)-1,DWORD dwFlags=INTERNET_FLAG_KEEP_CONNECTION)
		{
			ATLASSERT(hSession);
			Attach(InternetOpenUrl(hSession,szURL,szHeaders,dwHeadersLength,dwFlags,(DWORD_PTR)(CInternetHandle*)this));
		}

		BOOL SendRequest(LPCTSTR szHeaders=NULL,DWORD dwHeadersLength=(DWORD)-1,LPVOID pOptional=NULL,DWORD dwOptionalLength=0)
		{
			ATLASSERT(m_hHandle);
			return HttpSendRequest(m_hHandle,szHeaders,dwHeadersLength,pOptional,dwOptionalLength);
		}

		BOOL QueryInfo(DWORD dwInfoLevel,LPVOID pBuffer,LPDWORD pdwBufferLength,LPDWORD pdwIndex=NULL) const
		{
			ATLASSERT(m_hHandle);
			return HttpQueryInfo(m_hHandle,dwInfoLevel,pBuffer,pdwBufferLength,pdwIndex);
		}

		BOOL QueryInfo(DWORD dwInfoLevel,DWORD& dwValue,LPDWORD pdwIndex=NULL) const
		{
			DWORD dwLength=sizeof(dwValue);
			return QueryInfo(dwInfoLevel|HTTP_QUERY_FLAG_NUMBER,&dwValue,&dwLength,pdwIndex);
		}

		BOOL QueryInfo(DWORD dwInfoLevel,SYSTEMTIME& dtTime,LPDWORD pdwIndex=NULL) const
		{
			DWORD dwLength=sizeof(dtTime);
			return QueryInfo(dwInfoLevel|HTTP_QUERY_FLAG_SYSTEMTIME,&dtTime,&dwLength,pdwIndex);
		}

	};

	class CHttpsFile 
		: public CHttpFile
	{
	public:
		CHttpsFile(HINTERNET hConnection,LPCTSTR szVerb,LPCTSTR szObject,LPCTSTR szVersion=NULL,LPCTSTR szReferer=NULL,LPCTSTR* szAcceptTypes=NULL,DWORD dwFlags=INTERNET_FLAG_KEEP_CONNECTION) : CHttpFile(hConnection,szVerb,szObject,szVersion,szReferer,szAcceptTypes,dwFlags|INTERNET_FLAG_SECURE)
		{
		}

		CHttpsFile(HINTERNET hSession,LPCTSTR szURL,LPCTSTR szHeaders=NULL,DWORD dwHeadersLength=(DWORD)-1,DWORD dwFlags=INTERNET_FLAG_KEEP_CONNECTION) : CHttpFile(hSession,szURL,szHeaders,dwHeadersLength,dwFlags|INTERNET_FLAG_SECURE)
		{
		}
	};

	class CFtpFile 
		: public CInternetFile
	{
	public:
		CFtpFile(HINTERNET hConnection,LPCTSTR szFilename,DWORD dwAccess,DWORD dwFlags=FTP_TRANSFER_TYPE_UNKNOWN)
		{
			ATLASSERT(hConnection);
			Attach(FtpOpenFile(hConnection,szFilename,dwAccess,dwFlags,(DWORD_PTR)(CInternetHandle*)this));
		}

		CFtpFile(HINTERNET hSession,LPCTSTR szURL,LPCTSTR szHeaders=NULL,DWORD dwHeadersLength=(DWORD)-1,DWORD dwFlags=INTERNET_FLAG_PASSIVE)
		{
			ATLASSERT(hSession);
			Attach(InternetOpenUrl(hSession,szURL,szHeaders,dwHeadersLength,dwFlags,(DWORD_PTR)(CInternetHandle*)this));
		}

		ULONGLONG GetSize() const
		{
			ATLASSERT(m_hHandle);
			ULARGE_INTEGER Size;
			Size.LowPart=FtpGetFileSize(m_hHandle,&Size.HighPart);
			return Size.QuadPart;
		}
	};

	template<class T>
	class CInternetFindFile 
		: public CInternetHandle
	{
	public:
		CInternetFindFile()
		{
			ZeroMemory(&m_fd,sizeof(T));
		}

		BOOL FindNext()
		{
			ATLASSERT(m_hHandle);
			return InternetFindNextFile(m_hHandle,&m_fd);
		}

		operator T() const
		{
			return m_fd;
		}

	protected:
		T m_fd;
	};

	class CFtpFindFile 
		: public CInternetFindFile<WIN32_FIND_DATA>
	{
	public:
		ULONGLONG GetFileSize() const
		{
			ATLASSERT(m_hHandle);
			ULARGE_INTEGER nSize;
			nSize.LowPart=m_fd.nFileSizeLow;
			nSize.HighPart=m_fd.nFileSizeHigh;
			return nSize.QuadPart;
		}

		BOOL GetFileName(LPTSTR lpstrFileName, int cchLength) const
		{
			ATLASSERT(m_hHandle);
			if(lstrlen(m_fd.cFileName)>=cchLength)
				return FALSE;
			return (lstrcpy(lpstrFileName, m_fd.cFileName)!=NULL);
		}

		BOOL GetFileTitle(LPTSTR lpstrFileTitle, int cchLength) const
		{
			ATLASSERT(m_hHandle);
			TCHAR szBuff[MAX_PATH]={0};
			if(!GetFileName(szBuff,MAX_PATH))
				return FALSE;
			TCHAR szNameBuff[_MAX_FNAME]={0};
			_tsplitpath_s(szBuff, NULL, 0, NULL, 0, szNameBuff, _MAX_FNAME, NULL, 0);

			if(lstrlen(szNameBuff)>=cchLength)
				return FALSE;
			return (lstrcpy(lpstrFileTitle, szNameBuff)!=NULL);
		}


		void GetLastWriteTime(FILETIME* pTimeStamp) const
		{
			ATLASSERT(m_hHandle);
			ATLASSERT(pTimeStamp);
			*pTimeStamp = m_fd.ftLastWriteTime;
		}

		void GetLastAccessTime(FILETIME* pTimeStamp) const
		{
			ATLASSERT(m_hHandle);
			ATLASSERT(pTimeStamp);
			*pTimeStamp = m_fd.ftLastAccessTime;
		}

		BOOL GetCreationTime(FILETIME* pTimeStamp) const
		{
			ATLASSERT(m_hHandle);
			ATLASSERT(pTimeStamp);
			*pTimeStamp = m_fd.ftCreationTime;

			return TRUE;
		}

		BOOL MatchesMask(DWORD dwMask) const
		{
			ATLASSERT(m_hHandle);
			return (m_fd.dwFileAttributes & dwMask);
		}

		BOOL IsDots() const
		{
			ATLASSERT(m_hHandle);
			return (IsDirectory() && (m_fd.cFileName[0]=='.' && (m_fd.cFileName[1]=='\0' || (m_fd.cFileName[1]=='.' && m_fd.cFileName[2]=='\0'))));
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

	protected:
		friend class CFtpConnection;
	};

	inline BOOL CFtpConnection::FindFirst(CFtpFindFile& FF,LPCTSTR szSearch,DWORD dwFlags)
	{
		ATLASSERT(m_hHandle);
		ATLASSERT(FF.m_hHandle==NULL);
		return (FF.m_hHandle=FtpFindFirstFile(m_hHandle,szSearch,&FF.m_fd,dwFlags,(DWORD_PTR)(CInternetHandle*)this))!=NULL;
	}

	inline CInternetFile::CInfo::CInfo(CHttpFile& File)
	{
		DWORD dwLength;
		File.QueryInfo(HTTP_QUERY_CONTENT_LENGTH,dwLength);
		Reset(dwLength);
	}

	inline CInternetFile::CInfo::CInfo(CFtpFile& File)
	{
		Reset((DWORD)File.GetSize());
	}
}

#endif	// __WIN_INET_H