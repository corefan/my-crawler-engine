#ifndef __CRAWLER_HPP
#define __CRAWLER_HPP

#include <memory>

namespace crawler
{
	class mgr
	{
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


#endif