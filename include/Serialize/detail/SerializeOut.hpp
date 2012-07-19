#ifndef __SERIALIZE_OUT_HPP
#define __SERIALIZE_OUT_HPP


#include <cassert>


namespace utility
{
	namespace serialize
	{
		namespace detail
		{
			template < typename CharT, typename ImplT >
			class SerializeOut
			{
			public:
				typedef CharT				value_type;
				typedef CharT *				pointer;
				typedef value_type &		reference;
				typedef const CharT *		const_pointer;
				typedef const value_type &	const_reference;

				typedef std::char_traits<CharT> CharTraits;

			private:
				size_t outPos_;

			public:
				SerializeOut()
					: outPos_(0)
				{}

			public:
				size_t Length() const
				{
					return outPos_;
				}

			public:
				template < typename T >
				void Pop(T &val)
				{
					static_assert(std::tr1::is_pod<T>::value);

					assert(sizeof(T) + outPos_ < Impl()->BufferLength());
					if( sizeof(T) + outPos_ >= Impl()->BufferLength() )
						throw std::out_of_range("sizeof(T) + pos_ >= bufLen_");

					pointer buf = reinterpret_cast<pointer>(&val);
					Impl()->Read(buf, sizeof(T), outPos_);
					outPos_ += sizeof(T);
				}

				template < typename T, size_t N >
				void PopArray(T (&arr)[N])
				{
					static_assert(std::tr1::is_pod<T>::value);

					const size_t len = sizeof(T) * N;
					assert(len + outPos_ < Impl()->BufferLength());
					if( len + outPos_ >= Impl()->BufferLength() )
						throw std::out_of_range("sizeof(T) + pos_ >= bufLen_");

					pointer buf = reinterpret_cast<pointer>(&arr);
					Impl()->Read(buf, len, outPos_);
					outPos_ += len;
				}

				template < typename T >
				void PopPointer(T *ptr, size_t cnt = 1)
				{
					static_assert(std::tr1::is_pod<T>::value);

					const size_t len = sizeof(T) * cnt;
					assert(len + outPos_ < Impl()->BufferLength());
					if( len + outPos_ >= Impl()->BufferLength() )
						throw std::out_of_range("sizeof(T) + pos_ >= bufLen_");

					pointer buf = reinterpret_cast<pointer>(ptr);
					Impl()->Read(buf, len, outPos_);
					outPos_ += len;
				}

			private:
				ImplT *Impl()
				{
					return static_cast<ImplT *>(this);
				}
			};
		}
	}
}



#endif