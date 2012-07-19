#ifndef __NETWORK_HELPER_HPP
#define __NETWORK_HELPER_HPP

#include <WinSock2.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <vector>
#include <Psapi.h>


#include "../../AsyncIO/Network/IPAddress.hpp"
#include "../../Extend STL/StringAlgorithm.h"
#include "../../Unicode/string.hpp"


#pragma comment(lib, "Psapi")


namespace win32
{
	namespace network
	{
		// IP ���
		template < typename CharT >
		inline std::basic_string<CharT> IP2String(unsigned long ip);
		
		template <>
		inline std::string IP2String<char>(unsigned long ip)
		{
            return async::network::IPAddress::Parse(::htonl(ip));
		}

		template <>
		inline std::wstring IP2String<wchar_t>(unsigned long ip)
		{
			return std::wstring(CA2W(async::network::IPAddress::Parse(ip).c_str()));
		}

		inline unsigned long String2IP(const std::string &ip)
		{
			return async::network::IPAddress::Parse(ip);
		}

		inline unsigned long String2IP(const std::wstring &ip)
		{
			return String2IP((LPCSTR)CW2A(ip.c_str()));
		}

		inline bool IsValidIP(LPCTSTR lpszIP)
		{
			int nIP = 0, nCount = 0;   

            stdex::tString IP = lpszIP;
			std::vector<stdex::tString> vecIP;
			stdex::Split(vecIP, IP, _T('.'));

			// ��4���� ��Ϊ�����Ϲ���
			if( vecIP.size() != 4 ) 
				return false;

			// ip��β��Ϊ0 ��Ϊ�����Ϲ���
			if( *(vecIP.begin()) == _T("0") || *(vecIP.rbegin()) == _T("0") )
				return false;

			for(std::vector<stdex::tString>::const_iterator iter = vecIP.begin(); iter != vecIP.end(); ++iter)
			{
				if( *iter == _T("255") ) 
					++nCount;

				// ip ÿ���β���0��255��Χ�� ��Ϊ�����Ϲ���
				stdex::ToNumber(nIP, (*iter));
				if ( nIP < 0 || nIP > 255 )
					return false;
			}

			// ip ÿ���ζ���255 ��Ϊ�����Ϲ���
			if ( nCount == 4 ) 
				return false;

			// ip ���ͳɹ� ��Ϊ���Ϲ���
			if( ::inet_addr(CT2A(lpszIP)) != INADDR_NONE )   
				return true; 

			// ������Ϊ�����Ϲ���
			return false;
		}

		// �õ���������IP
		template < typename CharT >
		inline bool GetLocalIps(std::vector<std::basic_string<CharT>> &IPs)
		{
			IP_ADAPTER_INFO info[16] = {0};
			DWORD dwSize = sizeof(info);
			if( ERROR_SUCCESS != ::GetAdaptersInfo(info, &dwSize) )
				return false; 

			PIP_ADAPTER_INFO pAdapter = info;
			while (pAdapter != NULL)
			{
				PIP_ADDR_STRING pAddr = &pAdapter->IpAddressList;
				while (pAddr != NULL)
				{
					std::basic_string<CharT> tmp = unicode::translate_t<CharT>::utf(pAddr->IpAddress.String);

					IPs.push_back(tmp);
					pAddr = pAddr->Next;
				}
				pAdapter = pAdapter->Next;
			}
			return true;
		}

		// �Ƿ�Ϊ����IP
		inline bool IsLocalMachine(const std::string &ip)
		{
			if( ip == "0.0.0.0" )
				return false;

			if( ip == "127.0.0.1" )
				return true;

			std::vector<std::string> IPs;
			if( !GetLocalIps(IPs) )
				return false;

			for(std::vector<std::string>::iterator iter = IPs.begin(); iter != IPs.end(); ++iter)
			{
				if ( ip == (*iter) )
					return true;
			}

			return false;
		}


		// IP Helper

		template < typename CharT >
		inline bool GetLocalGateIps(std::vector<std::basic_string<CharT>> &gateIPs)
		{
			IP_ADAPTER_INFO info[16] = {0};
			DWORD dwSize = sizeof(info);
			if( ERROR_SUCCESS != ::GetAdaptersInfo(info, &dwSize) )
				return false; 

			PIP_ADAPTER_INFO pAdapter = info;
			while (pAdapter != NULL)
			{
				PIP_ADDR_STRING pAddr = &pAdapter->GatewayList;
				while (pAddr != NULL)
				{
					std::basic_string<CharT> tmp = unicode::aux::converter<CharT>::utf(pAddr->IpAddress.String, unicode::npos);
					gateIPs.push_back(tmp);
					pAddr = pAddr->Next;
				}
				pAdapter = pAdapter->Next;
			}
			return true;
		}

		// ��ȡMAC
		template < typename CharT >
		inline std::basic_string<CharT> GetMacAddress(const std::basic_string<CharT> &ip)
		{
			const int MAX_ADAPTER_NUM = 10; //���֧��10������
			IP_ADAPTER_INFO astAdapter[MAX_ADAPTER_NUM] = {0};
			ULONG nSize = sizeof(astAdapter);
			if( ERROR_SUCCESS != ::GetAdaptersInfo(astAdapter, &nSize) )
			{
				assert(0 && "��������������Ԥ��");
				return std::basic_string<CharT>();
			}

			const std::string srcIP(CT2A(ip.c_str()));
			for(PIP_ADAPTER_INFO pAdapter = astAdapter; pAdapter != NULL; pAdapter = pAdapter->Next)
			{
				// ȷ������̫��,ȷ��MAC��ַ�ĳ���Ϊ 00-00-00-00-00-00
				if(pAdapter->Type == MIB_IF_TYPE_ETHERNET && 
					pAdapter->AddressLength == 6 && 
					srcIP == pAdapter->IpAddressList.IpAddress.String)
				{
					CharT mac[32] = {0};
					_stprintf_s(mac,
						_T("%02X-%02X-%02X-%02X-%02X-%02X"),
						int (pAdapter->Address[0]),
						int (pAdapter->Address[1]),
						int (pAdapter->Address[2]),
						int (pAdapter->Address[3]),
						int (pAdapter->Address[4]),
						int (pAdapter->Address[5]));
					return std::basic_string<CharT>(mac);
				}
			}

			return std::basic_string<CharT>();
		}

		template < typename CharT >
		inline std::basic_string<CharT> StringMacToBinaryMac(const std::basic_string<CharT> &mac)
		{
			if( mac.empty() )
				return false;

			static const size_t len = ::strlen("00-12-EF-AC-0A-78");

			if( mac.length() != len ) 
				return false;

			std::basic_string<CharT> tmp;
			for(size_t i = 0; i < 6; ++i) 
			{
				tmp += (mac[i*3] - (mac[i*3] >= 'A' ? ('A'-10) : '0')) * 16;
				tmp += mac[i*3+1] - (mac[i*3+1] >= 'A' ? ('A'-10) : '0');
			}

			return tmp;
		}

		// �ж��Ƿ�Ϊ����IP
		template < typename CharT >
		inline bool IsLocalMachineByIP(const std::basic_string<CharT> &ip)
		{
			if( ip == unicode::aux::converter<CharT>::utf("0.0.0.0") )
				return false;

			if( ip == unicode::aux::converter<CharT>::utf("127.0.0.1") )
				return true;

			std::vector<std::basic_string<CharT>> IPs;
			if( !GetLocalIps(IPs) )
				return false;

			return std::find(IPs.begin(), IPs.end(), ip) != IPs.end();
		}

		// �ж��Ƿ�Ϊ����IP
		template < typename CharT >
		inline bool IsLocalMachineByName(const std::basic_string<CharT> &name)
		{
			hostent *remoteHost = ::gethostbyname(unicode::aux::converter<char>::utf(name));
			if( remoteHost == 0 )
				return false;

			in_addr addr = {0};
			addr.s_addr = *(u_long *)remoteHost->h_addr_list[0];

			std::string ip = ::inet_ntoa(addr);
			return IsLocalMachineByIP(ip);
		}

		inline DWORD GetDesIP(SOCKET hSocket)
		{
			assert(hSocket != NULL && hSocket != INVALID_SOCKET);
			sockaddr_in addr = {0};
			int len = sizeof(addr);
			if (::getpeername(hSocket, reinterpret_cast<PSOCKADDR>(&addr), &len) != 0)
			{
				assert(0 && "������׽���");
			}
			return addr.sin_addr.S_un.S_addr;
		}

		// ��ȡDNS
		template < typename CharT >
		inline bool GetLocalDNS(std::vector<std::basic_string<CharT>> &dns)
		{
			IP_ADDR_STRING *pIPAddr;

			FIXED_INFO fixed = {0};
			ULONG outBufLen = sizeof(FIXED_INFO);

			std::vector<char> tmpBuf;
			FIXED_INFO *tmpFixedInfo = 0;
			if( ::GetNetworkParams(&fixed, &outBufLen) == ERROR_BUFFER_OVERFLOW ) 
			{
				tmpBuf.resize(outBufLen);
				tmpFixedInfo = &tmpBuf[0];
			}
			else
			{
				tmpFixedInfo = &fixed;
			}

			DWORD dwRetVal;
			if( dwRetVal = ::GetNetworkParams(tmpFixedInfo, &outBufLen) == NO_ERROR ) 
			{
				std::basic_string<CharT> tmp = 
					unicode::aux::converter<CharT>::utf(tmpFixedInfo->DnsServerList.IpAddress.String, unicode::npos);
				dns.push_back(tmp);

				pIPAddr = tmpFixedInfo->DnsServerList.Next;
				while (pIPAddr) 
				{
					pIPAddr = tmpFixedInfo->DnsServerList.Next;
					tmp = unicode::aux::converter<CharT>::utf(pIPAddr->IpAddress.String, unicode::npos);
					dns.push_back(tmp);

					pIPAddr = pIPAddr->Next;
				}
			}


			return true;
		}
	}
}




#endif