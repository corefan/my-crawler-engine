#ifndef __THREAD_THREADPOOL_HPP
#define __THREAD_THREADPOOL_HPP

#pragma warning(disable:4100)	// ����δ�����βξ���



namespace async
{
	
	/*
		�̳߳ص����������Ϊ���ԣ�

							Timer						RegisterWait					BindIO							QueueUser
	
		�̵߳ĳ�ʼ��ֵ		����1						0								0								0

		������һ���߳�ʱ		�����õ�һ���̳߳غ���ʱ		ÿ63��ע�����һ���߳�				ϵͳ��ʹ����̽��������������Ӱ�죺
																							1. �Դ�����̺߳��Ѿ���ȥһ��ʱ��(��λms)
																							2. ʹ��WT_EXECUTELONGFUNCTION
																							3. �Ѿ��ŶӵĹ�����Ŀ������������ĳ����ֵ

		���̱߳�����ʱ		��������ֹʱ					���Ѿ�ע��ĵȴ���������Ϊ0		���߳�û��δ�����IO����
																						�����Ѿ�������һ����ֵ������		���߳̿�����һ����ֵ������
	
		�߳���εȴ�			�����ȴ�						WaitForMultipleObjects			�����ȴ�							GetQueuedCompletionStatus

		ʲô�����߳�			�ȴ���ʱ��֪ͨ�Ŷӵ�APC		�ں˶����Ϊ��֪ͨ״̬				�Ŷӵ��û�APC������ɵ�IO����		չʾ����ɵ�״̬��IO��ʾ
		



	*/

	


	/*
		WT_EXECUTEDEFAULT-- Ĭ������£��ûص��������Ŷӵ��� i/o �����̡߳���ͨ��IO��ɶ˿�ʵ�ֵ�
		�ص��������Ŷӣ�����ζ�����ǲ���ִ��һ�� alertable �ĵȴ� I/O ��ɶ˿�ʹ�õ��̡߳� 
		��ˣ������� I/O ������ APC���������ڵȴ���Ϊ���ܱ�֤���߳��ڻص���ɺ󽫽��� alertable �ȴ�״̬��

		WT_EXECUTEINIOTHREAD--�ص�������һ�� I/O �����߳����Ŷӡ� ���߳̽�ִ��һ�� alertable �ĵȴ��� 
		����Ч�ʽϵͣ���ˣ������ص�����ǰ�߳����� apc �ͺ���̷߳��ص��̳߳أ���Ӧִ�� APC ʱ����Ӧʹ�ô˱�־�� 
		�ص���������Ϊ APC �Ŷӡ� ����ú���ִ�� alertable �ȴ�����������ؽ�������������⡣

		WT_EXECUTEINPERSISTENTTHREAD--�ص��������Ŷӵ���Զ������ֹ���̡߳� �����ܱ�֤��ͬһ���߳���ÿ��ʹ�á�
		�˱�־Ӧ�����ڶ����񣬻������ܻ�Ӱ��������ʱ�������� 
		��ע�⵱ǰû�й��������߳��������־ã���������κι��� I/O ���󣬲�����ֹ�����̡߳�

		WT_EXECUTELONGFUNCTION--�ص���������ִ��һ����ʱ��ĵȴ��� �˱�־���԰������������Ӧ����һ�����̵߳�ϵͳ��

		WT_TRANSFER_IMPERSONATION--�ص�������ʹ���ڵ�ǰ�ķ������ƣ�����һ�����̻���ģ�����ơ�
		���δָ���˱�־����ص�������ִ�н������ơ�

	*/

	namespace thread 
	{
		enum { MAX_THREADS = 10 };

		// ��������̳߳�����
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


			// handler ������
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
			WT_EXECUTEDEFAULT				�÷�IO������߳�����������Ŀ
			WT_EXECUTEINIOTHREAD			�����Ҫ��ĳ��ʱ�䷢��һ���첽IO����
			WT_EXECUTEPERSISTENTTHREAD		�����Ҫ��һ����������ֹ���е��߳�������ù�����Ŀ
			WT_EXECUTELONGFUNCTION			�����Ϊ������Ŀ��Ҫ�ܳ�ʱ��������


			WT_EXECUTEDEFAULT
			0x00000000 Ĭ������£��ص��������Ŷӵ��� i/o �����̡߳�

			WT_EXECUTEINTIMERTHREAD
			0x00000020 �ɱ���ļ�ʱ���̵߳��ûص������� �˱�־Ӧ�����ڶ����񣬻������ܻ�Ӱ��������ʱ�������� �ص���������Ϊ APC �Ŷӡ� ����Ӧִ�б����Ȳ�����

			WT_EXECUTEINIOTHREAD
			0x00000001 �ص��������Ŷ���һ�� I/O �����̡߳� ����ú���Ӧִ���̵߳ȴ�����״̬��ʱ����Ӧʹ�ô˱�־�� �ص���������Ϊ APC �Ŷӡ� ����ú���ִ�б����Ȳ�����������ؽ�������������⡣

			WT_EXECUTEINPERSISTENTTHREAD
			0x00000080 �ص��������Ŷӵ���Զ������ֹ���̡߳� �������ܱ�֤��ͬһ�߳���ÿ��ʹ�á� �˱�־Ӧ�����ڶ����񣬻������ܻ�Ӱ��������ʱ�������� ��ע��Ŀǰû�й��������߳��������־���Ȼû�й��������߳̽���ֹ������κι���� I/O ����

			WT_EXECUTELONGFUNCTION
			0x00000010 �ص���������ִ��һ����ʱ��ĵȴ��� �˱�־�ɰ���ϵͳ�������Ƿ���Ӧ����һ���µ��̡߳�

			WT_EXECUTEONLYONCE
			0x00000008 ��ʱ����һ��ֻ������Ϊ�ѷ����ź�״̬�� ��������˴˱�־ �� ��������Ϊ�㡣

			WT_TRANSFER_IMPERSONATION
			0x00000100 �ص�������ʹ�õ�ǰ�ķ������ƣ������ǽ��̻�ģ�����ơ� �����ָ���˱�־����ص�����ִֻ�н������ơ�

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

			// �ı䶨ʱ���ĵ���ʱ��͵�������
			bool Change(DWORD dwDueTime, DWORD dwPeriod)
			{
				assert(newTimer_ != NULL);
				assert(callback_);
				return TRUE == ::ChangeTimerQueueTimer(NULL, newTimer_, dwDueTime, dwPeriod);
			}

			// ��Ӧ��ΪhCompletionEvent����INVALID_HANDLE_VALUE
			// ����Ǵ��ݵ�NULL����ú������Ϸ���
			// ����Ǵ��ݵ�һ���¼������������Ϸ��أ����ҵ���ʱ�������Ѿ��ŶӵĹ�����Ŀ��ɺ󣬻ᴥ�����¼�(��Ӧ����Ϊ�������¼�)
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
			Ͷ��һ���첽IO����,��IO��ɶ˿��ϣ��ص�����Ҳ�����̳߳��߳���ִ��

			������Ӧ�ó��򷢳�ĳЩ�첽IO���󣬵���Щ�������ʱ����Ҫ��һ���̳߳�׼��������������ɵ�IO����
			BindIoCompletionCallback���ڲ�����CreateIoCompletionPort������hDevice���ڲ���ɶ˿ڵľ����
			���øú������Ա�֤������һ���߳�ʼ���ڷ�IO����У����豸��ص���ɼ����ص�������̵ĵ�ַ��
			�����������豸��IO�������ʱ����IO�����֪�������ĸ��������Ա��ܹ����������IO����


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
			Ĭ������£��ûص��������Ŷӵ��� i/o �����̡߳�

			WT_EXECUTEINIOTHREAD	0x00000001 
			�ص�������һ�� I/O �����߳����Ŷӡ� ����ú���Ӧ�ȴ� alertable ״̬���߳���ִ�У�Ӧʹ�ô˱�־�� 
			�ص���������Ϊ APC �Ŷӡ� ����ú���ִ�� alertable �ȴ�������������ؽ�������������⡣

			WT_EXECUTEINPERSISTENTTHREAD 0x00000080 
			�ص��������Ŷӵ���Զ������ֹ���̡߳� �����ܱ�֤��ͬһ���߳���ÿ��ʹ�á� �˱�־Ӧ�����ڶ����񣬻������ܻ�Ӱ�������ȴ������� 
			��ע�⵱ǰû�й��������߳��������־õ���Ȼû�й��������̣߳�������κι��� I/O �������ֹ��

			WT_EXECUTEINWAITTHREA 0x00000004 
			�ɱ���ĵȴ��̵߳��ûص������� �˱�־Ӧ�����ڶ����񣬻������ܻ�Ӱ�������ȴ������� 
			�����һ���̻߳�ȡ�������͵��ûص�����ʱ�� UnregisterWait �� UnregisterWaitEx ��������ͼ��ȡ��ͬ�������ͻᷢ��������

			WT_EXECUTELONGFUNCTION	0x00000010 
			�ص���������ִ��һ����ʱ��ĵȴ��� �˱�־���԰������������Ӧ����һ�����̵߳�ϵͳ��

			WT_EXECUTEONLYONCE	0x00000008 
			�ص���������һ�θ��߳̽����ٵȴ��þ���� ����ÿ�εȴ��������֮ǰ�ȴ�������ȡ���������ü�ʱ����

			WT_TRANSFER_IMPERSONATION	0x00000100 
			�ص�������ʹ���ڵ�ǰ�ķ������ƣ�����һ�����̻���ģ�����ơ� ���δָ���˱�־����ص�������ִ�н������ơ�

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
			// ȡ��
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