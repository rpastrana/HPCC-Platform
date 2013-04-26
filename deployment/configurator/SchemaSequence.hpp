#ifndef _SCHEMA_SEQUENCE_HPP_
#define _SCHEMA_SEQUENCE_HPP_

#include "SchemaCommon.hpp"

class CElementArray;
class IPropertyTree;

class CSequence : public CXSDNode
{
public:

    CSequence(CXSDNodeBase* pParentNode = NULL, CElementArray* pElemArray = NULL) : CXSDNode::CXSDNode(pParentNode, XSD_SEQUENCE), p_mElementArray(pElemArray)
    {
    }

    virtual ~CSequence()
    {
    }

    virtual void dump(std::ostream& cout, unsigned int offset  = 0) const;

    static CSequence* load(CXSDNodeBase* pRootNode, IPropertyTree *pSchemaRoot, const char* xpath = NULL);

protected:

    CElementArray *p_mElementArray;

private:

/*    CSequence(CXSDNodeBase* pParentNode = NULL) : CXSDNode::CXSDNode(pParentNode, XSD_SEQUENCE)
    {
    }*/
};

#endif // _SCHEMA_SEQUENCE_HPP_
