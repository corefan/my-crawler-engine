#ifndef __STACK_WALKER_HPP
#define __STACK_WALKER_HPP


namespace utility
{

	struct IStackDumpHandler
	{
		virtual ~IStackDumpHandler() = 0 {}
		virtual void OnBegin() = 0;
		virtual void OnEntry(void *pvAddress, size_t lineNum, LPCSTR fileName, LPCSTR szModule, LPCSTR szSymbol) = 0;
		virtual void OnError(LPCSTR szError) = 0;
		virtual void OnEnd() = 0;
	};

	

	// Helper function to produce a stack dump
	void DumpStack(_In_ IStackDumpHandler *pHandler);
}





#endif