#ifndef __CRAWLER_URL_HPP
#define __CRAWLER_URL_HPP

#include "common.hpp"
#include "time/chrono.hpp"

#include <forward_list>

namespace crawler
{

	class url;
	typedef std::shared_ptr<url> url_ptr;


	class url
	{
	public:
		// 原始URL
		std::wstring url_;		

		// URL NUM
		std::uint32_t No_;

		// 此文章被其他文章引用的次数
		std::uint32_t hit_num_;

		// 文章的权重,包含导向词的信息
		std::uint16_t weight_;

		// 文章大小
		std::uint32_t file_size_;

		// 爬取的层数
		std::uint16_t layer_;

		// 此文章的编码
		std::wstring charset_;
		
		// 文章摘要
		std::wstring abstract_text_;
		
		// 文章作者
		std::wstring author_;
		
		// 文章介绍
		std::wstring description_;
		
		// 文章标题
		std::wstring title_;

		// 文章类型
		std::wstring type_;

		// 引用的链接
		std::forward_list<url_ptr> ref_urls_;
	
	public:
		url();
		explicit url(const std::wstring &url);
		~url();

	private:
		url(const url &);
		url &operator=(const url &);

	public:
		operator const std::wstring &() const
		{
			return url_;
		}

		const std::wstring &get_url() const
		{
			return url_;
		}
	};

	

	url_ptr create_url();
	url_ptr create_url(const std::wstring &url);
}


#endif