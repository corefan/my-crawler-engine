#ifndef __TIMER_TIMER_SERVICE_HPP
#define __TIMER_TIMER_SERVICE_HPP

#include <unordered_map>

#include "../../iocp/Dispatcher.hpp"

#include "../../MultiThread/Thread.hpp"




namespace async
{
	namespace timer
	{
		namespace impl
		{
			
			// alterable IO
			inline void WINAPI APCFunc(ULONG_PTR pParam)
			{
				// do nothing
			}

			namespace detail
			{
				template < typename T >
				struct FindTimer
				{
					const T &timer_;

					FindTimer(const T &timer)
						: timer_(timer)
					{}

					template < typename CallbackT >
					bool operator()(const std::pair<T, CallbackT> &val) const
					{
						return val.first == timer_;
					}
				};
			}
			// ------------------------------------------------
			// class TimerService

			template<typename TimerT, typename ServiceT>
			class TimerService
			{
			public:
				typedef TimerT							TimerType;
				typedef std::tr1::shared_ptr<TimerType>	TimerPointer;
				typedef ServiceT						ServiceType;

			private:
				typedef iocp::CallbackType				TimerCallback;
				typedef std::map<TimerPointer, TimerCallback>	Timers;
				typedef typename Timers::iterator				TimersIter;
				typedef async::thread::AutoCriticalSection		Mutex;
				typedef async::thread::AutoLock<Mutex>			Lock;

			private:
				Timers timers_;									// Timers

				async::thread::ThreadImplEx thread_;			// WaitForMutipleObjectEx
				async::thread::AutoEvent   update_;	
				Mutex mutex_;

				ServiceType &io_;								// Asynchronous Callback service

			private:
				TimerService(ServiceType &io)
					: io_(io)
				{
					update_.Create();

					// �����ȴ�Timer�߳�
					thread_.RegisterFunc(std::tr1::bind(&TimerService::_ThreadCallback, this));
					thread_.Start();
				}

				~TimerService()
				{
					// ������IO���˳������߳�
					::QueueUserAPC(APCFunc, thread_, NULL);

					thread_.Stop();
				}

				TimerService(const TimerService &);
				TimerService &operator=(const TimerService &);
				

			public:
				// ����
				static TimerService<TimerType, ServiceType> &GetInstance(ServiceType &io)
				{
					static TimerService<TimerType, ServiceType> service(io);
					return service;
				}

			public:
				// ����һ��Timer
				TimerPointer AddTimer(long period, long due, const TimerCallback &handler)
				{
					TimerPointer timer(new TimerType(period, due));
					
					{
						Lock lock(mutex_);
						timers_.insert(std::make_pair(timer, handler));
					}
 
					// ���ø����¼��ź�
					update_.SetEvent();

					return timer;
				}

				void SetTimer(const TimerPointer &timer, const TimerCallback &handler)
				{
					Lock lock(mutex_);
					TimersIter iter = timers_.find(timer);

					if( iter == timers_.end() )
						return;

					iter->second = handler;
				}

				void EraseTimer(const TimerPointer &timer)
				{
					{
						Lock lock(mutex_);
						TimersIter iter = timers_.find(timer);

						if( iter != timers_.end() )
							timers_.erase(iter);
					}
					

					// ���ø����¼��ź�
					update_.SetEvent();
				}

			private:
				DWORD _ThreadCallback()
				{
					std::vector<HANDLE> handles;

					while(!thread_.IsAborted())
					{
						// ����б仯��������
						if( WAIT_OBJECT_0 == ::WaitForSingleObject(update_, 0) )
						{
							_Copy(handles);
						}
						
						// ��ֹ������ʱû��timer����
						if( handles.size() == 0 )
						{
							if( WAIT_IO_COMPLETION == ::WaitForSingleObjectEx(update_, INFINITE, TRUE) )
								break;
							else
								_Copy(handles);
						}

						// �ȴ�Timer����
						DWORD res = ::WaitForMultipleObjectsEx(handles.size(), &handles[0], FALSE, INFINITE, TRUE);
						if( res == WAIT_IO_COMPLETION )
							break;

						if( WAIT_OBJECT_0 == ::WaitForSingleObject(update_, 0) )
						{
							update_.SetEvent();
							continue;
						}
						
						if( res == WAIT_FAILED )
						{
							update_.SetEvent();
							continue;
						}
						else if( res + WAIT_OBJECT_0 > timers_.size() )
							throw std::out_of_range("handle out of range");

						Lock lock(mutex_);
						TimersIter iter = timers_.begin();
						std::advance(iter, WAIT_OBJECT_0 + res);
						
						io_.Post(iter->second);
					}

					::OutputDebugString(_T("Exit Timer Service Thread\n"));
					return 0;
				}

				void _Copy(std::vector<HANDLE> &handles)
				{
					Lock lock(mutex_);
					handles.clear();

					for(TimersIter iter = timers_.begin(); iter != timers_.end(); ++iter)
					{
						handles.push_back(*(iter->first));
					}
				}
			};

			
		}
	}
}


#endif