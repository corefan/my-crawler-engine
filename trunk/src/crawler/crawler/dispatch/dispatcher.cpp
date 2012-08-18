#include "stdafx.h"
#include "dispatcher.hpp"

#include "AsyncIO/IOCP/Dispatcher.hpp"

#include "Container/BoundedBlockQueue.hpp"
#include "Container/SyncContainer.hpp"
#include "MultiThread/Thread.hpp"
#include "MultiThread/Lock.hpp"
#include "MultiThread/atomic/atomic.hpp"

#include "../log/log.hpp"
#include "../download/downloader.hpp"

#include "complete_handle.hpp"
#include "parse_handle.hpp"

#include "url.hpp"
#include "../todo_queue/todo_queue.hpp"
#include "../visited_queue/visited_queue.hpp"
#include "../filter/domain.hpp"


namespace crawler
{
	namespace dispatcher
	{

		

		struct mgr::impl
		{
			const size_t running_max_num_;

			async::iocp::IODispatcher io_;
			async::thread::AutoEvent exit_;

			async::log::mgr error_log_;

			typedef todo_queue URLQueue;
			URLQueue url_queue_;

			typedef visited_queue VisitedQueue;
			VisitedQueue visited_queue_;

			parse_handle_ptr	parse_handle_;
			complete_handle_ptr complete_handle_;


			typedef filter::domain restrict_check_type;
			restrict_check_type filter_;

			typedef async::container::BoundedBlockingQueue<size_t> running_thread_queue;
			running_thread_queue running_thread_;


			typedef async::thread::ThreadImplEx Thread;
			Thread thread_;


			impl()
				: running_max_num_(async::iocp::GetFitThreadNum())
				, io_(running_max_num_)
				, parse_handle_(create_parse_handle(io_))
				, complete_handle_(create_complete_handle(io_))
				, filter_(L"")
				, running_thread_(2 * running_max_num_)
			{

			}
			~impl()
			{

			}

			void _handle_error(const std::string &msg)
			{
				//std::cout << msg << std::endl;
				async::log::async_log(error_log_, msg, async::log::Error);
			}


			void _handle_download_complete(bool suc, const buffer_type &buffer, const url_ptr &url)
			{

				if( !suc || buffer.second == 0 )
				{
					_handle_process_complete(suc, url);
				}
				else
				{
					try
					{
						parse_handle_->run(url, buffer);
					}
					catch(std::exception &e)
					{
						_handle_error(e.what());
						_handle_process_complete(false, url);
					}
				}

			}

			void _handle_redirect(const std::wstring &raw_url, const url_ptr &url)
			{
				url_ptr tmp = create_url(raw_url);
				if( !visited_queue_.is_exsit(tmp) )
				{
					url_queue_.put(tmp);
				}
			}


			void _handle_parse_complete(const std::list<const url_ptr> &urls, const url_ptr &url, const buffer_type &buffer)
			{

				std::for_each(urls.begin(), urls.end(), [this](const url_ptr &val)
				{
					if( !visited_queue_.is_exsit(val) )
					{
						url_queue_.put(val);
					}
				});

				try
				{
					complete_handle_->run(url, buffer);
				}
				catch(std::exception &e)
				{
					_handle_error(e.what());
					_handle_process_complete(false, url);
				}
			}

			void _handle_process_complete(bool suc, const url_ptr &url)
			{
				try
				{
					if( suc )
					{
						visited_queue_.put(url);

						//std::wcout << L"success: " << url->get_url() << std::endl;
					}

					running_thread_.Get();
				}
				catch(std::exception &e)
				{
					_handle_error(e.what());
				}
			}


			DWORD _thread_run()
			{
				using namespace std::placeholders;

				while(1)
				{
					url_ptr url = url_queue_.get();
					if( exit_.IsSignalled() ||
						url.get() == 0 )
						break;

					if( !filter_.is_valid(url) )
						continue;
					
					std::wcout << url->get_url() << std::endl;

					running_thread_.Put(1);
					downloader::downloader_ptr downloader = downloader::create_downloader(io_, 
						std::bind(&impl::_handle_download_complete, this, _1, _2, url), 
						std::bind(&impl::_handle_redirect, this, _1, url), 
						std::bind(&impl::_handle_error, this, _1),
						exit_);
					downloader->run(url);

					
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
			using namespace std::placeholders;


			impl_->error_log_.start();
			impl_->url_queue_.start(std::bind(&impl::_handle_error, impl_.get(), _1));
			impl_->visited_queue_.start();
			impl_->parse_handle_->start();
			impl_->complete_handle_->start();

			impl_->parse_handle_->register_callback(
				std::bind(&impl::_handle_parse_complete, impl_.get(), _1, _2, _3),
				std::bind(&impl::_handle_error, impl_.get(), _1));

			impl_->complete_handle_->register_callback(
				std::bind(&impl::_handle_process_complete, impl_.get(), _1, _2),
				std::bind(&impl::_handle_error, impl_.get(), _1));
		
			impl_->exit_.Create(0, TRUE, FALSE);
			impl_->thread_.RegisterFunc(std::bind(&impl::_thread_run, impl_.get()));
			impl_->thread_.Start();

			url_ptr default_url = create_url(L"http://blog.csdn.net/chenyu2202863/");
			if( !impl_->visited_queue_.is_exsit(default_url) )
				impl_->url_queue_.put(default_url);
		}

		void mgr::stop()
		{
			impl_->exit_.SetEvent();
			
			while( !impl_->running_thread_.Empty() )
			{
				std::cout << '.';
				::Sleep(10);//impl_->running_thread_.Get();
			}

			impl_->complete_handle_->stop();
			impl_->parse_handle_->stop();
			impl_->visited_queue_.stop();
			impl_->url_queue_.stop();

			impl_->thread_.Stop();

			impl_->io_.Stop();

			impl_->error_log_.stop();
		}
	}
}
