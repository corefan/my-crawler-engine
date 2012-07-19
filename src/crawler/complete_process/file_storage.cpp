#include "file_storage.hpp"

#include <cassert>
#include <fstream>

#include "Utility/utility.h"
#include "Unicode/string.hpp"
#include "Win32/FileSystem/filesystem.hpp"


namespace crawler
{
	namespace storage
	{
	
		namespace
		{
			const std::wstring directory_name = L"files";
			const std::wstring path = utility::GetAppPath() + directory_name;


			void replace(std::wstring &val)
			{
				std::replace_if(val.begin(), val.end(), [](wchar_t c)->bool
				{
					if( c == '\\' || c == '/' || 
						c == '*' || c == '?' ||
						c == ':' || c == '|' ||
						c == '<' || c == '>' ||
						c == '\"'|| c == '\'' ||
						c == '.' )
						return true;
					return false;
				}, '_');
			}
		}

		void file::start()
		{
			static const win32::file::wpath dir(path);

			if( !win32::file::is_directory(dir) )
				win32::file::create_directories(dir);
		}
		
		void file::stop()
		{

		}

		void file::save(const url_ptr &url, const buffer_type &buffer, const handle_error_type &handle_error)
		{
			const std::wstring &ori_url = url->get_url();


			// construct domain 
			size_t start_pos = ori_url.find(L'.') + 1;
			assert(start_pos != std::wstring::npos);
			size_t stop_pos = ori_url.find(L'/', start_pos);
			if( stop_pos == std::wstring::npos )
				stop_pos = ori_url.length();

			std::wstring file_folder = ori_url.substr(start_pos, stop_pos - start_pos);
			replace(file_folder);

			// construct folder
			win32::file::wpath foler_path(path + L"/" + file_folder);
			if( !win32::file::is_directory(foler_path) )
				win32::file::create_directories(foler_path);

			// construct file
			size_t f_start = ori_url.find(L'/', start_pos) + 1;
			assert(start_pos != std::wstring::npos);
			size_t f_stop = ori_url.find(L'?', f_start);
			f_stop = f_stop == std::wstring::npos ? ori_url.length() : f_stop;
			std::wstring file_name = ori_url.substr(f_start, f_stop);
			replace(file_name);
			file_name += L".html";

			std::wstring file_path = foler_path / win32::file::wpath(file_name);
			std::ofstream out(file_path.c_str(), std::ios::binary);
			if( !out )
			{
				std::wostringstream os;
				os << L"open file " << file_path << L" error" << std::endl;

				handle_error(unicode::to_a(os.str()));
			}
			else
			{
				out.write(buffer.first.get(), buffer.second);
			}

		}
	}
}