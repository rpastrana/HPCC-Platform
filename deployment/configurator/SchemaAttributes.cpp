#include "jptree.hpp"
#include "SchemaCommon.hpp"
#include "SchemaAttributes.hpp"
#include "SchemaAppInfo.hpp"
#include "SchemaSimpleType.hpp"
#include "DocumentationMarkup.hpp"
#include "DojoJSMarkup.hpp"
#include "ConfigSchemaHelper.hpp"
#include "DojoHelper.hpp"

const char* CAttribute::getTitle() const
{
    if (this->m_pAnnotation != NULL && this->m_pAnnotation->getAppInfo() != NULL && this->m_pAnnotation->getAppInfo()->getTitle() != NULL && this->m_pAnnotation->getAppInfo()->getTitle()[0] != 0)
    {
        return this->m_pAnnotation->getAppInfo()->getTitle();
    }
    else
    {
        return this->getName();
    }
}

const char* CAttribute::getXML(const char* pComponent)
{
    if (m_strXML.length() == 0)
    {
        m_strXML.append(getName()).append("=\"").append(getDefault()).append("\"");
    }

    return m_strXML.str();
}

void CAttribute::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout,XSD_ATTRIBUTE_STR, offset);

    QUICK_OUT(cout,Name, offset);
    QUICK_OUT(cout,Type, offset);
    QUICK_OUT(cout,Default, offset);
    QUICK_OUT(cout,Use, offset);

    if (m_pAnnotation != NULL)
    {
        m_pAnnotation->dump(cout, offset);
    }

    if (m_pSimpleTypeArray != NULL)
    {
        m_pSimpleTypeArray->dump(cout, offset);
    }

    QuickOutFooter(cout,XSD_ATTRIBUTE_STR, offset);
}

void CAttribute::getDocumentation(StringBuffer &strDoc) const
{
    const char *pName = this->getTitle();
    const char *pToolTip = NULL;
    const char *pDefaultValue = this->getDefault();
    const char *pRequired = this->getUse();

    if (m_pAnnotation != NULL && m_pAnnotation->getAppInfo() != NULL)
    {
        const CAppInfo *pAppInfo = m_pAnnotation->getAppInfo();
        const char* pViewType = pAppInfo->getViewType();

        if (pViewType != NULL && stricmp("hidden", pViewType) == 0)
        {
            return; // HIDDEN
        }
        else
        {
            pToolTip = pAppInfo->getToolTip();
            /*if (pAppInfo->getTitle() != NULL && (pAppInfo->getTitle())[0] != 0)
            {
                pName = pAppInfo->getTitle();  // if we have a title, then we use it instead of the attribute name
            }*/
        }
    }

    strDoc.appendf("<%s>\n", DM_TABLE_ROW);
    strDoc.appendf("<%s>%s</%s>\n", DM_TABLE_ENTRY, pName, DM_TABLE_ENTRY);
    strDoc.appendf("<%s>%s</%s>\n", DM_TABLE_ENTRY, pToolTip, DM_TABLE_ENTRY);

    if (m_pSimpleTypeArray == NULL)
    {
        strDoc.appendf("<%s>%s</%s>\n", DM_TABLE_ENTRY, pDefaultValue, DM_TABLE_ENTRY);
    }
    else
    {
        StringBuffer strDocTemp(pDefaultValue);

        m_pSimpleTypeArray->getDocumentation(strDocTemp);

        strDoc.appendf("<%s>%s</%s>\n", DM_TABLE_ENTRY, strDocTemp.str(), DM_TABLE_ENTRY);
    }

    strDoc.appendf("<%s>%s</%s>\n", DM_TABLE_ENTRY, pRequired, DM_TABLE_ENTRY);
    strDoc.appendf("</%s>\n", DM_TABLE_ROW);
}

void CAttribute::getDojoJS(StringBuffer &strJS) const
{
    assert(this->getConstParentNode() != NULL);

    const char* pViewType = NULL;
    const char* pColumnIndex = NULL;
    const char* pXPath = NULL;
    const CXSDNodeBase *pGrandParentNode =this->getConstAncestorNode(2);
    const CElement *pNextHighestElement = dynamic_cast<const CElement*>(this->getParentNodeByType(XSD_ELEMENT));

    if (m_pAnnotation != NULL && m_pAnnotation->getAppInfo() != NULL)
    {
        const CAppInfo *pAppInfo = NULL;
        pAppInfo = m_pAnnotation->getAppInfo();
        pViewType = pAppInfo->getViewType();
        pColumnIndex = (pAppInfo->getColIndex() != NULL && pAppInfo->getColIndex()[0] != 0) ? pAppInfo->getColIndex() : NULL;
        pXPath = (pAppInfo->getXPath() != NULL && pAppInfo->getXPath()[0] != 0) ? pAppInfo->getXPath() : NULL;
    }

    if (pViewType != NULL && stricmp("hidden", pViewType) == 0)
    {
        return; // HIDDEN
    }

    if ((pColumnIndex != NULL && pColumnIndex[0] != 0) || (pXPath != NULL && pXPath[0] != 0) || (pGrandParentNode != NULL && pGrandParentNode->getNodeType() != XSD_ATTRIBUTE_GROUP && pGrandParentNode->getNodeType() != XSD_COMPLEX_TYPE && pGrandParentNode->getNodeType() != XSD_ELEMENT) || (pGrandParentNode->getNodeType() == XSD_ELEMENT && stricmp( (dynamic_cast<const CElement*>(pGrandParentNode))->getMaxOccurs(), "unbounded") == 0) || (pNextHighestElement != NULL && pNextHighestElement->getMaxOccurs() != NULL && pNextHighestElement->getMaxOccurs()[0] != 0))
    {
        strJS.append(DJ_LAYOUT_CONCAT_BEGIN);
        strJS.append(createDojoColumnLayout(this->getTitle(), getRandomID()));
        strJS.append(DJ_LAYOUT_CONCAT_END);
    }
    else //if (this->getDefault() != NULL && this->getDefault()[0] != 0)
    {
        StringBuffer id("ID_");
        id.append(getRandomID());

        StringBuffer strToolTip(DJ_TOOL_TIP_BEGIN);

        strToolTip.append(DJ_TOOL_TIP_CONNECT_ID_BEGIN);
        strToolTip.append(id.str());
        strToolTip.append(DJ_TOOL_TIP_CONNECT_ID_END);
        DEBUG_MARK_STRJS;

        if (this->getAnnotation()->getAppInfo() != NULL) // check for tooltip
        {
            StringBuffer strTT(this->getAnnotation()->getAppInfo()->getToolTip());
            strTT.replaceString("\"","\\\"");

            strToolTip.append(DJ_TOOL_TIP_LABEL_BEGIN).append(strTT.str()).append(DJ_TOOL_TIP_LABEL_END);
            strToolTip.append(DJ_TOOL_TIP_END);
            DEBUG_MARK_STRJS;
        }

        if (this->m_pSimpleTypeArray->length() == 0)
        {
            strJS.append(DJ_TABLE_ROW_PART_1).append(this->getTitle()).append(DJ_TABLE_ROW_PART_PLACE_HOLDER).append(this->getDefault()).append(DJ_TABLE_ROW_PART_ID_BEGIN).append(id.str()).append(DJ_TABLE_ROW_PART_ID_END);
        }
        else
        {
            m_pSimpleTypeArray->getDojoJS(strJS);

            if (this->getAnnotation()->getAppInfo() != NULL) // check for tooltip
            {
                CConfigSchemaHelper::getInstance()->addToolTip(strToolTip.str());
            }
        }

        if (this->getAnnotation() != NULL && this->getAnnotation()->getAppInfo() != NULL && this->getAnnotation() != NULL && this->getAnnotation()->getAppInfo()->getToolTip() != NULL && this->getAnnotation() != NULL && this->getAnnotation()->getAppInfo()->getToolTip()[0] != 0)
        {
            strJS.append(DJ_ADD_CHILD);
            DEBUG_MARK_STRJS;

            CConfigSchemaHelper::getInstance()->addToolTip(strToolTip.str());
        }
    }
  /*  else
    {
        strJS.append(DJ_TABLE_ROW_PART_1).append(this->getTitle()).append(DJ_TABLE_ROW_PART_2);
    }*/

/*    if (m_pSimpleTypeArray == NULL)
    {

    }
    else
    {
        m_pSimpleTypeArray->getDojoJS(strJS);
    }*/
}

void CAttribute::traverseAndProcessNodes() const
{
    CXSDNodeBase::processEntryHandlers(this);

    if (m_pAnnotation != NULL)
    {
        m_pAnnotation->traverseAndProcessNodes();
    }

    if (m_pSimpleTypeArray != NULL)
    {
        m_pSimpleTypeArray->traverseAndProcessNodes();
    }

    CXSDNodeBase::processExitHandlers(this);
}

CAttribute* CAttribute::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pSchemaRoot != NULL);

    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    CAttribute *pAttribute = new CAttribute(pParentNode);

    Owned<IAttributeIterator> iterAttrib = pSchemaRoot->queryPropTree(xpath)->getAttributes(true);

    ForEach(*iterAttrib)
    {
        if (strcmp(iterAttrib->queryName(), XML_ATTR_NAME) == 0)
        {
            pAttribute->setName(iterAttrib->queryValue());
        }
        else if (strcmp(iterAttrib->queryName(), XML_ATTR_TYPE) == 0)
        {
            pAttribute->setType(iterAttrib->queryValue());
        }
        else if (strcmp(iterAttrib->queryName(), XML_ATTR_DEFAULT) == 0)
        {
            pAttribute->setDefault(iterAttrib->queryValue());
        }
        else if (strcmp(iterAttrib->queryName(), XML_ATTR_USE) == 0)
        {
            pAttribute->setUse(iterAttrib->queryValue());
        }
    }

    const char *pType = pSchemaRoot->queryPropTree(xpath)->queryProp(XML_ATTR_TYPE);
/*
    // special case for naming.
    if (pType != NULL && stricmp(pType, TAG_COMPUTERTYPE) == 0)
    {
        pAttribute->setName(TAG_NAME);
    }
*/
    StringBuffer strXPathExt(xpath);

    strXPathExt.append("/").append(XSD_TAG_ANNOTATION);

    CAnnotation *pAnnotation = CAnnotation::load(pAttribute, pSchemaRoot, strXPathExt.str());

    if (pAnnotation != NULL)
    {
        pAttribute->setAnnotation(pAnnotation);
    }

    strXPathExt.clear().append(xpath);

    strXPathExt.append("/").append(XSD_TAG_SIMPLE_TYPE);

    CSimpleTypeArray *pSimpleTypeArray = CSimpleTypeArray::load(pAttribute, pSchemaRoot, strXPathExt.str());

    if (pSimpleTypeArray != NULL)
    {
        pAttribute->setSimpleTypeArray(pSimpleTypeArray);
    }

    return pAttribute;
}

const char* CAttributeArray::getXML(const char* /*pComponent*/)
{
    if (m_strXML.length() == 0)
    {
        int length = this->length();

        for (int idx = 0; idx < length; idx++)
        {
            CAttribute &Attribute = this->item(idx);

            m_strXML.append(" ").append(Attribute.getXML(NULL));

            if (idx+1 < length)
            {
                m_strXML.append("\n");
            }
        }
    }

    return m_strXML.str();
}

CAttributeArray* CAttributeArray::load(const char* pSchemaFile)
{
    assert(pSchemaFile != NULL);

    if (pSchemaFile == NULL)
    {
        return false;
    }

    Linked<IPropertyTree> pSchemaRoot;

    StringBuffer schemaPath;
    schemaPath.appendf("%s%s", DEFAULT_SCHEMA_DIRECTORY, pSchemaFile);
    pSchemaRoot.setown(createPTreeFromXMLFile(schemaPath.str()));

    StringBuffer strXPathExt("./");
    strXPathExt.append(XSD_TAG_ATTRIBUTE);

    return CAttributeArray::load(NULL, pSchemaRoot, strXPathExt.str());
}

CAttributeArray* CAttributeArray::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pSchemaRoot != NULL);

    if (pSchemaRoot == NULL || xpath == NULL)
    {
        return NULL;
    }

    StringBuffer strXPathExt(xpath);

    CAttributeArray *pAttribArray = new CAttributeArray(pParentNode);

    Owned<IPropertyTreeIterator> attributeIter = pSchemaRoot->getElements(xpath, ipt_ordered);

    int count = 1;
    ForEach(*attributeIter)
    {
        strXPathExt.clear().append(xpath).appendf("[%d]",count);

        CAttribute *pAttrib = CAttribute::load(pAttribArray, pSchemaRoot, strXPathExt.str());

        if (pAttrib != NULL)
        {
            pAttribArray->append(*pAttrib);
        }

        count++;
    }

    if (pAttribArray->length() == 0)
    {
        return NULL;
    }

    return pAttribArray;
}

void CAttributeArray::dump(std::ostream &cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_ATTRIBUTE_ARRAY_STR, offset);

    QUICK_OUT_ARRAY(cout, offset);

    QuickOutFooter(cout, XSD_ATTRIBUTE_ARRAY_STR, offset);
}


void CAttributeArray::getDocumentation(StringBuffer &strDoc) const
{
    assert(this->getConstParentNode() != NULL);

    strDoc.append(DM_SECT4_BEGIN);

    if (this->getConstParentNode()->getNodeType() == XSD_COMPLEX_TYPE && this->getConstParentNode()->getConstParentNode()->getNodeType() != XSD_COMPLEX_TYPE)
    {
        strDoc.appendf("%s%s%s", DM_TITLE_BEGIN, "Attributes", DM_TITLE_END);  // Attributes is hard coded default
        DEBUG_MARK_STRDOC;
    }
    else
    {
        strDoc.append(DM_TITLE_BEGIN).append(DM_TITLE_END);
        DEBUG_MARK_STRDOC;
    }

    strDoc.append(DM_TABLE_BEGIN);
    strDoc.append(DM_TGROUP4_BEGIN);
    strDoc.append(DM_COL_SPEC4);
    strDoc.append(DM_TBODY_BEGIN);

    DEBUG_MARK_STRDOC;
    QUICK_DOC_ARRAY(strDoc);
    DEBUG_MARK_STRDOC;

    strDoc.append(DM_TBODY_END);
    strDoc.append(DM_TGROUP4_END);
    strDoc.append(DM_TABLE_END);

    strDoc.append(DM_SECT4_END);
}

void CAttributeArray::getDojoJS(StringBuffer &strJS) const
{
    assert(this->getConstParentNode() != NULL);
    assert(this->getConstAncestorNode(2) != NULL);


    if (this->getConstParentNode()->getNodeType() == XSD_COMPLEX_TYPE && this->getConstAncestorNode(3) != NULL && this->getConstAncestorNode(3)->getNodeType() == XSD_ELEMENT && CDojoHelper::IsElementATab(dynamic_cast<const CElement*>(this->getConstAncestorNode(3))) == true)
    {
        const char *pName = NULL;

        pName = dynamic_cast<const CElement*>(this->getConstAncestorNode(3))->getName();

        assert(pName != NULL);
        assert(pName[0] != 0);


        genTabDojoJS(strJS, pName);
        DEBUG_MARK_STRJS;


        const CComplexType *pComplexType = dynamic_cast<const CComplexType*>(this->getConstParentNode());

        if (pComplexType->getSequence() == NULL)
        {
            strJS.append(DJ_TABLE_PART_1);
            DEBUG_MARK_STRJS;
        }

        QUICK_DOJO_JS_ARRAY(strJS);

        if (pComplexType->getSequence() != NULL)
        {
            strJS.append(DJ_GRID);
            DEBUG_MARK_STRJS;
        }

        const CElementArray *pElemArray = dynamic_cast<const CElementArray*>(this->getParentNodeByType(XSD_ELEMENT_ARRAY));
        if (pElemArray != NULL && pElemArray->getConstParentNode()->getNodeType() == XSD_CHOICE)
        {
            strJS.append(DJ_LAYOUT_END);
            DEBUG_MARK_STRJS;
        }
        //else
        {
            strJS.append(DJ_TABLE_PART_2);
            DEBUG_MARK_STRJS;
        }
    }
    else
    {
        if (this->getConstParentNode()->getNodeType() == XSD_COMPLEX_TYPE && this->getConstAncestorNode(2)->getNodeType() != XSD_COMPLEX_TYPE && CDojoHelper::IsElementATab(dynamic_cast<const CElement*>(this->getConstAncestorNode(2))) == true)
        {
    //        genTabDojoJS(strJS, dynamic_cast<const CElement*>(this->getConstAncestorNode(3))->getName());
            DEBUG_MARK_STRJS;
        }
        else if (this->getConstParentNode()->getNodeType() == XSD_ATTRIBUTE_GROUP)
        {
            const CAttributeGroup *pAttributeGroup = dynamic_cast<const CAttributeGroup*>(this->getConstParentNode());

            assert(pAttributeGroup != NULL);

            if (pAttributeGroup != NULL)
            {
                genTabDojoJS(strJS, pAttributeGroup->getName());
                DEBUG_MARK_STRJS;
            }
        }
        else if (CDojoHelper::IsElementATab(dynamic_cast<const CElement*>(this->getConstAncestorNode(2))) == false && CElement::isAncestorTopElement(this) == true)
        {
            genTabDojoJS(strJS, "Attributes");
            DEBUG_MARK_STRJS;
        }

        strJS.append(DJ_TABLE_PART_1);
        DEBUG_MARK_STRJS;

        QUICK_DOJO_JS_ARRAY(strJS);

        strJS.append(DJ_TABLE_PART_2);
        strJS.append(DJ_TABLE_END);
    }
}

void CAttributeArray::traverseAndProcessNodes() const
{
    QUICK_TRAVERSE_AND_PROCESS;
}

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

void CAttributeGroupArray::traverseAndProcessNodes() const
{
    QUICK_TRAVERSE_AND_PROCESS;
}
