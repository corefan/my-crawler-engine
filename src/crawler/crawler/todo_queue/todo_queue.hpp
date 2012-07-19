#ifndef __CRALER_TODO_QUEUE_HPP
#define __CRALER_TODO_QUEUE_HPP

#include <memory>
#include <string>
#include <functional>


namespace crawler
{
	class url;
	typedef std::shared_ptr<url> url_ptr;

	
	class todo_queue
	{
		typedef std::function<void(const std::string &)> handle_error_type;

		struct impl;
		std::auto_ptr<impl> impl_;

	public:
		todo_queue();
		~todo_queue();

	private:
		todo_queue(const todo_queue &);
		todo_queue &operator=(const todo_queue &);

	public:
		void start(const handle_error_type &error);
		void stop();

	public:
		void put(const url_ptr &val);
		url_ptr get();
	};


}



#endif