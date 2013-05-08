#ifndef _NODE_HANDLER_DOCUMENTATION_
#define _NODE_HANDLER_DOCUMENTATION_

#include "SchemaCommon.hpp"

class CElement;
class CAttribute;
class CComplexType;
class CAttributeArray;

class NodeHandlerDocumentation: public CXSDNodeHandler
{
public:

    NodeHandlerDocumentation();

    virtual ~NodeHandlerDocumentation();

    virtual void onEventEntry(const CXSDNodeBase *pNode) const;
    virtual void onEventExit(const CXSDNodeBase *pNode) const;

    bool isComponentNameElement(const CElement *pElement) const;
//    bool isAttributeTab(const CElement *pElement) const;
    bool isHiddenAttribute(const CAttribute *pAttribute) const;

    bool isDefaultAttributeTab(const CAttribute *pAttribute) const;
    bool isDefaultAttributeArray(const CAttributeArray *pAttributeArray) const;
    bool hasAttributeChildren(const CComplexType *pComplexType) const;

    //void getAttributeTab(StringBuffer )
};


#endif // _NODE_HANDLER_DOCUMENTATION_
