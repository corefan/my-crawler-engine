#ifndef __WIN_HTTP_OPTION_HPP
#define __WIN_HTTP_OPTION_HPP

namespace http
{

	namespace option
	{
		template < DWORD FlagT, typename OptionT >
		struct option_t;


		template < DWORD FlagT >
		struct option_t<FlagT, DWORD>
		{
			enum { val = FlagT };

			DWORD option_;

			option_t(DWORD option)
				: option_(option)
			{}

		public:
			DWORD flag() const
			{
				return val;
			}

			void *option()
			{
				return reinterpret_cast<void *>(&option_);
			}

			DWORD length() const
			{
				return sizeof(option_);
			}
		};

		template < DWORD FlagT >
		struct option_t<FlagT, std::wstring>
		{
			enum { val = FlagT };

			std::wstring option_;

			option_t(const std::wstring &option)
				: option_(std::move(option))
			{}

		public:
			DWORD flag() const
			{
				return val;
			}

			void *option()
			{
				return (void *)option_.c_str();
			}

			DWORD length() const
			{
				return option_.length() * sizeof(std::wstring::value_type);
			}
		};

		template < >
		struct option_t< WINHTTP_OPTION_PROXY, WINHTTP_PROXY_INFO >
		{
			enum { val = WINHTTP_OPTION_PROXY };

			WINHTTP_PROXY_INFO proxy_;

			option_t(const std::wstring &proxy)
			{
				proxy_.dwAccessType		= WINHTTP_ACCESS_TYPE_NAMED_PROXY;
				proxy_.lpszProxy		= const_cast<LPWSTR>(proxy.c_str());
				proxy_.lpszProxyBypass	= 0;
			}

		public:
			DWORD flag() const
			{
				return val;
			}

			void *option()
			{
				return &proxy_;
			}

			DWORD length() const
			{
				return sizeof(proxy_);
			}

		};




		typedef option_t<WINHTTP_OPTION_SECURITY_FLAGS, DWORD>			security;
		typedef option_t<WINHTTP_OPTION_PROXY_USERNAME, std::wstring>	user_name;
		typedef option_t<WINHTTP_OPTION_PROXY_PASSWORD, std::wstring>	password;
		typedef option_t<WINHTTP_OPTION_DISABLE_FEATURE, DWORD>			disable_feature;
		typedef option_t<WINHTTP_OPTION_PROXY, WINHTTP_PROXY_INFO>		proxy;
	}
	
	
}




#endif