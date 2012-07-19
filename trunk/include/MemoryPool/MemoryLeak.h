#ifndef __CRT_MEMORY_LEAK_HPP
#define __CRT_MEMORY_LEAK_HPP


// http://msdn.microsoft.com/zh-cn/library/e5ewb1h3(v=VS.90).aspx

#ifdef _DEBUG 

#include <cstdlib>
#include <crtdbg.h>


#define _CRTDBG_MAP_ALLOC //探测内存泄漏必须进行声明


// 定制new
#define DEBUG_NORMALBLOCK new( _NORMAL_BLOCK, __FILE__ , __LINE__ ) 
#ifdef new 
#undef new 
#endif 

#define new DEBUG_NORMALBLOCK 

// 定制malloc
#ifdef malloc 
#undef malloc 
#endif 

#define malloc(s) (_malloc_dbg( (s) , _NORMAL_BLOCK, __FILE__ , __LINE__ ))

#endif


namespace crt_memory_leak
{


	template<bool Debug>
	struct MemoryLeak
	{
		MemoryLeak() 
		{
			_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
			
			////控制台程序的情况
			////_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE); 
			//_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT); 
			//_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE); 
			//_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT); 
			//_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE); 
			//_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
		}
		~MemoryLeak()
		{
			//_CrtDumpMemoryLeaks();
		}
	};


	template<>
	struct MemoryLeak<false>
	{ 
		MemoryLeak()
		{
			_CrtSetReportMode( _CRT_WARN,  _CRTDBG_MODE_DEBUG ) ; 
			_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG ) ; 
			_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_DEBUG ) ; 
		}
	};


#ifdef _DEBUG
	__declspec(selectany) MemoryLeak<true> g_MemoryLeak;
#else
	__declspec(selectany) MemoryLeak<false> g_MemoryLeak;
#endif

}

#endif // ifndef __CRT_MEMORY_LEAK_HPP