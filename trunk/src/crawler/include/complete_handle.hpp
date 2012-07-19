#ifndef __CRAWLER_COMPLETE_HANDLE_HPP
#define __CRAWLER_COMPLETE_HANDLE_HPP


#include <functional>
#include "url.hpp"
#include "common.hpp"




namespace async
{
	namespace iocp
	{
		class IODispatcher;
	}
}

namespace crawler
{

	class complete_handle
	{
	public:
		typedef std::function<void(bool, const url_ptr &)>	handle_complete_type;
		typedef std::function<void(const std::string &)>	handle_error_type;

	private:
		struct impl;
		std::auto_ptr<impl> impl_;

		async::iocp::IODispatcher &io_;
		handle_complete_type handle_complete_;
		handle_error_type handle_error_;
	

	public:
		explicit complete_handle(async::iocp::IODispatcher &io);
		~complete_handle();

	private:
		complete_handle(const complete_handle &);
		complete_handle &operator=(const complete_handle &);

	public:
		void start();
		void stop();

		void register_callback(const handle_complete_type &complete, const handle_error_type &error);
		void run(const url_ptr &url, const buffer_type &buffer);

	private:
		void _run_impl(const url_ptr &url, const buffer_type &buffer);
	};

	typedef std::shared_ptr<complete_handle> complete_handle_ptr;


	inline complete_handle_ptr create_complete_handle(async::iocp::IODispatcher &io)
	{
		return complete_handle_ptr(new complete_handle(io));
	}
}


#endif