#ifndef _SCHEMA_RESTRICTION_HPP_
#define _SCHEMA_RESTRICTION_HPP_

#include "SchemaCommon.hpp"

class CEnumerationArray;

class CRestriction : public CXSDNode
{
public:

    virtual ~CRestriction()
    {
    }

    GETTERSETTER(ID)
    GETTERSETTER(Base)

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    virtual void getDocumentation(StringBuffer &strDoc) const;

    virtual void getDojoJS(StringBuffer &strJS) const;

    virtual void traverseAndProcessNodes() const;

    virtual const char* getXML(const char* /*pComponent*/);

    static CRestriction* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath);

protected:

    CRestriction(CXSDNodeBase* pParentNode = NULL, const char* pID = NULL, const char* pBase = NULL) : CXSDNode::CXSDNode(pParentNode, XSD_RESTRICTION), m_strID(pID), m_strBase(pBase), m_pEnumerationArray(NULL)
    {
    }

    void setEnumerationArray(CEnumerationArray *pEnumerationArray)
    {
        assert(pEnumerationArray != NULL);

        if (pEnumerationArray != NULL)
        {
            m_pEnumerationArray = pEnumerationArray;
        }
    }

    CEnumerationArray* getEnumerationArray()
    {
        return m_pEnumerationArray;
    }

    CEnumerationArray *m_pEnumerationArray;

private:

    CRestriction(CXSDNodeBase* pParentNode = NULL) : CXSDNode::CXSDNode(pParentNode, XSD_RESTRICTION), m_pEnumerationArray(NULL)
    {
    }
};


#endif // _SCHEMA_RESTRICTION_HPP_
