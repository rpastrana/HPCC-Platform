#ifndef _SCHEMA_ELEMENT_HPP_
#define _SCHEMA_ELEMENT_HPP_

#include "jiface.hpp"
#include "jstring.hpp"
#include "jlib.hpp"
#include "jarray.hpp"
#include "SchemaCommon.hpp"

class CAnnotation;
class CComplexTypeArray;
class IPropertyTree;
class CAttributeArray;

static const char* DEFAULT_ELEMENT_ARRAY_XPATH(".");

class CElement : public CXSDNode
{
public:

    virtual ~CElement()
    {
    }

    GETTERSETTER(Name)
    GETTERSETTER(MaxOccurs)
    GETTERSETTER(MinOccurs)
    GETTERSETTER(Type)

    virtual const char* getXML(const char* /*pComponent*/);

    virtual void dump(std::ostream &cout, unsigned int offset = 0) const;

    virtual void getDocumentation(StringBuffer &strJS) const;

    virtual void getDojoJS(StringBuffer &strDoc) const;

    virtual void traverseAndProcessNodes() const;

    const CAnnotation* getAnnotation() const
    {
        return m_pAnnotation;
    }

    static CElement* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath);

protected:

    CElement(CXSDNodeBase* pParentNode, const char* pName = "") : CXSDNode::CXSDNode(pParentNode, XSD_ELEMENT), m_strMinOccurs(""), m_strMaxOccurs(""), m_strName(pName), m_pAnnotation(NULL), m_pComplexTypeArray(NULL), m_pAttributeArray(NULL)
    {
    }

    CAnnotation* m_pAnnotation;
    CComplexTypeArray* m_pComplexTypeArray;
    CAttributeArray* m_pAttributeArray;

private:

    CElement() : CXSDNode::CXSDNode(NULL, XSD_ELEMENT)
    {
    }
};

class CElementArray : public CIArrayOf<CElement>, public InterfaceImpl, public CXSDNodeBase
{
public:

    CElementArray(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot = NULL) : CXSDNodeBase::CXSDNodeBase(pParentNode, XSD_ELEMENT_ARRAY), m_pSchemaRoot(pSchemaRoot)
    {
    }

    virtual ~CElementArray()
    {
    }

    virtual void dump(std::ostream &cout, unsigned int offset = 0) const;

    virtual void getDocumentation(StringBuffer &strDoc) const;

    virtual void getDojoJS(StringBuffer &strJS) const;

    virtual void traverseAndProcessNodes() const;

    virtual const char* getXML(const char* /*pComponent*/);

    static CElementArray* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath = DEFAULT_ELEMENT_ARRAY_XPATH);

    static CElementArray* load(const char *pSchemaFile);

protected:

    IPropertyTree *m_pSchemaRoot;

private:

    CElementArray() : CXSDNodeBase::CXSDNodeBase(NULL, XSD_ELEMENT_ARRAY)
    {
    }
};

#endif // _SCHEMA_ELEMENT_HPP_
