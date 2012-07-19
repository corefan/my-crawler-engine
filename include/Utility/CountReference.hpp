#ifndef __ULTILITY_COUNTING_REFERENCE_HPP
#define __ULTILITY_COUNTING_REFERENCE_HPP

#include "../MemoryPool/SGIMemoryPool.hpp"
#include "../Allocator/MemAllocator.hpp"

#include <Windows.h>



namespace async
{

	//------------------------------------------------------
	// class ReferenceCountor Allocator

	template<typename T, bool __IsMT>
	class ReferenceCountorAllocator
	{
	public:
		typedef memory::SGIMemoryPool<__IsMT, 256>			BufferPool;
		typedef memory::MemAllocator<T, BufferPool>			AllocType;

	public:
		static AllocType &GetMemory()
		{
			static BufferPool memoryPool;
			static AllocType alloc(memoryPool);

			return alloc;
		}

		static T *Allocate()
		{
			return GetMemory().Allocate();
		}

		static void Deallocate(T *p)
		{
			typedef typename std::tr1::remove_volatile<T>::type Type;
			GetMemory().Deallocate((Type *)p);
		}
	};


	//------------------------------------------------------
	// ReferenceCount Traits

	class MTReferenceCount
	{
		typedef ReferenceCountorAllocator<volatile long, true> Allocator;

	private:
		volatile long *m_pCount;

	public:
		MTReferenceCount()
			: m_pCount(NULL)
		{}

	public:
		template<typename T>
		void Init(T *)
		{
			m_pCount = Allocator::Allocate();
			*m_pCount = 1;
		}

		template<typename T>
		void Dispose(T *)
		{
			*m_pCount = 0;
			Allocator::Deallocate(m_pCount);
		}

		template<typename T>
		void Increment(T *)
		{
			::InterlockedIncrement(m_pCount);
		}

		template<typename T>
		void Decrement(T *)
		{
			::InterlockedDecrement(m_pCount);
		}

		template<typename T>
		bool IsZero(T *)
		{
			return *m_pCount == 0;
		}
	};


	class STReferenceCount
	{
		typedef ReferenceCountorAllocator<long, false> Allocator;

	private:
		long *m_pCount;

	public:
		STReferenceCount()
			: m_pCount(NULL)
		{}

	public:
		template<typename T>
		void Init(T *)
		{
			m_pCount = Allocator::Allocate();
			*m_pCount = 1;
		}

		template<typename T>
		void Dispose(T *)
		{
			*m_pCount = 0;
			Allocator::Deallocate(m_pCount);
		}

		template<typename T>
		void Increment(T *)
		{
			++*m_pCount;
		}

		template<typename T>
		void Decrement(T *)
		{
			--*m_pCount;
		}

		template<typename T>
		bool IsZero(T *)
		{
			return *m_pCount == 0;
		}
	};




	//--------------------------------------------------------
	// Release Traits

	class ReleaseObject
	{
	public:
		template<typename T>
		void Dispose(T *pObject)
		{
			delete pObject;
		}
	};

	class RelaseArray
	{
	public:
		template<typename T>
		void Dispose(T *pArray)
		{
			delete []pArray;
		}
	};



	//----------------------------------------------------------
	// class CountReferencePtr

	template
	<
		typename T, 
		typename ObjectPolicyT  = ReleaseObject, 
		typename CounterPolicyT = MTReferenceCount
	>
	class CountReferencePtr:
		private ObjectPolicyT,
		private CounterPolicyT
	{
	private:
		typedef ObjectPolicyT	OP;
		typedef CounterPolicyT	CP;
		

		T *m_pObject;

	public:
		explicit CountReferencePtr(T *ptr)
		{
			_Init(ptr);
		}

		CountReferencePtr(const CountReferencePtr<T, OP, CP> &cp)
			: OP((const OP&)cp)
			, CP((const CP&)cp)
		{
			_Attach(cp);
		}

		~CountReferencePtr()
		{
			_Detach();
		}


		CountReferencePtr<T, OP, CP> &operator=(T *ptr)
		{
			assert(ptr != m_pObject);
			_Detach();

			_Init(ptr);
			return *this;
		}

		CountReferencePtr<T, OP, CP> &operator=(const CountReferencePtr<T, OP, CP> &cp)
		{
			if( m_pObject != cp.m_pObject )
			{
				_Detach();
				OP::operator=((const OP &)cp);
				CP::operator=((const CP &)cp);
				
				_Attach(cp);
			}

			return *this;
		}

		template<typename U>
		operator CountReferencePtr<U, OP, CP>()
		{
			return CountReferencePtr<U, OP, CP>(m_pObject);
		}	

		T *operator->() const
		{
			return m_pObject;
		}

		T &operator*() const
		{
			return *m_pObject;
		}

		T *Get() const
		{
			return m_pObject;
		}

		void AddRef()
		{
			if( m_pObject != NULL )
			{
				CP::Increment(m_pObject);
			}
		}

		void Release()
		{
			if( m_pObject != NULL )
			{
				CP::Decrement(m_pObject);
			}
		}

	private:
		void _Init(T *ptr)
		{
			if( ptr != NULL )
			{
				CP::Init(ptr);
			}

			m_pObject = ptr;
		}

		void _Attach(const CountReferencePtr<T, OP, CP> &cp)
		{
			m_pObject = cp.m_pObject;
			if( cp.m_pObject != NULL )
			{
				CP::Increment(cp.m_pObject);
			}
		}

		void _Detach()
		{
			if( m_pObject != NULL )
			{
				CP::Decrement(m_pObject);
				if( CP::IsZero(m_pObject) )
				{
					CP::Dispose(m_pObject);
					OP::Dispose(m_pObject);
				}
			}
		}
	};


}




#endif