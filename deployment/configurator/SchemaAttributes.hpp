#ifndef _SCHEMA_ATTRIBUTES_HPP_
#define _SCHEMA_ATTRIBUTES_HPP_

#include "jhash.hpp"
#include "jatomic.hpp"
#include "XMLTags.h"
#include "SchemaCommon.hpp"
#include "SchemaAnnotation.hpp"

class CSimpleTypeArray;

static const char* DEFAULT_COMPONENT_ATTRIBUTE_XPATH("./xs:element/xs:complexType/xs:attribute");
static const char* DEFAULT_COMPONENT_ATTRIBUTE_GROUP_XPATH("./xs:attributeGroup");

class CAttribute : public CXSDNode
{
public:

    CAttribute(CXSDNodeBase* pParentNode, const char* pName = NULL) : CXSDNode::CXSDNode(pParentNode, XSD_ATTRIBUTE), m_strName(pName), m_pAnnotation(NULL)
    {
    }

    CAttribute(CXSDNodeBase* pParentNode, const char* pName, const char* pType, const char* pDefault, const char* pUse) : CXSDNode::CXSDNode(pParentNode, XSD_ATTRIBUTE), m_strName(pName), m_strType(pType), m_strDefault(pDefault), m_strUse(pUse), m_pAnnotation(NULL), m_pSimpleTypeArray(NULL)
    {
    }

    virtual ~CAttribute()
    {
    }

    GETTERSETTER(Name)
    GETTERSETTER(Type)
    GETTERSETTER(Default)
    GETTERSETTER(Use)
    GETTERSETTER(InstanceValue)

    const CAnnotation* getAnnotation() const
    {
        return m_pAnnotation;
    }

    const char* getTitle() const;

    virtual const char* getXML(const char* pComponent);

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    virtual void getDocumentation(StringBuffer &strDoc) const;

    virtual void getDojoJS(StringBuffer &strJS) const;

    virtual void getQML(StringBuffer &strQML) const;

    //virtual void loadXML();

    virtual void traverseAndProcessNodes() const;

    static CAttribute* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath = NULL);

protected:

    void setAnnotation(CAnnotation *pAnnotation)
    {
        assert(pAnnotation != NULL);  // why would this ever be NULL?

        if (m_pAnnotation != NULL)
        {
            //m_pAnnotation->Release();
        }

        m_pAnnotation = pAnnotation;
    }


    void setSimpleTypeArray(CSimpleTypeArray *pSimpleTypeArray)
    {
        assert(pSimpleTypeArray != NULL);  // why would this ever be NULL?

        if (pSimpleTypeArray != NULL)
        {
            //pSimpleType->Release();
        }

        m_pSimpleTypeArray = pSimpleTypeArray;
    }

    CAnnotation *m_pAnnotation;
    CSimpleTypeArray *m_pSimpleTypeArray;

private:

};


class CAttributeArray : public CIArrayOf<CAttribute>, public InterfaceImpl, public CXSDNodeBase
{
public:

    CAttributeArray(CXSDNodeBase* pParentNode = NULL) : CXSDNodeBase::CXSDNodeBase(pParentNode, XSD_ATTRIBUTE_ARRAY)
    {
    }

    virtual ~CAttributeArray()
    {
    }

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    virtual void getDocumentation(StringBuffer &strDoc) const;

    virtual void getDojoJS(StringBuffer &strJS) const;

    virtual void getQML(StringBuffer &strQML) const;

    //virtual void loadXML();

    virtual void traverseAndProcessNodes() const;

    virtual const char* getXML(const char* /*pComponent*/);

    static CAttributeArray* load(const char* pSchemaFile);
    static CAttributeArray* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath);

protected:

private:

};

class CAttributeGroup : public CXSDNode
{
public:

    virtual ~CAttributeGroup();

    GETTERSETTER(Name)
    GETTERSETTER(Ref)
    GETTERSETTER(ID)

    virtual const CXSDNodeBase* getNodeByTypeAndNameAscending(NODE_TYPES eNodeType, const char *pName) const;
    virtual const CXSDNodeBase* getNodeByTypeAndNameDescending(NODE_TYPES eNodeType, const char *pName) const;

    const CAttributeArray* getAttributeArray() const
    {
        return m_pAttributeArray;
    }

    void setAttributeArray(CAttributeArray *pAttribArray)
    {
        if (m_pAttributeArray != NULL)
        {
            m_pAttributeArray->Release();
        }

        m_pAttributeArray = pAttribArray;
    }

    void setRefNode(CAttributeGroup* pAttributeGroup)
    {
        assert(pAttributeGroup != NULL);

        if (pAttributeGroup != NULL)
        {
            //m_pXSDNode->Release();
        }

        m_pRefAttributeGroup = pAttributeGroup;
    }

    virtual const char* getXML(const char* /*pComponent*/);

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    virtual void getDocumentation(StringBuffer &strDoc) const;

    virtual void getDojoJS(StringBuffer &strJS) const;

    virtual void getQML(StringBuffer &strQML) const;

    //virtual void loadXML();

    virtual void traverseAndProcessNodes() const;

    static CAttributeGroup* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath);

protected:

    CAttributeGroup(CXSDNodeBase* pParentNode = NULL, CAttributeArray *pAttribArray = NULL) : CXSDNode::CXSDNode(pParentNode, XSD_ATTRIBUTE_GROUP), m_pAttributeArray(pAttribArray), m_pRefAttributeGroup(NULL)
    {
    }

    CAttributeGroup *m_pRefAttributeGroup;
    CAttributeArray *m_pAttributeArray;

private:

};

class CAttributeGroupArray : public CIArrayOf<CAttributeGroup>, public InterfaceImpl, public CXSDNodeBase
{
public:

    CAttributeGroupArray(CXSDNodeBase* pParentNode = NULL) : CXSDNodeBase::CXSDNodeBase(pParentNode, XSD_ATTRIBUTE_GROUP_ARRAY)
    {
    }

    virtual ~CAttributeGroupArray();


    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    virtual void getDocumentation(StringBuffer &strDoc) const;

    virtual void getDojoJS(StringBuffer &strJS) const;

    virtual void getQML(StringBuffer &strQML) const;

    virtual void traverseAndProcessNodes() const;

    static CAttributeGroupArray* load(const char* pSchemaFile);
    static CAttributeGroupArray* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath);

protected:

private:

};

#endif // _SCHEMA_ATTRIBUTES_HPP_
