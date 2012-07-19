#include "parse_handle.hpp"
#include "AsyncIO/iocp/Dispatcher.hpp"

#include <type_traits>
#include <list>
#include <regex>

#include "Unicode/string.hpp"

#include "html_parser.hpp"


namespace crawler
{
	struct parse_handle::impl
	{
		parser::html_parser html_;
	};

	parse_handle::parse_handle(async::iocp::IODispatcher &io)
		: io_(io)
		, impl_(new impl)
	{

	}

	parse_handle::~parse_handle()
	{
		
	}

	void parse_handle::start()
	{
		impl_->html_.start();
	}

	void parse_handle::stop()
	{
		impl_->html_.stop();
	}

	void parse_handle::register_callback(const handle_parse_type &complete, const handle_error_type &error)
	{
		handle_parse_ = complete;
		handle_error_ = error;
	}

	void parse_handle::run(const url_ptr &url, const buffer_type &buffer)
	{
		io_.Post(std::bind(&parse_handle::_run_impl, this, url, buffer));
	}


	void parse_handle::_run_impl(const url_ptr &url, const buffer_type &buffer)
	{
		std::list<const url_ptr> urls;
	
		try
		{
			static const std::regex link_pattern("<\\s*a.*?\\s+href\\s*=\\s*([\"']?)(.*?)\\1(\\s+.*?)*?>.*?</a>", std::regex::icase);
			//static const std::regex link_pattern("<[aA]\\s+.*?[hH][rR][eE][fF]=\\s*(\"|')?(.*)(\\1)(\\s[^>]*)*?>(.*?)<\\/[aA]>");
			static const std::cregex_token_iterator end;

			for(std::cregex_token_iterator i(buffer.first.get(), buffer.first.get() + buffer.second, link_pattern); i != end; ++i)
			{
				static const std::regex http_pattern("http(s)?://([\\w-]+\\.)+[\\w-]+(/[\\w-\\./?%=]*)?");

				std::string http = i->str();
				for (std::sregex_token_iterator j(http.begin(), http.end(), http_pattern);
					j != std::sregex_token_iterator(); ++j)
				{
					std::wstring url = unicode::to_w(j->str());

					urls.push_back(create_url(url));
				}
			}

			impl_->html_.parse(url, buffer);
		}
		catch(std::exception &e)
		{
			handle_error_(std::cref(std::string(e.what())));
		}

		handle_parse_(std::cref(urls), std::cref(url), std::cref(buffer));
	}
}

