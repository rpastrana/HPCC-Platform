#ifndef _SCHEMA_RESTRICTION_HPP_
#define _SCHEMA_RESTRICTION_HPP_

#include "SchemaCommon.hpp"

class CRestriction : public CXSDNode
{
public:

    virtual ~CRestriction()
    {
    }

    GETTERSETTER(ID)
    GETTERSETTER(Base)

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    virtual const char* getXML(const char* /*pComponent*/);

    static CRestriction* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath);

protected:

    CRestriction(CXSDNodeBase* pParentNode = NULL, const char* pID = NULL, const char* pBase = NULL) : CXSDNode::CXSDNode(pParentNode), m_strID(pID), m_strBase(pBase)
    {
    }

private:

    CRestriction(CXSDNodeBase* pParentNode = NULL) : CXSDNode::CXSDNode(pParentNode)
    {
    }
};


#endif // _SCHEMA_RESTRICTION_HPP_
