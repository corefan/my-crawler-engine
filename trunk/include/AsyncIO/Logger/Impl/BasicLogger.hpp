#ifndef __LOGGER_BASIC_LOGGER_HPP
#define __LOGGER_BASIC_LOGGER_HPP

#include "../../iocp/Object.hpp"


namespace async
{
	namespace logger
	{
		namespace impl
		{
			// --------------------------------------------------
			// class BasicLogger

			template<typename ServiceT>
			class BasicLogger
			{
				typedef ServiceT								ServiceType;
				typedef typename ServiceType::AsyncServiceType	AsyncServiceType;
				typedef typename ServiceType::LoggerImplType	LoggerImplType;

			private:
				ServiceType &service_;
				LoggerImplType impl_;

			public:
				explicit BasicLogger(AsyncServiceType &io, const std::string &id)
					: service_(ServiceType::GetInstance(io))
					, impl_(service_.Null())
				{
					service_.Create(impl_, id);
				}
				~BasicLogger()
				{
					service_.Destroy(impl_);
				}
		
			public:
				// 设置日志文件
				void UseFile(const std::string &file)
				{
					service_.UseFile(impl_, file);
				}
				// 记录消息
				void Log(const std::string &msg)
				{
					service_.Log(impl_, msg);
				}
			};
		}
	}
}

#endif