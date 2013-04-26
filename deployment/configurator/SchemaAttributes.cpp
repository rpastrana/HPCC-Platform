#include <cassert>
#include "jptree.hpp"
#include "SchemaAttributes.hpp"
#include "SchemaCommon.hpp"

/*
 CXSDNodeBase* CAttribute::getNodeByTypeAndNameAscending(NODE_TYPES eNodeType, const char *pName)
 {
    return NULL;

    /*if (eNodeType == this->getNodeType() && (pName != NULL ? !strcmp(pname, this->getNodeTypeStr()) : true))
    {
        assert(pname != NULL); // for now pName should always be populated

        return this;
    }

    return NULL;  // terminate recursion
}

 CXSDNodeBase* CAttribute::getNodeByTypeAndNameDescending(NODE_TYPES eNodeType, const char *pName)
 {
     return NULL;
     //return this->getNodeByTypeAndNameAscending(eNodeType, pName);
}*/
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

    QuickOutFooter(cout,XSD_ATTRIBUTE_STR, offset);
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

    StringBuffer strXPathExt(xpath);

    strXPathExt.append("/").append(XSD_TAG_ANNOTATION);

    CAnnotation *pAnnotation = CAnnotation::load(pAttribute, pSchemaRoot, strXPathExt.str());

    if (pAnnotation != NULL)
    {
        pAttribute->setAnnotation(pAnnotation);
    }

    return pAttribute;
}

/*CXSDNodeBase* CAttributeArray::getNodeByTypeAndNameAscending(NODE_TYPES eNodeType, const char *pName)
{
    CXSDNodeBase* pMatchingNode = NULL;

    len = this->length()-1;

    for (int idx = 0; idx < len; idx++)
    {

        pMatchingNode = this->getNodeByTypeAndNameDescending(eNodeType, pName);

        if (pMatchingNode != NULL)
        {
            return pMatchingNode;
        }

        pMatchingNode = this->item(idx).getNodeByTypeAndNameAscending(eNodeType, pName);

        if (pMatchingNode != NULL)
        {
            return pMatchingNode;
        }

    }

    return NULL;  // nothing found
}

CXSDNodeBase* CAttributeArray::getNodeByTypeAndNameDescending(NODE_TYPES eNodeType, const char *pName)
{
    CXSDNodeBase* pMatchingNode = NULL;

    len = this->length()-1;

    for (int idx = 0; idx < len; idx++)
    {
        pMatchingNode = this->item(idx).getNodeByTypeAndNameDescending(eNodeType, pName);

        if (pMatchingNode != NULL)
        {
            return pMatchingNode;
        }
    }

    return NULL;  // nothing found
}
*/
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

    Owned<IPropertyTreeIterator> attributeIter = pSchemaRoot->getElements(xpath);

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


CXSDNodeBase* CAttributeGroup::getNodeByTypeAndNameAscending(NODE_TYPES eNodeType, const char *pName)
{
    CXSDNodeBase* pMatchingNode = NULL;

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

CXSDNodeBase* CAttributeGroup::getNodeByTypeAndNameDescending(NODE_TYPES eNodeType, const char *pName)
{
    CXSDNodeBase* pMatchingNode = NULL;

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

    if (m_pAttributeArray != NULL)
    {
        m_pAttributeArray->dump(cout, offset);
    }

    QuickOutFooter(cout, XSD_ATTRIBUTE_GROUP_STR, offset);
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

    CAttributeGroup *pAttributeGroup = new CAttributeGroup(pParentNode);

    IPropertyTree *pTree = pSchemaRoot->queryPropTree(xpath);

    if (pTree == NULL)
    {
        return NULL;
    }

    pAttributeGroup->setName(pTree->queryProp(XML_ATTR_NAME));
    pAttributeGroup->setID(pTree->queryProp(XML_ATTR_ID));
    pAttributeGroup->setRef(pTree->queryProp(XML_ATTR_REF));

    StringBuffer strXPath(xpath);

    strXPath.append("/").append(XSD_TAG_ATTRIBUTE);

    CAttributeArray *pAttribArray = CAttributeArray::load(pAttributeGroup, pSchemaRoot, strXPath.str());

    if (pAttribArray != NULL)
    {
        pAttributeGroup->setAttributeArray(pAttribArray);
    }

    return pAttributeGroup;
}


/*virtual CXSDNodeBase* CAttributeGroupArray::getNodeByTypeAndNameAscending(NODE_TYPES eNodeType, const char *pName)
{
    CXSDNodeBase* pMatchingNode = NULL;

    len = this->length()-1;

    for (int idx = 0; idx < len; idx++)
    {

        pMatchingNode = this->getNodeByTypeAndNameDescending(eNodeType, pName);

        if (pMatchingNode != NULL)
        {
            return pMatchingNode;
        }

        pMatchingNode = this->item(idx).getNodeByTypeAndNameAscending(eNodeType, pName);

        if (pMatchingNode != NULL)
        {
            return pMatchingNode;
        }

    }

    return NULL;  // nothing found
}

virtual CXSDNodeBase* CAttributeGroupArray::getNodeByTypeAndNameDescending(NODE_TYPES eNodeType, const char *pName)
{
    CXSDNodeBase* pMatchingNode = NULL;

    len = this->length()-1;

    for (int idx = 0; idx < len; idx++)
    {
        pMatchingNode = this->item(idx).getNodeByTypeAndNameDescending(eNodeType, pName);

        if (pMatchingNode != NULL)
        {
            return pMatchingNode;
        }
    }

    return NULL;  // nothing found
}*/


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
