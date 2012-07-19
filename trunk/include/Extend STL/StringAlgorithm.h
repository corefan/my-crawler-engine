#ifndef __STRING_ALGORITHM_H
#define __STRING_ALGORITHM_H

#include <iomanip>
#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <assert.h>
#include <vector>
#include <atlenc.h>


namespace stdex
{
	namespace detail
	{
		template < typename CharT, typename FuncT >
		struct ret_helper
		{
			std::basic_string<CharT> &str_;
			FuncT &func_;
			ret_helper(std::basic_string<CharT> &str, FuncT &func)
				: str_(str)
				, func_(func)
			{}

			operator std::basic_string<CharT>() const
			{
				std::basic_string<CharT> tmp;
				tmp.resize(str_.length());
				std::transform(str_.begin(), str_.end(), tmp.begin(), func_);

				return tmp;
			}

			operator size_t() const
			{
				std::transform(str_.begin(), str_.end(), str_.begin(), func_);
				return str_.length();
			}
		};
	}

	template < typename CharT >
	detail::ret_helper<CharT, decltype(::toupper)> ToUpper(std::basic_string<CharT> &str)
	{
		return detail::ret_helper<CharT, decltype(::toupper)>(str, ::toupper);
	}
	
	template < typename CharT >
	detail::ret_helper<CharT, decltype(::toupper)> ToUpper(const std::basic_string<CharT> &str)
	{
		return detail::ret_helper<CharT, decltype(::toupper)>(const_cast<std::basic_string<CharT> &>(str), ::toupper);
	}

	template < typename CharT, size_t N >
	inline void ToUpper(CharT (&str)[N])
	{
		std::transform(str, str + N, str, ::toupper);
	}


	template < typename CharT >
	detail::ret_helper<CharT, decltype(::tolower)> ToLower(std::basic_string<CharT> &str)
	{
		return detail::ret_helper<CharT, decltype(::tolower)>(str, ::tolower);
	}

    template < typename CharT >
    detail::ret_helper<CharT, decltype(::tolower)> ToLower(const std::basic_string<CharT> &str)
    {
        return detail::ret_helper<CharT, decltype(::tolower)>(const_cast<std::basic_string<CharT> &>(str), ::tolower);
    }

	template<typename CharT, size_t N>
	inline void ToLower(CharT (&str)[N])
	{
		std::transform(str, str + N, str, ::tolower);
	}

	template< typename CharT >
	inline void TrimLeft(std::basic_string<CharT> &str)
	{
		str.erase(0, str.find_first_not_of(' '));
	}

	template< typename CharT >
	inline void TrimRight(std::basic_string<CharT> &str)
	{
		str.erase(str.find_last_not_of(' ') + 1);
	}

	template< typename CharT >
	inline void Trim(std::basic_string<CharT> &str)
	{
		str.erase(0, str.find_first_not_of(' '));
		str.erase(str.find_last_not_of(' ') + 1);
	}

	template<typename CharT>
	inline void Erase(std::basic_string<CharT> &str, const CharT &charactor)
	{
		str.erase(remove_if(str.begin(), str.end(), 
			bind2nd(std::equal_to<CharT>(), charactor)), str.end());
	}

	template<typename CharT>
	inline void Replace(std::basic_string<CharT> &str, const std::basic_string<CharT> &strObj, const std::basic_string<CharT> &strDest)
	{
		while(true)
		{
			CharT pos = str.find(strObj);

			if( pos != std::basic_string<CharT>::npos )
				str.replace(pos, strObj.size(), strDest);
			else
				break;
		}
		
	}

	template < typename CharT >
	inline bool is_start_with(const std::basic_string<CharT> &str, const std::basic_string<CharT> &strObj)
	{
		return str.compare(0, strObj.size(), strObj) == 0;
	}

	template<typename CharT>
	inline bool is_end_with(const std::basic_string<CharT> &str, const std::basic_string<CharT> &strObj)
	{
		return str.compare(str.size() - strObj.size(), strObj.size(), strObj) == 0;
	}

	template < typename CharT >
	inline std::basic_stringstream<CharT> &operator>>(std::basic_stringstream<CharT> &os, std::basic_string<CharT> &str)
	{
		str = os.str();
		return os;
	}

	template<typename CharT>
    inline std::vector<std::basic_string<CharT>>& Split(std::vector<std::basic_string<CharT>> &seq, const std::basic_string<CharT> &str, CharT separator)
	{
		if( str.empty() )
			return seq;

		std::basic_stringstream<CharT> iss(str);
		for(std::basic_string<CharT> s; std::getline(iss, s, separator); )
		{
            std::basic_string<CharT> val;
			std::basic_stringstream<CharT> isss(s);

			isss >> val;
			seq.push_back(val);
		}

		return seq;
	}

	template < typename CharT >
	inline std::basic_string<CharT> Split(const std::basic_string<CharT> &str, CharT separator, size_t index)
	{
		if( str.empty() )
			return std::basic_string<CharT>();

		std::basic_stringstream<CharT> iss(str);
		std::vector<std::basic_string<CharT>> tmp;

		for(std::basic_string<CharT> s; std::getline(iss, s, separator); )
		{
			std::basic_string<CharT> val;
			std::basic_stringstream<CharT> isss(s);

			isss >> val;
			tmp.push_back(val);
		}

		if (index >= tmp.size())
		{
			assert(index < tmp.size());
			return std::basic_string<CharT>();
		}
		
		return tmp[index];
	}


	// ---------------------
	template<typename T, typename CharT>
	inline T ToNumber(const std::basic_string<CharT> &str)
	{
		T num(0);
		std::basic_istringstream<CharT>(str) >> num;

		return num;
	}
	template<typename T, typename CharT>
	inline T ToNumber(const CharT *str)
	{
		std::basic_string<CharT> tmp(str);
		return ToNumber<T>(tmp);
	}

	template<typename T, typename CharT>
	inline void ToNumber(T &val, const std::basic_string<CharT> &str)
	{
		std::basic_istringstream<CharT>(str) >> val;
	}
	template<typename T, typename CharT>
	inline void ToNumber(T &val, const CharT *str)
	{
		std::basic_string<CharT> tmp(str);
		ToNumber(val, tmp);
	}
	
	template<typename CharT>
	inline void ToNumber(bool &val, const std::basic_string<CharT> &str)
	{
		std::basic_istringstream<CharT>(str) >> std::boolalpha >> val;
	}

	template<typename T, typename CharT, size_t N>
	inline void ToNumber(T &val, const CharT (&str)[N])
	{
		std::basic_istringstream<CharT>(str) >> val;
	}
	
	template<typename T, typename CharT>
	inline void ToString(const T &val, std::basic_string<CharT> &str)
	{
		std::basic_ostringstream<CharT> temp;
		temp << val;

		str = temp.str();
	}

	template<typename T, typename StringT>
	inline void ToString(const T &val, StringT &str, int prec)
	{
		std::basic_ostringstream<typename StringT::value_type> temp;
		temp << std::setiosflags(std::ios::fixed) << std::setprecision(prec) << val;

		str = temp.str();
	}

	template<typename StringT>
	inline void ToString(bool val, StringT &str)
	{
		std::basic_ostringstream<typename StringT::value_type> temp;
		temp << std::boolalpha << val;

		str = temp.str();
	}

	template<typename StringT, typename T>
	inline StringT ToString(const T &val)
	{
		std::basic_ostringstream<typename StringT::value_type> temp;
		temp << val;

		return temp.str();
	}

	template<typename StringT, typename T>
	inline StringT ToString(const T &val, int prec)
	{
		std::basic_ostringstream<typename StringT::value_type> temp;
		temp << std::setiosflags(std::ios::fixed) << std::setprecision(prec) << val;

		return temp.str();
	}

}


#endif