#include "Socket.hpp"
#include "SocketOption.hpp"

#include "../IOCP/WinException.hpp"
#include "Accept.hpp"


namespace async
{


	namespace network
	{

		Socket::Socket(AsyncIODispatcherType &io)
			: socket_(INVALID_SOCKET)
			, io_(io)
		{
			SocketProvider &provider = SocketProvider::GetSingleton(io_);
		}
		Socket::Socket(AsyncIODispatcherType &io, SOCKET sock)
			: socket_(sock)
			, io_(io)
		{
			SocketProvider &provider = SocketProvider::GetSingleton(io_);
		}
		Socket::Socket(AsyncIODispatcherType &io, int family, int type, int protocol)
			: socket_(INVALID_SOCKET)
			, io_(io)
		{
			SocketProvider &provider = SocketProvider::GetSingleton(io_);

			Open(family, type, protocol);
		}

		Socket::~Socket()
		{
			Close();
		}


		void Socket::Open(int family, int nType /* SOCK_STREAM */, int nProtocol /* IPPROTO_TCP */)
		{
			if( IsOpen() )
				throw std::logic_error("Socket already opened!");

			socket_ = ::WSASocket(family, nType, nProtocol, NULL, 0, WSA_FLAG_OVERLAPPED);
			if( socket_ == INVALID_SOCKET )
				throw Win32Exception("WSASocket");

			// 绑定到IOCP
			io_.Bind(reinterpret_cast<HANDLE>(socket_));
		}
		
		void Socket::Shutdown(int shut)
		{
			if( !IsOpen() )
				return;

			::shutdown(socket_, shut);
		}
		void Socket::Close()
		{
			if( !IsOpen() )
				return;

			::closesocket(socket_);
			socket_ = INVALID_SOCKET;
		}
		
		void Socket::Cancel()
		{
			if( !IsOpen() )
				throw std::logic_error("Socket not Open");
			else
				SocketProvider::CancelIO(socket_);
		}

		void Socket::Bind(int family/* AF_INET*/, u_short uPort /* 0 */, const IPAddress &addr /* INADDR_ANY */)
		{
			if( !IsOpen() )
				throw std::logic_error("Socket not open");

			sockaddr_in addrIn = {0};
			addrIn.sin_family		= family;
			addrIn.sin_port			= ::htons(uPort);
			addrIn.sin_addr.s_addr	= ::htonl(addr.Address());

			if( SOCKET_ERROR == ::bind(socket_, (const SOCKADDR *)&addrIn, sizeof(SOCKADDR)) )
				throw Win32Exception("bind");
		}

		void Socket::Listen(int nMax)
		{
			if( !IsOpen() )
				throw std::logic_error("Socket not open");

			if( SOCKET_ERROR == ::listen(socket_, nMax) )
				throw Win32Exception("listen");
		}

		SocketPtr Socket::Accept()
		{
			if( !IsOpen() )
				throw std::logic_error("Socket not open");

			SOCKET sock = ::accept(socket_, 0, 0);
			if( sock == INVALID_SOCKET )
				throw Win32Exception("accept");

			SocketPtr remote(MakeSocket(io_, sock));
			return remote;
		}

		void Socket::Connect(int family, const IPAddress &addr, u_short uPort)
		{
			if( !IsOpen() )
				throw std::logic_error("Socket not open");

			SOCKADDR_IN serverAddr = {0};
			serverAddr.sin_family		= family;
			serverAddr.sin_addr.s_addr	= ::htonl(addr.Address());
			serverAddr.sin_port			= ::htons(uPort);

			if( SOCKET_ERROR == ::connect(socket_, reinterpret_cast<const sockaddr *>(&serverAddr), sizeof(SOCKADDR_IN)) )
				throw Win32Exception("connect");
		}

		void Socket::DisConnect(int shut, bool bReuseSocket/* = true*/)
		{
			if( !IsOpen() )
				throw std::logic_error("Socket not open");

			::shutdown(socket_, shut);		
			
			if( bReuseSocket )
			{
				ReuseAddr reuse;
				SetOption(reuse);
			}
			else
			{
				Close();
			}
		}
		
		size_t Socket::Read(char *buf, size_t size, DWORD flag)
		{
			if( !IsOpen() )
				throw std::logic_error("Socket not open");

			WSABUF wsabuf = {0};
			wsabuf.buf = buf;
			wsabuf.len = size;

			if( wsabuf.len == 0 )
				throw std::logic_error("Buffer allocate size is zero");

			DWORD dwSize = 0;
			if( 0 != ::WSARecv(socket_, &wsabuf, 1, &dwSize, &flag, 0, 0) )
				throw Win32Exception("WSARecv");

			return dwSize;
		}

		size_t Socket::Write(const char *buffer, size_t sz, DWORD flag)
		{
			if( !IsOpen() )
				throw std::logic_error("Socket not open");

			WSABUF wsabuf = {0};
			wsabuf.buf = const_cast<char *>(buffer);
			wsabuf.len = sz;

			if( wsabuf.len == 0 )
				throw std::logic_error("Buffer size is zero");

			DWORD dwSize = 0;
			if( 0 != ::WSASend(socket_, &wsabuf, 1, &dwSize, flag, 0, 0) )
				throw Win32Exception("WSASend");

			return dwSize;
		}


		size_t Socket::SendTo(const iocp::ConstBuffer &buf, const SOCKADDR_IN *addr, DWORD flag)
		{
			if( !IsOpen() )
				throw std::logic_error("Socket not open");

			WSABUF wsabuf = {0};
			wsabuf.buf = const_cast<char *>(buf.data());
			wsabuf.len = buf.size();

			if( wsabuf.len == 0 )
				throw std::logic_error("Buffer size is zero");

			DWORD dwSize = 0;
			if( 0 != ::WSASendTo(socket_, &wsabuf, 1, &dwSize, flag, reinterpret_cast<const sockaddr *>(addr), addr == 0 ? 0 : sizeof(*addr), 0, 0) )
				throw Win32Exception("WSASendTo");

			return dwSize;
		}

		size_t Socket::RecvFrom(iocp::MutableBuffer &buf, SOCKADDR_IN *addr, DWORD flag)
		{
			if( !IsOpen() )
				throw std::logic_error("Socket not open");

			WSABUF wsabuf = {0};
			wsabuf.buf = buf.data();
			wsabuf.len = buf.size();

			if( wsabuf.len == 0 )
				throw std::logic_error("Buffer allocate size is zero");

			DWORD dwSize = 0;
			int addrLen = sizeof(*addr);

			if( 0 != ::WSARecvFrom(socket_, &wsabuf, 1, &dwSize, &flag, reinterpret_cast<sockaddr *>(addr), addr == 0 ? 0 : &addrLen, 0, 0) )
				throw Win32Exception("WSARecvFrom");

			return dwSize;
		}


		// 异步接收远程连接
		//void Socket::AsyncAccept(const SocketPtr &remoteSocket, size_t szOutSize, const iocp::CallbackType &callback)
		//{
		//	if( !IsOpen() ) 
		//		throw std::logic_error("Socket not open");

		//	if( !remoteSocket->IsOpen() )
		//		throw std::logic_error("Remote socket not open");

		//	typedef detail::AcceptorHandle<iocp::CallbackType> HookAcceptor;
		//	iocp::AutoBufferPtr acceptBuffer(MakeBuffer((sizeof(sockaddr_in) + 16) * 2 + szOutSize));
		//	AsyncCallbackBasePtr asyncResult = MakeAsyncCallback(HookAcceptor(*this, remoteSocket, acceptBuffer, callback));

		//	// 根据szOutSide大小判断，是否需要接收远程客户机第一块数据才返回。
		//	// 如果为0，则立即返回。若大于0，则接受数据后再返回
		//	DWORD dwRecvBytes = 0;
		//	if( !SocketProvider::GetSingleton(io_).AcceptEx(socket_, remoteSocket->socket_, acceptBuffer->data(), szOutSize,
		//		sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, &dwRecvBytes, asyncResult.Get()) 
		//		&& ::WSAGetLastError() != ERROR_IO_PENDING )
		//		throw Win32Exception("AcceptEx");
		//	
		//	asyncResult.Release();
		//}

		// 异步连接服务
		void Socket::AsyncConnect(const IPAddress &addr, u_short uPort, const iocp::CallbackType &callback)
		{
			if( !IsOpen() )
				throw std::logic_error("Socket not open");

			sockaddr_in localAddr		= {0};
			localAddr.sin_family		= AF_INET;

			// 很变态，需要先bind
			::bind(socket_, reinterpret_cast<const sockaddr *>(&localAddr), sizeof(localAddr));

			sockaddr_in remoteAddr		= {0};
			remoteAddr.sin_family		= AF_INET;
			remoteAddr.sin_port			= ::htons(uPort);
			remoteAddr.sin_addr.s_addr	= ::htonl(addr.Address());

			AsyncCallbackBasePtr asynResult(iocp::MakeAsyncCallback<iocp::AsyncCallback>(callback));

			if( !SocketProvider::GetSingleton(io_).ConnectEx(socket_, reinterpret_cast<SOCKADDR *>(&remoteAddr), sizeof(SOCKADDR), 0, 0, 0, asynResult.Get()) 
				&& ::WSAGetLastError() != WSA_IO_PENDING )
				throw Win32Exception("ConnectionEx");
			
			asynResult.Release();
		}

		// 异步关闭连接
		void Socket::AsyncDisconnect(const iocp::CallbackType &callback, bool bReuseSocket/* = true*/)
		{
			iocp::AsyncCallbackBasePtr asynResult(iocp::MakeAsyncCallback<iocp::AsyncCallback>(callback));

			DWORD dwFlags = bReuseSocket ? TF_REUSE_SOCKET : 0;

			if( !SocketProvider::GetSingleton(io_).DisconnectEx(socket_, asynResult.Get(), dwFlags, 0) 
				&& ::WSAGetLastError() != WSA_IO_PENDING )
				throw Win32Exception("DisConnectionEx");

			asynResult.Release();
		}

		// 异步接接收数据
		void Socket::AsyncRead(iocp::MutableBuffer &buf, const iocp::CallbackType &callback)
		{
			AsyncCallbackBasePtr asynResult(iocp::MakeAsyncCallback<iocp::AsyncCallback>(callback));

			WSABUF wsabuf = {0};
			wsabuf.buf = buf.data();
			wsabuf.len = buf.size();

			DWORD dwFlag = 0;
			DWORD dwSize = 0;

			if( 0 != ::WSARecv(socket_, &wsabuf, 1, &dwSize, &dwFlag, asynResult.Get(), NULL)
				&& ::WSAGetLastError() != WSA_IO_PENDING )
				throw Win32Exception("WSARecv");

			asynResult.Release();
		}

		// 异步发送数据
		void Socket::AsyncWrite(const iocp::ConstBuffer &buf, const iocp::CallbackType &callback)
		{
			AsyncCallbackBasePtr asynResult(iocp::MakeAsyncCallback<iocp::AsyncCallback>(callback));

			WSABUF wsabuf = {0};
			wsabuf.buf = const_cast<char *>(buf.data());
			wsabuf.len = buf.size();

			DWORD dwFlag = 0;
			DWORD dwSize = 0;

			if( 0 != ::WSASend(socket_, &wsabuf, 1, &dwSize, dwFlag, asynResult.Get(), NULL)
				&& ::WSAGetLastError() != WSA_IO_PENDING )
				throw Win32Exception("WSASend");

			asynResult.Release();
		}

		// 异步UDP写出
		void Socket::AsyncSendTo(const iocp::ConstBuffer &buf, const SOCKADDR_IN *addr, const iocp::CallbackType &callback)
		{
			AsyncCallbackBasePtr asynResult(iocp::MakeAsyncCallback<iocp::AsyncCallback>(callback));

			WSABUF wsabuf = {0};
			wsabuf.buf = const_cast<char *>(buf.data());
			wsabuf.len = buf.size();

			DWORD dwFlag = 0;
			DWORD dwSize = 0;

			if( 0 != ::WSASendTo(socket_, &wsabuf, 1, &dwSize, dwFlag, reinterpret_cast<const sockaddr *>(addr), addr == 0 ? 0 : sizeof(*addr), asynResult.Get(), NULL)
				&& ::WSAGetLastError() != WSA_IO_PENDING )
				throw Win32Exception("WSASendTo");
			
			asynResult.Release();
		}	

		// 异步UDP读入
		void Socket::AsyncRecvFrom(iocp::MutableBuffer &buf, SOCKADDR_IN *addr, const iocp::CallbackType &callback)
		{
			AsyncCallbackBasePtr asynResult(iocp::MakeAsyncCallback<iocp::AsyncCallback>(callback));

			WSABUF wsabuf = {0};
			wsabuf.buf = buf.data();
			wsabuf.len = buf.size();

			DWORD dwFlag = 0;
			DWORD dwSize = 0;
			int addrLen = (addr == 0 ? 0 : sizeof(addr));

			if( 0 != ::WSARecvFrom(socket_, &wsabuf, 1, &dwSize, &dwFlag, reinterpret_cast<sockaddr *>(addr), &addrLen, asynResult.Get(), NULL)
				&& ::WSAGetLastError() != WSA_IO_PENDING )
				throw Win32Exception("WSARecvFrom");
			
			asynResult.Release();
		}
	}

}