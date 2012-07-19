#pragma once
#include "./Xml.h"



namespace xml
{
	class Xsl
	{
	public:
		Xsl(void);
		~Xsl(void);

		// Open xsl file
		bool Open(LPCTSTR lpszXslFilePath);

		// close xsl file
		void Close(void);

		// transform to file
		bool TransformToFile( Xml & objXml, LPCTSTR lpszFilePath);

		// add a parameter
		bool AddParameter( LPCTSTR lpszParamName, LPCTSTR lpszParamValue, LPCTSTR lpszNamespaceURI = NULL);

	protected:
		MSXML2::IXSLTemplatePtr				m_pIXSLTemplate;
		MSXML2::IXMLDOMDocument2Ptr			m_pStyleSheet;
		MSXML2::IXSLProcessorPtr			m_pIXSLProcessor;
	};
}