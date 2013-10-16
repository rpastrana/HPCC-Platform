#include "jptree.hpp"
#include "XMLTags.h"
#include "SchemaSimpleType.hpp"
#include "SchemaRestriction.hpp"
#include "ConfigSchemaHelper.hpp"


CXSDNodeBase* CSimpleType::getNodeByTypeAndNameAscending(NODE_TYPES eNodeType, const char *pName)
{
    return (this->checkSelf(eNodeType, pName, this->getName()) ? this : NULL);
}

CXSDNodeBase* CSimpleType::getNodeByTypeAndNameDescending(NODE_TYPES eNodeType, const char *pName)
{
    return (this->checkSelf(eNodeType, pName, this->getName()) ? this : NULL);
}

void CSimpleType::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_SIMPLE_TYPE_STR, offset);

    QUICK_OUT_2(Name);
    QUICK_OUT_2(ID);

    if (m_pRestriction != NULL)
    {
        m_pRestriction->dump(cout, offset);
    }

    QuickOutFooter(cout, XSD_SIMPLE_TYPE_STR, offset);
}

void CSimpleType::getDocumentation(StringBuffer &strDoc) const
{
    if (m_pRestriction != NULL)
    {
        m_pRestriction->getDocumentation(strDoc);
    }
}

void CSimpleType::getDojoJS(StringBuffer &strJS) const
{
    if (m_pRestriction != NULL)
    {
        m_pRestriction->getDojoJS(strJS);
    }
}

void CSimpleType::getQML(StringBuffer &strQML) const
{
    if (m_pRestriction != NULL)
    {
        m_pRestriction->getQML(strQML);
    }
}

void CSimpleType::traverseAndProcessNodes() const
{
    CSimpleType::processEntryHandlers(this);

    if (m_pRestriction != NULL)
    {
        m_pRestriction->traverseAndProcessNodes();
    }

    CSimpleType::processExitHandlers(this);
}

CSimpleType* CSimpleType::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pSchemaRoot != NULL);
    assert(pParentNode != NULL);

    if (pSchemaRoot == NULL || pParentNode == NULL)
    {
        return NULL;
    }

    IPropertyTree *pTree = pSchemaRoot->queryPropTree(xpath);

    if (pTree == NULL)
    {
        return NULL;
    }

    const char* pName =  NULL;
    const char* pID =  NULL;

    pName = pTree->queryProp(XML_ATTR_NAME);

/*    assert (pName != NULL);

    if (pName == NULL)
    {
        return NULL;
    }

*/

    pID = pTree->queryProp(XML_ATTR_ID);


    CSimpleType* pSimpleType = new CSimpleType(pParentNode, pName,pID);

    pSimpleType->setXSDXPath(xpath);

    assert(pSimpleType != NULL);

    if (pSimpleType == NULL)
    {
        return NULL;
    }


    StringBuffer strXPathExt(xpath);
    strXPathExt.append("/").append(XSD_TAG_RESTRICTION);

    CRestriction *pRestriction = CRestriction::load(pSimpleType, pSchemaRoot, strXPathExt.str());

    if (pRestriction != NULL)
    {
        pSimpleType->setRestriciton(pRestriction);
    }

    if (pName != NULL)
    {
        CConfigSchemaHelper::getInstance()->setSimpleTypeWithName(pName, pSimpleType);
    }

    return pSimpleType;
}

const char* CSimpleType::getXML(const char* /*pComponent*/)
{
    if (m_strXML.length () == 0)
    {
        m_strXML.append("<").append(getName()).append("\n");
        m_strXML.append("<").append(getID()).append("\n");

        if (m_pRestriction != NULL)
        {
            m_strXML.append(m_pRestriction->getXML(NULL));
        }

        m_strXML.append("/>\n");
    }

    return m_strXML.str();
}


void CSimpleTypeArray::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_SIMPLE_TYPE_ARRAY_STR, offset);

    QUICK_OUT_ARRAY(cout, offset);

    QuickOutFooter(cout, XSD_SIMPLE_TYPE_ARRAY_STR, offset);
}

void CSimpleTypeArray::traverseAndProcessNodes() const
{
    QUICK_TRAVERSE_AND_PROCESS;
}

CSimpleTypeArray* CSimpleTypeArray::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pParentNode != NULL);
    assert(pSchemaRoot != NULL);

    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    CSimpleTypeArray *pSimpleTypeArray = new CSimpleTypeArray(pParentNode);

    pSimpleTypeArray->setXSDXPath(xpath);

    Owned<IPropertyTreeIterator> elemIter = pSchemaRoot->getElements(xpath);

    int count = 1;

    ForEach(*elemIter)
    {
        StringBuffer strXPathExt(xpath);
        strXPathExt.appendf("[%d]", count);

        CSimpleType *pSimpleType = CSimpleType::load(pSimpleTypeArray, pSchemaRoot, strXPathExt.str());

        assert(pSimpleType != NULL);

        pSimpleTypeArray->append(*pSimpleType);

        count++;
    }

    return pSimpleTypeArray;
}

void CSimpleTypeArray::getDocumentation(StringBuffer &strDoc) const
{
    QUICK_DOC_ARRAY(strDoc);
}

void CSimpleTypeArray::getDojoJS(StringBuffer &strJS) const
{
    QUICK_DOJO_JS_ARRAY(strJS);
}

void CSimpleTypeArray::getQML(StringBuffer &strQML) const
{
    QUICK_QML_ARRAY(strQML);
}
