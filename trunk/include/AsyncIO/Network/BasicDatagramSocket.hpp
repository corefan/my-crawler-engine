#ifndef __NETWORK_DATAGRAM_HPP
#define __NETWORK_DATAGRAM_HPP




namespace async
{
	namespace network
	{

		// ---------------------------------------------------
		// class BasicDatagramSocket

		template<typename ProtocolT>
		class BasicDatagramSocket
		{
		public:
			typedef ProtocolT						ProtocolType;
			typedef SocketPtr						ImplementType;
			typedef Socket::AsyncIODispatcherType	AsyncIODispatcherType;	

		private:
			ImplementType impl_;

		public:
			explicit BasicDatagramSocket(AsyncIODispatcherType &io)
				: impl_(MakeSocket(io))
			{}
			explicit BasicDatagramSocket(const ImplementType &impl)
				: impl_(impl)
			{}

			BasicDatagramSocket(AsyncIODispatcherType &io, const ProtocolType &protocol)
				: impl_(MakeSocket(io, protocol.Family(), protocol.Type(), protocol.Protocol()))
			{}
			BasicDatagramSocket(AsyncIODispatcherType &io, const ProtocolType &protocol, u_short port)
				: impl_(MakeSocket(io, protocol.Family(), protocol.Type(), protocol.Protocol()))
			{
				impl_->Bind(protocol.Family(), port, INADDR_ANY);
			}

		public:
			// ��ʾ��ȡ
			ImplementType &Get() 
			{
				return impl_;
			}
			const ImplementType &Get() const
			{
				return impl_;
			}

			// ֧����ʽת��
			operator ImplementType()
			{
				return impl_;
			}
			operator const ImplementType() const
			{
				return impl_;
			}


		public:
			void Open(const ProtocolType &protocol = ProtocolType::V4())
			{
				impl_->Open(protocol.Family(), protocol.Type(), protocol.Protocol());
			}

			bool IsOpen() const
			{
				return impl_->IsOpen();
			}

			void Close()
			{
				impl_->Close();
			}

			void Cancel()
			{
				return impl_->Cancel();
			}

			template<typename SetSocketOptionT>
			bool SetOption(const SetSocketOptionT &option)
			{
				return impl_->SetOption(option);
			}
			template<typename GetSocketOptionT>
			bool GetOption(GetSocketOptionT &option)
			{
				return impl_->GetOption(option)
			}
			template<typename IOControlCommandT>
			bool IOControl(IOControlCommandT &control)
			{
				return impl_->IOControl(control);
			}

			void Bind(int family, u_short port, const IPAddress &addr)
			{
				impl_->Bind(family, port, addr);
			}


			// ����Զ�̷���
			void Connect(int family, const IPAddress &addr, u_short port)
			{
				impl_->Connect(family, addr, port);
			}

			void DisConnect(int shut = SD_BOTH)
			{
				impl_->DisConnect(shut, true);
			}

			// �첽����
			template<typename HandlerT>
			AsyncCallback *AsyncConnect(const IPAddress &addr, u_short port, const HandlerT &handler)
			{
				return impl_->AsyncConnect(addr, port, handler);
			}
			template<typename AsyncT>
			const AsyncT &AsyncConnect(const AsyncT &result, const IPAddress &addr, u_short uPort)
			{
				return impl_->AsyncConnect(result, addr, uPort);
			}

			
			// ����ʽ��������ֱ�����ݷ��ͳɹ������
			template<typename ConstBufferT>
			size_t SendTo(const ConstBufferT &buffer, const SOCKADDR_IN *addr, u_long flag = 0)
			{
				return impl_->SendTo(buffer, addr, flag);
			}

			// ����ʽ��������ֱ���ɹ������
			template<typename MutableBufferT>
			size_t RecvFrom(MutableBufferT &buffer, SOCKADDR_IN *addr, u_long flag = 0)
			{
				return impl_->RecvFrom(buffer, addr, flag);
			}



			// �첽��������
			template<typename ConstBufferT, typename HandlerT>
			void AsyncSendTo(const ConstBufferT &buffer, const SOCKADDR_IN *addr, const HandlerT &callback)
			{
				return impl_->AsyncSendTo(buffer, addr, callback);
			}

			template<typename MutableBufferT, typename HandlerT>
			void AsyncRecvFrom(MutableBufferT &buffer, SOCKADDR_IN *addr, const HandlerT &callback)
			{
				return impl_->AsyncRecvFrom(buffer, addr, callback);
			}

		};
	}
}




#endif