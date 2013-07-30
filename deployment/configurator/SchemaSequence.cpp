#include <cassert>
#include "jptree.hpp"
#include "XMLTags.h"
#include "SchemaSequence.hpp"
#include "SchemaElement.hpp"

CSequence* CSequence::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pSchemaRoot != NULL);

    CSequence *pSequence = NULL;

    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    if (pSchemaRoot->queryPropTree(xpath) == NULL)
    {
        return NULL;  // no sequence node
    }

    StringBuffer strXPath(xpath);
    strXPath.append("/").append(XSD_TAG_ELEMENT);

    CElementArray *pElemArray = CElementArray::load(NULL, pSchemaRoot, strXPath.str());

    if (pElemArray != NULL)
    {
        pSequence = new CSequence(pParentNode, pElemArray);
    }

    SETPARENTNODE(pElemArray, pSequence)

    return pSequence;
}

void CSequence::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_SEQUENCE_STR, offset);

    if (p_mElementArray != NULL)
    {
        p_mElementArray->dump(cout, offset);
    }

    QuickOutFooter(cout, XSD_SEQUENCE_STR, offset);
}

void CSequence::getDocumentation(StringBuffer &strDoc) const
{
    if (p_mElementArray != NULL)
    {
        p_mElementArray->getDocumentation(strDoc);
    }
}

void CSequence::getDojoJS(StringBuffer &strJS) const
{
    if (p_mElementArray != NULL)
    {
        p_mElementArray->getDojoJS(strJS);
    }
}

void CSequence::getQML(StringBuffer &strQML) const
{
    if (p_mElementArray != NULL)
    {
        p_mElementArray->getQML(strQML);
    }
}

void CSequence::traverseAndProcessNodes() const
{
    CXSDNodeBase::processEntryHandlers(this);

    if (p_mElementArray != NULL)
    {
        p_mElementArray->traverseAndProcessNodes();
    }

    CXSDNodeBase::processExitHandlers(this);
}
