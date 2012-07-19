#include "StdAfx.h"
#include "./xmlnode.h"
#include "../../Extend STL/StringAlgorithm.h"


namespace xml
{

	//-------------------------------------------------------------------------
	// Function Name    :_GetValue
	// Parameter(s)     :CString & strValue OUT	value
	// Return           :BOOL					false means failed
	// Memo             :get the text value of the current node
	//					:NOTE: the node should not contain children if u invoke this method
	//-------------------------------------------------------------------------
	bool XmlNode::_GetValue(tString & strValue OUT) const
	{
		ASSERT( !IsNull() );

		HRESULT hr = S_OK;
		try
		{
			BSTR bstr = NULL;
			hr = m_pNode->get_text( &bstr );
			ASSERT( SUCCEEDED(hr) );	
			strValue = (LPCTSTR)_bstr_t( bstr, true);

			if( bstr != NULL )
			{
				SysFreeString(bstr);
				bstr = NULL;
			}
		}
		catch(const _com_error &e)
		{
			TRACE( _T("XmlNode::_GetValue failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
			return false;
		}
		
		return SUCCEEDED(hr);
	}



	//-------------------------------------------------------------------------
	// Function Name    :_SetValue
	// Parameter(s)     :CString & strValue IN
	// Return           :BOOL					false means failed
	// Memo             :Set the text value of the current node
	//-------------------------------------------------------------------------
	bool XmlNode:: _SetValue(const tString & strValue IN) const
	{
		ASSERT( !IsNull() );

		HRESULT hr = S_OK;

		try
		{
			hr = m_pNode->put_text( _bstr_t(strValue.c_str()) );
			ASSERT( SUCCEEDED(hr) );
		}
		catch(const _com_error &e)
		{
			TRACE( _T("XmlNode::_SetValue failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
			return false;
		}
		return SUCCEEDED(hr);
	}



	//-------------------------------------------------------------------------
	// Function Name    :_GetAttribute
	// Parameter(s)     :CString & strName	IN	
	// Return			:CString & strValue OUT	
	// Memo             :Get the attribute value
	//-------------------------------------------------------------------------
	bool XmlNode::_GetAttribute(const tString & strName IN, tString & strValue OUT) const
	{
		if( m_pNode == NULL )
		{
			ASSERT(false);
			return false;
		}

		HRESULT hr = S_OK;

		try
		{
			MSXML2::IXMLDOMNamedNodeMapPtr pIXMLDOMNamedNodeMap = NULL;
			hr = m_pNode->get_attributes(&pIXMLDOMNamedNodeMap);
			if( !SUCCEEDED(hr) )
			{
				ASSERT( false );
				return false;
			}

			MSXML2::IXMLDOMNodePtr pIXMLDOMNode = NULL;
			pIXMLDOMNode = pIXMLDOMNamedNodeMap->getNamedItem( _bstr_t(strName.c_str()) );

			if( pIXMLDOMNode == NULL )
			{
				strValue.clear();
			}
			else
			{
				VARIANT varValue;
				hr = pIXMLDOMNode->get_nodeValue(&varValue);
				if( !SUCCEEDED(hr) )
				{
					ASSERT( false );
					return false;
				}

				strValue = (LPCTSTR)(_bstr_t)varValue;
			}
		}
		catch(const _com_error &e)
		{
			TRACE( _T("XmlNode::_GetAttribute failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
			return false;
		}

		return true;
	}


	//-------------------------------------------------------------------------
	// Function Name    :_SetAttribute
	// Parameter(s)     :CString & strName	IN	
	//					:CString & strValue IN  
	// Return           :BOOL
	// Memo             :Set the attribute value
	//-------------------------------------------------------------------------
	bool XmlNode::_SetAttribute(const tString & strName IN
								, const tString & strValue IN
								, const tString & strPrefix IN
								, const tString & strNamespaceURI IN
								) const
	{
		if( m_pNode == NULL )
		{
			ASSERT(false);
			return false;
		}

		HRESULT hr = S_OK;
		try
		{
			MSXML2::IXMLDOMDocumentPtr pDoc = NULL;
			hr = m_pNode->get_ownerDocument(&pDoc);
			ASSERT( SUCCEEDED(hr) );
			
			tString strFullName;
			if( strPrefix.empty() )
				strFullName += strPrefix + strName;
			else
				strFullName += strPrefix + _T(":") + strName;

			MSXML2::IXMLDOMAttributePtr pAttribute = NULL;
			pAttribute = pDoc->createNode( _variant_t(_T("attribute")), _bstr_t(strFullName.c_str()), _bstr_t(strNamespaceURI.c_str()) );
			pAttribute->Putvalue(_variant_t(strValue.c_str()));
			ASSERT( pAttribute != NULL );
			
			MSXML2::IXMLDOMElementPtr pElement = static_cast<MSXML2::IXMLDOMElementPtr> (m_pNode);
			hr = pElement->setAttributeNode(pAttribute);
		}
		catch(const _com_error &e)
		{
			TRACE( _T("XmlNode::_SetAttribute failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
			return false;
		}

		return SUCCEEDED(hr);
	}





	//-------------------------------------------------------------------------
	// Function Name    :CXmlNode
	// Parameter(s)     :MSXML2::IXMLDOMNodePtr pNode	[in]
	// Memo             :constructor
	//-------------------------------------------------------------------------
	XmlNode::XmlNode( MSXML2::IXMLDOMNodePtr pNode IN)
	{
		m_pNode = pNode;
	}


	//-------------------------------------------------------------------------
	// Function Name    :CXmlNode
	// Parameter(s)     :void
	// Memo             :constructor
	//-------------------------------------------------------------------------
	XmlNode::XmlNode(void)
	{
		m_pNode = NULL;	
	}


	//-------------------------------------------------------------------------
	// Function Name    :CXmlNode
	// Parameter(s)     :CXmlNode & refNode [in]
	// Memo             :constructor
	//-------------------------------------------------------------------------
	XmlNode::XmlNode( const XmlNode & refNode IN)
	{
		m_pNode = refNode.m_pNode;
	}

	//-------------------------------------------------------------------------
	// Function Name    :CXmlNode
	// Parameter(s)     :CXmlNode & refNode [in]
	// Memo             :constructor
	//-------------------------------------------------------------------------
	XmlNode::XmlNode( const XmlNodePtr pNode IN)
	{
		m_pNode = pNode->m_pNode;
	}

	//-------------------------------------------------------------------------
	// Function Name    :~CXmlNode
	// Parameter(s)     :void
	// Memo             :destructor 
	//-------------------------------------------------------------------------
	XmlNode::~XmlNode(void)
	{
	}


	//-------------------------------------------------------------------------
	// Function Name    :Release
	// Parameter(s)     :void
	// Return           :void
	// Memo             :release the node
	//-------------------------------------------------------------------------
	void XmlNode::Release(void)
	{
	}




	//-------------------------------------------------------------------------
	// Function Name    :operator =
	// Parameter(s)     :CXmlNodePtr pNode	[in]
	// Return           :CXmlNodePtr
	// Memo             :override the =
	//-------------------------------------------------------------------------
	XmlNodePtr XmlNode::operator = (const XmlNodePtr &pNode IN)
	{
		m_pNode = pNode->m_pNode;
		return pNode;
	}


	//-------------------------------------------------------------------------
	// Function Name    :operator =
	// Parameter(s)     :CXmlNode & refNode	[in]
	// Return           :CXmlNode &
	// Memo             :override the =
	//-------------------------------------------------------------------------
	XmlNode & XmlNode::operator = (const XmlNode & refNode IN)
	{
		m_pNode = refNode.m_pNode;
		return (*this);
	}


	//-------------------------------------------------------------------------
	// Function Name    :IsNull
	// Parameter(s)     :void
	// Return           :BOOL
	// Memo             :Whether this node exist
	//-------------------------------------------------------------------------
	bool XmlNode::IsNull(void) const
	{
		return m_pNode == NULL;
	}

	//-------------------------------------------------------------------------
	// Function Name    :GetChild
	// Parameter(s)     :CString strName		节点名称
	//					:BOOL bBuildIfNotExist	true: create if the node not exist
	// Return           :The child node pointer
	// Memo             :get the child
	//-------------------------------------------------------------------------
	XmlNodePtr XmlNode::GetChild(LPCTSTR pszName, bool bBuildIfNotExist /* = true */)
	{
		ASSERT( m_pNode != NULL );

		XmlNodePtr pChild( new XmlNode() );

		tString strName = pszName;
		try
		{
			MSXML2::IXMLDOMNodePtr pChildNode = NULL;
			tString strXPath;

			//strName.replace(strName.find(_T("'")), 1, _T("''"));
			static const tString obj(_T("'")), dest(_T("''"));
			stdex::Replace(strName, obj, dest);
			strXPath = _T("*[local-name(.) = '") + strName + _T("']");

			pChildNode = m_pNode->selectSingleNode(_bstr_t(strXPath.c_str()));

			// create if not exist
			if( pChildNode == NULL && bBuildIfNotExist )
			{
				MSXML2::IXMLDOMDocumentPtr pDoc = NULL;
				HRESULT hr = m_pNode->get_ownerDocument(&pDoc);
				ASSERT( SUCCEEDED(hr) );

				pChildNode = pDoc->createElement( _bstr_t(strName.c_str()) );
				ASSERT( pChildNode != NULL );

				m_pNode->appendChild(pChildNode);
			}

			pChild->m_pNode = pChildNode;	
		}
		catch(const _com_error &e)
		{
			TRACE( _T("XmlNode::GetChild failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
		}

		return pChild;
	}


	//-------------------------------------------------------------------------
	// Function Name    :NewChild
	// Parameter(s)     :CString strName	the node name
	// Return           :CXmlNodePtr
	// Memo             :create new child
	//-------------------------------------------------------------------------
	XmlNodePtr XmlNode::NewChild(LPCTSTR lpszName )
	{
		ASSERT( m_pNode != NULL );

		XmlNodePtr pChild( new XmlNode() );

		try
		{
			MSXML2::IXMLDOMDocumentPtr pDoc = NULL;
			HRESULT hr = m_pNode->get_ownerDocument(&pDoc);
			ASSERT( SUCCEEDED(hr) );

			MSXML2::IXMLDOMNodePtr pChildNode = NULL;
			pChildNode = pDoc->createElement( _bstr_t(lpszName) );
			ASSERT( pChildNode != NULL );

			m_pNode->appendChild(pChildNode);
			pChild->m_pNode = pChildNode;
		}
		catch(const _com_error &e)
		{
			TRACE( _T("XmlNode::NewChild failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
		}
		return pChild;	
	}


	//-------------------------------------------------------------------------
	// Function Name    :AttachChild
	// Parameter(s)     :XmlNodePtr & pChildNode
	// Return           :void
	// Memo             :Attach a new child
	//-------------------------------------------------------------------------
	void XmlNode::AttachChild( XmlNodePtr & pChildNode)
	{
		ASSERT( m_pNode != NULL );

		try
		{
			m_pNode->appendChild( pChildNode->m_pNode );
		}
		catch(const _com_error &e)
		{
			TRACE( _T("CXmlNode::AttachChild failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
		}
	}


	//-------------------------------------------------------------------------
	// Function Name    :AttachChild
	// Parameter(s)     :CXmlNode & refChildNode
	// Return           :void
	// Memo             :Attach a new child
	//-------------------------------------------------------------------------
	void XmlNode::AttachChild( XmlNode & refChildNode)
	{
		ASSERT( m_pNode != NULL );

		try
		{
			m_pNode->appendChild( refChildNode.m_pNode );
		}
		catch(const _com_error &e)
		{
			TRACE( _T("XmlNode::AttachChild failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
		}
	}


	//-------------------------------------------------------------------------
	// Function Name    :GetParent
	// Parameter(s)     :void
	// Return           :CXmlNodePtr
	// Memo             :get the parent node
	//-------------------------------------------------------------------------
	XmlNodePtr XmlNode::GetParent(void)
	{
		ASSERT( m_pNode != NULL );

		XmlNodePtr pParent( new XmlNode() );

		try
		{
			pParent->m_pNode = m_pNode->GetparentNode();
		}
		catch(const _com_error &e)
		{
			TRACE( _T("XmlNode::GetParent failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
		}
		return pParent;
	}




	//-------------------------------------------------------------------------
	// Function Name    :GetName
	// Parameter(s)     :void
	// Return           :CString
	// Memo             :get the name of the current node
	//-------------------------------------------------------------------------
	bool XmlNode::GetName(tString &strName) const
	{
		ASSERT( m_pNode != NULL );

		try
		{
			BSTR bstr = NULL;
			HRESULT hr = m_pNode->get_nodeName(&bstr);
			ASSERT( SUCCEEDED(hr) );	
			strName = (LPCTSTR)_bstr_t( bstr, true);

			if( bstr != NULL )
			{
				SysFreeString(bstr);
				bstr = NULL;
			}
		}
		catch(const _com_error &e)
		{
			TRACE( _T("XmlNode::GetName failed:%s\n"), e.ErrorMessage());
			ASSERT( false );

			return false;
		}

		return true;
	}

	//-------------------------------------------------------------------------
	// Function Name    :RemoveAttribute
	// Parameter(s)     :CString strName IN 属性名称
	// Return           :BOOL
	// Memo             :remove attribute
	//-------------------------------------------------------------------------
	bool XmlNode::RemoveAttribute( const tString &strName IN )
	{
		ASSERT( m_pNode != NULL );

		HRESULT hr = S_OK;

		try
		{
			MSXML2::IXMLDOMNamedNodeMapPtr pIXMLDOMNamedNodeMap = NULL;
			hr = m_pNode->get_attributes(&pIXMLDOMNamedNodeMap);
			ASSERT( SUCCEEDED(hr) );

			if( SUCCEEDED(hr) )
				pIXMLDOMNamedNodeMap->removeNamedItem( _bstr_t(strName.c_str()) );
			ASSERT( SUCCEEDED(hr) );
		}
		catch(const _com_error &e)
		{
			TRACE( _T("CXmlNode::RemoveAttribute failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
			return false;
		}

		
		return SUCCEEDED(hr);
	}



	//-------------------------------------------------------------------------
	// Function Name    :HasChildren
	// Parameter(s)     :void
	// Return           :BOOL
	// Memo             :whether the node has children
	//-------------------------------------------------------------------------
	bool XmlNode::HasChildren(void)
	{
		ASSERT( m_pNode != NULL );

		bool bHasChildren = false;
		try
		{
			MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
			pNodeList =	m_pNode->selectNodes( _bstr_t(_T("child::*")) );
			ASSERT( pNodeList != NULL );

			bHasChildren = pNodeList->length > 0;
		}
		catch(const _com_error &e)
		{
			TRACE( _T("CXmlNode::HasChildren failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
		}
		return bHasChildren;
	}



	//-------------------------------------------------------------------------
	// Function Name    :Detach
	// Parameter(s)     :
	// Return           :
	// Memo             :detach the current node from parent
	//-------------------------------------------------------------------------
	XmlNode & XmlNode::Detach(void)
	{
		ASSERT( m_pNode != NULL );

		try
		{
			HRESULT hr = S_OK;
			MSXML2::IXMLDOMNodePtr pNode = NULL;
			hr = m_pNode->get_parentNode(&pNode);
			ASSERT( SUCCEEDED(hr) );

			pNode->removeChild(m_pNode);
		}
		catch(const _com_error &e)
		{
			TRACE( _T("XmlNode::Detach failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
		}
		return (*this);
	}

	//-------------------------------------------------------------------------
	// Function Name    :RemoveChildren
	// Parameter(s)     :
	// Return           :BOOL
	// Memo             :remove all children
	//-------------------------------------------------------------------------
	bool XmlNode::RemoveChildren(void)
	{
		ASSERT( m_pNode != NULL );

		try
		{
			MSXML2::IXMLDOMNodeListPtr pNodeList = m_pNode->childNodes;

			for( int i = pNodeList->length - 1; i >= 0; i--)
			{
				m_pNode->removeChild( pNodeList->item[i] );
			}
		}
		catch(const _com_error &e)
		{
			TRACE( _T("XmlNode::Remove failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
			return false;
		}

		return true;
	}


	//-------------------------------------------------------------------------
	// Function Name    :GetChildren
	// Parameter(s)     :void
	// Return           :CXmlNodesPtr
	// Memo             :get all children nodes
	//-------------------------------------------------------------------------
	XmlNodesPtr XmlNode::GetChildren()
	{
		ASSERT( m_pNode != NULL );

		XmlNodesPtr pNodes ( new XmlNodes() );
		try
		{
			MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
			pNodeList =	m_pNode->selectNodes(_bstr_t(_T("child::*")));

			pNodes->m_pNodeList = pNodeList;
		}
		catch(const _com_error &e)
		{
			TRACE( _T("XmlNode::GetChildren failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
		}
		return pNodes;
	}




	//-------------------------------------------------------------------------
	// Function Name    :SelectSingleNode
	// Parameter(s)     :LPCTSTR lpszPath		XPATH
	// Return           :CXmlNodePtr
	// Memo             :XPath selectSingleNode
	//-------------------------------------------------------------------------
	XmlNodePtr XmlNode::SelectSingleNode(LPCTSTR lpszPath)
	{
		ASSERT( m_pNode != NULL );

		XmlNodePtr pNode ( new XmlNode() );

		try
		{
			MSXML2::IXMLDOMNodePtr pItem = NULL;
			pItem = m_pNode->selectSingleNode( _bstr_t(lpszPath) );

			pNode->m_pNode = pItem;
		}
		catch(const _com_error &e)
		{
			TRACE( _T("XmlNode::SelectSingleNode( %s ) failed:%s\n"), lpszPath, e.ErrorMessage());
			ASSERT( false );
		}

		return pNode;
	}



	//-------------------------------------------------------------------------
	// Function Name    :SelectNodes
	// Parameter(s)     :LPCTSTR lpszPath		XPATH
	// Return           :CXmlNodesPtr
	// Memo             :XPath selectNodes
	//-------------------------------------------------------------------------
	XmlNodesPtr XmlNode::SelectNodes(LPCTSTR lpszPath)
	{
		ASSERT( m_pNode != NULL );

		XmlNodesPtr pNodes ( new XmlNodes() );
		try
		{
			MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
			pNodeList =	m_pNode->selectNodes( _bstr_t(lpszPath) );

			pNodes->m_pNodeList = pNodeList;
		}
		catch(const _com_error &e)
		{
			TRACE( _T("CXmlNode::SelectNodes failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
		}

		return pNodes;
	}




	//////////////////////////////////////////////////////////////////////////
	// the following methods are getting value

	// get CString value
	tString XmlNode::GetValue(LPCTSTR lpszValue /* = NULL */ ) const
	{
		tString strValue;
		_GetValue(strValue);

		if( strValue.empty() &&
			lpszValue != NULL )
		{
			strValue = lpszValue;
			_SetValue(strValue);
		}

		return strValue;
	}

	// get bool value
	//bool XmlNode::GetValue( bool bDefault ) const
	//{
	//	tString strValue;
	//	_GetValue(strValue);

	//	if( strValue == _T("1") )
	//	{
	//		return true;
	//	}
	//	else if( strValue == _T("0") )
	//	{
	//		return false;
	//	}
	//	else
	//	{
	//		strValue = bDefault ? _T("1") : _T("0");
	//		_SetValue(strValue);
	//		return bDefault;
	//	}	
	//}


	//////////////////////////////////////////////////////////////////////////
	// the following methods are setting value

	// set LPCTSTR value
	bool XmlNode::SetValue( LPCTSTR lpszValue )
	{
		tString strValue(lpszValue);
		return _SetValue(strValue);
	}

	// set bool value
	/*bool XmlNode::SetValue( bool bValue )
	{
		tString strValue;
		strValue = bValue ? _T("1") : _T("0");

		return _SetValue(strValue);
	}*/


	//////////////////////////////////////////////////////////////////////////
	// The following methods are getting attribute

	// get LPCTSTR attribute
	tString XmlNode::GetAttribute(const tString &strName, LPCTSTR lpszDefault /* = NULL */) const
	{
		tString strValue;
		_GetAttribute( strName, strValue);

		if( strValue.empty() && lpszDefault != NULL )
		{
			strValue = lpszDefault;
		}
		return strValue;
	}

	// get bool attribute
	/*bool XmlNode::GetAttribute(const tString &strName, bool bDefault) const
	{
		tString strValue;
		_GetAttribute( strName, strValue);

		if( strValue == _T("1") )
		{
			return true;
		}
		else if( strValue == _T("0") )
		{
			return false;
		}
		else
		{
			return bDefault;
		}	
	}*/


	//////////////////////////////////////////////////////////////////////////
	// The following methods are setting attribute

	// set LPCTSTR attribute
	bool XmlNode::SetAttribute(const tString &strName
		, LPCTSTR lpszValue
		, const tString &strPrefix /* = _T("") */
		, const tString &strNamespaceURI /* = _T("") */
		)
	{
		tString strValue = lpszValue;

		return _SetAttribute( strName, strValue, strPrefix, strNamespaceURI);	
	}

	// set bool attribute
	//bool XmlNode::SetAttribute(const tString &strName
	//	, bool bValue
	//	, const tString &strPrefix /* = _T("") */
	//	, const tString &strNamespaceURI /* = _T("") */
	//	)
	//{
	//	tString strValue;
	//	strValue = bValue ? _T("1") : _T("0");

	//	return _SetAttribute( strName, strValue, strPrefix, strNamespaceURI);
	//}

	

	//-------------------------------------------------------------------------
	// Function Name    :GetOuterHTML
	// Parameter(s)     :void
	// Return           :CString
	// Create			:2008-1-14 11:04 王嘉
	// Memo             :Get OuterXml
	//-------------------------------------------------------------------------
	void XmlNode::GetOuterXml(tString &outerXML IN OUT) const
	{
		ASSERT(!IsNull());

		try
		{
			if( !IsNull() )
			{
				HRESULT hr = S_OK;
				BSTR bstr = NULL;
				hr = m_pNode->get_xml(&bstr);

				ASSERT( SUCCEEDED(hr) );	
				outerXML = (LPCTSTR)_bstr_t( bstr, true);

				if( bstr != NULL )
				{
					SysFreeString(bstr);
					bstr = NULL;
				}
			}
		}
		catch(const _com_error &e)
		{
			TRACE( _T("CXmlNode::GetOuterXml failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
		}
	}// GetOuterXml


	//-------------------------------------------------------------------------
	// Function Name    :GetInnerXml
	// Parameter(s)     :void
	// Return           :
	// Memo             :get InnerXml
	//-------------------------------------------------------------------------
	void XmlNode::GetInnerXml(tString &innerXML IN OUT) const
	{
		ASSERT(!IsNull());

		GetOuterXml(innerXML);
		size_t nFirst = innerXML.find('>');
		size_t nEnd = innerXML.rfind('<');
		if( nFirst > 0 && nEnd > 0 && nEnd > nFirst )
			innerXML = innerXML.assign( innerXML.begin() + nFirst, innerXML.begin() + nEnd - nFirst - 1);

	}// GetOuterXml

}