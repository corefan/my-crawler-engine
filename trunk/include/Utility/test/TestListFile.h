#include "stdafx.h"

#include "../FileOperator.hpp"



struct PrintFile
{
	size_t cnt_;

	PrintFile()
		: cnt_(0)
	{}

	template < typename StringT >
	void operator()(const StringT &dir)
	{
		//std::wcout.imbue(std::locale("chs"));
		//std::wcout << dir.c_str() << std::endl;
		cnt_++;
	}

	~PrintFile()
	{
		std::cout << "×Ü¹²: " << cnt_ << std::endl;
	}
};

void TestRange()
{
	utility::CheckFileNull check;
	PrintFile print;

	utility::FileRangeSearch<utility::CheckFileNull, PrintFile> fileRangeSearch(check, print, 0);
	
	fileRangeSearch.Run(stdex::tString(_T("E:\\")));

}

void TestDepth()
{
	utility::CheckFileNull check;
	PrintFile print;

	utility::FileDepthSearch<utility::CheckFileNull, PrintFile> fileRangeSearch(check, print, true);

	fileRangeSearch.Run(stdex::tString(_T("E:\\")));
}