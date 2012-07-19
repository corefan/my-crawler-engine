#include "log.hpp"

#include <fstream>
#include <ctime>

#include <windows.h>
#include "Container/BlockingQueue.hpp"
#include "MultiThread/Thread.hpp"
#include "MultiThread/Lock.hpp"
#include "Utility/utility.h"
#include "win32/filesystem/filesystem.hpp"
#include "Unicode/string.hpp"


namespace async
{
	namespace log
	{
		const std::string &translate(priority prio)
		{
			struct map_2_msg
			{
				std::map<priority, std::string> info_;

				map_2_msg()
				{
					info_[Critical] = "Critical";
					info_[Error]	= "Error";
					info_[Warning]	= "Warning";
					info_[Notice]	= "Notice";
					info_[Info]		= "Info";
					info_[Debug]	= "Debug";
					info_[Trace]	= "Trace";
				}
			};

			static map_2_msg info;
			return info.info_[prio];
		}



		std::string time_2_string(time_t time)
		{
			struct tm tmp = {0};
			::localtime_s(&tmp, &time);

			char buf[32] = {0};
			::sprintf_s(buf, "%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",
				tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday,
				tmp.tm_hour, tmp.tm_min, tmp.tm_sec);

			return buf;
		}


		struct message
		{
			priority prio_;
			time_t time_;
			std::string msg_;

			message()
				: prio_(All)
				, time_(0)
			{}

			message(const std::string &msg, priority prio)
				: prio_(prio)
				, time_(::time(0))
				, msg_(std::move(msg))
			{}

			message(const message &rhs)
				: prio_(rhs.prio_)
				, time_(std::move(rhs.time_))
				, msg_(std::move(rhs.msg_))
			{}

			message &operator=(message &&rhs)
			{
				if( &rhs != this )
				{
					prio_ = rhs.prio_;
					time_ = std::move(rhs.time_);
					msg_ = std::move(rhs.msg_);
				}

				return *this;
			}
		};


		struct mgr::impl
		{
			typedef container::BlockingQueue<message> messages_type;
			messages_type msgs_;

			thread::ThreadImplEx thread_;
			thread::AutoEvent exit_;

			std::ofstream file_;


		public:
			impl()
			{}
			~impl()
			{}

		private:
			impl(const impl &);
			impl &operator=(const impl &);

		public:
			void start()
			{
				exit_.Create(0, TRUE);

				SYSTEMTIME gmt = {0};
				::GetSystemTime(&gmt);
				std::ostringstream os;
				os << unicode::to_a(utility::GetAppPath()) << "Log";
				win32::file::create_directory(win32::file::path(os.str()));
				
				os << "/svr-" << gmt.wYear << gmt.wMonth << gmt.wDay << ".log";
				file_.open(os.str(), std::ios::out | std::ios::app);
				if( !file_.good() )
					throw std::runtime_error("open log file error");

				thread_.RegisterFunc(std::bind(&impl::_thread_impl, this));
				thread_.Start();
			}
			void stop()
			{
				msgs_.Put(message());
				exit_.SetEvent();
				thread_.Stop();

				file_.close();
			}

			void push(const std::string &msg, priority prio)
			{
				msgs_.Put(message(msg, prio));
			}

		private:
			DWORD _thread_impl()
			{
				while(!thread_.IsAborted())
				{
					message msg = msgs_.Get();
					if( exit_.IsSignalled() )
						break;

					if( msg.msg_.empty() )
						continue;

					assert(file_.good());

#ifdef _DEBUG
					if( msg.prio_ < All )
#else
					if( msg.prio_ < DEBUG )
#endif
						file_ << time_2_string(msg.time_) << "(" << translate(msg.prio_) << "): " << msg.msg_ << std::endl;
				}

				return 0;
			}
		};


		mgr::mgr()
			: impl_(new impl)
		{

		}

		mgr::~mgr()
		{

		}

		void mgr::start()
		{
			impl_->start();
		}

		void mgr::stop()
		{
			impl_->stop();
		}



		void async_log(mgr &impl, const std::string &msg, priority prio)
		{
			impl.impl_->push(msg, prio);
		}
	}
}