#ifndef __XML_BASIC_HPP
#define __XML_BASIC_HPP

#include <atlbase.h>
#include <string>


#ifndef _UNICODE
	typedef std::string tString;
#else
	typedef std::wstring tString;
#endif


#ifndef ASSERT
#define ASSERT ATLASSERT
#endif

#ifndef TRACE
#define TRACE ATLTRACE
#endif


#ifndef VERIFY
#define VERIFY ATLASSERT
#endif



#endif