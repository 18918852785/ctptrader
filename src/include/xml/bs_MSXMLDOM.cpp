/// \file		MSXMLDOM.cpp
/// \brief		Windows上XML解析实现类的定义文件
/// \author		朱荣勋
/// \version	1.0
/// \date		2011-04-27
/// \history	2011-04-27 新建

#ifdef _MSC_VER
#include <atlbase.h>
#include <atlconv.h>
#endif

#if (_MSC_VER > 0)
#include "bs_MSXMLDOM.h"

#include <sstream>

#ifdef _DEBUG
#define new	new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace cosmos{
	std::string bstr2stdstring_and_free(BSTR s)
	{
		std::string ret;
		if( s )
		{
			_bstr_t bt(s,false);
			ret = (const char*)bt;
		}
		return ret;
	}

MSXML_DOMAttributes::~MSXML_DOMAttributes()
{
    if(m_pNodeMap) m_pNodeMap->Release();
}

bool MSXML_DOMAttributes::get( const std::string& name, std::string& value )
{
    if(!m_pNodeMap) return false;
    MSXML2::IXMLDOMNode* pNode = NULL;
    m_pNodeMap->getNamedItem(_bstr_t(name.c_str()), &pNode);
    if( pNode == NULL ) return false;

    BSTR result = 0;
    pNode->get_text(&result);
    value = bstr2stdstring_and_free(result);
    pNode->Release();
    return true;
}

bsDOMAttributes::map MSXML_DOMAttributes::toMap()
{
    return bsDOMAttributes::map();
}

MSXML_DOMNode::~MSXML_DOMNode()
{
    m_pNode->Release();
}

DOMNodePtr MSXML_DOMNode::getFirstChildNode()
{
    MSXML2::IXMLDOMNode* pNode = NULL;
    m_pNode->get_firstChild(&pNode);
    if( pNode == NULL ) return DOMNodePtr();
    return DOMNodePtr(new MSXML_DOMNode(pNode));
}

DOMNodePtr MSXML_DOMNode::getNextSiblingNode()
{
    MSXML2::IXMLDOMNode* pNode = NULL;
    m_pNode->get_nextSibling(&pNode);
    if( pNode == NULL ) return DOMNodePtr();
    return DOMNodePtr(new MSXML_DOMNode(pNode));
}

DOMAttributesPtr MSXML_DOMNode::getAttributes()
{
    return DOMAttributesPtr(new MSXML_DOMAttributes(m_pNode));
}

std::string MSXML_DOMNode::getName()
{
    BSTR result = 0;
    m_pNode->get_nodeName(&result);
	return bstr2stdstring_and_free(result);
}

std::string MSXML_DOMNode::getText()
{
    BSTR result = 0;
    m_pNode->get_text(&result);
	return bstr2stdstring_and_free(result);
}

MSXML_DOMDocument::MSXML_DOMDocument() throw(DOMException)
    : m_pDoc(NULL)
{
    if(FAILED(CoInitializeEx( 0, COINIT_MULTITHREADED )))
        if(FAILED(CoInitializeEx( 0, COINIT_APARTMENTTHREADED )))
            throw DOMException("Could not initialize COM");

    HRESULT hr = CoCreateInstance(
        CLSID_DOMDocument, NULL, CLSCTX_ALL, __uuidof( MSXML2::IXMLDOMDocument2 ),
        ( void ** ) & m_pDoc );

    if ( hr != S_OK )
        throw( DOMException( "MSXML DOM Document could not be created" ) );
}

MSXML_DOMDocument::~MSXML_DOMDocument()
{
    if(m_pDoc != NULL)
        m_pDoc->Release();
    CoUninitialize();
}

bool MSXML_DOMDocument::load( std::istream& stream )
{
    try
    {
        m_pDoc->put_async(VARIANT_FALSE);
        m_pDoc->put_resolveExternals(VARIANT_FALSE);
        m_pDoc->setProperty(_bstr_t("SelectionLanguage"), _variant_t("XPath"));

        std::stringstream sstream;
        sstream << stream.rdbuf();

        VARIANT_BOOL success = FALSE;
        m_pDoc->loadXML(_bstr_t(sstream.str().c_str()), &success);
        return success != FALSE;
    }
    catch( ... ) { return false; }
}

bool MSXML_DOMDocument::load( const std::string& url )
{
    try
    {
        m_pDoc->put_async(VARIANT_FALSE);
        m_pDoc->put_resolveExternals(VARIANT_FALSE);
        m_pDoc->setProperty(_bstr_t("SelectionLanguage"), _variant_t("XPath"));

        VARIANT_BOOL success = FALSE;
        m_pDoc->load(_variant_t(url.c_str()), &success);
        return success != FALSE;
    }
    catch( ... ) { return false; }
}

bool MSXML_DOMDocument::xml( std::ostream& out )
{
    try
    {
        BSTR result = 0;
        HRESULT hr = m_pDoc->get_xml(&result);
        if( hr != S_OK ) return false;
        
		auto s = bstr2stdstring_and_free(result);
		out << s;
        return true;
    }
    catch( ... ) { return false; }
}

DOMNodePtr MSXML_DOMDocument::getNode( const std::string& XPath )
{
	HRESULT hr;

	MSXML2::IXMLDOMNode* pNode = NULL;
	hr = m_pDoc->selectSingleNode(_bstr_t(XPath.c_str()), &pNode);
	if( pNode == NULL ) return DOMNodePtr();
	return DOMNodePtr(new MSXML_DOMNode(pNode));
}

DOMNodeListPtr MSXML_DOMDocument::getNodes( const std::string& XPath )
{
	HRESULT hr;

	MSXML2::IXMLDOMNodeListPtr pNode;
	hr = m_pDoc->selectNodes(_bstr_t(XPath.c_str()), &pNode);
	if( hr != S_OK )
		return DOMNodeListPtr();
	long len = 0;
	hr = pNode->get_length(&len);
	if( hr != S_OK )
		return DOMNodeListPtr();

	DOMNodeListPtr ret( new DOMNodeList() );
	ret->resize( len );
	for(long i=0; i<len;i++)
	{
		MSXML2::IXMLDOMNode* p = 0;
		hr = pNode->get_item(i, &p);
		if( hr != S_OK )
			return DOMNodeListPtr();
		DOMNodePtr n(new MSXML_DOMNode(p));
		(*ret.get())[i] = n;
	}
	return ret;
}


}
#endif