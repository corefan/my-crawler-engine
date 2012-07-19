#ifndef __FILE_SYSTEM_FILE_CHANGE_HOOK_HPP
#define __FILE_SYSTEM_FILE_CHANGE_HOOK_HPP


namespace async
{
	namespace filesystem
	{
		class FileSystemMonitor;

		namespace detail
		{
			// Hook File Change Callback
			template< typename HandlerT >
			struct FileChangeHandle
			{	
				FileSystemMonitor &fileChange_;
				HandlerT handler_;

				FileChangeHandle(FileSystemMonitor &fileChange, const HandlerT &handler)
					: fileChange_(fileChange)
					, handler_(handler)
				{}

			public:
				void operator()(u_long size, u_long error, FILE_NOTIFY_INFORMATION *notify)
				{
					if( size == 0 )
						return;

					fileChange_.Monitor(handler_);

					for(;;)
					{
						handler_(error, notify->Action, notify->FileName, notify->FileNameLength / sizeof(wchar_t));

						if( notify->NextEntryOffset == 0 )
							break;

						char *tmp = reinterpret_cast<char *>(notify) + notify->NextEntryOffset;
						notify = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(tmp);
					};
				}
			};
		}
	}
}





#endif