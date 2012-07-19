#ifndef __IOCP_POINTER_HPP
#define __IOCP_POINTER_HPP

#include "ObjectFactory.hpp"


namespace async
{


	namespace iocp
	{

		template< typename T, typename RelaseFuncT >
		class Pointer
		{
		private:
			RelaseFuncT release_;
			T *obj_;

		public:
			explicit Pointer(T *obj)
				: obj_(obj)
			{}

			~Pointer()
			{
				Reset();
			}

		private:
			Pointer(const Pointer &);
			Pointer &operator=(const Pointer &);

		public:
			T *Get() const
			{
				return obj_;
			}

			T *operator->()
			{
				return Get();
			}

			T &operator*()
			{
				return *Get();
			}

			T *Release()
			{
				T *tmp = obj_;
				obj_ = 0;
				return tmp;
			}

			void Reset()
			{
				if( obj_ )
				{
					release_(obj_);
					obj_ = 0;
				}
			}
		};

	}


}




#endif