#ifndef __IOCP_OBJECT_FACTORY_HPP
#define __IOCP_OBJECT_FACTORY_HPP

#include "../../MemoryPool/SGIMemoryPool.hpp"
#include "../../MemoryPool/FixedMemoryPool.hpp"
#include "../../Allocator/ContainerAllocator.hpp"
#include <memory>


namespace async
{
	namespace iocp
	{
		// 每个类型一个Memory Pool
		template< typename MemoryPoolT >
		struct ObjectPool
		{
			typedef MemoryPoolT	MemoryPool;

			static __declspec(thread) MemoryPool *pool;
			static MemoryPool &GetMemoryPool()
			{
				//if( pool == 0 )
				//	pool = new MemoryPool;

				//return *pool;

				static MemoryPool pool;
				return pool;
			}
		};

		template< typename MemoryPoolT >
		__declspec(thread) MemoryPoolT *ObjectPool<MemoryPoolT>::pool;


		// 每个类型的内存的声请释放
		template< typename T >
		struct ObjectFactory
		{
			typedef async::memory::SGISTMemoryPool	PoolType;
			typedef ObjectPool<PoolType>			ObjectPoolType;
		};


		namespace detail
		{
			// 内部使用
			template< typename T >
			inline void ReleaseBuffer(T *p)
			{
				ObjectFactory<T>::ObjectPoolType::GetMemoryPool().Deallocate(p, sizeof(T));
			}

			template< typename T >
			inline T *CreateBuffer(size_t len = sizeof(T))
			{
				return static_cast<T *>(ObjectFactory<T>::ObjectPoolType::GetMemoryPool().Allocate(len));
			}
		}
		

		// -------------------------------
		// new delete base class

		template< typename ImplT >
		struct NewDeleteBase
		{
			static void *operator new(size_t size)
			{
				assert(sizeof(ImplT) == size);

				return ObjectFactory<ImplT>::ObjectPoolType::GetMemoryPool().Allocate(size);
			}
			static void operator delete(void *ptr, size_t size)
			{
				assert(sizeof(ImplT) == size);

				if( ptr == NULL )
					return;

				return ObjectFactory<ImplT>::ObjectPoolType::GetMemoryPool().Deallocate(ptr, size);
			}
		};



		// 内存申请释放
		template<typename T>
		inline void ObjectDeallocate(T *p)
		{
			p->~T();
			return detail::ReleaseBuffer<T>(p);
		}

		template<typename T>
		inline T *ObjectAllocate()
		{
			return ::new (detail::CreateBuffer<T>()) T;
		}

		template<typename T, typename Arg1>
		inline T *ObjectAllocate(const Arg1 &a1)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1);
		}

		template<typename T, typename Arg1, typename Arg2>
		inline T *ObjectAllocate(const Arg1 &a1, const Arg2& a2)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3>
		inline T *ObjectAllocate(const Arg1& a1, const Arg2& a2, const Arg3& a3)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2, a3);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		inline T *ObjectAllocate(const Arg1& a1, const Arg2& a2, const Arg3& a3, const Arg4 &a4)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2, a3, a4);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		inline T *ObjectAllocate(const Arg1& a1, const Arg2& a2, const Arg3& a3, const Arg4 &a4, const Arg5 &a5)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2, a3, a4, a5);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
		inline T *ObjectAllocate(const Arg1& a1, const Arg2& a2, const Arg3& a3, const Arg4 &a4, const Arg5 &a5, const Arg6 &a6)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2, a3, a4, a5, a6);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
		inline T *ObjectAllocate(const Arg1& a1, const Arg2& a2, const Arg3& a3, const Arg4 &a4, const Arg5 &a5, const Arg6 &a6, const Arg7 &a7)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2, a3, a4, a5, a6, a7);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
		inline T *ObjectAllocate(const Arg1& a1, const Arg2& a2, const Arg3& a3, const Arg4 &a4, const Arg5 &a5, const Arg6 &a6, const Arg7 &a7, const Arg8 &a8)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2, a3, a4, a5, a6, a7, a8);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
		inline T *ObjectAllocate(const Arg1& a1, const Arg2& a2, const Arg3& a3, const Arg4 &a4, const Arg5 &a5, const Arg6 &a6, const Arg7 &a7, const Arg8 &a8, const Arg9 &a9)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2, a3, a4, a5, a6, a7, a8, a9);
		}


		template<typename T, typename Arg1>
		inline T *ObjectAllocate(Arg1 &a1)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1);
		}

		template<typename T, typename Arg1, typename Arg2>
		inline T *ObjectAllocate(Arg1 &a1, Arg2& a2)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3>
		inline T *ObjectAllocate(Arg1& a1, Arg2& a2, Arg3& a3)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2, a3);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		inline T *ObjectAllocate(Arg1& a1, Arg2& a2, Arg3& a3, Arg4 &a4)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2, a3, a4);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		inline T *ObjectAllocate(Arg1& a1, Arg2& a2, Arg3& a3, Arg4 &a4, Arg5 &a5)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2, a3, a4, a5);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
		inline T *ObjectAllocate(Arg1& a1, Arg2& a2, Arg3& a3, Arg4 &a4, Arg5 &a5, Arg6 &a6)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2, a3, a4, a5, a6);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
		inline T *ObjectAllocate(Arg1& a1, Arg2& a2, Arg3& a3, Arg4 &a4, Arg5 &a5, Arg6 &a6, Arg7 &a7)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2, a3, a4, a5, a6, a7);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
		inline T *ObjectAllocate(Arg1& a1, Arg2& a2, Arg3& a3, Arg4 &a4, Arg5 &a5, Arg6 &a6, Arg7 &a7, Arg8 &a8)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2, a3, a4, a5, a6, a7, a8);
		}

		template<typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
		inline T *ObjectAllocate(Arg1& a1, Arg2& a2, Arg3& a3, Arg4 &a4, Arg5 &a5, Arg6 &a6, Arg7 &a7, Arg8 &a8, Arg9 &a9)
		{
			return ::new (detail::CreateBuffer<T>()) T(a1, a2, a3, a4, a5, a6, a7, a8, a9);
		}
	}
}








#endif