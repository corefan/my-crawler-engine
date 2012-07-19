#ifndef __DOWNLOADER_HTTP_HPP
#define __DOWNLOADER_HTTP_HPP

#include "common.hpp"
#include "url.hpp"
#include "MultiThread/Lock.hpp"


namespace crawler
{
	namespace downloader
	{
		namespace detail
		{
			class ms_http
			{
			public:
				typedef std::function<void(bool, const buffer_type &)>	complete_callback_type;
				typedef std::function<void(const std::wstring &)>							redirect_type;
				typedef std::function<void(const std::string &)>							error_type;

			private:
				complete_callback_type complete_;
				redirect_type redirect_;
				error_type error_;
				async::thread::AutoEvent &exit_;

			public:
				ms_http(const complete_callback_type &callback, const redirect_type &redirect, const error_type &error, async::thread::AutoEvent &exit)
					: complete_(callback)
					, redirect_(redirect)
					, error_(error)
					, exit_(exit)
				{}

			public:
				void run(const url_ptr &val);
			};
		}
	}
}




#endif