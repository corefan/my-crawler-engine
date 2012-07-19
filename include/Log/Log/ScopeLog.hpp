#ifndef __LOG_SCOPED_LOG_HPP
#define __LOG_SCOPED_LOG_HPP


#include "Log.hpp"
#include "../Storage/ScopeStorage.hpp"

#include <vector>

//#include <shlwapi.h>
//#pragma comment(lib, "shlwapi")


namespace logsystem
{

	namespace impl
	{
		// ---------------------------------------------------------------------
		// class ScopeLogT

		template<typename ImplStorageT, typename FilterT, typename StorageBaseT = storage::StorageBase<typename ImplStorageT::char_type>>
		class ScopeLogT
			: public Log<storage::ScopeStorageT<ImplStorageT, StorageBaseT>, FilterT>
		{
		public:
			typedef Log<storage::ScopeStorageT<ImplStorageT, StorageBaseT>, FilterT>	BaseType;
			using BaseType::storage_;

		public:
			ScopeLogT()
			{}
			template<typename ArgT>
			ScopeLogT(const ArgT &arg)
				: BaseType(arg)
			{}
			template<typename ArgT1, typename ArgT2>
			ScopeLogT(const ArgT1 &arg1, const ArgT2 &arg2)
				: BaseType(arg1, arg2)
			{}

		public:
			void EnterScope()
			{
				storage_.EnterScope();
			}

			void LeaveScope()
			{
				storage_.LeaveScope();
			}

			void Commit()
			{
				storage_.Commit();
			}

			void TraceScopeMessage(const char_type *pScope, const char_type *pOperation, const char_type *pFile, int nLine)
			{
				return _TraceImpl(pScope, pOperation, pFile, nLine);
			}

			template<typename ErrorT>
			void ReportGuradError(const char_type *pGeneral, const ErrorT &nError, const char_type *pDetail, const char_type *pFile, int nLine)
			{
				return _ReportImpl(pGeneral, nError, pDetail, pFile, nLine);
			}

		private:
			void _TraceImpl(const char *pScope, const char *pOperation, const char *pFile, int nLine)
			{
				Trace("%s(%d): %s '%s'\n", pFile, nLine, pOperation, pScope);
			}
			void _TraceImpl(const wchar_t *pScope, const wchar_t *pOperation, const wchar_t *pFile, int nLine)
			{
				Trace(L"%S(%d): %S '%S'\n", pFile, nLine, pOperation, pScope);
			}

			template<typename ErrorT>
			void _ReportImpl(const char *pGeneral, const ErrorT &nError, const char *pDetail, const char *pFile, int nLine)
			{
				Trace("%s(%d):\n\t%s[%d] - %s\n", pFile, nLine, pGeneral, nError, pDetail);

				storage_.Commit();
			}
			template<typename ErrorT>
			void _ReportImpl(const wchar_t *pGeneral, const ErrorT &nError, const wchar_t *pDetail, const wchar_t *pFile, int nLine)
			{
				Trace(L"%S(%d):\n\t%S[%d] - %S\n", pFile, nLine, pGeneral, nError, pDetail);

				storage_.Commit();
			}
		};


		template<typename CharT, bool Owned, typename FilterT>
		class FileScopeLogT
			: public ScopeLogT<storage::FileStorageT<CharT, Owned>, FilterT>
		{
			typedef ScopeLogT<storage::FileStorageT<CharT, Owned>, FilterT>	BaseType;

		public:
			FileScopeLogT()
				: BaseType()
			{}
			template<typename Arg>
			FileScopeLogT(const Arg &arg)
				: BaseType(arg)
			{}
			FileScopeLogT(const char_type *szFile)
				: BaseType(szFile, Owned)
			{}
			~FileScopeLogT()
			{
				storage_.Commit();
				storage_.Close();
			}

		public:
			void Open(const char_type *file)
			{
				storage_.Open(file, Owned);
			}
		};

	}
	
}


#endif