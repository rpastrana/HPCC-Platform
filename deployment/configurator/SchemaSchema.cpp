#include "jptree.hpp"
#include "XMLTags.h"
#include "SchemaSchema.hpp"
#include "ConfigSchemaHelper.hpp"
#include "DocumentationMarkup.hpp"

CSchema* CSchema::load(const char* pSchemaLocation, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pSchemaRoot != NULL);

    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    CConfigSchemaHelper *pSchemaHelper = (CConfigSchemaHelper::getInstance());

    if (pSchemaHelper->getSchemaForXSD(pSchemaLocation) != NULL)  // check to see if the this schema has already been processed
    {
        return pSchemaHelper->getSchemaForXSD(pSchemaLocation);
    }


    CSchema* pSchema = new CSchema(pSchemaLocation);

    pSchema->setXSDXPath(xpath);

    IPropertyTree *pTree = pSchemaRoot->queryPropTree(xpath);

    if (pTree == NULL)
    {
        return NULL;
    }

    //pSchema->setParentNode(NULL);  // this is the root node in this schema
    pSchema->setXMLNS_XS(pTree->queryProp(XML_ATTR_XMLNS_XS));
    pSchema->setElementFormDefault(pTree->queryProp(XML_ATTR_ELEMENTFORMDEFAULT));
    pSchema->setAttributeFormDefault(pTree->queryProp(XML_ATTR_ATTRIBUTEFORMDEFAULT));

    StringBuffer strXPathExt(xpath);


    CIncludeArray* pIncludeArray = NULL;
    CSimpleTypeArray* pSimpleTypeArray = NULL;
    CComplexTypeArray* pComplexTypeArray = NULL;
    CElementArray* pElemArray = NULL;
    CAttributeGroupArray* pAttributeGroupArray  = NULL;

    StringBuffer strXPathExt2(strXPathExt);
    strXPathExt2.append("*");

    Owned<IPropertyTreeIterator> iter = pSchemaRoot->getElements(strXPathExt2.str());

    ForEach(*iter)
    {
        if (strcmp(XSD_TAG_INCLUDE, iter->get().queryName()) == 0)
        {

            strXPathExt.clear().append(xpath).append(XSD_TAG_INCLUDE);
            pIncludeArray = CIncludeArray::load(pSchema, pSchemaRoot, strXPathExt);
        }
        else if (strcmp(XSD_TAG_SIMPLE_TYPE, iter->get().queryName()) == 0)
        {
            strXPathExt.clear().append(xpath).append(XSD_TAG_SIMPLE_TYPE);
            pSimpleTypeArray = CSimpleTypeArray::load(pSchema, pSchemaRoot, strXPathExt);
        }
        else if (strcmp(XSD_TAG_COMPLEX_TYPE, iter->get().queryName()) == 0)
        {
            strXPathExt.clear().append(xpath).append(XSD_TAG_COMPLEX_TYPE);
            pComplexTypeArray = CComplexTypeArray::load(pSchema, pSchemaRoot, strXPathExt);

        }
        else if (strcmp(XSD_TAG_ELEMENT, iter->get().queryName()) == 0)
        {
            strXPathExt.clear().append(xpath).append(XSD_TAG_ELEMENT);
            pElemArray = CElementArray::load(pSchema, pSchemaRoot, strXPathExt.str());
        }
        else if (strcmp(XSD_TAG_ATTRIBUTE_GROUP, iter->get().queryName()) == 0)
        {
            strXPathExt.clear().append(xpath).append(XSD_TAG_ATTRIBUTE_GROUP);
            pAttributeGroupArray = CAttributeGroupArray::load(pSchema, pSchemaRoot, strXPathExt);
        }
    }

    /*strXPathExt.clear().append(xpath).append(XSD_TAG_INCLUDE);
    CIncludeArray* pIncludeArray = CIncludeArray::load(pSchema, pSchemaRoot, strXPathExt);

    strXPathExt.clear().append(xpath).append(XSD_TAG_SIMPLE_TYPE);
    CSimpleTypeArray* pSimpleTypeArray = CSimpleTypeArray::load(pSchema, pSchemaRoot, strXPathExt);

    strXPathExt.clear().append(xpath).append(XSD_TAG_COMPLEX_TYPE);
    CComplexTypeArray* pComplexTypeArray = CComplexTypeArray::load(pSchema, pSchemaRoot, strXPathExt);

    strXPathExt.clear().append(xpath).append(XSD_TAG_ELEMENT);
    CElementArray* pElemArray = CElementArray::load(pSchema, pSchemaRoot, strXPathExt.str());

    strXPathExt.clear().append(xpath).append(XSD_TAG_ATTRIBUTE_GROUP);
    CAttributeGroupArray* pAttributeGroupArray = CAttributeGroupArray::load(pSchema, pSchemaRoot, strXPathExt);
*/

    pSchema->m_pElementArray = pElemArray;
    pSchema->m_pComplexTypeArray = pComplexTypeArray;

    if (pSchema->m_pAttributeGroupArray == NULL)
    {
        pSchema->m_pAttributeGroupArray = pAttributeGroupArray;
    }
    else
    {
        // copy contents from from pAttributeGroupArray to pSchema->m_pAttributeGroupArray
    }
    pSchema->m_pSimpleTypeArray = pSimpleTypeArray;
    pSchema->m_pIncludeArray = pIncludeArray;

    pSchemaHelper->setSchemaForXSD(pSchemaLocation, pSchema);

    return pSchema;
}
CSchema* CSchema::load(const char* pSchemaLocation, CXSDNodeBase* pParentNode, const char *pDefaultDirOverride)
{
    if (pSchemaLocation == NULL)
    {
        return NULL;
    }

    Linked<IPropertyTree> pSchemaRoot;

    StringBuffer schemaPath;

    if (pDefaultDirOverride == NULL)
    {
        schemaPath.appendf("%s%s", DEFAULT_SCHEMA_DIRECTORY, pSchemaLocation);
    }
    else
    {
        schemaPath.appendf("%s%s", pDefaultDirOverride, pSchemaLocation);
    }

    pSchemaRoot.setown(createPTreeFromXMLFile(schemaPath.str()));

    CSchema *pSchema = CSchema::load(pSchemaLocation, pSchemaRoot, XSD_TAG_SCHEMA);

    SETPARENTNODE(pSchema, pParentNode)

    return pSchema;
}

void CSchema::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_SCHEMA_STR, offset);

    QUICK_OUT_2(XMLNS_XS);
    QUICK_OUT_2(ElementFormDefault);
    QUICK_OUT_2(AttributeFormDefault);

    if (m_pElementArray != NULL)
    {
        m_pElementArray->dump(cout, offset);
    }
    if (m_pComplexTypeArray != NULL)
    {
        m_pComplexTypeArray->dump(cout, offset);
    }
    if (m_pAttributeGroupArray != NULL)
    {
        m_pAttributeGroupArray->dump(cout, offset);
    }
    if (m_pSimpleTypeArray != NULL)
    {
        m_pSimpleTypeArray->dump(cout, offset);
    }
    if (m_pIncludeArray != NULL)
    {
        m_pIncludeArray->dump(cout, offset);
    }

    QuickOutFooter(cout, XSD_SCHEMA_STR, offset);
}

void CSchema::getDocumentation(StringBuffer &strDoc) const
{
    if (m_pElementArray != NULL)
    {
        m_pElementArray->getDocumentation(strDoc);
    }
    if (m_pComplexTypeArray != NULL)
    {
        m_pComplexTypeArray->getDocumentation(strDoc);
    }
    if (m_pAttributeGroupArray != NULL)
    {
        m_pAttributeGroupArray->getDocumentation(strDoc);
    }
    if (m_pSimpleTypeArray != NULL)
    {
        m_pSimpleTypeArray->getDocumentation(strDoc);
    }
    if (m_pIncludeArray != NULL)
    {
        m_pIncludeArray->getDocumentation(strDoc);
    }

    strDoc.append(DM_SECT2_END);
}

void CSchema::getDojoJS(StringBuffer &strJS) const
{
    if (m_pElementArray != NULL)
    {
        m_pElementArray->getDojoJS(strJS);
    }
    if (m_pComplexTypeArray != NULL)
    {
        m_pComplexTypeArray->getDojoJS(strJS);
    }
    if (m_pAttributeGroupArray != NULL)
    {
        m_pAttributeGroupArray->getDojoJS(strJS);
    }
    if (m_pSimpleTypeArray != NULL)
    {
        m_pSimpleTypeArray->getDojoJS(strJS);
    }
    if (m_pIncludeArray != NULL)
    {
        m_pIncludeArray->getDojoJS(strJS);
    }
}

void CSchema::getQML(StringBuffer &strQML) const
{
    if (m_pElementArray != NULL)
    {
        m_pElementArray->getQML(strQML);
    }
    if (m_pComplexTypeArray != NULL)
    {
        m_pComplexTypeArray->getQML(strQML);
    }
    if (m_pAttributeGroupArray != NULL)
    {
        m_pAttributeGroupArray->getQML(strQML);
    }
    if (m_pSimpleTypeArray != NULL)
    {
        m_pSimpleTypeArray->getQML(strQML);
    }
    if (m_pIncludeArray != NULL)
    {
        m_pIncludeArray->getQML(strQML);
    }
}

void CSchema::traverseAndProcessNodes() const
{
    CSchema::processEntryHandlers(this);

    if (m_pElementArray != NULL)
    {
        m_pElementArray->traverseAndProcessNodes();
    }
    if (m_pComplexTypeArray != NULL)
    {
        m_pComplexTypeArray->traverseAndProcessNodes();
    }
    if (m_pAttributeGroupArray != NULL)
    {
        m_pAttributeGroupArray->traverseAndProcessNodes();
    }
    if (m_pSimpleTypeArray != NULL)
    {
        m_pSimpleTypeArray->traverseAndProcessNodes();
    }
    if (m_pIncludeArray != NULL)
    {
        m_pIncludeArray->traverseAndProcessNodes();
    }

    CSchema::processExitHandlers(this);
}

const char* CSchema::getXML(const char* /*pComponent*/)
{
    if (m_strXML.length() == 0)
    {
        int length =  m_pElementArray->length();

        //m_strXML.append("<Environment>\n\t<Software>");

        if (m_pElementArray != NULL)
        {
            for (int idx = 0; idx < length; idx++)
            {
                CElement &Element = m_pElementArray->item(idx);

                m_strXML.append(Element.getXML(NULL));

                if (idx+1 < length)
                {
                    m_strXML.append("\n");
                }
            }
        }

        if (m_pAttributeGroupArray != NULL)
        {
            length = m_pAttributeGroupArray->length();
            for (int idx = 0; idx < length; idx++)
            {
                CAttributeGroup &AttributeGroup  = m_pAttributeGroupArray->item(idx);

                m_strXML.append(AttributeGroup.getXML(NULL));

                if (idx+1 < length)
                {
                    m_strXML.append("\n");
                }
            }
        }

         m_strXML.append("/>\n");

        if (m_pComplexTypeArray != NULL)
        {
            length = m_pComplexTypeArray->length();
            for (int idx = 0; idx < length; idx++)
            {
                CComplexType &ComplexType = m_pComplexTypeArray->item(idx);

                m_strXML.append(ComplexType.getXML(NULL));

                if (idx+1 < length)
                {
                    m_strXML.append("\n");
                }
            }
        }

        if (m_pSimpleTypeArray != NULL)
        {
            length = m_pSimpleTypeArray->length();
            for (int idx = 0; idx < length; idx++)
            {
                CSimpleType &SimpleType = m_pSimpleTypeArray->item(idx);

                m_strXML.append(SimpleType.getXML(NULL));

                if (idx+1 < length)
                {
                    m_strXML.append("\n");
                }
            }
        }

        if (m_pIncludeArray != NULL)
        {
            length = m_pIncludeArray->length();
            for (int idx = 0; idx < length; idx++)
            {
                CInclude &Include = m_pIncludeArray->item(idx);

                m_strXML.append(Include.getXML(NULL));

                if (idx+1 < length)
                {
                    m_strXML.append("\n");
                }
            }
        }

     //   m_strXML.append("\t</Software>\n</Environment>\n");
    }

    return m_strXML.str();
}

CXSDNode* CSchema::getExtensionType(const char* pExtensionTypeName) const
{
    if (pExtensionTypeName == NULL)
    {
        return NULL;
    }

    if (m_pSimpleTypeArray != NULL)
    {
        int length = m_pSimpleTypeArray->length();

        for (int idx = 0; idx < length; idx++)
        {
            CSimpleType &SimpleType = m_pSimpleTypeArray->item(idx);

            if (strcmp(SimpleType.getName(), pExtensionTypeName) == 0)
            {
                return &SimpleType;
            }
        }
    }

    if (m_pComplexTypeArray != NULL)
    {
        int length = m_pComplexTypeArray->length();

        for (int idx = 0; idx < length; idx++)
        {
            CComplexType &ComplexType = m_pComplexTypeArray->item(idx);

            if (strcmp(ComplexType.getName(), pExtensionTypeName) == 0)
            {
                return &ComplexType;
            }
        }
    }

    return NULL;
}
