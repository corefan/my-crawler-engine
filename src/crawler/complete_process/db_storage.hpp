#ifndef __CRAWLER_DB_STORAGE_HPP
#define __CRAWLER_DB_STORAGE_HPP

#include "common.hpp"
#include "url.hpp"




namespace crawler
{
	namespace storage
	{
		class db
		{
			typedef std::function<void(const std::string &)>	handle_error_type;

			struct impl;
			std::auto_ptr<impl> impl_;

		public:
			void start();
			void stop();

			void save(const url_ptr &url, const buffer_type &buffer, const handle_error_type &handle_error);
		};
	}
	
}


#endif