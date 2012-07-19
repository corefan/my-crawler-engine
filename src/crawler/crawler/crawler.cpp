#include "stdafx.h"
#include "crawler.hpp"

#include <iostream>

#include "Win32/System/except.h"
#include "dispatch/dispatcher.hpp"



namespace crawler
{
	struct mgr::impl
	{
		minidump::MiniDump dump_;
		dispatcher::mgr dispatch_;
		
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
		std::cout.sync_with_stdio(false);
		std::cerr.sync_with_stdio(false);

		
		impl_->dispatch_.start();
	}

	void mgr::stop()
	{
		impl_->dispatch_.stop();
		
	}
}
