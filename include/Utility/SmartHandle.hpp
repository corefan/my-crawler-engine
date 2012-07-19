#ifndef __SMART_POINTER_SMART_HANDLE_HPP
#define __SMART_POINTER_SMART_HANDLE_HPP


#include <Windows.h>
#include <Winsvc.h>
#include <type_traits>
#include <memory>



namespace utility
{


	// Release algorithms (release policies)

	template<typename T, typename PointedBy>
	struct CCloseHandle
	{
		void operator()(T handle)
		{
			::CloseHandle(handle);
		}

		~CCloseHandle()
		{
		}
	};



	template<typename T, typename PointedBy>
	struct CCloseRegKey
	{
		void operator()(T handle)
		{
			::RegCloseKey(handle);
		}

		~CCloseRegKey()
		{
		}
	};


	template<typename T, typename PointedBy>
	struct CCloseLibrary
	{
		void operator()(T handle)
		{
			::FreeLibrary(handle);
		}

		~CCloseLibrary()
		{
		}
	};


	template<typename T, typename PointedBy>
	struct CCloseViewOfFile
	{
		void operator()(T handle)
		{
			::UnmapViewOfFile(handle);
		}

		~CCloseViewOfFile()
		{
		}
	};


	template<typename T, typename PointedBy>
	struct CCloseService
	{
		void operator()(T handle)
		{
			::CloseServiceHandle(handle);
		}

		~CCloseService()
		{
		}
	};

	template<typename T, typename PointedBy>
	struct CCloseFindFile
	{
		void operator()(T handle)
		{
			::FindClose(handle);
		}
	};

	template<typename T, typename PointedBy>
	struct CCloseIcon
	{
		void operator()(T handle)
		{
			::DestroyIcon(handle);
		}
	};

	
	


	// Empty class used as default CAutoHandle template parameter.
	class CEmptyClass
	{
	};


	// Class CSmartHandle which implements release policy.
	// Second template parameter is ReleaseAlgorithm which is template itself.

	template<
		typename HandleT, 
		template<typename, typename> class ReleaseAlgorithm, 
		typename PointedBy = CEmptyClass,          // used for smart pointers
		HandleT NULL_VALUE = NULL
	>
	class CSmartHandle 
		: private ReleaseAlgorithm<HandleT, PointedBy>
	{
	public:
		typedef HandleT											Type;
		typedef typename std::tr1::remove_pointer<Type>::type	RealType;
		Type m_Handle;

	public:
		CSmartHandle()
			: m_Handle(NULL_VALUE)
		{
		}

		CSmartHandle(const Type &h)
			: m_Handle(h)
		{
		}

		CSmartHandle(const CSmartHandle &h)
		{
			CleanUp();
			m_Handle = h.m_Handle;
		}

		~CSmartHandle()
		{
			CleanUp();
		}

		CSmartHandle &operator=(const CSmartHandle &rhs) 
		{ 
			if( &rhs != this )
			{
				CleanUp();
				m_Handle = rhs.m_Handle;
			}

			return(*this);  
		}

		Type &operator=(const Type &hande) 
		{ 
			if( hande != m_Handle )
			{
				CleanUp();
				m_Handle = hande;
			}

			return m_Handle;  
		}

		operator Type()
		{
			return m_Handle;
		}

		operator const Type() const
		{
			return m_Handle;
		}

		Type Get()
		{
			return m_Handle;
		}

		const Type Get() const
		{
			return m_Handle;
		}
		
		PointedBy* operator->()                 // for using as smart pointer
		{
			// NOTE: adding this operator allows to use CAutoHandle object as pointer.
			// However, if PointedBy is CHandlePlaceHolder (used for handles),
			// this is not compiled because CHandlePlaceHolder has no functions.
			// This is exactly what I need.
			return m_Handle;
		}

		bool IsValid()
		{
			return m_Handle != NULL_VALUE;
		}

		Type Detach()
		{
			Type hHandle = m_Handle;
			m_Handle = NULL_VALUE;

			return hHandle;
		}

		void CleanUp()
		{
			if ( m_Handle != NULL_VALUE )
			{
				operator()(m_Handle);
				m_Handle = NULL_VALUE;
			}
		}

	};


	// Client code (definitions of standard Windows handles).
	typedef CSmartHandle<HANDLE,	CCloseHandle>		                    CAutoHandle;
	typedef CSmartHandle<HKEY,		CCloseRegKey>		                    CAutoRegKey;
	typedef CSmartHandle<PVOID,		CCloseViewOfFile>	                    CAutoViewOfFile;
	typedef CSmartHandle<HMODULE,	CCloseLibrary>							CAutoLibrary;
	typedef CSmartHandle<HANDLE,	CCloseHandle, CEmptyClass, INVALID_HANDLE_VALUE>	CAutoFile;										CAutoFile;
	typedef CSmartHandle<SC_HANDLE, CCloseService>							CAutoService;
	typedef CSmartHandle<HANDLE,	CCloseFindFile, CEmptyClass, INVALID_HANDLE_VALUE> CAutoFindFile;
	typedef CSmartHandle<HICON,		CCloseIcon>								CAutoIcon;
	typedef CSmartHandle<HANDLE,	CCloseHandle, CEmptyClass, INVALID_HANDLE_VALUE> CAutoToolhelp;
	typedef CSmartHandle<HANDLE,	CCloseHandle>							CAutoToken;

	namespace detail
	{
		struct IconTag
		{ 
			typedef HICON Type;
			typedef BOOL (__stdcall *DeletorFunc)(Type);
			
			static DeletorFunc GetDeletor()
			{ return &::DestroyIcon; }
		};

		struct BitmapTag
		{
			typedef HBITMAP Type;
			typedef BOOL (__stdcall *DeletorFunc)(HGDIOBJ );

			static DeletorFunc GetDeletor()
			{ return &::DeleteObject; }
		};

		struct HandleTag
		{
			typedef HANDLE Type;
			typedef BOOL (__stdcall *DeletorFunc)(Type);

			static DeletorFunc GetDeletor()
			{ return &::CloseHandle; }
		};
	}
	

	template < typename ResourceT >
	struct DeclareType
	{
		typedef ResourceT ResourceType;
		typedef typename ResourceType::Type Type;

		typedef std::tr1::shared_ptr<void> ResType;
		ResType res_;

		DeclareType()
		{}
		DeclareType(const Type &res)
			: res_(res, ResourceType::GetDeletor())
		{}
		DeclareType(const DeclareType &rhs)
			: res_(rhs.res_)
		{}
		DeclareType &operator=(const Type &res)
		{
			if( res_.get() != res )
				res_.reset(res, ResourceType::GetDeletor());

			return *this;
		}
		DeclareType &operator=(const DeclareType &rhs)
		{
			if( this != &rhs )
				res_ = rhs.res_;

			return *this;
		}

		operator ResType()
		{ return res_; }
		operator const ResType() const
		{ return res_; }

		operator Type()
		{ return static_cast<Type>(res_.get()); }
		operator const Type() const
		{ return static_cast<Type>(res_.get()); }
	};

	template < typename T >
	bool operator==(const DeclareType<T> &lhs, const DeclareType<T> &rhs)
	{
		return lhs.res_ == rhs.res_;
	}

	template < typename T >
	bool operator!=(const DeclareType<T> &lhs, const DeclareType<T> &rhs)
	{
		return !(lhs.res_ == rhs.res_);
	}

	template < typename T >
	bool operator==(const DeclareType<T> &lhs, size_t val)
	{
		assert(val == 0);
		return lhs.res_.get() == 0;
	}

	template < typename T >
	bool operator!=(const DeclareType<T> &lhs, size_t val)
	{
		assert(val == 0);
		return !(lhs.res_ == 0);
	}


	typedef DeclareType<detail::IconTag>		ICONPtr;
	typedef DeclareType<detail::HandleTag>		HANDLEPtr;
	typedef DeclareType<detail::BitmapTag>		BITMAPPtr;

}


#endif