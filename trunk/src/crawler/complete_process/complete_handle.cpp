#include "complete_handle.hpp"
#include "AsyncIO/iocp/Dispatcher.hpp"

#include <type_traits>

#include "file_storage.hpp"
#include "db_storage.hpp"


namespace crawler
{
	struct complete_handle::impl
	{
		storage::file file_;
		storage::db db_;
	};

	complete_handle::complete_handle(async::iocp::IODispatcher &io)
		: io_(io)
		, impl_(new impl)
	{

	}

	complete_handle::~complete_handle()
	{

	}

	void complete_handle::start()
	{
		impl_->file_.start();
		impl_->db_.start();
	}

	void complete_handle::stop()
	{
		impl_->file_.stop();
		impl_->db_.stop();
	}

	void complete_handle::register_callback(const handle_complete_type &complete, const handle_error_type &error)
	{
		handle_complete_ = complete;
		handle_error_ = error;
	}

	void complete_handle::run(const url_ptr &url, const buffer_type &buffer)
	{
		io_.Post(std::bind(&complete_handle::_run_impl, this, url, buffer));
	}


	void complete_handle::_run_impl(const url_ptr &url, const buffer_type &buffer)
	{
		bool suc = false;

		try
		{
			impl_->file_.save(url, buffer, handle_error_);
			impl_->db_.save(url, buffer, handle_error_);

			suc = true;
		}
		catch(std::exception &e)
		{
			handle_error_(std::cref(std::string(e.what())));
		}

		handle_complete_(suc, std::cref(url));
	}
}