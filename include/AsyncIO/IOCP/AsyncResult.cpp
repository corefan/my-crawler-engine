#include "AsyncResult.hpp"


namespace async
{
	namespace iocp
	{

		AsyncCallback::~AsyncCallback()
		{

		}

		void AsyncCallback::Invoke(AsyncCallbackBase *p, u_long error, u_long size)
		{
			if( handler_ != 0 )
				handler_(size, error);

			AsyncCallbackBasePtr ptr(p);
		}
	}
}