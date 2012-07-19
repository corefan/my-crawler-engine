#ifndef __FILE_ITERATOR_HPP
#define __FILE_ITERATOR_HPP

#include "../Extend STL/UnicodeStl.h"
#include "../Utility/SmartHandle.hpp"
#include <iterator>

namespace async
{
	namespace iterator
	{

		struct fileinfo
		{
			stdex::tString name_;
			unsigned long size_;
			bool isDirectory_;

			fileinfo()
				: isDirectory_(false)
				, size_(0)
			{}

			fileinfo(const WIN32_FIND_DATA& wfd)
				: name_(wfd.cFileName)
				, isDirectory_((FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes) == FILE_ATTRIBUTE_DIRECTORY)
				, size_(wfd.nFileSizeLow)
			{}
		};


		// -----------------------------

		template< typename FileInfoT >
		class basic_file_iterator
			:public std::iterator<std::input_iterator_tag, FileInfoT>
		{
			typedef basic_file_iterator self_type;

		public:
			typedef FileInfoT value_type;

		private:
			bool				end_;
			WIN32_FIND_DATA		wfd_;
			stdex::tString		path_;

			HANDLE				handle_;
			value_type			value_;

			utility::CAutoFindFile refer_;

		public:
			basic_file_iterator()
				: end_(true)
				, handle_(0)
			{}

			basic_file_iterator(const stdex::tString &filPath)
				: end_(false)
				, handle_(0)
			{
				_m_prepare(filPath);
			}

			const value_type &operator*() const 
			{ 
				return value_; 
			}

			const value_type *operator->() const 
			{ 
				return &(operator*()); 
			}

			self_type &operator++()
			{ 
				_m_read(); 
				return *this; 
			}

			self_type operator++(int)
			{
				self_type tmp = *this;
				_m_read();
				return tmp;
			}

			bool equal(const self_type& x) const
			{
				if( end_ && (end_ == x.end_) ) 
					return true;

				return value_.name_ == x.value_.name_;
			}


		private:
			void _m_prepare(const stdex::tString &filePath)
			{
				path_ = filePath;
				HANDLE handle = ::FindFirstFile((filePath + _T("\\*")).c_str(), &wfd_);

				if( handle == INVALID_HANDLE_VALUE )
				{
					end_ = true;
					return;
				}

				if( wfd_.cFileName[0] == '.' )
				{
					::FindNextFile(handle, &wfd_);
					if( !::FindNextFile(handle, &wfd_) )
					{
						end_ = true;
						::FindClose(handle);
						return;
					}
				}

				value_ = value_type(wfd_);

				if( false == end_ )
				{
					refer_ = handle;
					handle_ = handle;
				}
			}

			void _m_read()
			{
				if( handle_ != INVALID_HANDLE_VALUE )
				{
					if( ::FindNextFile(handle_, &wfd_) )
						value_ = value_type(wfd_);
					else
						end_ = true;

				}
			}
		};

		template<typename Value_Type>
		inline bool operator==(const basic_file_iterator<Value_Type> & x, const basic_file_iterator<Value_Type> & y)
		{
			return x.equal(y);
		}

		template<typename Value_Type>
		inline bool operator!=(const basic_file_iterator<Value_Type> & x, const basic_file_iterator<Value_Type> & y)
		{
			return !x.equal(y);
		}


		typedef basic_file_iterator<fileinfo> file_iterator;
	}
}







#endif