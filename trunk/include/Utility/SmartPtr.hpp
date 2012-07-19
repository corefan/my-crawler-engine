#ifndef __SMART_PTR_HPP
#define __SMART_PTR_HPP

#include "SmartHandle.hpp"

namespace utility
{


	template<typename T, typename PointedBy>
	struct CClosePointer
	{
		void operator()(T handle)
		{
			delete (PointedBy*)handle;
		}

	protected:
		~CClosePointer()
		{
		}
	};


	



	template<typename T, typename PointedBy>
	struct CCloseArrayPointer
	{
		void operator()(T handle)
		{
			delete[] (PointedBy*)handle;
		}

	protected:
		~CCloseArrayPointer()
		{
		}
	};




	template<typename T>
	struct CAutoPtr
	{
		typedef CSmartHandle<T*, CClosePointer, T> AutoPtr;
	};


	typedef CAutoPtr<char>::AutoPtr			CCharPtr;
	typedef CAutoPtr<wchar_t>::AutoPtr		CWCharPtr;
	typedef CAutoPtr<TCHAR>::AutoPtr		CTCharPtr;



	template<typename T>
	struct CAutoArrayPtr
	{
		typedef CSmartHandle<T*, CCloseArrayPointer, T> AutoPtr;
	};

	typedef CAutoArrayPtr<char>::AutoPtr		CArrayCharPtr;
	typedef CAutoArrayPtr<wchar_t>::AutoPtr		CArrayWCharPtr;
	typedef CAutoArrayPtr<TCHAR>::AutoPtr		CArrayTCharPtr;



	//-------------------------------------------------
	// Example
	// 
	// CAutoPtr<int>::AutoPtr ptr(new int);
	// CAutoArrayPtr<int>::AutoPtr arr(new int[100]);
	
}


#endif