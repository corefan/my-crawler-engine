#ifndef __CRAWLER_PARSE_HANDLE_HPP
#define __CRAWLER_PARSE_HANDLE_HPP


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

	class parse_handle
	{
	public:
		typedef std::function<void (const std::list<const url_ptr> &, const url_ptr &, const buffer_type &)> handle_parse_type;
		typedef std::function<void(const std::string &)>	handle_error_type;

	private:
		struct impl;
		std::auto_ptr<impl> impl_;

		async::iocp::IODispatcher &io_;
		handle_parse_type handle_parse_;
		handle_error_type handle_error_;


	public:
		explicit parse_handle(async::iocp::IODispatcher &io);
		~parse_handle();

	private:
		parse_handle(const parse_handle &);
		parse_handle &operator=(const parse_handle &);

	public:
		void start();
		void stop();

		void register_callback(const handle_parse_type &complete, const handle_error_type &error);
		void run(const url_ptr &url, const buffer_type &buffer);

	private:
		void _run_impl(const url_ptr &url, const buffer_type &buffer);
	};

	typedef std::shared_ptr<parse_handle> parse_handle_ptr;


	inline parse_handle_ptr create_parse_handle(async::iocp::IODispatcher &io)
	{
		return parse_handle_ptr(new parse_handle(io));
	}
}


#endif