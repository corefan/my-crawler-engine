#ifndef __THREAD_POOL_SIZE_CONTROL_HPP
#define __THREAD_POOL_SIZE_CONTROL_HPP



namespace async
{
	namespace threadpool
	{

		// ----------------------------------
		// 静态大小,不允许增长

		template < typename ImplT >
		class StaticSize
		{
		public:
			void Resize(ImplT &impl, size_t cnt)
			{
				return;
			}
		};

		// ----------------------------------
		// 动态增长,根据负载量控制

		template < typename ImplT >
		class DynamicSize
		{
		public:
			void Resize(ImplT &impl, size_t cnt)
			{
				impl.Resize(cnt);
				return;
			}
		};
	}
}






#endif