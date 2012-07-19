#ifndef __SCOPE_EXIT_HPP
#define __SCOPE_EXIT_HPP


#include <functional>
#include <memory>


namespace utility
{
	
	class scope_exit_base
	{
	public:
		virtual ~scope_exit_base()
		{}
	};
	typedef std::shared_ptr<scope_exit_base> scope_exit_base_ptr;


	template <typename InitFuncT, typename UninitFuncT>
	class scope_exit_t
		: public scope_exit_base
	{
		typedef InitFuncT		InitFuncionType;
		typedef UninitFuncT		UninitFunctionType;

	private:
		InitFuncionType			initFunc_;		// 初始化
		UninitFunctionType		unInitFunc_;	// 反初始化

	public:
		scope_exit_t(const InitFuncionType &init, const UninitFunctionType &unInt)
			: initFunc_(init)
			, unInitFunc_(unInt)
		{
			initFunc_();
		}
		~scope_exit_t()
		{
			unInitFunc_();
		}

	private:
		scope_exit_t(const scope_exit_t &);
		scope_exit_t &operator=(const scope_exit_t &);
	};


	template <typename InitT, typename UninitT>
	inline scope_exit_base_ptr make_scope_exit(const InitT &init, const UninitT &uninit)
	{
		return scope_exit_base_ptr(new scope_exit_t<InitT, UninitT>(init, uninit));
	}
}




#endif