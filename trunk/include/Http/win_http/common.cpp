#include "common.hpp"

#include <cassert>


namespace http
{	

	struct http_error_info
	{
		typedef std::map<int, std::string> error_info_table_type;
		error_info_table_type errors_;

		http_error_info()
		{
			errors_[ERROR_WINHTTP_AUTO_PROXY_SERVICE_ERROR]	= "Returned by WinHttpGetProxyForUrl when a proxy for the specified URL cannot be located.";
			errors_[ERROR_WINHTTP_AUTODETECTION_FAILED]		= "Returned by WinHttpDetectAutoProxyConfigUrl if WinHTTP was unable to discover the URL of the Proxy Auto-Configuration (PAC) file.";
			errors_[ERROR_WINHTTP_BAD_AUTO_PROXY_SCRIPT]		= "An error occurred executing the script code in the Proxy Auto-Configuration (PAC) file.";
			errors_[ERROR_WINHTTP_CANNOT_CALL_AFTER_OPEN]		= "Returned by the HttpRequest object if a specified option cannot be requested after the Open method has been called.";
			errors_[ERROR_WINHTTP_CANNOT_CALL_AFTER_SEND]		= "Returned by the HttpRequest object if a requested operation cannot be performed after calling the Send method.";
			errors_[ERROR_WINHTTP_CANNOT_CALL_BEFORE_OPEN]	= "Returned by the HttpRequest object if a requested operation cannot be performed before calling the Open method.";
			errors_[ERROR_WINHTTP_CANNOT_CALL_BEFORE_SEND]	= "Returned by the HttpRequest object if a requested operation cannot be performed before calling the Send method.";
			errors_[ERROR_WINHTTP_CANNOT_CONNECT]				= "Returned if connection to the server failed.";
			errors_[ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED]	= "The server requires SSL client Authentication. The application retrieves the list of certificate issuers by calling WinHttpQueryOption with the WINHTTP_OPTION_CLIENT_CERT_ISSUER_LIST option. For more information, see the WINHTTP_OPTION_CLIENT_CERT_ISSUER_LIST option.";
			errors_[ERROR_WINHTTP_CLIENT_CERT_NO_ACCESS_PRIVATE_KEY]= "The application does not have the required privileges to access the private key associated with the client certificate.";
			errors_[ERROR_WINHTTP_CLIENT_CERT_NO_PRIVATE_KEY]	= "The context for the SSL client certificate does not have a private key associated with it. The client certificate may have been imported to the computer without the private key.";
			errors_[ERROR_WINHTTP_CHUNKED_ENCODING_HEADER_SIZE_OVERFLOW]		= "Returned by WinHttpReceiveResponse when an overflow condition is encountered in the course of parsing chunked encoding.";
			errors_[ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED]	= "Returned by WinHttpReceiveResponse when the server requests client authentication";
			errors_[ERROR_WINHTTP_CONNECTION_ERROR]			= "The connection with the server has been reset or terminated, or an incompatible SSL protocol was encountered. For example, WinHTTP version 5.1 does not support SSL2 unless the client specifically enables it.";
			errors_[ERROR_WINHTTP_HEADER_ALREADY_EXISTS]		= "Obsolete; no longer used.";
			errors_[ERROR_WINHTTP_HEADER_COUNT_EXCEEDED]		= "Returned by WinHttpReceiveResponse when a larger number of headers were present in a response than WinHTTP could receive.";
			errors_[ERROR_WINHTTP_HEADER_NOT_FOUND]			= "The requested header cannot be located.";
			errors_[ERROR_WINHTTP_HEADER_SIZE_OVERFLOW]		= "Returned by WinHttpReceiveResponse when the size of headers received exceeds the limit for the request handle.";
			errors_[ERROR_WINHTTP_INCORRECT_HANDLE_STATE]		= "The requested operation cannot be carried out because the handle supplied is not in the correct state.";
			errors_[ERROR_WINHTTP_INCORRECT_HANDLE_TYPE]		= "The type of handle supplied is incorrect for this operation.";
			errors_[ERROR_WINHTTP_INTERNAL_ERROR]				= "An internal error has occurred.";
			errors_[ERROR_WINHTTP_INVALID_OPTION]				= "A request to WinHttpQueryOption or WinHttpSetOption specified an invalid option value.";
			errors_[ERROR_WINHTTP_INVALID_QUERY_REQUEST]		= "Obsolete; no longer used.";
			errors_[ERROR_WINHTTP_INVALID_SERVER_RESPONSE]	= "The server response cannot be parsed.";
			errors_[ERROR_WINHTTP_INVALID_URL]				= "The URL is not valid.";
			errors_[ERROR_WINHTTP_LOGIN_FAILURE]				= "The login attempt failed. When this error is encountered, the request handle should be closed with WinHttpCloseHandle. A new request handle must be created before retrying the function that originally produced this error.";
			errors_[ERROR_WINHTTP_NAME_NOT_RESOLVED]			= "The server name cannot be resolved.";
			errors_[ERROR_WINHTTP_NOT_INITIALIZED]			= "Obsolete; no longer used.";
			errors_[ERROR_WINHTTP_OPERATION_CANCELLED]		= "The operation was canceled, usually because the handle on which the request was operating was closed before the operation completed.";
			errors_[ERROR_WINHTTP_OPTION_NOT_SETTABLE]		= "The requested option cannot be set, only queried.";
			errors_[ERROR_WINHTTP_OUT_OF_HANDLES]				= "Obsolete; no longer used.";
			errors_[ERROR_WINHTTP_REDIRECT_FAILED]			= "The redirection failed because either the scheme changed or all attempts made to redirect failed (default is five attempts).";
			errors_[ERROR_WINHTTP_RESEND_REQUEST]				= "The WinHTTP function failed. The desired function can be retried on the same request handle.";
			errors_[ERROR_WINHTTP_RESPONSE_DRAIN_OVERFLOW]	= "Returned when an incoming response exceeds an internal WinHTTP size limit.";
			errors_[ERROR_WINHTTP_SECURE_CERT_CN_INVALID]		= "Returned when a certificate CN name does not match the passed value (equivalent to a CERT_E_CN_NO_MATCH error).";
			errors_[ERROR_NOT_SUPPORTED]						= "The required protocol stack is not loaded and the application cannot start WinSock.";
			errors_[ERROR_NO_MORE_ITEMS]						= "No more items have been found.";
			errors_[ERROR_NO_MORE_FILES]						= "No more files have been found.";
			errors_[ERROR_INVALID_HANDLE]						= "The handle passed to the application programming interface (API) has been either invalidated or closed.";
			errors_[ERROR_INSUFFICIENT_BUFFER]				= "The size, in bytes, of the buffer supplied to a function was insufficient to contain the returned data. For more information, see the specific function.";
			errors_[ERROR_NOT_ENOUGH_MEMORY]					= "Not enough memory was available to complete the requested operation.";
			errors_[ERROR_WINHTTP_UNRECOGNIZED_SCHEME]		= "The URL specified a scheme other than 'http:' or 'https:'.";
			errors_[ERROR_WINHTTP_UNABLE_TO_DOWNLOAD_SCRIPT]	= "The PAC file cannot be downloaded. For example, the server referenced by the PAC URL may not have been reachable, or the server returned a 404 NOT FOUND response.";
			errors_[ERROR_WINHTTP_TIMEOUT]					= "The request has timed out.";
			errors_[ERROR_WINHTTP_SHUTDOWN]					= "The WinHTTP function support is being shut down or unloaded.";
			errors_[ERROR_WINHTTP_SECURE_INVALID_CERT]		= "Indicates that a certificate is invalid (equivalent to errors such as CERT_E_ROLE, CERT_E_PATHLENCONST, CERT_E_CRITICAL, CERT_E_PURPOSE, CERT_E_ISSUERCHAINING, CERT_E_MALFORMED and CERT_E_CHAINING).";
			errors_[ERROR_WINHTTP_SECURE_INVALID_CA]			= "Indicates that a certificate chain was processed, but terminated in a root certificate that is not trusted by the trust provider (equivalent to CERT_E_UNTRUSTEDROOT).";
			errors_[ERROR_WINHTTP_SECURE_FAILURE]				= "One or more errors were found in the Secure Sockets Layer (SSL) certificate sent by the server. To determine what type of error was encountered, check for a WINHTTP_CALLBACK_STATUS_SECURE_FAILURE notification in a status callback function. For more information, see WINHTTP_STATUS_CALLBACK.";
			errors_[ERROR_WINHTTP_SECURE_CHANNEL_ERROR]		= "Indicates that an error occurred having to do with a secure channel (equivalent to error codes that begin with 'SEC_E_' and 'SEC_I_' listed in the 'winerror.h' header file).";
			errors_[ERROR_WINHTTP_SECURE_CERT_WRONG_USAGE]	= "Indicates that a certificate is not valid for the requested usage (equivalent to CERT_E_WRONG_USAGE).";
			errors_[ERROR_WINHTTP_SECURE_CERT_REVOKED]		= "Indicates that a certificate has been revoked (equivalent to CRYPT_E_REVOKED).";
			errors_[ERROR_WINHTTP_SECURE_CERT_REV_FAILED]		= "Indicates that revocation cannot be checked because the revocation server was offline (equivalent to CRYPT_E_REVOCATION_OFFLINE).";
			errors_[ERROR_WINHTTP_SECURE_CERT_DATE_INVALID]	= "Indicates that a required certificate is not within its validity period when verifying against the current system clock or the timestamp in the signed file, or that the validity periods of the certification chain do not nest correctly (equivalent to a CERT_E_EXPIRED or a CERT_E_VALIDITYPERIODNESTING error).";
		}
	};

	const http_error_info &make_error_info()
	{
		static http_error_info error_info;
		return error_info;
	}
	const http_error_info &error_info = make_error_info();



	// -------------------------------

	struct http_error_code
	{
		typedef std::map<error_code, std::string> error_code_table_type;
		error_code_table_type errors_;


		http_error_code()
		{
			errors_[malformed_status_line]			= "The response's status line was malformed.";
			errors_[malformed_response_headers]		= "The response's headers were malformed.";

			errors_[continue_request]				= "100 Continue";
			errors_[switching_protocols]			= "101 Switching Protocols";
			errors_[ok]								= "200 OK";
			errors_[created]						= "201 Created";
			errors_[accepted]						= "202 Accepted";
			errors_[non_authoritative_information]	= "203 Non-Authoritative Information";
			errors_[no_content]						= "204 No Content";
			errors_[reset_content]					= "205 Reset Content";
			errors_[partial_content]				= "206 Partial Content";
			errors_[multiple_choices]				= "300 Multiple Choices";
			errors_[moved_permanently]				= "301 Moved Permanently";
			errors_[found]							= "302 Found";
			errors_[see_other]						= "303 See Other";
			errors_[not_modified]					= "304 Not Modified";
			errors_[use_proxy]						= "305 Use Proxy";
			errors_[temporary_redirect]				= "307 Temporary Redirect";
			errors_[bad_request]					= "400 Bad Request";
			errors_[unauthorized]					= "401 Unauthorized";
			errors_[payment_required]				= "402 Payment Required";
			errors_[forbidden]						= "403 Forbidden";
			errors_[not_found]						= "404 Not Found";
			errors_[method_not_allowed]				= "405 Method Not Allowed";
			errors_[not_acceptable]					= "406 Not Acceptable";
			errors_[proxy_authentication_required]	= "407 Proxy Authentication Required";
			errors_[request_timeout]				= "408 Request Time-out";
			errors_[conflict]						= "409 Conflict";
			errors_[gone]							= "410 Gone";
			errors_[length_required]				= "411 Length Required";
			errors_[precondition_failed]			= "412 Precondition Failed";
			errors_[request_entity_too_large]		= "413 Request Entity Too Large";
			errors_[request_uri_too_large]			= "414 Request URI Too Large";
			errors_[unsupported_media_type]			= "415 Unsupported Media Type";
			errors_[requested_range_not_satisfiable]= "416 Requested Range Not Satisfiable";
			errors_[expectation_failed]				= "417 Expectation Failed";
			errors_[internal_server_error]			= "500 Internal Server Error";
			errors_[not_implemented]				= "501 Not Implemented";
			errors_[bad_gateway]					= "502 Bad Gateway";
			errors_[service_unavailable]			= "503 Service Unavailable";
			errors_[gateway_timeout]				= "504 Gateway Timeout";
			errors_[version_not_supported]			= "505 HTTP Version Not Supported";
		}
	};

	const http_error_code &make_error_code()
	{
		static http_error_code error_code;
		return error_code;
	}
	const http_error_code &error_code_ = make_error_code();

	// ------------------------------------------
	// 

	http_error::http_error(DWORD error)
		: runtime_error(0)
	{
		
		auto iter = error_info.errors_.find(error);

		if( iter != error_info.errors_.end() )
		{
			msg_ = iter->second;
		}
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

			msg_ = oss.str();
		}

	}


	http_error::http_error(const char *msg)
		: runtime_error(msg)
		, msg_(msg)
	{

	}

	http_error::http_error(error_code code)
		: runtime_error(0)
	{
		auto iter = error_code_.errors_.find(code);
		assert( iter != error_code_.errors_.end());
		if( iter != error_code_.errors_.end() )
			msg_ = iter->second;
	}
}