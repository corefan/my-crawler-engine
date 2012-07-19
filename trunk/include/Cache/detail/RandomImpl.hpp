#ifndef __CACHE_RANDOM_IMPL_HPP
#define __CACHE_RANDOM_IMPL_HPP



#include <cassert> 
#include <vector> 
#include <unordered_map>

#include "../MemoryPool/SGIMemoryPool.hpp"
#include "../Allocator/ContainerAllocator.hpp"


namespace async
{
	namespace cache
	{
		namespace detail
		{

			// Random

			template< 
				typename K, 
				typename V
			>
			class Random
			{
			public: 
				typedef K key_type; 
				typedef V value_type; 

				// cache list
				typedef std::vector<key_type> cache_list;

				
				typedef std::pair<const key_type, value_type> map_value_type;

				// map node pool 
				typedef async::memory::SGIMemoryPool<
					false, 
					256
				> map_pool;

				// map allocator
				typedef async::allocator::ContainerAllocator<
					map_value_type, 
					map_pool
				> map_allocator;

				// Key to value and key history iterator 
				typedef std::tr1::unordered_map< 
					key_type, 
					value_type,
					std::tr1::hash<key_type>,
					std::equal_to<key_type>/*,
										   map_allocator*/
				> cache_map;

				typedef typename cache_list::iterator		list_iterator;
				typedef typename cache_list::const_iterator	list_const_iterator;
				typedef typename cache_map::iterator		map_iterator;
				typedef typename cache_map::const_iterator	map_const_iterator;

			private:
				//list_cache_pool listCachePool_;

				// Key access history 
				cache_list list_;

				//map_pool mapPool_;

				// Key-to-value lookup 
				cache_map map_; 


				size_t insertIndex_;
				size_t eraseIndex_;
			public:
				// Constuctor specifies the cached function and 
				// the maximum number of records to be stored 
				explicit Random(size_t c) 
					: list_(c)
					, map_(c/*, std::tr1::hash<key_type>(), std::equal_to<key_type>(), map_allocator(mapPool_)*/)
					, insertIndex_(0)
					, eraseIndex_(0)
				{ 
					assert(c != 0);
				}


			public:
				map_iterator begin()
				{
					return map_.begin();
				}

				map_const_iterator begin() const
				{
					return map_.begin();
				}

				map_iterator end()
				{
					return map_.end();
				}

				map_const_iterator end() const
				{
					return map_.end();
				}

				// 清空所有数据
				void clear()
				{
					list_.clear();
					map_.clear();
				}

				bool exsit(const key_type& k)
				{
					// Attempt to find existing record 
					return map_.find(k) != map_.end(); 
				}

				// Obtain value of the cached function for k 
				std::pair<const value_type, bool> update(const key_type& k)
				{
					assert(exsit(k));

					// Attempt to find existing record 
					map_iterator it = map_.find(k); 

					// Return the retrieved value 
					return std::pair<const value_type, bool>(it->second, true); 
				} 

				void insert(const key_type &key, const value_type &val)
				{
					// Method is only called on cache misses 
					assert(map_.find(key) == map_.end()); 

					if( insertIndex_ >= list_.size() )
						insertIndex_ = 0;

					list_[insertIndex_++] = key;
					// Create the key-value entry, linked to the usage record. 
					map_.insert(std::make_pair(key, val));
				}

				// Purge the least-recently-used element in the cache 
				void evict() 
				{ 
					// Assert method is never called when cache is empty 
					assert(!list_.empty());
					if( list_.empty() )
						return;

					if( eraseIndex_ >= list_.size() )
						eraseIndex_ = 0;

					const key_type &key = list_[eraseIndex_++];
					map_.erase(key);
				}

			};
		}
	}
}




#endif