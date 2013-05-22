#include <cassert>
#include "jptree.hpp"
#include "jstring.hpp"
#include "XMLTags.h"
#include "SchemaAnnotation.hpp"
#include "SchemaCommon.hpp"
#include "SchemaElement.hpp"
#include "SchemaComplexType.hpp"
#include "SchemaElement.hpp"
#include "SchemaAttributes.hpp"
#include "SchemaAppInfo.hpp"
#include "SchemaDocumentation.hpp"
#include "DocumentationMarkup.hpp"


CElement* CElement::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pSchemaRoot != NULL);
    assert(pParentNode != NULL);

    if (pSchemaRoot == NULL || pParentNode == NULL)
    {
        return NULL;
    }

    CElement *pElement = new CElement();

    IPropertyTree *pTree = pSchemaRoot->queryPropTree(xpath);

    if (pElement != NULL && pTree != NULL)
    {
        pElement->setName(pTree->queryProp(XML_ATTR_NAME));
    }

    Owned<IAttributeIterator> iterAttrib = pTree->getAttributes(true);

    ForEach(*iterAttrib)
    {
        if (strcmp(iterAttrib->queryName(), XML_ATTR_NAME) == 0)
        {
            pElement->setName(iterAttrib->queryValue());
        }
        else if (strcmp(iterAttrib->queryName(), XML_ATTR_MAXOCCURS) == 0)
        {
            pElement->setMaxOccurs(iterAttrib->queryValue());
        }
        else if (strcmp(iterAttrib->queryName(), XML_ATTR_MINOCCURS) == 0)
        {
            pElement->setMinOccurs(iterAttrib->queryValue());
        }
        else if (strcmp(iterAttrib->queryName(), XML_ATTR_TYPE) == 0)
        {
            pElement->setType(iterAttrib->queryValue());
        }

        assert(iterAttrib->queryValue() != NULL);
    }

    assert(strlen(pElement->getName()) > 0);

    StringBuffer strXPathExt(xpath);

    strXPathExt.append("/").append(XSD_TAG_ANNOTATION);
    pElement->m_pAnnotation = CAnnotation::load(pElement, pSchemaRoot, strXPathExt.str());

    strXPathExt.clear().append(xpath).append("/").append(XSD_TAG_COMPLEX_TYPE);
    pElement->m_pComplexTypeArray = CComplexTypeArray::load(pElement, pSchemaRoot, strXPathExt.str());

    strXPathExt.clear().append(xpath).append("/").append(XSD_TAG_ATTRIBUTE);
    pElement->m_pAttributeArray = CAttributeArray::load(pElement, pSchemaRoot, strXPathExt.str());

    if (pElement->m_pAnnotation != NULL && pElement->m_pAnnotation->getAppInfo() != NULL && strlen(pElement->m_pAnnotation->getAppInfo()->getTitle()) > 0)
    {
        pElement->setName(pElement->m_pAnnotation->getAppInfo()->getTitle());
    }

    SETPARENTNODE(pElement, pParentNode);

    return pElement;
}


const char* CElement::getXML(const char* /*pComponent*/)
{
    if (m_strXML.length () == 0)
    {
        m_strXML.append("\n<").append(getName()).append(" ");

        if (m_pAnnotation != NULL)
        {
            m_strXML.append(m_pAnnotation->getXML(NULL));
        }
        if (m_pComplexTypeArray != NULL)
        {
            m_strXML.append(m_pComplexTypeArray->getXML(NULL));
        }
        if (m_pAttributeArray != NULL)
        {
            m_strXML.append(m_pAttributeArray->getXML(NULL));
        }

//        m_strXML.append("/>\n");
    }

    return m_strXML.str();
}

void CElement::dump(std::ostream &cout, unsigned int offset) const
{
    offset += STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_ELEMENT_STR, offset);

    QUICK_OUT(cout,Name, offset);
    QUICK_OUT(cout,Type, offset);
    QUICK_OUT(cout,MinOccurs, offset);
    QUICK_OUT(cout,MaxOccurs, offset);

    if (m_pAnnotation != NULL)
    {
        m_pAnnotation->dump(cout, offset);
    }

    if (m_pComplexTypeArray != NULL)
    {
        m_pComplexTypeArray->dump(cout, offset);
    }

    QuickOutFooter(cout, XSD_ELEMENT_STR, offset);
}

void CElement::getDocumentation(StringBuffer &strDoc) const
{
    const CXSDNodeBase *pGrandParentNode = this->getConstParentNode()->getConstParentNode();

    assert(pGrandParentNode != NULL);

    if (pGrandParentNode == NULL)
    {
        return;
    }

    if (this->getName() != NULL && (stricmp(this->getName(), "Instance") == 0 || stricmp(this->getName(), "Note") == 0) || stricmp(this->getName(), "Notes") == 0 )
    {
        return; // don't document instance
    }

    assert(strlen(this->getName()) > 0);

    if (pGrandParentNode->getNodeType() == XSD_SCHEMA)
    {
        StringBuffer strName(this->getName());

        strName.replace(' ', '_');

        // component name would be here
        strDoc.appendf("<%s %s=\"%s%s\">\n", DM_SECT2, DM_ID, strName.str(),"_mod");
        strDoc.appendf("<%s>%s</%s>\n", DM_TITLE_LITERAL, this->getName(), DM_TITLE_LITERAL);

        if (m_pAnnotation!= NULL)
        {
            m_pAnnotation->getDocumentation(strDoc);
            DEBUG_MARK_STRDOC;
        }


        strDoc.append(DM_SECT3_BEGIN);
        DEBUG_MARK_STRDOC;
        strDoc.append(DM_TITLE_BEGIN).append(DM_TITLE_END);

        if (m_pComplexTypeArray != NULL)
        {
            m_pComplexTypeArray->getDocumentation(strDoc);
        }

        strDoc.append(DM_SECT3_END);
        return;
    }
    else if (m_pComplexTypeArray != NULL)
    {
        if (pGrandParentNode->getNodeType() == XSD_CHOICE)
        {
            strDoc.appendf("%s%s%s", DM_PARA_BEGIN, this->getName(), DM_PARA_END);
        }
        else
        {
            strDoc.appendf("%s%s%s", DM_TITLE_BEGIN, this->getName(), DM_TITLE_END);
        }

        DEBUG_MARK_STRDOC;

        m_pComplexTypeArray->getDocumentation(strDoc);
    }
    else if (m_pComplexTypeArray == NULL)
    {
        strDoc.appendf("%s%s%s", DM_PARA_BEGIN, this->getName(), DM_PARA_END);
        DEBUG_MARK_STRDOC;

        if (m_pAnnotation != NULL && m_pAnnotation->getDocumentation() != NULL)
        {
            m_pAnnotation->getDocumentation(strDoc);
            DEBUG_MARK_STRDOC;
        }

        if (m_pAttributeArray != NULL)
        {
            m_pAttributeArray->getDocumentation(strDoc);
        }
    }

    if (m_pAnnotation!= NULL)
    {
        m_pAnnotation->getDocumentation(strDoc);
        DEBUG_MARK_STRDOC;
    }
}

void CElement::traverseAndProcessNodes() const
{
    CXSDNodeBase::processEntryHandlers(this);

    if (m_pAnnotation != NULL)
    {
        m_pAnnotation->traverseAndProcessNodes();
    }

    if (m_pComplexTypeArray != NULL)
    {
        m_pComplexTypeArray->traverseAndProcessNodes();
    }

    CXSDNodeBase::processExitHandlers(this);
}

void CElementArray::dump(std::ostream &cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_ELEMENT_ARRAY_STR, offset);

    QUICK_OUT_ARRAY(cout, offset);

    QuickOutFooter(cout, XSD_ELEMENT_ARRAY_STR, offset);
}

void CElementArray::getDocumentation(StringBuffer &strDoc) const
{
    QUICK_DOC_ARRAY(strDoc);
}

void CElementArray::traverseAndProcessNodes() const
{
    QUICK_TRAVERSE_AND_PROCESS;
}

const char* CElementArray::getXML(const char* /*pComponent*/)
{
    if (m_strXML.length() == 0)
    {
        int length = this->length();

        for (int idx = 0; idx < length; idx++)
        {
            CElement &Element = this->item(idx);

            m_strXML.append(Element.getXML(NULL));

            if (idx+1 < length)
            {
                m_strXML.append("\n");
            }
        }
    }

    return m_strXML.str();
}

CElementArray* CElementArray::load(const char* pSchemaFile)
{
    assert(pSchemaFile != NULL);

    if (pSchemaFile == NULL)
    {
        return NULL;
    }

    Linked<IPropertyTree> pSchemaRoot;

    StringBuffer schemaPath;

    schemaPath.appendf("%s%s", DEFAULT_SCHEMA_DIRECTORY, pSchemaFile);

    pSchemaRoot.setown(createPTreeFromXMLFile(schemaPath.str()));

    return CElementArray::load(NULL, pSchemaRoot, XSD_TAG_ELEMENT);
}

CElementArray* CElementArray::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pSchemaRoot != NULL);

    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    CElementArray *pElemArray = new CElementArray(pParentNode);

    Owned<IPropertyTreeIterator> elemIter = pSchemaRoot->getElements(xpath);

    int count = 1;

    ForEach(*elemIter)
    {
        StringBuffer strXPathExt(xpath);
        strXPathExt.appendf("[%d]", count);

        CElement *pElem = CElement::load(pElemArray, pSchemaRoot, strXPathExt.str());

        assert(pElem);

        pElemArray->append(*pElem);

        count++;
    }

    if (pElemArray->length() == 0)
    {
        return NULL;
    }

    SETPARENTNODE(pElemArray, pParentNode);

    return pElemArray;
}
