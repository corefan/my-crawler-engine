#ifndef __LOG_PRIORITY_HPP
#define __LOG_PRIORITY_HPP


#include <type_traits>

namespace logsystem
{
	namespace filter
	{

		namespace internal
		{
			template<typename T>
			struct Type2Type
			{
				typedef T value_type;
			};
		}

		// 日志级别

		enum
		{
			Critical = 0x00,
			Error,
			Warning,
			Notice,
			Info,
			Debug,
			Trace,
			All
		};

		

		// -----------------------------------------------------------
		// class PriorityFilter

		template<size_t _Priority>
		struct PriorityFilter
		{
			enum { BUFFER_RAW = 8, BUFFER_LEN = 16 };


			static bool IsPrint(size_t priority)
			{
				return priority < _Priority;
			}

			template<typename CharT>
			static const CharT *Level(size_t priority)
			{
				assert(priority < All);

				const CharT (&text)[BUFFER_RAW][BUFFER_LEN] = _PriorityText(internal::Type2Type<CharT>());

				return text[priority];
			}

		private:
			template<typename CharT>
			static const CharT *_PriorityText();

			static const char (&_PriorityText(internal::Type2Type<char>))[BUFFER_RAW][BUFFER_LEN]
			{
				static const char priorityText[BUFFER_RAW][BUFFER_LEN] = 
				{
					"(Critical) "	, "(Error) "	, "(Warning) "	,
					"(Notice) "		, "(Info) "		, "(Debug) "	, 
					"(Trace) "		, "(All) "
				};

				return priorityText;
			}

			
			static const wchar_t (&_PriorityText(internal::Type2Type<wchar_t>))[BUFFER_RAW][BUFFER_LEN]
			{
				static const wchar_t priorityText[BUFFER_RAW][BUFFER_LEN] = 
				{
					L"(Critical) "	, L"(Error) "	, L"(Warning) "	,
					L"(Notice) "	, L"(Info) "	, L"(Debug) "	, 
					L"(Trace) "		, L"(All) "
				};

				return priorityText;
			}

		};

		typedef PriorityFilter<All>			DebugFilter;
		typedef PriorityFilter<Debug>		ReleaseFilter;

#ifdef _DEBUG
		typedef DebugFilter		DefaultFilter;
#else			
		typedef ReleaseFilter	DefaultFilter;
#endif

		
	}
}


#endif