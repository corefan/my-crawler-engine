#include "domain.hpp"
#include "url.hpp"

#include "Extend STL/StringAlgorithm.h"


namespace crawler
{
	namespace filter
	{
		domain::domain(const std::wstring &domain_url)
			: domain_url_(domain_url)
		{}

		bool domain::is_valid(const url_ptr &url) const
		{
			return stdex::is_start_with(url->get_url(), domain_url_);
		}
	}
}