#ifndef _DOJOHELPER_HPP_
#define _DOJOHELPER_HPP_

class CXSDNodeBase;

class CElement;

class CDojoHelper
{
public:

    static bool IsElementATab(const CElement *pElement);
    static bool isAncestorTopElement(const CXSDNodeBase *pNode);

protected:

    static const CXSDNodeBase* getTopMostElement(const CXSDNodeBase *pNode);
    static const CXSDNodeBase* getAncestorElement(const CXSDNodeBase *pNode);
    static bool isViewType(const CElement *pElement, const char* pValue);
    static bool isViewChildNodes(const CElement *pElement);

};

#endif // _DOJOHELPER_HPP_
