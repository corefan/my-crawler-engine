#include "StdAfx.h"
#include "./xml.h"
#include <vector>


namespace xml
{
	Xml::Xml(void)
	: m_strFilePath(_T(""))
	, m_pDoc(NULL)
	, m_emVersion(MSXML_UNKNOWN)
	{
	}

	Xml::~Xml(void)
	{
		Close();
	}

	//-------------------------------------------------------------------------
	// Function Name    :IsFileExist	[static]
	// Parameter(s)     :CString strFilePath
	// Return           :BOOL
	// Memo             :Whether the file exist
	//-------------------------------------------------------------------------
	bool Xml::IsFileExist(LPCTSTR filePath)
	{
		bool bExist = false;
		HANDLE hFile = NULL;

		hFile = CreateFile( filePath
			, GENERIC_READ
			, FILE_SHARE_READ | FILE_SHARE_WRITE
			, NULL
			, OPEN_EXISTING
			, 0
			, 0
			);

		if( hFile != INVALID_HANDLE_VALUE )
		{
			CloseHandle( hFile );
			bExist = true;
		}

		return (bExist);
	}


	//-------------------------------------------------------------------------
	// Function Name    :CreateInstance
	// Parameter(s)     :
	// Return           :
	// Create			:2007-8-2 9:22 Jerry.Wang
	// Memo             :Create the MSXML instance
	//-------------------------------------------------------------------------
	bool Xml::CreateInstance(void)
	{
		Close();

		HRESULT hr = S_FALSE;

 		hr = (hr == S_OK) ? hr : m_pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument60) );
 		m_emVersion = (m_emVersion == 0 && hr == S_OK) ? MSXML60 : m_emVersion;	
 		hr = (hr == S_OK) ? hr : m_pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument30) );
 		m_emVersion = (m_emVersion == 0 && hr == S_OK) ? MSXML30 : m_emVersion;
		hr = (hr == S_OK) ? hr : m_pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument50) );
		m_emVersion = (m_emVersion == 0 && hr == S_OK) ? MSXML50 : m_emVersion;
		hr = (hr == S_OK) ? hr : m_pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument40) );
		m_emVersion = (m_emVersion == 0 && hr == S_OK) ? MSXML40 : m_emVersion;		
		hr = (hr == S_OK) ? hr : m_pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument26) );
		m_emVersion = (m_emVersion == 0 && hr == S_OK) ? MSXML26 : m_emVersion;
		hr = (hr == S_OK) ? hr : m_pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument) );
		m_emVersion = (m_emVersion == 0 && hr == S_OK) ? MSXML_UNKNOWN : m_emVersion;
		if( hr != S_OK ) 
		{
			// did u CoInitialize ?
			// did u install MSXML?
			ASSERT(false);
			return false;
		}

		m_pDoc->setProperty( _bstr_t(_T("SelectionLanguage")), _variant_t(_T("XPath")) );	// 3.0 only
		if( m_emVersion > MSXML40 )
        {
            m_pDoc->setProperty( _bstr_t(_T("NewParser")), _variant_t(true) );
            m_pDoc->setProperty( _bstr_t(_T("AllowXsltScript")), _variant_t(true));
        }

		m_pDoc->setProperty( _bstr_t(_T("AllowDocumentFunction")), _variant_t(true));
		m_pDoc->resolveExternals = VARIANT_TRUE; 
		m_pDoc->preserveWhiteSpace = VARIANT_FALSE;
		m_pDoc->validateOnParse = VARIANT_FALSE;
		m_strFilePath = _T("");
		m_mpNamespace.clear();

		return true;
	}

	//-------------------------------------------------------------------------
	// Function Name    :Open
	// Parameter(s)     :LPCTSTR lpszXmlFilePath
	//					:LPCTSTR lpszRootName		The name of the root element if create
	// Return           :BOOL		false when failed
	// Memo             :open xml file, if not exist then create a new one
	//-------------------------------------------------------------------------
	bool Xml::Open( LPCTSTR lpszXmlFilePath )
	{
		if( !IsFileExist(lpszXmlFilePath) )
			return false;

		if( !CreateInstance() )
			return false;

		m_strFilePath = lpszXmlFilePath;
		VARIANT_BOOL vbSuccessful = VARIANT_TRUE;

		try
		{
			vbSuccessful = m_pDoc->load( _variant_t(lpszXmlFilePath) );
		}
		catch( const _com_error &e )
		{
			TRACE( _T("Xml::Open( %s ) failed:%s\n"), lpszXmlFilePath, e.ErrorMessage());
			vbSuccessful = false;
			ASSERT( FALSE );
		}
		catch(...)
		{
			vbSuccessful = false;
			ASSERT( FALSE );
		}

		return (vbSuccessful == VARIANT_TRUE);
	}

	//-------------------------------------------------------------------------
	// Function Name    :Create
	// Parameter(s)     :lpszRootName		The name of the root node
	//					:lpszPrefix			The prefix of the root node [optional]
	//					:lpszNamespaceURI	The namespace URL of the root node [optional]
	// Return           :BOOL
	// Create			:2008-1-16 14:22 Jerry.Wang
	// Memo             :Create a new xml file
	//-------------------------------------------------------------------------
	bool Xml::Create( LPCTSTR lpszRootName /* = _T("xmlRoot") */
		, LPCTSTR lpszPrefix /* = _T("") */
		, LPCTSTR lpszNamespaceURI /* = _T("") */
		)
	{
		HRESULT hr = S_FALSE;

		if( !CreateInstance() )
			return false;

		m_strFilePath = _T("");
		VARIANT_BOOL vbSuccessful = VARIANT_TRUE;

		try
		{
			MSXML2::IXMLDOMProcessingInstructionPtr pPI = NULL;
			pPI = m_pDoc->createProcessingInstruction( _bstr_t(_T("xml")), _bstr_t(_T("version=\"1.0\" ")) );
			if( pPI == NULL )
			{
				ASSERT(FALSE);
				return false;
			}

			m_pDoc->appendChild(pPI);

			// create the root element
			XmlNode root = CreateNode( lpszRootName, lpszPrefix, lpszNamespaceURI);
			m_pDoc->appendChild(root.m_pNode);

			vbSuccessful = SUCCEEDED(hr) ? VARIANT_TRUE : VARIANT_FALSE;
		}
		catch(const _com_error &e)
		{
			TRACE( _T("Xml::Create( %s, %s, %s) failed:%s\n")
				, lpszRootName
				, lpszPrefix
				, lpszNamespaceURI
				, e.ErrorMessage()
				);
			vbSuccessful = FALSE;
			ASSERT( false );
		}
		catch(...)
		{
			vbSuccessful = FALSE;
			ASSERT( false );
		}

		return (vbSuccessful == VARIANT_TRUE);
	}

	//-------------------------------------------------------------------------
	// Function Name    :LoadXml
	// Parameter(s)     :LPCTSTR lpszXmlContent
	// Return           :BOOL
	// Memo             :Load the xml content
	//-------------------------------------------------------------------------
	bool Xml::LoadXml(LPCTSTR lpszXmlContent)
	{
		if( !CreateInstance() )
			return false;

		VARIANT_BOOL vbSuccessful = VARIANT_TRUE;

		try
		{
			vbSuccessful = m_pDoc->loadXML( _bstr_t(lpszXmlContent) );
		}
		catch(const _com_error &e)
		{
			TRACE( _T("Xml::LoadXml failed:%s\n"), e.ErrorMessage());
			vbSuccessful = false;
			ASSERT( FALSE );
		}
		catch(...)
		{
			vbSuccessful = false;
			ASSERT( FALSE );
		}

		return (vbSuccessful == VARIANT_TRUE);
	}


	//-------------------------------------------------------------------------
	// Function Name    :Close
	// Parameter(s)     :void
	// Return           :void
	// Memo             :Release
	//-------------------------------------------------------------------------
	void Xml::Close(void)
	{
		m_mpNamespace.clear();
		m_strFilePath.clear();
		m_emVersion = MSXML_UNKNOWN;
	}


	

	//-------------------------------------------------------------------------
	// Function Name    :GetXmlFile
	// Parameter(s)     :void
	// Return           :CString
	// Create			:2008-1-16 10:58 Jerry.Wang
	// Memo             :Get the path of the xml file
	//-------------------------------------------------------------------------
	const tString &Xml::GetXmlFile(void) const
	{
		return m_strFilePath;
	}


	//-------------------------------------------------------------------------
	// Function Name    :CreateNode
	// Parameter(s)     :LPCTSTR lpszName			node local name
	//					:LPCTSTR lpszPrefix			node prefix
	//					:LPCTSTR lpszNamespaceURI	namespace URI
	// Return           :CXmlNodePtr
	// Create			:2007-8-2 9:59 Jerry.Wang
	//-------------------------------------------------------------------------
	XmlNodePtr Xml::CreateNode(LPCTSTR lpszName
		, LPCTSTR lpszPrefix /* = _T("") */
		, LPCTSTR lpszNamespaceURI /* = _T("") */
		)
	{
		ASSERT( m_pDoc != NULL );
		
		XmlNodePtr pChild( new XmlNode() );

		tString strName;
		if( _tcslen(lpszPrefix) > 0 )
		{
			strName += lpszPrefix;
			strName += _T(":");
			strName += lpszName;
		}
		else
		{
			strName += lpszPrefix;
			strName += lpszName;
		}

		try
		{	
			MSXML2::IXMLDOMNodePtr pChildNode = NULL;
			pChildNode = m_pDoc->createNode(_variant_t(_T("element")), _bstr_t(strName.c_str()), _bstr_t(lpszNamespaceURI) );
			ASSERT( pChildNode != NULL );
			pChild->m_pNode = pChildNode;
		}
		catch(const _com_error &e)
		{
			TRACE( _T("Xml::CreateNode( %s, %s, %s) failed:%s\n"), lpszName, lpszPrefix, lpszNamespaceURI, e.ErrorMessage());
			ASSERT( false );
		}
		return pChild;
	}



	//-------------------------------------------------------------------------
	// Function Name    :AddSelectionNamespace
	// Parameter(s)     :LPCTSTR lpszPrefix	The prefix, should not contain xmlns
	//					:LPCTSTR lpszURI
	// Return           :
	// Create			:2007-8-1 18:06 Jerry.Wang
	// Memo             :Add the selection namespace for the XPath
	//-------------------------------------------------------------------------
	void Xml::AddSelectionNamespace( LPCTSTR lpszPrefix, LPCTSTR lpszURI)
	{
		ASSERT( m_pDoc != NULL );

		try
		{
			m_mpNamespace[lpszPrefix] = lpszURI;
			tString strNamespaces;
			std::map< tString, tString>::iterator iter;
			for( iter = m_mpNamespace.begin(); iter != m_mpNamespace.end(); iter++)
			{
				tString strNamespace = _T("xmlns:");
				strNamespace += iter->first + _T("=") + _T('\'') + iter->second + _T('\'');

				strNamespaces += strNamespace;
			}

			m_pDoc->setProperty( _T("SelectionNamespaces"), _variant_t(strNamespaces.c_str()));
		}
		catch(const _com_error &e)
		{
			TRACE( _T("Xml::AddNamespace( %s, %s) failed:%s\n"), lpszPrefix, lpszURI, e.ErrorMessage());
			ASSERT( FALSE );
		}
	}

	
	//-------------------------------------------------------------------------
	// Function Name    :GetRoot
	// Parameter(s)     :
	// Return           :
	// Memo             :get the root element
	//-------------------------------------------------------------------------
	XmlNodePtr Xml::GetRoot(void)
	{	
		ASSERT( m_pDoc != NULL );

		XmlNodePtr pNode( new XmlNode() );

		try
		{
			MSXML2::IXMLDOMElementPtr pElement = NULL;
			HRESULT hr = m_pDoc->get_documentElement(&pElement);
			ASSERT( SUCCEEDED(hr) );
			ASSERT( pElement != NULL );

			pNode->m_pNode = pElement;
		}
		catch(const _com_error &e)
		{
			TRACE( _T("Xml::GetRoot failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
		}
		
		return pNode;
	}


	//-------------------------------------------------------------------------
	// Function Name    :SaveWithFormatted
	// Parameter(s)     :LPCTSTR lpszFilePath	[in, optional] 
	// Return           :BOOL
	// Memo             :Save the xml file
	//-------------------------------------------------------------------------
	bool Xml::Save(LPCTSTR lpszFilePath)
	{
		if( m_pDoc == NULL )
		{
			ASSERT(FALSE);
			return false;
		}

		HRESULT hr = S_OK;

		try
		{
			if( lpszFilePath == NULL )
			{
				ASSERT(m_strFilePath.empty());
				hr = m_pDoc->save( _variant_t(m_strFilePath.c_str()));
			}
			else
			{
				hr = m_pDoc->save( _variant_t( lpszFilePath ) );
				if( SUCCEEDED(hr) )
					m_strFilePath = lpszFilePath;
			}
			return SUCCEEDED(hr);
		}
		catch(const _com_error &e)
		{
			TRACE( _T("Xml::Save( %s ) failed:%s\n"), lpszFilePath, e.ErrorMessage());
			ASSERT( FALSE );
			hr = e.Error();
		}

		return SUCCEEDED(hr);
	}
	//-------------------------------------------------------------------------
	// Function Name    :SaveWithFormatted
	// Parameter(s)     :LPCTSTR lpszFilePath	[in, optional] 
	//					:LPCTSTR lpszEncoding	[in, optional] 
	// Return           :BOOL
	// Memo             :Save the xml file in formatted
	// History			:V3.1 Thanks roel_'s advice to support formatted output
	//-------------------------------------------------------------------------
	bool Xml::SaveWithFormatted(LPCTSTR lpszFilePath /* = NULL */, LPCTSTR lpszEncoding /* = _T("UTF-8") */)
	{
		if( m_pDoc == NULL )
		{
			ASSERT(false);
			return FALSE;
		}

		HRESULT hr = S_OK;

		MSXML2::IMXWriterPtr pMXWriter = NULL;
		MSXML2::ISAXXMLReaderPtr pSAXReader = NULL;
		MSXML2::ISAXContentHandlerPtr pISAXContentHandler = NULL;
		MSXML2::ISAXErrorHandlerPtr pISAXErrorHandler = NULL;
		MSXML2::ISAXDTDHandlerPtr pISAXDTDHandler = NULL;
		try
		{
		
			// create
			hr = pMXWriter.CreateInstance( __uuidof(MSXML2::MXXMLWriter));
			if( FAILED(hr) )
			{ ASSERT(false); return false; }
			
			hr = pSAXReader.CreateInstance( __uuidof(MSXML2::SAXXMLReader));
			if( FAILED(hr) )
			{ ASSERT(FALSE); return false; }

			// save in formatted
			pISAXContentHandler = pMXWriter;
			pISAXErrorHandler = pMXWriter;
			pISAXDTDHandler = pMXWriter;

			if (FAILED (pMXWriter->put_omitXMLDeclaration (VARIANT_FALSE)) ||
				FAILED (pMXWriter->put_standalone (VARIANT_FALSE)) ||
				FAILED (pMXWriter->put_indent (VARIANT_TRUE)) ||
				FAILED (pMXWriter->put_encoding(_bstr_t(lpszEncoding)) )
				)
			{
				ASSERT(false);
				return SUCCEEDED(hr);
			}

			if (FAILED(pSAXReader->putContentHandler (pISAXContentHandler)) ||
				FAILED(pSAXReader->putDTDHandler (pISAXDTDHandler)) ||
				FAILED(pSAXReader->putErrorHandler (pISAXErrorHandler)) ||
				FAILED(pSAXReader->putProperty ((unsigned short*)L"http://xml.org/sax/properties/lexical-handler", _variant_t(pMXWriter.GetInterfacePtr()))) ||
				FAILED(pSAXReader->putProperty ((unsigned short*)L"http://xml.org/sax/properties/declaration-handler", _variant_t(pMXWriter.GetInterfacePtr()))))
			{
				ASSERT(FALSE);
				return SUCCEEDED(hr);
			}
						
			IStream * pOutStream = NULL;
			::CreateStreamOnHGlobal( NULL, true, &pOutStream);
			hr = pMXWriter->put_output(_variant_t(pOutStream));
			if( FAILED(hr) )
			{ ASSERT(false); return false; }

			hr = pSAXReader->parse(m_pDoc.GetInterfacePtr());
			if( FAILED(hr) )
			{ ASSERT(FALSE); return false; }
					
			if( lpszFilePath == NULL )
			{
				ASSERT(m_strFilePath.empty());
				if( !Xml::SaveStreamToFile( pOutStream, m_strFilePath.c_str()) )
					return false;
			}
			else
			{
				if( !Xml::SaveStreamToFile( pOutStream, lpszFilePath) )
					return false;
				m_strFilePath = lpszFilePath;
			}			
		}
		catch(const _com_error &e)
		{
			TRACE( _T("Xml::SaveWithFormatted( %s ) failed:%s\n"), lpszFilePath, e.ErrorMessage());
			ASSERT( false );
			hr = e.Error();
		}

		return SUCCEEDED(hr);
    }
    //-------------------------------------------------------------------------
    // Function Name    :SaveToMemory
    // Parameter(s)     :String& out
    // Return           :bool
    // Memo             :±£´æXMLÊý¾Ýµ½×Ö·û´®
    //-------------------------------------------------------------------------
    bool Xml::SaveToMemory(tString& out)
    {
        bool result = false;
        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, 0);

        IStream* pStream = NULL;
        if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
        {
            assert(pStream != NULL);
            if (m_pDoc->save(_variant_t(pStream)) == S_OK)
            {
                DWORD size = GlobalSize(hGlobal);
                char* pData = static_cast<char*>(GlobalLock(hGlobal));
                out = CA2T(pData, CP_UTF8);
                GlobalUnlock(hGlobal);
                result = true;
            }
            else
            {
                assert(0 && "±£´æXMLµ½×Ö·û´®Ê§°Ü");
            }
            pStream->Release();
        }
        GlobalFree(hGlobal);
        return result;
    }
	//-------------------------------------------------------------------------
	// Function Name    :SaveStreamToFile
	// Parameter(s)     :IStream * pStream
	//					:LPCTSTR lpszFilePath
	// Return           :BOOL
	// Memo             :Save IStream to file, this method is inner used by JWXml
	//-------------------------------------------------------------------------
	bool Xml::SaveStreamToFile(IStream * pStream, LPCTSTR lpszFilePath)
	{
		HRESULT hr = S_OK;
		LPVOID pOutput = NULL;
		HGLOBAL hGlobal = NULL;

		hr = ::GetHGlobalFromStream( pStream, &hGlobal);
		if( FAILED(hr) ) { ASSERT(FALSE); return false; }
		::GlobalUnlock(hGlobal);
		LARGE_INTEGER llStart = {0, 0};
		ULARGE_INTEGER ullSize = {0, 0};
		pStream->Seek( llStart, STREAM_SEEK_CUR, &ullSize);
		pOutput = ::GlobalLock(hGlobal);

		HANDLE hFile = ::CreateFile( lpszFilePath
			, GENERIC_WRITE
			, FILE_SHARE_WRITE | FILE_SHARE_READ
			, NULL
			, CREATE_ALWAYS
			, 0
			, NULL
			);
		if( hFile == INVALID_HANDLE_VALUE )
		{
			ASSERT(FALSE);
			return false;
		}

		DWORD dwWritten = 0;
		::WriteFile( hFile, pOutput, (UINT)ullSize.QuadPart, &dwWritten, NULL);
		::FlushFileBuffers(hFile);
		::CloseHandle(hFile);

		::GlobalUnlock(hGlobal);

		return true;
	}



	//-------------------------------------------------------------------------
	// Function Name    :SelectSingleNode
	// Parameter(s)     :LPCTSTR strPath 
	// Return           :CXmlNodePtr
	// Memo             :Query node by XPath
	//-------------------------------------------------------------------------
	XmlNodePtr Xml::SelectSingleNode(LPCTSTR lpszPath)
	{
		ASSERT( m_pDoc != NULL );

		XmlNodePtr pNode ( new XmlNode() );

		if( !GetRoot()->IsNull() )
			(*pNode) = GetRoot()->SelectSingleNode(lpszPath);
		
		return pNode;
	}

	//-------------------------------------------------------------------------
	// Function Name    :SelectNodes
	// Parameter(s)     :LPCTSTR strPath	
	// Return           :CXmlNodesPtr
	// Memo             :Query nodes by XPath
	//-------------------------------------------------------------------------
	XmlNodesPtr Xml::SelectNodes(LPCTSTR lpszPath)
	{
		ASSERT( m_pDoc != NULL );

		XmlNodesPtr pNodes( new XmlNodes() );

		if( !GetRoot()->IsNull() )
			(*pNodes) = GetRoot()->SelectNodes(lpszPath);

		return pNodes;
	}



	//-------------------------------------------------------------------------
	// Function Name    :EncodeBase64
	// Parameter(s)     :LPBYTE *pBuf	The binary buffer
	//					:ULONG ulSize	size
	// Return           :CString		the result
	// Memo             :encoding the binary buffer into Base64 string
	//-------------------------------------------------------------------------
	bool Xml::Base64Encode(tString &inoutBuf IN OUT, LPBYTE pBuf, ULONG ulSize)
	{
		ASSERT( m_pDoc != NULL );
		ASSERT( pBuf != NULL );
		
		try
		{
			MSXML2::IXMLDOMElementPtr pElement = NULL;
			pElement = m_pDoc->createElement( _bstr_t(_T("Base64")) );
			ASSERT( pElement != NULL );

			HRESULT hr = S_OK;
			hr = pElement->put_dataType( _bstr_t(_T("bin.base64")) );
			ASSERT( SUCCEEDED(hr) );

			SAFEARRAY * pAry = SafeArrayCreateVector( VT_UI1, 0L, ulSize);
			::memcpy( pAry->pvData, pBuf, ulSize);

			VARIANT var;
			VariantInit(&var);
			var.parray = pAry;
			var.vt = VT_ARRAY | VT_UI1;
			pElement->nodeTypedValue = var;

			BSTR bstr = NULL;
			hr = pElement->get_text( &bstr );
			ASSERT( SUCCEEDED(hr) );	
			inoutBuf = (LPCTSTR)_bstr_t( bstr, true);

			if( bstr != NULL )
			{
				SysFreeString(bstr);
				bstr = NULL;
			}

		}
		catch(const _com_error &e )
		{
			TRACE( _T("Xml::Base64Encode failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );

			return false;
		}

		return true;
	}



	//-------------------------------------------------------------------------
	// Function Name    :Base64Decode
	// Parameter(s)     :CString strIn		The base64 string
	//					:LPBYTE pBuf		The output buffer
	//					:ULONG * ulSize		the site of buffer
	// Return           :BOOL
	// Memo             :Decode the base64 string into buffer
	//					:NOTE: If the pBuf equal to NULL, then the lSize is the buffer length
	//					:Please see demo
	//-------------------------------------------------------------------------
	bool Xml::Base64Decode(const tString &strIn IN, LPBYTE pBuf IN OUT, LONG & lSize IN OUT)
	{
		ASSERT( m_pDoc != NULL );
		
		try
		{
			MSXML2::IXMLDOMElementPtr pElement = NULL;
			pElement = m_pDoc->createElement( _bstr_t(_T("Base64")) );
			ASSERT( pElement != NULL );

			HRESULT hr = S_OK;
			hr = pElement->put_dataType( _bstr_t(_T("bin.base64")) );
			ASSERT( SUCCEEDED(hr) );

			hr = pElement->put_text( _bstr_t(strIn.c_str()) );
			ASSERT( SUCCEEDED(hr) );

			hr = SafeArrayGetUBound( pElement->nodeTypedValue.parray, 1, &lSize);
			ASSERT( SUCCEEDED(hr) );
			lSize ++;
			if( pBuf )
			{
				memset( pBuf, 0, lSize);

				memcpy( pBuf, LPVOID(pElement->nodeTypedValue.parray->pvData), lSize);	
			}		
		}
		catch(const _com_error &e)
		{
			TRACE( _T("CXml::Base64Decode failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
			return false;
		}
		
		return true;
	}

	//-------------------------------------------------------------------------
	// Function Name    :GetVersion
	// Parameter(s)     :void
	// Return           :MSXML_VERSION
	// Create			:2008-1-16 15:05 Jerry.Wang
	// Memo             :Get the version of current MSXML
	//-------------------------------------------------------------------------
	MSXML_VERSION Xml::GetVersion(void) const
	{
		return m_emVersion;
	}

}