#ifndef __NETWORK_ACCEPT_HPP
#define __NETWORK_ACCEPT_HPP



namespace async
{
	namespace iocp
	{
		static std::tr1::_Ph<2> _Socket;
	}
	
	namespace network
	{
		namespace detail
		{
		
			// Hook User Accept Callback
			template<typename HandlerT>
			struct AcceptorHandle
			{	
				Socket &acceptor_;
				SocketPtr remoteSocket_;
				iocp::AutoBufferPtr buf_;
				HandlerT handler_;

				AcceptorHandle(Socket &acceptor, const SocketPtr &remoteSocket, const iocp::AutoBufferPtr &buf, const HandlerT &handler)
					: acceptor_(acceptor)
					, remoteSocket_(remoteSocket)
					, buf_(buf)
					, handler_(handler)
				{}

			public:
				void operator()(u_long size, u_long error)
				{
					// ¸´ÖÆListen socketÊôÐÔ
					UpdateAcceptContext context(acceptor_);
					remoteSocket_->SetOption(context);

					handler_(/*size, */error, std::tr1::cref(remoteSocket_));
				}
			};
		}

	}

}



#endif