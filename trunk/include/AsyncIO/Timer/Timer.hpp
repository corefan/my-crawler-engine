#ifndef __TIMER_TIMER_HPP
#define __TIMER_TIMER_HPP

#include "../Basic.hpp"
#include "impl/BasicTimer.hpp"
#include "impl/TimerImpl.hpp"


namespace async
{
	namespace timer
	{
		typedef impl::BasicTimer<impl::WaitableTimer, async::iocp::IODispatcher>	Timer;

		typedef std::tr1::shared_ptr<Timer>											TimerPtr;
	}
}



#endif