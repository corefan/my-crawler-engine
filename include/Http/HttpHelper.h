#ifndef __HTTP_HELPER_HPP
#define __HTTP_HELPER_HPP

#include <map>
#include <functional>
#include <sstream>
#include "../Zip/XUnzip.h"
#include "../MemoryPool/SGIMemoryPool.hpp"


#ifdef __USE_WININET

#include "INet.h"

#else

#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

#include "../Utility/SmartHandle.hpp"

template < typename T, typename P >
struct CCloseWinHttp
{
	void operator()(T handle)
	{
		::WinHttpCloseHandle(handle);
	}
};

namespace utility
{
	typedef CSmartHandle<HINTERNET, CCloseWinHttp>							CAutoWinHttp;
}

#endif

#include "../Zip/XUnzip.h"

#include "../Utility/SmartHandle.hpp"


//#include "INet.h"


#ifdef min
#undef min
#endif


namespace http
{
	typedef std::tr1::shared_ptr<char>	AutoBuffer;
	typedef std::pair<AutoBuffer, size_t>	Buffer;


	namespace detail
	{
		struct AllocatorTraits
		{
			typedef async::memory::SGIMemoryPool<true, 128 * 1024>	MemoryPool;

			static MemoryPool &GetPool()
			{
				static MemoryPool pool;

				return pool;
			}
		};


		inline void *BufferAlloc(size_t sz)
		{
			void *tmp = AllocatorTraits::GetPool().Allocate(sz);
			return tmp;
		}
		inline void BufferDealloc(void *p, size_t sz)
		{
			AllocatorTraits::GetPool().Deallocate(p, sz);
		}

		
		template < size_t N >
		struct HTTPError;

#ifdef __USE_WININET
		template <>
		struct HTTPError<0>
		{
			typedef std::map<int, std::string> MapErrorInfo;
			MapErrorInfo ErrorInfo;

			HTTPError()
			{

				ErrorInfo[ERROR_INTERNET_OUT_OF_HANDLES]		= "No more handles could be generated at this time";
				ErrorInfo[ERROR_INTERNET_TIMEOUT]				= "The request has timed out.";
				ErrorInfo[ERROR_INTERNET_EXTENDED_ERROR]		= "An extended error was returned from the server. This istypically a string or buffer containing a verbose errormessage. Call InternetGetLastResponseInfo to retrieve theerror text.";
				ErrorInfo[ERROR_INTERNET_INTERNAL_ERROR]		= "An internal error has occurred.";
				ErrorInfo[ERROR_INTERNET_INVALID_URL]			= "The URL is invalid.";
				ErrorInfo[ERROR_INTERNET_UNRECOGNIZED_SCHEME]	= "The URL scheme could not be recognized or is not supported.";
				ErrorInfo[ERROR_INTERNET_NAME_NOT_RESOLVED]		= "The server name could not be resolved.";
				ErrorInfo[ERROR_INTERNET_PROTOCOL_NOT_FOUND]	= "The requested protocol could not be located.";
				ErrorInfo[ERROR_INTERNET_INVALID_OPTION]		= "A request to InternetQueryOption or InternetSetOptionspecified an invalid option value.";
				ErrorInfo[ERROR_INTERNET_BAD_OPTION_LENGTH]		= "The length of an option supplied to InternetQueryOption orInternetSetOption is incorrect for the type of optionspecified.";
				ErrorInfo[ERROR_INTERNET_OPTION_NOT_SETTABLE]	= "The request option cannot be set, only queried.";
				ErrorInfo[ERROR_INTERNET_SHUTDOWN]				= "The Win32 Internet function support is being shut down orunloaded.";
				ErrorInfo[ERROR_INTERNET_INCORRECT_USER_NAME]	= "The request to connect and log on to an FTP server couldnot be completed because the supplied user name isincorrect.";
				ErrorInfo[ERROR_INTERNET_INCORRECT_PASSWORD]	= "The request to connect and log on to an FTP server couldnot be completed because the supplied password isincorrect.";
				ErrorInfo[ERROR_INTERNET_LOGIN_FAILURE]			= "The request to connect to and log on to an FTP serverfailed.";
				ErrorInfo[ERROR_INTERNET_INVALID_OPERATION]		= "The requested operation is invalid.";
				ErrorInfo[ERROR_INTERNET_OPERATION_CANCELLED]	= "The operation was canceled, usually because the handle onwhich the request was operating was closed before theoperation completed.";
				ErrorInfo[ERROR_INTERNET_INCORRECT_HANDLE_TYPE] = "The type of handle supplied is incorrect for thisoperation.";
				ErrorInfo[ERROR_INTERNET_INCORRECT_HANDLE_STATE]= "The requested operation cannot be carried out because thehandle supplied is not in the correct state.";
				ErrorInfo[ERROR_INTERNET_NOT_PROXY_REQUEST]		= "The request cannot be made via a proxy.";
				ErrorInfo[ERROR_INTERNET_REGISTRY_VALUE_NOT_FOUND]= "A required registry value could not be located.";
				ErrorInfo[ERROR_INTERNET_BAD_REGISTRY_PARAMETER] = "A required registry value was located but is an incorrecttype or has an invalid value.";
				ErrorInfo[ERROR_INTERNET_NO_DIRECT_ACCESS]		= "Direct network access cannot be made at this time.";
				ErrorInfo[ERROR_INTERNET_NO_CONTEXT]			= "An asynchronous request could not be made because a zerocontext value was supplied.";
				ErrorInfo[ERROR_INTERNET_NO_CALLBACK]			= "An asynchronous request could not be made because acallback function has not been set.";
				ErrorInfo[ERROR_INTERNET_REQUEST_PENDING]		= "The required operation could not be completed because oneor more requests are pending.";
				ErrorInfo[ERROR_INTERNET_INCORRECT_FORMAT]		= "The format of the request is invalid.";
				ErrorInfo[ERROR_INTERNET_ITEM_NOT_FOUND]		= "The requested item could not be located.";
				ErrorInfo[ERROR_INTERNET_CANNOT_CONNECT]		= "The attempt to connect to the server failed.";
				ErrorInfo[ERROR_INTERNET_CONNECTION_ABORTED]	= "The connection with the server has been terminated.";
				ErrorInfo[ERROR_INTERNET_CONNECTION_RESET]		= "The connection with the server has been reset.";
				ErrorInfo[ERROR_INTERNET_FORCE_RETRY]			= "Calls for the Win32 Internet function to redo the request.";
				ErrorInfo[ERROR_INTERNET_INVALID_PROXY_REQUEST] = "The request to the proxy was invalid.";
				ErrorInfo[ERROR_INTERNET_HANDLE_EXISTS]			= "The request failed because the handle already exists.";
				ErrorInfo[ERROR_INTERNET_SEC_CERT_DATE_INVALID] = "SSL certificate date that was received from the server is bad. The certificate is expired.";
				ErrorInfo[ERROR_INTERNET_SEC_CERT_CN_INVALID]	= "SSL certificate common name (host name field) is incorrect.";
				ErrorInfo[ERROR_INTERNET_HTTP_TO_HTTPS_ON_REDIR]= "The application is moving from a non-SSL to an SSL connection because of a redirect.";
				ErrorInfo[ERROR_INTERNET_HTTPS_TO_HTTP_ON_REDIR]= "The application is moving from an SSL to an non-SSL connection because of a redirect.";
				ErrorInfo[ERROR_INTERNET_MIXED_SECURITY]		= "Indicates that the content is not entirely secure. Some of the content being viewed may have come from unsecured servers.";
				ErrorInfo[ERROR_HTTP_REDIRECT_FAILED]			= "The redirection failed because either the scheme changed(for example, HTTP to FTP) or all attempts made to redirectfailed (default is five attempts).";
				ErrorInfo[ERROR_HTTP_HEADER_ALREADY_EXISTS]		= "The header could not be added because it already exists.";
				ErrorInfo[ERROR_HTTP_INVALID_QUERY_REQUEST]		= "The request made to HttpQueryInfo is invalid.";
				ErrorInfo[ERROR_HTTP_INVALID_HEADER]			= "The supplied header is invalid.";
				ErrorInfo[ERROR_HTTP_INVALID_SERVER_RESPONSE]	= "The server response could not be parsed.";
				ErrorInfo[ERROR_HTTP_DOWNLEVEL_SERVER]			= "The server did not return any headers.";
				ErrorInfo[ERROR_HTTP_HEADER_NOT_FOUND]			= "The requested header could not be located.";
				ErrorInfo[ERROR_GOPHER_UNKNOWN_LOCATOR]			= "The locator type is unknown.";
				ErrorInfo[ERROR_GOPHER_ATTRIBUTE_NOT_FOUND]		= "The requested attribute could not be located.";
				ErrorInfo[ERROR_GOPHER_NOT_GOPHER_PLUS]			= "The requested operation can only be made against a Gopher+server or with a locator that specifies a Gopher+operation.";
				ErrorInfo[ERROR_GOPHER_INCORRECT_LOCATOR_TYPE]	= "The type of the locator is not correct for this operation.";
				ErrorInfo[ERROR_GOPHER_INVALID_LOCATOR]			= "The supplied locator is not valid.";
				ErrorInfo[ERROR_GOPHER_END_OF_DATA]				= "The end of the data has been reached.";
				ErrorInfo[ERROR_GOPHER_DATA_ERROR]				= "An error was detected while receiving data from the gopherserver.";
				ErrorInfo[ERROR_GOPHER_NOT_FILE]				= "The request must be made for a file locator.";
				ErrorInfo[ERROR_GOPHER_PROTOCOL_ERROR]			= "An error was detected while parsing data returned from thegopher server.";
				ErrorInfo[ERROR_FTP_DROPPED]					= "The FTP operation was not completed because the session wasaborted.";
				ErrorInfo[ERROR_FTP_TRANSFER_IN_PROGRESS]		= "The requested operation cannot be made on the FTP sessionhandle because an operation is already in progress.";
				ErrorInfo[ERROR_INTERNET_POST_IS_NON_SECURE]	= "The application is posting data to a server that is notsecure.";
				ErrorInfo[ERROR_INTERNET_CHG_POST_IS_NON_SECURE]= "The application is posting and attempting to changemultiple lines of text on a server that is not secure.";
			}
		};

		HttpError<0> httpError;
#else
		template <>
		struct HTTPError<1>
		{
			typedef std::map<int, std::string> MapErrorInfo;
			MapErrorInfo ErrorInfo;
			HTTPError()
			{
				ErrorInfo[ERROR_WINHTTP_AUTO_PROXY_SERVICE_ERROR]	= "Returned by WinHttpGetProxyForUrl when a proxy for the specified URL cannot be located.";
				ErrorInfo[ERROR_WINHTTP_AUTODETECTION_FAILED]		= "Returned by WinHttpDetectAutoProxyConfigUrl if WinHTTP was unable to discover the URL of the Proxy Auto-Configuration (PAC) file.";
				ErrorInfo[ERROR_WINHTTP_BAD_AUTO_PROXY_SCRIPT]		= "An error occurred executing the script code in the Proxy Auto-Configuration (PAC) file.";
				ErrorInfo[ERROR_WINHTTP_CANNOT_CALL_AFTER_OPEN]		= "Returned by the HttpRequest object if a specified option cannot be requested after the Open method has been called.";
				ErrorInfo[ERROR_WINHTTP_CANNOT_CALL_AFTER_SEND]		= "Returned by the HttpRequest object if a requested operation cannot be performed after calling the Send method.";
				ErrorInfo[ERROR_WINHTTP_CANNOT_CALL_BEFORE_OPEN]	= "Returned by the HttpRequest object if a requested operation cannot be performed before calling the Open method.";
				ErrorInfo[ERROR_WINHTTP_CANNOT_CALL_BEFORE_SEND]	= "Returned by the HttpRequest object if a requested operation cannot be performed before calling the Send method.";
				ErrorInfo[ERROR_WINHTTP_CANNOT_CONNECT]				= "Returned if connection to the server failed.";
				ErrorInfo[ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED]	= "The server requires SSL client Authentication. The application retrieves the list of certificate issuers by calling WinHttpQueryOption with the WINHTTP_OPTION_CLIENT_CERT_ISSUER_LIST option. For more information, see the WINHTTP_OPTION_CLIENT_CERT_ISSUER_LIST option.";
				ErrorInfo[ERROR_WINHTTP_CLIENT_CERT_NO_ACCESS_PRIVATE_KEY]= "The application does not have the required privileges to access the private key associated with the client certificate.";
				ErrorInfo[ERROR_WINHTTP_CLIENT_CERT_NO_PRIVATE_KEY]	= "The context for the SSL client certificate does not have a private key associated with it. The client certificate may have been imported to the computer without the private key.";
				ErrorInfo[ERROR_WINHTTP_CHUNKED_ENCODING_HEADER_SIZE_OVERFLOW]		= "Returned by WinHttpReceiveResponse when an overflow condition is encountered in the course of parsing chunked encoding.";
				ErrorInfo[ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED]	= "Returned by WinHttpReceiveResponse when the server requests client authentication";
				ErrorInfo[ERROR_WINHTTP_CONNECTION_ERROR]			= "The connection with the server has been reset or terminated, or an incompatible SSL protocol was encountered. For example, WinHTTP version 5.1 does not support SSL2 unless the client specifically enables it.";
				ErrorInfo[ERROR_WINHTTP_HEADER_ALREADY_EXISTS]		= "Obsolete; no longer used.";
				ErrorInfo[ERROR_WINHTTP_HEADER_COUNT_EXCEEDED]		= "Returned by WinHttpReceiveResponse when a larger number of headers were present in a response than WinHTTP could receive.";
				ErrorInfo[ERROR_WINHTTP_HEADER_NOT_FOUND]			= "The requested header cannot be located.";
				ErrorInfo[ERROR_WINHTTP_HEADER_SIZE_OVERFLOW]		= "Returned by WinHttpReceiveResponse when the size of headers received exceeds the limit for the request handle.";
				ErrorInfo[ERROR_WINHTTP_INCORRECT_HANDLE_STATE]		= "The requested operation cannot be carried out because the handle supplied is not in the correct state.";
				ErrorInfo[ERROR_WINHTTP_INCORRECT_HANDLE_TYPE]		= "The type of handle supplied is incorrect for this operation.";
				ErrorInfo[ERROR_WINHTTP_INTERNAL_ERROR]				= "An internal error has occurred.";
				ErrorInfo[ERROR_WINHTTP_INVALID_OPTION]				= "A request to WinHttpQueryOption or WinHttpSetOption specified an invalid option value.";
				ErrorInfo[ERROR_WINHTTP_INVALID_QUERY_REQUEST]		= "Obsolete; no longer used.";
				ErrorInfo[ERROR_WINHTTP_INVALID_SERVER_RESPONSE]	= "The server response cannot be parsed.";
				ErrorInfo[ERROR_WINHTTP_INVALID_URL]				= "The URL is not valid.";
				ErrorInfo[ERROR_WINHTTP_LOGIN_FAILURE]				= "The login attempt failed. When this error is encountered, the request handle should be closed with WinHttpCloseHandle. A new request handle must be created before retrying the function that originally produced this error.";
				ErrorInfo[ERROR_WINHTTP_NAME_NOT_RESOLVED]			= "The server name cannot be resolved.";
				ErrorInfo[ERROR_WINHTTP_NOT_INITIALIZED]			= "Obsolete; no longer used.";
				ErrorInfo[ERROR_WINHTTP_OPERATION_CANCELLED]		= "The operation was canceled, usually because the handle on which the request was operating was closed before the operation completed.";
				ErrorInfo[ERROR_WINHTTP_OPTION_NOT_SETTABLE]		= "The requested option cannot be set, only queried.";
				ErrorInfo[ERROR_WINHTTP_OUT_OF_HANDLES]				= "Obsolete; no longer used.";
				ErrorInfo[ERROR_WINHTTP_REDIRECT_FAILED]			= "The redirection failed because either the scheme changed or all attempts made to redirect failed (default is five attempts).";
				ErrorInfo[ERROR_WINHTTP_RESEND_REQUEST]				= "The WinHTTP function failed. The desired function can be retried on the same request handle.";
				ErrorInfo[ERROR_WINHTTP_RESPONSE_DRAIN_OVERFLOW]	= "Returned when an incoming response exceeds an internal WinHTTP size limit.";
				ErrorInfo[ERROR_WINHTTP_SECURE_CERT_CN_INVALID]		= "Returned when a certificate CN name does not match the passed value (equivalent to a CERT_E_CN_NO_MATCH error).";
				ErrorInfo[ERROR_NOT_SUPPORTED]						= "The required protocol stack is not loaded and the application cannot start WinSock.";
				ErrorInfo[ERROR_NO_MORE_ITEMS]						= "No more items have been found.";
				ErrorInfo[ERROR_NO_MORE_FILES]						= "No more files have been found.";
				ErrorInfo[ERROR_INVALID_HANDLE]						= "The handle passed to the application programming interface (API) has been either invalidated or closed.";
				ErrorInfo[ERROR_INSUFFICIENT_BUFFER]				= "The size, in bytes, of the buffer supplied to a function was insufficient to contain the returned data. For more information, see the specific function.";
				ErrorInfo[ERROR_NOT_ENOUGH_MEMORY]					= "Not enough memory was available to complete the requested operation.";
				ErrorInfo[ERROR_WINHTTP_UNRECOGNIZED_SCHEME]		= "The URL specified a scheme other than 'http:' or 'https:'.";
				ErrorInfo[ERROR_WINHTTP_UNABLE_TO_DOWNLOAD_SCRIPT]	= "The PAC file cannot be downloaded. For example, the server referenced by the PAC URL may not have been reachable, or the server returned a 404 NOT FOUND response.";
				ErrorInfo[ERROR_WINHTTP_TIMEOUT]					= "The request has timed out.";
				ErrorInfo[ERROR_WINHTTP_SHUTDOWN]					= "The WinHTTP function support is being shut down or unloaded.";
				ErrorInfo[ERROR_WINHTTP_SECURE_INVALID_CERT]		= "Indicates that a certificate is invalid (equivalent to errors such as CERT_E_ROLE, CERT_E_PATHLENCONST, CERT_E_CRITICAL, CERT_E_PURPOSE, CERT_E_ISSUERCHAINING, CERT_E_MALFORMED and CERT_E_CHAINING).";
				ErrorInfo[ERROR_WINHTTP_SECURE_INVALID_CA]			= "Indicates that a certificate chain was processed, but terminated in a root certificate that is not trusted by the trust provider (equivalent to CERT_E_UNTRUSTEDROOT).";
				ErrorInfo[ERROR_WINHTTP_SECURE_FAILURE]				= "One or more errors were found in the Secure Sockets Layer (SSL) certificate sent by the server. To determine what type of error was encountered, check for a WINHTTP_CALLBACK_STATUS_SECURE_FAILURE notification in a status callback function. For more information, see WINHTTP_STATUS_CALLBACK.";
				ErrorInfo[ERROR_WINHTTP_SECURE_CHANNEL_ERROR]		= "Indicates that an error occurred having to do with a secure channel (equivalent to error codes that begin with 'SEC_E_' and 'SEC_I_' listed in the 'winerror.h' header file).";
				ErrorInfo[ERROR_WINHTTP_SECURE_CERT_WRONG_USAGE]	= "Indicates that a certificate is not valid for the requested usage (equivalent to CERT_E_WRONG_USAGE).";
				ErrorInfo[ERROR_WINHTTP_SECURE_CERT_REVOKED]		= "Indicates that a certificate has been revoked (equivalent to CRYPT_E_REVOKED).";
				ErrorInfo[ERROR_WINHTTP_SECURE_CERT_REV_FAILED]		= "Indicates that revocation cannot be checked because the revocation server was offline (equivalent to CRYPT_E_REVOCATION_OFFLINE).";
				ErrorInfo[ERROR_WINHTTP_SECURE_CERT_DATE_INVALID]	= "Indicates that a required certificate is not within its validity period when verifying against the current system clock or the timestamp in the signed file, or that the validity periods of the certification chain do not nest correctly (equivalent to a CERT_E_EXPIRED or a CERT_E_VALIDITYPERIODNESTING error).";
			}
		};

		HTTPError<1> httpError;
#endif

		template < size_t N >
		inline const std::string &WinHttpError(DWORD error)
		{
			HTTPError<N>::MapErrorInfo::const_iterator iter = httpError.ErrorInfo.find(error);

			if( iter != httpError.ErrorInfo.end() )
				return iter->second;
			else
			{
				std::ostringstream oss;
				char *buffer = 0;
				::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					0, error, 0, (LPSTR)&buffer, 0, 0);
				oss << "Win32 Error(" << error << ") : ";
				if( buffer != 0 )
				{
					oss << buffer;

					::LocalFree(buffer);
				}
				
				static std::string info = oss.str();
				return info;
			}
		}
	

	}

	inline Buffer MakeBuffer(size_t sz)
	{
		assert(sz != 0);
		return std::make_pair(AutoBuffer(static_cast<char *>(detail::BufferAlloc(sz)), 
			std::tr1::bind(&detail::BufferDealloc, std::tr1::placeholders::_1, sz)), sz);
	}

#ifdef __USE_WININET
	template< typename CallbackT >
	inline Buffer DownloadFile(const stdex::tString &url, const CallbackT &callback)
	{
		using namespace WinInet;
		CInternetSession session;

		// 建立连接，并下载
		CHttpFile httpFile(session, url.c_str(), 0, -1, 
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE);
		if( httpFile == NULL )
			throw std::runtime_error(detail::WinHttpError<0>(::GetLastError()));

		DWORD dwLength = 0;
		httpFile.QueryInfo(HTTP_QUERY_CONTENT_LENGTH, dwLength);

		Buffer buf = MakeBuffer(dwLength + 1);

		DWORD dwSize = 0;
		const size_t readLen = 512;
		CHttpFile::CInfo info(dwLength);
		while( dwSize != dwLength )
		{
			DWORD nSize = 0;
			if( !httpFile.Read(buf.first.get() + dwSize, readLen, nSize, info) )
				throw std::runtime_error(detail::WinHttpError<0>(::GetLastError()));

			if( callback(buf.first.get() + dwSize, nSize, info) )
				throw std::logic_error("终止下载");

			dwSize += nSize;
		}

		return buf;
	}

#else

	template < typename CallbackT >
	inline Buffer DownloadFile(const std::wstring &url, const CallbackT &callback)
	{
        static const int C_INFO_MAX = 1000;
		URL_COMPONENTS urlComp		= {0};
		wchar_t host[C_INFO_MAX]	= {0};
		wchar_t req[C_INFO_MAX]		= {0};
		urlComp.dwStructSize		= sizeof(urlComp);
		urlComp.lpszHostName		= host;
		urlComp.dwHostNameLength	= C_INFO_MAX;
		urlComp.lpszUrlPath			= req;
		urlComp.dwUrlPathLength		= C_INFO_MAX;
		if( !::WinHttpCrackUrl(url.c_str(), url.length(), 0, &urlComp) )
			throw std::runtime_error(detail::WinHttpError<1>(::GetLastError()));

		u_short port = urlComp.nPort;

		utility::CAutoWinHttp hSession = ::WinHttpOpen(L"A WinHTTP download Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
			WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0); 
		if( !hSession.IsValid() )
			throw std::runtime_error(detail::WinHttpError<1>(::GetLastError()));

		utility::CAutoWinHttp hConnect = ::WinHttpConnect(hSession, host, port, 0);
		if( !hConnect.IsValid() )
			throw std::runtime_error(detail::WinHttpError<1>(::GetLastError()));

		utility::CAutoWinHttp hRequest = ::WinHttpOpenRequest(hConnect, L"GET", req, NULL, WINHTTP_NO_REFERER, 
				WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
		if( !hRequest.IsValid() )
			throw std::runtime_error(detail::WinHttpError<1>(::GetLastError()));

		if( !::WinHttpSendRequest( hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
			WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
			throw std::runtime_error(detail::WinHttpError<1>(::GetLastError()));

		if( !::WinHttpReceiveResponse( hRequest, NULL) )
			throw std::runtime_error(detail::WinHttpError<1>(::GetLastError()));

		DWORD dwTotal = 0;
		DWORD dwSize = sizeof(DWORD);
		if( !::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER ,
				NULL, &dwTotal, &dwSize, WINHTTP_NO_HEADER_INDEX) || dwTotal != HTTP_STATUS_OK )
			throw std::runtime_error(detail::WinHttpError<1>(::GetLastError()));

		dwSize = sizeof(DWORD);
		if( !::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_LENGTH|WINHTTP_QUERY_FLAG_NUMBER,
				NULL, &dwTotal, &dwSize, WINHTTP_NO_HEADER_INDEX) )
			throw std::runtime_error(detail::WinHttpError<1>(::GetLastError()));

		DWORD dwReadBytes = 0;
		char* pszOutBuffer = NULL;
			
		Buffer buf = MakeBuffer(dwTotal);
		pszOutBuffer = buf.first.get();

		DWORD pos = 0;
		while( pos != dwTotal && !callback(pszOutBuffer + pos, pos) ) 
		{
			DWORD read = 0;
				
			if( !::WinHttpQueryDataAvailable(hRequest, &read ) )
				throw std::runtime_error(detail::WinHttpError<1>(::GetLastError()));

				
			if( !::WinHttpReadData(hRequest, (LPVOID)(pszOutBuffer + pos), read, &dwReadBytes) )
				throw std::runtime_error(detail::WinHttpError<1>(::GetLastError()));

			pos += dwReadBytes;
		}

		return buf;
	}

#endif


	template < typename T, typename U >
	struct AutoCloseZip
	{
		void operator()(T val)
		{
			CloseZip(val);
		}
	};

	typedef utility::CSmartHandle<HZIP, AutoCloseZip> AutoZipHandle;


	// 解压
	template < typename BufferT >
	inline bool UnZip(const BufferT &inBuf, const std::basic_string<TCHAR> &path)		
	{
		AutoZipHandle hz = ::OpenZip(inBuf.first.get(), inBuf.second, ZIP_MEMORY, path.c_str());
		if( !hz.IsValid() )
			return false;

		ZIPENTRYW ze = {0}; 
		HRESULT hRes = ::GetZipItem(hz, -1 ,&ze); 
		if( hRes != S_OK )
			return false;

		int numitems = ze.index;
		for(int i=0; i != numitems; ++i)
		{ 
			hRes = ::GetZipItem(hz,i, &ze);
			if( hRes != S_OK )
				return false;

			hRes = ::UnzipItem(hz, i, ze.name, 0, ZIP_FILENAME);
			if( hRes != S_OK )
				return false;
		}

		return true;
	}

	template < typename BufferT >
	inline BufferT UnZip(const BufferT &inBuf, BufferT (*MakeBufferFn)(size_t) = MakeBuffer)
	{
		// 解压
		AutoZipHandle hz = ::OpenZip(inBuf.first.get(), inBuf.second, ZIP_MEMORY, NULL);
		if( !hz.IsValid() )
			throw std::runtime_error("打开压缩文件出错!");

		ZIPENTRYW ze = {0}; 
		const int index = 0;
		HRESULT res = ::GetZipItemW(hz, index, &ze);
		if( res != S_OK )
			throw std::runtime_error("打开压缩文件出错!");

		BufferT outBuf = (*MakeBufferFn)(ze.unc_size);

		HRESULT zr = ZR_MORE; 
		unsigned long readSize = 0;
		const size_t unzipLen = 4096;
		while( zr == ZR_MORE )
		{
			const size_t leftLen = ze.unc_size - readSize;
			size_t readLen = std::min(leftLen, unzipLen);

			res = ::UnzipItem(hz,index, outBuf.first.get() + readSize, readLen, ZIP_MEMORY);

			if( res == S_OK )
				break;

			readSize += readLen;
		}

		assert(readSize == ze.unc_size);

		return outBuf;
	}
}




#endif