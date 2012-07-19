#ifndef __IOCP_BUFFER_HPP
#define __IOCP_BUFFER_HPP



#include <array>
#include <vector>
#include <string>

#include "ObjectFactory.hpp"
#include "ReadWriteBuffer.hpp"

#pragma warning(disable: 4996) // ȥ��

namespace async
{


	namespace iocp
	{
		
		enum { DEFAULT_SIZE = 8 * 1024 };

		//-------------------------------------------------------------------
		// class BufferT
		// �ṩ����������֧��STL��׼�ӿ�

		template<typename T, typename AllocT>
		class AutoBufferT
		{
		public:
			typedef T			value_type;
			typedef T*			pointer;
			typedef const T*	const_pointer;
			typedef T&			reference;
			typedef const T&	const_reference;

			// �ڴ��������
			typedef AllocT		allocator_type;

		private:
			allocator_type alloc_;		// ������

			bool internal_;				// �Ƿ��ⲿ�ṩ������
			size_t capacity_;			// �ѷ��仺������С
			size_t bufferSize_;			// ��ʹ�û�������С

			value_type *buffer_;		// ������ָ��
			
		public:
			explicit AutoBufferT(size_t defaultSize = DEFAULT_SIZE, const allocator_type &alloc = allocator_type())
				: alloc_(alloc)
				, internal_(true)
				, capacity_(defaultSize)
				, bufferSize_(0)
				, buffer_(_Allocate(defaultSize))
			{}
			AutoBufferT(pointer pStr, size_t nSize, const allocator_type &alloc = allocator_type())
				: alloc_(alloc)
				, internal_(false)
				, capacity_(nSize)
				, bufferSize_(nSize)
				, buffer_(pStr)
			{
			}
			AutoBufferT(const_pointer beg, const_pointer end, const allocator_type &alloc = allocator_type())
				: alloc_(alloc)
				, internal_(false)
				, capacity_(std::distance(beg, end))
				, bufferSize_(capacity_)
				, buffer_(beg)
			{
			}

			~AutoBufferT()
			{
				// ���Ϊ�ڲ����仺����������Ҫ�Զ��ͷ�
				if( internal_ )
					alloc_.deallocate(buffer_, capacity_);
			}

		public:
			pointer begin()
			{
				return buffer_;
			}
			const_pointer begin() const
			{
				return buffer_;
			}

			pointer end()
			{
				return buffer_ + bufferSize_;
			}
			const_pointer end() const
			{
				return buffer_ + bufferSize_;
			}

			size_t size() const
			{
				return bufferSize_;
			}
			size_t capacity() const
			{
				return capacity_;
			}

			void resize(size_t nNewSize)
			{
				// ������ⲿ������
				if( !internal_ )
				{
					if( nNewSize <= capacity_ )
						bufferSize_ = nNewSize;
					else
						throw std::out_of_range("buffer out of range");
				}

				if( nNewSize <= capacity_ )
					bufferSize_ = nNewSize;
				else
				{
					// �����»�����
					pointer pNewBuf = _Allocate(nNewSize);

					// ���ƻ�����
					std::copy(buffer_, buffer_ + capacity_, 
						stdext::make_checked_array_iterator(pNewBuf, nNewSize));

					// �ͷžɻ�����
					alloc_.deallocate(buffer_, capacity_);

					capacity_	= nNewSize;
					bufferSize_ = nNewSize;
					buffer_		= pNewBuf;
				}
			}

			pointer data(size_t offset = 0)
			{
				if( offset >= capacity_ )
					throw std::out_of_range("buffer offset >= allocSize_");

				return buffer_ + offset;
			}
			const_pointer data(size_t offset = 0) const
			{
				if( offset >= capacity_ )
					throw std::out_of_range("buffer offset >= allocSize_");

				return buffer_ + offset;
			}

			pointer operator[](size_t offset)
			{
				return buffer_ + offset;
			}
			const_pointer operator[](size_t offset) const
			{
				return buffer_ + offset;
			}

			void append(pointer buf, size_t len)
			{
				if( len > capacity_ - bufferSize_ )
					resize(2 * len);

				std::copy(buf, buf + len, buffer_);
				bufferSize_ += len;
			}

		private:
			pointer _Allocate(size_t nSize)
			{
				pointer pBuf = alloc_.allocate(nSize);
				std::uninitialized_fill_n(pBuf, nSize, 0);

				return pBuf;
			}
		};

		typedef async::memory::SGIMemoryPool<true, DEFAULT_SIZE>		MemoryPool;

		typedef async::allocator::ContainerAllocator< char, MemoryPool > AutoBufferAllocator;
		typedef AutoBufferT< char, AutoBufferAllocator >	AutoBuffer;
		typedef std::tr1::shared_ptr<AutoBuffer>			AutoBufferPtr;




		

		// -------------------- Buffer Helper Function -----------------------------

		inline AutoBufferPtr MakeBuffer(size_t sz)
		{
			return AutoBufferPtr(ObjectAllocate<AutoBuffer>(sz), &ObjectDeallocate<AutoBuffer>);
		}

		inline AutoBufferPtr MakeBuffer(char *buf, size_t sz)
		{
			return AutoBufferPtr(ObjectAllocate<AutoBuffer>(buf, sz), &ObjectDeallocate<AutoBuffer>);
		}

		inline AutoBufferPtr MakeBuffer(const char *buf, size_t sz)
		{
			return MakeBuffer(const_cast<char *>(buf), sz);
		}

		// --------------------------

		template<size_t _N>
		inline AutoBufferPtr MakeBuffer(char (&arr)[_N])
		{
			return AutoBufferPtr(ObjectAllocate<AutoBuffer>(arr, _N), &ObjectDeallocate<AutoBuffer>);
		}
		template<size_t _N>
		inline AutoBufferPtr MakeBuffer(const char (&arr)[_N])
		{
			return MakeBuffer(const_cast<char (&)[_N]>(arr));
		}

		// --------------------------

		template<size_t _N>
		inline AutoBufferPtr MakeBuffer(std::tr1::array<char, _N> &arr)
		{
			return AutoBufferPtr(ObjectAllocate<AutoBuffer>(arr.data(), _N), &ObjectDeallocate<AutoBuffer>);
		}
		template<size_t _N>
		inline AutoBufferPtr MakeBuffer(const std::tr1::array<char, _N> &arr)
		{
			return MakeBuffer(const_cast<std::tr1::array<char, _N> &>(arr));
		}

		// --------------------------

		inline AutoBufferPtr MakeBuffer(std::vector<char> &arr)
		{
			return AutoBufferPtr(ObjectAllocate<AutoBuffer>(&arr[0], arr.size()), &ObjectDeallocate<AutoBuffer>);
		}
		inline AutoBufferPtr MakeBuffer(const std::vector<char> &arr)
		{
			return MakeBuffer(const_cast<std::vector<char> &>(arr));
		}

		// --------------------------

		inline AutoBufferPtr MakeBuffer(std::string &arr)
		{
			return AutoBufferPtr(ObjectAllocate<AutoBuffer>(&*arr.begin(), arr.length()), &ObjectDeallocate<AutoBuffer>);
		}
		inline AutoBufferPtr MakeBuffer(const std::string &arr)
		{
			return MakeBuffer(const_cast<std::string &>(arr));
		}


		// -----------------------------
		inline ConstBuffer Buffer(const AutoBuffer &buf)
		{
			return ConstBuffer(buf.data(), buf.size());
		}

		inline MutableBuffer Buffer(AutoBuffer &buf)
		{
			return MutableBuffer(buf.data(), buf.size());
		}

	} // end of iocp


} // end of async





#endif