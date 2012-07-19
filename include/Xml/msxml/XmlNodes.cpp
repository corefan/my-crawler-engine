#include "StdAfx.h"
#include "./XmlNodes.h"


namespace xml
{


	// constructors
	XmlNodes::XmlNodes(MSXML2::IXMLDOMNodeListPtr pNodeList)
	{
		m_pNodeList = pNodeList;
	}

	XmlNodes::XmlNodes(const XmlNodes & refNodes )
	{
		m_pNodeList = refNodes.m_pNodeList;
	}

	XmlNodes::XmlNodes(const XmlNodesPtr &pNodes )
	{
		m_pNodeList = pNodes->m_pNodeList;
	}

	XmlNodes::XmlNodes(void)
	: m_pNodeList(NULL)
	{
	}

	XmlNodes::~XmlNodes(void)
	{
		Release();
	}

	//////////////////////////////////////////////////////////////////////////
	void XmlNodes::Release(void)
	{
	}


	//-------------------------------------------------------------------------
	// Function Name    :operator =
	// Parameter(s)     :CXmlNodesPtr pNodes
	// Return           :CXmlNodesPtr
	// Memo             :override the = symbol
	//-------------------------------------------------------------------------
	XmlNodesPtr XmlNodes::operator = ( XmlNodesPtr pNodes IN )
	{
		m_pNodeList = pNodes->m_pNodeList;
		return pNodes;
	}


	//-------------------------------------------------------------------------
	// Function Name    :operator =
	// Parameter(s)     :CXmlNodes & refNodes
	// Return           :CXmlNodes &
	// Memo             :override the = symbol
	//-------------------------------------------------------------------------
	XmlNodes & XmlNodes::operator = (const XmlNodes & refNodes)
	{
		m_pNodeList = refNodes.m_pNodeList;
		return (*this);
	}



	//-------------------------------------------------------------------------
	// Function Name    :GetCount
	// Parameter(s)     :void
	// Return           :LONG
	// Memo             :get the total count of the elements in this list
	//-------------------------------------------------------------------------
	LONG XmlNodes::GetCount(void)
	{
		ASSERT( m_pNodeList != NULL );

		try
		{
			return m_pNodeList->length;
		}
		catch(const _com_error &e)
		{
			TRACE( _T("XmlNodes::GetCount failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
			return -1;
		}
	}



	//-------------------------------------------------------------------------
	// Function Name    :GetItem
	// Parameter(s)     :LONG nIndex	The index of the item
	// Return           :CXmlNodePtr
	// Memo             :Get the item in the list
	//-------------------------------------------------------------------------
	XmlNodePtr XmlNodes::GetItem( LONG nIndex )
	{
		ASSERT( m_pNodeList != NULL );
		ASSERT( nIndex >= 0 && nIndex < GetCount() );

		XmlNodePtr pNode ( new XmlNode(m_pNodeList->item[nIndex]) );

		return pNode;
	}


	//-------------------------------------------------------------------------
	// Function Name    :operator[]
	// Parameter(s)     :INT nIndex
	// Return           :CXmlNodePtr
	// Memo             :override [], get the item in the list by index
	//-------------------------------------------------------------------------
	XmlNodePtr XmlNodes::operator[] ( LONG lIndex )
	{
		return GetItem(lIndex);
	}


	//-------------------------------------------------------------------------
	// Function Name    :GetItem
	// Parameter(s)     :LPCTSTR lpszName	the child node name
	// Return           :CXmlNodePtr
	// Memo             :get the child node by the node name
	//-------------------------------------------------------------------------
	XmlNodePtr XmlNodes::GetItem( LPCTSTR lpszName )
	{
		ASSERT( m_pNodeList != NULL );
		ASSERT( lpszName != NULL );
		
		XmlNodePtr pNode ( new XmlNode() );

		try
		{
			HRESULT hr = S_OK;
			tString strName;
			BSTR bstr = NULL;
			MSXML2::IXMLDOMNodePtr pItem = NULL;	

			for( int i = 0; i < m_pNodeList->length; i++)
			{
				pItem = m_pNodeList->item[i];

				hr = pItem->get_nodeName(&bstr);		

				strName = (LPCTSTR)_bstr_t( bstr, true);
				if( bstr != NULL )
				{
					SysFreeString(bstr);
					bstr = NULL;
				}

				// get the first element
				if( strName == lpszName )
				{
					pNode->m_pNode = pItem;
					return pNode;
				}
			}
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNodes::GetItem failed:%s\n"), e.ErrorMessage());
			ASSERT( false );
		}

		return pNode;
	}


	//-------------------------------------------------------------------------
	// Function Name    :operator[]
	// Parameter(s)     :LPCTSTR lpszName	
	// Return           :CXmlNodePtr
	// Memo             :override the [], same as GetItem( LPCTSTR lpszName )
	//-------------------------------------------------------------------------
	XmlNodePtr XmlNodes::operator[] ( LPCTSTR lpszName )
	{
		return GetItem(lpszName);
	}

}