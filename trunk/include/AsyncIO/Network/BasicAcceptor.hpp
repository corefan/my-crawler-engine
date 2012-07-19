#ifndef __NETWORK_BASIC_ACCEPTOR_HPP
#define __NETWORK_BASIC_ACCEPTOR_HPP



namespace async
{
	namespace network
	{

		// -------------------------------------------------
		// class BasicAcceptor

		template<typename ProtocolT>
		class BasicAcceptor
		{
		public:
			typedef ProtocolT						ProtocolType;
			typedef SocketPtr						ImplementType;
			typedef Socket::AsyncIODispatcherType	AsyncIODispatcherType;	

		private:
			ImplementType impl_;

		public:
			explicit BasicAcceptor(AsyncIODispatcherType &io)
				: impl_(MakeSocket(io))
			{}
			explicit BasicAcceptor(const ImplementType &impl)
				: impl_(impl)
			{}
			BasicAcceptor(AsyncIODispatcherType &io, const ProtocolType &protocol)
				: impl_(MakeSocket(io, protocol.Family(), protocol.Type(), protocol.Protocol()))
			{}
			BasicAcceptor(AsyncIODispatcherType &io, const ProtocolType &protocol, u_short port, const IPAddress &addr = INADDR_ANY, bool reuseAddr = true)
				: impl_(MakeSocket(io, protocol.Family(), protocol.Type(), protocol.Protocol()))
			{
				if( reuseAddr )
					impl_->SetOption(ReuseAddr(true));

				Bind(protocol.Family(), port, addr);
				Listen();
			}

		public:
			ImplementType &Get() 
			{
				return impl_;
			}
			const ImplementType &Get() const
			{
				return impl_;
			}

			void Open(const ProtocolType &protocol = ProtocolType::V4())
			{
				if( protocol.Type() == SOCK_STREAM )
					impl_->Open(protocol.Family(), protocol.Type(), protocol.Protocol());
				else
					throw std::logic_error("not Stream socket!");
			}

			bool IsOpen() const
			{
				return impl_->IsOpen();
			}

			void Close()
			{
				return impl_->Close();
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
				// warning: only support AF_INET
				impl_->Bind(family, port, addr);
			}

			void Listen(int backlog = SOMAXCONN)
			{
				impl_->Listen(backlog);
			}

			ImplementType Accept()
			{
				return impl_->Accept();
			}

			template < typename HandlerT >
			void AsyncAccept(size_t szOutSide, const HandlerT &callback)
			{
				return impl_->AsyncAccept(szOutSide, callback);
			}
		};
	}
}







#endif