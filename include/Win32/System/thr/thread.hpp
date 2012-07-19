#pragma once
#ifndef _THR_THREAD
#define _THR_THREAD
#ifndef RC_INVOKED
#include <vector>
#include <functional>
#include "threads.h"
#include "xthread.hpp"
#include "mutex.hpp"
#include "xtime.hpp"

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,3)

namespace std
{	// Dinkum C++ Threads Library
	class thread
	{	// class for observing and managing threads
	public:
		thread()
			: _Joinable(false)
		{	// construct
			_Thr = thrd_current();
		}

		template<class _Func>
		thread(_Func _Fp)
			: _Joinable(true)
		{	// construct with _Fp()
			_STD _Launch(&_Thr, _STD bind(_Fp));
		}

		~thread()
		{	// destroy
			if (_Joinable)
				thrd_detach(_Thr);
		}

		bool operator==(const thread& _Other) const
		{	// test for equality
			return (thrd_equal(_Thr, _Other._Thr) != 0);
		}

		bool operator!=(const thread& _Other) const
		{	// test for inequality
			return (!(*this == _Other));
		}

		void join()
		{	// join thread
			_THREAD_ASSERT(_Joinable,
				"threads::thread::join called on non-joinable thread");
			_Joinable = false;
			thrd_join(_Thr, 0);
		}

		static void sleep(const xtime& _Xt)
		{	// sleep for a time
			thrd_sleep(&_Xt);
		}

		static void yield()
		{	// yield control
			thrd_yield();
		}

	private:
		thrd_t _Thr;
		bool _Joinable;
		friend class thread_group;

		thread(const thread&);	// not defined
		thread& operator=(const thread&);	// not defined
	};

	class thread_group
	{	// class for observing multiple threads
	public:
		thread_group()
		{	// construct
		}

		_CRTIMP2_PURE ~thread_group() _NOEXCEPT;

		template<class _Func>
		thread *create_thread(_Func _Fp)
		{	// create a new thread
			thread *_Thr = 0;

#if _HAS_EXCEPTIONS
			try {	// don't let exceptions escape
				_Thr = new thread(_Fp);
				add_thread(_Thr);
			}
			catch (...)
			{	// delete incomplete thread
				delete _Thr;
				throw;
			}

#else /* _HAS_EXCEPTIONS */
			_Thr = new thread(_Fp);
			add_thread(_Thr);
#endif /* _HAS_EXCEPTIONS */

			return (_Thr);
		}

		void _CRTIMP2_PURE add_thread(thread *_Thrd);
		void _CRTIMP2_PURE remove_thread(thread *_Thrd);
		void _CRTIMP2_PURE join_all();

	private:
		mutex _Mtx;
		_STD vector<thread *> _Elts;

		thread_group(const thread_group&);	// not defined
		thread_group& operator=(const thread_group&);	// not defined
	};
}	// namespace 
#pragma warning(pop)
#pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _THR_THREAD */

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
