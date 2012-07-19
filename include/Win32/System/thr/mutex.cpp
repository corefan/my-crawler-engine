/* mutex.c -- mutex functions */
#include "threads.h"
#include "xtimec.h"
#include <stdlib.h>

#include "wrapwin.h"

#if !(defined(_M_CEE))
 #include <concrt.h>
 #include <ppl.h>
#endif

struct _Mtx_internal_imp_t
	{	/* Win32 mutex */
	int type;
	LPCRITICAL_SECTION hnd;
#if !(defined(_M_CEE))
	Concurrency::critical_section cs;
	long thread_id;
#endif
	int count;
	thrd_t owner;
	};

int _Mtx_init(_Mtx_t *mtx, int type)
	{	/* initialize mutex */
	_Mtx_t mutex;
	*mtx = 0;

	if ((mutex = (mtx_t)calloc(1, sizeof (struct _Mtx_internal_imp_t))) == 0)
		return (_Thrd_nomem);	/* report alloc failed */
	else if ((type & ~_Mtx_recursive) == _Mtx_plain)
		{	/* allocate for plain mutex */
#if !(defined(_M_CEE))
		new(&mutex->cs) Concurrency::critical_section();
		mutex->thread_id = -1;
#else
		if ((mutex->hnd =
			(LPCRITICAL_SECTION)malloc(sizeof(CRITICAL_SECTION))) == 0)
			{	/* report alloc failed */
			free(mutex);
			return (_Thrd_nomem);
			}
		InitializeCriticalSection(mutex->hnd);
#endif
		mutex->type = type;
		*mtx = mutex;
		return (_Thrd_success);
		}
#if !(defined(_M_CEE))
	else if ((type & _Mtx_timed) != _Mtx_timed)
		{
		new(&mutex->cs) Concurrency::critical_section();
		mutex->thread_id = -1;
		mutex->type = type;
		*mtx = mutex;
		return (_Thrd_success);
		}
#endif
	else if ((mutex->hnd = (LPCRITICAL_SECTION)CreateMutex(0, 0, 0)) == 0)
		{	/* report create failed */
		free(mutex);
		return (_Thrd_error);
		}
	else
		{	/* report success */
		mutex->type = type;
		*mtx = mutex;
		return (_Thrd_success);
		}
	}

void _Mtx_destroy(_Mtx_t *mtx)
	{	/* destroy mutex */
	if (mtx && *mtx)
		{	/* something to do, do it */
		_THREAD_ASSERT((*mtx)->count == 0, "mutex destroyed while busy");
#if !(defined(_M_CEE))
		if (((*mtx)->type & _Mtx_timed) != _Mtx_timed)
			(*mtx)->cs.~critical_section();
		else
#endif
		if (((*mtx)->type & ~_Mtx_recursive) == _Mtx_plain)
			{	/* free storage */
			DeleteCriticalSection((*mtx)->hnd);
			free((*mtx)->hnd);
			}
		else
			CloseHandle((*mtx)->hnd);
		free(*mtx);
		}
	}

static int mtx_do_lock(_Mtx_t *mtx, const xtime *target)
	{	/* lock mutex */
 #if 0	/* unprotected */
	_THREAD_ASSERT((*mtx)->count == 0
		|| ((*mtx)->type & _Mtx_recursive) == _Mtx_recursive
		|| !thrd_equal((*mtx)->owner, thrd_current()),
		"recursive lock of non-recursive mutex");
 #endif /* 0 */

	if (((*mtx)->type & ~_Mtx_recursive) == _Mtx_plain)
		{	/* set the lock */
#if !(defined(_M_CEE))
		if ((*mtx)->thread_id != GetCurrentThreadId())
			{
			(*mtx)->cs.lock();
			(*mtx)->thread_id = GetCurrentThreadId();
			}
		++(*mtx)->count;
#else
		EnterCriticalSection((*mtx)->hnd);
		(*mtx)->owner = thrd_current();
		if (1 < ++(*mtx)->count)
			LeaveCriticalSection((*mtx)->hnd);
#endif
		return (_Thrd_success);
		}
	else
		{	/* handle timed or recursive mutex */
		int res = WAIT_TIMEOUT;
		if (target == 0)
			{
			/* no target --> plain wait (i.e. infinite timeout) */
#if !(defined(_M_CEE))
			if (((*mtx)->type & _Mtx_timed) != _Mtx_timed)
				{
				if ((*mtx)->thread_id != GetCurrentThreadId())
					{
					(*mtx)->cs.lock();
					}
				res = WAIT_OBJECT_0;
				}
			else
#endif
			res = WaitForSingleObject((*mtx)->hnd, INFINITE);
			}
		else if (target->sec < 0 || target->sec == 0 && target->nsec <= 0)
			{
#if !(defined(_M_CEE))
			if (((*mtx)->type & _Mtx_timed) != _Mtx_timed)
				{
				if ((*mtx)->thread_id != GetCurrentThreadId())
					{
					if ((*mtx)->cs.try_lock())
						res = WAIT_OBJECT_0;
					else
						res = WAIT_TIMEOUT;
					}
				else
					res = WAIT_OBJECT_0;
				}
			else
#endif
			/* target time <= 0 --> plain trylock or timed wait for */
			/* time that has passed; try to lock with 0 timeout */
			res = WaitForSingleObject((*mtx)->hnd, 0);
			}
		else
			{	/* check timeout */
			xtime now;
			xtime_get(&now, TIME_UTC);
			while (now.sec < target->sec
				|| now.sec == target->sec && now.nsec < target->nsec)
				{	/* time has not expired */
				res = WaitForSingleObject((*mtx)->hnd,
					_Xtime_diff_to_millis2(target, &now));
				if (res != WAIT_TIMEOUT)
					break;
				xtime_get(&now, TIME_UTC);
				}
			}
		if (res != WAIT_OBJECT_0 && res != WAIT_ABANDONED)
			;
		else if (1 < ++(*mtx)->count)
			{	/* check count */
			if (((*mtx)->type & _Mtx_recursive) != _Mtx_recursive)
				{	/* not recursive, fixup count */
				--(*mtx)->count;
				res = WAIT_TIMEOUT;
				}
			ReleaseMutex((*mtx)->hnd);
			}
		else
			{
#if !(defined(_M_CEE))
			(*mtx)->thread_id = GetCurrentThreadId();
#else
			(*mtx)->owner = thrd_current();
#endif
			}
		return (res == WAIT_OBJECT_0 || res == WAIT_ABANDONED ? _Thrd_success
			: res != WAIT_TIMEOUT ? _Thrd_error
			: target != 0 && target->sec == 0
				&& target->nsec == 0 ? _Thrd_busy
			: _Thrd_timedout);
		}
	}

int _Mtx_unlock(_Mtx_t *mtx)
	{	/* unlock mutex */
#if !(defined(_M_CEE))
	_THREAD_ASSERT(1 <= (*mtx)->count
		&& (*mtx)->thread_id == GetCurrentThreadId(),
		"unlock of unowned mutex");
#else
	_THREAD_ASSERT(1 <= (*mtx)->count
		&& _Thrd_equal((*mtx)->owner, thrd_current()),
		"unlock of unowned mutex");
#endif
	if (--(*mtx)->count != 0)
		;
	else if (((*mtx)->type & ~_Mtx_recursive) == _Mtx_plain)
		{
#if !(defined(_M_CEE))
		(*mtx)->cs.unlock();
		(*mtx)->thread_id = -1;
#else
		LeaveCriticalSection((*mtx)->hnd);
#endif
		}
	else
		{
#if !(defined(_M_CEE))
		if (((*mtx)->type & _Mtx_timed) != _Mtx_timed)
			{
			(*mtx)->cs.unlock();
			(*mtx)->thread_id = -1;
			}
		else
#endif
		ReleaseMutex((*mtx)->hnd);
		}
	return (_Thrd_success);
	}

int _Mtx_lock(_Mtx_t *mtx)
	{	/* lock mutex */
	return (mtx_do_lock(mtx, 0));
	}

int _Mtx_trylock(_Mtx_t *mtx)
	{	/* attempt to lock try_mutex */
	xtime xt;
	_THREAD_ASSERT(((*mtx)->type & (_Mtx_try + _Mtx_timed)) != 0,
		"trylock not supported by mutex");
	xt.sec = xt.nsec = 0;
	return (mtx_do_lock(mtx, &xt));
	}

int _Mtx_timedlock(_Mtx_t *mtx, const xtime *xt)
	{	/* attempt to lock timed mutex */
	int res;

	_THREAD_ASSERT(((*mtx)->type & _Mtx_timed) != 0,
		"timedlock not supported by mutex");
	res = mtx_do_lock(mtx, xt);
	return (res == _Thrd_busy ? _Thrd_timedout : res);
	}

int _Save_state(struct _Mtx_internal_imp_t **mtx, _Mtx_state *state)
	{	/* save state */
	state->_Count = (*mtx)->count;
	(*mtx)->count = 1;
	return (mtx_unlock(mtx));
	}

int _Restore_state(struct _Mtx_internal_imp_t **mtx, _Mtx_state *state)
	{	/* restore state */
	int res = mtx_do_lock(mtx, 0);
	(*mtx)->count = state->_Count;
	return (res);
	}

int _Mtx_current_owns(_Mtx_t *mtx)
	{	/* test if current thread owns mutex */
#if !(defined(_M_CEE))
	return ((*mtx)->count != 0
		&& (*mtx)->thread_id == GetCurrentThreadId());
#else
	return ((*mtx)->count != 0
		&& thrd_equal((*mtx)->owner, thrd_current()));
#endif
	}

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
