#ifndef __LOG_STORAGE_HPP
#define __LOG_STORAGE_HPP


#include "StorageBase.hpp"

#include <cassert>
#include <array>
#include <functional>


namespace logsystem
{
	
	namespace storage
	{
		// -------------------------------------------------------------------------
		// class FileStorage

		template<typename CharT, bool _Own = true, template<typename> class Base = StorageBase>
		class FileStorageT
			: public Base<CharT>
		{
		public:
			typedef Base<CharT>						BaseType;

			typedef typename BaseType::char_type	char_type;
			typedef typename BaseType::size_type	size_type;

		protected:
			FILE *file_;

		public:
			FileStorageT()
				: file_(NULL)
			{}
			FileStorageT(FILE *fp)
				: file_(fp)
			{}
			FileStorageT(const CharT * const szFile, bool bCheckOwn = true)
			{
				_OpenImpl(szFile, bCheckOwn);
			}
			template<typename LogT>
			FileStorageT(LogT &log)
				: file_(log.GetStorage().file_)
			{}
			~FileStorageT()
			{
				if( _Own )
					Close();
			}

		public:
			bool Good() const
			{
				return file_ != NULL;
			}

			void Open(const char_type * const file, bool bCheckOwn = true)
			{
				assert(file_	== NULL);
				_OpenImpl(file, bCheckOwn);
			}

			void Close()
			{
				if( file_ != NULL )
				{
					::fclose(file_);
					file_ = NULL;
				}
			}

			void SetFile(FILE *fp)
			{
				file_ = fp;
			}

		public:
			void Put(char_type ch)
			{
				_PutImpl(ch);
			}

			void Put(size_type szCount, char_type ch)
			{
				_PutImpl(szCount, ch);
			}

			void Put(const char_type *pStr, size_type szCount)
			{
				_PutImpl(pStr, szCount);
			}

			void Put(const char_type *fmt, va_list args)
			{
				_PutImpl(fmt, args);
			}

			void Flush()
			{
				if( file_ != NULL )
					::fflush(file_);
			}

		private:
			void _OpenImpl(const char * const file, bool bCheckOwn)
			{
				assert(_Own || !bCheckOwn);

				::fopen_s(&file_, file, "wb+");
			}
			void _OpenImpl(const wchar_t * const file, bool bCheckOwn)
			{
				assert(_Own || !bCheckOwn);

				::_wfopen_s(&file_, file, L"wb+");
			}

			void _PutImpl(char ch)
			{
				::putc(ch, file_);
			}
			void _PutImpl(wchar_t ch) 
			{
				::putwc(ch, file_);
			}

			void _PutImpl(size_type szCount, char ch)
			{
				while(szCount--)
					::putc(ch, file_);
			}
			void _PutImpl(size_type szCount, wchar_t ch)
			{
				while(szCount--)
					::putwc(ch, file_);
			}

			void _PutImpl(const char *pStr, size_type szCount)
			{
				::fwrite(pStr, sizeof(char), szCount, file_);
			}
			void _PutImpl(const wchar_t *pStr, size_type szCount) 
			{
				::fwrite(pStr, sizeof(wchar_t), szCount, file_);
			}

			void _PutImpl(const char *fmt, va_list args)
			{
				::vfprintf(file_, fmt, args);
			}
			void _PutImpl(const wchar_t *fmt, va_list args)
			{
				::vfwprintf(file_, fmt, args);
			}
		};	


		// -------------------------------------------------------------------------
		// class DebugStorageT

		template<typename CharT, template<typename> class Base = StorageBase>
		class DebugStorageT
			: public Base<CharT>
		{
		public:
			typedef Base<CharT>						BaseType;

			typedef typename BaseType::char_type	char_type;
			typedef typename BaseType::size_type	size_type;

		private:
			enum { LOG_BUFFER_SIZE = 512 };

		public:
			bool Good() const
			{
				return true;
			}

		public:
			void Put(char_type ch)
			{
				return _PutImpl(ch);
			}

			void Put(size_type szCount, char_type ch)
			{
				return _PutImpl(szCount, ch);
			}

			void Put(const char_type *pStr, size_type szCount)
			{
				return _PutImpl(pStr, szCount);
			}

			void Put(const char_type *fmt, va_list args)
			{
				_PutImpl(fmt, args);
			}

			void Flush()
			{
			}

		private:
			void _PutImpl(char ch)
			{
				char buf[LOG_BUFFER_SIZE] = {0};
				sprintf_s(buf, "%c", ch);

				::OutputDebugStringA(buf);
			}
			void _PutImpl(wchar_t ch) 
			{
				wchar_t buf[LOG_BUFFER_SIZE] = {0};
				swprintf_s(buf, L"%C", ch);

				::OutputDebugStringW(buf);
			}

			void _PutImpl(size_type szCount, char ch)
			{
				char buf[LOG_BUFFER_SIZE] = {0};

				while(szCount--)
				{
					sprintf_s(buf, "%c", ch);
					::OutputDebugStringA(buf);
				}
			}
			void _PutImpl(size_type szCount, wchar_t ch)
			{
				wchar_t buf[LOG_BUFFER_SIZE] = {0};

				while(szCount--)
				{
					swprintf_s(buf, L"%C", ch);
					::OutputDebugStringW(buf);
				}
			}

			void _PutImpl(const char *pStr, size_type szCount)
			{
				::OutputDebugStringA(pStr);
			}

			void _PutImpl(const wchar_t *pStr, size_type szCount) 
			{
				::OutputDebugStringW(pStr);
			}

			void _PutImpl(const char *fmt, va_list args)
			{
				char buf[LOG_BUFFER_SIZE] = {0};
				_vsnprintf_s(buf, LOG_BUFFER_SIZE, fmt, args);

				::OutputDebugStringA(buf);
			}

			void _PutImpl(const wchar_t *fmt, va_list args)
			{
				wchar_t buf[LOG_BUFFER_SIZE] = {0};
				_vsnwprintf_s(buf, LOG_BUFFER_SIZE, fmt, args);

				::OutputDebugStringW(buf);
			}
		};




		// -------------------------------------------------------------------------
		// class StringStorage

		template<typename StringT, typename Base = StorageBase<typename StringT::value_type>>
		class StringStorageT
			: public Base
		{
		public:
			typedef Base							BaseType;

			typedef typename BaseType::char_type	char_type;
			typedef typename BaseType::size_type	size_type;

			typedef StringT							StorageType;

		private:
			enum { LOG_BUFFER_SIZE = 1024 };
			StorageType storage_;

		public:
			StorageType &operator()()
			{
				return storage_;
			}
			const StorageType &operator()() const
			{
				return storage_;
			}

		public:
			void Put(char_type ch)
			{
				storage_.append(1, ch);
			}

			void Put(size_type count, char_type ch)
			{
				storage_.append(count, ch);
			}

			void Put(const char_type *pStr, size_type count)
			{
				storage_.append(pStr, pStr + count);
			}

			void Put(const char_type *fmt, va_list args)
			{
				return _PutImpl(fmt, args);
			}

			void Flush()
			{

			}

		private:
			void _PutImpl(const char *fmt, va_list args)
			{
				char buf[LOG_BUFFER_SIZE] = {0};
				int cch = _vsnprintf_s(buf, LOG_BUFFER_SIZE, fmt, args);
				if( cch	< 0 )
					cch = LOG_BUFFER_SIZE;

				storage_.append(buf, cch);
			}
			void _PutImpl(const wchar_t *fmt, va_list args)
			{
				wchar_t buf[LOG_BUFFER_SIZE] = {0};
				int cch = _vsnwprintf_s(buf, LOG_BUFFER_SIZE, fmt, args);
				if( cch	< 0 )
					cch = LOG_BUFFER_SIZE;

				storage_.append(buf, cch);
			}
		};



		// -------------------------------------------------------------------------
		// class MultiStorage

		template<typename CharT, typename StorageContainer = std::deque<ILogStorage<CharT> *> >
		class MultiStorage
			: public StorageBase<CharT>
		{
			typedef MultiStorage<CharT, StorageContainer> ThisType;
			typedef typename StorageContainer::value_type ValueType;

		public:
			typedef typename StorageBase<CharT>::char_type		char_type;
			typedef typename StorageBase<CharT>::size_type		size_type;

		private:
			StorageContainer storages_;
			size_t pos_;

		public:
			MultiStorage()
				: pos_(0)
			{}

		public:
			void Clear()
			{
				storages_.clear();
				pos_ = 0;
			}

			void Add(ILogStorage<char_type> &stg)
			{
				storages_.push_back(&stg);
			}

		public:
			void Put(char_type ch)
			{
				for(StorageContainer::const_iterator iter = storages_.begin(); iter != storages_.end(); ++iter)
					(*iter)->Put(ch);
			}

			void Put(size_type count, char_type ch)
			{
				for(StorageContainer::const_iterator iter = storages_.begin(); iter != storages_.end(); ++iter)
					(*iter)->Put(count, ch);
			}

			void Put(const char_type *pStr, size_type count)
			{
				for(StorageContainer::const_iterator iter = storages_.begin(); iter != storages_.end(); ++iter)
					(*iter)->Put(pStr, count);
			}

			void Put(const char_type *fmt, va_list args)
			{
				for(StorageContainer::const_iterator iter = storages_.begin(); iter != storages_.end(); ++iter)
					(*iter)->Put(fmt, args);
			}

			void Flush()
			{
				std::for_each(storages_.begin(), storages_.end(),
					std::tr1::bind(&ILogStorage<CharT>::Flush, std::tr1::placeholders::_1));
			}
		};
	}


	
}




#endif 