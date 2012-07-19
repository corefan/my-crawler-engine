// recursive_mutex header for threads library
#pragma once
#ifndef _THR_RECURSIVE_MUTEX
#define _THR_RECURSIVE_MUTEX
#ifndef RC_INVOKED
#include "xmutex.hpp"

 #pragma pack(push,_CRT_PACKING)
 #pragma warning(push,3)

namespace std
		{	// Dinkum C++ Threads Library
class recursive_mutex
	: private _Mutex_base
	{	// class for recursive mutex objects
public:
	typedef recursive_mutex _Myt;
	typedef _Scoped_lock_imp<_Myt> scoped_lock;

	recursive_mutex()
		: _Mutex_base(_Mtx_plain | _Mtx_recursive)
		{	// construct
		}

	using _Mutex_base::lock;
	using _Mutex_base::unlock;
	using _Mutex_base::_Mtx;

private:
	recursive_mutex(const _Myt&);	// not defined
	_Myt& operator=(const _Myt&);	// not defined
	};

class recursive_try_mutex
	: private _Mutex_base
	{	// class for recursive try-mutex objects
public:
	typedef recursive_try_mutex _Myt;
	typedef _Scoped_lock_imp<_Myt> scoped_lock;
	typedef _Scoped_try_lock_imp<_Myt> scoped_try_lock;

	recursive_try_mutex()
		: _Mutex_base(_Mtx_try | _Mtx_recursive)
		{	// construct
		}

	using _Mutex_base::lock;
	using _Mutex_base::try_lock;
	using _Mutex_base::unlock;
	using _Mutex_base::_Mtx;

private:
	recursive_try_mutex(const _Myt&);	// not defined
	_Myt& operator=(const _Myt&);	// not defined
	};

class recursive_timed_mutex
	: private _Mutex_base
	{	// class for recursive timed-mutex objects
public:
	typedef recursive_timed_mutex _Myt;
	typedef _Scoped_lock_imp<_Myt> scoped_lock;
	typedef _Scoped_try_lock_imp<_Myt> scoped_try_lock;
	typedef _Scoped_timed_lock_imp<_Myt> scoped_timed_lock;

	recursive_timed_mutex()
		: _Mutex_base(_Mtx_timed | _Mtx_recursive)
		{	// construct
		}

	using _Mutex_base::lock;
	using _Mutex_base::try_lock;
	using _Mutex_base::timed_lock;
	using _Mutex_base::unlock;
	using _Mutex_base::_Mtx;

private:
	recursive_timed_mutex(const _Myt&);	// not defined
	_Myt& operator=(const _Myt&);	// not defined
	};
		}	// namespace threads
	}	// namespace stdext
 #pragma warning(pop)
 #pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _THR_RECURSIVE_MUTEX */

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
