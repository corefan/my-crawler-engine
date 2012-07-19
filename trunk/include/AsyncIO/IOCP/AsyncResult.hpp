#ifndef __IOCP_ASYNC_RESULT_HPP
#define __IOCP_ASYNC_RESULT_HPP

#include "../Basic.hpp"
#include "Pointer.hpp"
#include "ObjectFactory.hpp"
#include "../../MultiThread/Tls.hpp"
#include <functional>
#include <type_traits>


namespace async
{
	namespace iocp
	{

		// 回调接口
		typedef std::tr1::function<void(u_long , u_long )> CallbackType;
		static std::tr1::_Ph<1>	_Error;
		static std::tr1::_Ph<2> _Size;
		


		// Allocate IO Callback
	
		template < typename T >
		struct IOAsyncCallback;

		struct AsyncCallbackBase;
		struct AsyncCallback;
		typedef Pointer< AsyncCallbackBase, IOAsyncCallback<AsyncCallback> > AsyncCallbackBasePtr;

		

		//---------------------------------------------------------------------------
		// class AsyncCallbackBase

		struct AsyncCallbackBase
			: public OVERLAPPED
		{
			AsyncCallbackBase()
			{
				RtlZeroMemory((OVERLAPPED *)this, sizeof(OVERLAPPED));
			}

			virtual ~AsyncCallbackBase()
			{}

			virtual void Invoke(AsyncCallbackBase *p, u_long size, u_long error) = 0;

			template<typename KeyT, typename OverlappedT>
			static void Call(KeyT *key, OverlappedT *overlapped, u_long size, u_long error)
			{
				AsyncCallbackBase *p(static_cast<AsyncCallback *>(overlapped));
				
				p->Invoke(p, error, size);
			}
		};


		//---------------------------------------------------------------------------
		// class AsyncCallback

		struct AsyncCallback
			: public AsyncCallbackBase
		{
			CallbackType handler_;

			explicit AsyncCallback(const CallbackType &callback)
				: handler_(callback)
			{}
			virtual ~AsyncCallback();
		public:
			virtual void Invoke(AsyncCallbackBase *p, u_long size, u_long error);

		private:
			AsyncCallback();
		};

		

		// Memory Pool
		template< >
		struct ObjectFactory<AsyncCallback>
		{
			typedef memory::FixedMemoryPool<true, sizeof(AsyncCallback)>	PoolType;
			typedef ObjectPool<PoolType>									ObjectPoolType;
		};


		template < typename T, typename HandlerT >
		inline AsyncCallbackBase *MakeAsyncCallback(const HandlerT &handler)
		{
			return IOAsyncCallback<T>()(handler);
		}

		namespace detail
		{
			template < typename CallbackT >
			struct TlsMemoryPool
			{
				typedef typename std::tr1::aligned_storage<
					sizeof(CallbackT),
					std::tr1::alignment_of<CallbackT>::value 
				>::type StorageBuffer;

				static __declspec(thread) StorageBuffer buf_;

				template < typename HandlerT >
				static CallbackT *ObjectAllocate(const HandlerT &handler)
				{
					return ::new (&buf_) CallbackT(handler);
				}

				static void ObjectDeallocate(CallbackT *p)
				{
					p->~CallbackT();
				}
			};

			template< typename CallbackT >
			__declspec(thread) typename TlsMemoryPool<CallbackT>::StorageBuffer TlsMemoryPool<CallbackT>::buf_;

			typedef TlsMemoryPool<AsyncCallback> TlsAsyncMemoryPool;
		}


		// Pointer Realase
		template < >
		struct IOAsyncCallback<AsyncCallback>
		{
			template < typename HandlerT >
			AsyncCallbackBase *operator()(const HandlerT &handler)
			{
				//return detail::TlsAsyncMemoryPool::ObjectAllocate(handler);
				return reinterpret_cast<AsyncCallbackBase *>(ObjectAllocate<AsyncCallback>(handler));
			}

			void operator()(AsyncCallbackBase *p)
			{
				//detail::TlsAsyncMemoryPool::ObjectDeallocate(reinterpret_cast<AsyncCallback *>(p));
				ObjectDeallocate<AsyncCallback>(static_cast<AsyncCallback *>(p));
			}
		};

	}
}



#endif