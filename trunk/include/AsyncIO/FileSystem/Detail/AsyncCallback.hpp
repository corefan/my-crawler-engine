#ifndef __FILE_SYSTEM_ASYNC_CALLBACK_HPP
#define __FILE_SYSTEM_ASYNC_CALLBACK_HPP



namespace async
{
	namespace iocp
	{

		// forward declare 
		typedef std::tr1::function<void(u_long, u_long, FILE_NOTIFY_INFORMATION *)> FileChangeCallbackType;

		struct FileChangeCallback;


		// IO Callback Allocator & Deallocator
		template < >
		struct IOAsyncCallback< FileChangeCallback >
		{
			inline FileChangeCallback *operator()(const FileChangeCallbackType &handler)
			{
				return ObjectAllocate<FileChangeCallback>(handler);
			}

			void operator()(FileChangeCallback *p)
			{
				ObjectDeallocate<FileChangeCallback>(p);
			}	
		};

		typedef Pointer<FileChangeCallback, IOAsyncCallback<FileChangeCallback>> FileChangeCallbackPtr;
	
		template < typename HandlerT >
		inline FileChangeCallback *MakeAsyncCallback(const HandlerT &handler)
		{
			return IOAsyncCallback<FileChangeCallback>()(handler);
		}

	
		// IO Callback
		struct FileChangeCallback
			: public iocp::AsyncCallbackBase
		{
			static const size_t BUFFER_LEN	= 64 * 1024;
			static const size_t PADDING		= sizeof(u_long);

			FileChangeCallbackType handler_;
			std::tr1::aligned_storage<BUFFER_LEN, PADDING>::type buffer_;

			FileChangeCallback(const FileChangeCallbackType &handler)
				: handler_(handler)
			{
			}

			virtual ~FileChangeCallback()
			{}

			virtual void Invoke(AsyncCallbackBase *p, u_long size, u_long error)
			{
				FILE_NOTIFY_INFORMATION *notifyInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(&buffer_);

				handler_(size, error, notifyInfo);

				FileChangeCallbackPtr ptr(reinterpret_cast<FileChangeCallback *>(p));
			}
		};


		// Memory Pool
		template < >
		struct ObjectFactory< FileChangeCallback >
		{
			typedef memory::FixedMemoryPool<true, sizeof(FileChangeCallback)>	PoolType;
			typedef ObjectPool<PoolType>										ObjectPoolType;
		};

	}
}





#endif