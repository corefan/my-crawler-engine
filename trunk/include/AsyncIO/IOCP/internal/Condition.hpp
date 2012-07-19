#ifndef __IOCP_INTERNAL_CONDITION_HPP
#define __IOCP_INTERNAL_CONDITION_HPP

#include <limits>

namespace async
{
	namespace iocp
	{

		namespace detail
		{
			// 默认单次传输字节大小
			//enum { DEFAULT_MAX_TRANSFER =  };


			// ----------------------------------------------------
			// struct TransferAll

			// 传输所有字节
			struct TransferAllT
			{
				typedef size_t	result_type;

				result_type operator()(size_t min = 0) const
				{
					return std::numeric_limits<result_type>::max();
				}
			};


			// ----------------------------------------------------
			// class TransferAtLeat

			// 最少传输字节
			class TransferAtLeatT
			{
			public:
				typedef size_t	result_type;

			private:
				size_t min_;

			public:
				explicit TransferAtLeatT(size_t min)
					: min_(min)
				{}

			public:
				result_type operator()(size_t min = 0) const
				{
					return min_;
				}
			};
		}


		// 传输条件

		inline detail::TransferAllT TransferAll()
		{
			return detail::TransferAllT();
		}

		inline detail::TransferAtLeatT TransferAtLeast(size_t min)	
		{
			return detail::TransferAtLeatT(min);
		}
	}
}








#endif