#ifndef __STATIC_ASSERT_HPP
#define __STATIC_ASSERT_HPP

#include <type_traits>

namespace utility
{
	template < bool x >
	struct StaticAssert
	{
		enum { value = 0 };
	};

	template <>
	struct StaticAssert<true>
	{
		enum { value = 1 };
	};
}

#define static_assert(val) \
	sizeof(char [StaticAssert<val>::value]);





#endif