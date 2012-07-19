#include "stdafx.h"
#include "html_parser.hpp"

#include <regex>
#include <iostream>

#include "Unicode/string.hpp"


namespace crawler
{
	namespace parser
	{
		html_parser::html_parser()
		{

		}

		html_parser::~html_parser()
		{

		}

		void html_parser::start()
		{

		}

		void html_parser::stop()
		{

		}

		void html_parser::parse(const url_ptr &url, const buffer_type &buffer)
		{
			static const std::regex title_regex("<title>.*?</title>", std::regex::icase);

			std::match_results<const char *> results;
			bool suc = std::regex_search(buffer.first.get(), results, title_regex);
			if( suc )
			{
				//std::cout << results[0] << std::endl;
				url->title_ = unicode::to_w((std::string)results[0], CP_UTF8);
			}


			static const std::cregex_token_iterator js_end;
			static const std::regex script_regex("<script.*?</script>", std::regex::icase);
			for(std::cregex_token_iterator i(buffer.first.get(), buffer.first.get() + buffer.second, script_regex); i != js_end; ++i)
			{
				//std::cout << i->str() << std::endl;
			}


			static const std::cregex_token_iterator css_end;
			static const std::regex css_regex("<style.*?</style>", std::regex::icase);
			for(std::cregex_token_iterator i(buffer.first.get(), buffer.first.get() + buffer.second, css_regex); i != css_end; ++i)
			{
				//std::cout << i->str() << std::endl;

			}
		}

	}

}