#include "session.hpp"
#include <cassert>


namespace http
{
	session::session(const std::wstring &agent)
		: handle(::WinHttpOpen(agent.c_str(),
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		WINHTTP_FLAG_ASYNC))
	{
		if( !is_valid() )
			throw http_error();
	}

	void session::set_timeout(std::chrono::milliseconds resolve, std::chrono::milliseconds connect,
		std::chrono::milliseconds send, std::chrono::milliseconds recv)
	{
		assert(is_valid());

		auto suc = ::WinHttpSetTimeouts(*this, (int)resolve.count(), (int)connect.count(), 
			(int)send.count(), (int)recv.count());
		assert(suc);
	}
}