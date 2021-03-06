/* cond.c -- condition variable functions */
#include "xthreads.h"
#include "xtimec.h"

#include <stdlib.h>
#include <limits.h>
#include "wrapwin.h"

struct _Cnd_internal_imp_t
	{	/* condition variable implementation for Win32 */
	HANDLE gate;
	HANDLE queue;
	HANDLE internal;
	int gone;
	int blocked;
	int to_unblock;
	};

int _Cnd_init(_Cnd_t *pcond)
	{	/* initialize */
	_Cnd_t cond;
	*pcond = 0;

	if ((cond = (_Cnd_t)calloc(sizeof(struct _Cnd_internal_imp_t), 1)) == 0)
		return (_Thrd_nomem);	/* report alloc failed */
	else if ((cond->gate = CreateSemaphore(0, 1, 1, 0)) == 0)
		{	/* report gate failed */
		free(cond);
		return (_Thrd_error);
		}
	else if ((cond->queue = CreateSemaphore(0, 0, INT_MAX, 0)) == 0)
		{	/* report queue failed */
		CloseHandle(cond->gate);
		free(cond);
		return (_Thrd_error);
		}
	else if ((cond->internal = CreateMutex(0, FALSE, 0)) == 0)
		{	/* report internal failed */
		CloseHandle(cond->queue);
		CloseHandle(cond->gate);
		free(cond);
		return (_Thrd_error);
		}
	else
		{	/* report success */
		*pcond = cond;
		return (_Thrd_success);
		}
	}

void _Cnd_destroy(_Cnd_t *cond)
	{	/* clean up */
	if (cond && *cond)
		{	/* release resources */
		_THREAD_ASSERT((*cond)->blocked - (*cond)->gone == 0,
			"condition destroyed while busy");
		CloseHandle((*cond)->internal);
		CloseHandle((*cond)->queue);
		CloseHandle((*cond)->gate);
		free(*cond);
		}
	}

static int block_thread(_Cnd_t *cond, _Mtx_t *mtx, const xtime *target)
	{	/* block thread on mutex or duration */
	int res = WAIT_TIMEOUT;
	if (!target)
		res = WaitForSingleObject((*cond)->queue, INFINITE);
	else
		{	/* check time */
		long millis;
		xtime now;
		xtime_get(&now, TIME_UTC);
		millis = _Xtime_diff_to_millis2(target, &now);
		while (0 < millis)
			{	/* time out */
			res = WaitForSingleObject((*cond)->queue, millis);
			if (res != WAIT_TIMEOUT)
				break;
			xtime_get(&now, TIME_UTC);
			millis = _Xtime_diff_to_millis2(target, &now);
			}
		}
	return (res);
	}

static int do_wait(_Cnd_t *cond, _Mtx_t *mtx, const xtime *target)
	{	/* wait for signal or timeout */
	_Mtx_state state;
	int res = _Thrd_timedout;
	int val = WAIT_OBJECT_0;
	int ghosts = 0;
	int open_gate = 0;

 	/* close gate, increment count of blocked threads, open gate */
	if (WaitForSingleObject((*cond)->gate, INFINITE) == WAIT_FAILED)
		return (_Thrd_error);
	++(*cond)->blocked;
	if (ReleaseSemaphore((*cond)->gate, 1, 0) == 0)
		return (_Thrd_error);

 	/* unlock mutex */
	if (_Save_state(mtx, &state) != _Thrd_success)
		return (_Thrd_error);

 	/* wait */
	val = block_thread(cond, mtx, target);
	if (val != WAIT_OBJECT_0 && val != WAIT_TIMEOUT)
		return (_Thrd_error);

 	/* lock internal mutex */
	if (WaitForSingleObject((*cond)->internal, INFINITE) == WAIT_FAILED)
		return (_Thrd_error);

	if ((*cond)->to_unblock != 0)
		{	/* update state for thread that unblocked while signal pending */
		res = _Thrd_success;
		if (val == WAIT_OBJECT_0)
			;	/* unblocked by signal; no extra bookkeeping */
		/* otherwise, update for timeout or spurious wakeup: */
		else if ((*cond)->blocked != 0)
			--(*cond)->blocked;
		else
			++(*cond)->gone;

		--(*cond)->to_unblock;
		if ((*cond)->to_unblock == 0 && (*cond)->blocked == 0)
			{	/* ensure no ghost threads in queue */
			ghosts = (*cond)->gone;
			(*cond)->gone = 0;
			}

		/* if all signals have been processed, open the upper gate */
		open_gate = (*cond)->to_unblock == 0;
		}
	else if (++(*cond)->gone == INT_MAX / 2)
		{	/* update condvar state for thread unblocked with no signal */
		if (WaitForSingleObject((*cond)->gate, INFINITE) == WAIT_FAILED)
			return (_Thrd_error);
		(*cond)->blocked -= (*cond)->gone;
		if (ReleaseSemaphore((*cond)->gate, 1, 0) == 0)
			return (_Thrd_error);
		(*cond)->gone = 0;
		}
	if (ReleaseMutex((*cond)->internal) == 0 || res == _Thrd_error)
		return (_Thrd_error);

	while (ghosts-- != 0)
		WaitForSingleObject((*cond)->queue, INFINITE);
	if (open_gate && ReleaseSemaphore((*cond)->gate, 1, 0) == 0)
		return (_Thrd_error);
	if (_Restore_state(mtx, &state) != _Thrd_success)
		return (_Thrd_error);
	return (res);
	}

static int do_signal(_Cnd_t *cond, int all)
	{	/* release threads */
	int res = _Thrd_success;
	int to_unblock = 0;

	if (WaitForSingleObject((*cond)->internal, INFINITE) == WAIT_FAILED)
		res = _Thrd_error;
	else if ((*cond)->to_unblock != 0)
		{	/* have previous unprocessed signals; upper gate already locked */
		to_unblock =
			(*cond)->blocked == 0 ? 0
				: all ? (*cond)->blocked : 1;
		(*cond)->to_unblock += to_unblock;
		(*cond)->blocked -= to_unblock;
		}
	else if ((*cond)->gone < (*cond)->blocked)
		{	/* have waiting threads; lock upper gate */
		if (WaitForSingleObject((*cond)->gate, INFINITE) == WAIT_FAILED)
			res = _Thrd_error;
		else
			{	/* update counts, release blocked threads */
			(*cond)->blocked -= (*cond)->gone;
			(*cond)->gone = 0;
			to_unblock = all ? (*cond)->blocked : 1;
			(*cond)->to_unblock = to_unblock;
			(*cond)->blocked -= to_unblock;
			}
		}

	if (res != _Thrd_success)
		;
	else if (to_unblock != 0
		&& ReleaseSemaphore((*cond)->queue, to_unblock, 0) == 0)
		res = _Thrd_error;
	else if (ReleaseMutex((*cond)->internal) == 0)
		res = _Thrd_error;
	return (res);
	}

int _Cnd_wait(_Cnd_t *cond, _Mtx_t *mtx)
	{	/* wait until signaled */
	return (do_wait(cond, mtx, 0));
	}

int _Cnd_timedwait(_Cnd_t *cond, _Mtx_t *mtx, const xtime *xt)
	{	/* wait until signaled or timeout */
	return (do_wait(cond, mtx, xt));
	}

int _Cnd_signal(_Cnd_t *cond)
	{	/* release one waiting thread */
	return (do_signal(cond, 0));
	}

int _Cnd_broadcast(_Cnd_t *cond)
	{	/* release all waiting threads */
	return (do_signal(cond, 1));
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
