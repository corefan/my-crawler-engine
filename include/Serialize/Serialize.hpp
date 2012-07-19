#ifndef __SERIALIZE_HPP
#define __SERIALIZE_HPP

#include <cassert>

#include "detail/Dispatch.hpp"
#include "detail/Buffer.hpp"
#include "detail/SerializeIn.hpp"
#include "detail/SerializeOut.hpp"




namespace utility
{
	namespace serialize
	{
		
		// ------------------------------------
		// CharT 缓冲类型
		// OutT  输出目标
		
		template < typename CharT, typename OutT >
		class SerializeT
			: public detail::SerializeIn<CharT, SerializeT<CharT, OutT> >
			, public detail::SerializeOut<CharT, SerializeT<CharT, OutT> >
		{
		public:
			typedef typename OutT::value_type		value_type;
			typedef typename OutT::pointer			pointer;
			typedef typename OutT::reference		reference;
			typedef typename OutT::const_pointer	const_pointer;
			typedef typename OutT::const_reference	const_reference;

		private:
			typedef detail::SerializeIn<CharT, SerializeT<CharT, OutT> >	InType;
			typedef detail::SerializeOut<CharT, SerializeT<CharT, OutT> >	OutType;
			typedef OutT													BufferType;
			
		protected:
			BufferType buffer_;

		public:
			template < typename Arg1>
			SerializeT(Arg1 arg1)
				: buffer_(arg1)
			{}

			template < typename Arg1, typename Arg2 >
			SerializeT(Arg1 arg1, Arg2 arg2)
				: buffer_(arg1, arg2)
			{}

			template < typename Arg1, typename Arg2, typename Arg3 >
			SerializeT(Arg1 arg1, Arg2 arg2, Arg3 arg3)
				: buffer_(arg1, arg2, arg3)
			{}

			template < size_t N >
			explicit SerializeT(value_type (&buf)[N])
				: buffer_(buf)
			{}

		public:
			pointer Buffer()
			{
				return buffer_.Buffer();
			}

			const_pointer Buffer() const
			{
				return buffer_.Buffer();
			}

			size_t BufferLength() const
			{
				return buffer_.Length();
			}

			size_t DataLength() const
			{
				return buffer_.DataLength();
			}

			void Read(pointer buf, size_t len, size_t pos)
			{
				buffer_.Read(buf, len, pos);
			}
			
			void Write(const_pointer buf, size_t len, size_t pos)
			{
				buffer_.Write(buf, len, pos);
			}
		};

		typedef SerializeT<char,	detail::Memory<char> >		Serialize;
		typedef SerializeT<wchar_t, detail::Memory<wchar_t> >	SerializeW;

		typedef SerializeT<char,	detail::File<char> >		FileSerialize;
		typedef SerializeT<wchar_t, detail::File<wchar_t> >		FileSerializeW;
		


		// -------------------------------------------------

		template < typename CharT, typename OutT, typename T >
		inline SerializeT<CharT, OutT> &operator<<(SerializeT<CharT, OutT> &os, const T &val)
		{
			detail::SelectPointer<T>::PushDispacth(os, val);
			return os;
		}

		template < typename CharT, typename OutT, typename T, size_t N >
		inline SerializeT<CharT, OutT> &operator<<(SerializeT<CharT, OutT> &os, const T (&val)[N])
		{
			detail::SelectArray<T>::PushDispacth(os, val);
			return os;
		}

		template < typename CharT, typename OutT, typename StringT >
		inline SerializeT<CharT, OutT> &operator<<(SerializeT<CharT, OutT> &os, const std::basic_string<StringT> &val)
		{
			detail::PushString(os, val);
			return os;
		}

		template < typename CharT, typename OutT, typename T >
		inline SerializeT<CharT, OutT> &operator>>(SerializeT<CharT, OutT> &os, T &val)
		{
			detail::SelectPointer<T>::PopDispacth(os, val);
			return os;
		}

		template < typename CharT, typename OutT, typename T, size_t N >
		inline SerializeT<CharT, OutT> &operator>>(SerializeT<CharT, OutT> &os, T (&val)[N])
		{
			detail::SelectArray<T>::PopDispacth(os, val);
			return os;
		}

		template < typename CharT, typename OutT, typename StringT >
		inline SerializeT<CharT, OutT> &operator>>(SerializeT<CharT, OutT> &os, std::basic_string<StringT> &val)
		{
			detail::PopString(os, val);
			return os;
		}
	}
}




#endif