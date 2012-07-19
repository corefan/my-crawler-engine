#include "SocketProvider.hpp"
#include "../IOCP/WinException.hpp"
#include "SocketOption.hpp"
#include "Socket.hpp"




namespace async
{
	namespace network
	{

		SocketProvider::SocketProvider(IODispatcher &io)
		{
			static GUID guidTransmitFile		= WSAID_TRANSMITFILE;
			static GUID guidAcceptEx			= WSAID_ACCEPTEX;
			static GUID guidGetAcceptExSockaddrs= WSAID_GETACCEPTEXSOCKADDRS;
			static GUID guidTransmitPackets		= WSAID_TRANSMITPACKETS;
			static GUID guidConnectEx			= WSAID_CONNECTEX;
			static GUID guidDisconnectEx		= WSAID_DISCONNECTEX;
			static GUID guidWSARecvMsg			= WSAID_WSARECVMSG;

			SocketPtr socket(MakeSocket(io, AF_INET, SOCK_STREAM, IPPROTO_TCP));
			GetExtensionFuncPtr(socket, guidTransmitFile,			&TransmitFile);
			GetExtensionFuncPtr(socket, guidAcceptEx,				&AcceptEx);
			GetExtensionFuncPtr(socket, guidGetAcceptExSockaddrs,	&GetAcceptExSockaddrs);
			GetExtensionFuncPtr(socket, guidTransmitPackets,		&TransmitPackets);
			GetExtensionFuncPtr(socket, guidConnectEx,				&ConnectEx);
			GetExtensionFuncPtr(socket, guidDisconnectEx,			&DisconnectEx);
			GetExtensionFuncPtr(socket, guidWSARecvMsg,				&WSARecvMsg);
		}

		SocketProvider::~SocketProvider()
		{
		}


		void SocketProvider::GetExtensionFuncPtr(const SocketPtr &sock, const GUID &guid, LPVOID pFunc)
		{
			sock->IOControl(GetExtensionFunction(guid, pFunc));
		}

		void SocketProvider::CancelIO(SOCKET socket)
		{
			if( FARPROC cancelFuncPtr = ::GetProcAddress(::GetModuleHandleA("KERNEL32"), "CancelIoEx") )
			{
				// 仅在Vista以后支持，可以从不同的线程来取消IO操作
				typedef BOOL (__stdcall *CancelIOExPtr)(HANDLE, LPOVERLAPPED);
				CancelIOExPtr cancelIOEx = reinterpret_cast<CancelIOExPtr>(cancelFuncPtr);

				if( !cancelIOEx(reinterpret_cast<HANDLE>(socket), 0) )
					throw Win32Exception("CancelIOEx");
			}
			else
			{
				// 此处忽略了在同一线程的检查
				// CancelIO只能在同一个线程中取消IO操作
				if( !::CancelIo(reinterpret_cast<HANDLE>(socket)) )
					throw Win32Exception("CancelIo");
			}
		}
	}

}