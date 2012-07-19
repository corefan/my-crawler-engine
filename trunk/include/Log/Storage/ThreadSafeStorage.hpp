#ifndef __LOG_THREAD_SAFE_HPP
#define __LOG_THREAD_SAFE_HPP

#include "StorageBase.hpp"
#include "../MultiThread/Lock.hpp"

namespace logsystem
{

	namespace storage
	{
		// -----------------------------------------------------
		// class ThreadSafeLog

		template<typename LogT>
		class ThreadSafeAgent
		{
			typedef async::thread::AutoSpinLock				LockType;

			LockType lock_;
			LogT &log_;

		public:
			ThreadSafeAgent(LogT &log)
				: log_(log)
			{
				lock_.Lock();
			}
			~ThreadSafeAgent()
			{
				lock_.Unlock();
			}

		public:
			LogT &operator()()
			{
				return log_;
			}
		};

		template<typename LogT>
		inline ThreadSafeAgent<LogT> ThreadSafe(LogT &log)
		{
			return ThreadSafeAgent<LogT>(log);
		}

		template<typename LogT, typename T>
		inline ThreadSafeAgent<LogT> &operator<<(ThreadSafeAgent<LogT> &log, const T &val)
		{
			log() << val;
			return log;
		}

		// ---------------------------------------------------------------------
		// class ThreadSafeStorage

		template<typename LockT, typename ImplStorageT, typename Base = StorageBase<typename ImplStorageT::char_type>>
		class ThreadSafeStorage
			: public Base
		{
		private:
			typedef LockT									LockType;
			typedef async::thread::AutoLock<LockType>		AutoLock;

			typedef Base									StorageType;
			typedef ImplStorageT							ImpleStorageType;

		public:
			typedef typename StorageType::char_type			char_type;
			typedef typename StorageType::size_type			size_type;

		private:
			LockType lock_;
			ImpleStorageType implStorage_;

		public:
			ThreadSafeStorage()
			{}
			template<typename ArgT>
			ThreadSafeStorage(const ArgT &arg)
				: implStorage_(arg)
			{}
			template<typename ArgT1, typename ArgT2>
			ThreadSafeStorage(const ArgT1 &arg1, const ArgT2 &arg2)
				: implStorage_(arg1, arg2)
			{}

		public:
			void Put(char_type ch)
			{
				AutoLock lock(lock_);
				implStorage_.Put(ch);
			}

			void Put(size_t count, char_type ch)
			{
				AutoLock lock(lock_);
				implStorage_.Put(count, ch);
			}

			void Put(const char_type *pStr, size_t count)
			{
				AutoLock lock(lock_);
				implStorage_.Put(pStr, count);
			}

			void Put(const char_type *pStr, va_list args)
			{
				AutoLock lock(lock_);
				implStorage_.Put(pStr, args);
			}

			void Flush()
			{
				AutoLock lock(lock_);
				implStorage_.Flush();
			}
		};
	}
	
}


#endif