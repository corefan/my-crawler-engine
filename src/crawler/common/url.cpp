#include "stdafx.h"
#include "url.hpp"

namespace crawler
{

	namespace
	{
		url *allocate_url()
		{
			return new url;
		}

		url *allocate_url(const std::wstring &var)
		{
			return new url(var);
		}

		void deallocate_url(url *p)
		{
			delete p;
		}
	}	

	url_ptr create_url()
	{
		return url_ptr(allocate_url(), &deallocate_url);
	}

	url_ptr create_url(const std::wstring &url)
	{
		return url_ptr(allocate_url(url), &deallocate_url);
	}



	url::url()
		: No_(0)
		, hit_num_(0)
		, weight_(0)
		, file_size_(0)
		, layer_(0)
	{}

	url::url(const std::wstring &url)
		: url_(url)
		, No_(0)
		, hit_num_(0)
		, weight_(0)
		, file_size_(0)
		, layer_(0)
	{}


	url::~url()
	{

	}
}