#include "request.hpp"

#include <cassert>

#include "url.hpp"
#include "connection.hpp"
#include "Unicode/string.hpp"



namespace http
{


	request::request(const connection &con, const std::wstring &verb, const url &url_path)
		: handle(::WinHttpOpenRequest(con, verb.c_str(), url_path.path(), 0, WINHTTP_NO_REFERER, 
		WINHTTP_DEFAULT_ACCEPT_TYPES, url_path.is_secure() ? WINHTTP_FLAG_SECURE : 0))
		, url_(url_path)
	{
		if( !is_valid() )
			throw http_error();

		auto func = ::WinHttpSetStatusCallback(*this, &request::callback, WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, 0);
		assert(func != WINHTTP_INVALID_STATUS_CALLBACK);
		if( func == WINHTTP_INVALID_STATUS_CALLBACK )
			throw http_error("set http callback error");
	}

	request::~request()
	{
		::WinHttpSetStatusCallback(*this, 0, 0, 0);
	}

	void request::send_request(const std::wstring &headers, const std::vector<const void *> &request_data)
	{
		assert(response_complete_ != 0 &&
			read_ != 0 &&
			error_ != 0);

		BOOL suc = ::WinHttpSendRequest(*this, headers.c_str(), headers.length(), 
			(LPVOID)request_data.data(), request_data.size(), headers.length() + request_data.size(), (DWORD_PTR)this);
		assert(suc);
		if( !suc )
			throw http_error();
	}

	void request::register_callback(const handle_header_complete_type &header_complete, const handle_response_complete_type &response_complete, 
		const handle_read_type &read, const handle_redirect_type &redirect, const handle_error_type &error)
	{
		header_complete_	= header_complete;
		response_complete_	= response_complete;
		read_				= read;
		redirect_			= redirect;
		error_				= error;
	}

	void CALLBACK request::callback(HINTERNET handle, DWORD_PTR context, DWORD code, void* info, DWORD length)
	{
		if( 0 != context )
		{
			request* pT = reinterpret_cast<request *>(context);

			try
			{
				bool suc = pT->_on_callback(code, info, length);
			}
			catch(std::exception &e)
			{
				std::string msg = e.what() + std::string(" url:") + unicode::to_a(pT->url_.ori_url());
				pT->error_(std::cref(msg));
				pT->response_complete_(false);
			}
		}
	}


	namespace
	{
		void handle_default(request *req, const void* info, DWORD length)
		{
			
		}

		void handle_header_available(request *req, const void* info, DWORD length)
		{
			DWORD statusCode = 0;
			DWORD statusCodeSize = sizeof(DWORD);

			if( !::WinHttpQueryHeaders(*req,
				WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
				WINHTTP_HEADER_NAME_BY_INDEX,
				&statusCode,
				&statusCodeSize,
				WINHTTP_NO_HEADER_INDEX) )
				throw http_error();

			if( HTTP_STATUS_OK != statusCode )
				throw http_error(error_code(statusCode));

			DWORD data_len = 0;
			DWORD data_len_size = sizeof(data_len);
			if( !::WinHttpQueryHeaders(*req, 
				WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
				WINHTTP_HEADER_NAME_BY_INDEX, 
				&data_len, 
				&data_len_size, 
				WINHTTP_NO_HEADER_INDEX) )
				throw http_error();

			
			req->header_complete_(std::cref(*req), data_len);

			if( !::WinHttpReadData(*req, &req->buf_[0], req->buf_.size(), 0) ) // async result
				throw http_error();
		}

		void handle_read_complete(request *req, const void* info, DWORD length)
		{
			if( 0 < length )
			{
				if( !req->read_(req->buf_.data(), length) )
					throw http_error("user exit");

				if( !::WinHttpReadData(*req, &req->buf_[0], req->buf_.size(), 0) ) // async result
					throw http_error();
			}
			else
			{
				req->response_complete_(true);
			}
		}

		void handle_send_complete(request *req, const void* info, DWORD length)
		{
			if( !::WinHttpReceiveResponse(*req, 0) ) // reserved
				throw http_error();
		}

		void handle_redirect(request *req, const void* info, DWORD length)
		{
			req->redirect_(reinterpret_cast<const wchar_t *>(info), length);
		}

		void handle_ssl_error(request *req, const void* info, DWORD length)
		{
			throw http_error(ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED);
		}

		void handle_error(request *req, const void* info, DWORD length)
		{
			const WINHTTP_ASYNC_RESULT *result = reinterpret_cast<const WINHTTP_ASYNC_RESULT *>(info);
			throw http_error(result->dwError);
		}

		struct
		{
			size_t code_;
			typedef void (*status_handle)(request *, const void*, DWORD);
			status_handle handle_;
		}handle_code[] = 
		{
			{ WINHTTP_CALLBACK_STATUS_RESOLVING_NAME,			&handle_default },
			{ WINHTTP_CALLBACK_STATUS_NAME_RESOLVED,			&handle_default },
			{ WINHTTP_CALLBACK_STATUS_CONNECTING_TO_SERVER,		&handle_default },
			{ WINHTTP_CALLBACK_STATUS_CONNECTED_TO_SERVER,		&handle_default },
			{ WINHTTP_CALLBACK_STATUS_SENDING_REQUEST,			&handle_default },
			{ WINHTTP_CALLBACK_STATUS_REQUEST_SENT,				&handle_default },
			{ WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE,		&handle_default },
			{ WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED,		&handle_default },
			{ WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION,		&handle_default },
			{ WINHTTP_CALLBACK_STATUS_CONNECTION_CLOSED,		&handle_default },
			{ WINHTTP_CALLBACK_STATUS_HANDLE_CREATED,			&handle_default },
			{ WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING,			&handle_default },
			{ WINHTTP_CALLBACK_STATUS_DETECTING_PROXY,			&handle_default },
			{ WINHTTP_CALLBACK_STATUS_REDIRECT,					&handle_redirect },
			{ WINHTTP_CALLBACK_STATUS_INTERMEDIATE_RESPONSE,	&handle_default },
			{ WINHTTP_CALLBACK_STATUS_SECURE_FAILURE,			&handle_ssl_error },
			{ WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE,		&handle_header_available },
			{ WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE,			&handle_default },
			{ WINHTTP_CALLBACK_STATUS_READ_COMPLETE,			&handle_read_complete },
			{ WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE,			&handle_default },
			{ WINHTTP_CALLBACK_STATUS_REQUEST_ERROR,			&handle_error },
			{ WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE,		&handle_send_complete },
		};
	}

	bool request::_on_callback(DWORD code, const void* info, DWORD length)
	{
		for(size_t i = 0; i != _countof(handle_code); ++i)
		{
			if( code == handle_code[i].code_ )
			{
				handle_code[i].handle_(this, info, length);
				return true;
			}
		}
		
		assert(0);
		return false;
	}
}
