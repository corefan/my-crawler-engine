/* atomic.c -- implement generic atomics and UDTs of arbitrary sizes */
#include <string.h>
#include <stdlib.h>
#include "xatomic.h"

 #if _HAS_CPP0X
_STD_BEGIN
		/* SPIN LOCK FOR LOCKING VERSIONS OF OPERATIONS */
 void __CLRCALL_PURE_OR_CDECL _Lock_spin_lock(
	volatile _Atomic_flag_t *_Flag, memory_order _Order)
	{	/* spin until _Flag successfully set */
	while (_ATOMIC_FLAG_TEST_AND_SET(_Flag, _Order))
		;
	}

 void __CLRCALL_PURE_OR_CDECL _Unlock_spin_lock(
	volatile _Atomic_flag_t *_Flag, memory_order _Order)
	{	/* release previously obtained lock */
	_ATOMIC_FLAG_CLEAR(_Flag, _Order);
	}

		/* SPIN LOCK FOR shared_ptr ATOMIC OPERATIONS */
volatile _Atomic_flag_t _Shared_ptr_flag;

 void __CLRCALL_PURE_OR_CDECL _Lock_shared_ptr_spin_lock(memory_order _Order)
	{	/* spin until _Flag successfully set */
	while (_ATOMIC_FLAG_TEST_AND_SET(&_Shared_ptr_flag, _Order))
		;
	}

 void __CLRCALL_PURE_OR_CDECL _Unlock_shared_ptr_spin_lock(memory_order _Order)
	{	/* release previously obtained lock */
	_ATOMIC_FLAG_CLEAR(&_Shared_ptr_flag, _Order);
	}

 #if _ATOMIC_FLAG_USES_LOCK
static mtx_t mtx;
static once_flag mtx_initialized = ONCE_FLAG_INIT;

static void init_mtx(void)
	{	/* initialize mutex */
	mtx_init(&mtx, mtx_plain);
	}

 int __CLRCALL_PURE_OR_CDECL _Atomic_flag_test_and_set_locking(
	volatile _Atomic_flag_t *_Flag,
		memory_order _Order)
	{	/* atomically test flag and set to true */
	int res;

	call_once(&mtx_initialized, init_mtx);
	mtx_lock(&mtx);
	res = *_Flag;
	*_Flag = 1;
	mtx_unlock(&mtx);
	return (res);
	}

 void __CLRCALL_PURE_OR_CDECL _Atomic_flag_clear_locking(
	volatile _Atomic_flag_t *_Flag,
		memory_order _Order)
	{	/* atomically clear flag */
	call_once(&mtx_initialized, init_mtx);
	mtx_lock(&mtx);
	*_Flag = 0;
	mtx_unlock(&mtx);
	}
 #endif /* _ATOMIC_FLAG_USES_LOCK */

		/* ATOMIC OPERATIONS FOR OBJECTS WITH SIZES THAT
			DON'T MATCH THE SIZE OF ANY INTEGRAL TYPE */
 void __CLRCALL_PURE_OR_CDECL _Atomic_copy(
	volatile _Atomic_flag_t *_Flag, size_t _Size,
		volatile void *_Tgt, volatile const void *_Src,
			memory_order _Order)
	{	/* atomically copy *_Src to *_Tgt with memory ordering */
	_Lock_spin_lock(_Flag, _Order);
	memcpy((void *)_Tgt, (void *)_Src, _Size);
	_Unlock_spin_lock(_Flag, _Order);
	}

 void __CLRCALL_PURE_OR_CDECL _Atomic_exchange(
	volatile _Atomic_flag_t *_Flag, size_t _Size,
		volatile void *_Tgt, volatile void *_Src,
			memory_order _Order)
	{	/* atomically swap *_Src and *_Tgt with memory ordering */
	volatile unsigned char *left = (volatile unsigned char *)_Tgt;
	volatile unsigned char *right = (volatile unsigned char *)_Src;

	_Lock_spin_lock(_Flag, _Order);
	for (; 0 < _Size; --_Size)
		{	/* copy bytes */
		unsigned char tmp = *left;
		*left++ = *right;
		*right++ = tmp;
		}
	_Unlock_spin_lock(_Flag, _Order);
	}

 int __CLRCALL_PURE_OR_CDECL _Atomic_compare_exchange_weak(
	volatile _Atomic_flag_t *_Flag, size_t _Size,
		volatile void *_Tgt, volatile void *_Exp, const volatile void *_Src,
			memory_order _Order1, memory_order _Order2)
	{	/* atomically compare and exchange with memory ordering */
	int _Result;

	_Lock_spin_lock(_Flag, _Order1);
	_Result = memcmp((const void *)_Tgt, (const void *)_Exp, _Size) == 0;
	if (_Result != 0)
		memcpy((void *)_Tgt, (void *)_Src, _Size);
	else
		memcpy((void *)_Exp, (void *)_Tgt, _Size);
	_Unlock_spin_lock(_Flag, _Result ? _Order1 : _Order2);
	return (_Result);
	}

 int __CLRCALL_PURE_OR_CDECL _Atomic_compare_exchange_strong(
	volatile _Atomic_flag_t *_Flag, size_t _Size,
	volatile void *_Tgt, volatile void *_Exp, const volatile void *_Src,
	memory_order _Order1, memory_order _Order2)
	{	/* atomically compare and exchange with memory ordering */
	return (_Atomic_compare_exchange_weak(_Flag, _Size, _Tgt, _Exp, _Src,
	  _Order1, _Order2));
	}

		/* LOCK-FREE PROPERTY FOR INTEGRAL TYPES */
 int __CLRCALL_PURE_OR_CDECL _Atomic_is_lock_free_1(void)
	{	/* return true if 1-byte atomic values are lock-free */
	return (1 <= _ATOMIC_MAXBYTES_LOCK_FREE);
	}

 int __CLRCALL_PURE_OR_CDECL _Atomic_is_lock_free_2(void)
	{	/* return true if 2-byte atomic values are lock-free */
	return (2 <= _ATOMIC_MAXBYTES_LOCK_FREE);
	}

 int __CLRCALL_PURE_OR_CDECL _Atomic_is_lock_free_4(void)
	{	/* return true if 4-byte atomic values are lock-free */
	return (4 <= _ATOMIC_MAXBYTES_LOCK_FREE);
	}

 int __CLRCALL_PURE_OR_CDECL _Atomic_is_lock_free_8(void)
	{	/* return true if 8-byte atomic values are lock-free */
	return (8 <= _ATOMIC_MAXBYTES_LOCK_FREE);
	}

 #if _ATOMIC_MAXBYTES_LOCK_FREE < 1
 void __CLRCALL_PURE_OR_CDECL _Atomic_store_1_locking(
	volatile _Atomic_flag_t *_Flag, _Uint1_t *_Tgt, _Uint1_t _Value,
		memory_order _Order)
	{	/* store _Value while locked */
	_Lock_spin_lock(_Flag, _Order);
	*_Tgt = _Value;
	_Unlock_spin_lock(_Flag, _Order);
	}

 _Uint1_t __CLRCALL_PURE_OR_CDECL _Atomic_load_1_locking(
	volatile _Atomic_flag_t *_Flag, _Uint1_t *_Src,
		memory_order _Order)
	{	/* load value while locked */
	_Uint1_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Src;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint1_t __CLRCALL_PURE_OR_CDECL _Atomic_exchange_1_locking(
	volatile _Atomic_flag_t *_Flag, _Uint1_t *_Src, _Uint1_t _Value,
		memory_order _Order)
	{	/* swap values while locked */
	_Uint1_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Src;
	*_Src = _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 int __CLRCALL_PURE_OR_CDECL _Atomic_compare_exchange_weak_1_locking(
	volatile _Atomic_flag_t *_Flag,
		_Uint1_t *_Tgt, _Uint1_t *_Exp, _Uint1_t _Value,
			memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values while locked */
	int res;

	_Lock_spin_lock(_Flag, _Order1);
	res = *_Tgt == *_Exp;
	if (res)
		*_Tgt = _Value;
	else
		*_Exp = *_Tgt;
	_Unlock_spin_lock(_Flag, _Order2);
	return (res);
	}

 int __CLRCALL_PURE_OR_CDECL _Atomic_compare_exchange_strong_1_locking(
	volatile _Atomic_flag_t *_Flag,
		_Uint1_t *_Tgt, _Uint1_t *_Exp, _Uint1_t _Value,
			memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values while locked */
	int res;

	_Lock_spin_lock(_Flag, _Order1);
	res = *_Tgt == *_Exp;
	if (res)
		*_Tgt = _Value;
	else
		*_Exp = *_Tgt;
	_Unlock_spin_lock(_Flag, _Order2);
	return (res);
	}

 _Uint1_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_add_1_locking(
	volatile _Atomic_flag_t *_Flag,
		_Uint1_t *_Tgt, _Uint1_t _Value,
			memory_order _Order)
	{	/* add to value while locked */
	_Uint1_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt += _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint1_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_sub_1_locking(
	volatile _Atomic_flag_t *_Flag,
		_Uint1_t *_Tgt, _Uint1_t _Value,
			memory_order _Order)
	{	/* subtract from value while locked */
	_Uint1_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt -= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint1_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_and_1_locking(
	volatile _Atomic_flag_t *_Flag,
		_Uint1_t *_Tgt, _Uint1_t _Value,
			memory_order _Order)
	{	/* and with value while locked */
	_Uint1_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt &= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint1_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_or_1_locking(
	volatile _Atomic_flag_t *_Flag,
		_Uint1_t *_Tgt, _Uint1_t _Value,
			memory_order _Order)
	{	/* or with value while locked */
	_Uint1_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt |= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint1_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_xor_1_locking(
	volatile _Atomic_flag_t *_Flag,
		_Uint1_t *_Tgt, _Uint1_t _Value,
			memory_order _Order)
	{	/* xor with value while locked */
	_Uint1_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt ^= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}
 #endif /* _ATOMIC_MAXBYTES_LOCK_FREE < 1 */

 #if _ATOMIC_MAXBYTES_LOCK_FREE < 2
 void __CLRCALL_PURE_OR_CDECL _Atomic_store_2_locking(volatile _Atomic_flag_t *_Flag,
	_Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
	{	/* store _Value while locked */
	_Lock_spin_lock(_Flag, _Order);
	*_Tgt = _Value;
	_Unlock_spin_lock(_Flag, _Order);
	}

 _Uint2_t __CLRCALL_PURE_OR_CDECL _Atomic_load_2_locking(
	volatile _Atomic_flag_t *_Flag, _Uint2_t *_Src,
		memory_order _Order)
	{	/* load value while locked */
	_Uint2_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Src;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint2_t __CLRCALL_PURE_OR_CDECL _Atomic_exchange_2_locking(
	volatile _Atomic_flag_t *_Flag, _Uint2_t *_Src, _Uint2_t _Value,
		memory_order _Order)
	{	/* swap values while locked */
	_Uint2_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Src;
	*_Src = _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 int __CLRCALL_PURE_OR_CDECL _Atomic_compare_exchange_weak_2_locking(
	volatile _Atomic_flag_t *_Flag,
		_Uint2_t *_Tgt, _Uint2_t *_Exp, _Uint2_t _Value,
			memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values while locked */
	int res;

	_Lock_spin_lock(_Flag, _Order1);
	res = *_Tgt == *_Exp;
	if (res)
		*_Tgt = _Value;
	else
		*_Exp = *_Tgt;
	_Unlock_spin_lock(_Flag, _Order2);
	return (res);
	}

 int __CLRCALL_PURE_OR_CDECL _Atomic_compare_exchange_strong_2_locking(
	volatile _Atomic_flag_t *_Flag,
		_Uint2_t *_Tgt, _Uint2_t *_Exp, _Uint2_t _Value,
			memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values while locked */
	int res;

	_Lock_spin_lock(_Flag, _Order1);
	res = *_Tgt == *_Exp;
	if (res)
		*_Tgt = _Value;
	else
		*_Exp = *_Tgt;
	_Unlock_spin_lock(_Flag, _Order2);
	return (res);
	}

 _Uint2_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_add_2_locking(
	volatile _Atomic_flag_t *_Flag, _Uint2_t *_Tgt, _Uint2_t _Value,
		memory_order _Order)
	{	/* add to value while locked */
	_Uint2_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt += _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint2_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_sub_2_locking(
	volatile _Atomic_flag_t *_Flag, _Uint2_t *_Tgt, _Uint2_t _Value,
		memory_order _Order)
	{	/* subtract from value while locked */
	_Uint2_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt -= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint2_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_and_2_locking(
	volatile _Atomic_flag_t *_Flag, _Uint2_t *_Tgt, _Uint2_t _Value,
		memory_order _Order)
	{	/* and with value while locked */
	_Uint2_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt &= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint2_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_or_2_locking(
	volatile _Atomic_flag_t *_Flag, _Uint2_t *_Tgt, _Uint2_t _Value,
		memory_order _Order)
	{	/* or with value while locked */
	_Uint2_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt |= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint2_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_xor_2_locking(
	volatile _Atomic_flag_t *_Flag, _Uint2_t *_Tgt, _Uint2_t _Value,
		memory_order _Order)
	{	/* xor with value while locked */
	_Uint2_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt ^= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}
 #endif /* _ATOMIC_MAXBYTES_LOCK_FREE < 2 */

 #if _ATOMIC_MAXBYTES_LOCK_FREE < 4
 void __CLRCALL_PURE_OR_CDECL _Atomic_store_4_locking(
	volatile _Atomic_flag_t *_Flag, _Uint4_t *_Tgt, _Uint4_t _Value,
		memory_order _Order)
	{	/* store _Value while locked */
	_Lock_spin_lock(_Flag, _Order);
	*_Tgt = _Value;
	_Unlock_spin_lock(_Flag, _Order);
	}

 _Uint4_t __CLRCALL_PURE_OR_CDECL _Atomic_load_4_locking(
	volatile _Atomic_flag_t *_Flag, _Uint4_t *_Src,
		memory_order _Order)
	{	/* load value while locked */
	_Uint4_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Src;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint4_t __CLRCALL_PURE_OR_CDECL _Atomic_exchange_4_locking(
	volatile _Atomic_flag_t *_Flag, _Uint4_t *_Src, _Uint4_t _Value,
		memory_order _Order)
	{	/* swap values while locked */
	_Uint4_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Src;
	*_Src = _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 int __CLRCALL_PURE_OR_CDECL _Atomic_compare_exchange_weak_4_locking(
	volatile _Atomic_flag_t *_Flag,
		_Uint4_t *_Tgt, _Uint4_t *_Exp, _Uint4_t _Value,
			memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values while locked */
	int res;

	_Lock_spin_lock(_Flag, _Order1);
	res = *_Tgt == *_Exp;
	if (res)
		*_Tgt = _Value;
	else
		*_Exp = *_Tgt;
	_Unlock_spin_lock(_Flag, _Order2);
	return (res);
	}

 int __CLRCALL_PURE_OR_CDECL _Atomic_compare_exchange_strong_4_locking(
	volatile _Atomic_flag_t *_Flag,
		_Uint4_t *_Tgt, _Uint4_t *_Exp, _Uint4_t _Value,
			memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values while locked */
	int res;

	_Lock_spin_lock(_Flag, _Order1);
	res = *_Tgt == *_Exp;
	if (res)
		*_Tgt = _Value;
	else
		*_Exp = *_Tgt;
	_Unlock_spin_lock(_Flag, _Order2);
	return (res);
	}

 _Uint4_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_add_4_locking(
	volatile _Atomic_flag_t *_Flag, _Uint4_t *_Tgt, _Uint4_t _Value,
		memory_order _Order)
	{	/* add to value while locked */
	_Uint4_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt += _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint4_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_sub_4_locking(
	volatile _Atomic_flag_t *_Flag, _Uint4_t *_Tgt, _Uint4_t _Value,
		memory_order _Order)
	{	/* subtract from value while locked */
	_Uint4_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt -= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint4_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_and_4_locking(
	volatile _Atomic_flag_t *_Flag, _Uint4_t *_Tgt, _Uint4_t _Value,
		memory_order _Order)
	{	/* and with value while locked */
	_Uint4_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt &= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint4_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_or_4_locking(
	volatile _Atomic_flag_t *_Flag, _Uint4_t *_Tgt, _Uint4_t _Value,
		memory_order _Order)
	{	/* or with value while locked */
	_Uint4_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt |= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint4_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_xor_4_locking(
	volatile _Atomic_flag_t *_Flag, _Uint4_t *_Tgt, _Uint4_t _Value,
		memory_order _Order)
	{	/* xor with value while locked */
	_Uint4_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt ^= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}
 #endif /* _ATOMIC_MAXBYTES_LOCK_FREE < 4 */

 #if _ATOMIC_MAXBYTES_LOCK_FREE < 8
 void __CLRCALL_PURE_OR_CDECL _Atomic_store_8_locking(
	volatile _Atomic_flag_t *_Flag, _Uint8_t *_Tgt, _Uint8_t _Value,
		memory_order _Order)
	{	/* store _Value while locked */
	_Lock_spin_lock(_Flag, _Order);
	*_Tgt = _Value;
	_Unlock_spin_lock(_Flag, _Order);
	}

 _Uint8_t __CLRCALL_PURE_OR_CDECL _Atomic_load_8_locking(
	volatile _Atomic_flag_t *_Flag, _Uint8_t *_Src,
		memory_order _Order)
	{	/* load value while locked */
	_Uint8_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Src;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint8_t __CLRCALL_PURE_OR_CDECL _Atomic_exchange_8_locking(
	volatile _Atomic_flag_t *_Flag, _Uint8_t *_Src, _Uint8_t _Value,
	memory_order _Order)
	{	/* swap values while locked */
	_Uint8_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Src;
	*_Src = _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 int __CLRCALL_PURE_OR_CDECL _Atomic_compare_exchange_weak_8_locking(
	volatile _Atomic_flag_t *_Flag,
		_Uint8_t *_Tgt, _Uint8_t *_Exp, _Uint8_t _Value,
			memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values while locked */
	int res;

	_Lock_spin_lock(_Flag, _Order1);
	res = *_Tgt == *_Exp;
	if (res)
		*_Tgt = _Value;
	else
		*_Exp = *_Tgt;
	_Unlock_spin_lock(_Flag, _Order2);
	return (res);
	}

 int __CLRCALL_PURE_OR_CDECL _Atomic_compare_exchange_strong_8_locking(
	volatile _Atomic_flag_t *_Flag,
		_Uint8_t *_Tgt, _Uint8_t *_Exp, _Uint8_t _Value,
			memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values while locked */
	int res;

	_Lock_spin_lock(_Flag, _Order1);
	res = *_Tgt == *_Exp;
	if (res)
		*_Tgt = _Value;
	else
		*_Exp = *_Tgt;
	_Unlock_spin_lock(_Flag, _Order2);
	return (res);
	}

 _Uint8_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_add_8_locking(
	volatile _Atomic_flag_t *_Flag,
		_Uint8_t *_Tgt, _Uint8_t _Value,
			memory_order _Order)
	{	/* add to value while locked */
	_Uint8_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt += _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint8_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_sub_8_locking(
	volatile _Atomic_flag_t *_Flag, _Uint8_t *_Tgt, _Uint8_t _Value,
		memory_order _Order)
	{	/* subtract from value while locked */
	_Uint8_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt -= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint8_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_and_8_locking(
	volatile _Atomic_flag_t *_Flag, _Uint8_t *_Tgt, _Uint8_t _Value,
		memory_order _Order)
	{	/* and with value while locked */
	_Uint8_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt &= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint8_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_or_8_locking(
	volatile _Atomic_flag_t *_Flag, _Uint8_t *_Tgt, _Uint8_t _Value,
		memory_order _Order)
	{	/* or with value while locked */
	_Uint8_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt |= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}

 _Uint8_t __CLRCALL_PURE_OR_CDECL _Atomic_fetch_xor_8_locking(
	volatile _Atomic_flag_t *_Flag, _Uint8_t *_Tgt, _Uint8_t _Value,
		memory_order _Order)
	{	/* xor with value while locked */
	_Uint8_t res;

	_Lock_spin_lock(_Flag, _Order);
	res = *_Tgt;
	*_Tgt ^= _Value;
	_Unlock_spin_lock(_Flag, _Order);
	return (res);
	}
 #endif /* _ATOMIC_MAXBYTES_LOCK_FREE < 8 */

 #if _ATOMIC_FENCE_USES_LOCK
 void __CLRCALL_PURE_OR_CDECL _Atomic_thread_fence_locking(memory_order _Order)
	{	/* force memory visibility and inhibit compiler reordering */
	_Atomic_flag_t flag = 0;
	_Atomic_flag_test_and_set_locking(&flag, _Order);
	}

 void __CLRCALL_PURE_OR_CDECL _Atomic_signal_fence_locking(memory_order _Order)
	{	/* force memory visibility and inhibit compiler reordering */
	_Atomic_flag_t flag = 0;
	_Atomic_flag_test_and_set_locking(&flag, _Order);
	}
 #endif /* _ATOMIC_FENCE_USES_LOCK */
_STD_END
 #else /* _HAS_CPP0X */
 #endif /* _HAS_CPP0X */

/*
 * Copyright (c) 1992-2011 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V5.40:0009 */
