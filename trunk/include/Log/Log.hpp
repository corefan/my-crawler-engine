#ifndef __LOG_BASIC_HPP
#define __LOG_BASIC_HPP

#include <deque>

#include "Storage/Storage.hpp"
#include "Storage/CacheStorage.hpp"
#include "Storage/ThreadSafeStorage.hpp"


#include "Log/Log.hpp"
#include "Log/ScopeLog.hpp"
#include "Log/ThreadLog.hpp"

#include "Filter/Priority.hpp"
#include "Format/Format.hpp"




namespace logsystem
{
	// ----------------------------------------------------------
	// storage

	typedef storage::FileStorageT<char>			FileStorage;
	typedef storage::FileStorageT<wchar_t>		FileStorageW;

	typedef storage::DebugStorageT<char>		DebugStorage;
	typedef storage::DebugStorageT<wchar_t>		DebugStorageW;

	typedef storage::StringStorageT<std::basic_string<char>>		StringStorage;
	typedef storage::StringStorageT<std::basic_string<wchar_t>>		StringStorageW;



	// -----------------------------------------------------
	// logsystem

	typedef impl::Log<FileStorage, filter::DefaultFilter>		FileLog;
	typedef impl::Log<FileStorageW, filter::DefaultFilter>		FileLogW;

	typedef impl::Log<DebugStorage, filter::DefaultFilter>		DebugLog;
	typedef impl::Log<DebugStorageW, filter::DefaultFilter>		DebugLogW;

	typedef impl::Log<StringStorage, filter::DefaultFilter>		StringLog;
	typedef impl::Log<StringStorageW, filter::DefaultFilter>	StringLogW;



	typedef impl::FileScopeLogT<char, true, filter::DefaultFilter>		FileScopeLog;
	typedef impl::FileScopeLogT<wchar_t, true, filter::DefaultFilter>	FileScopeLogW;


	typedef impl::ThreadLog<FileLog>							ThreadFileLog;
	typedef impl::ThreadLog<FileLogW>							ThreadFileLogW;

	
	// ---------------------------------------------------------------------
	// class  MultiLog

	template< typename CharT, typename FilterT, class StorageContainer = std::deque<storage::ILogStorage<CharT>*> >
	class MultiLogT
		: public impl::Log<storage::MultiStorage<CharT, StorageContainer>, FilterT >
	{
	public:
		template<typename T>
		void Add(T &val)
		{
			GetStorage().Add(val);
		}
	};


	// ---------------------------

	template < typename LogT, typename MutexT >
	class thread_safe_helper
	{
		typedef MutexT	 Mutex;

		Mutex lock_;
		LogT &log_;

	public:
		thread_safe_helper(LogT &log)
			: log_(log)
		{
			lock_.Lock();
		}
		~thread_safe_helper()
		{
			lock_.Unlock();
		}

	public:
		LogT &operator()()
		{
			return log_;
		}
	};

	template < typename LogT, typename MutexT, typename T >
	inline thread_safe_helper< LogT, MutexT > &operator<<(thread_safe_helper<LogT, MutexT> &log, const T &val)
	{
		log() << val;
		return log;
	}


	// safe_log(multiLog)() << 12 << "34" << 5.6 << Endl;


	
	
}






#endif