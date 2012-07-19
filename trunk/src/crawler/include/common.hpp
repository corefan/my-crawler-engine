#ifndef __CRAWLER_COMMON_HPP
#define __CRAWLER_COMMON_HPP

#include <vector>
#include <list>
#include <string>
#include <functional>
#include <memory>
#include <cstdint>


namespace crawler
{

	typedef std::shared_ptr<char>			auto_buffer;
	typedef std::pair<auto_buffer, size_t>	buffer_type;


	buffer_type make_buffer(size_t len);


}


#endif