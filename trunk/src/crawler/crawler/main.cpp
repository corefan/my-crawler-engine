#include "stdafx.h"
#include "crawler.hpp"

#include "third_party/vld/vld.h"


int _tmain(int argc, _TCHAR* argv[])
{
	

	crawler::mgr crawler_;
	crawler_.start();

	system("pause");

	crawler_.stop();
	return 0;
}