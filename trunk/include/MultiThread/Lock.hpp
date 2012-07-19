#ifndef __THREAD_AUTO_LOCK_HPP
#define __THREAD_AUTO_LOCK_HPP

#include <cassert>
#include <limits>

#ifdef max
#undef max 
#endif

namespace async
{

	namespace thread
	{
		//-------------------------------------------------------
		// AutoLock

		template<typename T>
		class AutoLock
		{
		private:
			T &m_lock;

		public:
			explicit AutoLock(T &lock)
				: m_lock(lock)
			{
				m_lock.Lock();
			}
			~AutoLock()
			{
				m_lock.Unlock();
			}

		private:
			AutoLock(const AutoLock &);
			AutoLock &operator=(const AutoLock &);
		};


		// ------------------------------------------------
		// class AutoLockPtr

		// wrapper for a volatile pointer

		template < typename T, typename MutexT >
		class AutoLockPtr
		{
			T *obj_;
			MutexT &lock_;

		public:
			AutoLockPtr(T &obj, MutexT &mutex)
				: obj_(&obj)
				, lock_(mutex)
			{
				lock_.lock();
			}
			~AutoLockPtr()
			{
				lock_.unlock();
			}

			T &operator*() const
			{
				return *obj_;
			}

			T *operator->() const
			{
				return obj_;
			}
		};

		//-------------------------------------------------------
		// AutoNull

		class AutoNull
		{
		public:
			void Lock(){}
			void Unlock(){}
		};



		//-------------------------------------------------------
		// AutoSpinLock

		class AutoSpinLock
		{
		private:
			CRITICAL_SECTION mutex_;

		public:
			AutoSpinLock(unsigned long dwCount = 4000) 
			{
				::InitializeCriticalSectionAndSpinCount(&mutex_, dwCount);	// MS 
			}

			~AutoSpinLock() 
			{
				::DeleteCriticalSection(&mutex_);
			}

		public:
			void Lock() 
			{
				::EnterCriticalSection(&mutex_);
			}

			void Unlock() 
			{
				::LeaveCriticalSection(&mutex_);
			}

			operator CRITICAL_SECTION&()
			{
				return mutex_;
			}

			operator const CRITICAL_SECTION &() const
			{
				return mutex_;
			}
		};


		//-------------------------------------------------------
		// AutoCriticalSection

		class AutoCriticalSection
		{
		private:
			CRITICAL_SECTION mutex_;

		public:
			AutoCriticalSection() 
			{
				::InitializeCriticalSection(&mutex_);
			}

			~AutoCriticalSection() 
			{
				::DeleteCriticalSection(&mutex_);
			}

		public:
			void Lock() 
			{
				::EnterCriticalSection(&mutex_);
			}

			void Unlock() 
			{
				::LeaveCriticalSection(&mutex_);
			}

			operator CRITICAL_SECTION&()
			{
				return mutex_;
			}

			operator const CRITICAL_SECTION &() const
			{
				return mutex_;
			}
		};


		//-------------------------------------------------------
		// class RWLock

		template < bool IsShared >
		class RWLock
		{
			template < bool IsShared >
			struct SelectLock;

			template <>
			struct SelectLock<true>
			{
				RWLock &rwLock_;
				SelectLock(RWLock &lock)
					: rwLock_(lock)
				{}

				void Lock()
				{
					rwLock_.RLock();
				}
				void Unlock()
				{
					rwLock_.RUnlock();
				}
			};

			template <>
			struct SelectLock<false>
			{
				RWLock &rwLock_;
				SelectLock(RWLock &lock)
					: rwLock_(lock)
				{}

				void Lock()
				{
					rwLock_.WLock();
				}

				void Unlock()
				{
					rwLock_.WUnlock();
				}
			};

		private:
			SRWLOCK rwLock_;

		public:
			RWLock()
			{
				::InitializeSRWLock(&rwLock_);
			}
			~RWLock()
			{
				// nothing
			}

		public:
			// 共享模式
			void RLock()
			{
				::AcquireSRWLockShared(&rwLock_);
			}
			// 独占模式
			void WLock()
			{
				::AcquireSRWLockExclusive(&rwLock_);
			}

			void RUnlock()
			{
				::ReleaseSRWLockShared(&rwLock_);
			}
			void WUnlock()
			{
				::ReleaseSRWLockShared(&rwLock_);
			}

		public:
			void Lock()
			{
				SelectLock<IsShared>().Lock();
			}

			void Unlock()
			{
				SelectLock<IsShared>().Unlock();
			}
		};

		typedef RWLock<true> SharedRWLock;
		typedef RWLock<false> ExclusiveRWLock;


		//-------------------------------------------------------
		// AutoMutex

		class AutoMutex
		{
		private:
			HANDLE mutex_;

		public:
			AutoMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes = NULL, BOOL bInitialOwner = FALSE, LPCTSTR lpName = NULL)
				: mutex_(NULL)
			{
				mutex_ = ::CreateMutex(lpMutexAttributes, bInitialOwner, lpName);
			}
			~AutoMutex()
			{
				if( mutex_ != NULL )
					::CloseHandle(mutex_);
			}

		public:
			void Lock()
			{
				::WaitForSingleObject(mutex_, INFINITE);
			}

			void Unlock()
			{
				::ReleaseMutex(mutex_);
			}

			operator HANDLE &()
			{
				return mutex_;
			}
			operator const HANDLE &() const
			{
				return mutex_;
			}
		};

		//-------------------------------------------------------
		// AutoEvent

		class AutoEvent
		{
		public:
			HANDLE m_hEvent;

			AutoEvent(HANDLE hEvent = 0) 
				: m_hEvent(hEvent)
			{ 
			}

			~AutoEvent()
			{
				Close();
			}

			bool Create(LPCTSTR pstrName = NULL, BOOL bManualReset = FALSE, BOOL bInitialState = FALSE, LPSECURITY_ATTRIBUTES pEventAttributes = NULL)
			{
				assert(pstrName==NULL || !::IsBadStringPtr(pstrName,(UINT)-1));
				assert(m_hEvent==0);

				m_hEvent = ::CreateEvent(pEventAttributes, bManualReset, bInitialState, pstrName);
				assert(m_hEvent!=0);

				return m_hEvent != 0;
			}

			bool Open(LPCTSTR pstrName, DWORD dwDesiredAccess = EVENT_ALL_ACCESS, BOOL bInheritHandle = TRUE)
			{
				assert(!::IsBadStringPtr(pstrName,(UINT)-1));
				assert(m_hEvent==0);

				m_hEvent = ::OpenEvent(dwDesiredAccess, bInheritHandle, pstrName);

				return m_hEvent != NULL;
			}

			bool IsOpen() const
			{
				return m_hEvent != 0;
			}

			void Close()
			{
				if( m_hEvent == 0 ) 
					return;

				::CloseHandle(m_hEvent);
				m_hEvent = 0;
			}

			void Attach(HANDLE hEvent)
			{
				assert(m_hEvent==0);
				m_hEvent= hEvent;
			}  

			HANDLE Detach()
			{
				HANDLE hEvent = m_hEvent;
				m_hEvent = 0;
				return hEvent;
			}

			bool ResetEvent()
			{
				assert(m_hEvent!=0);
				return ::ResetEvent(m_hEvent) != 0;
			}

			bool SetEvent()
			{
				assert(m_hEvent!=0);
				return ::SetEvent(m_hEvent) != 0;
			}

			bool PulseEvent()
			{
				assert(m_hEvent!=0);
				return ::PulseEvent(m_hEvent) != 0;
			}

			bool IsSignalled() const
			{
				assert(m_hEvent!=0);
				return ::WaitForSingleObject(m_hEvent, 0) == WAIT_OBJECT_0;
			}

			bool WaitForEvent(DWORD dwTimeout = INFINITE)
			{
				assert(m_hEvent!=0);
				return ::WaitForSingleObject(m_hEvent, dwTimeout) == WAIT_OBJECT_0;
			}

			operator HANDLE&()
			{ 
				return m_hEvent; 
			}

			operator const HANDLE &() const 
			{ 
				return m_hEvent; 
			}
		};


		namespace detail
		{
			// a semaphore object usually can be considered as an event set.

			class Semaphore
			{
			private:
				HANDLE semaphore_;
				// 需要指定状态个数
				volatile long signalCount_;

			public:
				Semaphore(long initCount = 0, long maxCount = std::numeric_limits<long>::max())
				{
					semaphore_		= ::CreateSemaphore( NULL, initCount, maxCount, NULL );
					::InterlockedExchange(&signalCount_, initCount);
				}

				~Semaphore()
				{
					::CloseHandle(semaphore_);
				}

				// 设置状态为激活
				void Signal(long count)
				{
					if( count <= 0 ) 
						return ;

					::ReleaseSemaphore(semaphore_, count, 0);
					::InterlockedExchangeAdd(&signalCount_, count);
				}

				// 等待状态为激活
				// 可以通过Signal来激活状态
				template < typename MutexT >
				bool Wait(MutexT &mutex, DWORD waitTime)
				{
					mutex.Unlock();

					DWORD ret = ::WaitForSingleObject(semaphore_, waitTime);
					::InterlockedDecrement(&signalCount_);

					mutex.Lock();

					return ret == WAIT_OBJECT_0;
				}

				// 重置状态为非激活态
				void Reset()
				{
					for(long i = 0; i < signalCount_; ++i)
					{
						::WaitForSingleObject( semaphore_, INFINITE );
					}

					::InterlockedExchange(&signalCount_, 0);
				}


			};

			// ------------------------------------------------------------
			// class MultipleEvent

			template< long _Size = 10 >
			class MultipleEvent
			{
				typedef AutoCriticalSection Mutex;
				typedef AutoLock<Mutex>		AutoLock;

			private:
				// event array
				HANDLE events_[_Size];
				// state pointer
				long pos_;
				// Mutex
				Mutex mutex_;

			public:
				MultipleEvent()
				{
					for(long i = 0; i != _Size; ++i)
					{
						events_[i] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
					}
					pos_ = -1;
				}

				// destructor
				~MultipleEvent()
				{
					for(long i = 0; i != _Size; ++ i)
					{
						::CloseHandle(events_[i]);
					}
				}

				// 设置多少个event为激活状态
				void Signal(long count)
				{
					if( count <= 0 )
						return;

					AutoLock lock(mutex_);
					long max = pos_ + count >= _Size ? _Size - 1 : pos_ + count;
					for(long i = pos_ + 1; i <= max; ++ i)
					{
						::SetEvent(events_[i]);
					}

					pos_ += (max - pos_);
				}

				// 等待一个event
				template < typename MutexT >
				bool Wait(MutexT &mutex, DWORD waitTime)
				{
					mutex.Unlock();

					DWORD ret = 0;
					{
						AutoLock lock(mutex_);
						long waitPos = pos_ < 0 ? 0 : pos_;
						ret = ::SignalObjectAndWait(events_[waitPos], events_[waitPos], waitTime, FALSE);
						pos_ = waitPos == 0 ? pos_ : pos_ - 1;
					}
					

					mutex.Lock();
					return ret == WAIT_FAILED;
				}

				// 重置所有事件为非激活状态
				void Reset()
				{
					for(long i = 0; i <= pos_; ++i)
					{
						::ResetEvent(events_[i]);
					}

					pos_ = -1;
				}
			};

			class Condition
			{
				CONDITION_VARIABLE condition_;

			public:
				Condition()
				{
					::InitializeConditionVariable(&condition_);
				}

				void Signal(long count)
				{
					if( count == 1 )
						::WakeConditionVariable(&condition_);
					else
						::WakeAllConditionVariable(&condition_);
				}

				template < typename MutexT >
				bool Wait(MutexT &mutex, DWORD waitTime)
				{
					return _Wait(mutex, waitTime);
				}

				void Reset()
				{
					// do nothing
				}

			private:
				bool _Wait(CRITICAL_SECTION &mutex, DWORD waitTime)
				{
					return ::SleepConditionVariableCS(&condition_, &mutex, waitTime) == TRUE;
				}

				bool _Wait(SRWLOCK &mutex, DWORD waitTime)
				{
					return ::SleepConditionVariableSRW(&condition_, &mutex, waitTime, CONDITION_VARIABLE_LOCKMODE_SHARED) == TRUE;
				}
			};
		}



		// -----------------------------------------------------
		// class Condition

		template< typename MultipleEventT >
		class ConditionT
		{
		public:
			typedef MultipleEventT				MultipleEventType;

		private:
			// 事件
			MultipleEventType events_;
			// 需要等待的个数
			volatile long waitCount_;

		public:
			ConditionT() 
				: waitCount_(0)
			{}

			// set the Condition state to the signaled state
			void Signal()
			{
				if( ::InterlockedCompareExchange(&waitCount_, 0, 0) != 0 )
				{
					events_.Signal( 1 );
					::InterlockedDecrement(&waitCount_);
				}
			}

			// wait the Condition until it 's signaled state
			template < typename MutexT >
			bool Wait(MutexT &mutex, DWORD waitTime = INFINITE)
			{
				::InterlockedIncrement(&waitCount_);

				return events_.Wait(mutex, waitTime);
			}

			// 设置所有event为有信号状态
			void Broadcast()
			{

				if( ::InterlockedCompareExchange(&waitCount_, 0, 0) != 0 )
				{
					events_.Signal(waitCount_);
					::InterlockedExchange(&waitCount_, 0);
				}
			}

			// 重置所有event为非激活状态
			void Reset()
			{
				events_.Reset();
			}
		};

		typedef ConditionT<detail::MultipleEvent<>>	EventCondition;
		typedef ConditionT<detail::Semaphore>		SemaphoreCondition;
		typedef ConditionT<detail::Condition>		VistaCSCondtion;
		typedef ConditionT<detail::Condition>		VistaRWCondtion;

	}

}

#endif