#ifndef __SERAILIZE_MEMORY_BUFFER_HPP
#define __SERAILIZE_MEMORY_BUFFER_HPP


#include <fstream>

#pragma warning(disable: 4996)

#ifdef max
#undef max
#endif


namespace utility
{
	namespace serialize
	{
		namespace detail
		{

			template < typename CharT >
			class Memory
			{
			public:
				typedef CharT				value_type;
				typedef CharT *				pointer;
				typedef value_type &		reference;
				typedef const CharT *		const_pointer;
				typedef const value_type &	const_reference;

			private:
				pointer buf_;
				const size_t bufLen_;

			public:
				explicit Memory(pointer buf)
					: buf_(buf)
					, bufLen_(std::numeric_limits<size_t>::max())
				{}

				Memory(pointer buf, size_t len)
					: buf_(buf)
					, bufLen_(len)
				{}

				template < size_t N >
				explicit Memory(value_type (&buf)[N])
					: buf_(buf)
					, bufLen_(N)
				{}

			public:
				const_pointer Buffer() const
				{
					return buf_;
				}

				size_t Length() const
				{
					return bufLen_;
				}

				void Read(pointer buf, size_t len, size_t pos) const
				{
					//std::copy(buf_ + pos, buf_ + pos + len, buf);
					::memmove(buf, buf_ + pos, len);
				}

				void Write(const_pointer buf, size_t len, size_t pos)
				{
					//std::copy(buf, buf + len, buf_ + pos);
					::memmove(buf_ + pos, buf, len);
				}
			};



			template < typename CharT >
			class File
			{
			public:
				typedef CharT				value_type;
				typedef CharT *				pointer;
				typedef value_type &		reference;
				typedef const CharT *		const_pointer;
				typedef const value_type &	const_reference;

			private:
				std::basic_fstream<CharT> file_;

			public:
				template < typename PathT >
				explicit File(const PathT *path, 
					std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out | std::ios_base::binary,
					int prot = std::ios_base::_Openprot)
				{
					file_.open(path, mode, prot);
					assert(file_.good());
				}


			public:
				/*const_pointer Buffer() const
				{
					return file_.rdbuf()->;
				}*/

				size_t Length() const
				{
					return std::numeric_limits<size_t>::max();
				}

				void Read(pointer buf, size_t len, size_t pos)
				{
					assert(file_.good());
					if( !file_.good() )
						throw std::runtime_error("file not good");

					file_.seekp(pos);
					file_.read(buf, len);
				}

				void Write(const_pointer buf, size_t len, size_t pos)
				{
					assert(file_.good());
					if( !file_.good() )
						throw std::runtime_error("file not good");

					file_.seekg(pos);
					file_.write(buf, len);
				}
			};
		}
	}
}


#endif