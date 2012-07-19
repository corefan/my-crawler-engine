#include "IPAddress.hpp"


namespace async
{

	namespace network
	{

		IPAddress::IPAddress(u_long address)
			: address_(address)
		{
		}


		IPAddress IPAddress::Parse(const std::string &str)
		{
			u_long address = ::ntohl(::inet_addr(str.c_str()));

			return IPAddress(address);
		}

		std::string	IPAddress::Parse(const IPAddress &addr)
		{
			in_addr tmp = {0};
			tmp.s_addr = addr;

            char *p = ::inet_ntoa(tmp);
			return p;
		}
	}

}