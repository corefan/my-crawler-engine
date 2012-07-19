#include "StdAfx.h"
#include "./Xsl.h"

namespace xml
{

	Xsl::Xsl(void)
	{
	}

	Xsl::~Xsl(void)
	{
		Close();
	}

	//-------------------------------------------------------------------------
	// Function Name    :Open
	// Parameter(s)     :LPCTSTR lpszXslFilePath	XSL file path
	// Return           :BOOL
	// Create			:2008-1-16 16:09 Jerry.Wang
	// Memo             :Open the xsl file
	//-------------------------------------------------------------------------
	bool Xsl::Open(LPCTSTR lpszXslFilePath)
	{
		Close();

		HRESULT hr = S_OK;

		try
		{
			hr = m_pIXSLTemplate.CreateInstance( __uuidof(MSXML2::XSLTemplate) );
			if( FAILED(hr) ) 
			{ 
				ASSERT(false); 
				return false; 
			}

			hr = m_pStyleSheet.CreateInstance( __uuidof(MSXML2::FreeThreadedDOMDocument) );
			if( FAILED(hr) ) 
			{ 
				ASSERT(false); 
				return false; 
			}

			VARIANT_BOOL vbSuccessful = VARIANT_TRUE;
			vbSuccessful = m_pStyleSheet->load( _variant_t(lpszXslFilePath) );
			if( vbSuccessful == VARIANT_FALSE ) 
			{ 
				ASSERT(false); 
				return false; 
			}

			hr = m_pIXSLTemplate->putref_stylesheet(m_pStyleSheet);
			if( FAILED(hr) ) 
			{ 
				VERIFY(false); 
				return false; 
			}

			m_pIXSLProcessor = m_pIXSLTemplate->createProcessor();
			if( m_pIXSLProcessor == NULL ) 
			{ 
				ASSERT(false); 
				return false; 
			}

			return true;
		}
		catch(const _com_error &e)
		{
			TRACE( _T("CXsl::Open(%s) failed:%s\n"), lpszXslFilePath, e.ErrorMessage());
			ASSERT( false );
			return false;
		}
	}

	//-------------------------------------------------------------------------
	// Function Name    :Close
	// Parameter(s)     :void
	// Return           :void
	// Create			:2008-1-16 16:10 Jerry.Wang
	// Memo             :Close the xsl file
	//-------------------------------------------------------------------------
	void Xsl::Close(void)
	{
	}

	//-------------------------------------------------------------------------
	// Function Name    :AddParameter
	// Parameter(s)     :LPCTSTR lpszParamName
	//					:LPCTSTR lpszParamValue
	//					:LPCTSTR lpszNamespaceURI
	// Return           :BOOL
	// Create			:2008-1-18 22:10 Jerry.Wang
	// Memo             :Add a parameter to the XSLT
	//-------------------------------------------------------------------------
	bool Xsl::AddParameter( LPCTSTR lpszParamName, LPCTSTR lpszParamValue, LPCTSTR lpszNamespaceURI /* = _T("") */)
	{
		ASSERT(m_pIXSLProcessor != NULL);
		try
		{
			HRESULT hr = m_pIXSLProcessor->addParameter( _bstr_t(lpszParamName)
				, _variant_t(lpszParamValue)
				, _bstr_t(lpszNamespaceURI)
				);
			return SUCCEEDED(hr);
		}
		catch(const _com_error &e)
		{
			TRACE( _T("CXsl::AddParameter( %s, %s, %s) failed:%s\n")
				, lpszParamName
				, lpszParamValue
				, lpszNamespaceURI
				, e.ErrorMessage()
				);
			ASSERT( false );
			return false;
		}
	}

	//-------------------------------------------------------------------------
	// Function Name    :TransformToFile
	// Parameter(s)     :CXml & objXml			the CXml instance
	//					:LPCTSTR lpszFilePath	The destination file
	// Return           :BOOL
	// Create			:2008-1-16 16:38 Jerry.Wang
	// Memo             :Transform XML + XSL To File
	//-------------------------------------------------------------------------
	bool Xsl::TransformToFile( Xml & objXml, LPCTSTR lpszFilePath)
	{
		HRESULT hr					= S_FALSE;
		IStream * pOutStream		= NULL;
		VARIANT_BOOL vbSuccessful	= VARIANT_TRUE;

		try
		{
			::CreateStreamOnHGlobal( NULL, true, &pOutStream);
			m_pIXSLProcessor->put_output(_variant_t(pOutStream));

			hr = m_pIXSLProcessor->put_input( _variant_t((IUnknown*)objXml.m_pDoc) );
			if( FAILED(hr) ) 
			{ 
				ASSERT(false); 
				return false; 
			}

			vbSuccessful = m_pIXSLProcessor->transform();
			if( vbSuccessful == VARIANT_FALSE )
			{ 
				VERIFY(false); 
				return false; 
			}

			Xml::SaveStreamToFile( pOutStream, lpszFilePath);

			return true;
		}
		catch(const _com_error &e)
		{
			TRACE( _T("Xsl::Transform failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
			return false;
		}
	}
}