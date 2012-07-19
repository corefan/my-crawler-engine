#ifndef __THREAD_THREADPOOL_HPP
#define __THREAD_THREADPOOL_HPP

#pragma warning(disable:4100)	// 消除未引用形参警告



namespace async
{
	
	/*
		线程池的组件及其行为特性：

							Timer						RegisterWait					BindIO							QueueUser
	
		线程的初始数值		总是1						0								0								0

		当创建一个线程时		当调用第一个线程池函数时		每63个注册对象一个线程				系统会使用试探法，有如下因素影响：
																							1. 自从添加线程后已经过去一定时间(单位ms)
																							2. 使用WT_EXECUTELONGFUNCTION
																							3. 已经排队的工作项目的数量超过了某个阀值

		当线程被撤销时		当进程终止时					当已经注册的等待对象数量为0		当线程没有未处理的IO请求
																						并且已经空闲了一个阀值的周期		当线程空闲了一个阀值的周期
	
		线程如何等待			待命等待						WaitForMultipleObjects			待命等待							GetQueuedCompletionStatus

		什么唤醒线程			等待定时器通知排队的APC		内核对象变为已通知状态				排队的用户APC和已完成的IO请求		展示已完成的状态和IO请示
		



	*/

	


	/*
		WT_EXECUTEDEFAULT-- 默认情况下，该回调函数被排队到非 i/o 工作线程。是通过IO完成端口实现的
		回调函数被排队，这意味着它们不能执行一个 alertable 的等待 I/O 完成端口使用的线程。 
		因此，如果完成 I/O 但生成 APC可能无限期等待因为不能保证该线程在回调完成后将进入 alertable 等待状态。

		WT_EXECUTEINIOTHREAD--回调函数是一个 I/O 工作线程来排队。 此线程将执行一个 alertable 的等待。 
		这是效率较低，因此，仅当回调到当前线程生成 apc 和后该线程返回到线程池，则应执行 APC 时，才应使用此标志。 
		回调函数是作为 APC 排队。 如果该函数执行 alertable 等待操作，请务必解决可重入性问题。

		WT_EXECUTEINPERSISTENTTHREAD--回调函数被排队到永远不会终止的线程。 它不能保证在同一个线程在每次使用。
		此标志应仅用于短任务，或它可能会影响其他计时器操作。 
		请注意当前没有工作程序线程是真正持久，但如果有任何挂起 I/O 请求，不会终止工作线程。

		WT_EXECUTELONGFUNCTION--回调函数可以执行一个长时间的等待。 此标志可以帮助决定如果它应创建一个新线程的系统。

		WT_TRANSFER_IMPERSONATION--回调函数将使用在当前的访问令牌，它是一个过程还是模拟令牌。
		如果未指定此标志，则回调函数仅执行进程令牌。

	*/

	namespace thread 
	{
		enum { MAX_THREADS = 10 };

		// 设置最大线程池数量
		inline void SetThreadPoolMaxThread(DWORD dwFlag, DWORD dwThreads = MAX_THREADS)
		{
			WT_SET_MAX_THREADPOOL_THREADS(dwFlag, dwThreads);
		}


		// Handler Allocate / Deallocate HOOK

		inline void *HandlerAllocateHook(size_t sz, ...)
		{
			return ::operator new(sz);
		}
		inline void HandlerDeallocateHook(size_t sz, void *p, ...)
		{
			sz;
			::operator delete(p);
		}



		namespace detail
		{
			struct ThreadParamBase
			{
				ThreadParamBase()
				{}

				virtual ~ThreadParamBase()
				{}

				void Invoke()
				{
					CallBack(FALSE);
				}
				void Invoke(BOOLEAN wait)
				{
					CallBack(wait);
				}

				virtual void CallBack(BOOLEAN wait) = 0;
			};
		
			typedef std::tr1::shared_ptr<ThreadParamBase> ThreadParamBasePtr;

			template<typename HandlerT>
			struct ThreadParam
				: public ThreadParamBase
			{
				HandlerT handler_;

				ThreadParam(const HandlerT &handler)
					: ThreadParamBase()
					, handler_(handler)
				{}

				virtual void CallBack(BOOLEAN wait)
				{
					handler_(wait);
				}
			};


			// handler 分配器
			template<typename T>
			void _HandlerDeallocate(T *p)
			{
				p->~T();
				HandlerDeallocateHook(sizeof(T), p, &p);
			}

			template<typename HandlerT>
			detail::ThreadParamBasePtr _HandlerAllocate(const HandlerT &handler)
			{
				typedef detail::ThreadParam<HandlerT> Thread;
				void *p = 0;
				p = HandlerAllocateHook(sizeof(Thread), p);
				new (p) Thread(handler);

				detail::ThreadParamBasePtr threadParam(static_cast<detail::ThreadParamBase *>(p), _HandlerDeallocate<detail::ThreadParamBase>);
				return threadParam;
			}

		}



		//----------------------------------------------------------------------------------
		// class QueueWorkItem

		class QueueWorkItemPool
		{
		private:
			detail::ThreadParamBasePtr callback_;

		public:
			QueueWorkItemPool()
			{}
			template<typename HandlerT>
			explicit QueueWorkItemPool(const HandlerT &callback)
				: callback_(detail::_HandlerAllocate(callback))
			{}

		public:
			template<typename HandlerT>
			bool Call(const HandlerT &callback, ULONG nFlags = WT_EXECUTEDEFAULT)
			{
				if( callback_ == 0 )
					callback_ = detail::_HandlerAllocate(callback);

				assert(callback_);
				return Call(nFlags);
			}

			bool Call(ULONG nFlags = WT_EXECUTEDEFAULT)
			{
				assert(callback_);
				return TRUE == ::QueueUserWorkItem(&QueueWorkItemPool::WorkerThreadProc, callback_.get(), nFlags);
			}

		private:
			static DWORD WINAPI WorkerThreadProc(LPVOID pvParam)
			{
				detail::ThreadParamBase *p(static_cast<detail::ThreadParamBase *>(pvParam));

				try 
				{
					p->Invoke();
				}
				catch(...) 
				{}

				return 0;
			}

		private:
			QueueWorkItemPool(const QueueWorkItemPool &);
			QueueWorkItemPool &operator=(const QueueWorkItemPool &);
		};



		//----------------------------------------------------------------------------------
		// class QueueTimer

		/*
			WT_EXECUTEDEFAULT				让非IO组件的线程来处理工作项目
			WT_EXECUTEINIOTHREAD			如果想要在某个时间发出一个异步IO请求
			WT_EXECUTEPERSISTENTTHREAD		如果想要让一个绝不会终止运行的线程来处理该工作项目
			WT_EXECUTELONGFUNCTION			如果认为工作项目需要很长时间来运行


			WT_EXECUTEDEFAULT
			0x00000000 默认情况下，回调函数被排队到非 i/o 工作线程。

			WT_EXECUTEINTIMERTHREAD
			0x00000020 由本身的计时器线程调用回调函数。 此标志应仅用于短任务，或它可能会影响其他计时器操作。 回调函数是作为 APC 排队。 它不应执行报警等操作。

			WT_EXECUTEINIOTHREAD
			0x00000001 回调函数被排队以一个 I/O 工作线程。 如果该函数应执行线程等待报警状态中时，才应使用此标志。 回调函数是作为 APC 排队。 如果该函数执行报警等操作，，请务必解决可重入性问题。

			WT_EXECUTEINPERSISTENTTHREAD
			0x00000080 回调函数被排队到永远不会终止的线程。 它并不能保证在同一线程在每次使用。 此标志应仅用于短任务，或它可能会影响其他计时器操作。 请注意目前没有工作程序线程是真正持久虽然没有工作程序线程将终止如果有任何挂起的 I/O 请求。

			WT_EXECUTELONGFUNCTION
			0x00000010 回调函数可以执行一个长时间的等待。 此标志可帮助系统，决定是否它应创建一个新的线程。

			WT_EXECUTEONLYONCE
			0x00000008 计时器将一次只能设置为已发送信号状态。 如果设置了此标志 期 参数必须为零。

			WT_TRANSFER_IMPERSONATION
			0x00000100 回调函数将使用当前的访问令牌，不论是进程或模拟令牌。 如果不指定此标志，则回调函数只执行进程令牌。

		*/
		class QueueTimerPool
		{
			detail::ThreadParamBasePtr callback_;
			HANDLE newTimer_;

		public:
			QueueTimerPool()
				: newTimer_(0)
			{}
			template<typename HandlerT>
			explicit QueueTimerPool(const HandlerT &handler)
				: callback_(detail::_HandlerAllocate(handler))
				, newTimer_(NULL)
			{}

			~QueueTimerPool()
			{
				if( newTimer_ != NULL )
					Cancel();

				assert(callback_);
			}

		public:
			template<typename HandlerT>
			bool Call(const HandlerT &callback, DWORD dwDueTime, DWORD dwPeriod, ULONG nFlags = WT_EXECUTEDEFAULT)
			{
				if( *callback_ == 0 )
					callback_ = detail::_HandlerAllocate(callback);

				assert(callback);
				return Call(dwDueTime, dwPeriod, nFlags);
			}

			bool Call(DWORD dwDueTime, DWORD dwPeriod, ULONG nFlags = WT_EXECUTEDEFAULT)
			{
				assert(callback_);
				return TRUE == ::CreateTimerQueueTimer(&newTimer_, NULL, 
					reinterpret_cast<WAITORTIMERCALLBACK>(&QueueTimerPool::WorkerThreadProc), 
					callback_.get(), dwDueTime, dwPeriod, nFlags);
			}

			// 改变定时器的到期时间和到期周期
			bool Change(DWORD dwDueTime, DWORD dwPeriod)
			{
				assert(newTimer_ != NULL);
				assert(callback_);
				return TRUE == ::ChangeTimerQueueTimer(NULL, newTimer_, dwDueTime, dwPeriod);
			}

			// 不应该为hCompletionEvent传递INVALID_HANDLE_VALUE
			// 如果是传递的NULL，则该函数马上返回
			// 如果是传递的一个事件句柄，则会马上返回，并且当定时器所有已经排队的工作项目完成后，会触发此事件(不应该认为触发此事件)
			bool Cancel(HANDLE hCompletionEvent = NULL)
			{
				assert(newTimer_ != NULL);
				assert(callback_);
				if( ::DeleteTimerQueueTimer(NULL, newTimer_, hCompletionEvent) )
				{
					newTimer_ = NULL;
					return true;
				}

				return false;
			}

		private:
			static void WINAPI WorkerThreadProc(PVOID pvParam, BOOL bTimeout)
			{
				detail::ThreadParamBase *p = static_cast<detail::ThreadParamBase *>(pvParam);

				try 
				{
					p->Invoke(static_cast<BOOLEAN>(bTimeout));
				}
				catch(...) 
				{}
			}

		private:
			QueueTimerPool(const QueueTimerPool &);
			QueueTimerPool &operator=(const QueueTimerPool &);
		};



		/*
			投递一个异步IO操作,在IO完成端口上，回调函数也是由线程池线程来执行

			服务器应用程序发出某些异步IO请求，当这些请求完成时，需要让一个线程池准备好来处理已完成的IO请求。
			BindIoCompletionCallback在内部调用CreateIoCompletionPort，传递hDevice和内部完成端口的句柄。
			调用该函数可以保证至少有一个线程始终在非IO组件中，与设备相关的完成键是重叠完成例程的地址。
			这样，当该设备的IO运行完成时，非IO组件就知道调用哪个函数，以便能够处理已完成IO请求


		*/

		//----------------------------------------------------------------------------------
		// class IOCompletionPool 

		class IOCompletionPool
		{
			detail::ThreadParamBasePtr callback_;

		public:
			template<typename HandlerT>
			IOCompletionPool(const HandlerT &handler)
				: callback_(detail::_HandlerAllocate(handler))
			{}

		private:
			IOCompletionPool(const IOCompletionPool &);
			IOCompletionPool &operator=(const IOCompletionPool &);

		public:
			bool Call(HANDLE hBindHandle)
			{
				return ::BindIoCompletionCallback(hBindHandle, 
					&IOCompletionPool::WorkerThreadProc, 0) == TRUE;
			}

		private:
			static void WINAPI WorkerThreadProc(DWORD dwError, DWORD dwNum, OVERLAPPED *pOverlapped)
			{
				// .... to do
			}
		};


	
		/*
			WT_EXECUTEDEFAULT	0x00000000 
			默认情况下，该回调函数被排队到非 i/o 工作线程。

			WT_EXECUTEINIOTHREAD	0x00000001 
			回调函数是一个 I/O 工作线程来排队。 如果该函数应等待 alertable 状态的线程中执行，应使用此标志。 
			回调函数是作为 APC 排队。 如果该函数执行 alertable 等待操作，，请务必解决可重入性问题。

			WT_EXECUTEINPERSISTENTTHREAD 0x00000080 
			回调函数被排队到永远不会终止的线程。 它不能保证在同一个线程在每次使用。 此标志应仅用于短任务，或它可能会影响其他等待操作。 
			请注意当前没有工作程序线程是真正持久的虽然没有工作程序线程，如果有任何挂起 I/O 请求会终止。

			WT_EXECUTEINWAITTHREA 0x00000004 
			由本身的等待线程调用回调函数。 此标志应仅用于短任务，或它可能会影响其他等待操作。 
			如果另一个线程获取排他锁和调用回调函数时的 UnregisterWait 或 UnregisterWaitEx 函数正试图获取相同的锁，就会发生死锁。

			WT_EXECUTELONGFUNCTION	0x00000010 
			回调函数可以执行一个长时间的等待。 此标志可以帮助决定如果它应创建一个新线程的系统。

			WT_EXECUTEONLYONCE	0x00000008 
			回调函数调用一次该线程将不再等待该句柄。 否则，每次等待操作完成之前等待操作被取消，是重置计时器。

			WT_TRANSFER_IMPERSONATION	0x00000100 
			回调函数将使用在当前的访问令牌，它是一个过程还是模拟令牌。 如果未指定此标志，则回调函数仅执行进程令牌。

		*/

		//----------------------------------------------------------------------------------
		// class WaitObjectPool

		class WaitObjectPool
		{
		private:
			detail::ThreadParamBasePtr callback_;
			HANDLE waitObject_;

		public:
			WaitObjectPool()
				: waitObject_(NULL)
			{}
			template<typename HandlerT>
			explicit WaitObjectPool(const HandlerT &handler)
				: callback_(detail::_HandlerAllocate(handler))
				, waitObject_(NULL)
			{}
			~WaitObjectPool()
			{

			}

		private:
			WaitObjectPool(const WaitObjectPool &);
			WaitObjectPool &operator=(const WaitObjectPool &);
			

		public:
			template<typename HandlerT>
			bool Call(const HandlerT &handler, HANDLE hObject, ULONG dwWait = INFINITE, ULONG nFlags = WT_EXECUTEDEFAULT)
			{
				if( *callback_ == 0 )
					callback_ = detail::_HandlerAllocate(handler);

				assert(*callback_);
				return Call(hObject, dwWait, nFlags);
			}

			bool Call(HANDLE hObject, ULONG dwWait = INFINITE, ULONG nFlags = WT_EXECUTEDEFAULT)
			{
				assert(callback_);
				return TRUE == ::RegisterWaitForSingleObject(&waitObject_, hObject, 
					&WaitObjectPool::WorkerThreadProc, callback_.get(), dwWait, nFlags);
			}	
			// 取消
			bool Cancel(HANDLE hCompletion = NULL)
			{
				assert(callback_);
				assert(waitObject_ != NULL);
				return TRUE == ::UnregisterWaitEx(waitObject_, hCompletion);
			}


		private:
			static void WINAPI WorkerThreadProc(PVOID pvParam, BOOLEAN bTimerOrWaitFired)
			{
				detail::ThreadParamBase *p(static_cast<detail::ThreadParamBase *>(pvParam));

				try 
				{
					p->Invoke(bTimerOrWaitFired);
				}
				catch(...) 
				{}
			}
		};

	}

}

#endif //