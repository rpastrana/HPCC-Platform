#ifndef _SCHEMA_ATTRIBUTES_HPP_
#define _SCHEMA_ATTRIBUTES_HPP_

#include "jhash.hpp"
#include "jatomic.hpp"
#include "XMLTags.h"
#include "SchemaCommon.hpp"
#include "SchemaAnnotation.hpp"

static const char* DEFAULT_COMPONENT_ATTRIBUTE_XPATH("./xs:element/xs:complexType/xs:attribute");
static const char* DEFAULT_COMPONENT_ATTRIBUTE_GROUP_XPATH("./xs:attributeGroup");

class CAttribute : public CXSDNode
{
public:

    CAttribute(CXSDNodeBase* pParentNode, const char* pName = NULL) : CXSDNode::CXSDNode(pParentNode, XSD_ATTRIBUTE), m_strName(pName), m_pAnnotation(NULL)
    {
    }

    CAttribute(CXSDNodeBase* pParentNode, const char* pName, const char* pType, const char* pDefault, const char* pUse) : CXSDNode::CXSDNode(pParentNode, XSD_ATTRIBUTE), m_strName(pName), m_strType(pType), m_strDefault(pDefault), m_strUse(pUse), m_pAnnotation(NULL)
    {
    }

    virtual ~CAttribute()
    {
    }

    GETTERSETTER(Name)
    GETTERSETTER(Type)
    GETTERSETTER(Default)
    GETTERSETTER(Use)

    virtual const char* getXML(const char* pComponent);

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    static CAttribute* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath = NULL);

protected:

    void setAnnotation(CAnnotation *pAnnotation)
    {
        if (m_pAnnotation != NULL)
        {
            //m_pAnnotation->Release();
        }

        m_pAnnotation = pAnnotation;
    }

    CAnnotation *m_pAnnotation;

private:

    /*CAttribute(CXSDNodeBase* pParentNode = NULL) : CXSDNode::CXSDNode(pParentNode)
    {
    }*/

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


    /*virtual CXSDNodeBase* getNodeByTypeAndNameAscending(NODE_TYPES eNodeType, const char *pName);
    virtual CXSDNodeBase* getNodeByTypeAndNameDescending(NODE_TYPES eNodeType, const char *pName);*/

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

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

    virtual CXSDNodeBase* getNodeByTypeAndNameAscending(NODE_TYPES eNodeType, const char *pName);
    virtual CXSDNodeBase* getNodeByTypeAndNameDescending(NODE_TYPES eNodeType, const char *pName);

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

    virtual const char* getXML(const char* /*pComponent*/);

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    static CAttributeGroup* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath);

protected:

    CAttributeGroup(CXSDNodeBase* pParentNode = NULL, CAttributeArray *pAttribArray = NULL) : CXSDNode::CXSDNode(pParentNode, XSD_ATTRIBUTE_GROUP), m_pAttributeArray(pAttribArray)
    {
    }

    CAttributeArray *m_pAttributeArray;

private:

};

class CAttributeGroupArray : public CIArrayOf<CAttributeGroup>, public InterfaceImpl, public CXSDNodeBase
{
public:

    CAttributeGroupArray(CXSDNodeBase* pParentNode = NULL) : CXSDNodeBase::CXSDNodeBase(pParentNode, XSD_ATTRIBUTE_GROUP)
    {
    }

    virtual ~CAttributeGroupArray();

    //virtual CXSDNodeBase* getNodeByTypeAndNameAscending(NODE_TYPES eNodeType, const char *pName);
    //virtual CXSDNodeBase* getNodeByTypeAndNameDescending(NODE_TYPES eNodeType, const char *pName);

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    static CAttributeGroupArray* load(const char* pSchemaFile);
    static CAttributeGroupArray* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath);

protected:

private:

};

#endif // _SCHEMA_ATTRIBUTES_HPP_
