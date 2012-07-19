#ifndef __LOG_LOG_HANDLER_HPP
#define __LOG_LOG_HANDLER_HPP



namespace logsystem
{
	namespace Internal
	{
		template<size_t _N, typename T>
		inline void ItoaImpl(char (&arr)[_N], T val)
		{
			::_itoa_s(val, arr, 10);
		}

		template<size_t _N, typename T>
		inline void ItoaImpl(wchar_t (&arr)[_N], T val)
		{
			::_itow_s(val, arr, 10);
		}


		template<size_t _N>
		inline size_t StrlenImpl(const char (&arr)[_N])
		{
			return ::strlen(arr);
		}

		template<size_t _N>
		inline size_t StrlenImpl(const wchar_t (&arr)[_N])
		{
			return ::wcslen(arr);
		}

		inline size_t StrlenImpl(const char *text)
		{
			return ::strlen(text);
		}

		inline size_t StrlenImpl(const wchar_t *text)
		{
			return ::wcslen(text);
		}


		template<size_t _N>
		void SprintfDoubleImp(char (&arr)[_N], double d)
		{
			_snprintf_s(arr, _N, "%f", d);
		}
		template<size_t _N>
		inline void SprintfDoubleImp(wchar_t (&arr)[_N], double d)
		{
			_snwprintf_s(arr, _N, L"%f", d);
		}

		template<size_t _N>
		inline void SprintfLDoubleImp(char (&arr)[_N], long double d)
		{
			_snprintf_s(arr, _N, "%lf", d);
		}
		template<size_t _N>
		inline void SprintfLDoubleImp(wchar_t (&arr)[_N], long double d)
		{
			_snwprintf_s(arr, _N, L"%lf", d);
		}

		template<size_t _N>
		inline void SprintfInt64Imp(char (&arr)[_N], __int64 d)
		{
			_snprintf_s(arr, _N, "%I64d", d);
		}
		template<size_t _N>
		inline void SprintfLDoubleImp(wchar_t (&arr)[_N], __int64 d)
		{
			_snwprintf_s(arr, _N, L"%I64d", d);
		}
	}


	// Helper function
	template<typename CharT, size_t _N, typename T>
	inline void ItoaT(CharT (&arr)[_N], T val)
	{
		return Internal::ItoaImpl(arr, val);
	}

	template<typename CharT, size_t _N>
	inline size_t StrlenT(const CharT (&arr)[_N])
	{
		return Internal::StrlenImpl(arr);
	}
	template<typename CharT>
	inline size_t StrlenT(const CharT *text)
	{
		return Internal::StrlenImpl(text);
	}

	template<typename CharT, size_t _N>
	inline void SprintfDouble(CharT (&arr)[_N], double d)
	{
		return Internal::SprintfDoubleImp(arr, d);
	}

	template<typename CharT, size_t _N>
	inline void SprintfLDouble(CharT (&arr)[_N], double d)
	{
		return Internal::SprintfLDoubleImp(arr, d);
	}

	template<typename CharT, size_t _N>
	inline void SprintfInt64(CharT (&arr)[_N], __int64 d)
	{
		return Internal::SprintfInt64Imp(arr, d);
	}



	// function Ends
	template<typename StreamT>
	inline StreamT &Ends(StreamT &log)
	{
		log.NewLine();

		return log;
	}

	// function Endl
	template<typename StreamT>
	inline StreamT &Endl(StreamT &log)
	{
		log.NewLine();
		log.Flush();

		return log;
	}

	// function Space
	template<typename StreamT>
	inline StreamT &Space(StreamT &log)
	{
		log.WhiteSpace();

		return log;
	}

	


	//-------------------------------------------------------------------
	// class Ó¦ÓÃÆ÷

	/*template<typename ValT>
	class FuncObj
	{
		typedef ValT	ValueType;

	public:
		FuncObj(const ValueType &val)
			: val_(val)
		{}

		const ValueType &operator()() const
		{
			return val_;
		}

	private:
		const ValueType &val_;
	};

	template<typename ValT>
	inline FuncObj<ValT> Step(const ValT &val)
	{
		return FuncObj<ValT>(val);
	}

	template<typename StreamT, typename ValT>
	inline StreamT &operator<<(StreamT &log, const FuncObj<ValT> &obj)
	{
		log.Step(obj());
		return log;
	}*/


	

	template<typename ValueT>
	class FuncObj
	{
		typedef void (* FuncPtr)(impl::BasicLog &, const ValueT &, bool);

	private:
		FuncPtr func_;
		ValueT val_;
		bool show_;

	public:
		FuncObj(FuncPtr func, const ValueT &val, bool show)
			: func_(func)
			, val_(val)
			, show_(show)
		{}

		void operator()(impl::BasicLog &log) const
		{
			return (*func_)(log, val_, show_);
		}
	};


	// ----------------------------------------------------
	// Level

	template<typename LogT, typename ValueT>
	inline void LevelHelper(LogT &log, const ValueT &level, bool show)
	{
		log.Priority(level, show);
	}
	

	template<typename ValueT>
	inline FuncObj<ValueT> Level(const ValueT &val, bool show = true)
	{
		return FuncObj<ValueT>(&LevelHelper, val, show);
	}


	// ----------------------------------------------------
	// Color

	enum 
	{ 
		Red = FOREGROUND_RED | FOREGROUND_INTENSITY, 
		Green = FOREGROUND_GREEN | FOREGROUND_INTENSITY, 
		Blue = FOREGROUND_BLUE | FOREGROUND_INTENSITY
	};

	template<typename StreamT>
	inline void ColorHelper(StreamT &/*stream*/, const size_t &color, bool/* show*/)
	{
		::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), color);
	}

	inline FuncObj<size_t> Color(size_t color)
	{
		return FuncObj<size_t>(&ColorHelper, color, false);
	}

	// helper function--operator<<
	template<typename StreamT, typename T>
	inline StreamT &operator<<(StreamT &log, const T &val)
	{
		log.Print(val);

		return log;
	}


	

	template<typename LogT, typename ValueT>
	inline LogT &operator<<(LogT &log, const FuncObj<ValueT> &obj)
	{
		obj(log);

		return log;
	}
}









#endif