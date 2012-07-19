#ifndef __STATIC_SELECT_HPP
#define __STATIC_SELECT_HPP


namespace utility
{
	/************************************************************************
	*  
	*	Int2Type		: 常整数映射为类型，由编译期计算出来的结果选择不同的函数，达到静态分派
	*	调用方式			: Int2Type<isPolymorphics>
	*	适用情形			: 有需要根据某个编译期常数调用一个或数个不同的函数
	*					  有必要在编译期实施静态分派
	*
	*	Type2Type		: 利用函数重载机制，模拟template偏特化，利用轻量型型别来传递型别信息
	*
	***********************************************************************/
	template< int v >
	struct Int2Type
	{
		enum { value = v };
	};

	template< typename T >
	struct Type2Type
	{
		typedef T value_type;
	};
}





#endif