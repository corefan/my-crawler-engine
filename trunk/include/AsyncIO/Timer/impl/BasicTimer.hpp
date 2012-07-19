#ifndef __TIMER_BASIC_TIMER_HPP
#define __TIMER_BASIC_TIMER_HPP

#include "../../iocp/Dispatcher.hpp"

#include "TimerService.hpp"


namespace async
{
	namespace timer
	{
		namespace impl
		{
		
			// ---------------------------------------
			// class BasicTimer

			template<typename ImplT, typename ServiceT>
			class BasicTimer
			{
				typedef TimerService<ImplT, ServiceT>				TimerServiceType;
				typedef typename TimerServiceType::ServiceType		ServiceType;
				typedef typename TimerServiceType::TimerPointer		TimerPointer;

			private:
				TimerServiceType &service_;		// service
				TimerPointer timer_;			// Timer指针

			public:
				explicit BasicTimer(ServiceType &io)
					: service_(TimerServiceType::GetInstance(io))
					, timer_(service_.AddTimer(0, 0, 0))
	
				{}
				// 接受回调函数，且注册一个Timer
				template<typename HandlerT>
				BasicTimer(ServiceType &io, long period, long due, const HandlerT &handler)
					: service_(TimerServiceType::GetInstance(io))
					, timer_(service_.AddTimer(period, due, handler))
				{}
				~BasicTimer()
				{
					assert(timer_);
					service_.EraseTimer(timer_);
				}

			private:
				BasicTimer(const BasicTimer &);
				BasicTimer &operator=(const BasicTimer &);

			public:
				// 设置时间间隔
				// period 时间间隔
				// delay 延迟时间
				void SetTimer(long period, long delay = 0)
				{
					assert(timer_);
					timer_->SetTimer(period, delay);
				}

				// 取消Timer
				void Cancel()
				{
					assert(timer_);
					timer_->Cancel();
				}

				// 同步等待
				void SyncWait()
				{
					timer_->SyncWait();
				}
				template<typename HandlerT>
				void SyncWait(const HandlerT &handler, long period, long delay = 0)
				{
					SetTimer(period, delay);
					service_.SetTimer(timer_, handler);
					timer_->SyncWait();
				}

				// 异步等待
				void AsyncWait()
				{
					assert(timer_);
					timer_->AsyncWait();
				}

				template<typename HandlerT>
				void AsyncWait(const HandlerT &handler, long period, long delay = 0)
				{
					SetTimer(period, delay);
					service_.SetTimer(timer_, handler);
					timer_->AsyncWait();
				}
				
				void ExpireseAt()
				{
					
				}
			};
		}
	}
}


#endif