#ifndef __FILE_SYSTEM_FILE_HELPER_HPP
#define __FILE_SYSTEM_FILE_HELPER_HPP

#include <DbgHelp.h>	// for MakeSureDirectoryPathExists

#include "../../Utility/SmartHandle.hpp"
#include "../../Unicode/string.hpp"
#include "../System/SystemHelper.hpp"

#include "FileOperator.hpp"


#pragma comment(lib, "Dbghelp")


namespace win32
{

	namespace file
	{
		namespace detail
		{
			struct Compare
			{
				template < typename CharT >
				bool operator()(CharT val)
				{
					return val == '/' || val == '\\';
				}
			};

			template < typename T >
			struct Less
			{
				typedef T value_type;
				bool operator()(const value_type &lhs, const value_type &rhs) const
				{
					size_t lCnt = std::count_if(lhs.begin(), lhs.end(), Compare());
					size_t rCnt = std::count_if(rhs.begin(), rhs.end(), Compare());

					return lCnt > rCnt;
				}
			};

			struct CheckFile
			{
				typedef std::vector<stdex::tString> Directorys;
				Directorys directorys_;

				template<typename FileFindT>
				bool operator()(const FileFindT &filefind)
				{
					if( filefind.IsDirectory() )
					{
						stdex::tString path = filefind.GetFilePath();
						directorys_.push_back(path);
						return false;
					}
					else
					{
						::SetFileAttributes(filefind.GetFilePath().c_str(), FILE_ATTRIBUTE_NORMAL);
						return true;
					}
				}

				const Directorys &GetDirectorys() 
				{
					std::sort(directorys_.begin(), directorys_.end(), Less<stdex::tString>());
					return directorys_;
				}
			};
		}

		template < typename CharT, typename SizeT >
		inline bool check_disk_space(const std::basic_string<CharT> &path, SizeT size, std::basic_string<CharT> &info)
		{
			ULARGE_INTEGER FreeBytesAvailableToCaller = {0};
			ULARGE_INTEGER TotalNumberOfBytes = {0};
			ULARGE_INTEGER TotalNumberOfFreeBytes = {0};

			if( unicode::aux::select<CharT>(::GetDiskFreeSpaceExA, ::GetDiskFreeSpaceExW)(path.c_str(),
				&FreeBytesAvailableToCaller,
				&TotalNumberOfBytes,
				&TotalNumberOfFreeBytes) )
			{
				if( FreeBytesAvailableToCaller.QuadPart < size )
				{
					std::basic_ostringstream<CharT> os;
					os << _T("磁盘空间偏低, DIR=") << path << _T(", SPACE=") 
						<< FreeBytesAvailableToCaller.QuadPart/1024.0/1024.0/1024.0 << _T("GB");
					info = os.str();
					return false;
				}
			}
			else 
			{
				std::basic_ostringstream<CharT> os;
				os << _T("检测目录失败, DIR=") << path << _T(", CODE=") << ::GetLastError();
				info = os.str();
				return false;
			}

			return true;
		}


		template < typename CharT >
		inline size_t partition_speed(CharT partition)
		{
			TCHAR device[MAX_PATH] = {0};
			::_stprintf_s(device, _T("\\\\.\\%C:"), partition);

			utility::CAutoFile hFile = ::CreateFile(device, GENERIC_READ, 
				FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if( !hFile.IsValid() )
				return 0;

			LARGE_INTEGER liStart = {0}, liEnd = {0}, Frequency = {0};
			::QueryPerformanceFrequency(&Frequency);

			const size_t bufSize = 64 * 1024;
			char data[bufSize] = {0};
			::srand((unsigned)::time(0));

			::QueryPerformanceCounter(&liStart);
			DWORD dwTotalReadBytes = 0;
			for(size_t idx = 0; idx < 2000; ++idx)
			{
				DWORD dwReadBytes = 0;
				int n = rand() * rand();
				::SetFilePointer(hFile, n, 0, FILE_BEGIN);
				::ReadFile(hFile, data, bufSize, &dwReadBytes, NULL);
				dwTotalReadBytes += dwReadBytes;
			}

			::QueryPerformanceCounter(&liEnd);
			double speed = dwTotalReadBytes * 2000 / 1000.0 / 
				((liEnd.QuadPart - liStart.QuadPart) * 1.0 / Frequency.QuadPart);

			return static_cast<size_t>(speed);
		}


		template < typename CharT >
		inline bool del_directoy(const std::basic_string<CharT> &path)
        {
			// 删除所有文件
			typedef FileDepthSearch<detail::CheckFile, detail::DeleteFileT> DeleteFiles;

			detail::CheckFile checkfile; 
			detail::DeleteFileT deletefile;
			DeleteFiles deleteFiles(checkfile, deletefile, true);

			deleteFiles.Run(unicode::utf(path));

			// 删除目录
			const detail::CheckFile::Directorys &directorys = checkfile.GetDirectorys();

			bool suc = true;
			std::for_each(directorys.begin(), directorys.end(), [&suc](const stdex::tString &tmp)
			{
				if( !unicode::aux::select<CharT>(::RemoveDirectoryA, ::RemoveDirectoryW)(tmp.c_str()) )
					suc = false;
			});


			if( !unicode::aux::select<CharT>(::RemoveDirectoryA, ::RemoveDirectoryW)(unicode::utf(path).c_str()) )
				suc = false;

			return suc;
        }

		template < typename CharT >
		inline bool del_directoy(const CharT *path)
		{
			std::basic_string<CharT> dest_path(path);
			return del_directoy(dest_path);
		}

        
        template < typename CharT >
		inline bool mk_directory(const std::basic_string<CharT> &path)
		{
			if( path.empty() )
				return false;

			// 分隔符必须是backslash
			std::string dest_path = unicode::utf8(path);
			std::replace_if(dest_path.begin(), dest_path.end(), std::bind2nd(std::equal_to<char>(), '/'), '\\');
			
			if( *dest_path.cbegin() != '\\' )
				dest_path += "\\";

			return ::MakeSureDirectoryPathExists(dest_path.c_str()) == TRUE;
		}

		template < typename CharT >
		inline bool mk_directory(const CharT *path)
		{
			std::basic_string<CharT> dest_path(path);
			return mk_directory(dest_path);
		}


		// 检查文件是否存在
		template < typename CharT >
		inline bool is_file_exist(const std::basic_string<CharT> &path)
		{	    
			DWORD att = !unicode::aux::select<CharT>(::GetFileAttributesA, ::GetFileAttributesW)(unicode::utf(path).c_str());
			if( att == INVALID_FILE_ATTRIBUTES )
				return false;

			return !(att & FILE_ATTRIBUTE_DIRECTORY);
		}

		template < typename CharT >
		inline bool is_file_exist(const CharT *path)
		{
			return is_file_exist(std::basic_string<CharT>(path));
		}

		// 检查文件夹是否存在
		template < typename CharT >
		inline bool is_directory_exist(const std::basic_string<CharT> &path)
		{	    
			DWORD att = !unicode::aux::select<CharT>(::GetFileAttributesA, ::GetFileAttributesW)(unicode::utf(path).c_str());
			if( att == INVALID_FILE_ATTRIBUTES )
				return false;

			return !(att & FILE_ATTRIBUTE_DIRECTORY);
		}

		template < typename CharT >
		inline bool is_directory_exist(const CharT *path)
		{
			return is_directory_exist(std::basic_string<CharT>(path));
        }

        template < typename CharT >
        inline std::basic_string<CharT> combine_file_path(const std::basic_string<CharT> &directory, const std::basic_string<CharT> &fileName)
        {
            if( !directory.empty() && 
				*(directory.rbegin()) != '\\' && 
				*(directory.rbegin()) != '/')
            {
                return directory + CharT('\\') + fileName;
            }
            else
            {
                return directory + fileName;
            }
        }

        template < typename CharT >
        inline time_t file_size(const char * const file)
        {
            return file_size(std::basic_string<CharT>(file));
        }

        template < typename CharT >
        inline UINT64 file_size(const std::basic_string<CharT> &filePath)
        {
            utility::CAutoFile file = unicode::aux::select<CharT>(::CreateFileA, ::CreateFileW)(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

            if (!file.IsValid())
            {
                return 0;
            }
			LARGE_INTEGER fileSize = {0};
			BOOL result = ::GetFileSizeEx(file, &fileSize);
            assert(result && "取得文件大小失败");
            return fileSize.QuadPart;
        }

        template < typename CharT >
        inline time_t file_last_write_time(const char * const file)
        {
            return file_last_write_time(std::basic_string<CharT>(file));
        }

        template < typename CharT >
        inline time_t file_last_write_time(const std::basic_string<CharT> &filePath)
        {
            utility::CAutoFile file = unicode::aux::select<CharT>(::CreateFileA, ::CreateFileW)
				(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

            if (!file.IsValid())
            {
                return 0;
            }
			FILETIME fileTime = {0};
			BOOL result = ::GetFileTime(file, NULL, NULL, &fileTime);
            assert(result && "取得文件时间失败");
            return win32::system::FileTimeToTime(fileTime);
        }
	}
}




#endif