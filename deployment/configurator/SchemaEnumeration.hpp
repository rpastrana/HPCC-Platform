#ifndef SCHEMA_ENUMERATION_HPP
#define SCHEMA_ENUMERATION_HPP

#include "SchemaCommon.hpp"

class CEnumeration : public CXSDNode
{
public:

    virtual ~CEnumeration()
    {
    }

    GETTERSETTER(Value)

    virtual void dump(std::ostream &cout, unsigned int offset = 0) const;

    virtual void getDocumentation(StringBuffer &strDoc) const;

    virtual void getDojoJS(StringBuffer &strJS) const;

    virtual void traverseAndProcessNodes() const;

    virtual const char* getXML(const char* /*pComponent*/);

    static CEnumeration* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath);

protected:

    CEnumeration(CXSDNodeBase* pParentNode) : CXSDNode::CXSDNode(pParentNode, XSD_ENUMERATION), m_strValue("")
    {
    }

private:

    CEnumeration() : CXSDNode::CXSDNode(NULL, XSD_ENUMERATION)
    {
    }
};

class CEnumerationArray : public CIArrayOf<CEnumeration>, public InterfaceImpl, public CXSDNodeBase
{
public:

    CEnumerationArray(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot = NULL) : CXSDNodeBase::CXSDNodeBase(pParentNode, XSD_ENUMERATION_ARRAY), m_pSchemaRoot(pSchemaRoot)
    {
    }

    virtual ~CEnumerationArray()
    {
    }

    virtual void dump(std::ostream &cout, unsigned int offset = 0) const;

    virtual void getDocumentation(StringBuffer &strDoc) const;

    virtual void getDojoJS(StringBuffer &strJS) const;

    virtual void traverseAndProcessNodes() const;

    virtual const char* getXML(const char* /*pComponent*/);

    static CEnumerationArray* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath = NULL);

protected:

    IPropertyTree *m_pSchemaRoot;

private:

    CEnumerationArray() : CXSDNodeBase::CXSDNodeBase(NULL, XSD_ENUMERATION_ARRAY)
    {
    }
};

#endif // SCHEMA_ENUMERATION_HPP
