#ifndef __FILESYSTEM_BASIC_FILE_HPP
#define __FILESYSTEM_BASIC_FILE_HPP

#include "File.hpp"
#include "../iocp/ReadWriteBuffer.hpp"
#include "../iocp/Write.hpp"
#include "../iocp/Read.hpp"


namespace async
{
	namespace filesystem
	{

		// -------------------------------------------
		// class BasicFile

		class BasicFile
		{
		public:
			typedef FilePtr							ImplementType;
			typedef File::AsyncIODispatcherType		AsyncIODispatcherType;	

		private:
			ImplementType impl_;

		public:
			explicit BasicFile(AsyncIODispatcherType &io)
				: impl_(MakeFile(io))
			{}
			explicit BasicFile(const ImplementType &impl)
				: impl_(impl)
			{}
			BasicFile(AsyncIODispatcherType &io, LPCTSTR path, DWORD access, DWORD sharedMode, DWORD createMode, DWORD flag, LPSECURITY_ATTRIBUTES attribute = NULL, HANDLE templateMode = NULL)
				: impl_(MakeFile(io, path, access, sharedMode, createMode, flag, attribute, templateMode))
			{}

		public:
			// 显示获取
			ImplementType &Get() 
			{
				return impl_;
			}
			const ImplementType &Get() const
			{
				return impl_;
			}

			// 支持隐式转换
			operator ImplementType()
			{
				return impl_;
			}
			operator const ImplementType&() const
			{
				return impl_;
			}

		public:
			void Open(LPCTSTR path, DWORD access, DWORD sharedMode, DWORD createMode, DWORD flag, LPSECURITY_ATTRIBUTES attribute = NULL, HANDLE templateMode = NULL)
			{
				return impl_->Open(path, access, sharedMode, createMode, flag, attribute, templateMode);
			}

			void Close()
			{
				return impl_->Close();
			}

			bool IsOpen() const
			{
				return impl_->IsOpen();
			}
			
			bool Flush()
			{
				return impl_->Flush();
			}

			bool Cancel()
			{
				return impl_->Cancel();
			}

			void SetFileSize(unsigned long long size)
			{
				return impl_->SetFileSize(size);
			}

		public:
			// 阻塞式发送数据直到数据发送成功或出错
			template<typename ConstBufferT>
			size_t Write(const ConstBufferT &buffer)
			{
				return impl_->Write(buffer.data(), buffer.size(), 0);
			}

			template<typename ConstBufferT>
			size_t Write(const ConstBufferT &buffer, const u_int64 &offset)
			{
				return impl_->Write(buffer.data(), buffer.size(), offset);
			}

			// 异步发送数据
			template<typename ConstBufferT, typename HandlerT>
			void AsyncWrite(const ConstBufferT &buffer, const HandlerT &callback)
			{
				return impl_->AsyncWrite(buffer.data(), buffer.size(), 0, callback);
			}

			template<typename ConstBufferT, typename HandlerT>
			void AsyncWrite(const ConstBufferT &buffer, const u_int64 &offset, const HandlerT &callback)
			{
				return impl_->AsyncWrite(buffer.data(), buffer.size(), offset, callback);
			}


			// 阻塞式接收数据直到成功或出错
			template<typename MutableBufferT>
			size_t Read(MutableBufferT &buffer)
			{
				return impl_->Read(buffer.data(), buffer.size(), 0);
			}

			template<typename MutableBufferT>
			size_t Read(MutableBufferT &buffer, const u_int64 &offset)
			{
				return impl_->Read(buffer.data(), buffer.size(), offset);
			}

			// 异步接收数据
			template<typename MutableBufferT, typename HandlerT>
			void AsyncRead(MutableBufferT &buffer, const HandlerT &callback)
			{
				return impl_->AsyncRead(buffer.data(), buffer.size(), 0, callback);
			}

			// 异步接收数据
			template<typename MutableBufferT, typename HandlerT>
			void AsyncRead(MutableBufferT &buffer, const u_int64 &offset, const HandlerT &callback)
			{
				return impl_->AsyncRead(buffer.data(), buffer.size(), offset, callback);
			}
		};
	}
}



#endif