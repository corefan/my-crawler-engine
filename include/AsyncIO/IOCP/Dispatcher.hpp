#ifndef __IOCP_DISPATCHER_HPP
#define __IOCP_DISPATCHER_HPP


#include "IOCP.hpp"
#include "AsyncResult.hpp"
#include "WinException.hpp"
#include "../../MultiThread/Tls.hpp"

#include <vector>
#include <type_traits>


namespace async
{

	namespace iocp 
	{
		

		// ��ȡ�ʺ�ϵͳ���߳���
		inline size_t GetFitThreadNum(size_t perCPU = 2)
		{
			SYSTEM_INFO systemInfo = {0};
			::GetSystemInfo(&systemInfo);

			return perCPU * systemInfo.dwNumberOfProcessors + 2;
		}



		//------------------------------------------------------------------
		// class IODispatcher
		// ��ɶ˿�ʵ��

		class IODispatcher
		{
		public:
			// �߳���������
			typedef std::vector<HANDLE>			Threads;
			typedef Threads::const_iterator		ThreadsConstIter;
			typedef std::tr1::function<void()>	InitCallback;
			typedef std::tr1::function<void()>	UninitCallback;

		private:
			// iocp Handle
			Iocp iocp_;
			// �߳�����
			std::vector<HANDLE>	threads_;
			// �̴߳������ʼ������
			InitCallback initCallback_;
			// �߳��˳�ʱ��������
			UninitCallback unInitCallback_;

		public:
			explicit IODispatcher(size_t numThreads = GetFitThreadNum(), const InitCallback &init = 0, const UninitCallback &unint = 0);
			~IODispatcher();

		public:
			// ���豸����ɶ˿�
			void Bind(HANDLE);
			// ����ɶ˿�Ͷ������
			template<typename HandlerT>
			void Post(const HandlerT &handler);
			// ���������̳߳���ʱ����������з���
			template<typename HandlerT>
			void Dispatch(const HandlerT &handler);

			// ֹͣ����
			void Stop();

		private:
			void _ThreadIO();

		private:
			static size_t WINAPI _ThreadIOImpl(LPVOID);
		};


		template < typename HandlerT >
		void IODispatcher::Post(const HandlerT &handler)
		{
			AsyncCallbackBasePtr async(MakeAsyncCallback<AsyncCallback>(handler));

			if( !iocp_.PostStatus(0, 0, static_cast<OVERLAPPED *>(async.Get())) )
				throw Win32Exception("iocp_.PostStatus");

			async.Release();
		}

		template < typename HandlerT >
		void IODispatcher::Dispatch(const HandlerT &handler)
		{
			if( async::thread::CallStack<IODispatcher>::Contains(this) )
			{
				AsyncCallbackBasePtr async(MakeAsyncCallback<AsyncCallback>(handler));
				//AsyncCallbackBase::Call(0, async.Get(),);

				async.Release();
			}
			else
				Post(handler);
		}
	}
}



#endif