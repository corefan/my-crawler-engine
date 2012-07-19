#ifndef __IOCP_IOCP_HPP
#define __IOCP_IOCP_HPP

#include "../Basic.hpp"
#include <cassert>



namespace async
{

	namespace iocp
	{
		//--------------------------------------------------------------
		// class IOCP

		class Iocp
		{
		private:
			HANDLE m_hIOCP;

		public:
			Iocp()
				: m_hIOCP(NULL)
			{}
			~Iocp()
			{
				Close();
			}

		public:
			bool IsOpen() const
			{
				return m_hIOCP != NULL;
			}

			void Close()
			{
				if( m_hIOCP != NULL )
				{
					BOOL bRes = ::CloseHandle(m_hIOCP);
					m_hIOCP = NULL;
				}
			}

			bool Create(size_t nMaxConcurrency)
			{
				m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, nMaxConcurrency);

				assert(m_hIOCP != NULL);

				return m_hIOCP != NULL;
			}

			bool AssociateDevice(HANDLE hDevice, ULONG_PTR ulCompKey)
			{
				assert(m_hIOCP != NULL);

				return ::CreateIoCompletionPort(hDevice, m_hIOCP, ulCompKey, 0) == m_hIOCP;
			}

			bool PostStatus(ULONG_PTR ulCompKey, DWORD dwNumBytes = 0, OVERLAPPED *pOver = NULL)
			{
				BOOL bOk = ::PostQueuedCompletionStatus(m_hIOCP, dwNumBytes, ulCompKey, pOver);

				return bOk == TRUE;
			}

			bool GetStatus(ULONG_PTR *pCompKey, PDWORD pdwNumBytes, OVERLAPPED **pOver, DWORD dwMilliseconds = INFINITE)
			{
				return TRUE == ::GetQueuedCompletionStatus(m_hIOCP, pdwNumBytes, pCompKey, pOver, dwMilliseconds);
			}

			operator HANDLE()
			{
				return m_hIOCP;
			}

			operator const HANDLE() const
			{
				return m_hIOCP;
			}
		};


	}

}

#endif