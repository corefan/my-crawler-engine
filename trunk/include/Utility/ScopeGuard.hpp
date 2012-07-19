#ifndef __SCOPED_GUARD_HPP
#define __SCOPED_GUARD_HPP


#include <functional>



namespace async
{
	// -------------------------------------------------
	// class ScopeGuard

	template< typename R >
	class ScopeGuard
	{
		typedef std::tr1::function<R()> Func;
		const Func func_;
		bool isActive_;

	public:
		explicit ScopeGuard(const Func &func)
			: func_(func)
			, isActive_(true)
		{}
		~ScopeGuard()
		{
			if( isActive_ && func_ )
				func_();
		}

	private:
		ScopeGuard(const ScopeGuard &);
		ScopeGuard &operator=(const ScopeGuard &);

	public:
		void Disable()
		{
			isActive_ = false;
		}
	};
}






#endif