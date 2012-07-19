#ifndef __CRAWLER_DOWNLOADER_HPP
#define __CRAWLER_DOWNLOADER_HPP

#include "common.hpp"
#include "url.hpp"
#include "detail/ms_http.hpp"


namespace crawler
{
	namespace downloader
	{
	
		template < typename ImplT >
		class mgr_t
			: public std::enable_shared_from_this< mgr_t<ImplT> >
		{
			typedef ImplT impl_type;

		private:
			async::iocp::IODispatcher &io_;
			impl_type impl_;
			std::shared_ptr<mgr_t<ImplT>> self_;

			
		public:
			template < typename HandlerT, typename RedirectT, typename ErrorT, typename ExitEventT >
			mgr_t(async::iocp::IODispatcher &io, const HandlerT &handler, const RedirectT &redirect, const ErrorT &error, ExitEventT &event)
				: io_(io)
				, impl_(handler, redirect, error, event)
			{

			}
			~mgr_t()
			{

			}

		private:
			mgr_t(const mgr_t &);
			mgr_t &operator=(const mgr_t &);

		public:
			void run(const url_ptr &val)
			{
				io_.Post(std::bind(&mgr_t::_run_impl, shared_from_this(), val));
				//_run_impl(url);
			}
	
			void _run_impl(const url_ptr &val)
			{
				impl_.run(val);
			}

		};

		typedef mgr_t<detail::ms_http>	ms_http_downloader;
		typedef std::shared_ptr<ms_http_downloader> downloader_ptr;


		template < typename HandelrT, typename RedirectT, typename ErrorT, typename ExitEventT >
		inline downloader_ptr create_downloader(async::iocp::IODispatcher &io, const HandelrT &handler, const RedirectT &redirect, const ErrorT &error, ExitEventT &event)
		{
			return downloader_ptr(new ms_http_downloader(io, handler, redirect, error, event));
		}
	}
}


#endif