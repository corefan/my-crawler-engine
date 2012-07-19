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
				TimerPointer timer_;			// Timerָ��

			public:
				explicit BasicTimer(ServiceType &io)
					: service_(TimerServiceType::GetInstance(io))
					, timer_(service_.AddTimer(0, 0, 0))
	
				{}
				// ���ܻص���������ע��һ��Timer
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
				// ����ʱ����
				// period ʱ����
				// delay �ӳ�ʱ��
				void SetTimer(long period, long delay = 0)
				{
					assert(timer_);
					timer_->SetTimer(period, delay);
				}

				// ȡ��Timer
				void Cancel()
				{
					assert(timer_);
					timer_->Cancel();
				}

				// ͬ���ȴ�
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

				// �첽�ȴ�
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