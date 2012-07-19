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
		// ԭʼURL
		std::wstring url_;		

		// URL NUM
		std::uint32_t No_;

		// �����±������������õĴ���
		std::uint32_t hit_num_;

		// ���µ�Ȩ��,��������ʵ���Ϣ
		std::uint16_t weight_;

		// ���´�С
		std::uint32_t file_size_;

		// ��ȡ�Ĳ���
		std::uint16_t layer_;

		// �����µı���
		std::wstring charset_;
		
		// ����ժҪ
		std::wstring abstract_text_;
		
		// ��������
		std::wstring author_;
		
		// ���½���
		std::wstring description_;
		
		// ���±���
		std::wstring title_;

		// ��������
		std::wstring type_;

		// ���õ�����
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