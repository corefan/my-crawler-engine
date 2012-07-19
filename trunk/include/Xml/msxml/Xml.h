#ifndef __XML_XML_HPP
#define __XML_XML_HPP


#include "./XmlNode.h"
#include "./Xsl.h"
#include <map>








namespace xml
{



	typedef enum
	{
		MSXML_UNKNOWN	= 00,
		MSXML26 		= 26,
		MSXML30 		= 30,
		MSXML40 		= 40,
		MSXML50 		= 50,
		MSXML60 		= 60,
	} MSXML_VERSION;


	class Xml
	{
		friend class Xsl;

	public:
		Xml(void);
		~Xml(void);

	private:
		MSXML2::IXMLDOMDocument2Ptr m_pDoc; 
		tString						m_strFilePath;
		MSXML_VERSION				m_emVersion;
		std::map<tString, tString>	m_mpNamespace;

		bool CreateInstance(void);

	public:
		// Open xml file
		bool Open(LPCTSTR lpszXmlFilePath);

		// Create a new xml file
		bool Create( LPCTSTR lpszRootName = _T("xmlRoot")
			, LPCTSTR lpszPrefix = _T("")
			, LPCTSTR lpszNamespaceURI = _T("")
			);

		// Load Xml string
		bool LoadXml(LPCTSTR lpszXmlContent);

		// save xml file with formatted output
		bool Save(LPCTSTR lpszFilePath);
		bool SaveWithFormatted(LPCTSTR lpszFilePath = NULL, LPCTSTR lpszEncoding = _T("UTF-8"));
        bool SaveToMemory(tString& out);
		// close xml file
		void Close(void);

		const tString &GetXmlFile(void) const;

		// Encode the binary data into string
		bool Base64Encode(tString &strInOut IN OUT, LPBYTE pBuf, ULONG ulSize);

		// Decode the string into binary data
		bool Base64Decode( const tString &strIn IN, LPBYTE pBuf, LONG & lSize);
		

		// namespace
		void AddSelectionNamespace( LPCTSTR lpszPrefix, LPCTSTR lpszURI);

		// get the root element of 
		XmlNodePtr GetRoot(void);

		// get single node by XPath
		XmlNodePtr SelectSingleNode(LPCTSTR lpszPath);

		// get nodes by XPath
		XmlNodesPtr SelectNodes(LPCTSTR lpszPath);

		// create node
		XmlNodePtr CreateNode(LPCTSTR lpszName
			, LPCTSTR lpszPrefix = _T("")
			, LPCTSTR lpszNamespaceURI = _T("")
			);

		// get the current version of MSXML
		MSXML_VERSION GetVersion(void) const;

		// whether file exist
		static bool IsFileExist(LPCTSTR filePath);

		// save the stream to file
		static bool SaveStreamToFile(IStream * pStream, LPCTSTR lpszFilePath);
	};
}



#endif