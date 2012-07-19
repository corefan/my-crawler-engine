#ifndef __ALOGRITHM_BASE64_HPP
#define __ALOGRITHM_BASE64_HPP


#include "../Unicode/string.hpp"


namespace algorithm
{
	namespace base64
	{

		template < typename CharT >
		inline std::basic_string<CharT> Base64Encode(const std::basic_string<CharT> &val)
		{
			std::string sSrcString = unicode::to_a(val);
			int len = ATL::Base64EncodeGetRequiredLength(sSrcString.length());
			
			std::string dest;
			dest.resize(len + 1);
			ATL::Base64Encode(reinterpret_cast<const BYTE *>(sSrcString.c_str()), sSrcString.length(), &dest[0], &len);
			dest.resize(len);

			return unicode::translate_t<CharT>::utf(dest);
		}

		template < typename CharT >
		inline std::basic_string<CharT> Base64Decode(const std::basic_string<CharT> &val)
		{
			std::string sSrcString = unicode::to_a(val);
			int len = ATL::Base64DecodeGetRequiredLength(sSrcString.length()) + 1;
			std::string vec;
			vec.resize(len + 1);
			ATL::Base64Decode(sSrcString.c_str(), sSrcString.length(), reinterpret_cast<BYTE*>(&vec[0]), &len);
			vec.resize(len);

			return unicode::translate_t<CharT>::utf(vec);
		}
	}
}





#endif