/* atomic_x86.c -- atomic implementation for x86 */
#include "xatomic.h"
 #ifndef _M_CEE_PURE
  #include <intrin.h>
  #pragma warning (disable: 4100 4793)

_STD_BEGIN
  #define _Compiler_barrier()	_ReadWriteBarrier()

 int __cdecl _Atomic_flag_test_and_set(volatile _Atomic_flag_t *_Flag,
	memory_order _Order)
	{	/* atomically test flag and set to true */
	return (_interlockedbittestandset(_Flag, 1));	/* set bit 0 */
	}

 void __cdecl _Atomic_flag_clear(volatile _Atomic_flag_t *_Flag,
	memory_order _Order)
	{	/* atomically clear flag */
	_interlockedbittestandreset(_Flag, 1);	/* reset bit 0 */
	}

 #if _MS_64
 #else /* _MS_64 */
	/* 1-byte and 2-byte functions; not used with MS 64-bit */
static void _Store_relaxed_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
	{	/* store _Value atomically with relaxed memory order */
  #if _MS_32
	_Compiler_barrier();
	__asm
		{
		mov al, _Value;
		mov edx, _Tgt;
		mov [edx], al;
		}
	_Compiler_barrier();
  #endif /* _MS_32 */
	}

static void _Store_release_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
	{	/* store _Value atomically with release memory order */
  #if _MS_32
	_Compiler_barrier();
	__asm
		{
		mov al, _Value;
		mov edx, _Tgt;
		mov [edx], al;
		}
	_Compiler_barrier();
  #endif /* _MS_32 */
	}

	/* _Atomic_store_1 */
static void _Store_seq_cst_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
	{	/* store _Value atomically with sequentially
			consistent memory order */
	_InterlockedExchange8((volatile char *)_Tgt, _Value);
	}

 void __cdecl _Atomic_store_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value,
	memory_order _Order)
	{	/* store _Value atomically */
	switch(_Order)
		{
		case memory_order_relaxed:
			_Store_relaxed_1(_Tgt, _Value);
			break;

		case memory_order_release:
		case memory_order_acq_rel:
			_Store_release_1(_Tgt, _Value);
			break;

		case memory_order_consume:
		case memory_order_acquire:
		case memory_order_seq_cst:
			_Store_seq_cst_1(_Tgt, _Value);
			break;
		}
	}

	/* _Atomic_load_1 */
static _Uint1_t _Load_seq_cst_1(volatile _Uint1_t *_Tgt)
	{	/* load from *_Tgt atomically with
			sequentially consistent memory order */
	_Uint1_t _Value;
	_Value = _InterlockedOr8((volatile char *)_Tgt, 0);

	return (_Value);
	}

 _Uint1_t __cdecl _Atomic_load_1(volatile _Uint1_t *_Tgt, memory_order _Order)
	{	/* load from *_Tgt atomically
		use sequentially consistent for all memory orders: */
	return (_Load_seq_cst_1(_Tgt));
	}

	/* _Atomic_exchange_1 */
static _Uint1_t _Exchange_seq_cst_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
	{	/* exchange _Value and *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedExchange8((volatile char *)_Tgt, _Value);

	return (_Value);
	}

 _Uint1_t __cdecl _Atomic_exchange_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value,
	memory_order _Order)
	{	/* exchange _Value and *_Tgt atomically
		use sequentially consistent for all memory orders: */
	return (_Exchange_seq_cst_1(_Tgt, _Value));
	}

	/* _Atomic_compare_exchange_weak_1, _Atomic_compare_exchange_strong_1 */
static int _Compare_exchange_seq_cst_1(volatile _Uint1_t *_Tgt,
	_Uint1_t *_Exp, _Uint1_t _Value)
	{	/* compare and exchange values atomically with
			sequentially consistent memory order */
	_Uint1_t res;
	_Uint1_t prev = _InterlockedCompareExchange8((volatile char *)_Tgt,
		_Value, *_Exp);
	if (prev == *_Exp)
		res = 1;
	else
		{	/* copy old value */
		res = 0;
		*_Exp = prev;
		}

	return (res);
	}

 int __cdecl _Atomic_compare_exchange_weak_1(
	volatile _Uint1_t *_Tgt, _Uint1_t *_Exp, _Uint1_t _Value,
	memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Compare_exchange_seq_cst_1(_Tgt, _Exp, _Value));
	}

 int __cdecl _Atomic_compare_exchange_strong_1(
	volatile _Uint1_t *_Tgt, _Uint1_t *_Exp, _Uint1_t _Value,
	memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Compare_exchange_seq_cst_1(_Tgt, _Exp, _Value));
	}

	/* _Atomic_fetch_add_1, _Atomic_fetch_sub_1 */
static _Uint1_t _Fetch_add_seq_cst_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
	{	/* add _Value to *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedExchangeAdd8((volatile char *)_Tgt, _Value);

	return (_Value);
	}

 _Uint1_t __cdecl _Atomic_fetch_add_1(
	volatile _Uint1_t *_Tgt, _Uint1_t _Value, memory_order _Order)
	{	/* add _Value to *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_add_seq_cst_1(_Tgt, _Value));
	}

 _Uint1_t __cdecl _Atomic_fetch_sub_1(
	volatile _Uint1_t *_Tgt, _Uint1_t _Value, memory_order _Order)
	{	/* subtract _Value from *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_add_seq_cst_1(_Tgt, -_Value));
	}

	/* _Atomic_fetch_and_1 */
_Uint1_t _Fetch_and_seq_cst_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
	{	/* and _Value with *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedAnd8((volatile char *)_Tgt, _Value);

	return (_Value);
	}

 _Uint1_t __cdecl _Atomic_fetch_and_1(
	volatile _Uint1_t *_Tgt, _Uint1_t _Value, memory_order _Order)
	{	/* and _Value with *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_and_seq_cst_1(_Tgt, _Value));
	}

	/* _Atomic_fetch_or_1 */
_Uint1_t _Fetch_or_seq_cst_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
	{	/* or _Value with *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedOr8((volatile char *)_Tgt, _Value);

	return (_Value);
	}

 _Uint1_t __cdecl _Atomic_fetch_or_1(
	volatile _Uint1_t *_Tgt, _Uint1_t _Value, memory_order _Order)
	{	/* or _Value with *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_or_seq_cst_1(_Tgt, _Value));
	}

	/* _Atomic_fetch_xor_1 */
_Uint1_t _Fetch_xor_seq_cst_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
	{	/* xor _Value with *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedXor8((volatile char *)_Tgt, _Value);

	return (_Value);
	}

 _Uint1_t __cdecl _Atomic_fetch_xor_1(
	volatile _Uint1_t *_Tgt, _Uint1_t _Value, memory_order _Order)
	{	/* xor _Value with *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_xor_seq_cst_1(_Tgt, _Value));
	}

	/* _Atomic_store_2 */
static void _Store_relaxed_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
	{	/* store _Value atomically with relaxed memory order */
  #if _MS_32
	_Compiler_barrier();
	__asm
		{
		mov ax, _Value;
		mov edx, _Tgt;
		mov [edx], ax;
		}
	_Compiler_barrier();
  #endif /* _MS_32 */
	}

static void _Store_release_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
	{	/* store _Value atomically with release memory order */
  #if _MS_32
	_Compiler_barrier();
	__asm
		{
		mov ax, _Value;
		mov edx, _Tgt;
		mov [edx], ax;
		}
	_Compiler_barrier();
  #endif /* _MS_32 */
	}

static void _Store_seq_cst_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
	{	/* store _Value atomically with sequentially
			consistent memory order */
	_InterlockedExchange16((volatile short *)_Tgt, _Value);
	}

 void __cdecl _Atomic_store_2(
	volatile _Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
	{	/* store _Value atomically */
	switch(_Order)
		{
		case memory_order_relaxed:
			_Store_relaxed_2(_Tgt, _Value);
			break;
		case memory_order_release:
		case memory_order_acq_rel:
			_Store_release_2(_Tgt, _Value);
			break;
		case memory_order_consume:
		case memory_order_acquire:
		case memory_order_seq_cst:
			_Store_seq_cst_2(_Tgt, _Value);
			break;
		}
	}

	/* _Atomic_load_2 */
static _Uint2_t _Load_seq_cst_2(volatile _Uint2_t *_Tgt)
	{	/* load from *_Tgt atomically with
			sequentially consistent memory order */
	_Uint2_t _Value;

	_Value = _InterlockedOr16((volatile short *)_Tgt, 0);

	return (_Value);
	}

 _Uint2_t __cdecl _Atomic_load_2(
	volatile _Uint2_t *_Tgt, memory_order _Order)
	{	/* load from *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Load_seq_cst_2(_Tgt));
	}

	/* _Atomic_exchange_2 */
static _Uint2_t _Exchange_seq_cst_2(
	volatile _Uint2_t *_Tgt, _Uint2_t _Value)
	{	/* exchange _Value and *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedExchange16((volatile short *)_Tgt, _Value);

	return (_Value);
	}

 _Uint2_t __cdecl _Atomic_exchange_2(
	volatile _Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
	{	/* exchange _Value and *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Exchange_seq_cst_2(_Tgt, _Value));
	}

	/* _Atomic_compare_exchange_weak_2, _Atomic_compare_exchange_strong_2 */
static int _Compare_exchange_seq_cst_2(volatile _Uint2_t *_Tgt,
	_Uint2_t *_Exp, _Uint2_t _Value)
	{	/* compare and exchange values atomically with
			sequentially consistent memory order */
	_Uint1_t res;

	_Uint2_t prev = _InterlockedCompareExchange16((volatile short *)_Tgt,
		_Value, *_Exp);
	if (prev == *_Exp)
		res = 1;
	else
		{	/* copy old value */
		res = 0;
		*_Exp = prev;
		}

	return (res);
	}

 int __cdecl _Atomic_compare_exchange_weak_2(
	volatile _Uint2_t *_Tgt, _Uint2_t *_Exp, _Uint2_t _Value,
	memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Compare_exchange_seq_cst_2(_Tgt, _Exp, _Value));
	}

 int __cdecl _Atomic_compare_exchange_strong_2(
	volatile _Uint2_t *_Tgt, _Uint2_t *_Exp, _Uint2_t _Value,
	memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Compare_exchange_seq_cst_2(_Tgt, _Exp, _Value));
	}

	/* _Atomic_fetch_add_2, _Atomic_fetch_sub_2 */
static _Uint2_t _Fetch_add_seq_cst_2(volatile _Uint2_t *_Tgt,
	_Uint2_t _Value)
	{	/* add _Value to *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedExchangeAdd16((volatile short *)_Tgt, _Value);

	return (_Value);
	}

 _Uint2_t __cdecl _Atomic_fetch_add_2(
	volatile _Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
	{	/* add _Value to *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_add_seq_cst_2(_Tgt, _Value));
	}

	/* _Atomic_fetch_sub_2 */
 _Uint2_t __cdecl _Atomic_fetch_sub_2(
	volatile _Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
	{	/* subtract _Value from *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_add_seq_cst_2(_Tgt, -_Value));
	}

	/* _Atomic_fetch_and_2 */
_Uint2_t _Fetch_and_seq_cst_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
	{	/* and _Value with *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedAnd16((volatile short *)_Tgt, _Value);

	return (_Value);
	}

 _Uint2_t __cdecl _Atomic_fetch_and_2(
	volatile _Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
	{	/* and _Value with *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_and_seq_cst_2(_Tgt, _Value));
	}

	/* _Atomic_fetch_or_2 */
_Uint2_t _Fetch_or_seq_cst_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
	{	/* or _Value with *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedOr16((volatile short *)_Tgt, _Value);

	return (_Value);
	}

 _Uint2_t __cdecl _Atomic_fetch_or_2(
	volatile _Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
	{	/* or _Value with *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_or_seq_cst_2(_Tgt, _Value));
	}

	/* _Atomic_fetch_xor_2 */
_Uint2_t _Fetch_xor_seq_cst_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
	{	/* xor _Value with *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedXor16((volatile short *)_Tgt, _Value);

	return (_Value);
	}

 _Uint2_t __cdecl _Atomic_fetch_xor_2(
	volatile _Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
	{	/* xor _Value with *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_xor_seq_cst_2(_Tgt, _Value));
	}
 #endif /* _MS_64 */

	/* _Atomic_store_4 */
static void _Store_relaxed_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
	{	/* store _Value atomically with relaxed memory order */
 #if _MS_32
	_Compiler_barrier();
	__asm
		{
		mov eax, _Value;
		mov edx, _Tgt;
		mov [edx], eax;
		}
	_Compiler_barrier();

 #elif _MS_64
	_Compiler_barrier();
	*_Tgt = _Value;
	_Compiler_barrier();
 #endif /* _MS_32 */
	}

static void _Store_release_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
	{	/* store _Value atomically with release memory order */
 #if _MS_32
	_Compiler_barrier();
	__asm
		{
		mov eax, _Value;
		mov edx, _Tgt;
		mov [edx], eax;
		}
	_Compiler_barrier();

 #elif _MS_64
	_Compiler_barrier();
	*_Tgt = _Value;
	_Compiler_barrier();
 #endif /* _MS_32 */
	}

static void _Store_seq_cst_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
	{	/* store _Value atomically with
			sequentially consistent memory order */
	_InterlockedExchange((volatile long *)_Tgt, _Value);
	}

 void __cdecl _Atomic_store_4(
	volatile _Uint4_t *_Tgt, _Uint4_t _Value, memory_order _Order)
	{	/* store _Value atomically */
	switch(_Order)
		{
		case memory_order_relaxed:
			_Store_relaxed_4(_Tgt, _Value);
			break;
		case memory_order_release:
		case memory_order_acq_rel:
			_Store_release_4(_Tgt, _Value);
			break;
		case memory_order_consume:
		case memory_order_acquire:
		case memory_order_seq_cst:
			_Store_seq_cst_4(_Tgt, _Value);
			break;
		}
	}

	/* _Atomic_load_4 */
static _Uint4_t _Load_seq_cst_4(volatile _Uint4_t *_Tgt)
	{	/* load from *_Tgt atomically with
			sequentially consistent memory order */
	_Uint4_t _Value;

	_Value = _InterlockedOr((volatile long *)_Tgt, 0);
	return (_Value);
	}

 _Uint4_t __cdecl _Atomic_load_4(
	volatile _Uint4_t *_Tgt, memory_order _Order)
	{	/* load from *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Load_seq_cst_4(_Tgt));
	}

	/* _Atomic_exchange_4 */
static _Uint4_t _Exchange_seq_cst_4(
	volatile _Uint4_t *_Tgt, _Uint4_t _Value)
	{	/* exchange _Value and *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedExchange((volatile long *)_Tgt, _Value);
	return (_Value);
	}

 _Uint4_t __cdecl _Atomic_exchange_4(
	volatile _Uint4_t *_Tgt, _Uint4_t _Value, memory_order _Order)
	{	/* exchange _Value and *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Exchange_seq_cst_4(_Tgt, _Value));
	}

	/* _Atomic_compare_exchange_weak_4, _Atomic_compare_exchange_strong_4 */
static int _Compare_exchange_seq_cst_4(volatile _Uint4_t *_Tgt,
	_Uint4_t *_Exp, _Uint4_t _Value)
	{	/* compare and exchange values atomically with
			sequentially consistent memory order */
	_Uint1_t res;

	_Uint4_t prev = _InterlockedCompareExchange((volatile long *)_Tgt,
		_Value, *_Exp);
	if (prev == *_Exp)
		res = 1;
	else
		{	/* copy old value */
		res = 0;
		*_Exp = prev;
		}
	return (res);
	}

 int __cdecl _Atomic_compare_exchange_weak_4(
	volatile _Uint4_t *_Tgt, _Uint4_t *_Exp, _Uint4_t _Value,
	memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Compare_exchange_seq_cst_4(_Tgt, _Exp, _Value));
	}

 int __cdecl _Atomic_compare_exchange_strong_4(
	volatile _Uint4_t *_Tgt, _Uint4_t *_Exp, _Uint4_t _Value,
	memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Compare_exchange_seq_cst_4(_Tgt, _Exp, _Value));
	}

	/* _Atomic_fetch_add_4, _Atomic_fetch_sub_4 */
static _Uint4_t _Fetch_add_seq_cst_4(volatile _Uint4_t *_Tgt,
	_Uint4_t _Value)
	{	/* add _Value to *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedExchangeAdd((volatile long *)_Tgt, _Value);
	return (_Value);
	}

 _Uint4_t __cdecl _Atomic_fetch_add_4(
	volatile _Uint4_t *_Tgt, _Uint4_t _Value, memory_order _Order)
	{	/* add _Value to *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_add_seq_cst_4(_Tgt, _Value));
	}

 _Uint4_t __cdecl _Atomic_fetch_sub_4(
	volatile _Uint4_t *_Tgt, _Uint4_t _Value, memory_order _Order)
	{	/* subtract _Value from *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_add_seq_cst_4(_Tgt, 0 - _Value));
	}

	/* _Atomic_fetch_and_4 */
_Uint4_t _Fetch_and_seq_cst_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
	{	/* and _Value with *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedAnd((volatile long *)_Tgt, _Value);
	return (_Value);
	}

 _Uint4_t __cdecl _Atomic_fetch_and_4(
	volatile _Uint4_t *_Tgt, _Uint4_t _Value, memory_order _Order)
	{	/* and _Value with *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_and_seq_cst_4(_Tgt, _Value));
	}

	/* _Atomic_fetch_or_4 */
_Uint4_t _Fetch_or_seq_cst_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
	{	/* or _Value with *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedOr((volatile long *)_Tgt, _Value);
	return (_Value);
	}

 _Uint4_t __cdecl _Atomic_fetch_or_4(
	volatile _Uint4_t *_Tgt, _Uint4_t _Value, memory_order _Order)
	{	/* or _Value with *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_or_seq_cst_4(_Tgt, _Value));
	}

	/* _Atomic_fetch_xor_4 */
_Uint4_t _Fetch_xor_seq_cst_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
	{	/* xor _Value with *_Tgt atomically with
			sequentially consistent memory order */
	_Value = _InterlockedXor((volatile long *)_Tgt, _Value);
	return (_Value);
	}

 _Uint4_t __cdecl _Atomic_fetch_xor_4(
	volatile _Uint4_t *_Tgt, _Uint4_t _Value, memory_order _Order)
	{	/* xor _Value with *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_xor_seq_cst_4(_Tgt, _Value));
	}

	/* _Atomic_store_8 */
 #if _MS_64
static void _Store_relaxed_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
	{	/* store _Value atomically with relaxed memory order */
	_Compiler_barrier();
	*_Tgt = _Value;
	_Compiler_barrier();
	}

static void _Store_release_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
	{	/* store _Value atomically with release memory order */
	_Compiler_barrier();
	*_Tgt = _Value;
	_Compiler_barrier();
	}
 #endif /* _MS_64 */

static void _Store_seq_cst_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
	{	/* store _Value atomically with
			sequentially consistent memory order */
 #if _MS_32
	_Compiler_barrier();
	__asm
		{
		mov esi, _Tgt;
		mov ecx, dword ptr _Value[4];
		mov ebx, dword ptr _Value;
	again:
		lock cmpxchg8b [esi];
		jnz again;
		}
	_Compiler_barrier();

 #elif _MS_64
	_InterlockedExchange64((volatile _LONGLONG *)_Tgt, _Value);
 #endif /* _MS_32 */
	}

 void __cdecl _Atomic_store_8(
	volatile _Uint8_t *_Tgt, _Uint8_t _Value, memory_order _Order)
	{	/* store _Value atomically */
 #if _MS_64
	switch(_Order)
		{
		case memory_order_relaxed:
			_Store_relaxed_8(_Tgt, _Value);
			break;
		case memory_order_release:
		case memory_order_acq_rel:
			_Store_release_8(_Tgt, _Value);
			break;
		case memory_order_consume:
		case memory_order_acquire:
		case memory_order_seq_cst:
			_Store_seq_cst_8(_Tgt, _Value);
			break;
		}

 #else /* _MS_64 */
	_Store_seq_cst_8(_Tgt, _Value);
 #endif /* _MS_64 */
	}

	/* _Atomic_load_8 */
static _Uint8_t _Load_seq_cst_8(volatile _Uint8_t *_Tgt)
	{	/* load from *_Tgt atomically with
			sequentially consistent memory order */
	_Uint8_t _Value;

 #if _MS_32
	_Compiler_barrier();
	__asm
		{
		mov esi, _Tgt;
		mov ecx, edx;
		mov ebx, eax;
		lock cmpxchg8b [esi];
		mov dword ptr _Value[4], edx;
		mov dword ptr _Value, eax;
		}
	_Compiler_barrier();

 #elif _MS_64
	_Value = _InterlockedOr64((volatile _LONGLONG *)_Tgt, 0);
 #endif /* _MS_32 */

	return (_Value);
	}

 _Uint8_t __cdecl _Atomic_load_8(
	volatile _Uint8_t *_Tgt, memory_order _Order)
	{	/* load from *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Load_seq_cst_8(_Tgt));
	}

	/* _Atomic_exchange_8 */
static _Uint8_t _Exchange_seq_cst_8(volatile _Uint8_t *_Tgt,
	_Uint8_t _Value)
	{	/* exchange _Value and *_Tgt atomically with
			sequentially consistent memory order */
 #if _MS_32
	_Compiler_barrier();
	__asm
		{
		mov esi, _Tgt;
		mov ecx, dword ptr _Value[4];
		mov ebx, dword ptr _Value;
	again:
		lock cmpxchg8b [esi];
		jnz again;
		mov dword ptr _Value[4], edx;
		mov dword ptr _Value, eax;
		}
	_Compiler_barrier();

 #elif _MS_64
	_Value = _InterlockedExchange64((volatile _LONGLONG *)_Tgt, _Value);
 #endif /* _MS_32 */

	return (_Value);
	}

 _Uint8_t __cdecl _Atomic_exchange_8(
	volatile _Uint8_t *_Tgt, _Uint8_t _Value, memory_order _Order)
	{	/* exchange _Value and *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Exchange_seq_cst_8(_Tgt, _Value));
	}

	/* _Atomic_compare_exchange_weak_8, _Atomic_compare_exchange_strong_8 */
static int _Compare_exchange_seq_cst_8(volatile _Uint8_t *_Tgt,
	_Uint8_t *_Exp, _Uint8_t _Value)
	{	/* compare and exchange values atomically with
			sequentially consistent memory order */
	_Uint1_t res;

	_Uint8_t prev = _InterlockedCompareExchange64((volatile _LONGLONG *)_Tgt,
		_Value, *_Exp);
	if (prev == *_Exp)
		res = 1;
	else
		{	/* copy value */
		res = 0;
		*_Exp = prev;
		}
	return (res);
	}

 int __cdecl _Atomic_compare_exchange_weak_8(
	volatile _Uint8_t *_Tgt, _Uint8_t *_Exp, _Uint8_t _Value,
	memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Compare_exchange_seq_cst_8(_Tgt, _Exp, _Value));
	}

 int __cdecl _Atomic_compare_exchange_strong_8(
	volatile _Uint8_t *_Tgt, _Uint8_t *_Exp, _Uint8_t _Value,
	memory_order _Order1, memory_order _Order2)
	{	/* compare and exchange values atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Compare_exchange_seq_cst_8(_Tgt, _Exp, _Value));
	}

	/* _Atomic_fetch_add_8 */
_Uint8_t _Fetch_add_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
	{	/* add _Value to *_Tgt atomically with
			sequentially consistent memory order */
 #if _MS_32
	_Compiler_barrier();
	__asm
		{
		mov esi, _Tgt;
		mov edx, 4[esi];
		mov eax, [esi];
	again:
		mov ecx, edx;
		mov ebx, eax;
		add ebx, dword ptr _Value;
		adc ecx, dword ptr _Value[4];
		lock cmpxchg8b [esi];
		jnz again;
		mov dword ptr _Value, eax;
		mov dword ptr _Value[4], edx;
		}
	_Compiler_barrier();

 #elif _MS_64
	_Value = _InterlockedExchangeAdd64((volatile _LONGLONG *)_Tgt, _Value);
 #endif /* _MS_32 */
	return (_Value);
	}

 _Uint8_t __cdecl _Atomic_fetch_add_8(
	volatile _Uint8_t *_Tgt, _Uint8_t _Value, memory_order _Order)
	{	/* add _Value to *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_add_8(_Tgt, _Value));
	}

	/* _Atomic_fetch_sub_8 */
 _Uint8_t __cdecl _Atomic_fetch_sub_8(
	volatile _Uint8_t *_Tgt, _Uint8_t _Value, memory_order _Order)
	{	/* subtract _Value from *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_add_8(_Tgt, 0 - _Value));
	}

	/* _Atomic_fetch_and_8 */
_Uint8_t _Fetch_and_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
	{	/* and _Value with *_Tgt atomically with
			sequentially consistent memory order */
 #if _MS_32
	_Compiler_barrier();
	__asm
		{
		mov esi, _Tgt;
		mov eax, [esi];
		mov edx, 4[esi];
	again:
		mov ecx, edx;
		mov ebx, eax;
		and ebx, dword ptr _Value;
		and ecx, dword ptr _Value[4];
		lock cmpxchg8b [esi];
		jnz again;
		mov dword ptr _Value, eax;
		mov dword ptr _Value[4], edx;
		}
	_Compiler_barrier();

 #elif _MS_64
	_Value = _InterlockedAnd64((volatile _LONGLONG *)_Tgt, _Value);
 #endif /* _MS_32 */

	return (_Value);
	}

 _Uint8_t __cdecl _Atomic_fetch_and_8(
	volatile _Uint8_t *_Tgt, _Uint8_t _Value, memory_order _Order)
	{	/* and _Value with *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_and_8(_Tgt, _Value));
	}

	/* _Atomic_fetch_or_8 */
_Uint8_t _Fetch_or_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
	{	/* or _Value with *_Tgt atomically with
			sequentially consistent memory order */
 #if _MS_32
	_Compiler_barrier();
	__asm
		{
		mov esi, _Tgt;
		mov eax, [esi];
		mov edx, 4[esi];
	again:
		mov ecx, edx;
		mov ebx, eax;
		or ebx, dword ptr _Value;
		or ecx, dword ptr _Value[4];
		lock cmpxchg8b [esi];
		jnz again;
		mov dword ptr _Value, eax;
		mov dword ptr _Value[4], edx;
		}
	_Compiler_barrier();

 #elif _MS_64
	_Value = _InterlockedOr64((volatile _LONGLONG *)_Tgt, _Value);
 #endif /* _MS_32 */

	return (_Value);
	}

 _Uint8_t __cdecl _Atomic_fetch_or_8(
	volatile _Uint8_t *_Tgt, _Uint8_t _Value, memory_order _Order)
	{	/* or _Value with *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_or_8(_Tgt, _Value));
	}

	/* _Atomic_fetch_xor_8 */
_Uint8_t _Fetch_xor_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
	{	/* xor _Value with *_Tgt atomically with
			sequentially consistent memory order */
 #if _MS_32
	_Compiler_barrier();
	__asm
		{
		mov esi, _Tgt;
		mov eax, [esi];
		mov edx, 4[esi];
	again:
		mov ecx, edx;
		mov ebx, eax;
		xor ebx, dword ptr _Value;
		xor ecx, dword ptr _Value[4];
		lock cmpxchg8b [esi];
		jnz again;
		mov dword ptr _Value, eax;
		mov dword ptr _Value[4], edx;
		}
	_Compiler_barrier();

 #elif _MS_64
	_Value = _InterlockedXor64((volatile _LONGLONG *)_Tgt, _Value);
 #endif /* _MS_32 */

	return (_Value);
	}

 _Uint8_t __cdecl _Atomic_fetch_xor_8(
	volatile _Uint8_t *_Tgt, _Uint8_t _Value, memory_order _Order)
	{	/* xor _Value with *_Tgt atomically */
	/* use sequentially consistent for all memory orders: */
	return (_Fetch_xor_8(_Tgt, _Value));
	}

 void __cdecl _Atomic_thread_fence(memory_order _Order)
	{	/* force memory visibility and inhibit compiler reordering */
	_Compiler_barrier();
	if (_Order == memory_order_seq_cst)
		{	/* force visibility */
		static _Uint4_t _Guard;
		_Atomic_exchange_4(&_Guard, 0, memory_order_seq_cst);
		_Compiler_barrier();
		}
	}

 void __cdecl _Atomic_signal_fence(memory_order _Order)
	{	/* force memory visibility and inhibit compiler reordering */
	_Compiler_barrier();
	}
_STD_END
 #endif /* _M_CEE_PURE */

/*
 * Copyright (c) 1992-2011 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V5.40:0009 */
