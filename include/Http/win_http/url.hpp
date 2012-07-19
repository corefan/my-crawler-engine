#ifndef __WIN_HTTP_URL_HPP
#define __WIN_HTTP_URL_HPP

#include "common.hpp"


namespace http
{
	class url
	{
		static const size_t MAX_LENGTH = 256;

	private:
		std::wstring url_;
		bool secure_;	// HTTP ot HTTPS
		std::uint16_t port_;
		wchar_t host_[MAX_LENGTH];
		wchar_t path_[MAX_LENGTH];
		

	public:
		explicit url(const std::wstring &url_path);

	public:
		const wchar_t *host() const
		{ return host_; }

		const wchar_t *path() const
		{ return path_; }

		std::uint16_t port() const
		{ return port_; }

		bool is_secure() const
		{ return secure_; }

		const std::wstring &ori_url() const
		{
			return url_;
		}
	};
}



#endif