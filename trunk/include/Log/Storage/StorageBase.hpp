#ifndef __LOG_STORAGE_BASE_HPP
#define __LOG_STORAGE_BASE_HPP


namespace logsystem
{
	namespace storage
	{
		//--------------------------------------------------------------------------
		// class StorageBase

		template<typename CharT>
		struct StorageBase
		{
			typedef CharT	char_type;
			typedef size_t	size_type;
		};


		// -------------------------------------------------------------------------
		// interface ILogStorage

		template<typename CharT>
		struct ILogStorage
		{
			typedef CharT	char_type;
			typedef size_t	size_type;

		public:
			virtual ~ILogStorage() {}

			virtual void Put(char_type ch) = 0;
			virtual void Put(size_t szCount, char_type ch) = 0;
			virtual void Put(const char_type *pStr, size_t szCount) = 0;
			virtual void Put(const char_type *fmt, va_list args) = 0;

			virtual void Flush() = 0;
		};
	}
}


#endif