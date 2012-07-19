// once-function header for threads library
#pragma once
#ifndef _THR_ONCE
#define _THR_ONCE
#ifndef RC_INVOKED
#include "xthreads.h"

 #pragma pack(push,_CRT_PACKING)
 #pragma warning(push,3)

namespace std
{
	// Dinkum C++ Threads Library
typedef ::_Once_flag once_flag;
const _Once_flag once_init = _ONCE_FLAG_INIT;

template < typename HandlerT >
inline void call_once(const HandlerT &_Func, _Once_flag& _Flag)
	{	// execute func exactly once
	::_Call_once(&_Flag, _Func);
	}
}	// 
 #pragma warning(pop)
 #pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _THR_ONCE */

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
