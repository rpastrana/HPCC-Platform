#include <iostream>
#include "NodeHandlerDocumentation.hpp"
#include "SchemaElement.hpp"
#include "SchemaAnnotation.hpp"
#include "SchemaSchema.hpp"
#include "SchemaComplexType.hpp"
#include "SchemaAttributes.hpp"

NodeHandlerDocumentation::NodeHandlerDocumentation()
{

}

NodeHandlerDocumentation::~NodeHandlerDocumentation()
{

}

void NodeHandlerDocumentation::onEventEntry(const CXSDNodeBase *pNode) const
{
    assert(pNode != NULL);

    if (pNode == NULL)
    {
        return;
    }


    // ** CElement ** //
    const CElement *pElement = NULL;
    pElement = dynamic_cast<const CElement*>(pNode);

    if (pElement != NULL)
    {
        const char *pName = pElement->getName();

        if (isComponentNameElement(pElement) == true)
        {
            std::cout << "<sect2 id=\"" << pName << "_mod\">" << std::endl;
            std::cout << "<title>" << pName << "</title>" << std::endl;
        }
        else
        {
            std::cout << "<sect3>" << std::endl;
            std::cout << "<title>" << pName << "</title>" << std::endl;
            std::cout << "<informaltable colsep=\"1\" rowsep=\"1\">" << std::endl;
        }
        return;
    }


    // ** CComplexType ** //
    const CComplexType *pComplexType = NULL;
    pComplexType = dynamic_cast<const CComplexType*>(pNode);

    if (pComplexType != NULL)
    {
        std::cout << "<row>" << std::endl;
    }

    // ** CAttribute ** //
    const CAttribute *pAttribute = NULL;
    pAttribute = dynamic_cast<const CAttribute*>(pNode);

    if (pAttribute != NULL && isHiddenAttribute(pAttribute) == false)
    {
        std::cout << "\t<entry>" << pAttribute->getName() << "</entry>" << std::endl;
    }
    else if (pAttribute != NULL && isDefaultAttributeTab(pAttribute) == true)
    {
//        std::cout << "<sect3>" << std::endl;
//        std::cout << "<title>" << pName << "</title>" << std::endl;
//        std::cout << "<informaltable colsep=\"1\" rowsep=\"1\">" << std::endl;
    }

    // ** CAttributeArray ** //
    const CAttributeArray *pAttributeArray = NULL;
    pAttributeArray = dynamic_cast<const CAttributeArray*>(pNode);

    if (pAttributeArray != NULL && isDefaultAttributeArray(pAttributeArray) == true)
    {
        std::cout << "<sect3>" << std::endl;
        std::cout << "<title>" << "Attributes" << "</title>" << std::endl;
        std::cout << "<informaltable colsep=\"1\" rowsep=\"1\">" << std::endl;
    }

}

void NodeHandlerDocumentation::onEventExit(const CXSDNodeBase *pNode) const
{
    assert(pNode != NULL);

    if (pNode == NULL)
    {
        return;
    }

    // ** CElement ** //
    const CElement *pElement = NULL;
    pElement = dynamic_cast<const CElement*>(pNode);

    if (pElement != NULL)
    {
        if (isComponentNameElement(pElement) == true)
        {
            const char *pName = pElement->getName();
            std::cout << "</sect2>" << std::endl;
        }
        else
        {
            std::cout << "</informaltable>" << std::endl;
            std::cout << "</sect3>" << std::endl;
        }
        return;
    }

    // ** CComplexType ** //
    const CComplexType *pComplexType = NULL;
    pComplexType = dynamic_cast<const CComplexType*>(pNode);

    if (pComplexType != NULL)
    {
        std::cout << "</row>" << std::endl;
    }

    // ** CAttributeArray ** //
    const CAttributeArray *pAttributeArray = NULL;
    pAttributeArray = dynamic_cast<const CAttributeArray*>(pNode);

    if (pAttributeArray != NULL  && isDefaultAttributeArray(pAttributeArray) == true)
    {
        std::cout << "</informaltable>" << std::endl;
        std::cout << "</sect3>" << std::endl;
        std::cout << "OUT ATTRIBUTE ARRAY" << std::endl;
    }
}


bool NodeHandlerDocumentation::isComponentNameElement(const CElement *pElement) const
{
    const CSchema *pSchema = NULL;
    const CXSDNodeBase *pBase = const_cast<CXSDNodeBase*>(pElement->getConstParentNode());

    pSchema = dynamic_cast<const CSchema*>(pBase);

    return (pSchema != NULL);
}


bool NodeHandlerDocumentation::isHiddenAttribute(const CAttribute *pAttribute) const
{
    assert(pAttribute != NULL);

    if (pAttribute == NULL)
    {
        return false;
    }

    const CAnnotation *pAnnotation = NULL;
    pAnnotation = pAttribute->getAnnotation();

    if (pAnnotation != NULL)
    {
        const CAppInfo *pAppInfo = NULL;

        pAppInfo = pAnnotation->getAppInfo();

        if (pAppInfo != NULL)
        {
            const char *pViewType = NULL;

            pViewType = pAppInfo->getViewType();

            if (pViewType != NULL)
            {
                if (stricmp("hidden", pViewType) == 0)
                {
                    return true;
                }
            }
        }
    }

    return false;
}


bool NodeHandlerDocumentation::isDefaultAttributeTab(const CAttribute *pAttribute) const
{
    assert(pAttribute != NULL);

    if (pAttribute == NULL)
    {
        return false;
    }

    const CXSDNodeBase *pParentNode = (const_cast<CAttribute*>(pAttribute))->getParentNode();

    if (pParentNode != NULL)
    {
        const CElement *pElement = NULL;

        pElement = dynamic_cast<const CElement*>(pParentNode);

        if (pElement != NULL)
        {
            return isComponentNameElement(pElement);
        }
        /*const CComplexType *pComplexType = NULL;

        pComplexType = dynamic_cast<const CComplexType*>(pParentNode);

        if (pComplexType != NULL)
        {
            return true;
        }*/
    }

    return false;
}


bool NodeHandlerDocumentation::hasAttributeChildren(const CComplexType *pComplexType) const
{
    assert(pComplexType != NULL);

    if (pComplexType == NULL)
    {
        return false;
    }

    if (pComplexType->getAttributeArray() != NULL)
    {
        return true;
    }

    return false;
}

bool NodeHandlerDocumentation::isDefaultAttributeArray(const CAttributeArray *pAttributeArray) const
{
    // CAttributeArray->CComplexTypeArray->CElement
    assert(pAttributeArray != NULL);

    if (pAttributeArray == NULL)
    {
        return false;
    }

    const CComplexTypeArray *pComplexTypeArray = NULL;
    const CXSDNodeBase *pNode = NULL;

    pNode = pAttributeArray->getConstParentNode();

    if (pNode != NULL)  // expecting XSD_COMPLEX_TYPE
    {
        const CXSDNodeBase *pNode2 = pNode->getConstParentNode();

        if (pNode2 != NULL)  // expecting XSD_COMPLEX_TYPE_ARRAY
        {
            const CXSDNodeBase *pNode3 = pNode2->getConstParentNode();

            if (pNode3 != NULL) // expecting XSD_ELEMENT
            {
                const CXSDNodeBase *pNode4 = pNode3->getConstParentNode();

                if (pNode4 != NULL) // expecting XSD_ELEMENT_ARRAY
                {
                    const CXSDNodeBase *pNode5 = pNode4->getConstParentNode();

                    if (pNode5 != NULL)
                    {
                        return isComponentNameElement(dynamic_cast<const CElement*>(pNode5));
                    }
                }
            }
        }
    }

    return false;
}

//    pComplexTypeArray = dynamic_cast<const CComplexTypeArray*>(((const_cast<CAttributeArray*>(pAttributeArray))->getConstParentNode()));
/*
    if (pComplexTypeArray == NULL)
        return false;

    const CElement *pElement = NULL;

    pElement = dynamic_cast<const CElement*>(((const_cast<CComplexTypeArray*>(pComplexTypeArray))->getConstParentNode()));

    return (isComponentNameElement(pElement));
}
    //    const CXSDNodeBase *pParentNode = (const_cast<CAttributeArray*>(pAttributeArray))->getParentNode();

    //if (pParentNode != NULL)
    //{
//    const CComplexType *pComplexType = NULL;

  /*  pComplexType = dynamic_cast<const CComplexType*>((((const_cast<pParentNode);

    if (pComplexType != NULL)
    {
        const CComplexTypeArray *pComplexTypeArray = NULL;

        pComplexTypeArray = dynamic_cast<CComplexTypeArray*>(((const_cast<CComplexType*>(pComplexType))->getParentNode()));

        if (pComplexTypeArray != NULL)
        {
            const CElement *pElement = NULL;

            pElement = dynamic_cast<const CElement*>(((const_cast<CComplexTypeArray*>(pComplexTypeArray))->getParentNode()));

            if (pElement != NULL)
            {
                return isComponentNameElement(pElement);
            }
        }
    }

    return false;*/
//}
