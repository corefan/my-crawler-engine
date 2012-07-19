//#include "stdafx.h"
#include "ado_helper.hpp"

#include "Extend STL/StringAlgorithm.h"
#include "Unicode/string.hpp"


namespace ado
{

	connect_event::connect_event()
		: ref_(0)
	{}
	connect_event::~connect_event()
	{}


	void connect_event::register_callbacks(const select_complete_type &select_callback)
	{
		select_callback_ = select_callback;
	}

	STDMETHODIMP connect_event::raw_InfoMessage(Error *pError, EventStatusEnum *adStatus, _Connection *pConnection)
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP connect_event::raw_BeginTransComplete(LONG TransactionLevel, Error *pError, EventStatusEnum *adStatus, _Connection *pConnection) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP connect_event::raw_CommitTransComplete(Error *pError, EventStatusEnum *adStatus, _Connection *pConnection) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP connect_event::raw_RollbackTransComplete(Error *pError, EventStatusEnum *adStatus, _Connection *pConnection) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP connect_event::raw_WillExecute(BSTR *Source, CursorTypeEnum *CursorType, LockTypeEnum *LockType,
		long *Options, EventStatusEnum *adStatus, _Command *pCommand, _Recordset *pRecordset, _Connection *pConnection) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP connect_event::raw_ExecuteComplete(LONG RecordsAffected, Error *pError, EventStatusEnum *adStatus, 
		_Command *pCommand, _Recordset *pRecordset, _Connection *pConnection) 
	{
		assert(*adStatus == adStatusOK);
		select_callback_(RecordsAffected, pError);

		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP connect_event::raw_WillConnect(BSTR *ConnectionString, BSTR *UserID, BSTR *Password, 
		long *Options, EventStatusEnum *adStatus,  _Connection *pConnection) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP connect_event::raw_ConnectComplete(Error *pError, EventStatusEnum *adStatus, _Connection *pConnection) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP connect_event::raw_Disconnect(EventStatusEnum *adStatus, _Connection *pConnection) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	

	recordset_event::recordset_event()
		: ref_(0)
	{}

	recordset_event::~recordset_event()
	{}

	STDMETHODIMP recordset_event::raw_WillChangeField(LONG cFields, VARIANT Fields, EventStatusEnum *adStatus, _Recordset *pRecordset) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP recordset_event::raw_FieldChangeComplete(LONG cFields, VARIANT Fields, Error *pError,
		EventStatusEnum *adStatus, _Recordset *pRecordset) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP recordset_event::raw_WillChangeRecord(EventReasonEnum adReason, LONG cRecords, EventStatusEnum *adStatus, _Recordset *pRecordset) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP recordset_event::raw_RecordChangeComplete(EventReasonEnum adReason, LONG cRecords,
		Error *pError, EventStatusEnum *adStatus, _Recordset *pRecordset)
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP recordset_event::raw_WillChangeRecordset(EventReasonEnum adReason, EventStatusEnum *adStatus, _Recordset *pRecordset) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};


	STDMETHODIMP recordset_event::raw_RecordsetChangeComplete(EventReasonEnum adReason, Error *pError, EventStatusEnum *adStatus, _Recordset *pRecordset) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP recordset_event::raw_WillMove(EventReasonEnum adReason, EventStatusEnum *adStatus, _Recordset *pRecordset) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP recordset_event::raw_MoveComplete( EventReasonEnum adReason,
	struct Error *pError,
		EventStatusEnum *adStatus,
	struct _Recordset *pRecordset) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP recordset_event::raw_EndOfRecordset(VARIANT_BOOL *fMoreData, EventStatusEnum *adStatus, _Recordset *pRecordset) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP recordset_event::raw_FetchProgress(long Progress, long MaxProgress, EventStatusEnum *adStatus, _Recordset *pRecordset) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP recordset_event::raw_FetchComplete(Error *pError, EventStatusEnum *adStatus, _Recordset *pRecordset) 
	{
		*adStatus = adStatusUnwantedEvent;
		return S_OK;
	};

	STDMETHODIMP recordset_event::QueryInterface(REFIID riid, void ** ppv) 
	{
		*ppv = NULL;
		if (riid == __uuidof(IUnknown) || riid == __uuidof(RecordsetEventsVt)) 
			*ppv = this;

		if (*ppv == NULL)
			return ResultFromScode(E_NOINTERFACE);

		AddRef();
		return NOERROR;
	}

	STDMETHODIMP_(ULONG) recordset_event::AddRef() 
	{ 
		::InterlockedIncrement(&ref_);
		return ref_;
	};

	STDMETHODIMP_(ULONG) recordset_event::Release() 
	{ 
		::InterlockedExchangeAdd(&ref_, -1);
		if( 0 != ref_ ) 
			return ref_;

		return 0;
	}

	STDMETHODIMP connect_event::QueryInterface(REFIID riid, void ** ppv) 
	{
		*ppv = NULL;
		if (riid == __uuidof(IUnknown) || riid == __uuidof(ConnectionEventsVt)) 
			*ppv = this;

		if (*ppv == NULL)
			return ResultFromScode(E_NOINTERFACE);

		AddRef();
		return NOERROR;
	}

	STDMETHODIMP_(ULONG) connect_event::AddRef() 
	{ 
		::InterlockedIncrement(&ref_);
		return ref_;
	};

	STDMETHODIMP_(ULONG) connect_event::Release() 
	{ 
		::InterlockedExchangeAdd(&ref_, -1);
		if( 0 != ref_ ) 
			return ref_;

		return 0;
	}

	namespace detail
	{
		template < typename CallbackT >
		void execute(const _ConnectionPtr &connect, const std::wstring &sql, const std::wstring &param, CommandTypeEnum type, const CallbackT &callback)
		{
			valid(connect);

			_CommandPtr command;
			test_hr(command.CreateInstance(__uuidof(Command)));
			command->CommandType = type;
			command->ActiveConnection = connect;
			command->CommandText = sql.c_str();

			std::vector<std::wstring> params;
			stdex::Split(params, param, L'\n');

		
			try
			{
				for(size_t i = 0; i != params.size(); ++i)
				{
					const wchar_t *val = params[i].c_str() + 1;
					if( params[i][0] == 'N' )
						command->Parameters->Append(command->CreateParameter(L"", adDouble, adParamInput, 20, val));  
					else if( params[i][0] == 'D' )
						command->Parameters->Append(command->CreateParameter(L"", adDBDate, adParamInput, 20, val));  
					else
						command->Parameters->Append(command->CreateParameter(L"", adVarChar, adParamInput, params[i].length(), val));
				}

				callback(command);
			}
			catch(_com_error &e)
			{
				throw std::runtime_error((const char *)e.Description());
			}
		}
	}


	void execute_sql(const _ConnectionPtr &connect, const std::wstring &sql, const std::wstring &param, std::vector<char> &buf)
	{
		_RecordsetPtr recordset;

		detail::execute(connect, sql, param, adCmdText, [&recordset](const _CommandPtr &command)
		{
			test_hr(recordset.CreateInstance(__uuidof(Recordset)));
			recordset->CursorLocation = adUseClient;
			recordset->Open(static_cast<IDispatch *>(command), vtMissing, adOpenStatic, adLockBatchOptimistic, adOptionUnspecified);
		});

		IStreamPtr stream;
		stream.CreateInstance(__uuidof(IStream));

		if( ado::save_rs(recordset, stream) != S_OK ||
			!ado::stream_2_memory(stream, buf) )
			throw std::runtime_error("recorset serialize error");
	}

	void execute_sql(const _ConnectionPtr &connect, const std::wstring &sql, const std::wstring &param, std::uint32_t &affect)
	{
		_variant_t val;
		detail::execute(connect, sql, param, adCmdText, [&val](const _CommandPtr &command)
		{
			VARIANT tmp = {0};
			command->Execute(&tmp, 0, adCmdUnknown);
			val = tmp;
		});

		affect = val;
	}

	_RecordsetPtr execute_sql(const _ConnectionPtr &connect, const std::wstring &sql, const std::wstring &param)
	{
		_RecordsetPtr recordset;

		detail::execute(connect, sql, param, adCmdText, [&recordset](const _CommandPtr &command)
		{
			test_hr(recordset.CreateInstance(__uuidof(Recordset)));
			recordset->CursorLocation = adUseClient;
			recordset->Open(static_cast<IDispatch *>(command), vtMissing, adOpenStatic, adLockBatchOptimistic, adOptionUnspecified);
		});

		return recordset;
	}

	void execute_call(const _ConnectionPtr &connect, const std::wstring &sql, const std::wstring &param, std::vector<char> &buf)
	{
		_RecordsetPtr recordset;
		detail::execute(connect, sql, param, adCmdStoredProc, [&recordset](const _CommandPtr &command)
		{
			recordset = command->Execute(0, 0, adCmdStoredProc);
		});

		IStreamPtr stream;
		stream.CreateInstance(__uuidof(IStream));

		if( ado::save_rs(recordset, stream) != S_OK ||
			!ado::stream_2_memory(stream, buf) )
			throw std::runtime_error("recorset serialize error");
	}

	HRESULT save_rs(const _RecordsetPtr &pRS, IStreamPtr &stream)
	{
		HRESULT hr = S_OK;

		try
		{
			// QI and return IPersistStream
			IPersistStreamPtr pIPersist(pRS);
			if( pIPersist )
			{
				//Create a standard stream in memory
				if( FAILED(hr = ::CreateStreamOnHGlobal(0, TRUE, &stream)) )
					return hr;

				// Persist the pRS
				if (FAILED(hr = ::OleSaveToStream(pIPersist, stream)))
					return hr;
			}
			else
				return E_NOINTERFACE;
		}
		catch (_com_error & e)
		{
			return e.Error();
		}

		return hr;
	}


	bool stream_2_memory(const IStreamPtr &stream, std::vector<char> &buf)
	{
		ULARGE_INTEGER ulnSize = {0};
		LARGE_INTEGER lnOffset = {0};

		/* get the stream size */
		if( stream->Seek(lnOffset, STREAM_SEEK_END, &ulnSize) != S_OK )
		{
			return false;
		}

		if( stream->Seek(lnOffset, STREAM_SEEK_SET, NULL) != S_OK )
		{
			return false;
		}

		/* read it */
		buf.resize((size_t)ulnSize.QuadPart);

		ULONG bytesRead = 0;

		if( stream->Read(&buf[0], buf.size(), &bytesRead) != S_OK )
			return false;

		assert(ulnSize.QuadPart == bytesRead);
		if( ulnSize.QuadPart != bytesRead )
			return false;

		return true;
	}


	IStreamPtr memory_2_stream(const char *buf, size_t len)
	{
		HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, len);
		void *pData = ::GlobalLock(hGlobal);
		memcpy(pData, buf, len);
		::GlobalUnlock(hGlobal);

		IStreamPtr stream;
		HRESULT res = ::CreateStreamOnHGlobal(hGlobal, TRUE, &stream);
		assert(res == S_OK);

		//::GlobalFree(hGlobal);

		return stream;
	}

	HRESULT load_rs(_RecordsetPtr &ppRS, const IStreamPtr &pStream)
	{
		HRESULT hr = S_OK;

		try
		{
			if( !pStream )
				return E_NOINTERFACE;

			// Load the pRS.
			LARGE_INTEGER li = {0};

			//Set the pointer to the beginning of the stream
			if( FAILED(hr = pStream->Seek(li, STREAM_SEEK_SET, 0)) )
				return hr;

			if( FAILED(hr = ::OleLoadFromStream(pStream, __uuidof(_Recordset), reinterpret_cast<LPVOID *>(&ppRS))) )
				return hr;
		}
		catch(_com_error &e)
		{
			assert(!(const wchar_t *)e.Description());
			return e.Error();
		}

		return S_OK;
	}

}