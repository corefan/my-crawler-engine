// WinHTTP.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <tchar.h>
#include <Windows.h>
#include <iostream>
#include <cassert>


#include "Http/win_http/win_http.hpp"
#pragma comment(lib, "win_http")


void header(const http::request &req, std::uint32_t size)
{
	http::query::raw_headers accept;
	req.query_http_header(accept);

	http::query::content_type content_type;
	req.query_http_header(content_type);

	http::query::date date;
	req.query_http_header(date);

	http::query::expires expires;
	req.query_http_header(expires);

	std::wcout << (const wchar_t *)accept.buffer() << std::endl << size << std::endl;
}

void response_complete(bool suc)
{
	assert(suc);
}

bool read(const char *buf, size_t len)
{
	std::cout.write(buf, len);
	return true;
}

void redirect(const wchar_t *url, size_t len)
{
	std::cout << url << std::endl;
}

void error(const std::string &msg)
{
	std::cout << msg << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		http::session session(L"test");

		std::chrono::seconds second(5);
		session.set_timeout(second, second, second, second);
		

		http::url url(L"http://www.baidu.com");
		http::connection con(session, url);
		http::request request(con, L"GET", url);
		/*request.set_option(http::option::security(SECURITY_FLAG_IGNORE_CERT_CN_INVALID
			| SECURITY_FLAG_IGNORE_CERT_DATE_INVALID
			| SECURITY_FLAG_IGNORE_UNKNOWN_CA));
		request.set_option(http::option::user_name(L"default proxy name"));
		request.set_option(http::option::password(L"default proxy password"));*/
		request.register_callback(&header, &response_complete, &read, &redirect, &error);

		request.send_request(L"");
		system("pause");
	}
	catch(std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	system("pause");
	return 0;
}

