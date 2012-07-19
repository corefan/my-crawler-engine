#ifndef __WIN_HTTP_REQUEST_HPP
#define __WIN_HTTP_REQUEST_HPP


#include <array>
#include <vector>
#include <functional>
#include <cstdint>

#include "url.hpp"
#include "handle.hpp"
#include "query_header.hpp"

namespace http
{
	class connection;

	class request 
		: public handle
	{
	public:
		typedef std::function<void(const request &, std::uint32_t)>		handle_header_complete_type;
		typedef std::function<void(bool suc)>							handle_response_complete_type;
		typedef std::function<bool(const char *buf, size_t len)>		handle_read_type;
		typedef std::function<void(const wchar_t *url, size_t len)>		handle_redirect_type;
		typedef std::function<void(const std::string &)>				handle_error_type;

	public:
		handle_header_complete_type header_complete_;
		handle_response_complete_type response_complete_;
		handle_read_type read_;
		handle_redirect_type redirect_;
		handle_error_type error_;

		url url_;
		std::array<char, 8192> buf_;

	public:
		request(const connection &con, const std::wstring &verb, const url &url_path);
		~request();

	public:
		void send_request(const std::wstring &headers = L"", const std::vector<const void *> &request_data = std::vector<const void *>());
		void register_callback(const handle_header_complete_type &header_complete, const handle_response_complete_type &response_complete, 
			const handle_read_type &read, const handle_redirect_type &redirect, const handle_error_type &error);

		template < int level >
		bool query_http_header(query::header<level> &val) const
		{
			auto suc = ::WinHttpQueryHeaders(*this, val.level(), val.name(), val.buffer(), val.length(), val.index());
			if( !suc && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER )
			{
				val.resize(*(val.length()));
				suc = ::WinHttpQueryHeaders(*this, val.level(), val.name(), val.buffer(), val.length(), val.index());
			}
			
			return suc == TRUE;
		}

	private:
		static void CALLBACK callback(HINTERNET handle, DWORD_PTR context, DWORD code, void* info, DWORD length);
		bool _on_callback(DWORD code, const void* info, DWORD length);
	};
}



#endif