// condition header for threads library
#pragma once
#ifndef _THR_CONDITION
#define _THR_CONDITION
#ifndef RC_INVOKED
#include "xthrcommon.h"
#include "xthreads.h"
#include "xtime.hpp"
#include "exceptions.hpp"

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,3)

namespace std
{	
	class condition
	{	// condition variables
	public:
		condition()
		{	// construct
			_Validate(_Cnd_init(&_Cond));
		}

		~condition() _NOEXCEPT
		{	// destroy
			_Cnd_destroy(&_Cond);
		}

		void notify_one()
		{	// release up to one blocked thread
			_Validate(_Cnd_signal(&_Cond));
		}

		void notify_all()
		{	// release all blocked threads
			_Validate(_Cnd_broadcast(&_Cond));
		}

		template<class _Scopedlock>
		void wait(_Scopedlock& _Lock)
		{	// block until signalled
			if (!_Lock)
				_Throw_lock_error();
			_Validate(_Cnd_wait(&_Cond, &_Lock._My_lock._Mtx._Mtx));
		}

		template<class _Scopedlock,
		class _Predicate>
			void wait(_Scopedlock& _Lock, _Predicate _Pred)
		{	// block until signalled and _Pred is true
			if (!_Lock)
				_Throw_lock_error();
			while (!_Pred())
				_Validate(_Cnd_wait(&_Cond, &_Lock._My_lock._Mtx._Mtx));
		}

		template<class _Scopedlock>
		bool timed_wait(_Scopedlock& _Lock, const xtime& _Xt)
		{	// block until signalled, or until time _Xt
			if (!_Lock)
				_Throw_lock_error();
			return (_Validate(_Cnd_timedwait(&_Cond,
				&_Lock._My_lock._Mtx._Mtx, &_Xt)) == thrd_success);
		}

		template<class _Scopedlock,
		class _Predicate>
			bool timed_wait(_Scopedlock& _Lock, const xtime& _Xt,
			_Predicate _Pred)
		{	// block until signalled and _Pred is true, or until time _Xt
			if (!_Lock)
				_Throw_lock_error();
			int _Res = thrd_success;
			while (!_Pred() && _Res == thrd_success)
				_Res = _Validate(_Cnd_timedwait(&_Cond,
				&_Lock._My_lock._Mtx._Mtx, &_Xt));
			return (_Res == thrd_success);
		}

		template<class _Scopedlock>
		void _Register(_Scopedlock& _Lck)
		{	// register *this for notify_all at thread exit
			_Cnd_register_at_thread_exit(&_Cond, &_Lck._My_lock._Mtx._Mtx, 0);
		}

	private:
		_Cnd_t _Cond;

		condition(const condition&);	// not implemented
		condition& operator=(const condition&);	// not implemented
	};

	template <class _Scopedlock>
	void notify_all_at_thread_exit(condition& _Cond, _Scopedlock& _Lock)
	{	// register _Cond for notify_all at thread exit
		if (!_Lock)
			_Throw_lock_error();
		_Cond._Register(_Lock);
	}
}	// namespace 
#pragma warning(pop)
#pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _THR_CONDITION */

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
