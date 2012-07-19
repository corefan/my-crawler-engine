#ifndef __IOCP_INTERNAL_CONDITION_HPP
#define __IOCP_INTERNAL_CONDITION_HPP

#include <limits>

namespace async
{
	namespace iocp
	{

		namespace detail
		{
			// Ĭ�ϵ��δ����ֽڴ�С
			//enum { DEFAULT_MAX_TRANSFER =  };


			// ----------------------------------------------------
			// struct TransferAll

			// ���������ֽ�
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

			// ���ٴ����ֽ�
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


		// ��������

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