#ifndef __CACHE_HPP
#define __CACHE_HPP 

#include <cassert> 

#include "../MultiThread/Lock.hpp"
#include "detail/LFUImpl.hpp"
#include "detail/LRUImpl.hpp"
#include "detail/FIFOImpl.hpp"


#pragma warning(disable: 4503)

namespace async
{
	namespace cache
	{
		
		// 缓存策略
		/*
			1. 按过期时间
			2. 按间隔时间
			3. 依赖项
			4. LRU/MRU

		*/

		template< 
			typename K, 
			typename V,
			template< typename, typename > class CacheImplT, 
			typename L = thread::AutoCriticalSection
		>
		class CacheT
		{ 
		public: 
			typedef K key_type; 
			typedef V value_type; 
			typedef L Mutex;

			typedef CacheImplT<K, V> CacheImpl;

			typedef typename CacheImpl::map_iterator		map_iterator;
			typedef typename CacheImpl::map_const_iterator	map_const_iterator;

			typedef thread::AutoLock<Mutex>					AutoLock;

		private:
			size_t capacity_;			
			size_t size_;

			CacheImpl impl_;
			mutable Mutex mutex_;

		public:
			explicit CacheT(size_t c)
				: impl_(c)
				, capacity_(c)
				, size_(0)
			{

			}

			~CacheT()
			{
				clear();
			}

		private:
			CacheT(const CacheT &);
			CacheT &operator=(const CacheT &);

		public:
			map_iterator begin()
			{
				AutoLock lock(mutex_);
				return impl_.begin();
			}

			map_const_iterator begin() const
			{
				AutoLock lock(mutex_);
				return impl_.begin();
			}

			map_iterator end()
			{
				AutoLock lock(mutex_);
				return impl_.end();
			}

			map_const_iterator end() const
			{
				AutoLock lock(mutex_);
				return impl_.end();
			}

		public:
			void insert(const key_type &k, const value_type &v)
			{
				AutoLock lock(mutex_);

				if( capacity_ == size_ )
				{
					impl_.evict();
					--size_;
				}

				impl_.insert(k, v);
				++size_;
			}

			// Obtain value of the cached function for k 
			std::pair<const value_type, bool> get(const key_type& k)
			{
				AutoLock lock(mutex_);

				bool has = impl_.exsit(k); 
				if( !has ) 
				{
					static value_type tmp;
					return std::pair<const value_type, bool>(tmp, false);
				} 
				else 
				{ 
					return impl_.update(k);
				} 
			} 


			// 清空所有数据
			void clear()
			{
				AutoLock lock(mutex_);
				impl_.clear();

				size_ = 0;
			}

			size_t capacity() const
			{
				AutoLock lock(mutex_);
				return capacity_;
			}

			size_t size() const
			{
				AutoLock lock(mutex_);
				return size_;
			}
		};


		using detail::LRU;
		using detail::LFU;
		using detail::FIFO;
	}
}

#endif