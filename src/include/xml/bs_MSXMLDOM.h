/// \file		MSXMLDOM.h
/// \brief		Windows��XML����ʵ����������ļ�
/// \author		����ѫ
/// \version	1.0
/// \date		2011-04-27
/// \history	2011-04-27 �½�

#pragma once

#ifdef _MSC_VER
#pragma warning(disable : 4503 4355 4786 4290)
#endif
#include <memory>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iostream>

//#include "DOMDocument.h"
#ifdef _WIN32
#import <msxml3.dll> raw_interfaces_only

namespace cosmos
{

	/// \brief DOM ������
	class bsDOMAttributes
	{
	public:
		typedef std::unordered_map<std::string, std::string> map;

		/// \brief ��������
		virtual ~bsDOMAttributes() {}

		/// \brief ��ȡ��������
		virtual bool get(const std::string&, std::string&) = 0;
		
		/// \brief ����ӳ���ϵ
		//virtual std::unordered_map toMap() = 0;
	};
	typedef std::auto_ptr<bsDOMAttributes> DOMAttributesPtr;

/// \brief Windows��DOM������
class MSXML_DOMAttributes : public bsDOMAttributes
{
public:
	/// \brief ���캯��
	MSXML_DOMAttributes(MSXML2::IXMLDOMNode* pNode)
	{
		pNode->get_attributes(&m_pNodeMap);
	}

	/// \brief ��������
	~MSXML_DOMAttributes();

	/// \brief ��ȡ��������
	bool get(const std::string&, std::string&);
	/// \brief ����ӳ���ϵ
	bsDOMAttributes::map toMap();

private:
	MSXML2::IXMLDOMNamedNodeMap* m_pNodeMap;
};


/// \brief DOM �ڵ���
class DOMNode
{
public:
	/// \brief ��������
	virtual ~DOMNode() {}

	/// \brief ��ȡ��һ���ӽڵ�
	virtual std::auto_ptr<DOMNode> getFirstChildNode() = 0;
	/// \brief ��ȡ��һ���ֵܽڵ�
	virtual std::auto_ptr<DOMNode> getNextSiblingNode() = 0;
	/// \brief ��ȡ���Զ���ָ��
	virtual DOMAttributesPtr getAttributes() = 0;
	/// \brief ��ȡ�ڵ���
	virtual std::string getName() = 0;
	/// \brief ��ȡ�ڵ��ı�
	virtual std::string getText() = 0;
};

typedef std::auto_ptr<DOMNode> DOMNodePtr;

/// \brief Windows��DOM�ڵ���
class MSXML_DOMNode : public DOMNode
{
public:
	/// \brief ���캯��
	MSXML_DOMNode(MSXML2::IXMLDOMNode* pNode) : m_pNode(pNode) {}

	/// \brief ��������
	~MSXML_DOMNode();

	/// \brief ��ȡ��һ���ӽڵ�
	DOMNodePtr getFirstChildNode();
	/// \brief ��ȡ��һ���ֵܽڵ�
	DOMNodePtr getNextSiblingNode();
	/// \brief ��ȡ���Զ���ָ��
	DOMAttributesPtr getAttributes();
	/// \brief ��ȡ�ڵ���
	std::string getName();
	/// \brief ��ȡ�ڵ��ı�
	std::string getText();

private:
	MSXML2::IXMLDOMNode* m_pNode;
};

class DOMNodeList : public std::vector<DOMNodePtr>
{

};

typedef std::auto_ptr<DOMNodeList> DOMNodeListPtr;


/// \brief XML DOM ��������.
class DOMDocument
{
public:
	/// \brief ��������
	virtual ~DOMDocument() {}

	/// \brief ����xml��
	virtual bool load(std::istream&) = 0;
	/// \brief ����xml�ļ�
	virtual bool load(const std::string&) = 0;
	/// \brief ���xml��
	virtual bool xml(std::ostream&) = 0;

	/// \brief ��ȡ�ڵ����ָ��
	virtual DOMNodePtr getNode(const std::string&) = 0;
};

typedef std::auto_ptr<DOMDocument> DOMDocumentAPtr;


#include <unordered_map>
#include <string>
#include <stdexcept>
using namespace std;

/// \brief DOM�쳣��
class DOMException : public logic_error
{
public:
	/// \brief ���캯��
	DOMException(const string& strDescription) : logic_error(string("DOM Error: ") + strDescription) {}
	/// \brief ��������
	~DOMException() throw() {}
};

/// \brief Windows��XML����ʵ����
class MSXML_DOMDocument : public DOMDocument
{
public:
	/// \brief ���캯��
	MSXML_DOMDocument() throw(DOMException);
	/// \brief ��������
    ~MSXML_DOMDocument();

	/// \brief ����xml��
    bool load(istream&);
	/// \brief ����xml�ļ�
    bool load(const string&);
	/// \brief ���xml��
    bool xml(ostream&);

	/// \brief ��ȡ�ڵ����ָ��
    DOMNodePtr getNode(const string&);

	virtual DOMNodeListPtr getNodes( const std::string& );

  private:
    MSXML2::IXMLDOMDocument2* m_pDoc;
  };

}

#endif//win32
