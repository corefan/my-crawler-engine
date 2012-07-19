#include "Dispatcher.hpp"
#include "WinException.hpp"
#include "../../MultiThread/Tls.hpp"

#include "process.h"

namespace async
{
	
	namespace iocp
	{

		IODispatcher::IODispatcher(size_t numThreads/* = 0*/, const InitCallback &init/* = 0*/, const UninitCallback &unint/* = 0*/)
			: initCallback_(init)
			, unInitCallback_(unint)
		{
			if( !iocp_.Create(numThreads) )
				throw Win32Exception("iocp_.Create()");

			try
			{
				// 创建指定的线程数
				threads_.reserve(numThreads);

				for(int i = 0; i != numThreads; ++i)
				{
					HANDLE hThread = (HANDLE)::_beginthreadex(NULL, 0, &IODispatcher::_ThreadIOImpl, this, 0, 0);

					if( hThread == NULL )
						throw Win32Exception("_beginthreadex");

					threads_.push_back(hThread);
				}
			}
			catch(const std::exception &e)
			{
				Stop();
				throw e;
			}
		}

		IODispatcher::~IODispatcher()
		{
			try
			{		
				Stop();
				iocp_.Close();
			}
			catch(...)
			{
				std::cerr << "Unknown error!" << std::endl;
			}
		}


		void IODispatcher::Bind(HANDLE hHandle)
		{
			if( !iocp_.AssociateDevice(hHandle, 0) )
				throw Win32Exception("iocp_.AssociateDevice");
		}


		void IODispatcher::Stop()
		{
			// 取消所有线程上的待决的IO操作
			std::for_each(threads_.begin(), threads_.end(), std::ptr_fun(::CancelIo));

			// 先停止所有的线程
			for(ThreadsConstIter iter = threads_.begin(); iter != threads_.end(); ++iter)
			{
				iocp_.PostStatus(0, 0, NULL);
			}

			// 等待线程退出并关闭句柄
			if( !threads_.empty() )
				::WaitForMultipleObjects(threads_.size(), &*threads_.begin(), TRUE, INFINITE);

			std::for_each(threads_.begin(), threads_.end(), std::ptr_fun(::CloseHandle));

			threads_.clear();
		}

		void IODispatcher::_ThreadIO()
		{
			thread::CallStack<IODispatcher>::Context ctx(this);

			DWORD dwSize			= 0;
			ULONG_PTR uKey			= 0;
			OVERLAPPED *pOverlapped = 0;

			ULONG_PTR *key			= &uKey;

			while(true)
			{
				::SetLastError(0);
				bool bSuc = iocp_.GetStatus(reinterpret_cast<ULONG_PTR *>(&key), &dwSize, &pOverlapped);
				u_long err = ::GetLastError();

				// 请求退出
				if( key == 0 && pOverlapped == 0 )
					break;

				try
				{
					// 回调
					AsyncCallbackBase::Call(key, pOverlapped, dwSize, err);
				}
				catch(const std::exception &e)
				{
					std::cerr << e.what() << std::endl;
					assert(0);
					// Opps!!
				}
				catch(...)
				{
					assert(0);
					// Opps!!
				}
				
			}
		}


		size_t IODispatcher::_ThreadIOImpl(LPVOID pParam)
		{
			IODispatcher *pThis = reinterpret_cast<IODispatcher *>(pParam);

			if( pThis->initCallback_ != 0 )
				pThis->initCallback_();

			pThis->_ThreadIO();

			if( pThis->unInitCallback_ != 0 )
				pThis->unInitCallback_();

			::OutputDebugStringW(L"OVERLAPPED Thread Exit\n");
			return 0;
		}


	}

}