#ifndef __NETWORK_UDP_HPP
#define __NETWORK_UDP_HPP


#include "Socket.hpp"
#include "BasicDatagramSocket.hpp"
#include "SocketOption.hpp"

#include "../iocp/Write.hpp"
#include "../iocp/Read.hpp"
#include "../iocp/ReadWriteBuffer.hpp"


namespace async
{
	namespace network
	{

		// --------------------------------------------------
		// class Udp

		class Udp
		{
		public:
			typedef BasicDatagramSocket<Udp>		Socket;


		private:
			int family_;

		private:
			explicit Udp(int family)
				: family_(family)
			{}


		public:
			int Type() const
			{
				return SOCK_DGRAM;
			}

			int Protocol() const
			{
				return IPPROTO_UDP;
			}

			int Family() const
			{
				return family_;
			}

		public:
			static Udp V4()
			{
				return Udp(AF_INET);
			}

		public:
			friend bool operator==(const Udp &lhs, const Udp &rhs)
			{
				return lhs.family_ == rhs.family_;
			}
			friend bool operator!=(const Udp &lhs, const Udp &rhs)
			{
				return !(lhs == rhs);
			}
		};
	}
}










#endif