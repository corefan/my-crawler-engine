#ifndef TUT_CONFIG_H_GUARD
#define TUT_CONFIG_H_GUARD

// --------------------------------------------
// no thing


#include <cstring>
#include <cassert>
#include <cmath>



#include <typeinfo>
#include <limits>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <memory>

#include <string>
#include <vector>
#include <map>
#include <set>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

#include <atlbase.h>
#include <atlconv.h>

#include <windows.h>


namespace tut
{
	typedef std::basic_string<TCHAR>			tString;
	typedef std::basic_iostream<TCHAR>			tIOStream;
	typedef std::basic_ostream<TCHAR>			tOStream;
	typedef std::basic_stringstream<TCHAR>		tStringStream;
	typedef std::basic_ostringstream<TCHAR>		tOStringStream;

	typedef std::basic_ofstream<TCHAR>			tOfstream;
}



#endif
