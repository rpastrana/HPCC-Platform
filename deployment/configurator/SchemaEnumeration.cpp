#include "SchemaCommon.hpp"
#include "SchemaEnumeration.hpp"
#include "XMLTags.h"
#include "jptree.hpp"
#include "DocumentationMarkup.hpp"

CEnumeration* CEnumeration::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pSchemaRoot != NULL);

    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    CEnumeration *pEnumeration = new CEnumeration(pParentNode);

    if (xpath && *xpath)
    {
        IPropertyTree* pTree = pSchemaRoot->queryPropTree(xpath);

        if (pTree == NULL)
        {
            return NULL;
        }

        const char* pValue = pTree->queryProp(XML_ATTR_VALUE);

        if (pValue != NULL)
        {
            pEnumeration->setValue(pValue);
        }
    }

    return pEnumeration;
}

void CEnumeration::dump(std::ostream &cout, unsigned int offset) const
{
    offset += STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_ENUMERATION_STR, offset);

    QUICK_OUT(cout, Value, offset);

    QuickOutFooter(cout, XSD_ENUMERATION_STR, offset);
}

void CEnumeration::getDocumentation(StringBuffer &strDoc) const
{
    strDoc.appendf("<%s>* %s </%s>\n", DM_PARA, this->getValue(), DM_PARA);
}

void CEnumeration::getDojoJS(StringBuffer &strJS) const
{

}

void CEnumeration::traverseAndProcessNodes() const
{
    CXSDNodeBase::processEntryHandlers(this);
    CXSDNodeBase::processExitHandlers(this);
}

const char* CEnumeration::getXML(const char* /*pComponent*/)
{
    assert(false); // NOT IMPLEMENTED
    return NULL;
}

void CEnumerationArray::dump(std::ostream &cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_ENUMERATION_ARRAY_STR, offset);

    QUICK_OUT_ARRAY(cout, offset);

    QuickOutFooter(cout, XSD_ENUMERATION_ARRAY_STR, offset);
}

void CEnumerationArray::getDocumentation(StringBuffer &strDoc) const
{
    strDoc.append("\nChoices are: \n");
    QUICK_DOC_ARRAY(strDoc);
}

void CEnumerationArray::getDojoJS(StringBuffer &strJS) const
{
    QUICK_DOJO_JS_ARRAY(strJS);
}

void CEnumerationArray::traverseAndProcessNodes() const
{
    QUICK_TRAVERSE_AND_PROCESS;
}

const char* CEnumerationArray::getXML(const char* /*pComponent*/)
{
    assert(false); // NOT IMPLEMENTED
}

CEnumerationArray* CEnumerationArray::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pSchemaRoot != NULL);

    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    CEnumerationArray *pEnumerationArray = new CEnumerationArray(pParentNode);

    Owned<IPropertyTreeIterator> elemIter = pSchemaRoot->getElements(xpath);

    int count = 1;

    ForEach(*elemIter)
    {
        StringBuffer strXPathExt(xpath);
        strXPathExt.appendf("[%d]", count);

        CEnumeration *pEnumeration = CEnumeration::load(pEnumerationArray, pSchemaRoot, strXPathExt.str());

        pEnumerationArray->append(*pEnumeration);

        count++;
    }

    if (pEnumerationArray->length() == 0)
    {
        return NULL;
    }

    SETPARENTNODE(pEnumerationArray, pParentNode);

    return pEnumerationArray;
}
