#include "handle.hpp"

#include <cassert>


namespace http
{

	handle::handle() 
		: handle_(0)
	{}

	handle::~handle()
	{
		if( 0 != handle_ )
		{   
			auto suc = ::WinHttpCloseHandle(handle_);
			assert(suc);
			handle_ = 0;
		}
	}


	handle::handle(HINTERNET handle)
		: handle_(handle)
	{}


	HRESULT handle::query_option(DWORD option, void* value, DWORD& length) const
	{
		if( !::WinHttpQueryOption(handle_, option, value, &length) )
		{
			return HRESULT_FROM_WIN32(::GetLastError());
		}

		return S_OK;
	}


}