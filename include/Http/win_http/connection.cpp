#include "connection.hpp"

#include "session.hpp"
#include "url.hpp"



namespace http
{
	connection::connection(const session &session_val, const url &url_path)
		: handle(::WinHttpConnect(session_val, url_path.host(), url_path.port(), 0))
	{
		if( !is_valid() )
			throw http_error();
	}
}