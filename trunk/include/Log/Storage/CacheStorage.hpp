#ifndef __LOG_CACHE_LOG_HPP
#define __LOG_CACHE_LOG_HPP

#include "StorageBase.hpp"
#include <array>


#pragma warning (disable : 4996)

namespace logsystem
{

	namespace storage
	{
		// ---------------------------------------------------------------------
		// class CacheStorage

		template<size_t _CacheSize, typename ImplStorageT, typename Base = StorageBase<typename ImplStorageT::char_type>>
		class CacheStorageT
			: public Base
		{
			typedef Base									StorageType;
			typedef ImplStorageT							ImpleStorageType;

		public:
			typedef typename StorageType::char_type			char_type;
			typedef typename StorageType::size_type			size_type;

		private:
			ImpleStorageType implStorage_;
			std::tr1::array<char_type, _CacheSize> cache_;
			size_t pos_;

			// 解析不定值参数时的缓冲区
			enum { BUFFER_SIZE = 512 };

		public:
			CacheStorageT()
				: pos_(0)
			{
				std::fill_n(cache_.begin(), _CacheSize, 0);
			}
			template<typename ArgT>
			CacheStorageT(const ArgT &arg)
				: implStorage_(arg)
				, pos_(0)
			{
				std::fill_n(cache_.begin(), _CacheSize, 0);
			}
			template<typename ArgT1, typename ArgT2>
			CacheStorageT(const ArgT1 &arg1, const ArgT2 &arg2)
				: implStorage_(arg1, arg2)
				, pos_(0)
			{
				std::fill_n(cache_.begin(), _CacheSize, 0);
			}

		public:
			void Put(char_type ch)
			{
				if( _IsFull(1) )
					Flush();

				_Push(ch);
			}

			void Put(size_t count, char_type ch)
			{
				if( _IsFull(count) )
					Flush();

				_Push(count, ch);
			}

			void Put(const char_type *pStr, size_t count)
			{
				if( _IsFull(count) )
					Flush();

				_Push(pStr, count);
			}

			void Put(const char_type *pStr, va_list args)
			{
				// Warning... NonImplement
			}

			void Flush()
			{
				implStorage_.Put(cache_.data(), pos_);
				implStorage_.Flush();
				pos_ = 0;
			}


		private:
			// 判断缓冲区是否够容纳size大小的数据
			bool _IsFull(size_t size)
			{
				if( pos_ + size < _CacheSize )
					return false;
				else
					return true;
			}

			void _Push(char_type c)
			{
				cache_[pos_++] = c;
			}
			void _Push(size_t count, char_type ch)
			{
				for(; count >= 0; --count)
				{
					if( pos_ >= _CacheSize )
						Flush();

					cache_[pos_++] = ch;
				}
			}
			void _Push(const char_type *pStr, size_t count)
			{
				// 如果缓冲区足够，则写入缓冲
				if( count + pos_ < _CacheSize )
				{
					std::copy(pStr, pStr + count, 
						stdext::checked_array_iterator<char_type *>(&cache_[pos_], _CacheSize - pos_));
					pos_ += count;
				}
				// 缓冲区不足
				else
				{		
					// 如果缓冲区不够容纳当前写入数据,则直接写入设备
					if( count > _CacheSize )
					{
						// 把缓冲的数据写入设备
						Flush();
						implStorage_.Put(pStr, count);
					}
					// 写入缓冲
					else
                    {
						std::copy(pStr, pStr + count, 
							stdext::make_checked_array_iterator(&cache_[pos_], _CacheSize - pos_));
						pos_ += count;
					}
				}
			}

			void _Push(const char *fmt, va_list args)
			{
				char buf[BUFFER_SIZE] = {0};
				int cch = _vsnprintf_s(buf, BUFFER_SIZE, fmt, args);
				if( cch	< 0 )
				{
					assert(0);
					cch = 0;
				}

				std::copy(buf, buf + StrlenT(buf), &cache_[pos_]);
				pos_ += cch;
			}
			void _Push(const wchar_t *fmt, va_list args)
			{
				wchar_t buf[BUFFER_SIZE] = {0};
				int cch = _vsnwprintf_s(buf, BUFFER_SIZE, fmt, args);
				if( cch	< 0 )
				{
					assert(0);
					cch = 0;
				}

				std::copy(buf, buf + StrlenT(buf), &cache_[pos_]);
				pos_ += cch;
			}
		};
	}
	
}


#endif