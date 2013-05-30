#ifndef _SCHEMA_SIMPLE_TYPE_HPP_
#define _SCHEMA_SIMPLE_TYPE_HPP_

#include "jstring.hpp"
#include "jarray.hpp"
#include "SchemaCommon.hpp"
#include "SchemaRestriction.hpp"

class CRestriction;

class CSimpleType : public CXSDNode
{
public:

    virtual ~CSimpleType()
    {
    }

    virtual CXSDNodeBase* getNodeByTypeAndNameAscending(NODE_TYPES eNodeType, const char *pName);
    virtual CXSDNodeBase* getNodeByTypeAndNameDescending(NODE_TYPES eNodeType, const char *pName);

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    virtual void getDocumentation(StringBuffer &strDoc) const;

    virtual void getDojoJS(StringBuffer &strJS) const;

    virtual void traverseAndProcessNodes() const;

    virtual const char* getXML(const char* /*pComponent*/);

    static CSimpleType* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath);

    GETTERSETTER(Name)
    GETTERSETTER(ID)

    CRestriction* getRestriction()
    {
        return m_pRestriction;
    }

    void setRestriciton(CRestriction *pRestriction)
    {
        if (m_pRestriction != NULL)
        {
            m_pRestriction->Release();
            m_pRestriction = NULL;
        }

        m_pRestriction = pRestriction;
    }

protected:

    CSimpleType(CXSDNodeBase* pRootNode, const char* pName = NULL, const char* pID = NULL, CRestriction* pRestriction = NULL) : CXSDNode::CXSDNode(pRootNode, XSD_SIMPLE_TYPE),m_strName(pName), m_strID(pID), m_pRestriction(pRestriction)
    {
    }

    CRestriction* m_pRestriction;

private:

    CSimpleType() : CXSDNode::CXSDNode(NULL)
    {
    }
};

class CSimpleTypeArray : public CIArrayOf<CSimpleType>, public InterfaceImpl, public CXSDNodeBase
{
public:

    CSimpleTypeArray(CXSDNodeBase* pParentNode) : CXSDNodeBase::CXSDNodeBase(pParentNode, XSD_SIMPLE_TYPE_ARRAY)
    {
    }

    virtual ~CSimpleTypeArray()
    {
    }

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    virtual void getDocumentation(StringBuffer &strDoc) const;

    virtual void getDojoJS(StringBuffer &strJS) const;

    virtual void traverseAndProcessNodes() const;

    static CSimpleTypeArray* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath);

protected:

private:

    CSimpleTypeArray() : CXSDNodeBase::CXSDNodeBase(NULL)
    {
    }
};

#endif // _SCHEMA_SIMPLE_TYPE_HPP_
