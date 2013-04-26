#include "XMLTags.h"
#include "jptree.hpp"
#include "SchemaExtension.hpp"
#include "SchemaComplexType.hpp"
#include "SchemaSchema.hpp"

static const char* DEFAULT_ENVIRONMENT_XSD("Environment.xsd");

void CExtension::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_EXTENSION_STR, offset);

    if (this->getBaseNode() != NULL)
    {
        this->getBaseNode()->dump(cout, offset);
    }
    else
    {
        QUICK_OUT(cout, Base, offset);  // must be a built in type ??
    }

    QuickOutHeader(cout, XSD_EXTENSION_STR, offset);
}

const char* CExtension::getXML(const char* /*pComponent*/)
{
    if (m_strXML.length() == 0 && m_pXSDNode != NULL)
    {
        m_pXSDNode->getXML(NULL);
    }

    return NULL;
}

CExtension* CExtension::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    CExtension *pExtension = NULL;

    if (xpath && *xpath)
    {
        IPropertyTree* pTree = pSchemaRoot->queryPropTree(xpath);

        if (pTree == NULL)
        {
            return NULL; // no xs:extension node
        }

        const char* pBase = pSchemaRoot->getPropTree(xpath)->queryProp(XML_ATTR_BASE);

        if (pBase != NULL)
        {
            pExtension = new CExtension(pParentNode);
            pExtension->setBase(pBase);

            CXSDNodeBase *pBaseNode = pExtension->getNodeByTypeAndNameAscending( XSD_SIMPLE_TYPE | XSD_COMPLEX_TYPE, pBase);

            assert(pBaseNode != NULL);  // temporary to catch built in types or not defined types

            if (pBaseNode != NULL)
            {
                pExtension->setBaseNode(pBaseNode);
            }
        }
    }

    return pExtension;
}
