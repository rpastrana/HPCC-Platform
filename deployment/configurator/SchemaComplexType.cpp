#include <cassert>
#include "jstring.hpp"
#include "jptree.hpp"
#include "XMLTags.h"
#include "SchemaComplexType.hpp"
#include "SchemaSequence.hpp"
#include "SchemaComplexContent.hpp"
#include "SchemaAttributes.hpp"
#include "SchemaChoice.hpp"
#include "SchemaComplexType.hpp"
#include "SchemaElement.hpp"
#include "SchemaSchema.hpp"
#include "ConfigSchemaHelper.hpp"
#include "DocumentationMarkup.hpp"

void CComplexType::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_COMPLEX_TYPE_STR, offset);

    QUICK_OUT_2(Name)

    if (m_pSequence != NULL)
    {
        m_pSequence->dump(cout, offset);
    }

    if (m_pComplexContent != NULL)
    {
        m_pComplexContent->dump(cout, offset);
    }

    if (m_pAttributeArray != NULL)
    {
        m_pAttributeArray->dump(cout, offset);
    }

    if (m_pChoice != NULL)
    {
        m_pChoice->dump(cout, offset);
    }

    if (m_pElementArray != NULL)
    {
        m_pElementArray->dump(cout, offset);
    }

    if (m_pAttributeGroupArray != NULL)
    {
        m_pAttributeGroupArray->dump(cout, offset);
    }

    QuickOutFooter(cout, XSD_COMPLEX_TYPE_STR, offset);
}

void CComplexType::getDocumentation(StringBuffer &strDoc) const
{
    if (m_pSequence != NULL)
    {
        m_pSequence->getDocumentation(strDoc);
    }

    if (m_pComplexContent != NULL)
    {
        strDoc.append(DM_SECT3_BEGIN);
        m_pComplexContent->getDocumentation(strDoc);
        strDoc.append(DM_SECT3_END);
    }

    if (m_pAttributeArray != NULL)
    {
        if (this->getConstParentNode()->getConstParentNode()->getNodeType() == XSD_SCHEMA)
            strDoc.appendf("<%s>\n", DM_TABLE_ROW);

        m_pAttributeArray->getDocumentation(strDoc);

        if (this->getConstParentNode()->getConstParentNode()->getNodeType() == XSD_SCHEMA)
            strDoc.appendf("</%s>\n", DM_TABLE_ROW);
    }

    if (m_pChoice != NULL)
    {
        m_pChoice->getDocumentation(strDoc);
    }

    if (m_pElementArray != NULL)
    {
        DEBUG_MARK_STRDOC;
        strDoc.append(DM_SECT3_BEGIN);
        m_pElementArray->getDocumentation(strDoc);
        strDoc.append(DM_SECT3_END);
    }

    if (m_pAttributeGroupArray != NULL)
    {
        //strDoc.append(DM_SECT3_END);
        //strDoc.append(DM_SECT3_BEGIN);
        m_pAttributeGroupArray->getDocumentation(strDoc);
        //strDoc.append(DM_SECT3_END);
        //strDoc.append(DM_SECT3_BEGIN);
    }
}

void CComplexType::traverseAndProcessNodes() const
{
    CComplexType::processEntryHandlers(this);

    if (m_pSequence != NULL)
    {
        m_pSequence->traverseAndProcessNodes();
    }

    if (m_pComplexContent != NULL)
    {
        m_pComplexContent->traverseAndProcessNodes();
    }

    if (m_pAttributeArray != NULL)
    {
        m_pAttributeArray->traverseAndProcessNodes();
    }

    if (m_pChoice != NULL)
    {
        m_pChoice->traverseAndProcessNodes();
    }

    if (m_pElementArray != NULL)
    {
        m_pElementArray->traverseAndProcessNodes();
    }

    if (m_pAttributeGroupArray != NULL)
    {
        m_pAttributeGroupArray->traverseAndProcessNodes();
    }

    CComplexType::processExitHandlers(this);
}

const char* CComplexType::getXML(const char* /*pComponent*/)
{
    if (m_strXML.length() == 0)
    {
        if (m_pComplexContent != NULL)
        {
            m_strXML.append(m_pComplexContent->getXML(NULL));
        }
        if (m_pAttributeArray != NULL)
        {
            m_strXML.append(m_pAttributeArray->getXML(NULL));
        }
        if (m_pChoice != NULL)
        {
            m_strXML.append(m_pChoice->getXML(NULL));
        }
        if (m_pElementArray != NULL)
        {
            m_strXML.append(m_pElementArray->getXML(NULL));
        }
    }

    return m_strXML.str();
}

CComplexType* CComplexType::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pSchemaRoot != NULL);

    CComplexContent *pComplexContent = NULL;
    CAttributeArray *pAttributeArray =  NULL;
    CChoice *pChoice = NULL;
    CElementArray *pElementArray = NULL;
    CSequence *pSequence  = NULL;
    CAttributeGroupArray *pAttributeGroupArray = NULL;

    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    IPropertyTree *pTree = pSchemaRoot->queryPropTree(xpath);

    const char* pName = pTree->queryProp(XML_ATTR_NAME);

    StringBuffer strXPathExt(xpath);

    StringBuffer strXPathExt2(strXPathExt);
    strXPathExt2.append("*");

    Owned<IPropertyTreeIterator> iter = pSchemaRoot->getElements(strXPathExt2.str());

    ForEach(*iter)
    {
        //oTags.appendf("%s,",iter->get().queryName());

        if (strcmp(XSD_TAG_SEQUENCE, iter->get().queryName()) == 0)
        {
            strXPathExt.clear().append(xpath).append("/").append(XSD_TAG_SEQUENCE);
            pSequence = CSequence::load(NULL, pSchemaRoot, strXPathExt.str());
        }
        else if (strcmp(XSD_TAG_COMPLEX_CONTENT, iter->get().queryName()) == 0)
        {
            strXPathExt.clear().append(xpath).append("/").append(XSD_TAG_COMPLEX_CONTENT);
            pComplexContent = CComplexContent::load(NULL, pSchemaRoot, strXPathExt.str());
        }
        else if (strcmp(XSD_TAG_ATTRIBUTE, iter->get().queryName()) == 0)
        {
            strXPathExt.clear().append(xpath).append("/").append(XSD_TAG_ATTRIBUTE);
            pAttributeArray = CAttributeArray::load(NULL, pSchemaRoot, strXPathExt.str());
        }
        else if (strcmp(XSD_TAG_CHOICE, iter->get().queryName()) == 0)
        {
            strXPathExt.clear().append(xpath).append("/").append(XSD_TAG_CHOICE);
            pChoice = CChoice::load(NULL, pSchemaRoot, strXPathExt.str());
        }
        else if (strcmp(XSD_TAG_ELEMENT, iter->get().queryName()) == 0)
        {
            strXPathExt.clear().append(xpath).append("/").append(XSD_TAG_ELEMENT);
            pElementArray = CElementArray::load(NULL, pSchemaRoot, strXPathExt.str());
        }
        else if (strcmp(XSD_TAG_ATTRIBUTE_GROUP, iter->get().queryName()) == 0)
        {
            strXPathExt.clear().append(xpath).append("/").append(XSD_TAG_ATTRIBUTE_GROUP);
            pAttributeGroupArray = CAttributeGroupArray::load(NULL, pSchemaRoot, strXPathExt.str());
        }
    }

    CComplexType *pComplexType = new CComplexType(pParentNode, pName, pSequence, pComplexContent, pAttributeArray, pChoice, pElementArray, pAttributeGroupArray);

    assert(pComplexType != NULL);

    if (pComplexType != NULL)
    {

        SETPARENTNODE(pSequence, pComplexType)
        SETPARENTNODE(pComplexContent, pComplexType)
        SETPARENTNODE(pAttributeArray, pComplexType)
        SETPARENTNODE(pChoice, pComplexType)
        SETPARENTNODE(pElementArray, pComplexType)
        SETPARENTNODE(pAttributeGroupArray, pComplexType)

        if (pName != NULL)
        {
            CConfigSchemaHelper::getInstance()->setComplexTypeWithName(pName, pComplexType);
        }
    }

    return pComplexType;
}

void CComplexTypeArray::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_COMPLEX_TYPE_ARRAY_STR, offset);

    QUICK_OUT_ARRAY(cout, offset);

    QuickOutFooter(cout, XSD_COMPLEX_TYPE_ARRAY_STR, offset);
}

void CComplexTypeArray::getDocumentation(StringBuffer &strDoc) const
{
//    strDoc.append(DM_SECT3_BEGIN);

    QUICK_DOC_ARRAY(strDoc);

//    strDoc.append(DM_SECT3_END);
}

void CComplexTypeArray::traverseAndProcessNodes() const
{
    QUICK_TRAVERSE_AND_PROCESS;
}

const char* CComplexTypeArray::getXML(const char* /*pComponent*/)
{
    if (m_strXML.length() == 0)
    {
        int length = this->length();

        for (int idx = 0; idx < length; idx++)
        {
            CComplexType &ComplexType = this->item(idx);

            m_strXML.append(ComplexType.getXML(NULL));

            if (idx+1 < length)
            {
                m_strXML.append("\n");
            }
        }
    }

    return m_strXML.str();
}

CComplexTypeArray* CComplexTypeArray::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pSchemaRoot != NULL);

    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    CComplexTypeArray *pComplexTypeArray = new CComplexTypeArray(pParentNode);

    Owned<IPropertyTreeIterator> complexTypeIter = pSchemaRoot->getElements(xpath);

    int count = 1;

    ForEach(*complexTypeIter)
    {
        StringBuffer strXPathExt(xpath);
        strXPathExt.appendf("[%d]", count);

        CComplexType *pComplexType = CComplexType::load(pComplexTypeArray, pSchemaRoot, strXPathExt.str());

        assert(pComplexType != NULL);

        if (pComplexType != NULL)
        {
            pComplexTypeArray->append(*pComplexType);
        }

        count++;
    }

    return pComplexTypeArray;
}

CComplexTypeArray* CComplexTypeArray::load(CXSDNodeBase* pParentNode, const char* pSchemaFile)
{
    assert(false);  // why do still need to call this?
    assert(pSchemaFile != NULL);

    if (pSchemaFile == NULL)
    {
        return NULL;
    }

    if (pParentNode == NULL)
    {
        Linked<IPropertyTree> pSchemaRoot;

        StringBuffer schemaPath;

        schemaPath.appendf("%s%s", DEFAULT_SCHEMA_DIRECTORY, pSchemaFile);
        pSchemaRoot.setown(createPTreeFromXMLFile(schemaPath.str()));

        return CComplexTypeArray::load(pParentNode, pSchemaRoot, XSD_TAG_COMPLEX_TYPE);
    }
    else
    {
        CSchema *pSchema = (dynamic_cast<CSchema*>(pParentNode));

        if (pSchema != NULL)
        {
            return pSchema->getComplexTypeArray();
        }
    }

    return NULL;
}
