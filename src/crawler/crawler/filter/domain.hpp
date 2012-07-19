#ifndef __CRAWLER_FILTER_DOMAIN_HPP
#define __CRAWLER_FILTER_DOMAIN_HPP

#include <memory>
#include <string>


namespace crawler
{
	class url;
	typedef std::shared_ptr<url> url_ptr;


	namespace filter
	{

		class domain
		{
			const std::wstring domain_url_;

		public:
			explicit domain(const std::wstring &domain_url);

		private:
			domain(const domain &);
			domain &operator=(const domain &);

		public:
			bool is_valid(const url_ptr &url) const;
		};
	}
}

#endif