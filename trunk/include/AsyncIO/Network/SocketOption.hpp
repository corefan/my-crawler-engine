#ifndef __SOCKET_SOCKET_OPTION_HPP
#define __SOCKET_SOCKET_OPTION_HPP

#include <MSTcpIP.h>

namespace async
{
	namespace network
	{




		// ----------------------------------------------
		// ����bool����socket���Եİ�����

		template<int _Level, int _Name>
		class Boolean
		{
		private:
			int value_;

		public:
			explicit Boolean(bool val = true)
				: value_(val ? 1 : 0)
			{}
			Boolean &operator=(const Boolean &rhs)
			{
				value_	 = rhs.value_;
				return *this;
			}	
			Boolean &operator=(bool val)
			{
				value_ = val ? 1 : 0;
			}

		public:
			bool value() const
			{
				return !!value_;
			}

			// bool ת��
			operator bool() const
			{
				return !!value_;
			}

			bool operator!() const
			{
				return !value_;
			}

			// ��ȡSocket levalѡ��ֵ
			int level() const
			{
				return _Level;
			}

			// ��ȡSocket name
			int name() const
			{
				return _Name;
			}	

			// ��ȡSocketֵ
			char *data()
			{
				return reinterpret_cast<char *>(&value_);
			}
			const char *data() const
			{
				return reinterpret_cast<const char *>(&value_);
			}

			// ��ȡֵ��С
			size_t size() const
			{
				return sizeof(value_);
			}


			// ����boolean�Ĵ�С
			void resize(size_t nSize)
			{
				// ��ĳЩƽ̨��getsockopt����һ��sizeof(bool)--1�ֽڡ�	
				// 
				switch(nSize)
				{
				case sizeof(char):
					value_ = *reinterpret_cast<char *>(&value_) ? 1 : 0;
					break;
				case sizeof(value_):
					break;
				default:
					throw std::length_error("bool socket opetion resize");
				}
			}
		};


		// ----------------------------------------------------------
		// ����int����socket���Եİ�����

		template<int _Level, int _Name>
		class Integer
		{
		private:
			int value_;

		public:
			Integer()
				: value_(0)
			{}
			explicit Integer(int val)
				: value_(val)
			{}
			Integer &operator=(const Integer &rhs)
			{
				value_	 = rhs.value_;
				return *this;
			}	
			Integer &operator=(int val)
			{
				value_ = val;
			}

		public:
			bool value() const
			{
				return !!value_;
			}

			// ��ȡSocket levalѡ��ֵ
			int level() const
			{
				return _Level;
			}

			// ��ȡSocket name
			int name() const
			{
				return _Name;
			}	

			// ��ȡSocketֵ
			char *data()
			{
				return reinterpret_cast<char *>(&value_);
			}
			const char *data() const
			{
				return (const char *)&value_;
			}

			// ��ȡֵ��С
			size_t size() const
			{
				return sizeof(value_);
			}


			// ����int�Ĵ�С
			void resize(size_t nSize)
			{
				if( nSize != sizeof(value_) )
					throw std::length_error("Integer socket option resize");
			}
		};


		// -----------------------------------------------------------------
		// class Linger ѡ�����ð�����

		// SO_LINGER���ڿ��Ƶ�δ���͵��������׽������Ŷ�ʱ��һ��ִ����closesocket�󣬸ò�ȡ���ֶ���
		// ���������ʱ����δ���ͺͽ��ܵ����ݶ��ᶪ����ͬʱ����Է�������
		// Ҳ����ͨ������SO_DONTLINGER������

		template< >
		class Integer<SOL_SOCKET, SO_LINGER>
		{
		private:
			::linger value_;


		public:
			Integer()
			{
				value_.l_linger = 0;	// option on/off
				value_.l_onoff	= 0;	// linger time
			}
			Integer(bool bOn, int nTime)
			{
				enabled(bOn);
				timeout(nTime);
			}


		public:
			// �򿪻�ر�lingerѡ��
			void enabled(bool val)
			{
				value_.l_onoff = val ? 1 : 0;
			}

			bool enabled() const
			{
				return value_.l_onoff != 0;
			}

			void timeout(int val)
			{
				value_.l_linger = static_cast<u_short>(val);
			}
			int timeout() const
			{
				return static_cast<int>(value_.l_linger);
			}

			// ��ȡSocket levalѡ��ֵ
			int level() const
			{
				return SOL_SOCKET;
			}

			// ��ȡSocket name
			int name() const
			{
				return SO_LINGER;
			}	

			// ��ȡSocketֵ
			::linger *data()
			{
				return &value_;
			}
			const ::linger *data() const
			{
				return &value_;
			}

			// ��ȡֵ��С
			size_t size() const
			{
				return sizeof(value_);
			}

			// ����int�Ĵ�С
			void resize(size_t nSize)
			{
				if( nSize != sizeof(value_) )
					throw std::length_error("Linger socket option resize");
			}

		};






		typedef u_long IoctlArgType;

		/*template < size_t _CMD >
		class IOCtrl
		{
		private:
			IoctlArgType value_;

		public:
			IOCtrl()
				: value_(0)
			{}
			IOCtrl(bool value)
				: value_(value ? 1 : 0)
			{}

		public:
			int name() const
			{
				return _CMD;
			}

			void set(size_t value)
			{
				value_ = static_cast<IoctlArgType>(value);
			}
			size_t get() const
			{
				return static_cast<size_t>(value_);
			}

			IoctlArgType *data()
			{
				return &value_;
			}

			const IoctlArgType *data() const
			{
				return &value_;
			}
		};*/

		template < size_t _CMD >
		class IOCtrl;

		template <>
		class IOCtrl<FIONBIO>
		{
			u_long enable_;

		public:
			IOCtrl(bool enable)
				: enable_(enable ? 1 : 0)
			{}

			u_long Cmd() const
			{
				return FIONBIO;
			}

			void *InBuffer()
			{
				return &enable_;
			}

			size_t InSize() const
			{
				return sizeof(enable_);
			}

			void *OutBuffer()
			{
				return 0;
			}

			size_t OutSize() const
			{
				return 0;
			}
		};

		template <>
		class IOCtrl<SIO_KEEPALIVE_VALS>
		{
			tcp_keepalive inAlive_;
			tcp_keepalive outAlive_;

		public:
			IOCtrl(u_long inTime)
			{
				if( inTime != 0 )
					inAlive_.onoff = 1;
				else
					inAlive_.onoff = 0;

				inAlive_.keepalivetime = inTime * 1000;
				inAlive_.keepaliveinterval = 2 * 1000;

				inAlive_.onoff = 0;
				outAlive_.keepalivetime = 0;
				outAlive_.keepaliveinterval = 0;
			}

			u_long Cmd() const
			{
				return SIO_KEEPALIVE_VALS;
			}

			void *InBuffer()
			{
				return &inAlive_;
			}

			size_t InSize() const
			{
				return sizeof(inAlive_);
			}

			void *OutBuffer()
			{
				return &outAlive_;
			}

			size_t OutSize() const
			{
				return sizeof(outAlive_);
			}
		};

		template <>
		class IOCtrl<SIO_GET_EXTENSION_FUNCTION_POINTER>
		{
			GUID guid_;
			LPVOID func_;

		public:
			IOCtrl(const GUID &guid, LPVOID func)
				: guid_(guid)
				, func_(func)
			{}

			u_long Cmd() const
			{
				return SIO_GET_EXTENSION_FUNCTION_POINTER;
			}

			void *InBuffer()
			{
				return &guid_;
			}

			size_t InSize() const
			{
				return sizeof(guid_);
			}

			LPVOID OutBuffer()
			{
				return func_;
			}

			size_t OutSize() const
			{
				return sizeof(func_);
			}
		};


		//----------------------------------

		/*
			Example:

				tcp::socket sock(io);
				SocketBase::Broadcast opt(true);
				socket.SetOption(opt);
		*/

		typedef Boolean<SOL_SOCKET, SO_BROADCAST>	Broadcast;
		typedef Boolean<SOL_SOCKET, SO_CONDITIONAL_ACCEPT> ConditionalAccept;
		typedef Boolean<SOL_SOCKET, SO_DEBUG>		Debug;
		typedef Boolean<SOL_SOCKET, SO_DONTROUTE>	DontRoute;
		typedef Boolean<SOL_SOCKET, SO_DONTLINGER>	DontLinger;
		typedef Boolean<SOL_SOCKET, SO_KEEPALIVE>	KeepAlive;
		typedef Boolean<SOL_SOCKET, SO_REUSEADDR>	ReuseAddr;
		typedef Boolean<IPPROTO_TCP, TCP_NODELAY>	NoDelay;

		typedef Integer<SOL_SOCKET, SO_SNDBUF>		SendBufSize;
		typedef Integer<SOL_SOCKET, SO_SNDLOWAT>	SendLowWaterMark;
		typedef Integer<SOL_SOCKET, SO_RCVBUF>		RecvBufSize;
		typedef Integer<SOL_SOCKET, SO_RCVLOWAT>	RecvLowWwaterMark;
		typedef Integer<SOL_SOCKET, SO_SNDTIMEO>	SendTimeOut;
		typedef Integer<SOL_SOCKET, SO_RCVTIMEO>	RecvTimeOut;
		typedef Integer<SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT> UpdateAcceptContext;
		typedef Integer<SOL_SOCKET, SO_LINGER>		Linger;


		typedef IOCtrl<FIONBIO>						NonBlockingIO;
		//typedef IOCtrl<FIONREAD>					BytesReadable;
		typedef IOCtrl<SIO_KEEPALIVE_VALS>			IOKeepAlive;
		typedef IOCtrl<SIO_GET_EXTENSION_FUNCTION_POINTER> GetExtensionFunction;
	}
}





#endif