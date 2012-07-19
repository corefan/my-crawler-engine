#ifndef __ASYNC_LOG_HPP
#define __ASYNC_LOG_HPP

#include <string>
#include <memory>

namespace async
{

	namespace log
	{
		enum priority
		{ 
			Critical = 0x00,
			Error,
			Warning,
			Notice,
			Info,
			Debug,
			Trace,
			All
		};


		class mgr
		{
			struct impl;
			std::auto_ptr<impl> impl_;

			friend void async_log(mgr &impl, const std::string &msg, priority);

		public:
			mgr();
			~mgr();

		private:
			mgr(const mgr &);
			mgr &operator=(const mgr &);

		public:
			void start();
			void stop();
		};


		

		void async_log(mgr &impl, const std::string &msg, priority prio);
	}
	
	
	
	
}

#endif