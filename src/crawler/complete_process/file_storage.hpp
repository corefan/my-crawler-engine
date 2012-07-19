#ifndef __CRAWLER_FILE_STORAGE_HPP
#define __CRAWLER_FILE_STORAGE_HPP

#include "common.hpp"
#include "url.hpp"


namespace crawler
{
	namespace storage
	{
		class file
		{
			typedef std::function<void(const std::string &)>	handle_error_type;

		public:
			void start();
			void stop();

			void save(const url_ptr &url, const buffer_type &buffer, const handle_error_type &handle_error);
		};
	}
	
}


#endif