#include "FileMonitor.hpp"


namespace async
{
	namespace filesystem
	{

		FileSystemMonitor::FileSystemMonitor(iocp::IODispatcher &io, DWORD filter)
			: io_(io)
			, file_(INVALID_HANDLE_VALUE)
			, filter_(filter)
		{}

		FileSystemMonitor::FileSystemMonitor(iocp::IODispatcher &io, LPCTSTR path, DWORD filter)
			: io_(io)
			, file_(INVALID_HANDLE_VALUE)
			, filter_(filter)
		{
			Open(path);
		}
		FileSystemMonitor::~FileSystemMonitor()
		{
			Close();
		}


		void FileSystemMonitor::Open(LPCTSTR path)
		{
			assert(!IsValid());
			if( IsValid() )
				return;

			file_ = ::CreateFile(path,  FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, 0);
			if( file_ == INVALID_HANDLE_VALUE )
				throw iocp::Win32Exception("CreateFile");

			io_.Bind(file_);
		}

		void FileSystemMonitor::Close()
		{
			if( IsValid() )
			{
				::CloseHandle(file_);
				file_ = INVALID_HANDLE_VALUE;
			}
		}

	}
}