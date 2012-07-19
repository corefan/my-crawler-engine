#ifndef __WIN_HTTP_CONNECTION_HPP
#define __WIN_HTTP_CONNECTION_HPP

#include "handle.hpp"


namespace http
{
	class session;
	class url;


	class connection 
		: public handle
	{
	public:
		connection(const session &session_val, const url &url_path);
	};
}




#endif