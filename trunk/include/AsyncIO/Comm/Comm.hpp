#ifndef __COMM_HPP
#define __COMM_HPP

#include "../IOCP/Dispatcher.hpp"
#include "../IOCP/Buffer.hpp"
#include "../IOCP/Read.hpp"
#include "../IOCP/Write.hpp"
#include "../IOCP/ReadWriteBuffer.hpp"


namespace async
{
	using namespace iocp;

	namespace comm
	{
		// forward declare

		class Comm;
		typedef std::tr1::shared_ptr<Comm> CommPtr;


		//--------------------------------------------------------------------------------
		// class File

		class Comm
		{
		public:
			typedef iocp::IODispatcher	AsyncIODispatcherType;

		private:
			// Comm Handle
			HANDLE comm_;
			// IO����
			AsyncIODispatcherType &io_;

		public:
			explicit Comm(AsyncIODispatcherType &);
			Comm(AsyncIODispatcherType &, const std::string &device);
			~Comm();

			// non-copyable
		private:
			Comm(const Comm &);
			Comm &operator=(const Comm &);


		public:
			// explicitת��
			operator HANDLE()					{ return comm_; }
			operator const HANDLE () const		{ return comm_; }

			// ��ʾ��ȡ
			HANDLE GetHandle()					{ return comm_; }
			const HANDLE GetHandle() const		{ return comm_; }

		public:
			// ��Ŀ���ļ�
			void Open(const std::string &device);
			// �ر�
			void Close();

			// �Ƿ��
			bool IsOpen() const
			{ return comm_ != INVALID_HANDLE_VALUE; }

			//	ȡ��
			bool Cancel();

			// ����״̬
			void SetState(const DCB &dcb);

			// ��ȡ״̬
			DCB GetState() const;

			// ���ó�ʱ
			void SetTimeOut();

			// ��ȡ��ʱ
			void GetTimeOut() const;
			

			// ��������
			template < typename OptionT > 
			void SetOption(const OptionT &option);

			// ��ȡ����
			template < typename OptionT >
			void GetOption(OptionT &option);
			
			// �������ûص��ӿ�,ͬ������
		public:
			size_t Read(void *buf, size_t len);
			size_t Write(const void *buf, size_t len);

			// �첽���ýӿ�
		public:
			void AsyncRead(void *buf, size_t len, const CallbackType &handler);
			void AsyncWrite(const void *buf, size_t len, const CallbackType &handler);
		};
	}



	namespace iocp
	{
		typedef comm::Comm Comm;

		// File ����
		template<>
		struct ObjectFactory< Comm >
		{
			typedef memory::FixedMemoryPool<true, sizeof(Comm)>		PoolType;
			typedef ObjectPool< PoolType >							ObjectPoolType;
		};
	}


	namespace comm
	{
		inline CommPtr MakeComm(Comm::AsyncIODispatcherType &io)
		{
			return CommPtr(ObjectAllocate<Comm>(io), &ObjectDeallocate<Comm>);
		}

		inline CommPtr MakeComm(Comm::AsyncIODispatcherType &io, const std::string &device)
		{
			return CommPtr(ObjectAllocate<Comm>(io, device), &ObjectDeallocate<Comm>);
		}
	}


	template < typename OptionT > 
	void Comm::SetOption(const OptionT &option)
	{
		if( !IsOpen() )
			throw std::logic_error("Comm not open");
	
		DCB dcb = GetState();
		option.Store(dcb);

		SetState(dcb);
	}

	// ��ȡ����
	template < typename OptionT >
	void Comm::GetOption(OptionT &option)
	{
		if( !IsOpen() )
			throw std::logic_error("Comm not open");
	
		DCB dcb = GetState();

		option.Load(dcb);
	}
}




#endif