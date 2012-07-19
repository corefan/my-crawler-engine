#ifndef __LOG_THREAD_LOG_HPP
#define __LOG_THREAD_LOG_HPP


#include <shlwapi.h>
#pragma comment(lib, "shlwapi")

namespace logsystem
{

	namespace impl
	{
		// -----------------------------------------------------
		// class ThreadLog

		class ThreadLogName
		{
		public:
			template<typename CharT>
			static bool CreateDir(const CharT *szPath)
			{
				return _CreateDirImpl(szPath);
			}

			template<typename T, size_t nSize>
			static const T *Make(T (&szFile)[nSize])
			{
				return _MakeImpl(szFile, nSize);
			}

		private:
			static bool _CreateDirImpl(const char *szPath)
			{
				return ::CreateDirectoryA(szPath, NULL) ||
					::GetLastError() == ERROR_ALREADY_EXISTS;
			}

			static bool _CreateDirImpl(const wchar_t *szPath)
			{
				return ::CreateDirectoryW(szPath, NULL) ||
					::GetLastError() == ERROR_ALREADY_EXISTS;
			}

			static const char *_MakeImpl(char *pszFile, size_t nSize)
			{
				::GetModuleFileNameA(NULL, pszFile, nSize);

				SYSTEMTIME gmt = {0};
				::GetSystemTime(&gmt);

				char *pszFileName = ::PathFindExtensionA(pszFile);

				/*30 ~ _tcslen([%d-%d-%d][TID=%.4x].log)*/
				sprintf_s(pszFileName, 30, "[%d-%d-%d][TID=%.4x].log", 
					gmt.wYear, gmt.wMonth, gmt.wDay, ::GetCurrentThreadId());

				return pszFile;
			}

			static const wchar_t *_MakeImpl(wchar_t *pszFile, size_t nSize)
			{
				::GetModuleFileNameW(NULL, pszFile, nSize);

				SYSTEMTIME gmt = {0};
				::GetSystemTime(&gmt);

				wchar_t *pszFileName = ::PathFindExtensionW(pszFile);

				/*30 ~ _tcslen([%d-%d-%d][TID=%.4x].log)*/
				swprintf_s(pszFileName, 30, L"[%d-%d-%d][TID=%.4x].log", 
					gmt.wYear, gmt.wMonth, gmt.wDay, ::GetCurrentThreadId());

				return pszFile;
			}

		};


		template<typename LogT, typename NameT = ThreadLogName>
		class ThreadLog
		{
			typedef LogT							LogType;
			typedef NameT							NameMakerType;

		public:
			typedef typename LogType::char_type		char_type;
			typedef typename LogType::size_type		size_type;

		private:
			// 线程本地存储
			/*__declspec(thread) */std::auto_ptr<LogType> log_;

		public:
			ThreadLog()
			{
				_Init();
			}
			ThreadLog(const char_type *path)
				: log_(new LogType())
			{
				log_->Open(path);
			}
			~ThreadLog()
			{

			}

		private:
			ThreadLog(const ThreadLog &);
			ThreadLog &operator=(const ThreadLog &);

		public:
			LogType &GetLog()
			{
				return *log_;
			}
			const LogType &GetLog() const
			{
				return *log_;
			}

		private:
			void _Init()
			{
				char_type szFile[MAX_PATH] = {0};
				log_.reset(new LogType(NameMakerType::Make(szFile)));
			}
		};
	}
	

	

}



#endif