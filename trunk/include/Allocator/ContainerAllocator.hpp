#ifndef __ALLOCATOR_CONTAINER_ALLOCATOR_HPP
#define __ALLOCATOR_CONTAINER_ALLOCATOR_HPP

#include <limits>
#include <type_traits>
#include <cassert>

#ifdef max
#undef max
#endif


namespace async
{
	namespace allocator
	{
		namespace detail
		{
			template< typename PoolT >
			inline PoolT &DefaultPool()
			{
				static PoolT pool;
				return pool;
			}
		}


		template< typename T, typename MemoryPoolT >
		class ContainerAllocator
		{
		public:
			typedef MemoryPoolT					MemoryPoolType;

			typedef typename std::remove_const<T>::type	value_type;
			typedef value_type *				pointer;
			typedef const value_type *			const_pointer;
			typedef value_type &				reference;
			typedef const value_type &			const_reference;
			typedef size_t						size_type;
			typedef ptrdiff_t					difference_type;

			template<typename U>
			struct rebind
			{
				typedef ContainerAllocator<U, MemoryPoolType> other;
			};

		public:
			MemoryPoolType &pool_;

		public:
			ContainerAllocator()
				: pool_(detail::DefaultPool<MemoryPoolType>())
			{}

			explicit ContainerAllocator(MemoryPoolType &pool)
				: pool_(pool)
			{}
			ContainerAllocator(const ContainerAllocator<T, MemoryPoolType> &rhs)
				: pool_(rhs.pool_)
			{}
			template<typename U>
			ContainerAllocator(const ContainerAllocator<U, MemoryPoolType> &rhs)
				: pool_(rhs.pool_)
			{}
			template<typename U>
			ContainerAllocator &operator=(const ContainerAllocator<U, MemoryPoolType> &)
			{ return *this; }


		public:
			pointer address(reference r) const
			{ return &r; }
			const_pointer address(const_reference s) const
			{ return &s; }
			size_type max_size() const
			{ return (std::numeric_limits<size_type>::max)() / sizeof(value_type); }
			//void construct(pointer ptr, const T &t)
			//{ std::_Construct(ptr, t); }
			void construct(pointer _Ptr, T&& _Val)
			{	// construct object at _Ptr with value _Val
				::new ((void *)_Ptr) T(std::forward<T>(_Val));
			}

			template<typename U>
			void construct(pointer _Ptr, U _Val)
			{	
				::new ((void *)_Ptr) T(std::forward<U>(_Val));
			}

			void destroy(pointer ptr)
			{
				std::_Destroy(ptr);
				(void) ptr; // avoid unused variable warning
			}

			pointer allocate(const size_type n)
			{
				return reinterpret_cast<pointer>(pool_.Allocate(n * sizeof(value_type)));
			}
			pointer allocate(const size_type n, const void * const)
			{ return allocate(n); }

			void deallocate(const pointer ptr, const size_type n)
			{
				if (ptr == 0 || n == 0)
				{
					assert(0);
					return;
				}

				pool_.Deallocate(ptr, n * sizeof(value_type));
			}
		};


		template<typename T, typename U, typename MemoryPoolT>
		inline bool operator==(const ContainerAllocator<T, MemoryPoolT> &, const ContainerAllocator<U, MemoryPoolT> &)
		{ return true; }

		template<typename T, typename U, typename MemoryPoolT>
		inline bool operator!=(const ContainerAllocator<T, MemoryPoolT> &, const ContainerAllocator<U, MemoryPoolT> &)
		{ return false; }
	
		
		template< typename MemoryPoolT >
		class ContainerAllocator<void, MemoryPoolT>
		{
		public:
			typedef void*       pointer;
			typedef const void* const_pointer;
			typedef void        value_type;

			template<typename U> 
			struct rebind 
			{
				typedef ContainerAllocator<U, MemoryPoolT> other;
			};

			ContainerAllocator()
			{}

			ContainerAllocator(const ContainerAllocator<void, MemoryPoolT>&)
			{}

			template<class U>
			ContainerAllocator(const ContainerAllocator<U, MemoryPoolT>&)
			{}

			template<class U>
			ContainerAllocator& operator=(const ContainerAllocator<U, MemoryPoolT>&)
			{ return (*this); }
		};

	}


}





#endif