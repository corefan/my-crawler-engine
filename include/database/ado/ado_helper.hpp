#ifndef __ADO_HELPER_HPP
#define __ADO_HELPER_HPP

#import "c:\program files\common files\system\ado\msado15.dll" rename("EOF","adoEOF")

#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>


namespace ado
{
	using namespace ADODB;

	

	// ADO Connect Command Event
	class connect_event 
		: public ConnectionEventsVt 
	{
	public:
		typedef std::function<void(std::uint32_t, const Error *)> select_complete_type;

	private:
		volatile long ref_;
		select_complete_type select_callback_; 

	public:
		connect_event();
		~connect_event();

	public:
		void register_callbacks(const select_complete_type &select_callback);

	public:
		STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();

		STDMETHODIMP raw_InfoMessage(Error *pError, EventStatusEnum *adStatus, _Connection *pConnection);
		STDMETHODIMP raw_BeginTransComplete(LONG TransactionLevel, Error *pError, EventStatusEnum *adStatus, _Connection *pConnection);
		STDMETHODIMP raw_CommitTransComplete(Error *pError, EventStatusEnum *adStatus, _Connection *pConnection);
		STDMETHODIMP raw_RollbackTransComplete(Error *pError, EventStatusEnum *adStatus, _Connection *pConnection);

		STDMETHODIMP raw_WillExecute(BSTR *Source, CursorTypeEnum *CursorType, LockTypeEnum *LockType, long *Options,
			EventStatusEnum *adStatus, _Command *pCommand, _Recordset *pRecordset, _Connection *pConnection);
		STDMETHODIMP raw_ExecuteComplete(LONG RecordsAffected, Error *pError, EventStatusEnum *adStatus,
			_Command *pCommand, _Recordset *pRecordset, _Connection *pConnection);

		STDMETHODIMP raw_WillConnect(BSTR *ConnectionString, BSTR *UserID, BSTR *Password,
			long *Options, EventStatusEnum *adStatus, _Connection *pConnection);
		STDMETHODIMP raw_ConnectComplete(Error *pError, EventStatusEnum *adStatus, _Connection *pConnection);
		STDMETHODIMP raw_Disconnect(EventStatusEnum *adStatus, _Connection *pConnection);
	};


	// The Recordset events
	class recordset_event 
		: public RecordsetEventsVt 
	{
	private:
		volatile long ref_;

	public:
		recordset_event();
		~recordset_event();

	public:
		STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();

		STDMETHODIMP raw_WillChangeField(LONG cFields, VARIANT Fields, EventStatusEnum *adStatus, _Recordset *pRecordset);
		STDMETHODIMP raw_FieldChangeComplete(LONG cFields, VARIANT Fields, Error *pError, EventStatusEnum *adStatus, _Recordset *pRecordset);
		STDMETHODIMP raw_WillChangeRecord(EventReasonEnum adReason, LONG cRecords, EventStatusEnum *adStatus, _Recordset *pRecordset);
		STDMETHODIMP raw_RecordChangeComplete(EventReasonEnum adReason, LONG cRecords, Error *pError, EventStatusEnum *adStatus, _Recordset *pRecordset);
		STDMETHODIMP raw_WillChangeRecordset(EventReasonEnum adReason, EventStatusEnum *adStatus, _Recordset *pRecordset);

		STDMETHODIMP raw_RecordsetChangeComplete(EventReasonEnum adReason, Error *pError, EventStatusEnum *adStatus, _Recordset *pRecordset);
		STDMETHODIMP raw_WillMove(EventReasonEnum adReason, EventStatusEnum *adStatus, _Recordset *pRecordset);
		STDMETHODIMP raw_MoveComplete(EventReasonEnum adReason, Error *pError, EventStatusEnum *adStatus, _Recordset *pRecordset);
		STDMETHODIMP raw_EndOfRecordset(VARIANT_BOOL *fMoreData, EventStatusEnum *adStatus, _Recordset *pRecordset);

		STDMETHODIMP raw_FetchProgress(long Progress, long MaxProgress, EventStatusEnum *adStatus, _Recordset *pRecordset);
		STDMETHODIMP raw_FetchComplete(struct Error *pError, EventStatusEnum *adStatus, _Recordset *pRecordset);
	};


	template < typename T >
	struct event_helper
	{
		IUnknown &ado_event_;
		IUnknownPtr ado_type_;
		DWORD eventID_;

		event_helper(IUnknown &ado_event, const IUnknownPtr &ado_type)
			: ado_event_(ado_event)
			, ado_type_(ado_type)
			, eventID_(0)
		{
			IConnectionPointContainerPtr cpc(ado_type_);
			IConnectionPointPtr cp;
			test_hr(cpc->FindConnectionPoint(__uuidof(T), &cp));

			IUnknownPtr unknown;
			unknown = &ado_event_;
			if( !unknown ) 
				_com_issue_error(S_FALSE);

			test_hr(cp->Advise(unknown, &eventID_));
		}

		~event_helper()
		{
			assert(eventID_ != 0);

			try
			{
				IConnectionPointContainerPtr cpc(ado_type_);
				IConnectionPointPtr cp;
				test_hr(cpc->FindConnectionPoint(__uuidof(T), &cp));

				IUnknownPtr unknown;
				unknown = &ado_event_;
				if( !unknown ) 
					_com_issue_error(S_FALSE);

				test_hr(cp->Unadvise(eventID_));
			}
			catch(_com_error &e)
			{
				assert((const char *)e.Description() && 0);
			}
		}
	};

	typedef event_helper<ado::ConnectionEvents> connection_events;
	typedef event_helper<ado::RecordsetEvents>	recordset_events;

	typedef std::shared_ptr<connection_events>	connection_events_ptr;
	typedef std::shared_ptr<recordset_events>	recordset_events_ptr;


	inline connection_events_ptr make_connection_event(IUnknown &event, const IUnknownPtr &type)
	{
		return connection_events_ptr(new connection_events(event, type));
	}

	inline recordset_events_ptr make_recordset_event(IUnknown &event, const IUnknownPtr &type)
	{
		return recordset_events_ptr(new recordset_events(event, type));
	}

	
	// -------------------------------

	inline void test_hr(HRESULT x) 
	{
		try
		{
			if( FAILED(x) )
				_com_issue_error(x);
		}
		catch(_com_error &e)
		{
			throw std::runtime_error((const char *)e.Description());
		}
	};


	template < typename AdoT >
	void valid(const AdoT &type)
	{
		assert(type && type->State == ado::adStateOpen);
		if( !type || type->State != ado::adStateOpen )
			throw std::logic_error("ADO object is invalid");
	}

	template < typename AdoT >
	bool is_open(const AdoT &type)
	{
		return type && type->State == ado::adStateOpen;
	}

	void execute_sql(const _ConnectionPtr &connect, const std::wstring &sql, const std::wstring &param, std::vector<char> &buf);
	void execute_sql(const _ConnectionPtr &connect, const std::wstring &sql, const std::wstring &param, std::uint32_t &affect);
	_RecordsetPtr execute_sql(const _ConnectionPtr &connect, const std::wstring &sql, const std::wstring &param);

	void execute_call(const _ConnectionPtr &connect, const std::wstring &sql, const std::wstring &param, std::vector<char> &buf);

	HRESULT save_rs(const _RecordsetPtr &rs, IStreamPtr &stream);
	HRESULT load_rs(_RecordsetPtr &rs, const IStreamPtr &stream);

	bool stream_2_memory(const IStreamPtr &stream, std::vector<char> &buf);
	IStreamPtr memory_2_stream(const char *buf, size_t len);

}




#endif