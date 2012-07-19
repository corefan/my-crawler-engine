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


	namespace detail
	{
		inline char *allocate_buf(size_t len)
		{
			return reinterpret_cast<char *>(::operator new(len));
		}

		inline void deallocate_buf(char *p)
		{
			::operator delete(p);
		}
	}

	inline buffer_type make_buffer(size_t len)
	{
		auto_buffer buf(detail::allocate_buf(len), &detail::deallocate_buf);
		return std::make_pair(buf, len);
	}
}



#endif