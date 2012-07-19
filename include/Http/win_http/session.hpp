#ifndef __WIN_HTTP_SESSION_HPP
#define __WIN_HTTP_SESSION_HPP

#include "handle.hpp"
#include "time/chrono.hpp"

namespace http
{
	class session 
		: public handle
	{
	public:
		session(const std::wstring &agent);

	public:
		void set_timeout(std::chrono::milliseconds resolve, std::chrono::milliseconds connect,
			std::chrono::milliseconds send, std::chrono::milliseconds recv);
	};
}



#endif