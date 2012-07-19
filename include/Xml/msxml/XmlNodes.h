#pragma once

#include "./XmlNode.h"

namespace xml
{
	class XmlNodes
	{
		friend class Xml;
		friend class XmlNode;
		friend class XmlNodes;

	public:
		//			 ____________
		//__________| constuctor |______________________________________
		~XmlNodes(void);
		XmlNodes(void);
		XmlNodes( const XmlNodes & refNodes );
		XmlNodes( const XmlNodesPtr &pNodes );

		//			 __________
		//__________| override |______________________________________
		XmlNodesPtr operator = (XmlNodesPtr pNodes);
		XmlNodes & operator = (const XmlNodes & refNodes);
		XmlNodePtr operator[] ( LONG lIndex );
		XmlNodePtr operator[] ( LPCTSTR lpszName );


		LONG GetCount(void);
		void Release(void);

		XmlNodePtr GetItem( LONG nIndex );
		XmlNodePtr GetItem( LPCTSTR lpszName );

	protected:
		XmlNodes(MSXML2::IXMLDOMNodeListPtr pNodeList);
		MSXML2::IXMLDOMNodeListPtr m_pNodeList;

	};
}