#ifndef __FILESYSTEM_FILE_CHANGE_HPP
#define __FILESYSTEM_FILE_CHANGE_HPP

#include "../IOCP/Dispatcher.hpp"
#include "Detail/FileChangeHook.hpp"
#include "Detail/AsyncCallback.hpp"


namespace async
{

	namespace filesystem
	{
	

		// -----------------------------------------
		// class FileChange

		class FileSystemMonitor
		{
		private:
			// File Handle
			HANDLE file_;
			DWORD filter_;

			// IO服务
			iocp::IODispatcher &io_;

		public:
			explicit FileSystemMonitor(iocp::IODispatcher &, 
				DWORD filter = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
				FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS |
				FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY);
			FileSystemMonitor(iocp::IODispatcher &, LPCTSTR,
				DWORD filter = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
				FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS |
				FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY);
			~FileSystemMonitor();

			// non-copyable
		private:
			FileSystemMonitor(const FileSystemMonitor &);
			FileSystemMonitor &operator=(const FileSystemMonitor &);


		public:
			// explicit转换
			operator HANDLE()					{ return file_; }
			operator const HANDLE () const		{ return file_; }

			// 显示获取
			HANDLE GetHandle()					{ return file_; }
			const HANDLE GetHandle() const		{ return file_; }

		public:
			void Open(LPCTSTR path);

			void Close();

			bool IsValid() const
			{
				return file_ != INVALID_HANDLE_VALUE;
			}

		public:
			template < typename HandlerT >
			void Monitor(const HandlerT &handler, bool sub_dir = true)
			{
				if( !IsValid() )
					throw std::logic_error("File Path Not Valid");

				typedef detail::FileChangeHandle<HandlerT> FileChangeHandleHook;
				iocp::FileChangeCallbackPtr asynResult(iocp::MakeAsyncCallback(FileChangeHandleHook(*this, handler)));
				
				DWORD ret = 0;
				BOOL suc = ::ReadDirectoryChangesW(file_, &asynResult->buffer_, iocp::FileChangeCallback::BUFFER_LEN, sub_dir ? TRUE : FALSE, 
					filter_, &ret, asynResult.Get(), 0);
				
				if( !suc )
					throw iocp::Win32Exception("ReadDirectoryChangesW");

				asynResult.Release();
			}
		};
	}
}




#endif