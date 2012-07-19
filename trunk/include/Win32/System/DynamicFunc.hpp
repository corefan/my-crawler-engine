#ifndef __WIN32_DYNAMIC_FUNC_HPP
#define __WIN32_DYNAMIC_FUNC_HPP

#include <cassert>


namespace win32
{
	namespace system
	{

		// -------------------------

		class DynamicFnBase 
		{
		public:
			DynamicFnBase(const TCHAR* dllName, const char* fnName) 
				: dllHandle(0)
				, fnPtr(0) 
			{
				dllHandle = ::LoadLibrary(dllName);
				if( !dllHandle ) 
				{
					assert(0);
					return;
				}

				fnPtr = ::GetProcAddress(dllHandle, fnName);
				if (!fnPtr)
				{
					assert(0);
					return;
				}
			}

			DynamicFnBase::~DynamicFnBase() 
			{
				if (dllHandle)
					::FreeLibrary(dllHandle);
			}

			bool IsValid() const {return fnPtr != 0;}
		protected:
			void* fnPtr;
			HMODULE dllHandle;
		private:
			DynamicFnBase(const DynamicFnBase&);
			DynamicFnBase operator=(const DynamicFnBase&);
		};

		template< typename T > 
		class DynamicFunc 
			: public DynamicFnBase 
		{
		public:
			DynamicFunc(const TCHAR* dllName, const char* fnName) 
				: DynamicFnBase(dllName, fnName) 
			{}

			T operator *() const 
			{ return (T)fnPtr; };
		};
	}
}




#endif