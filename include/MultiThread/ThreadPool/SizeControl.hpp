#ifndef __THREAD_POOL_SIZE_CONTROL_HPP
#define __THREAD_POOL_SIZE_CONTROL_HPP



namespace async
{
	namespace threadpool
	{

		// ----------------------------------
		// ��̬��С,����������

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
		// ��̬����,���ݸ���������

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