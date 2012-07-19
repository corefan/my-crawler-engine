#ifndef __PIPE_HPP
#define __PIPE_HPP



#include "../IOCP/Dispatcher.hpp"


namespace async
{
	namespace pipe
	{
		using namespace iocp;

		class Pipe;
		typedef std::tr1::shared_ptr<Pipe> PipePtr;

		// --------------------------------
		// class Pipe

		class Pipe
		{
		public:
			typedef iocp::IODispatcher	DispatcherType;

		private:
			// File Handle
			HANDLE pipe_;
			// IO����
			DispatcherType &io_;

		public:
			explicit Pipe(DispatcherType &);
			Pipe(DispatcherType &, HANDLE);
			Pipe(DispatcherType &);
			~Pipe();

			// non-copyable
		private:
			Pipe(const Pipe &);
			Pipe &operator=(const Pipe &);

		public:
			// explicitת��
			operator HANDLE()					{ return pipe_; }
			operator const HANDLE () const		{ return pipe_; }

			// ��ʾ��ȡ
			HANDLE GetHandle()					{ return pipe_; }
			const HANDLE GetHandle() const		{ return pipe_; }

		public:
			// ����
			void Create();

			// ��Ŀ���ļ�
			void Open();
			// �ر�
			void Close();

			// �Ƿ��
			bool IsOpen() const
			{ return pipe_ != INVALID_HANDLE_VALUE; }

			// ����
			void Connect();




			// �������ûص��ӿ�,ͬ������
		public:
			size_t Read(const void *buf, size_t len);
			size_t Write(const void *buf, size_t len);

			void AsyncRead();
			void AsyncWrite();
		};
	}
}


#endif