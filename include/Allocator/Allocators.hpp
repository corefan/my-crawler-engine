#ifndef __ALLOCATORS_HPP
#define __ALLOCATORS_HPP

#include <list>
#include "../MultiThread/Lock.hpp"



/*
	Allocator:
	
	AllocatorNewdel 
	AllocatorUnbounded 
	AllocatorFixedSize 
	AllocatorVariableSize 
	AllocatorSuballoc 
	AllocatorChunklist 


	Cache:				定义一个数据块分配器，分配和释放一个大小的内存块。

	CacheFreelist		
	CacheSuballoc 
	CacheChunklist		

	RtsAlloc

	Sync:

	SyncNone 
	SyncPerContainer 
	SyncPerThread 
	SyncShared 

	
	Max:

	MaxNone 
	MaxUnbounded 
	MaxFixedSize 
	MaxVariableSize 


	适用情况：

	基于节点的容器 （std::list、 std::set、 std::multiset、 std::map 和 std::multimap） 
	的标准 c + + 库中各个节点中存储它们的元素。容器类型的所有节点都都相同的大小，因此不需要一个通用的内存管理器。
	由于每个内存块的大小在编译时已知的内存管理器可以更简单、 更快捷。


	并不是基于节点 （例如，标准 c + + 库容器 std::vector std::deque 和 std::basic_string） 的容器所用 alllocator 模板将正常工作，
	但不可能提供任何性能改进与默认分配器相比。



*/

namespace async
{
	
	namespace allocator
	{
		// 同步过滤器

		
		// -----------------------------------------------------------
		// class SyncNone

		// 没有同步的缓冲

		template< typename CacheT >
		class SyncNone
		{
		private:
			static CacheT cache_;

		public:
			void *allocate(size_t sz)
			{ return cache_.allocate(sz); }

			void deallocate(void *ptr, size_t sz)
			{ return cache_.deallocate(ptr, sz); }

			bool equals(const SyncNone<CacheT> &rhs) const
			{ return cache_.equals(rhs.cache_); }
		};
		
		template<typename CacheT>
		CacheT SyncNone<CacheT>::cache_;


		// ---------------------------------------------------
		// class SyncPerContainer

		// 每个容器自己的同步缓冲

		template< typename CacheT >
		class SyncPerContainer
			: public CacheT
		{
		public:
			// 不共享内存
			bool equals(const SyncPerContainer<CacheT> &) const
			{ return false; }
		};


		// ---------------------------------------------------
		// class SyncShared

		// 同步共享缓冲区

		template< typename CacheT >
		class SyncShared
		{
			typedef async::thread::AutoCriticalSection	Mutex;
			typedef async::thread::AutoLock<Mutex>		AutoLock;

		private:
			static Mutex mutex_;
			static CacheT cache_;

		public:
			void *allocate(size_t sz)
			{ 
				AutoLock lock(mutex_);
				return cache_.allocate(sz);
			}

			void deallocate(void *ptr, size_t sz)
			{ 
				AutoLock lock(mutex_);
				cache_.deallocate(ptr, sz);
			}

			bool equals(const SyncNone<CacheT> &rhs) const
			{ return cache_.equals(rhs.cache_); }
		};

		template<typename CacheT>
		typename SyncShared<CacheT>::Mutex SyncShared<CacheT>::mutex_;

		template<typename CacheT>
		CacheT SyncShared<CacheT>::cache_;


		// ---------------------------------------------------
		// class SyncPerThread

		// 线程同步共享缓存
		
		template< typename CacheT >
		class SyncPerThread
		{
			struct memory_helper
			{
				std::list<CacheT *> memorys_;
		
				~memory_helper()
				{
					std::for_each(memorys_.begin(), memorys_.end(), [](CacheT *p)
					{
						delete p;
					});
				}

				void add(CacheT *val)
				{
					memorys_.push_back(val);
				}


				static memory_helper &singleton()
				{
					static memory_helper helper;
					return helper;
				}
			};
		private:
			static __declspec(thread) CacheT *cache_;

		public:
			void *allocate(size_t sz)
			{
				if( cache_ == 0 )
				{
					cache_ = new CacheT();

					memory_helper::singleton().add(cache_);
				}
				return (cache_ != 0) ? cache_->allocate(sz) : 0;
			}

			void deallocate(void *ptr, size_t sz)
			{
				if( cache_ != 0 )
					cache_->deallocate(ptr, sz);
			}

			bool equals(const SyncPerThread<CacheT> &rhs) const
			{
				return (cache_ != 0) && 
					rhs.cache_ != 0 &&
					cache_->equals(*rhs.cache_);
			}
		};

		template<typename CacheT>
		__declspec(thread) CacheT *SyncPerThread<CacheT>::cache_;



		// ----------------------------------------------------------
		// 缓冲区

		
		class MaxNone
		{
			// 为CacheNone服务
		public:
			bool full() const
			{ return true; }
			
			void saved() 
			{}

			void released()
			{}

			void allocated(size_t = 1)
			{}

			void deallocated(size_t = 1)
			{}
		};

		class MaxUnbounded
		{
			// 为Free List服务，且没有限制大小
		public:
			bool full() const
			{ return false; }

			void saved() 
			{}

			void released()
			{}

			void allocated(size_t = 1)
			{}

			void deallocated(size_t = 1)
			{}
		};

		template< size_t Max >
		class MaxFixedSize
		{
			// 为Free List服务，且有大小限制
		private:
			unsigned long numBlocks_;

		public:
			MaxFixedSize()
				: numBlocks_(0)
			{}

		public:
			bool full() const
			{ return Max <= numBlocks_; }

			void saved() 
			{ ++numBlocks_; }

			void released()
			{ --numBlocks_; }

			void allocated(size_t = 1)
			{}

			void deallocated(size_t = 1)
			{}
		};

		class MaxVariableSize
		{
		private:
			unsigned long numBlocks_;
			unsigned long numAllocs_;

		public:
			MaxVariableSize()
				: numBlocks_(0)
				, numAllocs_(0)
			{}
		
		public:
			bool full() const
			{ return numAllocs_ / 16 + 16 <= numBlocks_; }

			void saved() 
			{ ++numBlocks_; }
 
			void released()
			{ --numBlocks_; }

			void allocated(size_t sz = 1)
			{ numAllocs_ += sz; }

			void deallocated(size_t sz = 1)
			{ numAllocs_ -= sz; }

		};

		template< size_t _Nelts = 20 >
		struct MaxNelts
		{
			static const size_t Nelts = _Nelts;
		};


		// --------------------------------------------------------------
		// class _FreeList

		// 
		template< size_t Size, typename MaxT >
		class _FreeList
			: public MaxT
		{
		private:
			struct _node
			{
				_node *next_;
				_node()
					: next_(0)
				{}
			};
			_node *head_;

		public:
			_FreeList()
				: head_(0)
			{}

		public:
			bool push(void *ptr)
			{
				if( MaxT::full() )
					return false;
				else
				{
					_node *tmp = static_cast<_node *>(ptr);
					tmp->next_ = head_;
					head_ = tmp;

					MaxT::saved();
					return true;
				}
			}

			void *pop()
			{
				void *ptr = head_;
				if( ptr != 0 )
				{
					head_ = head_->next_;
					MaxT::released();
				}

				return ptr;
			}
		};


		// -----------------------------------------------------------
		// class CacheFreelist

		// 从堆里分配内存，利用Freelist进行缓存
		template< size_t Size, typename MaxT >
		class CacheFreelist
		{
		private:
			_FreeList<Size, MaxT> freeList_;

		public:
			~CacheFreelist()
			{
				void *ptr = 0;
				while( (ptr = freeList_.pop()) != 0 )
					::operator delete(ptr);
			}

		public:
			void *allocate(size_t sz)
			{
				void *res = freeList_.pop();

				// 如果为空，则从堆里分配
				if( res == 0 )
				{
					if( sz < sizeof(void *) )
						res = ::operator new(sizeof(void *));
					else
						res = ::operator new(sz);

					freeList_.allocated();
				}

				return res;
			}

			void deallocate(void *ptr, size_t)
			{
				// 放回FreeList
				if( !freeList_.push(ptr) )
				{
					// 释放
					::operator delete(ptr);
					freeList_.deallocated();
				}
			}

			bool equals(const CacheFreelist<Size, MaxT> &) const
			{ return true; }
		};


		// --------------------------------------------------
		// class CacheSubAlloc

		// 

		template< size_t Size, typename NeltsT >
		class CacheSuballoc
		{
		private:
			_FreeList<Size, MaxUnbounded> freeList_;
			//_FreeList<Size, MaxUnbounded> helper_;
			char *begin_;
			char *end_;

		public:
			CacheSuballoc()
				: begin_(0)
				, end_(0)
			{}
			~CacheSuballoc()
			{
				/*void *ptr = 0;
				while( (ptr = helper_.pop()) != 0 )
					::operator delete(ptr);*/
			}

		public:
			// 一块一块的分配
			void *allocate(size_t sz)
			{
				void *res = freeList_.pop();

				if( res == 0 )
				{
					if( begin_ == end_ )
					{
						if( sz * NeltsT::Nelts < sizeof(void *) )
							begin_ = static_cast<char *>(::operator new(sizeof(void *)));
						else
							begin_ = static_cast<char *>(::operator new(sz * NeltsT::Nelts));

						end_ = begin_ + sz * NeltsT::Nelts;
						freeList_.allocated(NeltsT::Nelts);

						//helper_.push(begin_);
					}

					res = begin_;
					begin_ += sz;
				}

				return res;
			}

			void deallocate(void *ptr, size_t)
			{
				freeList_.push(ptr);
			}

			bool equals(const CacheSuballoc<Size, NeltsT> &) const
			{ return true; }
		};


		// ---------------------------------------------------
		// class CacheChunklist

		// 

		template< size_t Size, typename NeltsT >
		class CacheChunklist
		{
		private:
			class _DataBlock;

			// 带有父节点的内存块
			struct _DataNode
			{
				_DataBlock *parent_;
				union 
				{
					_DataNode *next_;
					char data_[Size];
				};
			};

			// 管理数组数据
			class _DataArray
			{
			private:
				_DataNode elem_[NeltsT::Nelts];

			public:
				_DataArray()
				{}
				_DataNode &operator[](int index)
				{
					return elem_[index];
				}
			};

			// 内存块
			class _DataBlock
			{
			private:
				_DataArray data_;
				_DataNode *node_;

			public:
				size_t freeCount_;
				_DataBlock *next_, *prev_;

			public:
				_DataBlock(size_t sz)
					: node_(0)
					, freeCount_(0)
					, next_(0), prev_(0)
				{
					node_ = &data_[0];
					freeCount_ = NeltsT::Nelts;

					// 构造链表
					for(size_t i = 0; i != freeCount_ - 1; ++i)
					{
						data_[i].parent_	= this;
						data_[i].next_		= &data_[i + 1];
					}

					data_[freeCount_ - 1].parent_	= this;
					data_[freeCount_ - 1].next_		= 0;
				}

				void *allocate()
				{
					_DataNode *node = node_;
					node_ = node_->next_;
					--freeCount_;

					return &node->data_;
				}

				static _DataBlock *deallocate(void *p)
				{
					_DataNode *node = reinterpret_cast<_DataNode *>((char *)p - offsetof(_DataNode, data_));
					_DataBlock *block = node->parent_;

					node->next_		= block->node_;
					block->node_	= node;
					++block->freeCount_;

					return block;
				}
			};

			_DataBlock *list_;
			_DataBlock *cacheBlock_;

		public:
			CacheChunklist()
				: list_(0)
				, cacheBlock_(0)
			{}

		public:
			void *allocate(size_t sz)
			{
				if( list_ == 0 )
				{
					list_ = new _DataBlock(sz);
					list_->next_ = list_->prev_ = list_;
				}
				else if( list_->freeCount_ == 0 )
				{
					_DataBlock *block = 0;
					if( cacheBlock_ != 0 )
					{
						block = cacheBlock_;
						cacheBlock_ = 0;
					}
					else
						block = new _DataBlock(sz);

					block->next_ = list_->next_;
					block->prev_ = list_;
					block->next_->prev_ = block;
					block->prev_->next_ = block;
					list_ = block;
				}

				void *res = list_->allocate();
				if( list_->freeCount_ == 0 && list_->next_ != list_ )
				{
					list_->next_->prev_ = list_->prev_;
					list_->prev_->next_ = list_->next_;
					list_ = list_->next_;
				}

				return res;
			}

			void deallocate(void *p, size_t)
			{
				_DataBlock *block = _DataBlock::deallocate(p);

				if( block == list_->prev_ )
					;
				else if( block->freeCount_ == 1 )
				{
					_DataBlock *tail = list_->prev_;
					tail->prev_->next_ = block;
					block->prev_ = tail->prev_;
					tail->prev_ = block;
					block->next_ = tail;

					if( list_->freeCount_ == 0 )
						list_ = list_->next_;
				}
				else if( block->freeCount_ == NeltsT::Nelts )
				{
					block->next_->prev_ = block->prev_;
					block->prev_->next_ = block->next_;
					if( list_ == block )
						list_ = list_->next_;
					if( cacheBlock_ )
						delete block;
					else
						cacheBlock_ = block;
				}
			}

			bool equals(const CacheChunklist<Size, NeltsT> &) const
			{ return true; }
		};


		// ----------------------------------------------------
		// class RtsAlloc

		template< typename CacheT >
		class RtsAlloc
		{
			static const int WIDTH = 18;
			static const int COUNT = 16;

		public:
			void *allocate(size_t sz)
			{
				sz = Align_(sz);

				return sz < WIDTH * COUNT 
					? caches_[Index_(sz)].allocate(sz) 
					: ::operator new(sz);
			}

			void deallocate(void *ptr, size_t sz)
			{
				sz = Align_(sz);

				if( sz < WIDTH * COUNT )
					caches_[Index_(sz)].deallocate(ptr, sz);
				else
					::operator delete(ptr);
			}

			bool equals(const RtsAlloc<CacheT> &rhs) const
			{
				return caches_[0].equals(rhs.caches_[0]);
			}

		private:
			int Index_(int sz)
			{ return sz / WIDTH; }

			int Align_(int sz)
			{ return WIDTH * (sz + WIDTH - 1) / WIDTH; }

		private:
			CacheT caches_[COUNT];
		};



		// allocator

		// -------------------------------------------------------------------
		// class Allocator

		template< typename T, 
			typename SyncT = SyncShared<CacheFreelist<100, MaxNone>> 
		>
		class Allocator
			: public SyncT			// 潜在空基类优化
		{
		public:
			typedef size_t			size_type;
			typedef ptrdiff_t		difference_type;
			typedef T				value_type;
			typedef T *				pointer;
			typedef const T *		const_pointer;
			typedef T &				reference;
			typedef const T &		const_reference;

			template<typename U>						
			struct rebind								
			{ typedef Allocator<U, SyncT> other; };

		public:
			Allocator()
			{}

			template<typename U>
			Allocator(const Allocator<U, SyncT> &rhs)
				: SyncT(rhs)
			{}

		public:
			pointer address(reference val)
			{ return &val; }
			const_pointer address(const_reference val)
			{ return &val; }

			template<typename U>
			pointer allocate(size_type sz, const U *)
			{ return allocate(sz); }

			pointer allocate(size_type sz)
			{
				if( sz <= 0 )
					sz = 0;
				else if( (size_t)(-1) / sizeof(T) < sz )
					throw std::bad_alloc(0);

				if( sz == 1 )
					return static_cast<pointer>(SyncT::allocate(sizeof(value_type)));
				else
					return static_cast<pointer>(::operator new(sz * sizeof(value_type)));
			}

			void deallocate(pointer ptr, size_type sz)
			{
				if( ptr == 0 )
					;
				else if( sz == 1 )
					deallocate(ptr, sizeof(value_type));
				else
					::operator delete(ptr);
			}

			void construct(pointer ptr, const_reference val)
			{
				::new (static_cast<void *>(ptr)) value_type(val);
			}

			void destroy(pointer ptr)
			{
				ptr->~value_type();
			}

			size_type max_size() const
			{
				size_t count = size_t(-1) / sizeof(value_type);
				return 0 < count ? count : 1;
			}
		};


		template< typename SyncT >										
		class Allocator<void, SyncT>								
		{												
		public:											
			typedef void *				pointer;		
			typedef const void *		const_pointer;	
			typedef void				value_type;		

		public:											
			Allocator() {}									
			template<typename T>						
			Allocator(const Allocator<T, SyncT> &) {}					
			template<typename T>						
			Allocator &operator=(const Allocator<T,SyncT> &)			
			{ return *this; }	

		public:											
			template<typename U>						
			struct rebind								
			{ typedef Allocator<U, SyncT> other; };					
		};


		template< typename T, typename SyncT >
		inline bool operator==(const Allocator<T, SyncT> &lhs, const Allocator<T, SyncT> &rhs)
		{
			return lhs.equals(rhs);
		}

		template< typename T, typename SyncT >
		inline bool operator!=(const Allocator<T, SyncT> &lhs, const Allocator<T, SyncT> &rhs)
		{
			return !(lhs == rhs);
		}
		

		template < typename T >
		class AllocatorUnbounded
			: public Allocator<T, SyncShared<RtsAlloc<CacheFreelist<100, MaxUnbounded>>>>
		{};

		template < typename T, size_t Size = 20 >
		class AllocatorFixedSize
			: public Allocator<T, SyncShared<RtsAlloc<CacheFreelist<100, MaxFixedSize<Size>>>>>
		{};
		
		template < typename T >
		class AllocatorVariableSize
			: public Allocator<T, SyncShared<RtsAlloc<CacheFreelist<100, MaxVariableSize>>>>
		{};

		template < typename T >
		class AllocatorSuballoc
			: public Allocator<T, SyncShared<RtsAlloc<CacheSuballoc<sizeof(T), MaxNelts<>>>>>
		{};

		template < typename T >
		class AllocatorChunklist
			: public Allocator<T, SyncShared<RtsAlloc<CacheChunklist<sizeof(T), MaxNelts<>>>>>
		{};


		template < typename T >
		class AllocatorSyncNone
			: public Allocator<T, SyncNone<RtsAlloc<CacheFreelist<100, MaxNone>>>>
		{};

		template < typename T >
		class AllocatorSyncPerContainer
			: public Allocator<T, SyncPerContainer<RtsAlloc<CacheFreelist<100, MaxNone>>>>
		{};

		template < typename T >
		class AllocatorSyncPerThread
			: public Allocator<T, SyncPerThread<RtsAlloc<CacheFreelist<100, MaxNone>>>>
		{};
	}

}







#endif