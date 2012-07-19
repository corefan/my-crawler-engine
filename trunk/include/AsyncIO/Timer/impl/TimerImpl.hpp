#ifndef __TIMER_WAITEABLE_TIMER_HPP
#define __TIMER_WAITEABLE_TIMER_HPP

#include "../../Basic.hpp"
#include "../../iocp/WinException.hpp"


namespace async
{
	namespace timer
	{
		namespace impl
		{
			// --------------------------------------------------
			// class WaitableTimer

			class WaitableTimer
			{
			private:
				HANDLE timer_;
				long period_;		// 间隔时间
				long due_;			// 第一次后延长时间
				
			public:
				WaitableTimer(long period, long due, bool manualReset = false, const wchar_t *timerName = NULL)
					: timer_(NULL)
					, period_(period)
					, due_(due)
				{
					timer_ = ::CreateWaitableTimerW(NULL, manualReset ? TRUE : FALSE, timerName);
				}

				~WaitableTimer()
				{
					if( timer_ != NULL )
					{
						::CloseHandle(timer_);
						timer_ = NULL;
					}
				}

			public:
				HANDLE Get()
				{
					return timer_;
				}
				const HANDLE Get() const
				{
					return timer_;
				}

				operator HANDLE()
				{
					return timer_;
				}
				operator const HANDLE() const
				{
					return timer_;
				}

				bool operator==(const WaitableTimer &timer) const
				{
					return timer.timer_ == timer_;
				}

			public:
				void SetTimer(long period, long delay)
				{
					assert(timer_ != NULL);

					LARGE_INTEGER dueTime = {0};
					dueTime.QuadPart = -(delay * 10000000);

					period_ = period;
					due_ = delay;

					if( !::SetWaitableTimer(timer_, &dueTime, period, NULL, NULL, TRUE) )
						throw async::iocp::Win32Exception("SetWaitableTimer");
				}

				void Cancel()
				{
					assert(timer_ != NULL);
					if( !::CancelWaitableTimer(timer_) )
						throw async::iocp::Win32Exception("CancelWaitableTimer");
				}

				void SyncWait() const
				{
					assert(timer_ != NULL);

					DWORD res = ::WaitForSingleObject(timer_, period_);
					if( res == WAIT_FAILED )
						throw async::iocp::Win32Exception("WaitForSingleObject");
				}

				void AsyncWait()
				{
					assert(timer_ != NULL);

					SetTimer(period_, due_);
				}
			};

		}
	}
}



#endif