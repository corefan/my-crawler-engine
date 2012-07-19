#ifndef __CRAWLER_VISITED_QUEUE_HPP
#define __CRAWLER_VISITED_QUEUE_HPP

#include <memory>


namespace crawler
{
	class url;
	typedef std::shared_ptr<url> url_ptr;


	class visited_queue
	{
		struct impl;
		std::auto_ptr<impl> impl_;

	public:
		visited_queue();
		~visited_queue();

	private:
		visited_queue(const visited_queue &);
		visited_queue &operator=(const visited_queue &);

	public:
		void start();
		void stop();

	public:
		bool is_exsit(const url_ptr &val) const;
		void put(const url_ptr &val);
		
	};

}


#endif