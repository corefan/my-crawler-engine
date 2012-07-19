#ifndef __UTLITY_SINGLETON_HPP
#define __UTLITY_SINGLETON_HPP

#include <Comm/Constraints.hpp>
#include <Comm/Thread/Lock.hpp>


namespace DataStructure
{
	//----------------------------------------------------------
	// Create policy

	template<typename T, bool __IsMT>
	class NoCreation 
	{
	public: 
		NoCreation() 
		{}
		~NoCreation() 
		{}
		
		static T* CreateInstance() 
		{ throw std::logic_error("NoCreation::CreateInstance called"); }

		static void DestroyInstance(T*) 
		{ throw std::logic_error("NoCreation::DestroyInstance called"); }

	private : 
		NoCreation(const NoCreation &);
		NoCreation& operator=(const NoCreation &);
	};

	template<typename T, bool __IsMT>
	class CreateUsingNew 
	{
	public : 
		CreateUsingNew() 
		{}
		~CreateUsingNew() 
		{}

		static T* CreateInstance() 
		{ return new T; }

		static void DestroyInstance(T* t) 
		{ delete t; }

	private : 
		CreateUsingNew(const CreateUsingNew &);
		CreateUsingNew& operator=(const CreateUsingNew&);
	};	


	template<typename T, bool __IsMT>
	class CreateUsingPool
	{
		typedef typename Select<__IsMT, SGIMTMemoryPool, SGISTMemoryPool>::value_type PoolType;
		typedef MemAllocator<T, PoolType> Allocator;

	public : 
		CreateUsingPool() 
		{}
		~CreateUsingPool() 
		{}

		static Allocator &GetPool()
		{
			static PoolType pool;
			static Allocator alloc(pool);

			return alloc;
		}

		static T* CreateInstance()
		{ 
			T *p = reinterpret_cast<T *>(GetPool().Allocate());
			GetPool().Construct(p);

			return p;
		}

		static void DestroyInstance(T* p) 
		{ 
			GetPool().Deallocate(p);
		}


	private : 
		CreateUsingPool(const CreateUsingPool &);
		CreateUsingPool& operator=(const CreateUsingPool&);
	};	

	template<typename T, bool __IsMT>
	class CreateUsingStatic 
	{
	public : 
		CreateUsingStatic() 
		{}
		~CreateUsingStatic() 
		{}

		
		static T* CreateInstance() 
		{
			static T singleton;
			return reinterpret_cast<T*>(&singleton);
		}
		static void DestroyInstance(T* t) 
		{ 
			//t->~T(); 
		}

	private : 
		CreateUsingStatic(const CreateUsingStatic &);
		CreateUsingStatic& operator=(const CreateUsingStatic &);
	};	


	//---------------------------------------------------------
	// Lifetime policy

	template<typename T>
	class DefaultLifetime 
	{
	public : 
		DefaultLifetime() 
		{}
		~DefaultLifetime() 
		{}

		static void OnDeadReference() 
		{ throw std::logic_error("Dead reference Detected"); }

		static void ScheduleForDestruction(void (*pFun)()) 
		{ std::atexit(pFun); }

	private : 
		DefaultLifetime(const DefaultLifetime &);
		DefaultLifetime& operator=(const DefaultLifetime&);
	};	


	template<typename T>
	class PhoenixSingleton 
	{
	public : 
		PhoenixSingleton() 
		{}
		~PhoenixSingleton() 
		{}

		static void OnDeadReference() 
		{}
		static void ScheduleForDestruction(void (*pFun)()) 
		{ std::atexit(pFun); }

	private : 
		PhoenixSingleton(const PhoenixSingleton&);
		PhoenixSingleton& operator=(const PhoenixSingleton &);
	};	

	
	template<typename T>
	class NoDestruction 
	{
	public : 
		NoDestruction()
		{}
		~NoDestruction() 
		{}

		static void OnDeadReference() 
		{}
		static void ScheduleForDestruction(void (*)())
		{}

	private : 
		NoDestruction(const NoDestruction &);
		NoDestruction& operator=(const NoDestruction &);
	};	

	



	//---------------------------------------------------------------
	// class Singleton

	template<
		typename T,
		bool __IsMT = true,
		typename CreationPolicy = CreateUsingStatic<T, __IsMT>, 
		template <typename> class LifetimePolicy = DefaultLifetime >
	class Singleton
	{
		typedef T				value_type;
		typedef T*				PointerType;
		typedef const T*		ConstPointerType;
		typedef T&				ReferenceType;
		typedef const T&		CostReferenceType;

		typedef typename Select<__IsMT, CAutoSpinLock, CAutoNull>::value_type LockType;
		typedef CAutoLock<LockType>	AutoLock;

		typedef CreationPolicy	CreationType;
		typedef LifetimePolicy<T> LifetimeType;
		

		typedef void (*UserSuppliedDestroy)(T*);

	private: 
		static PointerType m_instance;
		static bool m_bDestroy;
		static UserSuppliedDestroy m_pFunc;
		static LockType m_lock;

	public:
		static ReferenceType Instance()
		{
			if( Singleton::m_instance == 0 ) 
			{
				AutoLock lock(m_lock);
				if( Singleton::m_instance == 0 ) 
				{
					Singleton::m_pFunc = 0;
					if( Singleton::m_bDestroy ) 
					{
						LifetimeType::OnDeadReference();
						Singleton::m_bDestroy = false;
					}

					Singleton::m_instance = CreationType::CreateInstance();
					try 
					{
						LifetimeType::ScheduleForDestruction(Singleton::Destroy);
					} 
					catch(...) 
					{
						assert(0);
						CreationType::DestroyInstance(Singleton::m_instance);
					}
				}
			}

			return *(Singleton::m_instance);
		}
		
		static void Destroy()
		{
			if( Singleton::m_instance != 0 ) 
			{
				AutoLock lock(m_lock);
				if( Singleton::m_instance != 0 ) 
				{
					if( Singleton::m_pFunc != NULL ) 
					{
						Singleton::m_pFunc(Singleton::m_instance);
					} 
					else 
					{
						CreationType::DestroyInstance(Singleton::m_instance);
					}

					Singleton::m_instance = 0;
					Singleton::m_bDestroy = true;
				}
			}
		}
		
		static void Reset(PointerType p, void (*pFun)(T*))
		{
			AutoLock lock(m_lock);
			if( Singleton::m_instance != 0 )
			{
				CreationType::DestroyInstance(Singleton::m_instance);
			} 
			else if( p != NULL ) 
			{
				LifetimeType::ScheduleForDestruction(Singleton::Destroy);
			}

			Singleton::m_instance = p;
			Singleton::m_pFunc = pFun;
		}

	public : 
		Singleton() 
		{ 
		}
		~Singleton() 
		{
			Singleton::m_instance = 0; 
			Singleton::m_bDestroy = true; 
			Singleton::m_pFunc = 0; 
		}


	private : 
		Singleton(const Singleton &);
		Singleton& operator=(const Singleton &);
	};	

	
	template<typename T, bool __IsMT, typename CreationPolicy, template <typename> class LifetimePolicy>
	typename Singleton<T, __IsMT, CreationPolicy, LifetimePolicy>::PointerType Singleton<T, __IsMT, CreationPolicy, LifetimePolicy>::m_instance = 0;

	template<typename T, bool __IsMT, typename CreationPolicy, template <typename> class LifetimePolicy>
	bool Singleton<T, __IsMT, CreationPolicy, LifetimePolicy>::m_bDestroy = false;
	
	template<typename T, bool __IsMT, typename CreationPolicy, template <typename> class LifetimePolicy>
	typename Singleton<T, __IsMT, CreationPolicy, LifetimePolicy>::UserSuppliedDestroy Singleton<T, __IsMT, CreationPolicy, LifetimePolicy>::m_pFunc = 0;

	template<typename T, bool __IsMT, typename CreationPolicy, template <typename> class LifetimePolicy>
	typename Singleton<T, __IsMT, CreationPolicy, LifetimePolicy>::LockType Singleton<T, __IsMT, CreationPolicy, LifetimePolicy>::m_lock;

}








#endif