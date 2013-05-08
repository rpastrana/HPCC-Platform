#include "jptree.hpp"
#include "jstring.hpp"
#include "SchemaDocumentation.hpp"

void CDocumentation::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_DOCUMENTATION_STR, offset);

    QUICK_OUT(cout,Documentation, offset);

    QuickOutFooter(cout, XSD_DOCUMENTATION_STR, offset);
}

void CDocumentation::traverseAndProcessNodes() const
{
    CXSDNodeBase::processEntryHandlers(this);
    CXSDNodeBase::processExitHandlers(this);
}

CDocumentation* CDocumentation::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    if (pSchemaRoot->queryPropTree(xpath) == NULL)
    {
        return NULL; // no documentation node
    }

    StringBuffer strDoc;

    if (xpath && *xpath)
    {
        strDoc.append(pSchemaRoot->queryPropTree(xpath)->queryProp(""));
    }

    return new CDocumentation(pParentNode, strDoc.str());
}
