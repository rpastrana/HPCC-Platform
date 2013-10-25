#include "jptree.hpp"
#include "ConfigSchemaHelper.hpp"
#include "SchemaAttributeGroup.hpp"
#include "DocumentationMarkup.hpp"

const CXSDNodeBase* CAttributeGroup::getNodeByTypeAndNameAscending(NODE_TYPES eNodeType, const char *pName) const
{
    const CXSDNodeBase* pMatchingNode = NULL;

    if (eNodeType == this->getNodeType() && (pName != NULL ? !strcmp(pName, this->getNodeTypeStr()) : true))
    {
        assert(pName != NULL); // for now pName should always be populated

        return this;
    }

    if (m_pAttributeArray != NULL)
    {
        pMatchingNode =  m_pAttributeArray->getNodeByTypeAndNameAscending(eNodeType, pName);
    }

    if (pMatchingNode == NULL && m_pAttributeArray != NULL)
    {
        pMatchingNode =  m_pAttributeArray->getNodeByTypeAndNameDescending(eNodeType, pName);
    }


    return pMatchingNode;
}

CAttributeGroup::~CAttributeGroup()
{

}

const CXSDNodeBase* CAttributeGroup::getNodeByTypeAndNameDescending(NODE_TYPES eNodeType, const char *pName) const
{
    const CXSDNodeBase* pMatchingNode = NULL;

    if (eNodeType == this->getNodeType() && (pName != NULL ? !strcmp(pName, this->getNodeTypeStr()) : true))
    {
        assert(pName != NULL); // for now pName should always be populated

        return this;
    }

    if (m_pAttributeArray != NULL)
    {
        pMatchingNode =  m_pAttributeArray->getNodeByTypeAndNameDescending(eNodeType, pName);
    }

    return pMatchingNode;
}

void CAttributeGroup::dump(std::ostream &cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_ATTRIBUTE_GROUP_STR, offset);

    QUICK_OUT(cout,Name, offset);
    QUICK_OUT(cout,Ref, offset);
    QUICK_OUT(cout,ID, offset);

    if (m_pRefAttributeGroup != NULL)
    {
        m_pRefAttributeGroup->dump(cout, offset);
    }

    if (m_pAttributeArray != NULL)
    {
        m_pAttributeArray->dump(cout, offset);
    }

    QuickOutFooter(cout, XSD_ATTRIBUTE_GROUP_STR, offset);
}

void CAttributeGroup::getDocumentation(StringBuffer &strDoc) const
{
    if (this->getRef() != NULL && this->getRef()[0] != 0 && m_pRefAttributeGroup != NULL)
    {
       // m_pRefAttributeGroup->getDocumentation(strDoc);
        strDoc.appendf("%s%s%s", DM_TITLE_BEGIN, m_pRefAttributeGroup->getName(), DM_TITLE_END);
        DEBUG_MARK_STRDOC;

        if (m_pRefAttributeGroup->getAttributeArray() != NULL)
        {
            m_pRefAttributeGroup->getAttributeArray()->getDocumentation(strDoc);
        }
    }
    /*else if (m_pAttributeArray != NULL)
    {
        strDoc.appendf("%s%s%s", DM_TITLE_BEGIN, this->getName(), DM_TITLE_END);
        m_pAttributeArray->getDocumentation(strDoc);
    }*/
}

void CAttributeGroup::getDojoJS(StringBuffer &strJS) const
{
    if (this->getRef() != NULL && this->getRef()[0] != 0 && m_pRefAttributeGroup != NULL)
    {
        if (m_pRefAttributeGroup->getAttributeArray() != NULL)
        {
            m_pRefAttributeGroup->getAttributeArray()->getDojoJS(strJS);
        }
    }
}

void CAttributeGroup::getQML(StringBuffer &strQML) const
{
    if (this->getRef() != NULL && this->getRef()[0] != 0 && m_pRefAttributeGroup != NULL)
    {
        if (m_pRefAttributeGroup->getAttributeArray() != NULL)
        {
            m_pRefAttributeGroup->getAttributeArray()->getQML(strQML);
        }
    }
}

void CAttributeGroup::traverseAndProcessNodes() const
{
    CXSDNodeBase::processEntryHandlers(this);

    if (m_pAttributeArray != NULL)
    {
        m_pAttributeArray->traverseAndProcessNodes();
    }

    CXSDNodeBase::processExitHandlers(this);
}

const char* CAttributeGroup::getXML(const char* /*pComponent*/)
{
    if (m_strXML.length() == 0 && m_pAttributeArray != NULL)
    {
        for (int idx = 0; idx < m_pAttributeArray->length(); idx++)
        {
            m_strXML.append("\n").append(m_pAttributeArray->item(idx).getXML(NULL));
        }
    }

    return m_strXML.str();
}

CAttributeGroup* CAttributeGroup::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pSchemaRoot != NULL);

    if (pSchemaRoot == NULL || xpath == NULL)
    {
        return NULL;
    }

    assert(pParentNode->getNodeType() != XSD_ATTRIBUTE_GROUP);
    CAttributeGroup *pAttributeGroup = new CAttributeGroup(pParentNode);

    pAttributeGroup->setXSDXPath(xpath);

    assert(pAttributeGroup);

    if (pAttributeGroup == NULL)
    {
        return NULL;
    }

    IPropertyTree *pTree = pSchemaRoot->queryPropTree(xpath);

    if (pTree == NULL)
    {
        return NULL;
    }

    pAttributeGroup->setName(pTree->queryProp(XML_ATTR_NAME));
    pAttributeGroup->setID(pTree->queryProp(XML_ATTR_ID));
    pAttributeGroup->setRef(pTree->queryProp(XML_ATTR_REF));

    if (pAttributeGroup->getRef() != NULL && pAttributeGroup->getRef()[0] != 0)
    {
        if (pAttributeGroup->getName() != NULL && pAttributeGroup->getName()[0] != 0)
        {
            assert(false); //can't have both nameand ref set
            return NULL;
        }
        else
        {
            CConfigSchemaHelper::getInstance()->addAttributeGroupToBeProcessed(pAttributeGroup);
        }
    }
    else if (pAttributeGroup->getName() != NULL && pAttributeGroup->getName()[0] != 0)
    {
        CConfigSchemaHelper::getInstance()->setAttributeGroupTypeWithName(pAttributeGroup->getName(), pAttributeGroup);
    }

    StringBuffer strXPath(xpath);

    strXPath.append("/").append(XSD_TAG_ATTRIBUTE);

    CAttributeArray *pAttribArray = CAttributeArray::load(pAttributeGroup, pSchemaRoot, strXPath.str());

    if (pAttribArray != NULL)
    {
        pAttributeGroup->setAttributeArray(pAttribArray);
    }

    strXPath.setf("%s/%s",xpath, XSD_TAG_ANNOTATION);
    pAttributeGroup->setAnnotation(CAnnotation::load(pAttributeGroup, pSchemaRoot, strXPath.str()));

    return pAttributeGroup;
}

CAttributeGroupArray::~CAttributeGroupArray()
{
}

CAttributeGroupArray* CAttributeGroupArray::load(const char* pSchemaFile)
{
    assert(false);  // Should never call this?
    if (pSchemaFile == NULL)
    {
        return false;
    }

    Linked<IPropertyTree> pSchemaRoot;
    StringBuffer schemaPath;

    schemaPath.appendf("%s%s", DEFAULT_SCHEMA_DIRECTORY, pSchemaFile);
    pSchemaRoot.setown(createPTreeFromXMLFile(schemaPath.str()));

    return CAttributeGroupArray::load(NULL, pSchemaRoot, XSD_TAG_ATTRIBUTE_GROUP);
}

CAttributeGroupArray* CAttributeGroupArray::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pSchemaRoot != NULL);

    if (pSchemaRoot == NULL || xpath == NULL)
    {
        return NULL;
    }

    CAttributeGroupArray *pAttribGroupArray = new CAttributeGroupArray(pParentNode);

    pAttribGroupArray->setXSDXPath(xpath);

    StringBuffer strXPathExt(xpath);

    // to iterate over xs:attributeGroup nodes at same level in tree
    Owned<IPropertyTreeIterator> attribGroupsIter = pSchemaRoot->getElements(strXPathExt.str());

    int count = 1;
    ForEach(*attribGroupsIter)
    {
        strXPathExt.clear().appendf("%s[%d]", xpath, count);

        CAttributeGroup *pAttribGroup = CAttributeGroup::load(pAttribGroupArray, pSchemaRoot, strXPathExt.str());

        if (pAttribGroup != NULL)
        {
            pAttribGroupArray->append(*pAttribGroup);
        }

        count++;
    }

    if (pAttribGroupArray->length() == 0)
    {
        return NULL;
    }

    return pAttribGroupArray;
}

void CAttributeGroupArray::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout,XSD_ATTRIBUTE_GROUP_ARRAY_STR, offset);

    QUICK_OUT_ARRAY(cout, offset);

    QuickOutFooter(cout,XSD_ATTRIBUTE_GROUP_ARRAY_STR, offset);
}

void CAttributeGroupArray::getDocumentation(StringBuffer &strDoc) const
{
    StringBuffer strDocDupe1(strDoc);

    QUICK_DOC_ARRAY(strDocDupe1);

    if (strDocDupe1.length() == strDoc.length())
    {
       return;
    }

    for (int idx=0; idx < this->length(); idx++)
    {
        strDoc.append(DM_SECT3_BEGIN);
        (this->item(idx)).getDocumentation(strDoc);
        strDoc.append(DM_SECT3_END);
    }
}

void CAttributeGroupArray::getDojoJS(StringBuffer &strJS) const
{
    StringBuffer strJSDupe1(strJS);

    QUICK_DOC_ARRAY(strJSDupe1);

    if (strJSDupe1.length() == strJS.length())
    {
       return;
    }

    QUICK_DOJO_JS_ARRAY(strJS);
}

void CAttributeGroupArray::getQML(StringBuffer &strQML) const
{
    StringBuffer strQMLDupe1(strQML);

    QUICK_QML_ARRAY(strQMLDupe1);

    if (strQMLDupe1.length() == strQML.length())
    {
       return;
    }

    QUICK_QML_ARRAY(strQML);
}

void CAttributeGroupArray::traverseAndProcessNodes() const
{
    QUICK_TRAVERSE_AND_PROCESS;
}
