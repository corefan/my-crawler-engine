// xxatomic internal header
// NOTE: no include guard

// Defines struct _ATOMIC_ITYPE, an atomic struct that manages objects
// of type _ITYPE whose size is _ISIZE bytes. Also defines the non-member
// and member functions for atomic operations on _ATOMIC_TYPE.
//
// Used by header <atomic> to define atomic types for integral types and
// addresses.

 #if _MS_64 && (_ISIZE == 1 || _ISIZE == 2)
  #define _ATOMIC_UINT	_Uint4_t
  #define _ATOMIC_LOCK_FREE_PRIMITIVE_NAME(name)	\
	_CONCAT(name, 4)
 #define _ATOMIC_LOCKING_PRIMITIVE_NAME(name)	\
	_CONCAT(_CONCAT(name, 4), _locking)

 #else /* _MS_64 && (_ISIZE == 1 || _ISIZE == 2) */
  #define _ATOMIC_UINT _CONCAT(_CONCAT(_Uint, _ISIZE), _t)
  #define _ATOMIC_LOCK_FREE_PRIMITIVE_NAME(name)	\
	_CONCAT(name, _ISIZE)
  #define _ATOMIC_LOCKING_PRIMITIVE_NAME(name)	\
	_CONCAT(_CONCAT(name, _ISIZE), _locking)
 #endif /* _MS_64 && (_ISIZE == 1 || _ISIZE == 2) */

#define _ATOMIC_IS_LOCK_FREE	_CONCAT(_Atomic_is_lock_free_, _ISIZE)

 #if _ATOMIC_MAXBYTES_LOCK_FREE < _ISIZE
  #define _ATOMIC_STORE(_Atom, _Value, _Order)	\
	_ATOMIC_LOCKING_PRIMITIVE_NAME(_Atomic_store_)\
	(&_Atom->_My_flag,	\
	(_ATOMIC_UINT*)&_Atom->_My_val,	\
	(_ATOMIC_UINT)_Value, _Order)

  #define _ATOMIC_LOAD(_Atom, _Order)	\
	(_ITYPE)_ATOMIC_LOCKING_PRIMITIVE_NAME(_Atomic_load_)	\
	(&_Atom->_My_flag,	\
	(_ATOMIC_UINT*)&_Atom->_My_val, _Order)

  #define _ATOMIC_EXCHANGE(_Atom, _Value, _Order)	\
	(_ITYPE)_ATOMIC_LOCKING_PRIMITIVE_NAME(_Atomic_exchange_)	\
	(&_Atom->_My_flag,	\
	(_ATOMIC_UINT*)&_Atom->_My_val, (_ATOMIC_UINT)_Value, _Order)

  #define _ATOMIC_COMPARE_EXCHANGE_WEAK(_Atom, _Expected, _Desired,\
	_Order1, _Order2)	\
	_ATOMIC_LOCKING_PRIMITIVE_NAME(_Atomic_compare_exchange_weak_)	\
	(&_Atom->_My_flag,	\
	(_ATOMIC_UINT*)&_Atom->_My_val,	\
	(_ATOMIC_UINT*)_Expected, (_ATOMIC_UINT)_Value,	\
	_Order1, _Order2)

  #define _ATOMIC_COMPARE_EXCHANGE_STRONG(_Atom, _Expected, _Desired,\
	_Order1, _Order2)	\
	_ATOMIC_LOCKING_PRIMITIVE_NAME(_Atomic_compare_exchange_strong_)	\
	(&_Atom->_My_flag,	\
	(_ATOMIC_UINT*)&_Atom->_My_val,	\
	(_ATOMIC_UINT*)_Expected, (_ATOMIC_UINT)_Value,	\
	_Order1, _Order2)

  #define _ATOMIC_FETCH_ADD(_Atom, _Value, _Order)	\
	(_ITYPE)_ATOMIC_LOCKING_PRIMITIVE_NAME(_Atomic_fetch_add_)	\
	(&_Atom->_My_flag,	\
	(_ATOMIC_UINT*)&_Atom->_My_val, (_ATOMIC_UINT)_Value, _Order)

  #define _ATOMIC_FETCH_SUB(_Atom, _Value, _Order)	\
	(_ATOMIC_UINT)_ATOMIC_LOCKING_PRIMITIVE_NAME(_Atomic_fetch_sub_)	\
	(&_Atom->_My_flag,	\
	(_ATOMIC_UINT*)&_Atom->_My_val, (_ATOMIC_UINT)_Value, _Order)

  #define _ATOMIC_FETCH_AND(_Atom, _Value, _Order)	\
	(_ITYPE)_ATOMIC_LOCKING_PRIMITIVE_NAME(_Atomic_fetch_and_)	\
	(&_Atom->_My_flag,	\
	(_ATOMIC_UINT*)&_Atom->_My_val, (_ATOMIC_UINT)_Value, _Order)

  #define _ATOMIC_FETCH_OR(_Atom, _Value, _Order)	\
	(_ITYPE)_ATOMIC_LOCKING_PRIMITIVE_NAME(_Atomic_fetch_or_)	\
	(&_Atom->_My_flag,	\
	(_ATOMIC_UINT*)&_Atom->_My_val, (_ATOMIC_UINT)_Value, _Order)

  #define _ATOMIC_FETCH_XOR(_Atom, _Value, _Order)	\
	(_ITYPE)_ATOMIC_LOCKING_PRIMITIVE_NAME(_Atomic_fetch_xor_)	\
	(&_Atom->_My_flag,	\
	(_ATOMIC_UINT*)&_Atom->_My_val, (_ATOMIC_UINT)_Value, _Order)

 #else /* _ATOMIC_MAXBYTES_LOCK_FREE < _ISIZE */
  #define _ATOMIC_STORE(_Atom, _Value, _Order)	\
	_ATOMIC_LOCK_FREE_PRIMITIVE_NAME(_Atomic_store_)\
		((_ATOMIC_UINT*)&_Atom->_My_val,	\
		(_ATOMIC_UINT)_Value, _Order)

  #define _ATOMIC_LOAD(_Atom, _Order)	\
	(_ITYPE)_ATOMIC_LOCK_FREE_PRIMITIVE_NAME(_Atomic_load_)	\
	((_ATOMIC_UINT*)&_Atom->_My_val, _Order)

  #define _ATOMIC_EXCHANGE(_Atom, _Value, _Order)	\
	(_ITYPE)_ATOMIC_LOCK_FREE_PRIMITIVE_NAME(_Atomic_exchange_)	\
	((_ATOMIC_UINT*)&_Atom->_My_val, (_ATOMIC_UINT)_Value, _Order)

  #define _ATOMIC_COMPARE_EXCHANGE_WEAK(_Atom, _Expected, _Desired,\
	_Order1, _Order2)	\
	_ATOMIC_LOCK_FREE_PRIMITIVE_NAME(_Atomic_compare_exchange_weak_)	\
	((_ATOMIC_UINT*)&_Atom->_My_val,	\
		(_ATOMIC_UINT*)_Expected, (_ATOMIC_UINT)_Value,	\
		_Order1, _Order2)

  #define _ATOMIC_COMPARE_EXCHANGE_STRONG(_Atom, _Expected, _Desired,\
	_Order1, _Order2)	\
	_ATOMIC_LOCK_FREE_PRIMITIVE_NAME(_Atomic_compare_exchange_strong_)	\
	((_ATOMIC_UINT*)&_Atom->_My_val,	\
		(_ATOMIC_UINT*)_Expected, (_ATOMIC_UINT)_Value,	\
		_Order1, _Order2)

  #define _ATOMIC_FETCH_ADD(_Atom, _Value, _Order)	\
	(_ITYPE)_ATOMIC_LOCK_FREE_PRIMITIVE_NAME(_Atomic_fetch_add_)	\
	((_ATOMIC_UINT*)&_Atom->_My_val, (_ATOMIC_UINT)_Value, _Order)

  #define _ATOMIC_FETCH_SUB(_Atom, _Value, _Order)	\
	(_ITYPE)_ATOMIC_LOCK_FREE_PRIMITIVE_NAME(_Atomic_fetch_sub_)	\
	((_ATOMIC_UINT*)&_Atom->_My_val, (_ATOMIC_UINT)_Value, _Order)

  #define _ATOMIC_FETCH_AND(_Atom, _Value, _Order)	\
	(_ITYPE)_ATOMIC_LOCK_FREE_PRIMITIVE_NAME(_Atomic_fetch_and_)	\
	((_ATOMIC_UINT*)&_Atom->_My_val, (_ATOMIC_UINT)_Value, _Order)

  #define _ATOMIC_FETCH_OR(_Atom, _Value, _Order)	\
	(_ITYPE)_ATOMIC_LOCK_FREE_PRIMITIVE_NAME(_Atomic_fetch_or_)	\
	((_ATOMIC_UINT*)&_Atom->_My_val, (_ATOMIC_UINT)_Value, _Order)

  #define _ATOMIC_FETCH_XOR(_Atom, _Value, _Order)	\
	(_ITYPE)_ATOMIC_LOCK_FREE_PRIMITIVE_NAME(_Atomic_fetch_xor_)	\
	((_ATOMIC_UINT*)&_Atom->_My_val, (_ATOMIC_UINT)_Value, _Order)
 #endif /* _ATOMIC_MAXBYTES_LOCK_FREE < _ISIZE */

		// STRUCT _ATOMIC_ITYPE
typedef struct _ATOMIC_ITYPE
	{	// struct for atomic operations on type _ITYPE
	_ITYPE operator=(_ITYPE) volatile ;
	_ITYPE operator=(_ITYPE) ;

	bool is_lock_free() const volatile ;
	bool is_lock_free() const ;
	void store(_ITYPE, memory_order = memory_order_seq_cst)
		volatile ;
	void store(_ITYPE, memory_order = memory_order_seq_cst)
		 ;
	_ITYPE load(memory_order = memory_order_seq_cst)
		const volatile ;
	_ITYPE load(memory_order = memory_order_seq_cst)
		const ;
	operator _ITYPE() const volatile;
	operator _ITYPE() const;
	_ITYPE exchange(_ITYPE,
		memory_order = memory_order_seq_cst) volatile ;
	_ITYPE exchange(_ITYPE,
		memory_order = memory_order_seq_cst) ;
	bool compare_exchange_weak(_ITYPE&, _ITYPE,
		memory_order, memory_order) volatile ;
	bool compare_exchange_weak(_ITYPE&, _ITYPE,
		memory_order, memory_order) ;
	bool compare_exchange_strong(_ITYPE&, _ITYPE,
		memory_order, memory_order) volatile ;
	bool compare_exchange_strong(_ITYPE&, _ITYPE,
		memory_order, memory_order) ;
	bool compare_exchange_weak(_ITYPE&, _ITYPE,
		memory_order = memory_order_seq_cst) volatile ;
	bool compare_exchange_weak(_ITYPE&, _ITYPE,
		memory_order = memory_order_seq_cst) ;
	bool compare_exchange_strong(_ITYPE&, _ITYPE,
		memory_order = memory_order_seq_cst) volatile ;
	bool compare_exchange_strong(_ITYPE&, _ITYPE,
		memory_order = memory_order_seq_cst) ;

 #ifdef _ATOMIC_HAS_ARITHMETIC_OPS
	_ITYPE operator++(int) volatile ;
	_ITYPE operator++(int) ;
	_ITYPE operator--(int) volatile ;
	_ITYPE operator--(int) ;
	_ITYPE operator++() volatile ;
	_ITYPE operator++() ;
	_ITYPE operator--() volatile ;
	_ITYPE operator--() ;
	_ITYPE operator+=(_ITYPE) volatile ;
	_ITYPE operator+=(_ITYPE) ;
	_ITYPE operator-=(_ITYPE) volatile ;
	_ITYPE operator-=(_ITYPE) ;
	_ITYPE operator&=(_ITYPE) volatile ;
	_ITYPE operator&=(_ITYPE) ;
	_ITYPE operator|=(_ITYPE) volatile ;
	_ITYPE operator|=(_ITYPE) ;
	_ITYPE operator^=(_ITYPE) volatile ;
	_ITYPE operator^=(_ITYPE) ;

	_ITYPE fetch_add(_ITYPE, memory_order = memory_order_seq_cst)
		volatile ;
	_ITYPE fetch_add(_ITYPE, memory_order = memory_order_seq_cst) ;
	_ITYPE fetch_sub(_ITYPE, memory_order = memory_order_seq_cst)
		volatile ;
	_ITYPE fetch_sub(_ITYPE, memory_order = memory_order_seq_cst) ;
	_ITYPE fetch_and(_ITYPE, memory_order = memory_order_seq_cst)
		volatile ;
	_ITYPE fetch_and(_ITYPE, memory_order = memory_order_seq_cst) ;
	_ITYPE fetch_or(_ITYPE, memory_order = memory_order_seq_cst)
		volatile ;
	_ITYPE fetch_or(_ITYPE, memory_order = memory_order_seq_cst) ;
	_ITYPE fetch_xor(_ITYPE, memory_order = memory_order_seq_cst)
		volatile ;
	_ITYPE fetch_xor(_ITYPE, memory_order = memory_order_seq_cst) ;
 #endif /* _ATOMIC_HAS_ARITHMETIC_OPS */

 #ifdef _ATOMIC_IS_ADDRESS_TYPE
	bool compare_exchange_weak(const void*&, const void*,
		memory_order, memory_order) volatile ;
	bool compare_exchange_weak(const void*&, const void*,
		memory_order, memory_order) ;
	bool compare_exchange_strong(const void*&, const void*,
		memory_order, memory_order) volatile ;
	bool compare_exchange_strong(const void*&, const void*,
		memory_order, memory_order) ;
	bool compare_exchange_weak(const void*&, const void*,
		memory_order = memory_order_seq_cst) volatile ;
	bool compare_exchange_weak(const void*&, const void*,
		memory_order = memory_order_seq_cst) ;
	bool compare_exchange_strong(const void*&, const void*,
		memory_order = memory_order_seq_cst) volatile ;
	bool compare_exchange_strong(const void*&, const void*,
		memory_order = memory_order_seq_cst) ;

	_ITYPE fetch_add(ptrdiff_t,
		memory_order = memory_order_seq_cst) volatile ;
	_ITYPE fetch_add(ptrdiff_t,
		memory_order = memory_order_seq_cst) ;
	_ITYPE fetch_sub(ptrdiff_t,
		memory_order = memory_order_seq_cst) volatile ;
	_ITYPE fetch_sub(ptrdiff_t,
		memory_order = memory_order_seq_cst) ;
	_ITYPE operator+=(ptrdiff_t) volatile ;
	_ITYPE operator+=(ptrdiff_t) ;
	_ITYPE operator-=(ptrdiff_t) volatile ;
	_ITYPE operator-=(ptrdiff_t) ;
 #endif /* _ATOMIC_IS_ADDRESS_TYPE */

	_ATOMIC_UINT _My_val;

 #if _ATOMIC_MAXBYTES_LOCK_FREE < _ISIZE
	mutable _Atomic_flag_t _My_flag;
 #endif /* _ATOMIC_MAXBYTES_LOCK_FREE < _ISIZE */
	} _ATOMIC_ITYPE;

 #ifndef _ATOMIC_HAS_NO_SPECIALIZATION
		// TEMPLATE SPECIALIZATION atomic<_ITYPE>
template<>
	struct atomic<_ITYPE>
		: _ATOMIC_ITYPE
	{	/* template specialization that manages
			values of _ITYPE atomically */

	_ITYPE operator=(_ITYPE _Val) volatile 
		{	// assign from _Val
		return _ATOMIC_ITYPE::operator=(_Val);
		}

	_ITYPE operator=(_ITYPE _Val) 
		{	// assign from _Val
		return _ATOMIC_ITYPE::operator=(_Val);
		}
	};
 #endif /* _ATOMIC_HAS_NO_SPECIALIZATION */

 #ifdef _ATOMIC_IS_ADDRESS_TYPE
		// TEMPLATE CLASS atomic<_Ty *>
template<class _Ty>
	struct atomic<_Ty *>
		: _Atomic_address
	{	// template that manages values of _Ty * atomically
	atomic() 
		{	// default constructor
		_Atomic_address::operator=((_Ty *)0);
		}

private:
	atomic(const atomic&);	// not defined
	atomic& operator=(const atomic&);	// not defined
	atomic& operator=(const atomic&) volatile;	// not defined

public:
	atomic(_Ty *_Right) 
		{	// construct from _Right
		_Atomic_address::operator=(_Right);
		}

	_Ty *operator=(_Ty *_Right) volatile 
		{	// assign from _Right
		return static_cast<_Ty *>(_Atomic_address::operator=(_Right));
		}

	_Ty *operator=(_Ty *_Right) 
		{	// assign from _Right
		return static_cast<_Ty *>(_Atomic_address::operator=(_Right));
		}

	void store(_Ty *_Value,
		memory_order _Order = memory_order_seq_cst) volatile 
		{	// store _Value into *this
		_Atomic_address::store(_Value, _Order);
		}

	void store(_Ty *_Value,
		memory_order _Order = memory_order_seq_cst) 
		{	// store _Value into *this
		_Atomic_address::store(_Value, _Order);
		}

	_Ty *load(memory_order _Order = memory_order_seq_cst)
		const volatile 
		{	// return value held in *this
		return static_cast<_Ty *>(_Atomic_address::load(_Order));
		}

	_Ty *load(memory_order _Order = memory_order_seq_cst)
		const 
		{	// return value held in *this
		return static_cast<_Ty *>(_Atomic_address::load(_Order));
		}

	operator _Ty *() const volatile 
		{	// return value held in *this
		return static_cast<_Ty *>(_Atomic_address::operator void *());
		}

	operator _Ty *() const 
		{	// return value held in *this
		return static_cast<_Ty *>(_Atomic_address::operator void *());
		}

	_Ty *exchange(_Ty *_Value,
		memory_order _Order = memory_order_seq_cst) volatile 
		{	// exchange value stored in *this with _Value
		return static_cast<_Ty *>(_Atomic_address::exchange(_Value, _Order));
		}

	_Ty *exchange(_Ty *_Value,
		memory_order _Order = memory_order_seq_cst) 
		{	// exchange value stored in *this with _Value
		return static_cast<_Ty *>(_Atomic_address::exchange(_Value, _Order));
		}

	bool compare_exchange_weak(
		_Ty *& _Exp, _Ty *_Value,
			memory_order _Order1,
			memory_order _Order2) volatile 
		{	// compare and exchange value stored in *this with  *_Exp, _Value
		void *_Exp0 = _Exp;
		void *_Value0 = _Value;
		bool _Result = _Atomic_address::compare_exchange_weak(
			_Exp0, _Value0, _Order1, _Order2);
		_Exp = static_cast<_Ty *>(_Exp0);
		return _Result;
		}

	bool compare_exchange_weak(
		_Ty *& _Exp, _Ty *_Value,
		memory_order _Order1,
		memory_order _Order2) 
		{	// compare and exchange value stored in *this with  *_Exp, _Value
		void *_Exp0 = _Exp;
		void *_Value0 = _Value;
		bool _Result = _Atomic_address::compare_exchange_weak(
			_Exp0, _Value0, _Order1, _Order2);
		_Exp = static_cast<_Ty *>(_Exp0);
		return _Result;
		}

	bool compare_exchange_weak(
		_Ty *& _Exp, _Ty *_Value,
		memory_order _Order = memory_order_seq_cst) volatile 
		{	// compare and exchange value stored in *this with  *_Exp, _Value
		void *_Exp0 = _Exp;
		void *_Value0 = _Value;
		bool _Result = _Atomic_address::compare_exchange_weak(
			_Exp0, _Value0, _Order);
		_Exp = static_cast<_Ty *>(_Exp0);
		return _Result;
		}

	bool compare_exchange_weak(
		_Ty *& _Exp, _Ty *_Value,
		memory_order _Order = memory_order_seq_cst) 
		{	// compare and exchange value stored in *this with  *_Exp, _Value
		void *_Exp0 = _Exp;
		void *_Value0 = _Value;
		bool _Result = _Atomic_address::compare_exchange_weak(
			_Exp0, _Value0, _Order);
		_Exp = static_cast<_Ty *>(_Exp0);
		return _Result;
		}

	bool compare_exchange_strong(
		_Ty *& _Exp, _Ty *_Value,
		memory_order _Order1, memory_order _Order2) volatile 
		{	// compare and exchange value stored in *this with  *_Exp, _Value
		void *_Exp0 = _Exp;
		void *_Value0 = _Value;
		bool _Result = _Atomic_address::compare_exchange_strong(
			_Exp0, _Value0, _Order1, _Order2);
		_Exp = static_cast<_Ty *>(_Exp0);
		return _Result;
		}

	bool compare_exchange_strong(
		_Ty *& _Exp, _Ty *_Value,
		memory_order _Order1, memory_order _Order2) 
		{	// compare and exchange value stored in *this with  *_Exp, _Value
		void *_Exp0 = _Exp;
		void *_Value0 = _Value;
		bool _Result = _Atomic_address::compare_exchange_strong(
			_Exp0, _Value0, _Order1, _Order2);
		_Exp = static_cast<_Ty *>(_Exp0);
		return _Result;
		}

	bool compare_exchange_strong(
		_Ty *& _Exp, _Ty *_Value,
		memory_order _Order = memory_order_seq_cst) volatile 
		{	// compare and exchange value stored in *this with  *_Exp, _Value
		void *_Exp0 = _Exp;
		void *_Value0 = _Value;
		bool _Result = _Atomic_address::compare_exchange_strong(
			_Exp0, _Value0, _Order);
		_Exp = static_cast<_Ty *>(_Exp0);
		return _Result;
		}

	bool compare_exchange_strong(
		_Ty *& _Exp, _Ty *_Value,
		memory_order _Order = memory_order_seq_cst) 
		{	// compare and exchange value stored in *this with  *_Exp, _Value
		void *_Exp0 = _Exp;
		void *_Value0 = _Value;
		bool _Result = _Atomic_address::compare_exchange_strong(
			_Exp0, _Value0, _Order);
		_Exp = static_cast<_Ty *>(_Exp0);
		return _Result;
		}

	_Ty *fetch_add(ptrdiff_t _Value,
		memory_order _Order = memory_order_seq_cst) volatile 
		{	// add _Value to value stored in *this
		return static_cast<_Ty *>(
			_Atomic_address::fetch_add(_Value * sizeof (_Ty), _Order));
		}

	_Ty *fetch_add(ptrdiff_t _Value,
		memory_order _Order = memory_order_seq_cst) 
		{	// add _Value to value stored in *this
		return static_cast<_Ty *>(
			_Atomic_address::fetch_add(_Value * sizeof (_Ty), _Order));
		}

	_Ty *fetch_sub(ptrdiff_t _Value,
		memory_order _Order = memory_order_seq_cst) volatile 
		{	// subtract _Value from value stored in *this
		return static_cast<_Ty *>(
			_Atomic_address::fetch_sub(_Value * sizeof (_Ty), _Order));
		}

	_Ty *fetch_sub(ptrdiff_t _Value,
		memory_order _Order = memory_order_seq_cst) 
		{	// subtract _Value from value stored in *this
		return static_cast<_Ty *>(
			_Atomic_address::fetch_sub(_Value * sizeof (_Ty), _Order));
		}

	_Ty *operator++(int) volatile 
		{	// increment stored pointer
		return fetch_add(1);
		}

	_Ty *operator++(int) 
		{	// increment stored pointer
		return fetch_add(1);
		}

	_Ty *operator--(int) volatile 
		{	// decrement stored pointer
		return fetch_sub(1);
		}

	_Ty *operator--(int) 
		{	// decrement stored pointer
		return fetch_sub(1);
		}

	_Ty *operator++() volatile 
		{	// increment stored pointer
		return fetch_add(1) + 1;
		}

	_Ty *operator++() 
		{	// increment stored pointer
		return fetch_add(1) + 1;
		}

	_Ty *operator--() volatile 
		{	// decrement stored pointer
		return fetch_sub(1) - 1;
		}

	_Ty *operator--() 
		{	// decrement stored pointer
		return fetch_sub(1) - 1;
		}

	_Ty *operator+=(ptrdiff_t _Right) volatile 
		{	// add _Right to value stored in *this
		return fetch_add(_Right) + _Right;
		}

	_Ty *operator+=(ptrdiff_t _Right) 
		{	// add _Right to value stored in *this
		return fetch_add(_Right) + _Right;
		}

	_Ty *operator-=(ptrdiff_t _Right) volatile 
		{	// subtract _Right from value stored in *this
		return fetch_sub(_Right) - _Right;
		}

	_Ty *operator-=(ptrdiff_t _Right) 
		{	// subtract _Right from value stored in *this
		return fetch_sub(_Right) - _Right;
		}
	};
 #endif /* _ATOMIC_IS_ADDRESS_TYPE */

		// CORE NON-MEMBER FUNCTIONS
	// TEMPLATE FUNCTION atomic_is_lock_free
inline bool atomic_is_lock_free(const volatile _ATOMIC_ITYPE *) 
	{	// return true if operations on _ATOMIC_ITYPE are lock-free
	return _ATOMIC_IS_LOCK_FREE();
	}

inline bool atomic_is_lock_free(const _ATOMIC_ITYPE *) 
	{	// return true if operations on _ATOMIC_ITYPE are lock-free
	return _ATOMIC_IS_LOCK_FREE();
	}

 #ifndef _ATOMIC_HAS_NO_SPECIALIZATION
inline bool atomic_is_lock_free(const volatile atomic<_ITYPE> *) 
	{	// return true if operations on _ATOMIC_ITYPE are lock-free
	return _ATOMIC_IS_LOCK_FREE();
	}

inline bool atomic_is_lock_free(const atomic<_ITYPE> *) 
	{	// return true if operations on _ATOMIC_ITYPE are lock-free
	return _ATOMIC_IS_LOCK_FREE();
	}
 #endif /* _ATOMIC_HAS_NO_SPECIALIZATION */

	// TEMPLATE FUNCTION atomic_init
inline void atomic_init(volatile _ATOMIC_ITYPE *_Atom,
	_ITYPE _Value) 
	{	// non-atomically initialize *_Atom to _Value
	_Atom->_My_val = (_ATOMIC_UINT)_Value;

 #if _ATOMIC_MAXBYTES_LOCK_FREE < _ISIZE
	_Atom->_My_flag = 0;
 #endif /* _ATOMIC_MAXBYTES_LOCK_FREE < _ISIZE */
	}

inline void atomic_init(_ATOMIC_ITYPE *_Atom,
	_ITYPE _Value) 
	{	// non-atomically initialize *_Atom to _Value
	_Atom->_My_val = (_ATOMIC_UINT)_Value;

 #if _ATOMIC_MAXBYTES_LOCK_FREE < _ISIZE
	_Atom->_My_flag = 0;
 #endif /* _ATOMIC_MAXBYTES_LOCK_FREE < _ISIZE */
	}

 #ifndef _ATOMIC_HAS_NO_SPECIALIZATION
inline void atomic_init(volatile atomic<_ITYPE> *_Atom,
	_ITYPE _Value) 
	{	// non-atomically initialize *_Atom to _Value
	_Atom->_My_val = (_ATOMIC_UINT)_Value;

 #if _ATOMIC_MAXBYTES_LOCK_FREE < _ISIZE
	_Atom->_My_flag = 0;
 #endif /* _ATOMIC_MAXBYTES_LOCK_FREE < _ISIZE */
	}

inline void atomic_init(atomic<_ITYPE> *_Atom,
	_ITYPE _Value) 
	{	// non-atomically initialize *_Atom to _Value
	_Atom->_My_val = (_ATOMIC_UINT)_Value;

 #if _ATOMIC_MAXBYTES_LOCK_FREE < _ISIZE
	_Atom->_My_flag = 0;
 #endif /* _ATOMIC_MAXBYTES_LOCK_FREE < _ISIZE */
	}
 #endif /* _ATOMIC_HAS_NO_SPECIALIZATION */

inline void atomic_store_explicit(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{	// store _Value into *_Atom
	assert(_ISIZE == sizeof (_ITYPE));
	_ATOMIC_STORE(_Atom, _Value, _Order);
	}

inline void atomic_store_explicit(
	_ATOMIC_ITYPE *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{	// store _Value into *_Atom
	assert(_ISIZE == sizeof (_ITYPE));
	_ATOMIC_STORE(_Atom, _Value, _Order);
	}

inline void atomic_store(volatile _ATOMIC_ITYPE *_Atom,
	_ITYPE _Value) 
	{	// store _Value into *_Atom
	atomic_store_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline void atomic_store(_ATOMIC_ITYPE *_Atom,
	_ITYPE _Value) 
	{	// store _Value into *_Atom
	atomic_store_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline _ITYPE atomic_load_explicit(
	const volatile _ATOMIC_ITYPE *_Atom, memory_order _Order) 
	{	// return value held in *_Atom
	return _ATOMIC_LOAD(_Atom, _Order);
	}

inline _ITYPE atomic_load_explicit(
	const _ATOMIC_ITYPE *_Atom, memory_order _Order) 
	{	// return value held in *_Atom
	return _ATOMIC_LOAD(_Atom, _Order);
	}

inline _ITYPE atomic_load(
	const volatile _ATOMIC_ITYPE *_Atom) 
	{	// return value stored in *_Atom
	return atomic_load_explicit(_Atom, memory_order_seq_cst);
	}

inline _ITYPE atomic_load(
	const _ATOMIC_ITYPE *_Atom) 
	{	// return value stored in *_Atom
	return atomic_load_explicit(_Atom, memory_order_seq_cst);
	}

inline _ITYPE atomic_exchange_explicit(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{	// exchange value stored in *_Atom with _Value
	return _ATOMIC_EXCHANGE(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_exchange_explicit(
	_ATOMIC_ITYPE *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{	// exchange value stored in *_Atom with _Value
	return _ATOMIC_EXCHANGE(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_exchange(volatile _ATOMIC_ITYPE *_Atom,
	_ITYPE _Value) 
	{	// exchange value stored in *_Atom with _Value
	return atomic_exchange_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline _ITYPE atomic_exchange(_ATOMIC_ITYPE *_Atom,
	_ITYPE _Value) 
	{	// exchange value stored in *_Atom with _Value
	return atomic_exchange_explicit(_Atom, _Value, memory_order_seq_cst);
	}

 #if _MS_64
inline bool atomic_compare_exchange_weak_explicit(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE *_Exp, _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) 
	{	// compare and exchange value stored in *_Atom with *_Exp, _Value
	assert(_Order2 != memory_order_release);
	assert(_Order2 != memory_order_acq_rel);
	assert(_Order2 <= _Order1);
	_ATOMIC_UINT _Temp = (_ATOMIC_UINT)*_Exp;
	bool res = _ATOMIC_COMPARE_EXCHANGE_WEAK(_Atom, &_Temp, _Value,
		_Order1, _Order2);
	*_Exp = (_ITYPE)_Temp;
	return res;
	}

inline bool atomic_compare_exchange_weak_explicit(
	_ATOMIC_ITYPE *_Atom, _ITYPE *_Exp, _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) 
	{	// compare and exchange value stored in *_Atom with *_Exp, _Value
	assert(_Order2 != memory_order_release);
	assert(_Order2 != memory_order_acq_rel);
	assert(_Order2 <= _Order1);
	_ATOMIC_UINT _Temp = (_ATOMIC_UINT)*_Exp;
	bool res = _ATOMIC_COMPARE_EXCHANGE_WEAK(_Atom, &_Temp, _Value,
		_Order1, _Order2);
	*_Exp = (_ITYPE)_Temp;
	return res;
	}

 #else /* _MS_64 */
inline bool atomic_compare_exchange_weak_explicit(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE *_Exp, _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) 
	{	// compare and exchange value stored in *_Atom with *_Exp, _Value
	assert(_Order2 != memory_order_release);
	assert(_Order2 != memory_order_acq_rel);
	assert(_Order2 <= _Order1);
	return _ATOMIC_COMPARE_EXCHANGE_WEAK(_Atom, _Exp, _Value,
		_Order1, _Order2);
	}

inline bool atomic_compare_exchange_weak_explicit(
	_ATOMIC_ITYPE *_Atom, _ITYPE *_Exp, _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) 
	{	// compare and exchange value stored in *_Atom with *_Exp, _Value
	assert(_Order2 != memory_order_release);
	assert(_Order2 != memory_order_acq_rel);
	assert(_Order2 <= _Order1);
	return _ATOMIC_COMPARE_EXCHANGE_WEAK(_Atom, _Exp, _Value,
		_Order1, _Order2);
	}
 #endif /* _MS_64 */

inline bool atomic_compare_exchange_weak(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE *_Exp,
		_ITYPE _Value) 
	{	// compare and exchange value stored in *_Atom with *_Exp, _Value
	return atomic_compare_exchange_weak_explicit(
		_Atom, _Exp, _Value, memory_order_seq_cst, memory_order_seq_cst);
	}

inline bool atomic_compare_exchange_weak(
	_ATOMIC_ITYPE *_Atom, _ITYPE *_Exp,
		_ITYPE _Value) 
	{	// compare and exchange value stored in *_Atom with *_Exp, _Value
	return atomic_compare_exchange_weak_explicit(
		_Atom, _Exp, _Value, memory_order_seq_cst, memory_order_seq_cst);
	}

 #if _MS_64
inline bool atomic_compare_exchange_strong_explicit(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE *_Exp, _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) 
	{	// compare and exchange value stored in *_Atom with *_Exp, _Value
	assert(_Order2 != memory_order_release);
	assert(_Order2 != memory_order_acq_rel);
	assert(_Order2 <= _Order1);
	_ATOMIC_UINT _Temp = (_ATOMIC_UINT)*_Exp;
	bool res = _ATOMIC_COMPARE_EXCHANGE_STRONG(_Atom, &_Temp, _Value,
		_Order1, _Order2);
	*_Exp = (_ITYPE)_Temp;
	return res;
	}

inline bool atomic_compare_exchange_strong_explicit(
	_ATOMIC_ITYPE *_Atom, _ITYPE *_Exp, _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) 
	{	// compare and exchange value stored in *_Atom with *_Exp, _Value
	assert(_Order2 != memory_order_release);
	assert(_Order2 != memory_order_acq_rel);
	assert(_Order2 <= _Order1);
	_ATOMIC_UINT _Temp = (_ATOMIC_UINT)*_Exp;
	bool res = _ATOMIC_COMPARE_EXCHANGE_STRONG(_Atom, &_Temp, _Value,
		_Order1, _Order2);
	*_Exp = (_ITYPE)_Temp;
	return res;
	}

 #else /* _MS_64 */
inline bool atomic_compare_exchange_strong_explicit(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE *_Exp, _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) 
	{	// compare and exchange value stored in *_Atom with *_Exp, _Value
	assert(_Order2 != memory_order_release);
	assert(_Order2 != memory_order_acq_rel);
	assert(_Order2 <= _Order1);
	return _ATOMIC_COMPARE_EXCHANGE_STRONG(_Atom, _Exp, _Value,
		_Order1, _Order2);
	}

inline bool atomic_compare_exchange_strong_explicit(
	_ATOMIC_ITYPE *_Atom, _ITYPE *_Exp, _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) 
	{	// compare and exchange value stored in *_Atom with *_Exp, _Value
	assert(_Order2 != memory_order_release);
	assert(_Order2 != memory_order_acq_rel);
	assert(_Order2 <= _Order1);
	return _ATOMIC_COMPARE_EXCHANGE_STRONG(_Atom, _Exp, _Value,
		_Order1, _Order2);
	}
 #endif /* _MS_64 */

inline bool atomic_compare_exchange_strong(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE *_Exp, _ITYPE _Value) 
	{	// compare and exchange value stored in *_Atom with *_Exp, _Value
	return atomic_compare_exchange_strong_explicit(
		_Atom, _Exp, _Value, memory_order_seq_cst, memory_order_seq_cst);
	}

inline bool atomic_compare_exchange_strong(
	_ATOMIC_ITYPE *_Atom, _ITYPE *_Exp, _ITYPE _Value) 
	{	// compare and exchange value stored in *_Atom with *_Exp, _Value
	return atomic_compare_exchange_strong_explicit(
		_Atom, _Exp, _Value, memory_order_seq_cst, memory_order_seq_cst);
	}

		// CORE MEMBER FUNCTIONS
inline _ITYPE _ATOMIC_ITYPE::operator=(_ITYPE _Value) volatile 
	{	// assign _Value to *this
	atomic_store(this, _Value);
	return _Value;
	}

inline _ITYPE _ATOMIC_ITYPE::operator=(_ITYPE _Value) 
	{	// assign _Value to *this
	atomic_store(this, _Value);
	return _Value;
	}

inline bool _ATOMIC_ITYPE::is_lock_free() const volatile 
	{	// return true if operations on *this are lock-free
	return atomic_is_lock_free(this);
	}

inline bool _ATOMIC_ITYPE::is_lock_free() const 
	{	// return true if operations on *this are lock-free
	return atomic_is_lock_free(this);
	}

inline void _ATOMIC_ITYPE::store(_ITYPE _Value,
	memory_order _Order) volatile 
	{	// store _Value into *this
	atomic_store_explicit(this, _Value, _Order);
	}

inline void _ATOMIC_ITYPE::store(_ITYPE _Value,
	memory_order _Order) 
	{	// store _Value into *this
	atomic_store_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::load(
	memory_order _Order) const volatile 
	{	// return value held in *this
	return atomic_load_explicit(this, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::load(
	memory_order _Order) const 
	{	// return value held in *this
	return atomic_load_explicit(this, _Order);
	}

inline _ATOMIC_ITYPE::operator _ITYPE() const volatile 
	{	// return value held in *this
	return atomic_load(this);
	}

inline _ATOMIC_ITYPE::operator _ITYPE() const 
	{	// return value held in *this
	return atomic_load(this);
	}

inline _ITYPE _ATOMIC_ITYPE::exchange(
	_ITYPE _Value, memory_order _Order) volatile 
	{	// exchange value stored in *this with _Value
	return atomic_exchange_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::exchange(
	_ITYPE _Value, memory_order _Order) 
	{	// exchange value stored in *this with _Value
	return atomic_exchange_explicit(this, _Value, _Order);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_weak(
	_ITYPE& _Exp, _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) volatile 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return atomic_compare_exchange_weak_explicit(
		this, &_Exp, _Value, _Order1, _Order2);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_weak(
	_ITYPE& _Exp, _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return atomic_compare_exchange_weak_explicit(
		this, &_Exp, _Value, _Order1, _Order2);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_weak(
	_ITYPE& _Exp, _ITYPE _Value,
	memory_order _Order) volatile 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return atomic_compare_exchange_weak_explicit(
		this, &_Exp, _Value, _Order, _Order);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_weak(
	_ITYPE& _Exp, _ITYPE _Value,
	memory_order _Order) 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return atomic_compare_exchange_weak_explicit(
		this, &_Exp, _Value, _Order, _Order);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_strong(
	_ITYPE& _Exp, _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) volatile 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return atomic_compare_exchange_strong_explicit(
		this, &_Exp, _Value, _Order1, _Order2);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_strong(
	_ITYPE& _Exp, _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return atomic_compare_exchange_strong_explicit(
		this, &_Exp, _Value, _Order1, _Order2);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_strong(
	_ITYPE& _Exp, _ITYPE _Value,
	memory_order _Order) volatile 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return atomic_compare_exchange_strong_explicit(
		this, &_Exp, _Value, _Order, _Order);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_strong(
	_ITYPE& _Exp, _ITYPE _Value,
	memory_order _Order) 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return atomic_compare_exchange_strong_explicit(
		this, &_Exp, _Value, _Order, _Order);
	}

		// SPECIALIZATIONS OF GENERAL OPERATIONS ON ATOMIC TYPES
 #ifndef _ATOMIC_HAS_NO_SPECIALIZATION
template<>
	inline bool atomic_is_lock_free(
		const volatile atomic<_ITYPE> *_Atom) 
	{
	return atomic_is_lock_free(
		static_cast<const volatile _ATOMIC_ITYPE *>(_Atom));
	}

template<>
	inline bool atomic_is_lock_free(
		const atomic<_ITYPE> *_Atom) 
	{
	return atomic_is_lock_free(static_cast<const _ATOMIC_ITYPE *>(_Atom));
	}

template<>
	inline void atomic_init(volatile atomic<_ITYPE> *_Atom,
		_ITYPE _Value) 
	{
	atomic_init(static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline void atomic_init(atomic<_ITYPE> *_Atom,
		_ITYPE _Value) 
	{
	atomic_init(static_cast<_ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline void atomic_store(volatile atomic<_ITYPE> *_Atom,
		_ITYPE _Value) 
	{
	atomic_store(static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline void atomic_store(atomic<_ITYPE> *_Atom,
		_ITYPE _Value) 
	{
	atomic_store(static_cast<_ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline void atomic_store_explicit(volatile atomic<_ITYPE> *_Atom,
		_ITYPE _Value, memory_order _Order) 
	{
	atomic_store_explicit(static_cast<volatile _ATOMIC_ITYPE *>(_Atom),
		_Value, _Order);
	}

template<>
	inline void atomic_store_explicit(atomic<_ITYPE> *_Atom,
		_ITYPE _Value, memory_order _Order) 
	{
	atomic_store_explicit(static_cast<_ATOMIC_ITYPE *>(_Atom),
		_Value, _Order);
	}

template<>
	inline _ITYPE atomic_load(const volatile atomic<_ITYPE> *_Atom) 
	{
	return atomic_load(static_cast<const volatile _ATOMIC_ITYPE *>(_Atom));
	}

template<>
	inline _ITYPE atomic_load(const atomic<_ITYPE> *_Atom) 
	{
	return atomic_load(static_cast<const volatile _ATOMIC_ITYPE *>(_Atom));
	}

template<>
	inline _ITYPE atomic_load_explicit(
		const volatile atomic<_ITYPE> *_Atom, memory_order _Order) 
	{
	return atomic_load_explicit(
		static_cast<const volatile _ATOMIC_ITYPE *>(_Atom), _Order);
	}

template<>
	inline _ITYPE atomic_load_explicit(
		const atomic<_ITYPE> *_Atom, memory_order _Order) 
	{
	return atomic_load_explicit(
		static_cast<const _ATOMIC_ITYPE *>(_Atom), _Order);
	}

template<>
	inline _ITYPE atomic_exchange(
		volatile atomic<_ITYPE> *_Atom, _ITYPE _Value) 
	{
	return atomic_exchange(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline _ITYPE atomic_exchange(atomic<_ITYPE> *_Atom,
		_ITYPE _Value) 
	{
	return atomic_exchange(static_cast<_ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline _ITYPE atomic_exchange_explicit(
		volatile atomic<_ITYPE> *_Atom,
			_ITYPE _Value, memory_order _Order) 
	{
	return atomic_exchange_explicit(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Value, _Order);
	}

template<>
	inline _ITYPE atomic_exchange_explicit(atomic<_ITYPE> *_Atom,
		_ITYPE _Value, memory_order _Order) 
	{
	return atomic_exchange_explicit(static_cast<_ATOMIC_ITYPE *>(_Atom),
		_Value, _Order);
	}

template<>
	inline bool atomic_compare_exchange_weak(
		volatile atomic<_ITYPE> *_Atom, _ITYPE *_Exp,
			_ITYPE _Value) 
	{
	return atomic_compare_exchange_weak(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Exp, _Value);
	}

template<>
	inline bool atomic_compare_exchange_weak(
		atomic<_ITYPE> *_Atom, _ITYPE *_Exp,
			_ITYPE _Value) 
	{
	return atomic_compare_exchange_weak(
		static_cast<_ATOMIC_ITYPE *>(_Atom), _Exp, _Value);
	}

template<>
	inline bool atomic_compare_exchange_weak_explicit(
		volatile atomic<_ITYPE> *_Atom, _ITYPE *_Exp, _ITYPE _Value,
		memory_order _Order1, memory_order _Order2) 
	{
	return atomic_compare_exchange_weak_explicit(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Exp,
			_Value, _Order1, _Order2);
	}

template<>
	inline bool atomic_compare_exchange_weak_explicit(
		atomic<_ITYPE> *_Atom, _ITYPE *_Exp, _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) 
	{
	return atomic_compare_exchange_weak_explicit(
		static_cast<_ATOMIC_ITYPE *>(_Atom), _Exp, _Value, _Order1, _Order2);
	}

template<>
	inline bool atomic_compare_exchange_strong(
		volatile atomic<_ITYPE> *_Atom, _ITYPE *_Exp,
			_ITYPE _Value) 
	{
	return atomic_compare_exchange_strong(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Exp, _Value);
	}

template<>
	inline bool atomic_compare_exchange_strong(
		atomic<_ITYPE> *_Atom, _ITYPE *_Exp,
			_ITYPE _Value) 
	{
	return atomic_compare_exchange_strong(
		static_cast<_ATOMIC_ITYPE *>(_Atom), _Exp, _Value);
	}

template<>
	inline bool atomic_compare_exchange_strong_explicit(
		volatile atomic<_ITYPE> *_Atom, _ITYPE *_Exp, _ITYPE _Value,
		memory_order _Order1, memory_order _Order2) 
	{
	return atomic_compare_exchange_strong_explicit(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Exp,
			_Value, _Order1, _Order2);
	}

template<>
	inline bool atomic_compare_exchange_strong_explicit(
		atomic<_ITYPE> *_Atom, _ITYPE *_Exp, _ITYPE _Value,
		memory_order _Order1, memory_order _Order2) 
	{
	return atomic_compare_exchange_strong_explicit(
		static_cast<_ATOMIC_ITYPE *>(_Atom), _Exp, _Value, _Order1, _Order2);
	}
 #endif /* _ATOMIC_HAS_NO_SPECIALIZATION */

 #ifdef _ATOMIC_HAS_ARITHMETIC_OPS
		// ARITHMETIC NON-MEMBER FUNCTIONS
inline _ITYPE atomic_fetch_add_explicit(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{	// add _Value to value stored in *_Atom
	return _ATOMIC_FETCH_ADD(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_fetch_add_explicit(
	_ATOMIC_ITYPE *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{	// add _Value to value stored in *_Atom
	return _ATOMIC_FETCH_ADD(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_fetch_add(volatile _ATOMIC_ITYPE *_Atom,
		_ITYPE _Value) 
	{	// add _Value to value stored in *_Atom
	return atomic_fetch_add_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline _ITYPE atomic_fetch_add(_ATOMIC_ITYPE *_Atom,
	_ITYPE _Value) 
	{	// add _Value to value stored in *_Atom
	return atomic_fetch_add_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline _ITYPE atomic_fetch_sub_explicit(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{	// subtract _Value from value stored in *_Atom
	return _ATOMIC_FETCH_SUB(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_fetch_sub_explicit(
	_ATOMIC_ITYPE *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{	// subtract _Value from value stored in *_Atom
	return _ATOMIC_FETCH_SUB(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_fetch_sub(volatile _ATOMIC_ITYPE *_Atom,
	_ITYPE _Value) 
	{	// subtract _Value from value stored in *_Atom
	return atomic_fetch_sub_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline _ITYPE atomic_fetch_sub(_ATOMIC_ITYPE *_Atom,
	_ITYPE _Value) 
	{	// subtract _Value from value stored in *_Atom
	return atomic_fetch_sub_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline _ITYPE atomic_fetch_and_explicit(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{	// and _Value with value stored in *_Atom
	return _ATOMIC_FETCH_AND(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_fetch_and_explicit(
	_ATOMIC_ITYPE *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{	// and _Value with value stored in *_Atom
	return _ATOMIC_FETCH_AND(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_fetch_and(volatile _ATOMIC_ITYPE *_Atom,
	_ITYPE _Value) 
	{	// and _Value with value stored in *_Atom
	return atomic_fetch_and_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline _ITYPE atomic_fetch_and(_ATOMIC_ITYPE *_Atom,
	_ITYPE _Value) 
	{	// and _Value with value stored in *_Atom
	return atomic_fetch_and_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline _ITYPE atomic_fetch_or_explicit(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{	// or _Value with value stored in *_Atom
	return _ATOMIC_FETCH_OR(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_fetch_or_explicit(
	_ATOMIC_ITYPE *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{	// or _Value with value stored in *_Atom
	return _ATOMIC_FETCH_OR(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_fetch_or(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE _Value) 
	{	// or _Value with value stored in *_Atom
	return atomic_fetch_or_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline _ITYPE atomic_fetch_or(
	_ATOMIC_ITYPE *_Atom, _ITYPE _Value) 
	{	// or _Value with value stored in *_Atom
	return atomic_fetch_or_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline _ITYPE atomic_fetch_xor_explicit(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{	// xor _Value with value stored in *_Atom
	return _ATOMIC_FETCH_XOR(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_fetch_xor_explicit(
	_ATOMIC_ITYPE *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{	// xor _Value with value stored in *_Atom
	return _ATOMIC_FETCH_XOR(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_fetch_xor(
	volatile _ATOMIC_ITYPE *_Atom, _ITYPE _Value) 
	{	// xor _Value with value stored in *_Atom
	return atomic_fetch_xor_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline _ITYPE atomic_fetch_xor(
	_ATOMIC_ITYPE *_Atom, _ITYPE _Value) 
	{	// xor _Value with value stored in *_Atom
	return atomic_fetch_xor_explicit(_Atom, _Value, memory_order_seq_cst);
	}

		// ARITHMETIC MEMBER FUNCTIONS
inline _ITYPE _ATOMIC_ITYPE::operator++(int) volatile 
	{	// increment value stored in *this
	return atomic_fetch_add(this, 1);
	}

inline _ITYPE _ATOMIC_ITYPE::operator++(int) 
	{	// increment value stored in *this
	return atomic_fetch_add(this, 1);
	}

inline _ITYPE _ATOMIC_ITYPE::operator--(int) volatile 
	{	// decrement value stored in *this
	return atomic_fetch_sub(this, 1);
	}

inline _ITYPE _ATOMIC_ITYPE::operator--(int) 
	{	// decrement value stored in *this
	return atomic_fetch_sub(this, 1);
	}

inline _ITYPE _ATOMIC_ITYPE::operator++() volatile 
	{	// increment value stored in *this
	return atomic_fetch_add(this, 1) + 1;
	}

inline _ITYPE _ATOMIC_ITYPE::operator++() 
	{	// increment value stored in *this
	return atomic_fetch_add(this, 1) + 1;
	}

inline _ITYPE _ATOMIC_ITYPE::operator--() volatile 
	{	// decrement value stored in *this
	return atomic_fetch_sub(this, 1) - 1;
	}

inline _ITYPE _ATOMIC_ITYPE::operator--() 
	{	// decrement value stored in *this
	return atomic_fetch_sub(this, 1) - 1;
	}

inline _ITYPE _ATOMIC_ITYPE::operator+=(_ITYPE _Right) volatile 
	{	// add _Right to value stored in *this
	return atomic_fetch_add(this, _Right) + _Right;
	}

inline _ITYPE _ATOMIC_ITYPE::operator+=(_ITYPE _Right) 
	{	// add _Right to value stored in *this
	return atomic_fetch_add(this, _Right) + _Right;
	}

inline _ITYPE _ATOMIC_ITYPE::operator-=(_ITYPE _Right) volatile 
	{	// subtract _Right from value stored in *this
	return atomic_fetch_sub(this, _Right) - _Right;
	}

inline _ITYPE _ATOMIC_ITYPE::operator-=(_ITYPE _Right) 
	{	// subtract _Right from value stored in *this
	return atomic_fetch_sub(this, _Right) - _Right;
	}

inline _ITYPE _ATOMIC_ITYPE::operator&=(_ITYPE _Right) volatile 
	{	// and _Right with value stored in *this
	return atomic_fetch_and(this, _Right) & _Right;
	}

inline _ITYPE _ATOMIC_ITYPE::operator&=(_ITYPE _Right) 
	{	// and _Right with value stored in *this
	return atomic_fetch_and(this, _Right) & _Right;
	}

inline _ITYPE _ATOMIC_ITYPE::operator|=(_ITYPE _Right) volatile 
	{	// or _Right with value stored in *this
	return atomic_fetch_or(this, _Right) | _Right;
	}

inline _ITYPE _ATOMIC_ITYPE::operator|=(_ITYPE _Right) 
	{	// or _Right with value stored in *this
	return atomic_fetch_or(this, _Right) | _Right;
	}

inline _ITYPE _ATOMIC_ITYPE::operator^=(_ITYPE _Right) volatile 
	{	// xor _Right with value stored in *this
	return atomic_fetch_xor(this, _Right) ^ _Right;
	}

inline _ITYPE _ATOMIC_ITYPE::operator^=(_ITYPE _Right) 
	{	// xor _Right with value stored in *this
	return atomic_fetch_xor(this, _Right) ^ _Right;
	}

inline _ITYPE _ATOMIC_ITYPE::fetch_add(
	_ITYPE _Value, memory_order _Order) volatile 
	{	// add _Value to value stored in *this
	return atomic_fetch_add_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::fetch_add(
	_ITYPE _Value, memory_order _Order) 
	{	// add _Value to value stored in *this
	return atomic_fetch_add_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::fetch_sub(
	_ITYPE _Value, memory_order _Order) volatile 
	{	// subtract _Value from value stored in *this
	return atomic_fetch_sub_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::fetch_sub(
	_ITYPE _Value, memory_order _Order) 
	{	// subtract _Value from value stored in *this
	return atomic_fetch_sub_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::fetch_and(
	_ITYPE _Value, memory_order _Order) volatile 
	{	// and _Value with value stored in *this
	return atomic_fetch_and_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::fetch_and(
	_ITYPE _Value, memory_order _Order) 
	{	// and _Value with value stored in *this
	return atomic_fetch_and_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::fetch_or(
	_ITYPE _Value, memory_order _Order) volatile 
	{	// or _Value with value stored in *this
	return atomic_fetch_or_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::fetch_or(
	_ITYPE _Value, memory_order _Order) 
	{	// or _Value with value stored in *this
	return atomic_fetch_or_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::fetch_xor(
	_ITYPE _Value, memory_order _Order) volatile 
	{	// xor _Value with value stored in *this
	return atomic_fetch_xor_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::fetch_xor(
	_ITYPE _Value, memory_order _Order) 
	{	// xor _Value with value stored in *this
	return atomic_fetch_xor_explicit(this, _Value, _Order);
	}

	// SPECIALIZATIONS OF ARITHMETIC OPERATIONS ON ATOMIC TYPES
 #ifndef _ATOMIC_HAS_NO_SPECIALIZATION
template<>
	inline _ITYPE atomic_fetch_add(
		volatile atomic<_ITYPE> *_Atom,
			_ITYPE _Value) 
	{
	return atomic_fetch_add(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline _ITYPE atomic_fetch_add(atomic<_ITYPE> *_Atom,
		_ITYPE _Value) 
	{
	return atomic_fetch_add(static_cast<_ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline _ITYPE atomic_fetch_add_explicit(
		volatile atomic<_ITYPE> *_Atom, _ITYPE _Value,
			memory_order _Order) 
	{
	return atomic_fetch_add_explicit(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Value, _Order);
	}

template<>
	inline _ITYPE atomic_fetch_add_explicit(
	atomic<_ITYPE> *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{
	return atomic_fetch_add_explicit(
		static_cast<_ATOMIC_ITYPE *>(_Atom), _Value, _Order);
	}

template<>
	inline _ITYPE atomic_fetch_sub(
		volatile atomic<_ITYPE> *_Atom,
			_ITYPE _Value) 
	{
	return atomic_fetch_sub(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline _ITYPE atomic_fetch_sub(atomic<_ITYPE> *_Atom,
		_ITYPE _Value) 
	{
	return atomic_fetch_sub(static_cast<_ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline _ITYPE atomic_fetch_sub_explicit(
	volatile atomic<_ITYPE> *_Atom, _ITYPE _Value,
		memory_order _Order) 
	{
	return atomic_fetch_sub_explicit(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Value, _Order);
	}

template<>
	inline _ITYPE atomic_fetch_sub_explicit(
		atomic<_ITYPE> *_Atom, _ITYPE _Value,
			memory_order _Order) 
	{
	return atomic_fetch_sub_explicit(
		static_cast<_ATOMIC_ITYPE *>(_Atom), _Value, _Order);
	}

template<>
	inline _ITYPE atomic_fetch_and(
		volatile atomic<_ITYPE> *_Atom, _ITYPE _Value) 
	{
	return atomic_fetch_and(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline _ITYPE atomic_fetch_and(
		atomic<_ITYPE> *_Atom, _ITYPE _Value) 
	{
	return atomic_fetch_and(static_cast<_ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline _ITYPE atomic_fetch_and_explicit(
		volatile atomic<_ITYPE> *_Atom, _ITYPE _Value,
			memory_order _Order) 
	{
	return atomic_fetch_and_explicit(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Value, _Order);
	}

template<>
	inline _ITYPE atomic_fetch_and_explicit(
		atomic<_ITYPE> *_Atom, _ITYPE _Value,
			memory_order _Order) 
	{
	return atomic_fetch_and_explicit(
		static_cast<_ATOMIC_ITYPE *>(_Atom), _Value, _Order);
	}

template<>
	inline _ITYPE atomic_fetch_or(volatile atomic<_ITYPE> *_Atom,
		_ITYPE _Value) 
	{
	return atomic_fetch_or(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline _ITYPE atomic_fetch_or(atomic<_ITYPE> *_Atom,
		_ITYPE _Value) 
	{
	return atomic_fetch_or(static_cast<_ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline _ITYPE atomic_fetch_or_explicit(
		volatile atomic<_ITYPE> *_Atom, _ITYPE _Value,
			memory_order _Order) 
	{
	return atomic_fetch_or_explicit(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Value, _Order);
	}

template<>
	inline _ITYPE atomic_fetch_or_explicit(
		atomic<_ITYPE> *_Atom, _ITYPE _Value,
			memory_order _Order) 
	{
	return atomic_fetch_or_explicit(
		static_cast<_ATOMIC_ITYPE *>(_Atom), _Value, _Order);
	}

template<>
	inline _ITYPE atomic_fetch_xor(volatile atomic<_ITYPE> *_Atom,
		_ITYPE _Value) 
	{
	return atomic_fetch_xor(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline _ITYPE atomic_fetch_xor(atomic<_ITYPE> *_Atom,
		_ITYPE _Value) 
	{
	return atomic_fetch_xor(static_cast<_ATOMIC_ITYPE *>(_Atom), _Value);
	}

template<>
	inline _ITYPE atomic_fetch_xor_explicit(
		volatile atomic<_ITYPE> *_Atom, _ITYPE _Value,
			memory_order _Order) 
	{
	return atomic_fetch_xor_explicit(
		static_cast<volatile _ATOMIC_ITYPE *>(_Atom), _Value, _Order);
	}

template<>
	inline _ITYPE atomic_fetch_xor_explicit(
		atomic<_ITYPE> *_Atom, _ITYPE _Value,
			memory_order _Order) 
	{
	return atomic_fetch_xor_explicit(
		static_cast<_ATOMIC_ITYPE *>(_Atom), _Value, _Order);
	}
 #endif /* _ATOMIC_HAS_NO_SPECIALIZATION */
 #endif /* _ATOMIC_HAS_ARITHMETIC_OPS */

 #ifdef _ATOMIC_IS_ADDRESS_TYPE
		// ADDRESS NON-MEMBER FUNCTIONS
inline _ITYPE atomic_fetch_add_explicit(
	volatile _ATOMIC_ITYPE *_Atom, ptrdiff_t _Value,
		memory_order _Order) 
	{	// add _Value to value stored in *_Atom
	return (_ITYPE)_ATOMIC_FETCH_ADD(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_fetch_add_explicit(
	_ATOMIC_ITYPE *_Atom, ptrdiff_t _Value,
		memory_order _Order) 
	{	// add _Value to value stored in *_Atom
	return (_ITYPE)_ATOMIC_FETCH_ADD(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_fetch_add(
	volatile _ATOMIC_ITYPE *_Atom, ptrdiff_t _Value) 
	{	// add _Value to value stored in *_Atom
	return atomic_fetch_add_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline _ITYPE atomic_fetch_add(
	_ATOMIC_ITYPE *_Atom, ptrdiff_t _Value) 
	{	// add _Value to value stored in *_Atom
	return atomic_fetch_add_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline _ITYPE atomic_fetch_sub_explicit(
	volatile _ATOMIC_ITYPE *_Atom, ptrdiff_t _Value,
		memory_order _Order) 
	{	// subtract _Value from value stored in *_Atom
	return (_ITYPE)_ATOMIC_FETCH_SUB(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_fetch_sub_explicit(
	_ATOMIC_ITYPE *_Atom, ptrdiff_t _Value,
		memory_order _Order) 
	{	// subtract _Value from value stored in *_Atom
	return (_ITYPE)_ATOMIC_FETCH_SUB(_Atom, _Value, _Order);
	}

inline _ITYPE atomic_fetch_sub(
	volatile _ATOMIC_ITYPE *_Atom, ptrdiff_t _Value) 
	{	// subtract _Value from value stored in *_Atom
	return atomic_fetch_sub_explicit(_Atom, _Value, memory_order_seq_cst);
	}

inline _ITYPE atomic_fetch_sub(
	_ATOMIC_ITYPE *_Atom, ptrdiff_t _Value) 
	{	// subtract _Value from value stored in *_Atom
	return atomic_fetch_sub_explicit(_Atom, _Value, memory_order_seq_cst);
	}

		// ADDRESS MEMBER FUNCTIONS
inline bool _ATOMIC_ITYPE::compare_exchange_weak(
	const _ITYPE& _Exp, const _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) volatile 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return _ATOMIC_COMPARE_EXCHANGE_WEAK(
		this, &_Exp, _Value, _Order1, _Order2);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_weak(
	const _ITYPE& _Exp, const _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return _ATOMIC_COMPARE_EXCHANGE_WEAK(
		this, &_Exp, _Value, _Order1, _Order2);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_weak(
	const _ITYPE& _Exp, const _ITYPE _Value,
		memory_order _Order) volatile 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return compare_exchange_weak(_Exp, _Value, _Order, _Order);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_weak(
	const _ITYPE& _Exp, const _ITYPE _Value,
		memory_order _Order) 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return compare_exchange_weak(_Exp, _Value, _Order, _Order);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_strong(
	const _ITYPE& _Exp, const _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) volatile 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return _ATOMIC_COMPARE_EXCHANGE_STRONG(
		this, &_Exp, _Value, _Order1, _Order2);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_strong(
	const _ITYPE& _Exp, const _ITYPE _Value,
	memory_order _Order1, memory_order _Order2) 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return _ATOMIC_COMPARE_EXCHANGE_STRONG(
		this, &_Exp, _Value, _Order1, _Order2);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_strong(
	const _ITYPE& _Exp, const _ITYPE _Value,
		memory_order _Order) volatile 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return compare_exchange_strong(_Exp, _Value, _Order, _Order);
	}

inline bool _ATOMIC_ITYPE::compare_exchange_strong(
	const _ITYPE& _Exp, const _ITYPE _Value,
		memory_order _Order) 
	{	// compare and exchange value stored in *this with *_Exp, _Value
	return compare_exchange_strong(_Exp, _Value, _Order, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::fetch_add(
	ptrdiff_t _Value, memory_order _Order) volatile 
	{	// add _Value to value stored in *this
	return atomic_fetch_add_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::fetch_add(
	ptrdiff_t _Value, memory_order _Order) 
	{	// add _Value to value stored in *this
	return atomic_fetch_add_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::fetch_sub(
	ptrdiff_t _Value, memory_order _Order) volatile 
	{	// subtract _Value from value stored in *this
	return atomic_fetch_sub_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::fetch_sub(
	ptrdiff_t _Value, memory_order _Order) 
	{	// subtract _Value from value stored in *this
	return atomic_fetch_sub_explicit(this, _Value, _Order);
	}

inline _ITYPE _ATOMIC_ITYPE::operator+=(ptrdiff_t _Right) volatile 
	{	// add _Right to value stored in *this
	return (char*)fetch_add(_Right) + _Right;
	}

inline _ITYPE _ATOMIC_ITYPE::operator+=(ptrdiff_t _Right) 
	{	// add _Right to value stored in *this
	return (char*)fetch_add(_Right) + _Right;
	}

inline _ITYPE _ATOMIC_ITYPE::operator-=(ptrdiff_t _Right) volatile 
	{	// subtract _Right from value stored in *this
	return (char*)fetch_sub(_Right) - _Right;
	}

inline _ITYPE _ATOMIC_ITYPE::operator-=(ptrdiff_t _Right) 
	{	// subtract _Right from value stored in *this
	return (char*)fetch_sub(_Right) - _Right;
	}
 #endif /* _ATOMIC_IS_ADDRESS_TYPE */

#undef _ATOMIC_FETCH_XOR
#undef _ATOMIC_FETCH_OR
#undef _ATOMIC_FETCH_AND
#undef _ATOMIC_FETCH_SUB
#undef _ATOMIC_FETCH_ADD
#undef _ATOMIC_COMPARE_EXCHANGE_STRONG
#undef _ATOMIC_COMPARE_EXCHANGE_WEAK
#undef _ATOMIC_EXCHANGE
#undef _ATOMIC_LOAD
#undef _ATOMIC_STORE
#undef _ATOMIC_IS_LOCK_FREE
#undef _ATOMIC_LOCKING_PRIMITIVE_NAME
#undef _ATOMIC_LOCK_FREE_PRIMITIVE_NAME
#undef _ATOMIC_UINT

#undef _ATOMIC_IS_ADDRESS_TYPE
#undef _ATOMIC_HAS_NO_SPECIALIZATION
#undef _ATOMIC_HAS_ARITHMETIC_OPS
#undef _ISIZE
#undef _ITYPE
#undef _ATOMIC_ITYPE

/*
 * Copyright (c) 1992-2011 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V5.40:0009 */
