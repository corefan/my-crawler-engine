#ifndef __NETWORK_BASIC_STREAM_SOCKET_HPP
#define __NETWORK_BASIC_STREAM_SOCKET_HPP


namespace async
{
	namespace network
	{
		// -------------------------------------------------
		// class BasicStreamSocket

		template<typename ProtocolT>
		class BasicStreamSocket
		{
		public:
			typedef ProtocolT						ProtocolType;
			typedef SocketPtr						ImplementType;
			typedef Socket::AsyncIODispatcherType	AsyncIODispatcherType;	

		private:
			ImplementType impl_;

		public:
			explicit BasicStreamSocket(AsyncIODispatcherType &io)
				: impl_(MakeSocket(io))
			{}
			explicit BasicStreamSocket(const ImplementType &impl)
				: impl_(impl)
			{}

			BasicStreamSocket(AsyncIODispatcherType &io, const ProtocolType &protocol)
				: impl_(MakeSocket(io, protocol.Family(), protocol.Type(), protocol.Protocol()))
			{}

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

			void Shutdown(int shut)
			{
				impl_->Shutdown(shut);
			}
			void Close()
			{
				impl_->Shutdown(SD_BOTH);
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
			template < typename HandlerT >
			void AsyncConnect(const IPAddress &addr, u_short port, const HandlerT &handler)
			{
				return impl_->AsyncConnect(addr, port, handler);
			}
			

			// �첽�Ͽ�����
			template < typename HandlerT >
			void AsyncDisconnect(const HandlerT &callback, bool reuse = true)
			{
				return impl_->AsyncDisconnect(callback, reuse);
			}
			

			// ����ʽ��������ֱ�����ݷ��ͳɹ������
			template<typename ConstBufferT>
			size_t Write(const ConstBufferT &buffer)
			{
				return Write(buffer, 0);
			}
			template<typename ConstBufferT>
			size_t Write(const ConstBufferT &buffer, DWORD flag)
			{
				return impl_->Write(buffer, flag);
			}

			// �첽��������
			template<typename ConstBufferT, typename HandlerT >
			void AsyncWrite(const ConstBufferT &buffer, const HandlerT &callback)
			{
				return impl_->AsyncWrite(buffer, callback);
			}

			

			// ����ʽ��������ֱ���ɹ������
			template<typename MutableBufferT>
			size_t Read(MutableBufferT &buffer)
			{
				return Read(buffer, 0);
			}
			template<typename MutableBufferT>
			size_t Read(MutableBufferT &buffer, u_long flag)
			{
				return impl_->Read(buffer.data(), buffer.size(), flag);
			}
	

			// �첽��������
			template<typename MutableBufferT, typename HandlerT >
			void AsyncRead(MutableBufferT &buffer, const HandlerT &callback)
			{
				return impl_->AsyncRead(buffer, callback);
			}

		};
	}
}





#endif