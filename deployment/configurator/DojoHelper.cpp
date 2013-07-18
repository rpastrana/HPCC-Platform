#include "DojoHelper.hpp"
#include "SchemaElement.hpp"
#include "SchemaAnnotation.hpp"
#include "SchemaAppInfo.hpp"

bool CDojoHelper::IsElementATab(const CElement *pElement)
{
    if (pElement == NULL)
    {
        return false;
    }

    if ( pElement->getMaxOccurs() != NULL && stricmp(pElement->getMaxOccurs(), "unbounded") == 0 && isViewType(pElement, "list") == false && isViewChildNodes(pElement) == true )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CDojoHelper::isViewType(const CElement *pElement, const char* pValue)
{
    if (pElement == NULL || pValue == NULL)
    {
        return false;
    }
    else if (pElement->getAnnotation() != NULL && pElement->getAnnotation()->getAppInfo() != NULL && pElement->getAnnotation()->getAppInfo()->getViewType() != NULL)
    {
        return (stricmp(pElement->getAnnotation()->getAppInfo()->getViewType(), pValue) == 0);
    }
    else
    {
        return false;
    }
}

bool CDojoHelper::isViewChildNodes(const CElement *pElement)
{
    if (pElement == NULL)
    {
      return false;
    }
    else if (pElement->getAnnotation() != NULL && pElement->getAnnotation()->getAppInfo() != NULL && pElement->getAnnotation()->getAppInfo()->getViewChildNodes() != NULL && (stricmp(pElement->getAnnotation()->getAppInfo()->getViewChildNodes(), "false") == 0))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool CDojoHelper::isAncestorTopElement(const CXSDNodeBase *pNode)
{
    if (pNode == NULL)
    {
        return false;
    }

    if (pNode->getParentNodeByType(XSD_ELEMENT) == getTopMostElement(pNode))
    {
        return true;
    }
    else
    {
        return false;
    }
}

const CXSDNodeBase* CDojoHelper::getAncestorElement(const CXSDNodeBase *pNode)
{
    if (pNode == NULL)
    {
        return NULL;
    }
    else
    {
        const CXSDNodeBase *pParentNode = pNode->getConstParentNode();

        if (pParentNode != NULL && pParentNode->getNodeType() == XSD_ELEMENT)
        {
            return pParentNode;
        }
        else
        {
            return getAncestorElement(pParentNode);
        }
    }
    return NULL;
}

const CXSDNodeBase* CDojoHelper::getTopMostElement(const CXSDNodeBase *pNode)
{
    if (pNode == NULL)
    {
        return NULL;
    }

    const CXSDNodeBase *pAncestorElementNode = getAncestorElement(pNode);

    if (pAncestorElementNode == NULL)
    {
        if (pNode->getNodeType() == XSD_ELEMENT)
        {
            return pNode;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return getTopMostElement(pAncestorElementNode);
    }
}
