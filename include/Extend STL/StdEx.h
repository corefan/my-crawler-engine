#ifndef __STD_EX_HPP
#define __STD_EX_HPP



#include <functional>
#include <map>
#include <vector>


namespace stdex
{


	template<typename OperatorT1, typename OperatorT2, typename OperatorT3>
	class BinaryCompose
	{
	public:
		typedef typename OperatorT1::result_type	result_type;

	private:
		OperatorT1 m_func1;
		OperatorT2 m_func2;
		OperatorT3 m_func3;

	public:
		BinaryCompose(const OperatorT1 &x, const OperatorT2 &y, const OperatorT3 &z)
			: m_func1(x)
			, m_func2(y)
			, m_func3(z)
		{
		}

		template < typename T >
		result_type operator()(const T &x) const
		{
			return m_func1(m_func2(x), m_func3(x));
		}
	};

	template<typename OperatorT1, typename OperatorT2, typename OperatorT3>
	inline BinaryCompose<OperatorT1, OperatorT2, OperatorT3> Compose2(const OperatorT1 &func1, 
		const OperatorT2 &func2, const OperatorT3 &func3)
	{
		return BinaryCompose<OperatorT1, OperatorT2, OperatorT3>(func1, func2, func3);
	}


	namespace detail
	{
		template< typename tPair > 
		struct second_t 
		{     
			typename tPair::second_type operator()( const tPair& p ) const 
			{ return p.second; } 
		};  
	}
	

	template< typename tMap >  
	inline detail::second_t< typename tMap::value_type > Second( const tMap& m )
	{
		return detail::second_t< typename tMap::value_type >(); 
	}


	template < typename KeyT, typename ValueT, typename LessT, typename AllocatorT, typename VecAllocatorT >
	inline void map2vector(const std::map<KeyT, ValueT, LessT, AllocatorT> &maps, std::vector<ValueT, VecAllocatorT> &vecs)
	{
		std::transform(maps.begin(), maps.end(),
			std::back_inserter(vecs),
			std::tr1::bind(&std::map<KeyT, ValueT, LessT, AllocatorT>::value_type::second, std::tr1::placeholders::_1) );
	}
}

#endif