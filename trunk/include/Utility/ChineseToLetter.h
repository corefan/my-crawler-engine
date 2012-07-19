#ifndef __CHINESE_2_LETTER_HPP
#define __CHINESE_2_LETTER_HPP

#include "../Extend STL/UnicodeStl.h"

namespace game_client
{
	stdex::tString GetPyLetter(const stdex::tString& src);
    std::pair<stdex::tString, stdex::tString> GetMulPy(const stdex::tString& src);
}


#endif