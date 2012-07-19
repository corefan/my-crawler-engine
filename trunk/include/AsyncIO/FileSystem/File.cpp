#include "File.hpp"


namespace  async
{
	namespace filesystem
	{


		File::File(AsyncIODispatcherType &io)
			: file_(INVALID_HANDLE_VALUE)
			, io_(io)
		{}


		File::File(AsyncIODispatcherType &io, LPCTSTR lpszFilePath, DWORD dwAccess, DWORD dwShareMode, 
			DWORD dwCreatePosition, DWORD dwFlag, LPSECURITY_ATTRIBUTES lpAttributes/* = NULL*/, HANDLE hTemplate/* = NULL*/)
			: file_(INVALID_HANDLE_VALUE)
			, io_(io)
		{
			Open(lpszFilePath, dwAccess, dwShareMode, dwCreatePosition, dwFlag, lpAttributes, hTemplate);
		}	

		File::~File()
		{
			Close();
		}


		void File::Open(LPCTSTR lpszFilePath, DWORD dwAccess, DWORD dwShareMode, DWORD dwCreatePosition, DWORD dwFlag, LPSECURITY_ATTRIBUTES attribute /* = NULL */, HANDLE hTemplate /* = NULL */)
		{
			// 创建文件句柄
			file_ = ::CreateFile(lpszFilePath, dwAccess, dwShareMode, attribute, dwCreatePosition, dwFlag, hTemplate);
			if( file_ == INVALID_HANDLE_VALUE )
				throw iocp::Win32Exception("CreateFile");

			// 不触发文件对象 Vista
			//::SetFileCompletionNotificationModes(file_, FILE_SKIP_EVENT_ON_HANDLE);

			if( dwFlag & FILE_FLAG_NO_BUFFERING ||
				dwFlag & FILE_FLAG_OVERLAPPED )
			{
				// 绑定到IOCP
				io_.Bind(file_);
			}
		}


		void File::Close()
		{
			if( file_ != INVALID_HANDLE_VALUE )
			{
				::CloseHandle(file_);
				file_ = INVALID_HANDLE_VALUE;
			}
		}

		bool File::Flush()
		{
			assert(file_ != INVALID_HANDLE_VALUE);
			return ::FlushFileBuffers(file_) == TRUE;
		}

		bool File::Cancel()
		{
			assert(file_ != INVALID_HANDLE_VALUE);
			return ::CancelIo(file_) == TRUE;
		}

		void File::SetFileSize(unsigned long long size)
		{
			LARGE_INTEGER offset = {0};
			offset.QuadPart = size;
			if( !::SetFilePointerEx(file_, offset, 0, FILE_BEGIN) )
				throw iocp::Win32Exception("SetFilePointerEx");

			if( !::SetEndOfFile(file_) )
				throw iocp::Win32Exception("SetEndOfFile");
		}


		size_t File::Read(void *buf, size_t len, const u_int64 &offset)
		{
			if( !IsOpen() )
				throw std::logic_error("File not open");

			LARGE_INTEGER off = {0};
			off.QuadPart = offset;
			if( !::SetFilePointerEx(file_, off, 0, FILE_BEGIN) )
				throw iocp::Win32Exception("SetFilePointerEx");

			DWORD read = 0;
			if( !::ReadFile(file_, buf, len, &read, 0) )
				throw iocp::Win32Exception("ReadFile");

			return read;
		}

		size_t File::Write(const void *buf, size_t len, const u_int64 &offset)
		{
			if( !IsOpen() )
				throw std::logic_error("File not open");

			LARGE_INTEGER off = {0};
			off.QuadPart = offset;
			if( !::SetFilePointerEx(file_, off, 0, FILE_BEGIN) )
				throw iocp::Win32Exception("SetFilePointerEx");

			DWORD read = 0;
			if( !::WriteFile(file_, buf, len, &read, 0) )
				throw iocp::Win32Exception("ReadFile");

			return read;

			return 0;
		}

		void File::AsyncRead(void *buf, size_t len, const u_int64 &offset, const CallbackType &handler)
		{
			if( !IsOpen() )
				throw std::logic_error("File not open");

			AsyncCallbackBasePtr asynResult(iocp::MakeAsyncCallback<iocp::AsyncCallback>(handler));
			asynResult->Offset		= offset & 0xFFFFFFFF;
			asynResult->OffsetHigh	= (offset >> 32) & 0xFFFFFFFF;

			DWORD bytesRead = 0;
			BOOL bSuc = ::ReadFile(file_, buf, len, &bytesRead, asynResult.Get());
			if( !bSuc && ::GetLastError() != ERROR_IO_PENDING )
				throw iocp::Win32Exception("ReadFile");

			asynResult.Release();
		}

		void File::AsyncWrite(const void *buf, size_t len, const u_int64 &offset, const CallbackType &handler)
		{
			if( !IsOpen() )
				throw std::logic_error("File not open");

			AsyncCallbackBasePtr asynResult(iocp::MakeAsyncCallback<iocp::AsyncCallback>(handler));

			asynResult->Offset		= offset & 0xFFFFFFFF;
			asynResult->OffsetHigh	= (offset >> 32) & 0xFFFFFFFF;

			DWORD bytesRead = 0;
			BOOL bSuc = ::WriteFile(file_, buf, len, &bytesRead, asynResult.Get());

			if( !bSuc && ::GetLastError() != ERROR_IO_PENDING )
				throw iocp::Win32Exception("WriteFile");

			asynResult.Release();
		}


		

	}

}