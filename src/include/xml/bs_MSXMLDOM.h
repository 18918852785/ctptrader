/// \file		MSXMLDOM.h
/// \brief		Windows上XML解析实现类的声明文件
/// \author		朱荣勋
/// \version	1.0
/// \date		2011-04-27
/// \history	2011-04-27 新建

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

	/// \brief DOM 属性类
	class bsDOMAttributes
	{
	public:
		typedef std::unordered_map<std::string, std::string> map;

		/// \brief 析构函数
		virtual ~bsDOMAttributes() {}

		/// \brief 获取属性内容
		virtual bool get(const std::string&, std::string&) = 0;
		
		/// \brief 建立映射关系
		//virtual std::unordered_map toMap() = 0;
	};
	typedef std::auto_ptr<bsDOMAttributes> DOMAttributesPtr;

/// \brief Windows上DOM属性类
class MSXML_DOMAttributes : public bsDOMAttributes
{
public:
	/// \brief 构造函数
	MSXML_DOMAttributes(MSXML2::IXMLDOMNode* pNode)
	{
		pNode->get_attributes(&m_pNodeMap);
	}

	/// \brief 析构函数
	~MSXML_DOMAttributes();

	/// \brief 获取属性内容
	bool get(const std::string&, std::string&);
	/// \brief 建立映射关系
	bsDOMAttributes::map toMap();

private:
	MSXML2::IXMLDOMNamedNodeMap* m_pNodeMap;
};


/// \brief DOM 节点类
class DOMNode
{
public:
	/// \brief 析构函数
	virtual ~DOMNode() {}

	/// \brief 获取第一个子节点
	virtual std::auto_ptr<DOMNode> getFirstChildNode() = 0;
	/// \brief 获取下一个兄弟节点
	virtual std::auto_ptr<DOMNode> getNextSiblingNode() = 0;
	/// \brief 获取属性对象指针
	virtual DOMAttributesPtr getAttributes() = 0;
	/// \brief 获取节点名
	virtual std::string getName() = 0;
	/// \brief 获取节点文本
	virtual std::string getText() = 0;
};

typedef std::auto_ptr<DOMNode> DOMNodePtr;

/// \brief Windows上DOM节点类
class MSXML_DOMNode : public DOMNode
{
public:
	/// \brief 构造函数
	MSXML_DOMNode(MSXML2::IXMLDOMNode* pNode) : m_pNode(pNode) {}

	/// \brief 析构函数
	~MSXML_DOMNode();

	/// \brief 获取第一个子节点
	DOMNodePtr getFirstChildNode();
	/// \brief 获取下一个兄弟节点
	DOMNodePtr getNextSiblingNode();
	/// \brief 获取属性对象指针
	DOMAttributesPtr getAttributes();
	/// \brief 获取节点名
	std::string getName();
	/// \brief 获取节点文本
	std::string getText();

private:
	MSXML2::IXMLDOMNode* m_pNode;
};

class DOMNodeList : public std::vector<DOMNodePtr>
{

};

typedef std::auto_ptr<DOMNodeList> DOMNodeListPtr;


/// \brief XML DOM 解析基类.
class DOMDocument
{
public:
	/// \brief 析构函数
	virtual ~DOMDocument() {}

	/// \brief 加载xml流
	virtual bool load(std::istream&) = 0;
	/// \brief 加载xml文件
	virtual bool load(const std::string&) = 0;
	/// \brief 输出xml流
	virtual bool xml(std::ostream&) = 0;

	/// \brief 获取节点对象指针
	virtual DOMNodePtr getNode(const std::string&) = 0;
};

typedef std::auto_ptr<DOMDocument> DOMDocumentAPtr;


#include <unordered_map>
#include <string>
#include <stdexcept>
using namespace std;

/// \brief DOM异常类
class DOMException : public logic_error
{
public:
	/// \brief 构造函数
	DOMException(const string& strDescription) : logic_error(string("DOM Error: ") + strDescription) {}
	/// \brief 析构函数
	~DOMException() throw() {}
};

/// \brief Windows上XML解析实现类
class MSXML_DOMDocument : public DOMDocument
{
public:
	/// \brief 构造函数
	MSXML_DOMDocument() throw(DOMException);
	/// \brief 析构函数
    ~MSXML_DOMDocument();

	/// \brief 加载xml流
    bool load(istream&);
	/// \brief 加载xml文件
    bool load(const string&);
	/// \brief 输出xml流
    bool xml(ostream&);

	/// \brief 获取节点对象指针
    DOMNodePtr getNode(const string&);

	virtual DOMNodeListPtr getNodes( const std::string& );

  private:
    MSXML2::IXMLDOMDocument2* m_pDoc;
  };

}

#endif//win32
