#ifndef _SCHEMA_COMPLEX_TYPE_HPP
#define _SCHEMA_COMPLEX_TYPE_HPP

#include "SchemaCommon.hpp"
#include "jarray.hpp"

class CSequence;
class CComplexContent;
class CAttributeArray;
class IPropertyTree;
class CChoice;
class CComplexType;
class CElementArray;
class CAttributeGroupArray;

class CComplexType : public CXSDNode
{
public:

    virtual ~CComplexType()
    {
    }

    GETTERSETTER(Name)

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    virtual void getDocumentation(StringBuffer &strDoc) const;

    virtual void getDojoJS(StringBuffer &strJS) const;

    virtual void getQML(StringBuffer &strQML) const;

    virtual void traverseAndProcessNodes() const;

    virtual const char* getXML(const char* /*pComponent*/);

    virtual const CAttributeArray* getAttributeArray() const
    {
        return m_pAttributeArray;
    }

    virtual const CSequence* getSequence() const
    {
        return m_pSequence;
    }

    static CComplexType* load(CXSDNodeBase* pRootNode, IPropertyTree *pSchemaRoot, const char* xpath = NULL);

protected:

    CComplexType(CXSDNodeBase* pParentNode, const char* pName = NULL, CSequence *pSequence = NULL, CComplexContent *pComplexContent = NULL, CAttributeArray *pAttributeArray = NULL, CChoice *pChoice = NULL, CElementArray *pElementArray = NULL, CAttributeGroupArray *pAttributeGroupArray = NULL) : CXSDNode::CXSDNode(pParentNode, XSD_COMPLEX_TYPE), m_strName(pName), m_pSequence(pSequence), m_pComplexContent(pComplexContent), m_pAttributeArray(pAttributeArray), m_pChoice(pChoice), m_pElementArray(pElementArray), m_pAttributeGroupArray(pAttributeGroupArray)
    {
    }

    CSequence *m_pSequence;
    CComplexContent *m_pComplexContent;
    CAttributeArray *m_pAttributeArray;
    CElementArray* m_pElementArray;
    CChoice *m_pChoice;
    CAttributeGroupArray *m_pAttributeGroupArray;

private:

    CComplexType() : CXSDNode::CXSDNode(NULL)
    {
    }

};

class CComplexTypeArray : public CIArrayOf<CComplexType>, public InterfaceImpl, public CXSDNodeBase
{
public:

    CComplexTypeArray(CXSDNodeBase* pParentNode = NULL) : CXSDNodeBase::CXSDNodeBase(pParentNode, XSD_COMPLEX_TYPE_ARRAY)
    {
    }

    virtual ~CComplexTypeArray()
    {
    }

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    virtual void getDocumentation(StringBuffer &strDoc) const;

    virtual void getDojoJS(StringBuffer &strJS) const;

    virtual void getQML(StringBuffer &strQML) const;

    virtual void traverseAndProcessNodes() const;

    virtual const char* getXML(const char* /*pComponent*/);

    static CComplexTypeArray* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath = NULL);
    static CComplexTypeArray* load(CXSDNodeBase* pParentNode, const char* pSchemaFile);

protected:

private:

};

#endif // _SCHEMA_COMPLEX_TYPE_HPP
