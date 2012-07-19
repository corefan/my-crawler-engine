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
				// ����Vista�Ժ�֧�֣����ԴӲ�ͬ���߳���ȡ��IO����
				typedef BOOL (__stdcall *CancelIOExPtr)(HANDLE, LPOVERLAPPED);
				CancelIOExPtr cancelIOEx = reinterpret_cast<CancelIOExPtr>(cancelFuncPtr);

				if( !cancelIOEx(reinterpret_cast<HANDLE>(socket), 0) )
					throw Win32Exception("CancelIOEx");
			}
			else
			{
				// �˴���������ͬһ�̵߳ļ��
				// CancelIOֻ����ͬһ���߳���ȡ��IO����
				if( !::CancelIo(reinterpret_cast<HANDLE>(socket)) )
					throw Win32Exception("CancelIo");
			}
		}
	}

}