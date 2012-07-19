#ifndef __CRAWLER_HRML_PARSER_HPP
#define __CRAWLER_HRML_PARSER_HPP

#include "common.hpp"
#include "url.hpp"

namespace crawler
{
	namespace parser
	{
		class html_parser
		{
		public:
			html_parser();
			~html_parser();

		private:
			html_parser(const html_parser &);
			html_parser &operator=(const html_parser &);

		public:
			void start();
			void stop();

			void parse(const url_ptr &, const buffer_type &);
		};
	}
}

#endif