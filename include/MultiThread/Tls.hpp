#ifndef __THREAD_TLS_HPP
#define __THREAD_TLS_HPP



namespace async
{
	namespace thread
	{

		// ----------------------------------------
		// class TssPtr

		template<typename T>
		class TssPtr
		{
		private:
			// 检测线程是否在线程池中
			DWORD tssKey_;

		public:
			TssPtr()
				: tssKey_(::TlsAlloc())
			{
				if( tssKey_ == TLS_OUT_OF_INDEXES )
					throw iocp::Win32Exception("TlsAlloc");
			}
			~TssPtr()
			{
				::TlsFree(tssKey_);
			}

		public:
			operator T*()
			{
				return static_cast<T *>(::TlsGetValue(tssKey_));
			}
			T *operator->()
			{
				return static_cast<T *>(::TlsGetValue(tssKey_));
			}

			void operator=(T *val)
			{
				::TlsSetValue(tssKey_, val);
			}
		};



		// --------------------------------------------------
		// class CallStack

		// 检测当前是否在线程进行分派

		template<typename Owner>
		class CallStack
		{
		public:
			// 在栈上设置owner
			class Context;
			
		private:
			// 在栈顶的调用
			static TssPtr<Context> top_;

		public:
			// 检测owner是否在栈上
			static bool Contains(Owner *owner)
			{
				Context *val = top_;
				while( val )
				{
					if( val->owner_ == owner )
						return true;

					val = val->next_;
				}

				return false;
			}

		};

		template<typename Owner>
		TssPtr<typename CallStack<Owner>::Context> CallStack<Owner>::top_;



		template<typename Owner>
		class CallStack<Owner>::Context
		{
		private:
			Owner *owner_;		// owner与context关联
			Context *next_;		// 在栈上的下一个元素

			friend class CallStack<Owner>;

		public:
			explicit Context(Owner *owner)
				: owner_(owner)
				, next_(CallStack<Owner>::top_)
			{
				CallStack<Owner>::top_ = this;
			}
			~Context()
			{
				CallStack<Owner>::top_ = next_;
			}

		private:
			Context(const Context &);
			Context &operator=(const Context &);
		};
	}
}






#endif