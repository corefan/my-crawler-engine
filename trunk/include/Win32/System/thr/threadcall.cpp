// threadcall.cpp -- Dinkum thread support functions
#include "xthread.hpp"

 #if _HAS_EXCEPTIONS
#include <stdexcept>
#include <string>

 #else /* _HAS_EXCEPTIONS */
#include <cstdio>
#include <cstdlib>
 #endif /* _HAS_EXCEPTIONS */

_STD_BEGIN

 #if _HAS_CPP0X
 #else /* _HAS_CPP0X */
 #if _HAS_EXCEPTIONS
void _Throw_Cpp_error(int)
	{	// throw error object
	_THROW_NCEE(logic_error, "C++ Thread error");
	}

void _Throw_C_error(int)
	{	// throw error object for C error
	_THROW_NCEE(logic_error, "C Thread error");
	}

 #else /* _HAS_EXCEPTIONS */
void _Throw_Cpp_error(int code)
	{	// report system error
	_CSTD fputs("C++ thread error\n", stderr);
	_CSTD abort();
	}

void _Throw_C_error(int code)
	{	// throw error object for C error
	_CSTD fputs("C thread error\n", stderr);
	_CSTD abort();
	}
 #endif /* _HAS_EXCEPTIONS */
 #endif /* _HAS_CPP0X */

typedef unsigned int _Call_func_ret;
#define _CALL_FUNC_MODIFIER	_STDCALL

_EXTERN_C
static _Call_func_ret _CALL_FUNC_MODIFIER _Call_func(void *_Data)
	{	// entry point for new thread
    _Call_func_ret _Res = 0;
 #if _HAS_EXCEPTIONS
	try {	// don't let exceptions escape
		_Res = (_Call_func_ret)static_cast<_Pad *>(_Data)->_Go();
		}
	catch (...)
		{	// uncaught exception in thread
		int zero = 0;
		if (zero == 0)
 #if 1300 <= _MSC_VER
			terminate();	// to quiet diagnostics
 #else /* 1300 <= _MSC_VER */
			_XSTD terminate();	// to quiet diagnostics
 #endif /* 1300 <= _MSC_VER */
		}

 #else /* _HAS_EXCEPTIONS */
	_Res = (_Call_func_ret)static_cast<_Pad *>(_Data)->_Go();
 #endif /* _HAS_EXCEPTIONS */

	_Cnd_do_broadcast_at_thread_exit();
	return (_Res);
	}
_END_EXTERN_C


_STD_END

/*
 * Copyright (c) 1992-2011 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V5.40:0009 */
