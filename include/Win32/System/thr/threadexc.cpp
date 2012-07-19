// threadexc.cpp -- threads exception support functions
#include <new>

#pragma warning(disable:4100 4245 4275)

#include <string>
#include <utility>
#include "exceptions.hpp"

namespace std
{

	lock_error::lock_error()
		: _STD runtime_error("lock error")
	{}

	thread_resource_error::thread_resource_error()
		: _STD runtime_error("thread resource error")
	{}

	void _Throw_lock_error()
	{	// throw lock_error exception
		_RAISE(lock_error());
	}

	void _Throw_resource_error()
	{	// throw resource_error exception
		_RAISE(thread_resource_error());
	}

	void _Throw_bad_alloc()
	{	// throw std__bad_alloc exception
		_RAISE(_XSTD bad_alloc());
	}

}	// namespace 

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
