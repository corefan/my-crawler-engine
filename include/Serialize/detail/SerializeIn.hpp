#ifndef __SERIALIZE_IN_HPP
#define __SERIALIZE_IN_HPP


#include "../Utility/StaticAssert.hpp"


namespace utility
{
	namespace serialize
	{
		namespace detail
		{
			template < typename CharT, typename ImplT >
			class SerializeIn
			{
			public:
				typedef CharT				value_type;
				typedef CharT *				pointer;
				typedef value_type &		reference;
				typedef const CharT *		const_pointer;
				typedef const value_type &	const_reference;

				typedef std::char_traits<CharT> CharTraits;

			public:
				SerializeIn()
					: inPos_(0)
				{}

			private:
				size_t inPos_;

			public:
				size_t Length() const
				{
					return inPos_;
				}

			public:
				template < typename T >
				void Push(const T &val)
				{
					static_assert(std::tr1::is_pod<T>::value);

					// ºÏ≤‚T¿‡–Õ
					assert(sizeof(T) + inPos_ <= Impl()->BufferLength());
					if( sizeof(T) + inPos_ > Impl()->BufferLength() )
						throw std::out_of_range("sizeof(T) + pos_ > bufLen_");

					const_pointer buf = reinterpret_cast<const_pointer>(&val);
					Impl()->Write(buf, sizeof(T), inPos_);
					inPos_ += sizeof(T);
				}

				template < typename T, size_t N >
				void PushArray(const T (&arr)[N])
				{
					static_assert(std::tr1::is_pod<T>::value);

					const size_t len = sizeof(T) * N;
					assert(len + inPos_ <= Impl()->BufferLength());
					if( len + inPos_ > Impl()->BufferLength() )
						throw std::out_of_range("sizeof(T) + pos_ > bufLen_");

					const_pointer buf = reinterpret_cast<const_pointer>(&arr);
					Impl()->Write(buf, len, inPos_);
					inPos_ += len;
				}

				template < typename T >
				void PushPointer(const T * const ptr, size_t cnt = 1)
				{
					static_assert(std::tr1::is_pod<T>::value);

					const size_t len = sizeof(T) * cnt;
					assert(len + inPos_ <= Impl()->BufferLength());
					if( len + inPos_ > Impl()->BufferLength() )
						throw std::out_of_range("sizeof(T) + pos_ > bufLen_");

					const_pointer buf = reinterpret_cast<const_pointer>(ptr);
					Impl()->Write(buf, len, inPos_);
					inPos_ += len;
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