#ifndef __NETWORK_IP_ADDRESS_HPP
#define __NETWORK_IP_ADDRESS_HPP


#include "../Basic.hpp"
#include <string>


namespace async
{


	namespace network
	{

		class IPAddress 
		{
		private:
			u_long address_;

		public:
			IPAddress(u_long address);

		public:
			u_long Address() const
			{ return address_; }
			operator u_long() const
			{ return address_; }

			bool operator==(const IPAddress &ipaddr)
			{ return address_ == ipaddr.address_; }

			bool operator!=(const IPAddress &ipaddr)
			{ return address_ != ipaddr.address_; }

		public:
			static IPAddress Parse(const std::string &str);
			static std::string Parse(const IPAddress &addr);
		};
	}


}



#endif