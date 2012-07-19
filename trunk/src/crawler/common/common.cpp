#include "common.hpp"


namespace crawler
{

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

	buffer_type make_buffer(size_t len)
	{
		auto_buffer buf(detail::allocate_buf(len), &detail::deallocate_buf);
		return std::make_pair(buf, len);
	}

}