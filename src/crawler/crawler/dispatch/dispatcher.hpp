#ifndef __CRAWLER_DISPATCHER_HPP
#define __CRAWLER_DISPATCHER_HPP

#include "common.hpp"


namespace crawler
{
	namespace dispatcher
	{

		class mgr
		{
		private:
			struct impl;
			std::auto_ptr<impl> impl_;

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
	}
}



#endif