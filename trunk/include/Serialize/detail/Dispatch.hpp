#ifndef __SERIALIZE_DISPATCH_HPP
#define __SERIALIZE_DISPATCH_HPP

#include "../../Utility/StaticSelect.hpp"
#include "ContainerDispatch.hpp"

namespace utility
{
	namespace serialize
	{
		// forward declare
		template < typename CharT, typename OutT >
		class SerializeT;


		namespace detail
		{

			namespace helper
			{
				template < typename CharT, typename OutT, typename T >
				void PushString(SerializeT<CharT, OutT> &os, const T *val)
				{
					const size_t bufLen = std::char_traits<T>::length(val);

					PushString(os, val, bufLen);
				}
				template < typename CharT, typename OutT, typename T >
				void PushString(SerializeT<CharT, OutT> &os, const T *val, size_t bufLen)
				{
					os.Push(bufLen);
					os.PushPointer(val, bufLen);
				}
				template < typename CharT, typename OutT, typename T, size_t N >
				void PushStringArray(SerializeT<CharT, OutT> &os, T (&val)[N])
				{
					PushString(os, val);
				}
				

				template < typename CharT, typename OutT, typename T >
				void PopString(SerializeT<CharT, OutT> &os, T *val)
				{
					size_t bufLen = 0;
					os.Pop(bufLen);
					os.PopPointer(val, bufLen);
				}
				template < typename CharT, typename OutT, typename T, size_t N >
				void PopStringArray(SerializeT<CharT, OutT> &os, T (&val)[N])
				{
					size_t bufLen = 0;
					os.Pop(bufLen);

					assert(bufLen <= N);
					if( bufLen > N )
						throw std::out_of_range("bufLen > N");

					os.PopPointer(val, bufLen);
				}
				
			}


			template < typename CharT, typename OutT, typename T >
			void PushString(SerializeT<CharT, OutT> &os, const std::basic_string<T> &val)
			{
				helper::PushString(os, val.c_str(), val.length());
			}

			template < typename CharT, typename OutT, typename T >
			void PopString(SerializeT<CharT, OutT> &os, std::basic_string<T> &val)
			{
				size_t bufLen = 0;
				os.Pop(bufLen);

				val.resize(bufLen);
				os.PopPointer(const_cast<T *>(val.data()), bufLen);
			}


			template < typename T >
			struct SelectArray
			{
				template < typename CharT, typename OutT, size_t N > 
				static void PushDispacth(SerializeT<CharT, OutT> &os, const T (&val)[N])
				{
					_PushDispacth(os, val, Int2Type<std::tr1::is_pod<T>::value>());
				}

				template < typename CharT, typename OutT, size_t N > 
				static void PopDispacth(SerializeT<CharT, OutT> &os, T (&val)[N])
				{
					_PopDispacth(os, val, Int2Type<std::tr1::is_pod<T>::value>());
				}

				template < typename CharT, typename OutT, size_t N > 
				static void _PushDispacth(SerializeT<CharT, OutT> &os, const T (&val)[N], Int2Type<true>)
				{
					os.PushArray(val);
				}

				template < typename CharT, typename OutT, size_t N > 
				static void _PopDispacth(SerializeT<CharT, OutT> &os, T (&val)[N], Int2Type<true>)
				{
					os.PopArray(val);
				}

				template < typename CharT, typename OutT, size_t N > 
				static void _PushDispacth(SerializeT<CharT, OutT> &os, const T (&val)[N], Int2Type<false>)
				{
					for(size_t i = 0; i != N; ++i)
						os << val[i];
				}

				template < typename CharT, typename OutT, size_t N > 
				static void _PopDispacth(SerializeT<CharT, OutT> &os, T (&val)[N], Int2Type<false>)
				{
					for(size_t i = 0; i != N; ++i)
						os >> val[i];
				}
			};

			template < >
			struct SelectArray< char >
			{
				template < typename CharT, typename OutT, size_t N >
				static void PushDispacth(SerializeT<CharT, OutT> &os, const char (&val)[N])
				{
					helper::PushStringArray(os, val);
				}

				template < typename CharT, typename OutT, size_t N >
				static void PopDispacth(SerializeT<CharT, OutT> &os, char (&val)[N])
				{
					helper::PopStringArray(os, val);
				}
			};

			template < >
			struct SelectArray< wchar_t >
			{
				template < typename CharT, typename OutT, size_t N >
				static void PushDispacth(SerializeT<CharT, OutT> &os, const wchar_t (&val)[N])
				{
					helper::PushStringArray(os, val);
				}

				template < typename CharT, typename OutT, size_t N >
				static void PopDispacth(SerializeT<CharT, OutT> &os, wchar_t (&val)[N])
				{
					helper::PopStringArray(os, val);
				}
			};



			template < typename T >
			struct SelectPointer
			{
				template < typename CharT, typename OutT > 
				static void PushDispacth(SerializeT<CharT, OutT> &os, const T &val)
				{
					_PushDispacth(os, val,
						Int2Type<std::tr1::is_pod<typename std::tr1::remove_reference<T>::type>::value>());
				}

				template < typename CharT, typename OutT > 
				static void PopDispacth(SerializeT<CharT, OutT> &os, T &val)
				{
					_PopDispacth(os, val, 
						Int2Type<std::tr1::is_pod<typename std::tr1::remove_reference<T>::type>::value>());
				}

				template < typename CharT, typename OutT > 
				static void _PushDispacth(SerializeT<CharT, OutT> &os, const T &val, Int2Type<true>)
				{
					os.Push(val);
				}

				template < typename CharT, typename OutT > 
				static void _PopDispacth(SerializeT<CharT, OutT> &os, T &val, Int2Type<true>)
				{
					os.Pop(val);
				}

				template < typename CharT, typename OutT > 
				static void _PushDispacth(SerializeT<CharT, OutT> &os, const T &val, Int2Type<false>)
				{
					ValueTrait<T>::PushDispatch(os, val);
				}

				template < typename CharT, typename OutT > 
				static void _PopDispacth(SerializeT<CharT, OutT> &os, T &val, Int2Type<false>)
				{
					ValueTrait<T>::PopDispatch(os, val);
				}
			};

			template < typename T >
			struct SelectPointer< T * >
			{
				template < typename CharT, typename OutT > 
				static void PushDispacth(SerializeT<CharT, OutT> &os, const T *val)
				{
					os << *val;
				}

				template < typename CharT, typename OutT > 
				static void PopDispacth(SerializeT<CharT, OutT> &os, T *val)
				{
					os.PopPointer(val);
				}
			};

			template < >
			struct SelectPointer< char * >
			{
				template < typename CharT, typename OutT > 
				static void PushDispacth(SerializeT<CharT, OutT> &os, const char *val)
				{
					helper::PushString(os, val);
				}

				template < typename CharT, typename OutT > 
				static void PopDispacth(SerializeT<CharT, OutT> &os, char *val)
				{
					helper::PopString(os, val);
				}
			};

			template < >
			struct SelectPointer< wchar_t * >
			{
				template < typename CharT, typename OutT > 
				static void PushDispacth(SerializeT<CharT, OutT> &os, const wchar_t *val)
				{
					helper::PushString(os, val);
				}

				template < typename CharT, typename OutT > 
				static void PopDispacth(SerializeT<CharT, OutT> &os, wchar_t *val)
				{
					helper::PopString(os, val);
				}
			};


			template < typename T >
			struct SelectPointer< std::tr1::shared_ptr<T> >
			{
				template < typename CharT, typename OutT > 
				static void PushDispacth(SerializeT<CharT, OutT> &os, const std::tr1::shared_ptr<T> &val)
				{
					os << *val;
				}

				template < typename CharT, typename OutT > 
				static void PopDispacth(SerializeT<CharT, OutT> &os, std::tr1::shared_ptr<T> &val)
				{
					val.reset(new T);
					os >> *val;
				}
			};
			
		}
	}
}




#endif