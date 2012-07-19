#ifndef __FILE_OPERATOR_HPP
#define __FILE_OPERATOR_HPP


#include "FindFile.hpp"
#include <functional>



namespace win32
{
	namespace file
	{

		namespace detail
		{
			// -----------------------
			struct CheckFileTimeT
			{
				enum { DAYS = 7 };
				static const __int64 PerSecDay = (__int64)10000000 * 24 * 60 * 60;

				template<typename FileFindT>
				bool operator()(const FileFindT &filefind)
				{
					// 文件最后写时间
					ULARGE_INTEGER time = {0};
					::memcpy(&time, &filefind.m_fd.ftLastWriteTime, sizeof(ULARGE_INTEGER));

					// 系统当前时间
					ULARGE_INTEGER now = {0};
					SYSTEMTIME st = {0};
					::GetSystemTime(&st);
					FILETIME f = {0};
					::SystemTimeToFileTime(&st, &f);
					::memcpy(&now, &f, sizeof(now));

					// 时间差
					__int64 span = now.QuadPart - time.QuadPart;

					if( span >= PerSecDay * DAYS )
						return true;

					return false;
				}
			};

			struct CheckFileNull
			{
				template<typename FileFindT>
				bool operator()(const FileFindT &filefind)
				{
					return true;
				}

				template<typename FileFindT>
				bool operator()(const FileFindT &filefind, size_t deep)
				{
					return true;
				}
			};

			// -----------------------
			struct DeleteFileT
			{
				void operator()(const stdex::tString &path)
				{
                    ::SetFileAttributes(path.c_str(), FILE_ATTRIBUTE_NORMAL);
					::DeleteFile(path.c_str());
				}
			};

			struct OperateNull
			{
				void operator()(const stdex::tString &path)
				{
					// do nothing
				}
			};
		}
		

		// -----------------------
		// 深度文件搜索

		template< typename CondTraits, typename OPTraits >
		struct FileDepthSearch
		{
			typedef FileDepthSearch<CondTraits, OPTraits> ThisType;

			bool isSubDir_;
			CondTraits &cond_;
			OPTraits &op_;

			FileDepthSearch(CondTraits &cond, OPTraits &op, bool isSubDir = false)
				: cond_(cond)
				, op_(op)
				, isSubDir_(isSubDir)
			{}

			void Run(const stdex::tString &dir)
			{
				filesystem::CFindFile find;

				stdex::tString tmp = dir;
				if( tmp[tmp.length() - 1] != '\\' ||
					tmp[tmp.length() - 1] != '/' )
					tmp += _T("\\");
				tmp += _T("*");

				BOOL bSuc = find.FindFile(tmp.c_str());
				while( bSuc )
				{
					bSuc = find.FindNextFile();
					if( !bSuc )
						break;

					if( find.IsDots() )
						continue;

					if( cond_(find) )
					{
						op_(find.GetFilePath());
					}

					if( isSubDir_ && find.IsDirectory() )
					{
						Run(find.GetFilePath());
					}
				}
			}
		};


		// ------------------------
		// 广度文件搜索

		template < typename CondTraits, typename OPTraits >
		struct FileRangeSearch
		{
			typedef FileRangeSearch<CondTraits, OPTraits> ThisType;

			size_t depth_;
			CondTraits &cond_;
			OPTraits &op_;

			FileRangeSearch(CondTraits &cond, OPTraits &op, size_t depth = 0)
				: cond_(cond)
				, op_(op)
				, depth_(depth)
			{}

			void Run(const stdex::tString &dir)
			{
				_RunImpl(dir, 0);
			}

			void _RunImpl(const stdex::tString &dir, size_t depth)
			{
				if( depth_ != 0 && (depth < 0 || depth > depth_) )
					return;

				std::vector<stdex::tString> curDirectorys;
				filesystem::CFindFile find;

				stdex::tString tmp = dir;
				if( tmp[tmp.length() - 1] != '\\' ||
					tmp[tmp.length() - 1] != '/' )
					tmp += _T("\\");
				tmp += _T("*");

				BOOL bSuc = find.FindFile(tmp.c_str());
				while( bSuc )
				{
					bSuc = find.FindNextFile();
					if( !bSuc )
						break;

					if( find.IsDots() )
						continue;

					if( cond_(find, depth) )
					{
						op_(find.GetFilePath());
					}

					if( find.IsDirectory() )
					{
						curDirectorys.push_back(find.GetFilePath());
					}
				}

				std::for_each(curDirectorys.begin(), curDirectorys.end(), 
					std::tr1::bind(&ThisType::_RunImpl, this, std::tr1::placeholders::_1, depth + 1));
			}
		};

	}
}




#endif