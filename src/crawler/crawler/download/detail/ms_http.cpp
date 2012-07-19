#include "stdafx.h"
#include "ms_http.hpp"

#include <iterator>
#include <iostream>

#include "Http/win_http/session.hpp"
#include "Http/win_http/connection.hpp"
#include "Http/win_http/request.hpp"
#include "Http/win_http/url.hpp"



namespace crawler
{
	namespace downloader
	{
		namespace detail
		{
			class http_helper;
			typedef std::shared_ptr<http_helper> http_ptr;


			template < typename CallbackT, typename RedirectT, typename ErrorT >
			void make_http(const CallbackT &callback, const RedirectT &redirect, const ErrorT &error, async::thread::AutoEvent &exit, const std::wstring &url_path)
			{
				try
				{
					http::url url(url_path);
					http_ptr val(new http_helper(callback, redirect, error, exit, url));
					val->run();
				}
				catch(std::exception &e)
				{
					static buffer_type buf;
					callback(false, std::cref(buf));
					error(std::cref(std::string(e.what())));
				}
			}



			class http_helper
				: public std::enable_shared_from_this< http_helper >
			{
				typedef ms_http::complete_callback_type handle_complete_type;
				typedef ms_http::redirect_type			handle_redirect_type;
				typedef ms_http::error_type				handle_error_type;

			private:
				handle_complete_type handle_complete_;
				handle_redirect_type handle_redirect_;
				handle_error_type    handle_error_;

				async::thread::AutoEvent &exit_;
				http_ptr self_;

				http::session session_;
				http::connection con_;
				http::request req_;

				buffer_type buf_;
				size_t index_;



			public:
				http_helper(const handle_complete_type &complete_callback, const handle_redirect_type &redirect, const handle_error_type &error,
					async::thread::AutoEvent &exit, const http::url &url)
					: handle_complete_(complete_callback)
					, handle_redirect_(redirect)
					, handle_error_(error)
					, exit_(exit)
					, session_(L"async crawler downloader 0.1")
					, con_(session_, url)
					, req_(con_, L"GET", url)
					, index_(0)
				{
					std::chrono::milliseconds millisecond(5 * 1000);
					session_.set_timeout(millisecond, millisecond, millisecond, millisecond);

				}
				~http_helper()
				{

				}

				void run()
				{
					using namespace std::placeholders;

					self_ = shared_from_this();
					req_.register_callback(
						std::bind(&http_helper::_handle_header, this, _1, _2),
						std::bind(&http_helper::_handle_complete, this, _1),
						std::bind(&http_helper::_handle_read, this, _1, _2),
						std::bind(&http_helper::_handle_redirect, this, _1, _2),
						std::bind(&http_helper::_handle_error, this, _1));
					req_.send_request();
				}

				void _handle_header(const http::request &req, std::uint32_t size)
				{	
					buf_ = make_buffer(size);
				}

				void _handle_complete(bool suc)
				{
					//assert(suc);
					if( handle_complete_ )
						handle_complete_(suc, std::cref(buf_));

					self_.reset();
				}

				bool _handle_read(const char *buf, size_t len)
				{
					if( exit_.IsSignalled() )
						return false;

					std::copy(buf, buf + len, stdext::make_checked_array_iterator(buf_.first.get() + index_, buf_.second));
					index_ += len;
					return true;
				}

				void _handle_redirect(const wchar_t *url, size_t len)
				{
					std::wstring tmp(url, len);
					handle_redirect_(std::cref(tmp));
				}

				void _handle_error(const std::string &msg)
				{
					handle_error_(std::cref(msg));

					//assert(0 && msg.c_str());
				}
			};



			

			// ------------------------------------

			void ms_http::run(const url_ptr &val)
			{
				make_http(complete_, redirect_, error_, exit_, val->get_url());
			}
		}
	}
}