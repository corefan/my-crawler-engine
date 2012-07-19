#ifndef __LOG_LOG_HPP
#define __LOG_LOG_HPP




#include <string>
#include <cstdarg>

#include "BasicLog.hpp"



namespace logsystem
{
	
	namespace impl
	{
		// ---------------------------------------------------------------------
		// class Log

		template<typename StorageT, typename FilterT>
		class Log
			: public BasicLog
		{
		public:
			typedef StorageT	StorageType;
			typedef FilterT		FilterType;

			typedef typename StorageT::size_type	size_type;
			typedef typename StorageT::char_type	char_type;


		protected:
			StorageType storage_;			// StorageT
			
		private:
			enum { BUFFER_SIZE = 64 };

		public:
			Log()
			{}

			template<typename ArgT>
			Log(const ArgT &arg)
				: storage_(arg)
			{}

			template<typename ArgT1, typename ArgT2>
			Log(const ArgT1 &arg1, const ArgT2 &arg2)
				: storage_(arg1, arg2)
			{}

		public:
			StorageT &GetStorage()
			{
				return storage_;
			}
			const StorageT &GetStorage() const
			{
				return storage_;
			}

			bool Good() const
			{
				return storage_.Good();
			}

			template<typename ArgT>
			void Open(const ArgT &arg)
			{
				assert(!storage_.Good());
				storage_.Open(arg);
			}

			void Close()
			{
				storage_.Close();
			}

			void Flush()
			{
				storage_.Flush();
			}

			Log &WhiteSpace()
			{
				_PrintString(' ');
				return *this;
			}

			Log &NewLine()
			{			
				_PrintString('\n');

				return *this;
			}
			Log &NewLine(size_type count)
			{
				_PrintString(count, '\n');

				return *this;
			}

						
			// Support endl,ends
			Log &operator<<(Log& (__cdecl *pFun)(Log&))
			{
				return pFun(*this);
			}

		public:
			Log &VTrace(const char_type *fmt, va_list arg)
			{			
				_PrintString(fmt, arg);
				return *this;
			}
			Log &Trace(const char_type *fmt, ...)
			{			
				va_list args;
				va_start(args, fmt);

				_PrintString(fmt, args);

				va_end(args);

				return *this;
			}

			// Print
			template<typename IterT>
			Log &Print(IterT first, IterT last)
			{			
				_PrintString(first, last);

				return *this;
			}
			Log &Print(const std::basic_string<char_type> &str)
			{			
				_PrintString(str);

				return *this;
			}
			template<typename T>
			Log &Print(const T &data, const char_type *fmt)
			{
				Trace(fmt, data);

				return *this;
			}
			template<typename T>
			Log &Print(const T &data)
			{
				_PrintString(data);

				return *this;
			}
			Log &Print(const char_type *pStr)
			{
				_PrintString(pStr);

				return *this;
			}

		private:
			template<typename CharT>
			void _PrintLevel(const CharT *text)
			{
				storage_.Put(text, Internal::StrlenImpl(text));
			}

		private:
			Log &_PrintString(char data)
			{
				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));
					storage_.Put(data);
				}

				return *this;
			}
			Log &_PrintString(wchar_t data)
			{
				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));
					storage_.Put(data);
				}

				return *this;
			}
			Log &_PrintString(int data)
			{
				char_type buf[BUFFER_SIZE] = {0};
				ItoaT(buf, data);

				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));
					storage_.Put(buf, Internal::StrlenImpl(buf));
				}

				return *this;
			}
			Log &_PrintString(unsigned int data)
			{
				char_type buf[BUFFER_SIZE] = {0};
				ItoaT(buf, data);

				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));
					storage_.Put(buf, Internal::StrlenImpl(buf));
				}

				return *this;
			}
			Log &_PrintString(short data)
			{
				char_type buf[BUFFER_SIZE] = {0};
				ItoaT(buf, data);

				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));
					storage_.Put(buf, Internal::StrlenImpl(buf));
				}

				return *this;
			}
			Log &_PrintString(unsigned short data)
			{
				char_type buf[BUFFER_SIZE] = {0};
				ItoaT(buf, data);

				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));

					storage_.Put(buf, Internal::StrlenImpl(buf));
				}

				return *this;
			}
			Log &_PrintString(long data)
			{
				char_type buf[BUFFER_SIZE] = {0};
				ItoaT(buf, data);

				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));
					storage_.Put(buf, Internal::StrlenImpl(buf));
				}

				return *this;
			}
			Log &_PrintString(unsigned long data)
			{
				char_type buf[BUFFER_SIZE] = {0};
				ItoaT(buf, data);

				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));
					storage_.Put(buf, Internal::StrlenImpl(buf));
				}

				return *this;
			}
			Log &_PrintString(double data)
			{
				char_type buf[BUFFER_SIZE] = {0};
				SprintfDouble(buf, data);

				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));
					storage_.Put(buf, Internal::StrlenImpl(buf));
				}

				return *this;
			}
			Log &_PrintString(long double data)
			{
				char_type buf[BUFFER_SIZE] = {0};
				SprintfLDouble(buf, data);

				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));
					storage_.Put(buf, Internal::StrlenImpl(buf));
				}

				return *this;
			}
			Log &_PrintString(__int64 data)
			{
				char_type buf[BUFFER_SIZE] = {0};
				SprintfInt64(buf, data);

				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));
					storage_.Put(buf, Internal::StrlenImpl(buf));
				}

				return *this;
			}

			template<typename IterT>
			Log &_PrintString(IterT first, IterT last)
			{			
				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));
					storage_.Put(first, last - first);
				}

				return *this;
			}
			Log &_PrintString(const char_type *pStr)
			{
				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));
					storage_.Put(pStr, Internal::StrlenImpl(pStr));
				}
				return *this;
			}
			Log &_PrintString(const std::basic_string<char_type> &str)
			{			
				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));
					storage_.Put(str.c_str(), str.size());
				}

				return *this;
			}
			Log &_PrintString(const char_type *szText, va_list arg)
			{
				if( FilterType::IsPrint(printLevel_) )
				{
					if( Show() )
						_PrintLevel(FilterType::Level<char_type>(printLevel_));	
					storage_.Put(szText, arg);
				}

				return *this;
			}
		};
	}
	

}



#endif