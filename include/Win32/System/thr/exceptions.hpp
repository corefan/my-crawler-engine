// exceptions header for threads library
#pragma once
#ifndef _THR_EXCEPTIONS
#define _THR_EXCEPTIONS
#ifndef RC_INVOKED
#include <stdexcept>
#include "threads.h"

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,3)

namespace std
{	
	class lock_error
		: public _STD runtime_error
	{	// lock error exception
	public:
		lock_error();
	};

	class thread_resource_error
		: public _STD runtime_error
	{	// resource error exception
	public:
		thread_resource_error();
	};

	void  _Throw_lock_error();
	void  _Throw_resource_error();
	void  _Throw_bad_alloc();

	inline int _Validate(int _Res)
	{	// convert error code to exception
		if (_Res == thrd_nomem)
			_Throw_bad_alloc();
		else if (_Res == thrd_error)
			_Throw_resource_error();
		return (_Res);
	}
}	// namespace 
#pragma warning(pop)
#pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _THR_EXCEPTIONS */

/*
* This file is derived from software bearing the following
* restrictions:
*
* (c) Copyright William E. Kempf 2001
*
* Permission to use, copy, modify, distribute and sell this
* software and its documentation for any purpose is hereby
* granted without fee, provided that the above copyright
* notice appear in all copies and that both that copyright
* notice and this permission notice appear in supporting
* documentation. William E. Kempf makes no representations
* about the suitability of this software for any purpose.
* It is provided "as is" without express or implied warranty.
*/

/*
* Copyright (c) 1992-2011 by P.J. Plauger.  ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
V5.40:0009 */
