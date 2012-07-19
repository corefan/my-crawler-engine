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
			// ����߳��Ƿ����̳߳���
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

		// ��⵱ǰ�Ƿ����߳̽��з���

		template<typename Owner>
		class CallStack
		{
		public:
			// ��ջ������owner
			class Context;
			
		private:
			// ��ջ���ĵ���
			static TssPtr<Context> top_;

		public:
			// ���owner�Ƿ���ջ��
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
			Owner *owner_;		// owner��context����
			Context *next_;		// ��ջ�ϵ���һ��Ԫ��

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