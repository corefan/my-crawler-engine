#ifndef __FILESYSTEM_FILE_HPP
#define __FILESYSTEM_FILE_HPP

#include "../IOCP/Dispatcher.hpp"
#include "../IOCP/Buffer.hpp"
#include "../IOCP/Read.hpp"
#include "../IOCP/Write.hpp"
#include "../IOCP/ReadWriteBuffer.hpp"


namespace async
{
	using namespace iocp;

	namespace filesystem
	{
		// forward declare

		class File;
		typedef std::tr1::shared_ptr<File> FilePtr;
		

		//--------------------------------------------------------------------------------
		// class File

		class File
		{
		public:
			typedef iocp::IODispatcher	AsyncIODispatcherType;

		private:
			// File Handle
			HANDLE file_;
			// IO����
			AsyncIODispatcherType &io_;

		public:
			explicit File(AsyncIODispatcherType &);
			File(AsyncIODispatcherType &, LPCTSTR, DWORD, DWORD, DWORD, DWORD, LPSECURITY_ATTRIBUTES = NULL, HANDLE = NULL);
			~File();

			// non-copyable
		private:
			File(const File &);
			File &operator=(const File &);


		public:
			// explicitת��
			operator HANDLE()					{ return file_; }
			operator const HANDLE () const		{ return file_; }

			// ��ʾ��ȡ
			HANDLE GetHandle()					{ return file_; }
			const HANDLE GetHandle() const		{ return file_; }

		public:
			// ��Ŀ���ļ�
			void Open(LPCTSTR, DWORD, DWORD, DWORD, DWORD, LPSECURITY_ATTRIBUTES = NULL, HANDLE = NULL);
			// �ر�
			void Close();
			
			// �Ƿ��
			bool IsOpen() const
			{ return file_ != INVALID_HANDLE_VALUE; }

			// ˢ��
			bool Flush();
			
			//	ȡ��
			bool Cancel();

			// �����ļ���С
			void SetFileSize(unsigned long long size);

			// �������ûص��ӿ�,ͬ������
		public:
			size_t Read(void *buf, size_t len, const u_int64 &offset);
			size_t Write(const void *buf, size_t len, const u_int64 &offset);

			// �첽���ýӿ�
		public:
			void AsyncRead(void *buf, size_t len, const u_int64 &offset, const CallbackType &handler);
			void AsyncWrite(const void *buf, size_t len, const u_int64 &offset, const CallbackType &handler);
		};
	}



	namespace iocp
	{
		typedef filesystem::File File;

		// File ����
		template<>
		struct ObjectFactory< File >
		{
			typedef memory::FixedMemoryPool<true, sizeof(File)>		PoolType;
			typedef ObjectPool< PoolType >							ObjectPoolType;
		};
	}


	namespace filesystem
	{
		inline FilePtr MakeFile(File::AsyncIODispatcherType &io)
		{
			return FilePtr(ObjectAllocate<File>(io), &ObjectDeallocate<File>);
		}

		inline FilePtr MakeFile(File::AsyncIODispatcherType &io, LPCTSTR path, DWORD access, DWORD shared, DWORD create, DWORD flag, LPSECURITY_ATTRIBUTES attribute = NULL, HANDLE templateMode = NULL)
		{
			return FilePtr(ObjectAllocate<File>(io, path, access, shared, create, flag, attribute, templateMode), &ObjectDeallocate<File>);
		}
	}

	
}




#endif