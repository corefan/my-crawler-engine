#ifndef __MMAP_MAP_FILE_HPP
#define __MMAP_MAP_FILE_HPP

#include <cassert>
#include <string>


namespace mmap
{
	// -------------------------------------------------------------------------
	// struct mapping_readwrite

	struct mapping_readwrite
	{
		static const size_t PageProtect				= PAGE_READWRITE;
		static const size_t PageAccess				= FILE_MAP_READ | FILE_MAP_WRITE;
		static const size_t FileDesireAccess		= GENERIC_WRITE | GENERIC_READ;
		static const size_t FileShareMode			= FILE_SHARE_READ;
		static const size_t FileCreationDisposition	= OPEN_ALWAYS;
		static const size_t FileFlagsAndAttributes	= FILE_FLAG_WRITE_THROUGH | FILE_ATTRIBUTE_ARCHIVE;
		static const size_t GetSizeOnOpen			= FALSE;
	};

	// -------------------------------------------------------------------------
	// struct mapping_readonly

	struct mapping_readonly
	{
		static const size_t PageProtect				= PAGE_READONLY;
		static const size_t PageAccess				= FILE_MAP_READ;
		static const size_t FileDesireAccess		= GENERIC_READ;
		static const size_t FileShareMode			= FILE_SHARE_READ | FILE_SHARE_WRITE;
		static const size_t FileCreationDisposition	= OPEN_EXISTING;
		static const size_t FileFlagsAndAttributes	= FILE_FLAG_SEQUENTIAL_SCAN;
		static const size_t GetSizeOnOpen			= TRUE;
	};


	// -------------------------------------------------------------------------
	// class map_file_t
	// 提供对File Mapping的直接操作

	template< typename ConfigT >
	class map_file_t
	{
	public:
		typedef size_t		size_type;
		typedef UINT64		pos_type;
		typedef ConfigT		ConfigType;

	private:
		HANDLE hFile_;
		HANDLE hFileMapping_;

	public:
		map_file_t()
			: hFile_(INVALID_HANDLE_VALUE)
			, hFileMapping_(NULL)
		{}
 
		template< typename CharT >
		explicit map_file_t(const std::basic_string<CharT> &szFile, pos_type *offset = 0)
			: hFile_(INVALID_HANDLE_VALUE)
			, hFileMapping_(NULL)
		{
			open(szFile, offset);
		}

		~map_file_t()
		{
			close();
		}

	public:
		template < typename CharT >
		bool open(const std::basic_string<CharT> &szFile, pos_type *offset = 0)
		{
			if( good() )
				return false;

			hFile_ = _CreateFileImpl(szFile);
			if( hFile_ == INVALID_HANDLE_VALUE )
				return false;

			if( ConfigType::GetSizeOnOpen )
			{
				DWORD dwFileSizeHigh = 0;
				DWORD dwFileSize = ::GetFileSize(hFile_, &dwFileSizeHigh);

				hFileMapping_ = ::CreateFileMapping(hFile_, 
					NULL, 
					ConfigType::PageProtect,
					dwFileSizeHigh,
					dwFileSize,
					NULL);

				if( offset != 0 )
					*offset = (static_cast<pos_type>(dwFileSizeHigh) << 32) | dwFileSize;
			}
			else
			{
				hFileMapping_ = NULL;
			}

			return true;
		}

		void close()
		{
			if( hFile_ != INVALID_HANDLE_VALUE )
			{
				if( hFileMapping_ != NULL )
				{
					::CloseHandle(hFileMapping_);
					hFileMapping_ = NULL;
				}

				::CloseHandle(hFile_);
				hFile_ = INVALID_HANDLE_VALUE;
			}
		}


		bool good() const
		{
			return hFile_ != INVALID_HANDLE_VALUE;
		}


		bool resize(pos_type newSize)
		{
			if( hFileMapping_ != NULL )
				assert(::CloseHandle(hFileMapping_));

			hFileMapping_ = ::CreateFileMapping(hFile_, NULL,
				ConfigType::PageProtect, 
				static_cast<DWORD>(newSize >> 32), 
				static_cast<DWORD>(newSize), 
				NULL);

			return hFileMapping_ ? true : false;
		}

		pos_type size() const
		{
			LARGE_INTEGER len = {0};
			BOOL suc = ::GetFileSizeEx(hFile_, &len);
			assert(suc);

			return len.QuadPart;
		}

		void *map(pos_type offset, pos_type size, void *pBaseAddr = NULL)
		{
			return ::MapViewOfFileEx(hFileMapping_, 
				ConfigType::PageAccess,
				static_cast<DWORD>(offset >> 32),
				static_cast<DWORD>(offset),
				static_cast<size_type>(size),
				pBaseAddr);
		}

		static void unmap(void *p, size_type/* size*/)
		{
			::UnmapViewOfFile(p);
		}

		static bool flush(void *p, size_type size)
		{
			return ::FlushViewOfFile(p, size) ? true : false;
		}


	private:
		HANDLE _CreateFileImpl(const std::string &path)
		{
			return ::CreateFileA(path.c_str(), 
				ConfigType::FileDesireAccess, 
				ConfigType::FileShareMode,
				NULL,
				ConfigType::FileCreationDisposition,
				ConfigType::FileFlagsAndAttributes,
				NULL);
		}

		HANDLE _CreateFileImpl(const std::wstring &path)
		{
			return ::CreateFileW(path.c_str(), 
				ConfigType::FileDesireAccess, 
				ConfigType::FileShareMode,
				NULL,
				ConfigType::FileCreationDisposition,
				ConfigType::FileFlagsAndAttributes,
				NULL);
		}
	};


	typedef map_file_t<mapping_readonly>	map_file_readonly;
	typedef map_file_t<mapping_readwrite>	map_file_readwrite;
}

#endif