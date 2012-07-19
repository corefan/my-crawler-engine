#pragma once


#include "Basic.h"

#import "msxml3.dll"   // msxml3.dll or msxml4.dll or msxml5.dll or msxml6.dll
#include <memory>
#include "../../Extend STL/StringAlgorithm.h"




namespace MSXML2
{
	struct __declspec(uuid("f6d90f11-9c73-11d3-b32e-00c04f990bb4"))
		DOMDocument;

	struct __declspec(uuid("f5078f1b-c551-11d3-89b9-0000f81fe221"))
		DOMDocument26;

	struct __declspec(uuid("f5078f32-c551-11d3-89b9-0000f81fe221"))
		DOMDocument30;

	struct __declspec(uuid("88d969c0-f192-11d4-a65f-0040963251e5"))
		DOMDocument40;

	struct __declspec(uuid("88d969e5-f192-11d4-a65f-0040963251e5"))
		DOMDocument50;

	struct __declspec(uuid("88d96a05-f192-11d4-a65f-0040963251e5"))
		DOMDocument60;
}



namespace xml
{
	class Xml;
	class XmlNode;
	class XmlNodes;

#ifdef _HAS_TR1
	typedef std::tr1::shared_ptr<XmlNode>  XmlNodePtr;
	typedef std::tr1::shared_ptr<XmlNodes>  XmlNodesPtr;
#else
	typedef std::auto_ptr<XmlNode>  XmlNodePtr;
	typedef std::auto_ptr<XmlNodes>  XmlNodesPtr;
#endif

	
}

#include ".\XmlNodes.h"

namespace xml
{
	class XmlNode
	{
		friend class Xml;
		friend class XmlNode;
		friend class XmlNodes;

	protected:		
		MSXML2::IXMLDOMNodePtr   m_pNode;

		XmlNode( MSXML2::IXMLDOMNodePtr pNode);

		bool _GetValue(tString & strValue) const;
		bool _SetValue(const tString & strValue) const;

		bool _GetAttribute(const tString & strName, tString & strValue) const;
		bool _SetAttribute( const tString & strName IN
						  , const tString & strValue IN
						  , const tString & strPrefix IN
					      , const  tString & strNamespaceURI IN
						  ) const;

	public:

		//		 _____________
		//______| constructor |___________________________________________________________________
		XmlNode(void);
		XmlNode(const XmlNode & refNode IN);
		XmlNode(const XmlNodePtr pNode IN);
		~XmlNode(void);

		//		 _______________
		//______|	override =	|___________________________________________________________________
		XmlNodePtr operator = (const XmlNodePtr &pNode);
		XmlNode & operator = (const XmlNode & refNode);



		bool IsNull(void) const; 	// Whether the current element exist
		bool GetName(tString &strName) const;// Get the name of the current node
		XmlNode & Detach(void);	// Detach the current node
		void Release(void);			// Release this node


		//		 _______________
		//______|Parent - Child	|___________________________________________________________________
		XmlNodePtr GetChild(LPCTSTR pszName, bool bBuildIfNotExist = true);
		XmlNodePtr NewChild(LPCTSTR pszName );
		XmlNodePtr GetParent(void);
		XmlNodesPtr GetChildren();
		void AttachChild( XmlNodePtr & pChildNode);
		void AttachChild( XmlNode & refChildNode);
		bool HasChildren(void);
		bool RemoveChildren(void);
        template <typename CharT>
        inline std::basic_string<CharT> GetAddress();



		//		 _______________
		//______|	attribute	|___________________________________________________________________
		tString	GetAttribute( const tString &strName, LPCTSTR lpszDefault = NULL)	const;
		template<typename T>
		T	GetAttribute( const tString &strName, const T &default)				const;	

		bool	SetAttribute( const tString &strName, LPCTSTR lpszValue, const tString &strPrefix = _T(""), const tString &strNamespaceURI = _T(""));
		template<typename T>
		bool	SetAttribute( const tString &strName, const T &value, const tString &strPrefix = _T(""), const tString &strNamespaceURI = _T(""));
	
		bool RemoveAttribute( const tString &strName );


		//		 _______
		//______| value |___________________________________________________________________	
		tString	GetValue( LPCTSTR lpszDefault = NULL )						const;
		template<typename T>
		T		GetValue(const T &default)									const;
		template<typename CharT>
        void	GetValue(std::basic_string<CharT> &val)						const;
        template<typename CharT, int iCount>
        void	GetValue(CharT(&val)[iCount])       						const;
		

		bool	SetValue( LPCTSTR lpszValue );
		template<typename T>
		bool	SetValue(const T &nValue );



		XmlNodePtr SelectSingleNode(LPCTSTR lpszPath);
		XmlNodesPtr SelectNodes(LPCTSTR lpszPath);

		//		 _______________________
		//______|	InnerXml OuterXml	|___________________________________________________________________
		void GetOuterXml(tString &outerXML IN OUT) const;
		void GetInnerXml(tString &innerXML IN OUT) const;

	};


	template<typename T>
	T XmlNode::GetValue(const T &default) const
	{
		tString strValue;
		_GetValue(strValue);

		if( strValue.empty() )
		{
			stdex::ToString(default, strValue);
			_SetValue(strValue);
		}
		
		T tmp;
		stdex::ToNumber(tmp, strValue);
		return tmp;
	}

	template < typename CharT >
	void XmlNode::GetValue(std::basic_string<CharT> &val) const
	{		
		ASSERT( !IsNull() );

		HRESULT hr = S_OK;
		try
		{
			BSTR bstr = NULL;
			hr = m_pNode->get_text( &bstr );
			ASSERT( SUCCEEDED(hr) );	
			val = (const CharT *)_bstr_t( bstr, true);

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
		}
	}

    template < typename CharT, int iCount >
    void XmlNode::GetValue(CharT(&val)[iCount]) const
    {		
        ASSERT( !IsNull() );

        HRESULT hr = S_OK;
        try
        {
            BSTR bstr = NULL;
            hr = m_pNode->get_text( &bstr );
            ASSERT( SUCCEEDED(hr) );
            _bstr_t clStr( bstr, true);
            ASSERT( clStr.length() < iCount);
            lstrcpy(val, static_cast<const CharT*>(clStr));

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
        }
    }

	template<typename T>
	bool XmlNode::SetValue(const T &value )
	{
		tString strValue;
		stdex::ToString(value, strValue);

		return _SetValue(strValue);
	}


	template<typename T>
	T XmlNode::GetAttribute(const tString &strName, const T &default) const
	{
		tString strValue;
		_GetAttribute( strName, strValue);

		if( strValue.empty() )
			stdex::ToString(default, strValue);

		T ret(0);
		stdex::ToNumber(ret, strValue);
		return ret;
	}

	template<typename T>
	bool XmlNode::SetAttribute( const tString &strName, const T &value, const tString &strPrefix/* = _T("")*/, const tString &strNamespaceURI/* = _T("")*/)
	{
		tString strValue;
		stdex::ToString(value, strValue);

		return _SetAttribute( strName, strValue, strPrefix, strNamespaceURI);
	}

    template < typename CharT >
    std::basic_string<CharT> XmlNode::GetAddress()
    {		
        ASSERT( !IsNull() );
        
        std::basic_string<CharT> sAddress;
        GetName(sAddress);
        XmlNodePtr clNode = GetParent();
        while (!clNode->IsNull())
        {
            std::basic_string<CharT> sNodeName;
            clNode->GetName(sNodeName);
            sAddress = sNodeName + std::basic_string<CharT>(_bstr_t(_T("\\"))) + sAddress;
            clNode = clNode->GetParent();
        }
        return sAddress;
    }
}
