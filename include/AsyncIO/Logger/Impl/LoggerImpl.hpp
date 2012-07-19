#ifndef __LOGGER_LOGGER_IMPL_HPP
#define __LOGGER_LOGGER_IMPL_HPP


#include "LoggerService.hpp"
#include "BasicLogger.hpp"


namespace async
{
	namespace logger
	{
		typedef impl::BasicLogger< impl::LoggerService<async::iocp::IODispatcher> > Logger;
		typedef std::tr1::shared_ptr<Logger>										LoggerPtr;
	}
}






#endif